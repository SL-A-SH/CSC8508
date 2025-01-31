cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)
project(ImguiIntegration LANGUAGES CXX)

# Create an interface target to hold include directories and link properties
add_library(Imgui INTERFACE)
target_include_directories(Imgui INTERFACE 
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/backends
)

# Link OpenGL32 (Windows OpenGL library) so that any target linking to Imgui gets it automatically.
target_link_libraries(Imgui INTERFACE opengl32.lib)

# Expose the list of ImGui source files as a variable
set(IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp"
)

# Expose header files as well
set(IMGUI_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/imgui.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imconfig.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imstb_rectpack.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imstb_textedit.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/imstb_truetype.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_win32.h"
)

# Optionally, you can save these variables in a cache or file to be used by parent projects.
