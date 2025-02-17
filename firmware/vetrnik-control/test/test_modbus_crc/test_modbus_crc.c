#include <unity.h>
#include <stdio.h>
#include <modbus_crc.h>


void test_modbus_crc_update()
{
    modbus_crc_t crc = MODBUS_CRC_INIT;
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, crc);
    modbus_crc_update(&crc, 0x01);
    TEST_ASSERT_EQUAL_UINT16(0x807E, crc);
    modbus_crc_update(&crc, 0x03);
    modbus_crc_update(&crc, 0x00);
    modbus_crc_update(&crc, 0x00);
    modbus_crc_update(&crc, 0x00);
    modbus_crc_update(&crc, 0x01);
    TEST_ASSERT_EQUAL_UINT16(0x0A84, crc);
    modbus_crc_update(&crc, 0x84);
    modbus_crc_update(&crc, 0x0A);
    TEST_ASSERT_EQUAL_UINT16(0x0000, crc);
}


void test_modbus_crc_check()
{
    // host inquiry frame
    const uint8_t msg1[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A };
    TEST_ASSERT_TRUE(modbus_crc_check(msg1, sizeof msg1));
    // slave response frame
    const uint8_t msg2[] = { 0x01, 0x03, 0x02, 0x00, 0x56, 0x38, 0x7A };
    TEST_ASSERT_TRUE(modbus_crc_check(msg2, sizeof msg2));

    // corrupt host inquiry frame
    const uint8_t msg3[] = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x1A };
    TEST_ASSERT_FALSE(modbus_crc_check(msg3, sizeof msg3));
    // corrupt host inquiry frame
    const uint8_t msg4[] = { 0x01, 0x03, 0x00, 0x00, 0x04, 0x01, 0x84, 0x0A };
    TEST_ASSERT_FALSE(modbus_crc_check(msg4, sizeof msg4));
}


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_modbus_crc_update);
    RUN_TEST(test_modbus_crc_check);
    return UNITY_END();
}
