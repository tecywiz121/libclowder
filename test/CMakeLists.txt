add_executable(clowder_test
                main
                address_tests
                bencode_tests
                node_id_tests
                routing_table_tests
                hash_table_tests
                scheduler_tests)

find_package(Botan REQUIRED)

target_link_libraries(clowder_test clowder)
target_include_directories(clowder_test SYSTEM PRIVATE ${BOTAN_INCLUDE_DIR})
add_test(clowder_test clowder_test)
