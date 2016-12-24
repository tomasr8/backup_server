#include <syslog.h>
#include "unity.h"
#include "utils.h"

void test_lastModified(void) {
    uint32_t lm;

    syslog(LOG_INFO, "testing function last_modified()");

    TEST_ASSERT_TRUE(last_modified("./dummy_files/a.txt", &lm));
    TEST_ASSERT_TRUE(lm > 0);

    TEST_ASSERT_TRUE(last_modified("./dummy_files/b.txt", &lm));
    TEST_ASSERT_TRUE(lm > 0);

    TEST_ASSERT_FALSE(last_modified("./non_existent_file.txt", &lm));
}

void test_readFile(void){
    char buffer[MAX_SIZE + 1];

    syslog(LOG_INFO, "testing function read_file()");

    TEST_ASSERT_FALSE(read_file("./non_existent_file.txt", buffer));
    TEST_ASSERT_FALSE(read_file("./dummy_files/large_file.txt", buffer));

    TEST_ASSERT_TRUE(read_file("./dummy_files/b.txt", buffer));
    TEST_ASSERT_TRUE(read_file("./dummy_files/b.txt", buffer));
}

int main(void) {
    openlog("backupServer-testSuite", LOG_PID, LOG_USER);
    UNITY_BEGIN();
    RUN_TEST(test_lastModified);
    RUN_TEST(test_readFile);
    closelog();
    return UNITY_END();
}
