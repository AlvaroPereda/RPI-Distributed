cmake_minimum_required(VERSION 3.16)
project(orchestrator VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

set(SOURCES
    orchestrator.cpp
    src/rpc-embedding.cpp
    src/storage.cpp
    src/sqlite-vec.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_options(${PROJECT_NAME} PRIVATE
    -static-libgcc
    -static-libstdc++
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

set_source_files_properties(src/sqlite-vec.c PROPERTIES
    COMPILE_DEFINITIONS "SQLITE_CORE"
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    common 
    llama 
    ${CMAKE_THREAD_LIBS_INIT} 
    sqlite3
    ssh
)

install(TARGETS ${PROJECT_NAME} RUNTIME)