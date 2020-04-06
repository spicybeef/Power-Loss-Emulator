#ifndef UTILS_H
#define UTILS_H

void putch(char c);
int getch(void);
int getche(void);
int scanf(const char *format, ...);

void Util_GeneratePulse(void);
void Util_WaitMicroseconds(uint16_t microseconds);

#endif // UTILS_H