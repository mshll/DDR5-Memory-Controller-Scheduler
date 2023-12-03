## Coding Conventions

variables_use_snake_case
```
int array_index = 0;
```

functions_use_snake_case
```
void foo_bar();
```

StructUsePascalCase
```
typedef struct PersonInfo {
    char *name;

} PersonInfo_t;
```

EnumsUsePascalCase
```
enum PersonInfo {
    NAME = 1,
    AGE = 2
};
```

**don't**:
```
foo_bar(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);
```

**do**:
```
foo_bar(
    param1,
    param2,
    param3,
    param4,
    param5,
    param6,
    param7,
    param8,
    param9,
    param10);
```