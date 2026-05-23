#!/usr/bin/env python3
"""gen_wrappers.py — generate C++ wrapper methods for LVGL C functions.

Parses LVGL C headers, finds functions not yet wrapped in the lv C++ library,
and generates .gen.hpp files that can be #include'd inside existing class bodies.

Usage:
    python3 scripts/gen_wrappers.py                  # generate all .gen.hpp files
    python3 scripts/gen_wrappers.py --dry-run         # show what would be generated
    python3 scripts/gen_wrappers.py --report          # coverage report per module
    python3 scripts/gen_wrappers.py --module label    # generate only for one module
    python3 scripts/gen_wrappers.py --list-skipped    # show skipped functions
"""

from __future__ import annotations

import argparse
import os
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Optional

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------
SCRIPT_DIR = Path(__file__).resolve().parent
ROOT = SCRIPT_DIR.parent                         # lv/
LV_WRAPPER = ROOT / "include" / "lv"
LVGL_SRC = ROOT.parent / "lvgl" / "src"

# ---------------------------------------------------------------------------
# Configuration: prefix → (class_name, hpp_path_relative_to_LV_WRAPPER, is_mixin)
# ---------------------------------------------------------------------------
# Built manually from the exploration of existing wrappers.
# The key is the C function prefix (without trailing underscore).
# Order matters for disambiguation: longer prefixes first.

PREFIX_MAP: dict[str, tuple[str, str, bool]] = {
    # Widgets
    "lv_animimg":       ("AnimImage",     "widgets/animimage.hpp",     False),
    "lv_arc":           ("Arc",           "widgets/arc.hpp",           False),
    "lv_arclabel":      ("ArcLabel",      "widgets/arclabel.hpp",      False),
    "lv_bar":           ("Bar",           "widgets/bar.hpp",           False),
    "lv_button":        ("Button",        "widgets/button.hpp",        False),
    "lv_buttonmatrix":  ("ButtonMatrix",  "widgets/buttonmatrix.hpp",  False),
    "lv_calendar":      ("Calendar",      "widgets/calendar.hpp",      False),
    "lv_canvas":        ("Canvas",        "widgets/canvas.hpp",        False),
    "lv_chart":         ("Chart",         "widgets/chart.hpp",         False),
    "lv_checkbox":      ("Checkbox",      "widgets/checkbox.hpp",      False),
    "lv_dropdown":      ("Dropdown",      "widgets/dropdown.hpp",      False),
    "lv_file_explorer": ("FileExplorer",  "widgets/file_explorer.hpp", False),
    "lv_image":         ("Image",         "widgets/image.hpp",         False),
    "lv_imagebutton":   ("ImageButton",   "widgets/imagebutton.hpp",   False),
    "lv_ime_pinyin":    ("IMEPinyin",     "widgets/ime_pinyin.hpp",    False),
    "lv_keyboard":      ("Keyboard",      "widgets/keyboard.hpp",      False),
    "lv_label":         ("Label",         "widgets/label.hpp",         False),
    "lv_led":           ("Led",           "widgets/led.hpp",           False),
    "lv_line":          ("Line",          "widgets/line.hpp",          False),
    "lv_list":          ("List",          "widgets/list.hpp",          False),
    "lv_lottie":        ("Lottie",        "widgets/lottie.hpp",        False),
    "lv_menu":          ("Menu",          "widgets/menu.hpp",          False),
    "lv_msgbox":        ("Msgbox",        "widgets/msgbox.hpp",        False),
    "lv_roller":        ("Roller",        "widgets/roller.hpp",        False),
    "lv_scale":         ("Scale",         "widgets/scale.hpp",         False),
    "lv_slider":        ("Slider",        "widgets/slider.hpp",        False),
    "lv_spangroup":     ("Spangroup",     "widgets/span.hpp",          False),
    "lv_spinbox":       ("Spinbox",       "widgets/spinbox.hpp",       False),
    "lv_spinner":       ("Spinner",       "widgets/spinner.hpp",       False),
    "lv_switch":        ("Switch",        "widgets/switch.hpp",        False),
    "lv_table":         ("Table",         "widgets/table.hpp",         False),
    "lv_tabview":       ("Tabview",       "widgets/tabview.hpp",       False),
    "lv_textarea":      ("Textarea",      "widgets/textarea.hpp",      False),
    "lv_3dtexture":     ("Texture3D",     "widgets/texture3d.hpp",     False),
    "lv_tileview":      ("Tileview",      "widgets/tileview.hpp",      False),
    "lv_win":           ("Window",        "widgets/win.hpp",           False),
    # Core — order matters: more specific prefixes first
    "lv_obj_set_style": ("StyleMixin",    "core/style.hpp",            True),
    "lv_obj_get_style": ("StyleMixin",    "core/style.hpp",            True),
    "lv_obj":           ("ObjectMixin",   "core/object.hpp",           True),
    "lv_display":       ("Display",       "core/display.hpp",          False),
    "lv_group":         ("Group",         "core/focus.hpp",            False),
    "lv_anim_timeline": ("AnimTimeline",  "core/anim_timeline.hpp",    False),
    "lv_anim":          ("Anim",          "core/anim.hpp",             False),
    "lv_timer":         ("Timer",         "core/timer.hpp",            False),
    "lv_style":         ("Style",         "core/style.hpp",            False),
    "lv_indev":         ("Indev",         "core/indev.hpp",            False),
    "lv_subject":       ("State",         "core/state.hpp",            False),
}

