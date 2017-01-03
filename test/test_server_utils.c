#include "unity.h"
#include "server_utils.h"

void test_pathJoin(void) {
    log_info("testing path_join()\n");

    char* result;
    TEST_ASSERT_EQUAL_STRING("./data/res1.txt", (result = path_join("./data", 1)));
    free(result);
    TEST_ASSERT_EQUAL_STRING("./data/res1.txt", (result = path_join("./data/", 1)));
    free(result);
    TEST_ASSERT_EQUAL_STRING("../data/res15.txt", (result = path_join("../data/", 15)));
    free(result);
    TEST_ASSERT_EQUAL_STRING("~/data/res32.txt", (result = path_join("~/data", 32)));
    free(result);
    TEST_ASSERT_EQUAL_STRING("/some/random/path/res0.txt", (result = path_join("/some/random/path/", 0)));
    free(result);
}

int main(void) {
    openlog(TESTSUITE_LOGGER, LOG_PID, LOG_USER);
    UNITY_BEGIN();
    RUN_TEST(test_pathJoin);
    closelog();
    return UNITY_END();
}
