add_library(test test.c)
target_link_libraries(test PUBLIC mbx_opts)

add_executable(cli cli.c)
target_link_libraries(cli PRIVATE test)

add_executable(shorthand shorthand.c)
target_link_libraries(shorthand PRIVATE test)

add_executable(longhand longhand.c)
target_link_libraries(longhand PRIVATE test)

add_executable(benchmark_shorthand benchmark_shorthand.c)
target_link_libraries(benchmark_shorthand PRIVATE test)

add_executable(benchmark_longhand benchmark_longhand.c)
target_link_libraries(benchmark_longhand PRIVATE test)

add_executable(benchmark_longhand_equals benchmark_longhand_equals.c)
target_link_libraries(benchmark_longhand_equals PRIVATE test)

add_executable(benchmark_mixed benchmark_mixed.c)
target_link_libraries(benchmark_mixed PRIVATE test)
