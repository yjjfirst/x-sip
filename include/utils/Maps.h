#define MAP_STRING_MAX_LENGTH 32

struct IntStringMap {
    int method;
    char string[MAP_STRING_MAX_LENGTH];
};

char *IntMap2String(int integer, struct IntStringMap *map);
int StringMap2Int(char *string, struct IntStringMap *map);
