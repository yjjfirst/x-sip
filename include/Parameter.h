struct Parameter;
struct Parameters;

struct Parameter *CreateParameter();
struct Parameters *CreateParameters();
void DestoryParameter(struct Parameter *p);
void DestoryParameters(struct Parameters *ps);

int ParseParameters(char *string, void *target);
char *GetParameter(struct Parameters *p, char *name);

char *Parameters2String(char *pos, void *element, struct HeaderPattern *pattern);
