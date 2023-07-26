cmake_minimum_required(VERSION 3.22)
include(cmake/CPM.cmake)

project(PiezoGUI)

set(CMAKE_CXX_STANDARD 20)

if (WIN32)
    include_directories($ENV{GageDir}/include)
    link_directories($ENV{GageDir}/lib64)
    set(GAGE_DIR $ENV{GageDir}/CompuScope\ C\ SDK/C\ Common)
    set(CsSsm $ENV{GageDir}/lib64/CsSsm.lib)
    set(CsAppSupport $ENV{GageDir}/lib64/CsAppSupport.lib)

elseif (UNIX)
    add_library(CsRmDLL SHARED IMPORTED GLOBAL)
    set_target_properties(CsRmDLL PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsRmDll.so)
    target_include_directories(CsRmDLL INTERFACE ../Middle/CsRmDLL/Common)

    add_library(CsFs SHARED IMPORTED GLOBAL)
    set_target_properties(CsFs PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsFs.so)
    target_include_directories(CsFs INTERFACE ../Middle/CsFs/Common)

    add_library(CsDisp SHARED IMPORTED GLOBAL)
    set_target_properties(CsDisp PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsDisp.so)
    target_include_directories(CsDisp INTERFACE ../Middle/CsDisp)

    add_library(CsSsm SHARED IMPORTED GLOBAL)
    set_target_properties(CsSsm PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsSsm.so)
    target_include_directories(CsSsm INTERFACE ../Middle/CsSsm/Common ../Middle/CsRm/Common ../Include/Public ../Include/Private)
    target_link_libraries(CsSsm INTERFACE CsRmDLL CsFs CsDisp)

    add_library(CsAppSupport SHARED IMPORTED GLOBAL)
    set_target_properties(CsAppSupport PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsAppSupport.so)
    target_include_directories(CsAppSupport INTERFACE ../Sdk/CsAppSupport)
    target_link_libraries(CsAppSupport INTERFACE CsSsm)

    set(GAGE_DIR ../Sdk/C_Common)

endif()

add_library(SBCBinaryFormat STATIC include/SBCBinaryFormat/Utilities.hpp include/SBCBinaryFormat/Writers.hpp include/SBCBinaryFormat/Reader.hpp)
set_target_properties(SBCBinaryFormat PROPERTIES LINKER_LANGUAGE CXX)

include(cmake/Dependencies.cmake)

add_executable(PiezoGUI main.cpp)
target_include_directories(PiezoGUI PUBLIC ${GAGE_DIR} include)
target_sources(PiezoGUI PUBLIC src/gage_helpers.cpp src/imgui_helpers.cpp src/file_helpers.cpp ${GAGE_DIR}/CsSdkMisc.c)
target_link_libraries(PiezoGUI PUBLIC imgui implot ImGuiFileDialog glfw spdlog::spdlog CsSsm CsAppSupport SBCBinaryFormat)
