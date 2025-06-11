cmake_minimum_required(VERSION 3.10)

project(dtp_omega1 C)

set(CMAKE_C_STANDARD 99)

# Platform-specific setup
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Sources
set(SOURCES
    dtp_main.c
    dtp_client.c
    dtp_server.c
    dtp_util.c
    dtp_config.c
)

add_executable(dtp ${SOURCES})

target_include_directories(dtp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
    target_link_libraries(dtp ws2_32)
endif()
