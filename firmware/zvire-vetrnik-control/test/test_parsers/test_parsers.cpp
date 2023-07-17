#include <unity.h>
#include <stdio.h>
#include <stdint.h>
#include <parsers.h>

void test_onewire_address_(const char * str, const uint8_t expected[8])
{
    uint8_t address[8] = { 0 };
    TEST_ASSERT_TRUE(parse_onewire_address(address, str));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, address, sizeof address);
}

void test_onewire_address_valid()
{
    const uint8_t expected[8] = {0x28, 0x67, 0xE3, 0x76, 0xE0, 0x01, 0x3C, 0x73};
    test_onewire_address_("2867E376E0013C73", expected);
}

void test_onewire_address_trailing()
{
    const uint8_t expected[8] = {0x28, 0x67, 0xE3, 0x76, 0xE0, 0x01, 0x3C, 0x73};
    test_onewire_address_("2867E376E0013C731234", expected);
}


void test_MAC_lowercase()
{
    uint8_t MAC[6] = { 0 };
    const uint8_t expected[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    TEST_ASSERT_TRUE(parse_MAC(MAC, "de:ad:be:ef:fe:ed"));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, MAC, sizeof MAC);
}

void test_MAC_uppercase()
{
    uint8_t MAC[6] = { 0 };
    const uint8_t expected[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    TEST_ASSERT_TRUE(parse_MAC(MAC, "DE:AD:BE:EF:FE:ED"));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, MAC, sizeof MAC);
}

void test_MAC_short()
{
    uint8_t MAC[6] = { 0 };
    const uint8_t expected[6] = { 0 };
    TEST_ASSERT_FALSE(parse_MAC(MAC, "DE:AD:BE:EF:FE:"));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, MAC, sizeof MAC);
}

void test_MAC_long()
{
    uint8_t MAC[6] = { 0 };
    const uint8_t expected[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    TEST_ASSERT_TRUE(parse_MAC(MAC, "de:ad:be:ef:fe:ed:testing"));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, MAC, sizeof MAC);
}

void test_MAC_nothex()
{
    uint8_t MAC[6] = { 0 };
    const uint8_t expected[6] = { 0 };
    TEST_ASSERT_FALSE(parse_MAC(MAC, "DT:AD:BE:EF:FE:ED"));
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, MAC, sizeof MAC);
}

int runUnityTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_onewire_address_valid);
    RUN_TEST(test_onewire_address_trailing);
    RUN_TEST(test_MAC_lowercase);
    RUN_TEST(test_MAC_uppercase);
    RUN_TEST(test_MAC_short);
    RUN_TEST(test_MAC_long);
    RUN_TEST(test_MAC_nothex);
    return UNITY_END();
}


int main()
{
    return runUnityTests();
}
