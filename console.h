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
#define ERASE_SCREEN       "\x1b[2J"

typedef enum
{
    ERROR   =  -1,
    SUCCESS =   0,
} functionResult_e;

#define NO_TOP_MENU                 (0) // NULL
#define NO_SUB_MENU                 (0) // NULL
#define NO_FUNCTION_POINTER         (0) // NULL
#define NO_ARGS                     (0) // NULL
#define MAX_MENU_NAME_LENGTH        (32)
#define MAX_MENU_DESCRIPTION_LENGTH (32)
#define CONSOLE_WIDTH               (80)
#define HEADER_TITLE_EXTRAS_WIDTH   (6) // "=[  ]=" = 6 characters
#define MAX_HEADER_TITLE_WIDTH      (CONSOLE_WIDTH - HEADER_TITLE_EXTRAS_WIDTH) 

typedef struct consoleMenuId
{
    char                name[MAX_MENU_NAME_LENGTH];
    char                description[MAX_MENU_DESCRIPTION_LENGTH];
} consoleMenuId_t;

typedef struct consoleMenuItem
{
    consoleMenuId_t     id;
    struct consoleMenu  *subMenu;
    functionResult_e    (*functionPointer)(unsigned int numArgs, int[]);
} consoleMenuItem_t;

typedef struct consoleMenu
{
    consoleMenuId_t     id;
    consoleMenuItem_t   *menuItems;
    struct consoleMenu  *topMenu;
    unsigned int        menuLength;
} consoleMenu_t;

typedef struct consoleSelection
{
    char                key;
    const char          *description;
} consoleSelection_t;

#define MENU_SIZE(x)        sizeof(x)/sizeof(consoleMenuItem_t)
#define SELECTION_SIZE(x)   sizeof(x)/sizeof(consoleSelection_t)

// User should define a splash screen as an as array of const pointer to const char.
typedef const char *const splash_t[];

void Console_Init(splash_t *splashScreen, unsigned int splashLines, consoleMenu_t *mainMenu);
void Console_Main(void);


void Console_WaitForKey(void);

void Console_TraverseMenus(consoleMenu_t *menu);
char Console_PrintOptionsAndGetResponse(consoleSelection_t *selections, unsigned int numSelections, unsigned int numMenuSelections);
void Console_Print(const char *format, ...);
void Console_PrintNoEol(const char *format, ...);
void Console_PrintNewLine(void);
void Console_PrintHeader(char *headerString);
void Console_PrintDivider(void);
void Console_PrintMenu(consoleMenu_t *menu);

#endif // CONSOLE_H