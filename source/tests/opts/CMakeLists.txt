add_library(test test.c)
target_include_directories(test PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(test PUBLIC)

add_library(test_env test.c)
target_include_directories(test_env PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_definitions(test_env PUBLIC STARK_OPTS_ENABLE_HEAP STARK_OPTS_ENABLE_ENV STARK_OPTS_ENV_LUT_SIZE=32)

add_executable(test_cli test_cli.c)
target_link_libraries(test_cli PRIVATE test)
add_executable(test_cli_env test_cli.c)
target_link_libraries(test_cli_env PRIVATE test_env)

add_executable(test_shorthand test_shorthand.c)
target_link_libraries(test_shorthand PRIVATE test)
add_executable(test_shorthand_env test_shorthand.c)
target_link_libraries(test_shorthand_env PRIVATE test_env)

add_executable(test_longhand test_longhand.c)
target_link_libraries(test_longhand PRIVATE test)
add_executable(test_longhand_env test_longhand.c)
target_link_libraries(test_longhand_env PRIVATE test_env)

add_executable(test_positional test_positional.c)
target_link_libraries(test_positional PRIVATE test)
add_executable(test_positional_env test_positional.c)
target_link_libraries(test_positional_env PRIVATE test_env)

add_executable(test_array test_array.c)
target_link_libraries(test_array PRIVATE test)
add_executable(test_array_heap test_array.c)
target_link_libraries(test_array_heap PRIVATE test_env)

add_executable(benchmark_shorthand benchmark_shorthand.c)
target_link_libraries(benchmark_shorthand PRIVATE test)

add_executable(benchmark_shorthand_same benchmark_shorthand_same.c)
target_link_libraries(benchmark_shorthand_same PRIVATE test)

add_executable(benchmark_longhand benchmark_longhand.c)
target_link_libraries(benchmark_longhand PRIVATE test)

add_executable(benchmark_longhand_equals benchmark_longhand_equals.c)
target_link_libraries(benchmark_longhand_equals PRIVATE test)

add_executable(benchmark_mixed benchmark_mixed.c)
target_link_libraries(benchmark_mixed PRIVATE test)

add_executable(benchmark_getopt benchmark_getopt.c)
