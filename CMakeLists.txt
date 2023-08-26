cmake_minimum_required(VERSION 3.22)
project(hylchan)

set(CMAKE_CXX_STANDARD 20)

add_executable(hylchan main.cpp
        utils/EpollNeko.cpp
        utils/SocketOpt.cpp
        utils/Channel.cpp
        utils/Socket.cpp
        utils/NonCopyable.cpp
        hy0chan/HylChan.cpp
        utils/HttpServer.cpp
        utils/HttpConn.cpp
        utils/Buffer.cpp
        utils/Mmaper.cpp
        utils/ServerParams.cpp
        utils/Formater.cpp
        utils/HttpContext.cpp
        utils/Request.cpp
        utils/Response.cpp
        utils/State.cpp
        hy0chan/Reactor.cpp
        utils/ThreadPoll.cpp
        utils/MmapManager.cpp
        utils/MmapManager.h
        utils/Filer.cpp
        utils/Filer.h
)

target_include_directories(${PROJECT_NAME} PRIVATE hy0chan utils)

target_link_libraries(${PROJECT_NAME} profiler unwind)

if (${CMAKE_BUILD_TYPE} STREQUAL "Release")
    add_definitions("-O2")
endif ()