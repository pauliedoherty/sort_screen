#include "display.h"

/*Constructors*/
Display::Display()
{
    mDelayTime = DEFAULT_DELAY_TIME; //Set default delay time
    mDisplayWin = createNewWin(LINES, COLS, 0, 0, "Default Window");
}

Display::Display(int height, int width, int starty, int startx,  std::string title, int delay)
{
    mDelayTime = delay;
    mDisplayWin = createNewWin(height, width, starty, startx, title);
}

/*Destructors*/
Display::~Display()
{
    destroyWin(mDisplayWin, LINES, COLS);
}

/*Protected member functions*/
WINDOW* Display::createNewWin(int height, int width,
              int starty, int startx, std::string title)
/*  Fuction to create new ncurses window  */
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    mvwprintw(local_win, 0, 0, "+");
    mvwprintw(local_win, 0, width-1, "+");
    mvwprintw(local_win, height-1, 0, "+");
    mvwprintw(local_win, height-1, width-1, "+");
    mvwprintw(local_win, 1, (width/2) - (title.length()/2), title.c_str());
    for(int i=1; i<width-1; i++){
        mvwprintw(local_win, 0, i, "-");
        mvwprintw(local_win, height-1, i, "-");
        wrefresh(local_win);
        std::this_thread::sleep_for(std::chrono::milliseconds(mDelayTime));
    }

    for(int i=1; i < height-1; i++){
        mvwprintw(local_win, i, 0, "|");
        mvwprintw(local_win, i, width-1, "|");
        wrefresh(local_win);
        std::this_thread::sleep_for(std::chrono::milliseconds(mDelayTime));
    }

    wrefresh(local_win);		/* Show that box 		*/
    return local_win;
}

void Display::clearDisp(int height, int width)
{
    for(int i=1; i < height-1; i++){
        for(int j=1; j < width-1; j++){
            mvwprintw(mDisplayWin, i, j, " ");
            wrefresh(mDisplayWin);
            std::this_thread::sleep_for(std::chrono::milliseconds(mDelayTime));
        }
    }
}

void Display::clearData(int height, int width)
{
    for(int i=2; i < height-1; i++){
        for(int j=1; j < width-1; j++){
            mvwprintw(mDisplayWin, i, j, " ");
            wrefresh(mDisplayWin);
            std::this_thread::sleep_for(std::chrono::milliseconds(mDelayTime));
        }
    }
}

void Display::print(char* data, int size, int height, int width) const
{
    int row=2, col=1;       //hold position of where to print
    for(int i=0; i<size; i++){
        mvwprintw(mDisplayWin, row, col, "%c ", data[i]);
        col = col+2;
        if(col > width-2){
            col = 1;
            row++;
        }if(row > height-3){    //Leave bottom row blank for time taken
            row = 3;
        }
    }
    wrefresh(mDisplayWin);
}

void Display::printInit(char *data, int size, int height, int width) const
{
    {
        int row=2, col=1;       //hold position of where to print
        for(int i=0; i<size; i++){
            std::this_thread::sleep_for(std::chrono::milliseconds(mDelayTime));
            mvwprintw(mDisplayWin, row, col, "%c ", data[i]);
            wrefresh(mDisplayWin);
            col = col+2;
            if(col > width-2){
                col = 1;
                row++;
            }if(row > height-3){    //Leave bottom row blank for time taken
                row = 3;
            }
        }
    }
}

void Display::printRandFooter(int line, int elements) const
{
    mvwprintw(mDisplayWin, line, 1, "Number of Chars: %d", elements);
    wrefresh(mDisplayWin);
}

void Display::printSortFooter(int line, int swapCount) const
{
    mvwprintw(mDisplayWin, line, 1, "Total Swaps: %d", swapCount);
    wrefresh(mDisplayWin);
}

void Display::setDelayTime(int delay)
{
    mDelayTime = delay;
}

int Display::getDelayTime()
{
    return mDelayTime;
}

WINDOW* Display::getDisplayWin() const
{
    return mDisplayWin;
}

void Display::destroyWin(WINDOW *local_win, int height, int width)
{
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    delwin(local_win);
}
