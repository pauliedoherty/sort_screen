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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

template <class T>          /*template to print array to console*/
void print(T* x, int y, WINDOW* localWin, int height, int width)
{
    int row=2, col=1;       //hold position of where to print
    for(int i=0; i<y; i++){
        mvwprintw(localWin, row, col, "%c ", x[i]);
        wrefresh(localWin);
        col = col+2;
        if(col > width-2){
            col = 1;
            row++;
        }if(row > height-3){    //Leave bottom row blank for time taken
            row = 3;
        }
    }
}


template <class T>              /*Template to swap the values of 2 variables*/
void swap(T& x, T& y)
{
    T temp = x;
    x = y;
    y = temp;
}

struct sortParams
 /*sortParams is used to hold the data required to sort the array
   generated fromby the random number generator - randomArray  */
{
    char* sortList;
    int arrSize;
    WINDOW* localWin;
    int winHeight;
    int winWidth;

    sortParams(char* arr, int size, WINDOW* myWin, int height, int width)
        : arrSize(size), localWin(myWin), winHeight(height), winWidth(width)
    {
        sortList = new char[arrSize];
        for(int i=0; i< arrSize; i++){
            sortList[i] = arr[i];
        }
    }
};

void bubbleSort(WINDOW* win, void* data)
{
    struct sortParams *myData;
    char* arr;
    WINDOW* localWin;
    int size, height, width;
    myData = (sortParams*) data;

    arr = myData->sortList;
    size = myData->arrSize;
    localWin = myData->localWin;
    height = myData->winHeight;
    width = myData->winWidth;

    for(int i=1; i<size; i++){
        for(int j=1; j <= size-i; j++){
            if(arr[j-1] > arr[j]){
                swap(arr[j-1],arr[j]);
                pthread_mutex_lock(&mutex);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                print(arr, size, localWin, height, width);
                pthread_mutex_unlock(&mutex);
            }
        }
    }
}

void* initBub(void* data)
{
    struct sortParams *myData;
    WINDOW* localWin;
    myData = (sortParams*) data;

    localWin = myData->localWin;

    use_window(localWin, (NCURSES_WINDOW_CB)bubbleSort, (void*) myData);
    pthread_exit(NULL);
}

