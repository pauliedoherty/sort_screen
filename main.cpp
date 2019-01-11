#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <random>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <ncurses.h>
#include "asciisort.h"
#include "display.h"

#define DELAY_TIME 5     //Delay time for display
#define SORT_SIZE 152     //Number of chars to ge generated and sorted

int getKey(WINDOW* localWin);   //Function to input from keyboard

int main()
{

    std::cout << "Sort animation starting...\n";
    std::cout << "Press 'Q' to end animation immediately\n";
    std::cout << "Press any other key to end animation after current sort completes\n";
    std::cout << "Press Enter to begin...\n";
    std::cin.get();

    initscr();                     //Start curses mode
    noecho();                      //stops key entries from being displayed on screen
    WINDOW *inputWin;              //Window to scan for user input
    inputWin = newwin(1, 1, LINES+1, COLS+1); /*initialise window and set it outside*/
                                              /*of the visible area of the screen   */
    cbreak();                      //turn off line buffering
    keypad(inputWin, TRUE);        //Turn on keypad mode
    nodelay(inputWin, TRUE);       //getch() call will not wait for user entry
    int startx, starty, width, height;  //Window parameters
    height = LINES/2;              //LINES is number of console rows after initscr() is called
    width = COLS/2;                //COLS is number of console characters in a line
    starty = 0;                    //Define where to position window on console
    startx = 0;                    //Top left corner

    AsciiSort sortMe(SORT_SIZE);   //Object to generate Random chars and control sorting
    /*TODO - Put strings into structs OR config file*/
    Display randWin(height, width, starty, startx, "Random Ascii List", DELAY_TIME);
    Display bubWin(height, width, starty, startx+width, "Bubble Sort", DELAY_TIME);
    Display selWin(height, width, starty+height, startx, "Selection Sort", DELAY_TIME);
    Display insWin(height, width, starty+height, startx+width, "Insertion Sort", DELAY_TIME);

    sortMe.generateRand();          //Generate random array of ascii chars

    /*Output random generated chars to sort windows*/
    randWin.printInit(sortMe.getAsciiChars(), sortMe.getNumElements(), height, width);
    randWin.printRandFooter(height-2, SORT_SIZE);
    bubWin.printInit(sortMe.getBubChars(), sortMe.getNumElements(), height, width);
    bubWin.printSortFooter(height-2, sortMe.getBubSwapCount());
    selWin.printInit(sortMe.getSelChars(), sortMe.getNumElements(), height, width);
    selWin.printSortFooter(height-2, sortMe.getSelSwapCount());
    insWin.printInit(sortMe.getInsChars(), sortMe.getNumElements(), height, width);
    insWin.printSortFooter(height-2, sortMe.getInsSwapCount());

    pthread_mutex_lock(&sortMe.mainMutex);  //Lock mutex to control next Display update cycle
    sortMe.initAllSorts();                  //Starts sorting threads

    while(sortMe.contFlag && !sortMe.exitFlag){

        while(sortMe.activeSorts > 0 && !sortMe.exitFlag){

            switch(getKey(inputWin)){   //Check for user input and terminate program if so
                case 0: break;                  //no input detected - do nothing
                case 1:                         //Escape key detected - begin shutdown
                    sortMe.shutdownRoutine();
                    continue;
                case 2:                         //non Escape key detected -
                    sortMe.contFlag = false;    //close after current sort cycle
                    break;
            }

            /*Wait for sort threads to give go-ahead to output next swap iteration to console*/
            if(!sortMe.mainFlag){
                pthread_cond_wait(&sortMe.mainCond, &sortMe.mainMutex);
                sortMe.mainFlag = false;
            }
            pthread_mutex_unlock(&sortMe.mainMutex);

            /*Update console with new sort iteration*/
            bubWin.print(sortMe.getBubChars(), sortMe.getNumElements(), height, width);
            bubWin.printSortFooter(height-2, sortMe.getBubSwapCount());
            selWin.print(sortMe.getSelChars(), sortMe.getNumElements(), height, width);
            selWin.printSortFooter(height-2, sortMe.getSelSwapCount());
            insWin.print(sortMe.getInsChars(), sortMe.getNumElements(), height, width);
            insWin.printSortFooter(height-2, sortMe.getInsSwapCount());

            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_TIME)); //delay

            pthread_mutex_lock(&sortMe.sortMutex);      //Lock sort mutex
            sortMe.activeSwaps = sortMe.activeSorts;    //TODO - rethink how these are initialised
            sortMe.swapFlag = true;                     //Set sortFlag so next swap iteration can start
            pthread_cond_broadcast(&sortMe.sortCond);   //Tell sort threads to start next swap iter
            pthread_mutex_unlock(&sortMe.sortMutex);    //release mutex
        }

        if(!sortMe.contFlag){
            pthread_mutex_lock(&sortMe.contMutex);      //required in case user ends app when
            pthread_cond_broadcast(&sortMe.contCond);   //one sort has already complete and is
            pthread_mutex_unlock(&sortMe.contMutex);    //waiting on contCond signal
            break;
        }

        randWin.clearData(height, width);   //Clear windows for next sort cycle
        bubWin.clearData(height, width);
        selWin.clearData(height, width);
        insWin.clearData(height, width);

        sortMe.activeSwaps = 3;           //Reinitialise Sort/Swap trackers
        sortMe.activeSorts = 3;
        sortMe.resetBubSwapCount();
        sortMe.resetSelSwapCount();
        sortMe.resetInsSwapCount();
        sortMe.mainFlag = false;
        sortMe.swapFlag = false;

        sortMe.generateRand();
        randWin.printInit(sortMe.getAsciiChars(), sortMe.getNumElements(), height, width);
        randWin.printRandFooter(height-2, SORT_SIZE);
        bubWin.printInit(sortMe.getBubChars(), sortMe.getNumElements(), height, width);
        bubWin.printSortFooter(height-2, sortMe.getBubSwapCount());
        selWin.printInit(sortMe.getSelChars(), sortMe.getNumElements(), height, width);
        selWin.printSortFooter(height-2, sortMe.getSelSwapCount());
        insWin.printInit(sortMe.getInsChars(), sortMe.getNumElements(), height, width);
        insWin.printSortFooter(height-2, sortMe.getInsSwapCount());
        pthread_mutex_lock(&sortMe.mainMutex);

        pthread_mutex_lock(&sortMe.contMutex);      //Signal to sort threads that new
        pthread_cond_broadcast(&sortMe.contCond);   //sort cycle can start
        pthread_mutex_unlock(&sortMe.contMutex);

    }

    sortMe.waitForBubSort();                    //wait for threads to finish
    sortMe.waitForSelSort();
    sortMe.waitForInsSort();

    randWin.clearDisp(height, width);           //Clear displays
    bubWin.clearDisp(height, width);
    selWin.clearDisp(height, width);
    insWin.clearDisp(height, width);
    wmove(randWin.getDisplayWin(),0,0);         //reposition curser
    delwin(inputWin);                           //Delete key input window

    pthread_mutex_destroy(&sortMe.sortMutex);   //destroy mutex's and conditional variables
    pthread_cond_destroy(&sortMe.sortCond);
    pthread_mutex_destroy(&sortMe.mainMutex);
    pthread_cond_destroy(&sortMe.mainCond);
    pthread_mutex_destroy(&sortMe.contMutex);
    pthread_cond_destroy(&sortMe.contCond);

    endwin();                           //ends curses mode
    return 0;                           //end program
}

int getKey(WINDOW* localWin)            /*Checks window for keyboard input*/
{                                       /*in no delay mode getch() return */
    int ch;                             /*ERR if no input is detected     */
    ch = wgetch(localWin);
    if(ch == 'Q'){
        ungetch(ch);                    //Remove returned character
        return 1;
    }else if(ch != ERR && ch != 'Q'){   /*return 2 if any input other than*/
        ungetch(ch);                    /*Q has been detected             */
        return 2;
    }else return 0;
}