# Sort by prefix length descending so longer prefixes match first
_SORTED_PREFIXES = sorted(PREFIX_MAP.keys(), key=len, reverse=True)

# Self-expression overrides for classes that don't use m_obj.
# Key = class_name, Value = C++ expression for the self pointer/reference.
SELF_EXPR: dict[str, str] = {
    "Style":         "&m_style",
    "Anim":          "&m_anim",
    "Display":       "m_display",
    "AnimTimeline":  "m_timeline",
    "Group":         "m_group",
    "Timer":         "m_timer",
    "Indev":         "m_indev",
    "State":         "&m_subject",
}

# C++ reserved keywords that can't be used as method names
_CPP_KEYWORDS = {
    "delete", "new", "class", "struct", "template", "typename",
    "virtual", "override", "final", "operator", "throw", "try",
    "catch", "namespace", "using", "return", "break", "continue",
    "switch", "case", "default", "goto", "if", "else", "for",
    "while", "do", "sizeof", "alignof", "static_cast", "const_cast",
    "dynamic_cast", "reinterpret_cast", "true", "false", "nullptr",
    "this", "auto", "register", "volatile", "mutable", "explicit",
    "export", "extern", "friend", "inline", "constexpr", "consteval",
    "constinit", "decltype", "noexcept", "static_assert", "typedef",
    "concept", "requires", "co_await", "co_return", "co_yield",
}

# Types that indicate a function should be skipped (conditional/internal features)
_SKIP_TYPES = {
    "lv_property_t",
    "lv_obj_spec_attr_t",
    "lv_obj_id_t",
    "lv_indev_touch_data_t",
    "lv_indev_gesture_recognizer_t",
    "lv_style_value_t",         # internal style value union
    "lv_style_prop_t",          # when used as first param (not self)
}

# Function name patterns to skip (feature-gated or internal)
_SKIP_FUNC_PATTERNS = [
    "lv_indev_gesture_",        # behind LV_USE_GESTURE_RECOGNITION
    "lv_indev_scroll_handler",  # internal scroll processing
    "lv_indev_scroll_throw",    # internal scroll processing
    "lv_indev_find_scroll",     # internal scroll processing
    "lv_indev_recognizer_",     # gesture recognizer internals
    "lv_display_get_screen_by_name",  # requires LV_USE_OBJ_NAME
    "lv_indev_set_pinch_",            # behind LV_USE_GESTURE_RECOGNITION
    "lv_indev_set_rotation_rad",      # behind LV_USE_GESTURE_RECOGNITION
    "lv_indev_set_gesture_data",      # behind LV_USE_GESTURE_RECOGNITION
    "lv_indev_get_gesture_center",    # behind LV_USE_GESTURE_RECOGNITION
    "lv_indev_get_gesture_primary",   # behind LV_USE_GESTURE_RECOGNITION
    "lv_calendar_set_chinese_mode",   # behind LV_USE_CALENDAR_CHINESE
]


# ---------------------------------------------------------------------------
# Data structures
# ---------------------------------------------------------------------------
@dataclass
class Param:
    type: str
    name: str

    def decl(self) -> str:
        """C++ parameter declaration."""
        return f"{self.type} {self.name}"


