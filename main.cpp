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



#define DELAY_TIME 5      //Delay time for display !!TODO investigate issues with display.h Glob_var
#define SORT_SIZE 152     //Number of chars to ge generated and sorted

struct params
{
    params(int numChars = DEFAULT_NUM_CHARS, int minChar = DEFAULT_MIN,
           int maxChar = DEFAULT_MAX, int delay = DELAY_TIME)
        : _numChars(numChars), _minChar(minChar), _maxChar(maxChar), _delay(delay) {}
    int _numChars;      //Number of elements in array to be generated and sorted
    int _minChar;       //minimum ascii char that can be selected
    int _maxChar;       //maximum ascii char that can be selected
    int _delay;         //delay time in milliseconds between display updates

    std::string randWinTitle = "Random Ascii List";
    std::string bubWinTitle = "Bubble Sort";
    std::string selWinTitle = "Selection Sort";
    std::string insWinTitle = "Insertion Sort";

};

bool checkInput(int argc, char* argv[]);
void displayHelp(std::string appName);
int getKey(WINDOW* localWin);   //Function to input from keyboard


int main(int argc, char* argv[])
{

    params params;              //Struct to hold application parameters

    /*Check input arguments if there are any...*/
    if(argc > 1) { if(!checkInput(argc, argv)) { displayHelp(argv[0]); return 0; } }

    switch (argc){
        case(1):
            break;              //Assign default parameters
        case(2):
            params._numChars = strtol(argv[1], NULL, 10);         //inputted _charNums by user
            break;
        case(3):
            params._numChars = strtol(argv[1], NULL, 10);
            params._minChar = strtol(argv[2], NULL, 10);
            break;
        case(4):
            params._numChars = strtol(argv[1], NULL, 10);
            params._minChar = strtol(argv[2], NULL, 10);
            params._maxChar = strtol(argv[3], NULL, 10);
            break;
        case(5):
            params._numChars = strtol(argv[1], NULL, 10);
            params._minChar = strtol(argv[2], NULL, 10);
            params._maxChar = strtol(argv[3], NULL, 10);
            params._delay = strtol(argv[4], NULL, 10);
            break;
        default:
            std::cout << "Too many arguments\n";
            displayHelp(argv[0]);
            return 0;
    }

    /*Check user inputs are valid*/
    if(params._numChars < 2 || params._numChars > 152) { displayHelp(argv[0]); return 0; }
    if(params._minChar < 33 || params._minChar > 125) { displayHelp(argv[0]); return 0; }
    if(params._maxChar < 34 || params._maxChar > 126) { displayHelp(argv[0]); return 0; }
    if(params._delay < 1 || params._delay > 5000) { displayHelp(argv[0]); return 0; }


    std::cout << "Sort animation starting...\n";
    std::cout << "Press 'Q' to end animation immediately\n";
    std::cout << "Press any other key to end animation after current sort completes\n";
    std::cout << "Press Enter to begin...\n";
    std::cin.get();

    initscr();                                //Start curses mode
    noecho();                                 //stops key entries from being displayed on screen
    WINDOW *inputWin;                         //Window to scan for user input
    inputWin = newwin(1, 1, LINES+1, COLS+1); /*initialise window and set it outside*/
                                              /*of the visible area of the screen   */
    cbreak();                                 //turn off line buffering
    keypad(inputWin, TRUE);                   //Turn on keypad mode
    nodelay(inputWin, TRUE);                  //getch() call will not wait for user entry
    int startx, starty, width, height;        //Window parameters
    height = LINES/2;                         //LINES = number of console display rows
    width = COLS/2;                           //COLS = number of console display chars/row
    starty = 0;                               //Define where to position window on console
    startx = 0;                               //Top left corner


    /* Create bject to generate Random chars and control sorting */
    AsciiSort sortMe(params._numChars, params._minChar, params._maxChar);

    /* Create display windows to show sorting progress           */
    Display randWin(height, width, starty, startx, params.randWinTitle, params._delay);
    Display bubWin(height, width, starty, startx+width, params.bubWinTitle, params._delay);
    Display selWin(height, width, starty+height, startx, params.selWinTitle, params._delay);
    Display insWin(height, width, starty+height, startx+width, params.insWinTitle, params._delay);

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

            switch(getKey(inputWin)){           //Check for user input and terminate program if so
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

    echo();
    nocbreak();
    endwin();                           //ends curses mode
    return 0;                           //end program
}

bool checkInput(int argc, char* argv[])
{
    char* p;                    //pointer for string to long function
    int temp;                   //hold argv[i] value for testing
    bool output = true;         //temporarily hold output
    if(argc > 5) {
        std::cout << "Too many input arguments...\n";
        return false; }
    for(int i=1; i<argc; i++){
        temp = strtol(argv[i], &p, 10);
        if(*p != '\0') { output = false; }
    }
    return output;
}

void displayHelp(std::string appName)
{
    std::cout << appName << " accepts 4 input parameter:\n";
    std::cout << "1. Number of chars to generate\n";
    std::cout << "   Type: <int> Restriction: 2 <= Int <= 152\n";
    std::cout << "   Default Value: 152\n";
    std::cout << "2. Minimum ascii value to generate\n";
    std::cout << "   Type: <int> Restriction: 33 <= Int <= 151\n";
    std::cout << "   Default Value: 33\n";
    std::cout << "3. Maximum ascii value to generate\n";
    std::cout << "   Type: <int> Restriction: 34 <= Int <= 126\n";
    std::cout << "   Default Value: 126\n";
    std::cout << "4. Delay time between display updates during sort cycles\n";
    std::cout << "   Type: <int> Restriction: 1 <= Int <= 5000 (milliseconds)\n";
    std::cout << "   Default Value: 5 (milliseconds)\n";
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
