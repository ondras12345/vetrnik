#include <unity.h>
#include <ArduinoFake.h>
using namespace fakeit;

#include <stdio.h>
#include <stdint.h>
#include <CLIeditor.h>


struct EditorInterface {
    virtual void write_response(char c) = 0;
    virtual void write_file(void * buf, size_t len) = 0;
};
EditorInterface * EditorInterfaceMock = nullptr;

Mock <EditorInterface> EditorMock;

void write_response(char c)
{
    EditorInterfaceMock->write_response(c);
}

void write_file(void * buf, size_t len)
{
    EditorInterfaceMock->write_file(buf, len);
}


void setUp()
{
    ArduinoFakeReset();

    EditorInterface & em = EditorMock.get();
    EditorInterfaceMock = &em;
    When(Method(EditorMock, write_response)).AlwaysReturn();
    When(Method(EditorMock, write_file)).AlwaysReturn();
}


void test_CLIeditor_empty()
{
    CLIeditor ed(write_response, write_file);
    TEST_ASSERT_FALSE(ed.process('\r'));
    TEST_ASSERT_FALSE(ed.process('.'));
    TEST_ASSERT_TRUE(ed.process('\r'));

    Verify(
        Method(EditorMock, write_response).Using('\r'),
        Method(EditorMock, write_response).Using('\n'),
        Method(EditorMock, write_response).Using('.'),
        Method(EditorMock, write_response).Using('\r'),
        Method(EditorMock, write_response).Using('\n')
    ).Once();
    VerifyNoOtherInvocations(EditorMock);
}


void process_all(CLIeditor & ed, const char *input)
{
    if (*input == '\0') return;

    for (; *(input+1) != '\0'; input++)
        TEST_ASSERT_FALSE_MESSAGE(ed.process(*input), "ended when it shouldn't have");

    TEST_ASSERT_TRUE_MESSAGE(ed.process(*input), "end sequence was not detected");
}


void test_CLIeditor_short()
{
    CLIeditor ed(write_response, write_file);
    const char * input = "test\r.\r";
    process_all(ed, input);

    Verify(Method(EditorMock, write_file).Using(_, strlen("test"))).Once();
    VerifyNoOtherInvocations(Method(EditorMock, write_file));
}


void test_CLIeditor_shifted_end_sequence()
{
    // '.' should work after any number of '\r' | '\n'
    CLIeditor ed(write_response, write_file);
    const char * input = "test\r\r.\r";
    process_all(ed, input);

    Verify(Method(EditorMock, write_file).Using(_, strlen("test\n"))).Once();
    VerifyNoOtherInvocations(Method(EditorMock, write_file));
}


int runUnityTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_CLIeditor_empty);
    RUN_TEST(test_CLIeditor_short);
    RUN_TEST(test_CLIeditor_shifted_end_sequence);
    // TODO add more tests
    return UNITY_END();
}


int main()
{
    return runUnityTests();
}
