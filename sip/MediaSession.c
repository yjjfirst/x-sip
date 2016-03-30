#include "MediaSession.h"

struct MediaSession {
};

struct MediaSession *CreateMediaSessionImpl()
{
    struct MediaSession *ms = calloc(1, sizeof(struct MediaSession));
    return ms;
}

void DestoryMediaSessionImpl(struct MediaSession **ms)
{
    if (*ms != NULL) {
        free(*ms);
        *ms = NULL;
    }
}

struct MediaSession *(*CreateMediaSession)() = CreateMediaSessionImpl;
void (*DestoryMediaSession)(struct MediaSession **ms) = DestoryMediaSessionImpl;