void selectionSort(WINDOW* win, void* data)
//*Find minimum element and swap with i on each iteration*/
{
    struct sortParams* myData;
    myData = (sortParams*) data;
    WINDOW* localWin;
    char* arr;
    int size, height, width;
    int min;

    arr = myData->sortList;
    size = myData->arrSize;
    height = myData->winHeight;
    width = myData->winWidth;
    localWin = myData->localWin;

   for(int i=0; i < size-1; i++){
        min = i;
        for(int j=i+1; j < size; j++){
            if(arr[j] < arr[min]){
                min=j;
            }
        }
        swap(arr[min], arr[i]);
        pthread_mutex_lock(&mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        print(arr, size, localWin, height, width);
        pthread_mutex_unlock(&mutex);
   }
    //auto finish = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> elapsed = finish - start;
    //std::cout << "Selection sort:" << std::endl;        //Selection Sort
    //print(a, n);

}

void* initSel(void* data)
{
    struct sortParams* myData;
    myData = (sortParams*) data;
    WINDOW* localWin;
    localWin = myData->localWin;
    use_window(localWin, (NCURSES_WINDOW_CB)selectionSort, (void*) myData);
    pthread_exit(NULL);
}

void insertionSort(WINDOW* win, void* data)
{
    struct sortParams* myData;
    myData = (sortParams*) data;
    WINDOW* localWin;
    char* arr;
    int size, height, width;

    arr = myData->sortList;
    size = myData->arrSize;
    height = myData->winHeight;
    width = myData->winWidth;
    localWin = myData->localWin;

    for(int i=1; i < size; i++){
        char key = arr[i];      //Key holds value to be inserted
        for(int j = i; j > 0 && arr[j-1] > key; j--){
            arr[j] = arr[j-1];  //Sliding over
            arr[j-1] = key;
            pthread_mutex_lock(&mutex);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            print(arr, size, localWin, height, width);
            pthread_mutex_unlock(&mutex);
         }
    }
}

void* initIns(void* data)
{
    struct sortParams* myData;
    myData = (sortParams*) data;
    WINDOW* localWin;
    localWin = myData->localWin;
    use_window(localWin, (NCURSES_WINDOW_CB)insertionSort, myData);
    pthread_exit(NULL);
}

char* randomArray(char* arr, int size, WINDOW *localWin, int height, int width, int min=33, int max=126)
{
    /*Random Number generator Random numbers
    always require an engine and a distribution*/
    std::mt19937 eng(static_cast<unsigned long>(time(nullptr)));    //Mersenne Twister Engine Seed
    for(int i=0; i<size; i++){                                      //Only seed random generator ONCE!!
        std::uniform_int_distribution<int> dist(min, max);          //Uniform integer distribution
        arr[i] = char(dist(eng));                                   //The engine is now passed as a
    }                                                               //Parameter to the distribution

    print(arr, size, localWin, height, width);
    mvwprintw(localWin, height-2, 1, "Random ascii chars will now sort :))");
    wrefresh(localWin);
    return arr;

}

WINDOW* create_newwin(int height, int width, int starty, int startx, std::string title);
void destroy_win(WINDOW* local_win);

int main()
{
    //std::cout.sync_with_stdio(true)
    //auto start = std::chrono::high_resolution_clock::now();

    AsciiSort testAsciis;
    testAsciis.generateRand();
    //testAsciis.print();
    AsciiSort testAscii2(152);
    testAscii2.generateRand();
    testAscii2.print(testAscii2.getAsciiChars());
    testAscii2.runBubbleSort();
    while(!testAscii2.waitForBubSort();
    testAscii2.print(testAscii2.getBubChars());

    WINDOW *winRand, *winBub, *winSel, *winIns;       //Create ptrs for display windows
    pthread_t tBub, tSel, tIns;
    int startx, starty, width, height;                //Hold Window parameters
    int ch;                                           //hold user input

    initscr();                                        //Start curses mode
    cbreak();                                         //turn off line buffering

    keypad(stdscr, TRUE);                             //Turn on keypad mode

    height = LINES/2;         //LINES is number of console rows after initscr() is called
    width = COLS/2;           //COLS is number of console characters in a line
    starty = 0;               //Define where to position window
    startx = 0;

    /* Create a windows for each dispay */
    winRand = create_newwin(height, width, starty, startx, "Random character generator");
    winBub = create_newwin(height, width, starty, startx+width, "Bubble Sort");
    winSel = create_newwin(height, width, starty+height, startx, "Selection Sort");
    winIns = create_newwin(height, width, starty+height, startx+width, "Insertion Sort");
    
    noecho();

    const int size = ((height - 4)*(width - 2)/2);     //Define size of Arrays to be sorted (size of window including headers and spaces)
    char* randArray = new char[size];                //Create char array on Heap

    randArray = randomArray(randArray, size, winRand, height, width);        //Fill array with random ascii symbols

    sortParams randData(randArray, size, winRand, height, width);   //Create structure to old Random Array
    sortParams* bubData = new sortParams(randData);
    sortParams* selData = new sortParams(randData);
    sortParams* insData = new sortParams(randData);
    bubData->localWin = winBub;
    selData->localWin = winSel;
    insData->localWin = winIns;

/*    char* bubSort = new char[size];                   //Create new array for Bubble sort
    for(int i=0; i < size; i++){
        bubSort[i] = randArray[i];
    }
    char* selSort = new char[size];                   //Create new array for Selection sort
    for(int i=0; i < size; i++){
        selSort[i] = randArray[i];
    }
    char* insSort = new char[size];                   //Create new array for Insertion sort
    for(int i=0; i < size; i++){
        insSort[i] = randArray[i];
    }

    double timeBub, timeSel, timeIns;   */              //Hold execution time of sort algos
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tBub, &attr, initBub, (void*) bubData);
    pthread_create(&tSel, &attr, initSel, (void*) selData);
    pthread_create(&tIns, &attr, initIns, (void*) insData);

    pthread_join(tBub, NULL);
    pthread_join(tSel, NULL);
    pthread_join(tIns, NULL);

    delete[] randArray;     //Delete heap memory
    delete[] bubData->sortList;
    delete bubData;
    delete[] selData->sortList;
    delete selData;
    delete[] insData->sortList;
    delete insData;

    std::cin.get();
    return 0;
}

WINDOW *create_newwin(int height, int width, int starty, int startx, std::string title)
/*  Fuction to create new ncurses window  */
{	WINDOW *local_win;

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

void destroy_win(WINDOW *local_win)
{
    wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    delwin(local_win);
}




