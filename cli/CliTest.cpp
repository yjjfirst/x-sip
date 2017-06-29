#include "CppUTest/TestHarness.h"

extern "C" {
#include "../cli/cli.h"
}
    
TEST_GROUP(CliTestGroup)
{
};

TEST(CliTestGroup, GetCommandTest)
{
    char command[128] = {0};
    get_command(command);
}
