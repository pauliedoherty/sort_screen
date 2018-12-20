#include "asciisort.h"
#include <random>
#include <iostream>
//#include <pthread.h>

#define MIN 33;     //lower limit of Ascii char allowed
#define MAX 126;    //Upper limit of Ascii char allowed

AsciiSort::AsciiSort():
    mSize(10)
{
    mAsciiChars = new char[mSize];
    mBubChars = new char[mSize];
    mSelChars = new char[mSize];
    mInsChars = new char[mSize];
    mBubTime = 0.0;
    mSelTime = 0.0;
    mInsTime = 0.0;
}

AsciiSort::AsciiSort(int size):
    mSize(size)
{
    mAsciiChars = new char[size];
    mBubChars = new char[size];
    mSelChars = new char[size];
    mInsChars = new char[size];
}

AsciiSort::~AsciiSort()
{
    delete[] mAsciiChars;
    delete[] mBubChars;
    delete[] mSelChars;
    delete[] mInsChars;
}

void AsciiSort::generateRand()
/*Random Number generator Random numbers*/
{
    std::mt19937 eng(static_cast<unsigned long>(time(nullptr)));  //Marsenne Twister Engine Seed
    for(int i=0; i < mSize; i++){
        std::uniform_int_distribution<int> dist(33, 126);   /*Upper and lower limits of Ascii chars*/
        mAsciiChars[i] = char(dist(eng));                   /*Allowed                              */
    }
}

void AsciiSort::mSwap(char &x, char &y)
{
    char temp = x;
    x = y;
    y = temp;
}

void AsciiSort::runBubbleSort()
{
    mCreateCopy(mBubChars);  //Create copy of chars
//    pthread_t tBub;
//    pthread_attr_t attr;
//    pthread_attr_init(&attr);
    pthread_create(&mtBub, NULL, mBubbleSort, this);
}

void AsciiSort::waitForBubSort() const
{
    (void) pthread_join(mtBub, NULL);
}

void* AsciiSort::mBubbleSort(void* This)
{
    int size = ((AsciiSort*)This)->mSize;
    char* list = ((AsciiSort*)This)->getBubChars();
    for(int i=1; i<size; i++){
        for(int j=1; j <= size-i; j++){
            if(list[j-1] > list[j]){
                ((AsciiSort*)This)->mSwap(list[j-1], list[j]);
            }
        }
    }
    pthread_exit(NULL);
}

char* AsciiSort::getAsciiChars() const
{
    return mAsciiChars;
}

char* AsciiSort::getBubChars() const
{
    return mBubChars;
}

char* AsciiSort::getSelChars() const
{
    return mSelChars;
}

char* AsciiSort::getInsChars() const
{
    return mInsChars;
}

double AsciiSort::getBubTime() const
{
    return mBubTime;
}

double AsciiSort::getSelTime() const
{
    return mSelTime;
}

double AsciiSort::getInsTime() const
{
    return mInsTime;
}

int AsciiSort::getElements() const
{
    return mSize;
}

void AsciiSort::print(char* asciiChars) const
{
    for(int i=0; i<mSize; i++){
        std::cout << asciiChars[i] << " ";
    }
    std::cout << std::endl;
}

void AsciiSort::mCreateCopy(char *asciiChars)
{
    for(int i=0; i<mSize; i++){
        asciiChars[i] = mAsciiChars[i]; //Create copy
    }
}
