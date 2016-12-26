#include "unity.h"
#include "utils.h"

char buffer_send[256];
char buffer_recv[256];
int p = 0;

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    char *buf_char = (char *)buf;

    for(size_t i = 0; i < len; i++) {
        buffer_send[i] = buf_char[i];
    }
    buffer_send[len] = '\0';

    return len;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    char *buf_char = (char *)buf;

    // for(size_t i = 0; i < len; i++) {
    //     buf_char[i] = buffer_recv[i];
    // }
    buf_char[0] = buffer_recv[p++]; // simulate partial recv

    return 1; //len;
}

uint16_t uint16_from_buf(char *buf) {
    uint16_t n;
    char *p = (char *)&n;
    p[0] = buf[0];
    p[1] = buf[1];
    return ntohs(n);
}

void uint16_to_buf(uint16_t n, char *buf) {
    n = ntohs(n);
    char *p = (char *)&n;
    buf[0] = p[0];
    buf[1] = p[1];
}

uint32_t uint32_from_buf(char *buf) {
    uint32_t n;
    char *p = (char *)&n;
    p[0] = buf[0];
    p[1] = buf[1];
    p[2] = buf[2];
    p[3] = buf[3];
    return ntohl(n);
}

void uint32_to_buf(uint32_t n, char *buf) {
    n = ntohl(n);
    char *p = (char *)&n;
    buf[0] = p[0];
    buf[1] = p[1];
    buf[2] = p[2];
    buf[3] = p[3];
}

void test_send_id(void) {
    for(int i = 0; i < 65536; i++) {
        TEST_ASSERT_TRUE(send_id(10, (uint16_t)i));
        TEST_ASSERT_EQUAL_INT(i, uint16_from_buf(buffer_send));
    }
}

void test_send_uint16() {
    for(int i = 0; i < 65536; i++) {
        TEST_ASSERT_TRUE(send_uint16(10, (uint16_t)i));
        TEST_ASSERT_EQUAL_INT(i, uint16_from_buf(buffer_send));
    }
}

void test_send_uint32() {
    for(int i = 0; i < 65536; i++) { // not gonna loop over 2^32 possiblities
        TEST_ASSERT_TRUE(send_uint32(10, (uint32_t)i));
        TEST_ASSERT_EQUAL_INT(i, uint32_from_buf(buffer_send));
    }
}

void test_read_uint16(void) {
    uint16_t n;

    for(int i = 0; i < 65536; i++) {
        uint16_to_buf((uint16_t)i, buffer_recv);
        TEST_ASSERT_TRUE(read_uint16(10, &n));
        TEST_ASSERT_EQUAL_INT(i, n);
        p = 0;
    }
}

void test_read_uint32(void) {
    uint32_t n;

    for(int i = 0; i < 65535; i++) { // not gonna loop over 2^32 possiblities
        uint32_to_buf((uint32_t)i, buffer_recv);
        TEST_ASSERT_TRUE(read_uint32(10, &n));
        TEST_ASSERT_EQUAL_INT(i, n);
        p = 0;
    }
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
