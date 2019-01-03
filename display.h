#ifndef DISPLAY_H
#define DISPLAY_H
#include "asciisort.h"
#include <ncurses.h>
#include <string>

class Display
{
public:
    Display();                              //Constructor for standard window
    Display(int height, int width,          //Constructor for window with known parameters
            int starty, int startx, std::string title);
    ~Display();                             //Default Destructor for window
    void clearDisp(int height, int width);
    void print(char* data, int size, int height, int width) const;  //Print to window
    void printColor(WINDOW *localWin, AsciiSort &data,    //Prints to window with special chars coloured
                            int char1, int char2) const;
protected:
    WINDOW *mDisplayWin;                    //holds display window for object
    WINDOW *createNewWin(int height,       //Creates new window
            int width, int starty, int startx, std::string title);
    void destroyWin(WINDOW *local_win, int height, int width);    //Deletes window
};

#endif // DISPLAY_H
