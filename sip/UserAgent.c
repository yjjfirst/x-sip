struct UserAgent {
};

struct UserAgent *CreateUserAgent()
{
    struct UserAgent *ua = calloc(1, sizeof(struct UserAgent));
    return ua;
}

void DestoryUserAgent(struct UserAgent **ua)
{
    if (*ua != NULL) {
        free(*ua);
        *ua = NULL;
    }
}
