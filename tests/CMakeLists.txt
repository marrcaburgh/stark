add_library(test test.c)
target_link_libraries(test PUBLIC mbx_opts)

add_executable(errors errors.c)
target_link_libraries(errors PRIVATE test)

add_executable(shorthand shorthand.c)
target_link_libraries(shorthand PRIVATE test)

add_executable(longhand longhand.c)
target_link_libraries(longhand PRIVATE test)

add_executable(cli cli.c)
target_link_libraries(cli PRIVATE test)

add_executable(l1 l1.c)
target_link_libraries(l1 PRIVATE mbx_opts)
