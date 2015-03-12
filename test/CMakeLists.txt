add_executable(clowder_test
                main
                node_id_tests
                routing_table_tests
                address_tests
                secure_id_tests
                hash_table_tests)

find_package(Botan REQUIRED)

target_link_libraries(clowder_test clowder)
target_include_directories(clowder_test PRIVATE ${BOTAN_INCLUDE_DIR})
add_test(clowder_test clowder_test)
