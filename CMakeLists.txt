cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(rtsp-server VERSION 0.1 LANGUAGES CXX)

list(APPEND CMAKE_PREFIX_PATH "/usr/local/MAVSDK/install")
find_package(MAVSDK REQUIRED)
find_package(PkgConfig)

pkg_check_modules(GST REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(GST_RTSP_SERVER REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0)

add_executable(rtsp_server
	src/main.cpp
	src/RtspServer.cpp
)

target_link_libraries(rtsp_server
	PkgConfig::GST
	PkgConfig::GST_RTSP_SERVER
    MAVSDK::mavsdk
)

install(TARGETS rtsp_server RUNTIME DESTINATION bin)