@dataclass
class CFunction:
    """Parsed C function declaration."""
    return_type: str
    name: str
    params: list[Param]
    is_const: bool = False      # first param is const lv_obj_t*
    source_file: str = ""

    @property
    def extra_params(self) -> list[Param]:
        """Parameters after the first (self) parameter."""
        return self.params[1:] if self.params else []


@dataclass
class GeneratedMethod:
    """A C++ method to be generated."""
    cpp_name: str
    c_func: str
    return_type: str
    params: list[Param]         # without self
    is_getter: bool
    is_const: bool
    class_name: str
    is_mixin: bool


@dataclass
class ModuleInfo:
    """Collection of methods for a single C++ class."""
    class_name: str
    hpp_path: str
    is_mixin: bool
    methods: list[GeneratedMethod] = field(default_factory=list)
    total_c_funcs: int = 0
    already_wrapped: int = 0
    skipped: int = 0


# ---------------------------------------------------------------------------
# Overrides (loaded from gen_overrides.yaml if available, or inline defaults)
# ---------------------------------------------------------------------------
DEFAULT_SKIP = {
    # va_list / variadic — not useful in C++
    "lv_label_set_text_vfmt",
    # Internal / private
    "_lv_",
    # Property system (behind LV_USE_OBJ_PROPERTY, not universally available)
    "lv_obj_get_property",
    "lv_obj_set_property",
    "lv_obj_get_properties",
    # ID system (behind LV_USE_OBJ_ID)
    "lv_obj_assign_id",
    "lv_obj_stringify_id",
    "lv_obj_free_id",
    "lv_obj_id_compare",
    # Functions using callback typedefs that need special wrapping
    "lv_obj_class_create_obj",
}

DEFAULT_RENAME: dict[str, str] = {}


def load_overrides() -> tuple[set[str], dict[str, str]]:
    """Load skip/rename from gen_overrides.yaml if it exists."""
    overrides_path = SCRIPT_DIR / "gen_overrides.yaml"
    skip = set(DEFAULT_SKIP)
    rename = dict(DEFAULT_RENAME)
    if overrides_path.exists():
        try:
            import yaml
            with open(overrides_path) as f:
                data = yaml.safe_load(f) or {}
            skip.update(data.get("skip", []))
            rename.update(data.get("rename", {}))
        except ImportError:
            # Fallback: simple line parser for skip list
            with open(overrides_path) as f:
                content = f.read()
            for m in re.finditer(r"^\s*-\s+(lv_\w+)", content, re.MULTILINE):
                skip.add(m.group(1))
    return skip, rename


# ---------------------------------------------------------------------------
# Step 1: Parse LVGL C headers
# ---------------------------------------------------------------------------
# Regex to match function declarations in LVGL headers.
# Handles multi-line declarations by first joining continuation lines.
FUNC_DECL_RE = re.compile(
    r"^"
    r"(?:(?:static\s+inline|LV_ATTRIBUTE_\w+|extern)\s+)*"  # optional qualifiers
    r"([\w\s\*]+?)"                   # return type (group 1)
    r"\s+(lv_\w+)"                    # function name (group 2)
    r"\s*\(([^)]*)\)"                 # parameters (group 3)
    r"\s*(?:LV_FORMAT_ATTRIBUTE\([^)]*\))?"  # optional format attr
    r"\s*;",
    re.MULTILINE,
)

PARAM_RE = re.compile(
    r"^\s*((?:const\s+)?[\w\s\*]+?)\s+(\w+)\s*$"
)


def parse_params(raw: str) -> list[Param]:
    """Parse a C parameter list string into Param objects."""
    raw = raw.strip()
    if not raw or raw == "void":
        return []
    parts = raw.split(",")
    params = []
    for part in parts:
        part = part.strip()
        if part == "...":
            params.append(Param("...", ""))
            continue
        m = PARAM_RE.match(part)
        if m:
            params.append(Param(m.group(1).strip(), m.group(2).strip()))
        else:
            # Fallback: try to split on last space/star
            # e.g., "lv_obj_t*parent" (no space)
            idx = max(part.rfind(" "), part.rfind("*"))
            if idx > 0:
                ptype = part[: idx + 1].strip()
                pname = part[idx + 1 :].strip()
                if pname:
                    params.append(Param(ptype, pname))
    return params


