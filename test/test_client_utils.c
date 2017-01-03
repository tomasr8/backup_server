#include "unity.h"
#include "client_utils.h"

void test_parseLine(void) {
    log_info("testing parse_line()\n");

    request req = { 0, 0, 0, {0} };
    TEST_ASSERT_TRUE(parse_line("get 0", &req));
    TEST_ASSERT_TRUE(parse_line("get 1", &req));
    TEST_ASSERT_TRUE(parse_line("get 20", &req));
    TEST_ASSERT_TRUE(parse_line("get 31", &req));
    TEST_ASSERT_TRUE(parse_line("get 32", &req));

    TEST_ASSERT_TRUE(parse_line("set 0 data", &req));
    TEST_ASSERT_TRUE(parse_line("set 1 data", &req));
    TEST_ASSERT_TRUE(parse_line("set 31 data", &req));
    TEST_ASSERT_TRUE(parse_line("set 32 data", &req));

    TEST_ASSERT_TRUE(parse_line("set 12 some data with spaces", &req));
    TEST_ASSERT_EQUAL_STRING("some data with spaces", req.data);

    TEST_ASSERT_TRUE(parse_line("set 0  data with leading space", &req));
    TEST_ASSERT_EQUAL_STRING(" data with leading space", req.data);

    TEST_ASSERT_TRUE(parse_line("set 32 data!$#with&*()special%^&characters", &req));
    TEST_ASSERT_EQUAL_STRING("data!$#with&*()special%^&characters", req.data);

    TEST_ASSERT_FALSE(parse_line("get 0 ", &req));
    TEST_ASSERT_FALSE(parse_line("get 33", &req));
    TEST_ASSERT_FALSE(parse_line("get10", &req));
    //TEST_ASSERT_FALSE(parse_line("get 02", &req));
    TEST_ASSERT_TRUE(parse_line("get 000017", &req));
    TEST_ASSERT_EQUAL_INT(17, req.res);

    TEST_ASSERT_FALSE(parse_line("get -5", &req));
    TEST_ASSERT_FALSE(parse_line("set 0", &req));
    TEST_ASSERT_FALSE(parse_line("set 0no_space", &req));
    TEST_ASSERT_FALSE(parse_line("set 33", &req));
    TEST_ASSERT_FALSE(parse_line("set -12", &req));
    TEST_ASSERT_FALSE(parse_line("set .0", &req));


}

int main(void) {
    openlog(TESTSUITE_LOGGER, LOG_PID, LOG_USER);
    UNITY_BEGIN();
    RUN_TEST(test_parseLine);
    closelog();
    return UNITY_END();
}
