#ifndef ASCIISORT_H
#define ASCIISORT_H


class AsciiSort
{
public:
    AsciiSort();
    AsciiSort(int size);
    ~AsciiSort();
    void generateRand();            //Generates Random Ascii Array
    void runBubbleSort();           //runs BubbleSort on its own thread
    void runSelectionSort();        //runs SelectionSort on its own thread
    void runInsertionSort();        //runs InsertionSort on its own thread
    void runSorts();                //runs all 3 sorting algorithms
    char* getAsciiChars() const;    //ptr to asciiChars
    char* getBubChars() const;      //ptr to bubble chars
    char* getSelChars() const;      //ptr to Selection chars
    char* getInsChars() const;      //ptr to INsertion chars
    double getBubTime() const;      //Return time taken to Bubble Sort
    double getSelTime() const;      //Return time taken to Selection Sort
    double getInsTime() const;      //Return time taken to Insertion Sort
    int getElements() const;        //Return number of Ascii Chars generated
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
    void* mBubbleSort(void* arg);             //Executes bubbleSort alg
    void mSelectionSort();          //Executes SelectionSort alg
    void mInsertionSort();          //Executes InsertionSort alg
};

#endif // ASCIISORT_H