def normalize_type(t: str) -> str:
    """Normalize whitespace in a C type string."""
    t = re.sub(r"\s+", " ", t.strip())
    # Normalize "type *" → "type*" for pointer types
    t = re.sub(r"\s*\*\s*", "* ", t).strip()
    if t.endswith("* "):
        t = t.rstrip()
    return t


def parse_headers(lvgl_src: Path) -> list[CFunction]:
    """Parse all LVGL headers and extract function declarations."""
    funcs = []
    for hfile in sorted(lvgl_src.rglob("*.h")):
        text = hfile.read_text(errors="replace")
        # Remove block comments
        text = re.sub(r"/\*.*?\*/", "", text, flags=re.DOTALL)
        # Remove line comments
        text = re.sub(r"//[^\n]*", "", text)
        # Join line continuations
        text = re.sub(r"\\\n", " ", text)
        # Remove preprocessor directives
        text = re.sub(r"^\s*#[^\n]*$", "", text, flags=re.MULTILINE)

        for m in FUNC_DECL_RE.finditer(text):
            ret_type = normalize_type(m.group(1))
            fname = m.group(2)
            params_raw = m.group(3)
            params = parse_params(params_raw)

            # Determine constness from first parameter
            is_const = False
            if params and "const" in params[0].type:
                is_const = True

            funcs.append(CFunction(
                return_type=ret_type,
                name=fname,
                params=params,
                is_const=is_const,
                source_file=str(hfile.relative_to(lvgl_src)),
            ))
    return funcs


# ---------------------------------------------------------------------------
# Step 2: Scan existing wrappers for already-referenced C functions
# ---------------------------------------------------------------------------
def scan_wrapped_functions(lv_wrapper: Path) -> set[str]:
    """Find all lv_*( references in existing .hpp files (excluding .gen.hpp)."""
    wrapped = set()
    ref_re = re.compile(r"\blv_[a-z][a-z0-9_]*(?=\s*\()")
    for hpp in lv_wrapper.rglob("*.hpp"):
        if hpp.name.endswith(".gen.hpp"):
            continue
        text = hpp.read_text(errors="replace")
        wrapped.update(ref_re.findall(text))
    return wrapped


def scan_cpp_method_names(lv_wrapper: Path, hpp_path: str) -> set[str]:
    """Scan a specific .hpp file for C++ method names (hand-written).

    Used to detect name conflicts between mixin classes sharing inheritance.
    """
    names = set()
    full_path = lv_wrapper / hpp_path
    if not full_path.exists():
        return names
    text = full_path.read_text(errors="replace")
    # Match method declarations: return_type name(params)
    # Captures the method name from patterns like:
    #   Derived& name(
    #   void name(
    #   [[nodiscard]] int32_t name(
    method_re = re.compile(r"(?:^|\s)(\w+)\s*\([^)]*\)\s*(?:const\s*)?noexcept", re.MULTILINE)
    for m in method_re.finditer(text):
        name = m.group(1)
        # Skip keywords and types
        if name not in ("if", "for", "while", "return", "noexcept", "static",
                        "template", "class", "struct", "void", "bool", "int",
                        "auto", "constexpr", "inline", "explicit", "virtual"):
            names.add(name)
    return names


# ---------------------------------------------------------------------------
# Step 3: Map C functions to C++ classes
# ---------------------------------------------------------------------------
def find_prefix(fname: str) -> Optional[str]:
    """Find the longest matching prefix for a C function name."""
    for prefix in _SORTED_PREFIXES:
        if fname.startswith(prefix + "_") or fname == prefix:
            return prefix
    return None


def cpp_method_name(fname: str, prefix: str, rename_map: dict[str, str]) -> Optional[str]:
    """Derive the C++ method name from a C function name.

    Returns None if the name is a C++ reserved keyword.
    """
    if fname in rename_map:
        return rename_map[fname]
    # Strip prefix + underscore
    suffix = fname[len(prefix) + 1:]
    if suffix in _CPP_KEYWORDS:
        return None
    return suffix


def classify_method(func: CFunction, prefix: str, class_name: str,
                    is_mixin: bool, name: str) -> GeneratedMethod:
    """Classify a C function and create a GeneratedMethod."""
    is_getter = (
        func.is_const
        or name.startswith("get_")
        or name.startswith("is_")
        or name.startswith("has_")
    )
    return GeneratedMethod(
        cpp_name=name,
        c_func=func.name,
        return_type=func.return_type,
        params=func.extra_params,
        is_getter=is_getter,
        is_const=func.is_const or is_getter,
        class_name=class_name,
        is_mixin=is_mixin,
    )


