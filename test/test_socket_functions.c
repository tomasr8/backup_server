#include "unity.h"
#include "utils.h"

char buffer[256];
char buffer_out[256];

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    char *buf_char = (char *)buf;

    for(size_t i = 0; i < len; i++) {
        buffer[i] = buf_char[i];
    }
    buffer[len] = '\0';

    return len;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    char *buf_char = (char *)buf;

    for(size_t i = 0; i < len; i++) {
        buf_char[i] = buffer_out[i];
    }

    return len;
}

uint16_t uint16_buf() {
    uint16_t n;
    char *p = (char *)&n;
    p[0] = buffer[0];
    p[1] = buffer[1];
    return ntohs(n);
}

void uint16_buf_out(uint16_t n) {
    n = ntohs(n);
    char *p = (char *)&n;
    buffer_out[0] = p[0];
    buffer_out[1] = p[1];
}

uint32_t uint32_buf() {
    uint32_t n;
    char *p = (char *)&n;
    p[0] = buffer[0];
    p[1] = buffer[1];
    p[2] = buffer[2];
    p[3] = buffer[3];
    return ntohl(n);
}

void uint32_buf_out(uint32_t n) {
    n = ntohl(n);
    char *p = (char *)&n;
    buffer_out[0] = p[0];
    buffer_out[1] = p[1];
    buffer_out[2] = p[2];
    buffer_out[3] = p[3];
}

void test_send_id(void) {
    TEST_ASSERT_TRUE(send_id(10, (uint16_t)123));
    TEST_ASSERT_EQUAL_INT(123, uint16_buf());

    TEST_ASSERT_TRUE(send_id(10, (uint16_t)456));
    TEST_ASSERT_EQUAL_INT(456, uint16_buf());

    TEST_ASSERT_TRUE(send_id(10, (uint16_t)0));
    TEST_ASSERT_EQUAL_INT(0, uint16_buf());

    TEST_ASSERT_TRUE(send_id(10, (uint16_t)12345));
    TEST_ASSERT_EQUAL_INT(12345, uint16_buf());
}

void test_send_uint16() {
    TEST_ASSERT_TRUE(send_uint16(10, (uint16_t)123));
    TEST_ASSERT_EQUAL_INT(123, uint16_buf());

    TEST_ASSERT_TRUE(send_uint16(10, (uint16_t)456));
    TEST_ASSERT_EQUAL_INT(456, uint16_buf());

    TEST_ASSERT_TRUE(send_uint16(10, (uint16_t)0));
    TEST_ASSERT_EQUAL_INT(0, uint16_buf());

    TEST_ASSERT_TRUE(send_uint16(10, (uint16_t)12345));
    TEST_ASSERT_EQUAL_INT(12345, uint16_buf());
}

void test_send_uint32() {
    TEST_ASSERT_TRUE(send_uint32(10, (uint32_t)123));
    TEST_ASSERT_EQUAL_INT(123, uint32_buf());

    TEST_ASSERT_TRUE(send_uint32(10, (uint32_t)456));
    TEST_ASSERT_EQUAL_INT(456, uint32_buf());

    TEST_ASSERT_TRUE(send_uint32(10, (uint32_t)0));
    TEST_ASSERT_EQUAL_INT(0, uint32_buf());

    TEST_ASSERT_TRUE(send_uint32(10, (uint32_t)12345));
    TEST_ASSERT_EQUAL_INT(12345, uint32_buf());
}

void test_read_uint16(void) {
    uint16_t n;

    uint16_buf_out(123);
    TEST_ASSERT_TRUE(read_uint16(10, &n));
    TEST_ASSERT_EQUAL_INT(123, n);

    uint16_buf_out(456);
    TEST_ASSERT_TRUE(read_uint16(10, &n));
    TEST_ASSERT_EQUAL_INT(456, n);

    uint16_buf_out(0);
    TEST_ASSERT_TRUE(read_uint16(10, &n));
    TEST_ASSERT_EQUAL_INT(0, n);

    uint16_buf_out(65535);
    TEST_ASSERT_TRUE(read_uint16(10, &n));
    TEST_ASSERT_EQUAL_INT(65535, n);
}

void test_read_uint32(void) {
    uint32_t n;

    uint32_buf_out(123);
    TEST_ASSERT_TRUE(read_uint32(10, &n));
    TEST_ASSERT_EQUAL_INT(123, n);

    uint32_buf_out(456);
    TEST_ASSERT_TRUE(read_uint32(10, &n));
    TEST_ASSERT_EQUAL_INT(456, n);

    uint32_buf_out(0);
    TEST_ASSERT_TRUE(read_uint32(10, &n));
    TEST_ASSERT_EQUAL_INT(0, n);

    uint32_buf_out(65535);
    TEST_ASSERT_TRUE(read_uint32(10, &n));
    TEST_ASSERT_EQUAL_INT(65535, n);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_send_id);
    RUN_TEST(test_send_uint16);
    RUN_TEST(test_send_uint32);
    RUN_TEST(test_read_uint16);
    RUN_TEST(test_read_uint32);
    return UNITY_END();
}
