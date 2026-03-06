// #ifndef CHARCHECK_H
// #define CHARCHECK_H

// #include <iostream>
// #ifdef _WIN32
// #include <conio.h> // For _getch() on Windows
// #define OS_NAME "Windows"
// #define CLEAR "cls"
// #elif __APPLE__
// #include <TargetConditionals.h>
// #include <termios.h>
// #include <unistd.h>
// #if TARGET_OS_MAC
// #define OS_NAME "macOS"
// #define CLEAR "clear"
// #endif
// #else
// #define OS_NAME "Unknown OS"
// #define CLEAR "clear"
// #endif

// char getSingleChar() {
// #ifdef _WIN32
//     return _getch(); // Windows: Get character without Enter
// #elif __APPLE__
//     struct termios oldt, newt;
//     char ch;
//     tcgetattr(STDIN_FILENO, &oldt); // Get current terminal attributes
//     newt = oldt;
//     newt.c_lflag &= ~(ICANON | ECHO); // Disable line buffering & echo
//     tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new attributes
//     ch = getchar(); // Read single character
//     tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old attributes
//     return ch;
// #endif
// }

// #endif // CHARCHECK_H

// chatgpt
#ifndef CHARCHECK_H
#define CHARCHECK_H

#include <iostream>

#ifdef _WIN32
#include <conio.h> // For _getch() on Windows
#define OS_NAME "Windows"
#define CLEAR "cls"
#elif __APPLE__
#include <TargetConditionals.h>
#include <termios.h>
#include <unistd.h>
#if TARGET_OS_MAC
#define OS_NAME "macOS"
#define CLEAR "clear"
#endif
#else
#define OS_NAME "Unknown OS"
#define CLEAR "clear"
#endif

// f
char getSingleChar() {
#ifdef _WIN32
    char ch;
    ch = _getch();  // Windows: Get character without Enter
    switch (ch) {
    case 72 : ch = 'w'; break; //Up
    case 80 : ch = 's'; break; // Down
    case 75 : ch = 'a'; break; // Left
    case 77 : ch = 'd'; break; // Right
    }
    return ch;
#elif __APPLE__
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();

    // obsługa strzałek
    if(ch == 27) { // ESC
        char ch2 = getchar();
        if(ch2 == '[') {
            char ch3 = getchar();
            switch(ch3) {
            case 'A': ch = 'w'; break; // Up
            case 'B': ch = 's'; break; // Down
            case 'C': ch = 'd'; break; // Right
            case 'D': ch = 'a'; break; // Left
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
#endif
}

#endif // CHARCHECK_H
