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
    void waitForBubSort() const;    //Wait for Bubble sort thread to finish
    void waitForSelSort() const;    //Wait for Selection sort thread to finish
    void waitForInsSort() const;    //Wait for Insertion sort thread to finish
    char* getAsciiChars() const;    //ptr to asciiChars
    char* getBubChars() const;      //ptr to bubble chars
    char* getSelChars() const;      //ptr to Selection chars
    char* getInsChars() const;      //ptr to INsertion chars
    double getBubTime() const;      //Return time taken to Bubble Sort
    double getSelTime() const;      //Return time taken to Selection Sort
    double getInsTime() const;      //Return time taken to Insertion Sort
    int getNumElements() const;        //Return number of Ascii Chars generated
    void print(char* asciiChars) const;             //Test method

protected:
    int mSize;                      //Number of chars in ascii array
    char* mAsciiChars;              //Pointer to ascii array
    char* mBubChars;                //Bubble Sorted Chars
    char* mSelChars;                //Selection Sort chars
    char* mInsChars;                //Insertion Sort chars
    double mBubTime;                //Holds time for BubSort to run
    double mSelTime;                //Holds time for SelSort to run
    double mInsTime;                //Holds time for InsSort to run
    void mCreateCopy(char* asciiChars); //Create copy of AsciiChars
    void mSwap(char &x, char &y);   //Swaps the value of 2 chars
    void mInitBubbleSort();          //runs BubbleSort on its own thread
    void mInitSelectionSort();       //runs SelectionSort on its own thread
    void mInitInsertionSort();       //runs InsertionSort on its own thread
    pthread_t mtBub;                //Thread to run BubbleSort Algorithm
    pthread_t mtSel;                //Thread to run SelectionSort Alg
    pthread_t mtIns;                //Thread to run InsertionSort Alg

private:
    static void* mBubbleSort(void* arg);       //Executes bubbleSort alg
    static void* mSelectionSort(void* arg);    //Executes SelectionSort
    static void* mInsertionSort(void* arg);    //Executes InsertionSort
};

#endif // ASCIISORT_H
