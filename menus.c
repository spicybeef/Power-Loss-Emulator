#include "menus.h"

consoleMenuItem_t mainMenuItems[] = 
{
    {{"First",  "The first menu item"},     NO_FUNCTION_POINTER},
    {{"Second", "The second menu item"},    NO_FUNCTION_POINTER},
    {{"Third",  "The third menu item"},     NO_FUNCTION_POINTER},
};
consoleMenu_t mainMenu = {{"Main Menu", ""}, mainMenuItems, NO_TOP_MENU, MENU_LENGTH(mainMenuItems)};