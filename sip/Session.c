struct Session {
};

struct Session *CreateSessionImpl()
{
    return NULL;
}

void DestorySessionImpl(struct Session **s)
{
    if (s == NULL) return;

    free(*s);
    *s = NULL;
}

struct Session *(*CreateSession)() = CreateSessionImpl;
void (*DestorySession)(struct Session **session) = DestorySessionImpl;








