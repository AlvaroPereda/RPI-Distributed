set(TARGET main)
add_executable(${TARGET}
    main.cpp
    src/rpc-embedding.cpp
    src/storage.cpp
)
target_include_directories(${TARGET} PRIVATE include)
target_link_libraries(${TARGET} PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT} sqlite3)
target_compile_features(${TARGET} PRIVATE cxx_std_17)
install(TARGETS ${TARGET} RUNTIME)