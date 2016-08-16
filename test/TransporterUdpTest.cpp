#include "CppUTest/TestHarness.h"

extern "C" {
#include "TransporterUdp.h"
}

TEST_GROUP(TransporterUdpTestGroup)
{
};

TEST(TransporterUdpTestGroup, InitTest)
{
    UdpTransporterInit();
}
