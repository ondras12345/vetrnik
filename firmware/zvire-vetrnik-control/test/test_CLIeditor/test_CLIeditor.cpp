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
}


void test_CLIeditor_empty()
{
    Mock <EditorInterface> EditorMock;
    EditorInterface & em = EditorMock.get();
    EditorInterfaceMock = &em;

    When(Method(EditorMock, write_response)).AlwaysReturn();
    When(Method(EditorMock, write_file)).AlwaysReturn();

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


void test_CLIeditor_short()
{
    Mock <EditorInterface> EditorMock;
    EditorInterface & em = EditorMock.get();
    EditorInterfaceMock = &em;

    When(Method(EditorMock, write_response)).AlwaysReturn();
    When(Method(EditorMock, write_file)).AlwaysReturn();

    CLIeditor ed(write_response, write_file);
    const char * input = "test\r.\r";
    const char *c = input;
    for (; *c != '\0' && *(c+1) != '\0' ; c++)
        TEST_ASSERT_FALSE(ed.process(*c));

    TEST_ASSERT_TRUE(ed.process(*c));

    Verify(Method(EditorMock, write_file).Using(_, strlen("test"))).Once();
    VerifyNoOtherInvocations(Method(EditorMock, write_file));
}


int runUnityTests()
{
    UNITY_BEGIN();
    RUN_TEST(test_CLIeditor_empty);
    RUN_TEST(test_CLIeditor_short);
    // TODO add more tests
    return UNITY_END();
}


int main()
{
    return runUnityTests();
}
