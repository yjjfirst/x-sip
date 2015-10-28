struct Parameter;
struct Parameters;

struct Parameter *CreateParameter();
struct Parameters *CreateParameters();
void DestoryParameter(struct Parameter *p);
void DestoryParameters(struct Parameters *ps);

int ParseParameters(char *string, void *target);
int ParseParametersExt(char *string, void *target);
char *GetParameter(struct Parameters *p, char *name);
int AddParameter(struct Parameters *ps, char *name, char *value);

char *Parameters2String(char *pos, void *ps, struct HeaderPattern *pattern);
char *Parameters2StringExt(char *pos, void *ps, struct HeaderPattern *pattern);
