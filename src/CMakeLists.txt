set(SRCS node_id.cpp address.cpp bencode.cpp channel.cpp peer.cpp routing_table.cpp hash_table.cpp secure_id.cpp state.cpp)
add_library(clowder ${SRCS})

find_package(Botan REQUIRED)

if(COVERAGE OR COVERALLS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
endif()

if(COVERAGE)
    include(CodeCoverage)
    SETUP_TARGET_FOR_COVERAGE(coverage test/clowder_test coverage)
endif()

if(COVERALLS)
    include(Coveralls)

    # Build the list of sources with absolute paths
    foreach(src_file ${SRCS})
        set(COVERALLS_SRCS ${COVERALLS_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/${src_file})
    endforeach()
    coveralls_setup("${COVERALLS_SRCS}" ON "${CMAKE_SOURCE_DIR}/cmake/Modules/")
endif()

target_include_directories(clowder
                            PUBLIC
                                ${CMAKE_SOURCE_DIR}/include
                            PRIVATE
                                ${BOTAN_INCLUDE_DIR})

if(CPPCHECK)
    find_package(cppcheck REQUIRED)
    include(CppcheckTargets)
    set(CPPCHECK_TEMPLATE_ARG ${CPPCHECK_TEMPLATE_ARG} --error-exitcode=2 --suppress=missingIncludeSystem "--suppress=*:${BOTAN_INCLUDE_DIR}/*" "--suppress=unusedFunction")
    add_cppcheck(clowder STYLE POSSIBLE_ERROR FAIL_ON_WARNINGS)
endif()


target_link_libraries(clowder
                        ${BOTAN_LIBRARIES})
