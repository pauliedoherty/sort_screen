#ifndef ASCIISORT_H
#define ASCIISORT_H
#include "pthread.h"


class AsciiSort
{
public:
    AsciiSort();
    AsciiSort(int size);
    ~AsciiSort();
    void generateRand();            //Generates Random Ascii Array
    void bubbleSort();              //Call bubble sort Algorithm
    void selectionSort();           //runs SelectionSort on its own thread
    void insertionSort();           //runs InsertionSort on its own thread
    void runAllSorts();             //runs all 3 sorting algorithms
    void contSorts();
    void waitForBubSort() const;    //Wait for Bubble sort thread to finish
    void waitForSelSort() const;    //Wait for Selection sort thread to finish
    void waitForInsSort() const;    //Wait for Insertion sort thread to finish
    char* getAsciiChars() const;    //ptr to asciiChars
    char* getBubChars() const;      //ptr to bubble chars
    char* getSelChars() const;      //ptr to Selection chars
    char* getInsChars() const;      //ptr to INsertion chars
//    bool getBubStatus() const;      //returns true if bubSort has been executed on object
//    bool getSelStatus() const;      //returns treu if SelSort has been executed on object
    int getTotalThreads() const;
    void setActiveThreads(int numThreads);
    int getBubSwapCount() const;    //Return number of swaps required to sort an array
    int getSelSwapCount() const;
    int getInsSwapCount() const;
    void setBubSwapCount(int count);
    void setSelSwapCount(int count);
    void setInsSwapCount(int count);
    int getNumElements() const;        //Return number of Ascii Chars generated
    void print(char* asciiChars) const;             //Test method
    pthread_mutex_t sortMutex;       //Mutex to handle bubble sort
    pthread_cond_t sortCond;         //Conditional variable bubble
    pthread_mutex_t mainMutex;       //Mutex to handle bubble sort
    pthread_cond_t mainCond;         //Conditional variable bubble
    pthread_mutex_t contMutex;       //Mutex for locking continue cond var
    pthread_cond_t contCond;         //Conditional variable to hold thread while waiting for restart
    bool mainFlag;
    bool sortFlag;
    bool contFlag;
    int activeThreads;
    int totalThreads;

protected:
    int mSize;                       //Number of chars in ascii array
    char* mAsciiChars;               //Pointer to ascii array
    char* mBubChars;                 //Bubble Sorted Chars
    char* mSelChars;                 //Selection Sort chars
    char* mInsChars;                 //Insertion Sort chars
    int mBubSwapCount;               //Holds number of swaps required for array to sort
    int mSelSwapCount;
    int mInsSwapCount;
    bool mbubComplete;               //Tells us if bubSort has finished
    bool mSelComplete;               //Tells us if selection Sort has finished
    void mCreateCopy(char* asciiChars); //Create copy of AsciiChars
    void mSwap(char &x, char &y);    //Swaps the value of 2 chars
    void mInitCopy();
    void mInitBubbleSort();          //runs BubbleSort on its own thread
    void mInitSelectionSort();       //runs SelectionSort on its own thread
    void mInitInsertionSort();       //runs InsertionSort on its own thread
    void mContBubSort();
    void mContSelSort();
    void mContInsSort();
    pthread_t mtBub;                 //Thread to run BubbleSort Algorithm
    pthread_t mtSel;                 //Thread to run SelectionSort Alg
    pthread_t mtIns;                 //Thread to run InsertionSort Alg

private:
    static void* mBubbleSort(void* arg);       //Executes bubbleSort alg
    static void* mSelectionSort(void* arg);    //Executes SelectionSort
    static void* mInsertionSort(void* arg);    //Executes InsertionSort
};

#endif // ASCIISORT_H
