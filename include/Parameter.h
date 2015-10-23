struct Parameter;
struct Parameters;

struct Parameter *CreateParameter();
void DestoryParameter(struct Parameter *p);
void DestoryParameters(struct Parameters *ps);

struct Parameters *ParseParameters(char *string);
char *GetParameter(struct Parameters *p, char *name);

void Parameters2String(struct Parameters *p, char *result);
