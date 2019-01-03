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


int main()
{
    int printCount = 0;
    //std::cout.sync_with_stdio(true)
    //auto start = std::chrono::high_resolution_clock::now();


    initscr();                                        //Start curses mode
    cbreak();                                         //turn off line buffering
    int startx, starty, width, height;                //Hold Window parameters
    height = LINES/2;         //LINES is number of console rows after initscr() is called
    width = COLS/2;           //COLS is number of console characters in a line
    starty = 0;               //Define where to position window
    startx = 0;

    Display randWin(height, width, starty, startx, "Random Ascii List");
    Display bubWin(height, width, starty, startx+width, "Bubble Sort");
    Display selWin(height, width, starty+height, startx, "Selection Sort");
    Display insWin(height, width, starty+height, startx+width, "Insertion Sort");

    AsciiSort sortMe(152);
    sortMe.generateRand();
    randWin.print(sortMe.getAsciiChars(), sortMe.getNumElements(), height, width);
    pthread_mutex_init(&sortMe.sortMutex, NULL);
    pthread_cond_init(&sortMe.sortCond, NULL);

    pthread_mutex_lock(&sortMe.mainMutex);
    sortMe.activeThreads = 3;
    sortMe.bubbleSort();
    sortMe.selectionSort();
    sortMe.insertionSort();

    while(sortMe.totalThreads > 0){

        if(!sortMe.mainFlag){
            pthread_cond_wait(&sortMe.mainCond, &sortMe.mainMutex);
            sortMe.mainFlag = false;
        }
        pthread_mutex_unlock(&sortMe.mainMutex);

        bubWin.print(sortMe.getBubChars(), sortMe.getNumElements(), height, width);
        selWin.print(sortMe.getSelChars(), sortMe.getNumElements(), height, width);
        insWin.print(sortMe.getInsChars(), sortMe.getNumElements(), height, width);
        printCount++;

        pthread_mutex_unlock(&sortMe.mainMutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(3));
        pthread_mutex_lock(&sortMe.sortMutex);
        sortMe.activeThreads = sortMe.totalThreads;
        sortMe.sortFlag = true;
        pthread_cond_broadcast(&sortMe.sortCond);
        pthread_mutex_unlock(&sortMe.sortMutex);

    }

    sortMe.waitForBubSort();
    sortMe.waitForSelSort();
    sortMe.waitForInsSort();

    int ch;                                           //hold user input                                 //turn off line buffering

    keypad(stdscr, TRUE);                             //Turn on keypad mode
    
    noecho();

    std::cin.get();
    return 0;
}

