#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "console.h"

static splash_t *consoleSplashScreen;
static unsigned int consoleSplashLines;
static consoleMenu_t *consoleMainMenu;
static consoleSelection_t splashOptions[] = {{'m',"menus"},{'o',"options"}};
static consoleSelection_t menuOptions[] = {{'t',"top"},{'u',"up"},{'q',"quit"}};

void Console_Init(splash_t *splashScreen, unsigned int splashLines, consoleMenu_t *mainMenu)
{
    consoleSplashScreen = splashScreen;
    consoleSplashLines = splashLines;
    consoleMainMenu = mainMenu;
}

void Console_Main(void)
{
    char selection;

    for (;;)
    {
        Console_PrintNewLine();
        Console_PrintNewLine();
        Console_PrintHeader("Welcome");
        for(int line = 0; line < consoleSplashLines; line++)
        {
            Console_Print("%s", (*consoleSplashScreen)[line]);
        }
        selection = Console_PrintOptionsAndGetResponse(splashOptions, SELECTION_SIZE(splashOptions), 0);
        
        switch(selection)
        {
            case 'm':
                Console_TraverseMenus(consoleMainMenu);
                break;
            case 'o':
                Console_Print(ANSI_COLOR_RED" Options not implemented."ANSI_COLOR_RESET);
                break;
            default:
                Console_Print(ANSI_COLOR_RED" Something went wrong..."ANSI_COLOR_RESET);
                for(;;);
                break;
        };
    }
}

void Console_WaitForKey(void)
{
    Console_Print("Press any key to continue");
    getch();
}

void Console_TraverseMenus(consoleMenu_t *menu)
{
    bool stayPut = true;;
    consoleMenu_t *currentMenu = menu;
    char selection;
    
    do
    {
        Console_PrintMenu(currentMenu);
        selection = Console_PrintOptionsAndGetResponse(menuOptions, SELECTION_SIZE(menuOptions), currentMenu->menuLength);

        // First check if it's a menu selection (selection should be valid)
        if (selection <= '9')
        {
            // Check if we have a submenu
            if (currentMenu->menuItems[(selection-'0')].subMenu != NO_SUB_MENU)
            {
                currentMenu = currentMenu->menuItems[(selection-'0')].subMenu;
            }
            // Check if we have a function pointer
            else if (currentMenu->menuItems[(selection-'0')].functionPointer != NO_FUNCTION_POINTER)
            {
                currentMenu->menuItems[(selection-'0')].functionPointer(NO_ARGS, NO_ARGS);
                // We stay put after executing a function
                // ToDo: Print function return status
            }
            else
            {
                Console_Print(ANSI_COLOR_RED" No submenu or function pointer!!!"ANSI_COLOR_RESET);
                for(;;);
            }
        }
        // Check if we're traversing up
        else if (selection == 'u')
        {
            Console_Print(ANSI_COLOR_GREEN" Going up!"ANSI_COLOR_RESET);
            // Go up if we can
            if (currentMenu->topMenu != NO_TOP_MENU)
            {
                currentMenu = currentMenu->topMenu;
            }
        }
        // Check if we're traversing to top
        else if (selection == 't')
        {
            Console_Print(ANSI_COLOR_GREEN" Going to the top!"ANSI_COLOR_RESET);
            // Go up until we hit top menu
            while (currentMenu->topMenu != NO_TOP_MENU)
            {
                currentMenu = currentMenu->topMenu;
            };
            
        }
        // Check if we're quitting
        else if (selection == 'q')
        {
            Console_Print(ANSI_COLOR_GREEN" Quitting!"ANSI_COLOR_RESET);
            stayPut = false;
        }
        else
        {
            Console_Print(ANSI_COLOR_RED" Something went wrong..."ANSI_COLOR_RESET);
            for(;;);
        }
    }
    while (stayPut);
}

char Console_PrintOptionsAndGetResponse(consoleSelection_t *selections, unsigned int numSelections, unsigned int numMenuSelections)
{
    // ToDo: Assert on number of menu selections greater than 10
    char c;
    bool valid = false;
    
    do
    {
        Console_PrintDivider();
        // Print menu selections (these will override any conflicting passed in selections)
        if (numMenuSelections != 0)
        {
            Console_PrintNoEol(" ["ANSI_COLOR_YELLOW"0"ANSI_COLOR_RESET"-"ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-item ", ('0' + (numMenuSelections - 1)));
        }
        // Print passed in selections
        for (int i = 0; i < numSelections; i++)
        {
            Console_PrintNoEol(" ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"]-%s ", selections[i].key, selections[i].description);
        }
        Console_PrintNewLine();
        Console_PrintDivider();
        Console_PrintNoEol(" Selection > ");
        c = getch();

        // If we have menu selections, check for those first
        if (numMenuSelections != 0)
        {
            // Check if it's a valid menu selection
            if ((c - '0') < numMenuSelections)
            {
                valid = true;
            }
        }
        
        // We didn't get a valid value yet
        if (!valid)
        {
            for (int i = 0; i < numSelections; i++)
            {
                if (c == selections[i].key)
                {
                    valid = true;
                    break;
                }
            }
        }
        
        if (!valid)
        {
            Console_PrintNewLine();
            Console_Print(" Bad selection! ");
        }
    }
    while (!valid);
    
    Console_Print(ANSI_COLOR_GREEN" Selecting %c!"ANSI_COLOR_RESET, c);
    Console_PrintDivider();
    
    return c;
}

void Console_PutChar(char c)
{
    putchar(c);
}

void Console_Print(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    Console_PrintNewLine();
}

void Console_PrintNoEol(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

void Console_PrintNewLine(void)
{
    Console_PutChar('\r');
    Console_PutChar('\n');
}

void Console_PrintHeader(char *headerString)
{
    unsigned int stringLength = strlen(headerString);

    if (stringLength > CONSOLE_WIDTH)
    {
        stringLength = CONSOLE_WIDTH;
        headerString[CONSOLE_WIDTH] = 0;
    }

    Console_PrintNoEol("=[ "ANSI_COLOR_YELLOW"%s"ANSI_COLOR_RESET" ]=", headerString);
    // Fill the rest of the line with '='
    for (int i = 0; i < (CONSOLE_WIDTH - stringLength - 6); i++)
    {
        Console_PutChar('=');
    }
    Console_PrintNewLine();
}

void Console_PrintDivider(void)
{
    for (int i = 0; i < CONSOLE_WIDTH; i++)
    {
        Console_PutChar('-');
    }
    Console_PrintNewLine();
}

void Console_PrintMenu(consoleMenu_t *menu)
{
    Console_PrintNewLine();
    Console_PrintHeader(menu->id.name);
    Console_PrintNewLine();
    Console_Print(" %s", menu->id.description);
    Console_PrintNewLine();
    for (int i = 0; i < menu->menuLength; i++)
    {
        consoleMenuItem_t *menuItem = &(menu->menuItems[i]);
        Console_Print(" ["ANSI_COLOR_YELLOW"%c"ANSI_COLOR_RESET"] %s - %s", '0' + i, menuItem->id.name, menuItem->id.description);
    }
    Console_PrintNewLine();
}