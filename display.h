#ifndef DISPLAY_H
#define DISPLAY_H
#include <ncurses.h>
#include <string>
#include <thread>

class Display
{
public:
    Display();                              //Constructor for standard window
    Display(int height, int width,          //Constructor for window with known parameters
            int starty, int startx, std::string title);
    ~Display();                             //Default Destructor for window
    void clearDisp(int height, int width);
    void clearData(int height, int width);
    void print(char* data, int size, int height, int width) const;  //Print to window
    void printInit(char* data, int size, int height, int width) const;  //print with sleep added
//    void printTitle(int height, int width, std::string title) const;              //prints title to window
    void printRandFooter(int line, int elements) const;
    void printSortFooter(int line, int swapCount) const;
protected:
    WINDOW *mDisplayWin;                    //holds display window for object
    WINDOW *createNewWin(int height,       //Creates new window
            int width, int starty, int startx, std::string title);
    void destroyWin(WINDOW *local_win, int height, int width);    //Deletes window
};

#endif // DISPLAY_H
