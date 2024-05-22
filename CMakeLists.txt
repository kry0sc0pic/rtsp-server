cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(rtsp-server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(PkgConfig)

pkg_check_modules(GST REQUIRED gstreamer-1.0)
pkg_check_modules(GST_RTSP_SERVER REQUIRED gstreamer-rtsp-server-1.0)

include_directories(${GST_INCLUDE_DIRS} ${GST_RTSP_SERVER_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS} ${GST_RTSP_SERVER_LIBRARY_DIRS})

add_executable(rtsp_server
	src/main.cpp
	src/RtspServer.cpp
)

target_link_libraries(rtsp_server
	${GST_LIBRARIES}
	${GST_RTSP_SERVER_LIBRARIES}
)

install(TARGETS rtsp_server RUNTIME DESTINATION bin)

