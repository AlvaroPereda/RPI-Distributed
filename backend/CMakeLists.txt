set(TARGET orchestator)

project(${TARGET} CXX)

add_executable(${TARGET} 
    orchestator.cpp
    src/rpc-embedding.cpp
    src/storage.cpp
)

target_compile_features(${TARGET} PRIVATE cxx_std_17)
target_link_options(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
target_include_directories(${TARGET} PRIVATE include)
target_link_libraries(${TARGET} PRIVATE 
    common 
    llama 
    ${CMAKE_THREAD_LIBS_INIT} 
    sqlite3
    ssh
)
install(TARGETS ${TARGET} RUNTIME)

