#ifndef ASCIISORT_H
#define ASCIISORT_H
#include "pthread.h"

#define MIN 33          //MIN ascii char to generate
#define MAX 126         //MAX ascii char to generate
#define NUM_CHARS 152   /*Number of ascii chars to generate 152 chars      */
                        /*works nicely with 4 windows on std 24x80 console */

class AsciiSort
{
public:
    AsciiSort();
    AsciiSort(int size);
    ~AsciiSort();
    void generateRand();             //Generates Random Ascii Array
    void initAllSorts();             //initialise all sorting algorithms
//    void bubbleSort();               //Call bubble sort Algorithm             /*TODO: update required
//    void selectionSort();            //runs SelectionSort on its own thread   /*to make compatible
//    void insertionSort();            //runs InsertionSort on its own thread   /*w/main
    void waitForBubSort() const;     //Wait for Bubble sort thread to finish
    void waitForSelSort() const;     //Wait for Selection sort thread to finish
    void waitForInsSort() const;     //Wait for Insertion sort thread to finish
    char* getAsciiChars() const;     //ptr to asciiChars
    char* getBubChars() const;       //ptr to bubble chars
    char* getSelChars() const;       //ptr to Selection chars
    char* getInsChars() const;       //ptr to INsertion chars
    int getBubSwapCount() const;     //Return number of swaps required to sort an array
    int getSelSwapCount() const;
    int getInsSwapCount() const;
    void resetBubSwapCount();        //Reset swap counts to zero
    void resetSelSwapCount();
    void resetInsSwapCount();
    int getNumElements() const;      //Return number of Ascii Chars generated
    void shutdownRoutine();          //Call to quit sort cycle
    int activeSwaps;                 //holds number of threads that are in process of a swap iteration
    int activeSorts;                 //Holds number of threads that are in process of sort cycle
    bool mainFlag;                   //Object uses this to tell main program swap iteration completed
    bool swapFlag;                   //Calling program uses this to tell object to begin swap cycle again
    bool contFlag;                   //Tells the threads to stay alive and restart the sort cycle
    bool exitFlag;                   //Tells the sort threads that user has requested immediate shutdown
    pthread_mutex_t sortMutex;       //Mutex to handle bubble sort
    pthread_cond_t sortCond;         //Conditional variable bubble
    pthread_mutex_t mainMutex;       //Mutex to handle bubble sort
    pthread_cond_t mainCond;         //Conditional variable bubble
    pthread_mutex_t contMutex;       //Mutex for locking continue cond var
    pthread_cond_t contCond;         //Conditional variable to hold thread while waiting for restart

protected:
    int mSize;                       //Number of chars in ascii array
    char* mAsciiChars;               //Pointer to ascii array
    char* mBubChars;                 //Bubble Sorted Chars
    char* mSelChars;                 //Selection Sort chars
    char* mInsChars;                 //Insertion Sort chars
    int mBubSwapCount;               //Holds number of swaps required for array to sort
    int mSelSwapCount;
    int mInsSwapCount;
    void mCreateCopy(char* asciiChars); //Create copy of AsciiChars
    void mSwap(char &x, char &y);    //Swaps the value of 2 chars
    void mInitCopy();                //Populates sort arrays with randomly generated chars
    void mInitBubbleSort();          //Starts BubbleSort on its own thread
    void mInitSelectionSort();       //Starts SelectionSort on its own thread
    void mInitInsertionSort();       //Starts InsertionSort on its own thread
    void mRestartBubSort();          //Continues Bubble
    void mRestartSelSort();
    void mRestartInsSort();
    pthread_t mtBub;                 //Thread to run BubbleSort Algorithm
    pthread_t mtSel;                 //Thread to run SelectionSort Alg
    pthread_t mtIns;                 //Thread to run InsertionSort Alg

private:
    static void* mBubbleSort(void* arg);       //Executes bubbleSort alg
    static void* mSelectionSort(void* arg);    //Executes SelectionSort
    static void* mInsertionSort(void* arg);    //Executes InsertionSort
};

#endif // ASCIISORT_H