# ---------------------------------------------------------------------------
# Step 4: Generate C++ code
# ---------------------------------------------------------------------------
def gen_method_code(m: GeneratedMethod) -> str:
    """Generate C++ method code for a single method."""
    # Parameter declarations (without self)
    param_decls = ", ".join(p.decl() for p in m.params)
    # Parameter forwards (just names)
    param_names = ", ".join(p.name for p in m.params)

    # Self accessor
    if m.is_mixin:
        self_expr = "obj()"
    elif m.class_name in SELF_EXPR:
        self_expr = SELF_EXPR[m.class_name]
    else:
        self_expr = "m_obj"

    # Call expression
    args = self_expr
    if param_names:
        args += ", " + param_names
    call = f"{m.c_func}({args})"

    const = " const" if m.is_const else ""

    if m.return_type != "void":
        # Non-void return — mark getters as [[nodiscard]]
        nodiscard = "[[nodiscard]] " if m.is_getter else ""
        return (
            f"{nodiscard}{m.return_type} {m.cpp_name}({param_decls}){const} noexcept {{\n"
            f"    return {call};\n"
            f"}}\n"
        )
    elif m.is_const:
        # Void const method (e.g., get_letter_pos — output via pointer param)
        return (
            f"void {m.cpp_name}({param_decls}){const} noexcept {{\n"
            f"    {call};\n"
            f"}}\n"
        )
    else:
        # Fluent setter/action
        if m.is_mixin:
            ret_type = "Derived&"
            ret_expr = "*static_cast<Derived*>(this)"
        else:
            ret_type = f"{m.class_name}&"
            ret_expr = "*this"
        return (
            f"{ret_type} {m.cpp_name}({param_decls}) noexcept {{\n"
            f"    {call};\n"
            f"    return {ret_expr};\n"
            f"}}\n"
        )


def gen_file_content(module: ModuleInfo) -> str:
    """Generate complete .gen.hpp file content."""
    lines = [
        f"// Auto-generated by gen_wrappers.py — do not edit\n",
        f"// Missing {module.class_name} wrappers for LVGL C functions\n",
        f"\n",
    ]
    for method in sorted(module.methods, key=lambda m: m.cpp_name):
        lines.append(gen_method_code(method))
        lines.append("\n")
    return "".join(lines)


# ---------------------------------------------------------------------------
# Step 5: Determine output paths
# ---------------------------------------------------------------------------
def gen_hpp_path(hpp_path: str, class_name: str, is_mixin: bool) -> Path:
    """Compute the .gen.hpp output path from the wrapper .hpp path.

    Uses _mixin suffix to disambiguate when a mixin shares an .hpp with
    a concrete class (e.g., StyleMixin vs Style in core/style.hpp).
    """
    p = Path(hpp_path)
    stem = p.stem  # e.g., "style"
    if is_mixin:
        return p.parent / f"{stem}_mixin.gen.hpp"
    return p.parent / f"{stem}.gen.hpp"


def should_skip_func(func: CFunction, skip_set: set[str],
                     wrapped: set[str]) -> Optional[str]:
    """Return skip reason or None if function should be wrapped."""
    # Already wrapped in hand-written code
    if func.name in wrapped:
        return "already wrapped"

    # Explicit skip list
    if func.name in skip_set:
        return "in skip list"
    for pattern in skip_set:
        if pattern in func.name:
            return f"matches skip pattern '{pattern}'"

    # Internal functions
    if func.name.startswith("_lv_"):
        return "internal (_lv_)"

    # Function name patterns
    for pattern in _SKIP_FUNC_PATTERNS:
        if pattern in func.name:
            return f"matches pattern '{pattern}'"

    # Create functions (factories) — handled separately
    if func.name.endswith("_create"):
        return "factory (_create)"

    # Variadic functions
    if any(p.type == "..." for p in func.params):
        return "variadic"

    # Functions using internal/conditional types
    all_types = [func.return_type] + [p.type for p in func.params]
    for t in all_types:
        for skip_type in _SKIP_TYPES:
            if skip_type in t:
                return f"uses internal type '{skip_type}'"

    # Functions with array parameters (e.g., "const int32_t col_dsc[]")
    for p in func.params:
        if "[" in p.type or "[" in p.name:
            return "array parameter"

    # Functions with no parameters (global utils, not methods)
    if not func.params:
        return "no parameters (not a method)"

    # First param must match the expected self type for this prefix
    prefix = find_prefix(func.name)
    if not prefix:
        return "no matching prefix"

    first = func.params[0].type
    expected_types = _expected_self_types(prefix)
    if not any(et in first for et in expected_types):
        return f"first param '{first}' not expected self type"

    return None


