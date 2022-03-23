add_library(radix radix.c)

add_executable(radix_test radix_test.c)
target_link_libraries(radix_test radix)
add_test(radix_test radix_test)
