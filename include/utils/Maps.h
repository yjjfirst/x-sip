#define MAP_STRING_MAX_LENGTH 64

#define INT_STRING_MAP(integer) \
    {integer, #integer}

struct IntStringMap {
    int integer;
    char string[MAP_STRING_MAX_LENGTH];
};

char *IntMap2String(int integer, struct IntStringMap *map);
int StringMap2Int(char *string, struct IntStringMap *map);
