#include "console.h"

void Console_PrintMenu(consoleMenu_t *menu)
{
    for (int i = 0; i < menu->menuLength; i++)
    {
        consoleMenuItem_t *menuItem = &(menu->menuItems[i]);
        printf(" [%c] %s - %s\r\n", '0' + i, menuItem.id.name, menuItem.id.description);
    }
}