add_executable(clowder_test
                main
                address_tests
                bencode_tests
                node_id_tests
                routing_table_tests
                hash_table_tests
                scheduler_tests)

target_link_libraries(clowder_test clowder)
add_test(clowder_test clowder_test)
