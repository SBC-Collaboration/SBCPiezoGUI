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
    target_include_directories(CsRmDLL PUBLIC SYSTEM ../Middle/CsRmDLL/Common)

    add_library(CsFs SHARED IMPORTED GLOBAL)
    set_target_properties(CsFs PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsFs.so)
    target_include_directories(CsFs PUBLIC SYSTEM ../Middle/CsFs/Common)

    add_library(CsDisp SHARED IMPORTED GLOBAL)
    set_target_properties(CsDisp PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsDisp.so)
    target_include_directories(CsDisp PUBLIC SYSTEM ../Middle/CsDisp)

    add_library(CsSsm SHARED IMPORTED GLOBAL)
    set_target_properties(CsSsm PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsSsm.so)
    target_include_directories(CsSsm PUBLIC SYSTEM ../Middle/CsSsm/Common ../Middle/CsRm/Common ../Include/Public ../Include/Private)
    target_link_libraries(CsSsm PUBLIC CsRmDLL CsFs CsDisp)

    add_library(CsAppSupport SHARED IMPORTED GLOBAL)
    set_target_properties(CsAppSupport PROPERTIES IMPORTED_LOCATION /usr/local/lib/libCsAppSupport.so)
    target_include_directories(CsAppSupport PUBLIC SYSTEM ../Sdk/CsAppSupport)
    target_link_libraries(CsAppSupport PUBLIC CsSsm)

    set(GAGE_DIR ../Sdk/C_Common)

endif()


include(cmake/Dependencies.cmake)

add_executable(PiezoGUI main.cpp src/file_helpers.cpp include/file_helpers.h include/SBCPiezoStructs.h)
target_include_directories(PiezoGUI PUBLIC ${GAGE_DIR})
target_sources(PiezoGUI PUBLIC src/gage_helpers.cpp src/imgui_helpers.cpp src/file_helpers.cpp ${GAGE_DIR}/CsSdkMisc.c include/Writers.hpp include/Utilities.hpp include/Reader.hpp)
target_link_libraries(PiezoGUI PUBLIC imgui implot ImGuiFileDialog glfw spdlog::spdlog CsSsm CsAppSupport)
