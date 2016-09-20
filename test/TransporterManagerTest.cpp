#include "CppUTest/TestHarness.h"

extern "C" {
#include "TransporterManager.h"
#include "Transporter.h"
}

TEST_GROUP(TransporterManagerTestGroup)
{
};

struct MessageTransporter t1 = {NULL, NULL, NULL, NULL, 103};

TEST(TransporterManagerTestGroup, AddTransporterTest)
{
//    AddTransporter(&t1);
}
