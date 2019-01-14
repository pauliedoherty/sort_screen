#ifndef DISPLAY_H
#define DISPLAY_H
#include <ncurses.h>
#include <string>
#include <thread>

#define DEFAULT_DELAY_TIME 5;                               //Wait period for display updated

class Display
{
public:
    Display();                                              //Constructor for standard window
    Display(int height, int width, int starty,              //Constructor for window with known parameters
             int startx, std::string title, int delay) ;
    ~Display();                                             //Destructor for window
    void clearDisp(int height, int width);                  //Clears all data within window
    void clearData(int height, int width);                  //Clears all data except window title
    void print(char* data, int size, int height, int width) const;      //Print data to window
    void printInit(char* data, int size, int height, int width) const;  //print data with delay
    /*TODO - Make printFooter methods general to allow user configuration*/
    void printRandFooter(int line, int elements) const;     //Prints footer for Rand Window
    void printSortFooter(int line, int swapCount) const;    //Prints footer for Sort windows
    void setDelayTime(int delay);                           //Setter for mDelayTime
    int getDelayTime();                                     //returns mDelayTime
    WINDOW* getDisplayWin() const;                          //return display window

protected:
    WINDOW *mDisplayWin;                                    //holds display window for object
    WINDOW *createNewWin(int height, int width, int starty, //Creates new window
            int startx, std::string title);
    void destroyWin(WINDOW *local_win, int height, int width);    //Deletes window
    int mDelayTime;       //holds time in milliseconds that diplay will pause between updates
};

#endif // DISPLAY_H
