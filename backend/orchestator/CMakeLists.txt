set(TARGET orchestator)

project(${TARGET} CXX)

add_executable(${TARGET} orchestator.cpp)

target_compile_features(${TARGET} PRIVATE cxx_std_17)
target_link_options(${TARGET} PRIVATE -static-libgcc -static-libstdc++)
target_link_libraries(${TARGET} PRIVATE 
    ${CMAKE_THREAD_LIBS_INIT} 
    ssh
)
install(TARGETS ${TARGET} RUNTIME)

