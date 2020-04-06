#ifndef CONSOLE_H
#define CONSOLE_H

// Console ANSI colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum
{
    ERROR   =  -1,
    SUCCESS =   0,
} functionResult_e;

#define NO_TOP_MENU                 NULL
#define NO_FUNCTION_POINTER         NULL
#define MAX_MENU_NAME_LENGTH        (32)
#define MAX_MENU_DESCRIPTION_LENGTH (32)

typedef struct consoleMenuId
{
    char                name[MAX_MENU_NAME_LENGTH];
    char                description[MAX_MENU_DESCRIPTION_LENGTH];
} consoleMenuId_t;

typedef struct consoleMenuItem
{
    consoleMenuId_t     id;
    functionResult_e    (*functionPointer)(unsigned int numArgs, int[]);
} consoleMenuItem_t;

typedef struct consoleMenu
{
    consoleMenuId_t     id;
    consoleMenuItem_t   *menuItems;
    struct              *consoleMenu topMenu;
    unsigned int        menuLength;
} consoleMenu_t;

#define MENU_LENGTH(x) sizeof(x)/sizeof(consoleMenuItem_t)

void Console_PrintMenu(consoleMenu_t *menu);

#endif // CONSOLE_H