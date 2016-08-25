#include "CppUTest/TestHarness.h"

extern "C" {
#include "Transporter.h"
#include "TransporterUdp.h"
}

TEST_GROUP(TransporterUdpTestGroup)
{
};

TEST(TransporterUdpTestGroup, InitTest)
{
    UT_PTR_SET(Transporter, &UdpTransporter);

    UdpTransporterInit();
}