def _expected_self_types(prefix: str) -> list[str]:
    """Return expected self-parameter types for a given prefix."""
    type_map = {
        "lv_obj": ["lv_obj_t"],
        "lv_obj_set_style": ["lv_obj_t"],
        "lv_obj_get_style": ["lv_obj_t"],
        "lv_display": ["lv_display_t"],
        "lv_group": ["lv_group_t"],
        "lv_anim": ["lv_anim_t"],
        "lv_anim_timeline": ["lv_anim_timeline_t"],
        "lv_timer": ["lv_timer_t"],
        "lv_style": ["lv_style_t"],
        "lv_indev": ["lv_indev_t"],
        "lv_subject": ["lv_subject_t"],
    }
    # Widgets all use lv_obj_t
    if prefix not in type_map:
        return ["lv_obj_t"]
    return type_map[prefix]


# ---------------------------------------------------------------------------
# Step 6: Resolve name conflicts between mixin classes
# ---------------------------------------------------------------------------

# Mixin classes that share inheritance in widget classes.
# For each mixin, list the sibling mixin headers whose method names must not collide.
_MIXIN_SIBLINGS: dict[str, list[str]] = {
    "StyleMixin": ["core/object.hpp", "core/event.hpp"],
    "ObjectMixin": ["core/style.hpp", "core/event.hpp"],
}


def _resolve_mixin_conflicts(modules: dict[str, ModuleInfo],
                             lv_wrapper: Path) -> None:
    """Rename generated methods that conflict with sibling mixin methods.

    When StyleMixin generates a method named 'x' but ObjectMixin already has 'x',
    rename the StyleMixin method to 'style_x' to avoid ambiguity in derived classes.
    """
    for key, mod in modules.items():
        if mod.class_name not in _MIXIN_SIBLINGS:
            continue

        # Collect all method names from sibling mixin headers
        sibling_names: set[str] = set()
        for sibling_hpp in _MIXIN_SIBLINGS[mod.class_name]:
            sibling_names.update(scan_cpp_method_names(lv_wrapper, sibling_hpp))

        # Also collect names from sibling .gen.hpp files (already generated)
        for sib_key, sib_mod in modules.items():
            if sib_key == key:
                continue
            if sib_mod.hpp_path in _MIXIN_SIBLINGS.get(mod.class_name, []):
                for m in sib_mod.methods:
                    sibling_names.add(m.cpp_name)

        # Rename conflicting methods
        prefix_map = {
            "StyleMixin": "style_",
            "ObjectMixin": "obj_",
        }
        prefix = prefix_map.get(mod.class_name, "")
        for method in mod.methods:
            if method.cpp_name in sibling_names:
                method.cpp_name = prefix + method.cpp_name


