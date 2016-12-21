#include "unity.h"
#include "utils.h"

char buffer[256];

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    //fprintf(stderr, "some stuff to be printed: len: %d\n", (int)len);
    uint16_t num;
    char *num_char = (char *)&num;
    char *buf_char = (char *)buf;

    for(int i = 0; i < len; i++) {
        num_char[i] = buf_char[i];
    }
    //buffer[len] = '\0';
    fprintf(stderr, "sent number: %d\n", ntohs(num));
    return len;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    char *buf_char = (char *)buf;

}

void test_send_id(void) {
    TEST_ASSERT_TRUE(send_id(10, (uint16_t)123));
    TEST_ASSERT_TRUE(send_id(10, (uint16_t)456));
    TEST_ASSERT_TRUE(send_id(10, (uint16_t)0));
    TEST_ASSERT_TRUE(send_id(10, (uint16_t)12345));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_send_id);
    return UNITY_END();
}
