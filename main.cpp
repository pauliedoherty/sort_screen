#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <random>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <functional>
#include <ncurses.h>
#include "asciisort.h"
#include "display.h"

#define SORT_SIZE 152

int main()
{

    initscr();                     //Start curses mode
    cbreak();                      //turn off line buffering
    keypad(stdscr, TRUE);          //Turn on keypad mode
    noecho();                      //turn off line buffering
    int ch;                        //hold user input
    int startx, starty, width, height;    //Hold Window parameters
    height = LINES/2;         //LINES is number of console rows after initscr() is called
    width = COLS/2;           //COLS is number of console characters in a line
    starty = 0;               //Define where to position window
    startx = 0;

    Display randWin(height, width, starty, startx, "Random Ascii List");
    Display bubWin(height, width, starty, startx+width, "Bubble Sort");
    Display selWin(height, width, starty+height, startx, "Selection Sort");
    Display insWin(height, width, starty+height, startx+width, "Insertion Sort");

    AsciiSort sortMe(SORT_SIZE);

//    pthread_mutex_init(&sortMe.mainMutex, NULL);
//    pthread_cond_init(&sortMe.mainCond, NULL);

//    pthread_mutex_init(&sortMe.sortMutex, NULL);
//    pthread_cond_init(&sortMe.sortCond, NULL);

    sortMe.generateRand();
    randWin.print(sortMe.getAsciiChars(), sortMe.getNumElements(), height, width);
    randWin.printRandFooter(height-2, SORT_SIZE);
    pthread_mutex_lock(&sortMe.mainMutex);
    sortMe.runAllSorts();

    while(1){



        while(sortMe.totalThreads > 0){

            if(!sortMe.mainFlag){
                pthread_cond_wait(&sortMe.mainCond, &sortMe.mainMutex);
                sortMe.mainFlag = false;
            }
            pthread_mutex_unlock(&sortMe.mainMutex);

            bubWin.print(sortMe.getBubChars(), sortMe.getNumElements(), height, width);
            bubWin.printSortFooter(height-2, sortMe.getBubSwapCount());
            selWin.print(sortMe.getSelChars(), sortMe.getNumElements(), height, width);
            selWin.printSortFooter(height-2, sortMe.getSelSwapCount());
            insWin.print(sortMe.getInsChars(), sortMe.getNumElements(), height, width);
            insWin.printSortFooter(height-2, sortMe.getInsSwapCount());

            //pthread_mutex_unlock(&sortMe.mainMutex);
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
            pthread_mutex_lock(&sortMe.sortMutex);
            sortMe.activeThreads = sortMe.totalThreads;
            sortMe.sortFlag = true;
            pthread_cond_broadcast(&sortMe.sortCond);
            pthread_mutex_unlock(&sortMe.sortMutex);

        }

        randWin.clearDisp(height, width);
        bubWin.clearDisp(height, width);
        selWin.clearDisp(height, width);
        insWin.clearDisp(height, width);

        //TODO - implement way of resetting search chars
        if(!sortMe.contFlag) break;

        sortMe.activeThreads = 3;
        sortMe.totalThreads = 3;
        sortMe.setBubSwapCount(0);
        sortMe.setSelSwapCount(0);
        sortMe.setInsSwapCount(0);
        sortMe.mainFlag = false;
        sortMe.sortFlag = false;

        sortMe.generateRand();
        randWin.print(sortMe.getAsciiChars(), sortMe.getNumElements(), height, width);
        randWin.printRandFooter(height-2, SORT_SIZE);
        pthread_mutex_lock(&sortMe.mainMutex);

        pthread_mutex_lock(&sortMe.contMutex);
        pthread_cond_broadcast(&sortMe.contCond);
        pthread_mutex_unlock(&sortMe.contMutex);

        //TODO - implement way of broadcasting signal to cont condVar

        //if(FLAG){
        //Mutex for continuing
        //Conditional Variable for continuing
        //Flag for continuing
        //}



    }

    sortMe.waitForBubSort();
    sortMe.waitForSelSort();
    sortMe.waitForInsSort();

    pthread_mutex_destroy(&sortMe.sortMutex);
    pthread_cond_destroy(&sortMe.sortCond);

    pthread_mutex_destroy(&sortMe.mainMutex);
    pthread_cond_destroy(&sortMe.mainCond);



    std::cin.get();
    return 0;
}

