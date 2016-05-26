struct Session {
};

struct Session *CreateSessionImpl()
{
    return calloc(1, sizeof(struct Session));
}

void DestorySessionImpl(struct Session **s)
{
    if (s == NULL) return;

    free(*s);
    *s = NULL;
}

struct Session *(*CreateSession)() = CreateSessionImpl;
void (*DestorySession)(struct Session **session) = DestorySessionImpl;








