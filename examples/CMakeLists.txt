# Examples - supports native X11 or SDL backends

# List of example executables
set(EXAMPLES hello counter settings chart navigation animation form grid_demo translation_demo lottie_demo canvas_draw)

# Create executables
foreach(example ${EXAMPLES})
    add_executable(${example} ${example}.cpp)
    target_link_libraries(${example} PRIVATE lv::lv lvgl)
endforeach()

# All examples
set(ALL_EXAMPLES ${EXAMPLES})

# Try to find X11 first (native Linux)
find_package(X11 QUIET)
if(X11_FOUND)
    foreach(example ${ALL_EXAMPLES})
        target_link_libraries(${example} PRIVATE ${X11_LIBRARIES})
        target_include_directories(${example} PRIVATE ${X11_INCLUDE_DIR})
    endforeach()
    message(STATUS "X11 found - using native display")
endif()

# Also try SDL2 (cross-platform fallback)
find_package(SDL2 QUIET)
if(SDL2_FOUND OR TARGET SDL2::SDL2)
    foreach(example ${ALL_EXAMPLES})
        if(TARGET SDL2::SDL2)
            target_link_libraries(${example} PRIVATE SDL2::SDL2)
        else()
            target_link_libraries(${example} PRIVATE ${SDL2_LIBRARIES})
            target_include_directories(${example} PRIVATE ${SDL2_INCLUDE_DIRS})
        endif()
    endforeach()
    message(STATUS "SDL2 found - available as fallback")
endif()

if(NOT X11_FOUND AND NOT SDL2_FOUND)
    message(WARNING "Neither X11 nor SDL2 found - examples may not work")
endif()