# ---------------------------------------------------------------------------
# Main pipeline
# ---------------------------------------------------------------------------
def run(args: argparse.Namespace) -> int:
    if not LVGL_SRC.is_dir():
        print(f"error: LVGL source not found at {LVGL_SRC}", file=sys.stderr)
        return 2
    if not LV_WRAPPER.is_dir():
        print(f"error: lv wrapper not found at {LV_WRAPPER}", file=sys.stderr)
        return 2

    skip_set, rename_map = load_overrides()

    # Parse all LVGL C headers
    all_funcs = parse_headers(LVGL_SRC)
    print(f"Parsed {len(all_funcs)} function declarations from LVGL headers")

    # Scan existing wrappers
    wrapped = scan_wrapped_functions(LV_WRAPPER)
    print(f"Found {len(wrapped)} C functions already referenced in lv wrappers")

    # Classify functions into modules
    modules: dict[str, ModuleInfo] = {}
    skipped_funcs: list[tuple[str, str]] = []

    for func in all_funcs:
        prefix = find_prefix(func.name)
        if prefix is None:
            skipped_funcs.append((func.name, "no matching prefix"))
            continue

        class_name, hpp_path, is_mixin = PREFIX_MAP[prefix]

        # Filter by --module if specified
        if args.module and class_name.lower() != args.module.lower():
            continue

        # Initialize module
        key = f"{class_name}:{hpp_path}"
        if key not in modules:
            modules[key] = ModuleInfo(
                class_name=class_name,
                hpp_path=hpp_path,
                is_mixin=is_mixin,
            )
        mod = modules[key]
        mod.total_c_funcs += 1

        # Check if should skip
        reason = should_skip_func(func, skip_set, wrapped)
        if reason:
            if reason == "already wrapped":
                mod.already_wrapped += 1
            else:
                mod.skipped += 1
            skipped_funcs.append((func.name, reason))
            continue

        # Derive C++ name — skip if it's a reserved keyword
        name = cpp_method_name(func.name, prefix, rename_map)
        if name is None:
            mod.skipped += 1
            skipped_funcs.append((func.name, "C++ reserved keyword"))
            continue

        # Classify and add
        method = classify_method(func, prefix, class_name, is_mixin, name)
        mod.methods.append(method)

    # Deduplicate methods with same C++ name within a module
    for mod in modules.values():
        seen: dict[str, GeneratedMethod] = {}
        unique = []
        for m in mod.methods:
            if m.cpp_name not in seen:
                seen[m.cpp_name] = m
                unique.append(m)
        mod.methods = unique

    # Resolve name conflicts between mixin classes that share inheritance.
    # Widgets inherit ObjectMixin + StyleMixin + EventMixin, so method names
    # must not collide across these. Scan sibling mixin headers for names.
    _resolve_mixin_conflicts(modules, LV_WRAPPER)

    # --- Report mode ---
    if args.report:
        print(f"\n{'CLASS':<25} {'TOTAL':>6} {'WRAPPED':>8} {'GEN':>5} {'SKIP':>6}")
        print(f"{'-----':<25} {'-----':>6} {'-------':>8} {'---':>5} {'----':>6}")
        for key in sorted(modules):
            mod = modules[key]
            print(f"{mod.class_name:<25} {mod.total_c_funcs:>6} "
                  f"{mod.already_wrapped:>8} {len(mod.methods):>5} {mod.skipped:>6}")
        total_funcs = sum(m.total_c_funcs for m in modules.values())
        total_wrapped = sum(m.already_wrapped for m in modules.values())
        total_gen = sum(len(m.methods) for m in modules.values())
        total_skip = sum(m.skipped for m in modules.values())
        print(f"{'TOTAL':<25} {total_funcs:>6} {total_wrapped:>8} {total_gen:>5} {total_skip:>6}")
        return 0

    # --- List skipped mode ---
    if args.list_skipped:
        print(f"\nSkipped functions ({len(skipped_funcs)}):")
        for fname, reason in sorted(skipped_funcs):
            print(f"  {fname:<50} ({reason})")
        return 0

    # --- Generate mode ---
    generated_count = 0
    for key in sorted(modules):
        mod = modules[key]
        if not mod.methods:
            continue

        out_path = LV_WRAPPER / gen_hpp_path(mod.hpp_path, mod.class_name, mod.is_mixin)
        content = gen_file_content(mod)

        if args.dry_run:
            print(f"\n{'='*60}")
            print(f"Would write: {out_path}")
            print(f"  {len(mod.methods)} methods for {mod.class_name}")
            print(f"{'='*60}")
            print(content)
        else:
            out_path.parent.mkdir(parents=True, exist_ok=True)
            out_path.write_text(content)
            print(f"Generated {out_path.relative_to(LV_WRAPPER)} "
                  f"({len(mod.methods)} methods)")
            generated_count += 1

    if not args.dry_run:
        print(f"\nGenerated {generated_count} .gen.hpp files")
    return 0


def main():
    parser = argparse.ArgumentParser(
        description="Generate C++ wrapper methods for LVGL C functions")
    parser.add_argument("--dry-run", action="store_true",
                        help="Show what would be generated without writing files")
    parser.add_argument("--report", action="store_true",
                        help="Show coverage report per module")
    parser.add_argument("--module", type=str, default="",
                        help="Generate only for a specific module/class")
    parser.add_argument("--list-skipped", action="store_true",
                        help="List all skipped functions with reasons")
    return run(parser.parse_args())


if __name__ == "__main__":
    sys.exit(main())
