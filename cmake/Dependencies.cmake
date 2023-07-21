find_package(OpenGL REQUIRED)
include_directories(SYSTEM ${OPENGL_INCLUDE_DIR})

CPMAddPackage(NAME glfw
        GIT_TAG dd8a678
        GITHUB_REPOSITORY glfw/glfw
        OPTIONS "GLFW_BUILD_EXAMPLES OFF" "GLFW_BUILD_TESTS OFF" "GLFW_BUILD_DOCS OFF"
        "GLFW_INSTALL OFF" "GLFW_DOCUMENT_INTERNALS OFF")

include_directories(${GLFW_DIR}/deps)

CPMAddPackage(NAME spdlog
        GITHUB_REPOSITORY gabime/spdlog
        VERSION 1.8.2)

CPMAddPackage(NAME imgui
        VERSION 1.89.6
        GITHUB_REPOSITORY ocornut/imgui
        DOWNLOAD_ONLY YES)

if(imgui_ADDED)
    list(APPEND ImGUI_BACKENDS ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp)
    list(APPEND ImGUI_BACKENDS ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

    add_library(imgui STATIC
            ${ImGUI_BACKENDS}
            ${imgui_SOURCE_DIR}/imgui.cpp
            ${imgui_SOURCE_DIR}/imgui_draw.cpp
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/imgui_tables.cpp
            ${imgui_SOURCE_DIR}/imgui_widgets.cpp
            ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp)

    target_include_directories(imgui
            SYSTEM PUBLIC
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>/
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>/backends)
    target_link_libraries(imgui PUBLIC glfw ${OPENGL_LIBRARIES} ${IMGUI_LIBRARIES})
else()
    message(FATAL_ERROR "ImGUI not found.")
endif()

CPMAddPackage(NAME implot
        VERSION 0.14
        GITHUB_REPOSITORY epezent/implot
        DOWNLOAD_ONLY YES)

if(implot_ADDED)
    add_library(implot STATIC
            ${implot_SOURCE_DIR}/implot_items.cpp
            ${implot_SOURCE_DIR}/implot.cpp
            ${implot_SOURCE_DIR}/implot_demo.cpp)

    target_include_directories(implot SYSTEM PUBLIC
            imgui $<BUILD_INTERFACE:${implot_SOURCE_DIR}>/)
    target_link_libraries(implot PRIVATE imgui)
else()
    message(FATAL_ERROR "ImPlot not found.
    Make sure to run git submodules init first")
endif()

CPMAddPackage(NAME ImGuiFileDialog
        VERSION 0.6.5
        GITHUB_REPOSITORY aiekick/ImGuiFileDialog
        DOWNLOAD_ONLY YES)

if(ImGuiFileDialog_ADDED)
    add_library(ImGuiFileDialog STATIC
            ${ImGuiFileDialog_SOURCE_DIR}/ImGuiFileDialog.cpp)
    target_include_directories(ImGuiFileDialog SYSTEM PUBLIC
            imgui $<BUILD_INTERFACE:${ImGuiFileDialog_SOURCE_DIR}>/)
    target_link_libraries(ImGuiFileDialog PRIVATE imgui)
else()
    message(FATAL_ERROR "ImGuiFileDialog not found.
    Make sure to run git submodules init first")
endif()
