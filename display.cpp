#include "display.h"

/*Constructors*/
Display::Display()
{
    mDisplayWin = createNewWin(LINES, COLS, 0, 0, "Default Window");
}

Display::Display(int height, int width, int starty, int startx, std::string title)
{
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
    }

    for(int i=1; i < height-1; i++){
        mvwprintw(local_win, i, 0, "|");
        mvwprintw(local_win, i, width-1, "|");
        wrefresh(local_win);
    }

    wrefresh(local_win);		/* Show that box 		*/
    return local_win;
}

void Display::clearDisp(int height, int width)
{
    for(int i=1; i < height-1; i++){
        for(int j=1; j < width-1; j++){
            mvwprintw(mDisplayWin, i, j, " ");
            //wrefresh(mDisplayWin);
        }
    }
    wrefresh(mDisplayWin);
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

void Display::destroyWin(WINDOW *local_win, int height, int width)
{
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    delwin(local_win);
}
