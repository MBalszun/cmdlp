# -----------------------------------------------------------------------------
# @brief  : Test directory.
# @author : Enrico Fraccaroli
# @create : 03/01/2022
# @update : 03/01/2022
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
add_executable(test_cmdlp test_cmdlp.cpp)
target_link_libraries(test_cmdlp cmdlp)
add_test(NAME run_test_cmdlp COMMAND test_cmdlp)

add_custom_target(
    check COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS
    test_cmdlp
)