#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "MediaSession.h"
}

TEST_GROUP(MediaSessionTestGroup)
{
};

TEST(MediaSessionTestGroup, CreateMediaSessionTest)
{
    struct MediaSession *ms = CreateMediaSession();

    CHECK_TRUE(ms != NULL);
    DestoryMediaSession(&ms);
}
