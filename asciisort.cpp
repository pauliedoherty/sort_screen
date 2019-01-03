#include "asciisort.h"
#include <random>
#include <iostream>

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
    mbubComplete = false;
    mSelComplete = false;
    swapCount = 0;
    loopCount = 0;
    mainFlag = false;
    sortFlag = false;
    totalThreads = 3;
    activeThreads = 3;
}

AsciiSort::AsciiSort(int size):
    mSize(size)
{
    mAsciiChars = new char[size];
    mBubChars = new char[size];
    mSelChars = new char[size];
    mInsChars = new char[size];
    mBubTime = 0.0;
    mSelTime = 0.0;
    mInsTime = 0.0;
    mbubComplete = false;
    mSelComplete = false;
    swapCount = 0;
    loopCount = 0;
    mainFlag = false;
    sortFlag = false;
    totalThreads = 2;
    activeThreads = 2;
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


void AsciiSort::bubbleSort()
{
    mInitBubbleSort();
}

void AsciiSort::selectionSort()
{
    mInitSelectionSort();
}

void AsciiSort::insertionSort()
{
    mInitInsertionSort();
}

void AsciiSort::runAllSorts()
{
    mInitBubbleSort();
    mInitSelectionSort();
    mInitInsertionSort();
}

void AsciiSort::waitForBubSort() const
{
    (void) pthread_join(mtBub, NULL);
}

void AsciiSort::waitForSelSort() const
{
    pthread_join(mtSel, NULL);      //Compare (void) in other wait method
}

void AsciiSort::waitForInsSort() const
{
    pthread_join(mtIns, NULL);
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

bool AsciiSort::getBubStatus() const
{
    return mbubComplete;
}

bool AsciiSort::getSelStatus() const
{
    return mSelComplete;
}


//double AsciiSort::getBubTime() const
//{
//    return mBubTime;
//}

//double AsciiSort::getSelTime() const
//{
//    return mSelTime;
//}

//double AsciiSort::getInsTime() const
//{
//    return mInsTime;
//}

int AsciiSort::getNumElements() const
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

/*Protected member functions*/
void AsciiSort::mCreateCopy(char *asciiChars)
{
    for(int i=0; i<mSize; i++){
        asciiChars[i] = mAsciiChars[i]; //Create copy
    }
}

void AsciiSort::mSwap(char &x, char &y)
{
    char temp = x;
    x = y;
    y = temp;
}

void AsciiSort::mInitBubbleSort()
{
    mCreateCopy(mBubChars);  //Create copy of chars
    pthread_create(&mtBub, NULL, mBubbleSort, this);
}

void AsciiSort::mInitSelectionSort()
{
    mCreateCopy(mSelChars);
    pthread_create(&mtSel, NULL, mSelectionSort, this);
}

void AsciiSort::mInitInsertionSort()
{
    mCreateCopy(mInsChars);     //Populates mInsChars w/AsciiSort
    pthread_create(&mtIns, NULL, mInsertionSort, this);
}

/*Private static member functions that get*
 *executed within their own threads       */
void* AsciiSort::mBubbleSort(void* This)
{
    int size = ((AsciiSort*)This)->mSize;
    char* list = ((AsciiSort*)This)->getBubChars();

    for(int i=1; i<size; i++){
        ((AsciiSort*)This)->loopCount++;
        for(int j=1; j <= size-i; j++){

            if(list[j-1] > list[j]){

                pthread_mutex_lock(&((AsciiSort*)This)->sortMutex);  //lock Thread mutex so list can be safely accessed
                    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex); //locks main Mutex

                    ((AsciiSort*)This)->mSwap(list[j-1], list[j]);
                    ((AsciiSort*)This)->swapCount++;

                    ((AsciiSort*)This)->activeThreads--;
                    if(((AsciiSort*)This)->activeThreads==0){
                        ((AsciiSort*)This)->mainFlag = true;
                        pthread_cond_signal(&((AsciiSort*)This)->mainCond);
                    }

                    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);

                if(!((AsciiSort*)This)->sortFlag){
                    pthread_cond_wait(&((AsciiSort*)This)->sortCond, &((AsciiSort*)This)->sortMutex);
                    ((AsciiSort*)This)->sortFlag = false;
                }
                pthread_mutex_unlock(&((AsciiSort*)This)->sortMutex);
            }
        }
    }
    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);
        ((AsciiSort*)This)->totalThreads--;
        ((AsciiSort*)This)->mainFlag = true;
    pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);
    pthread_exit(NULL);
}

void* AsciiSort::mSelectionSort(void* This)
{
    int size = ((AsciiSort*)This)->mSize;
    char* list = ((AsciiSort*)This)->getSelChars();
    int min;
    for(int i=0; i < size-1; i++){
         min = i;
         for(int j=i+1; j < size; j++){
             if(list[j] < list[min]){                 
                min=j;
             }
         }
         pthread_mutex_lock(&((AsciiSort*)This)->sortMutex);  //lock Thread mutex so list can be safely accessed
             pthread_mutex_lock(&((AsciiSort*)This)->mainMutex); //locks main Mutex

             ((AsciiSort*)This)->mSwap(list[min], list[i]);

             ((AsciiSort*)This)->activeThreads--;
             if(((AsciiSort*)This)->activeThreads==0){
                 ((AsciiSort*)This)->mainFlag = true;
                 pthread_cond_signal(&((AsciiSort*)This)->mainCond);
             }

             pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);

         if(!((AsciiSort*)This)->sortFlag){
             pthread_cond_wait(&((AsciiSort*)This)->sortCond, &((AsciiSort*)This)->sortMutex);
             ((AsciiSort*)This)->sortFlag = false;
         }
         pthread_mutex_unlock(&((AsciiSort*)This)->sortMutex);
    }
    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);
        ((AsciiSort*)This)->totalThreads--;
        ((AsciiSort*)This)->mainFlag = true;
    pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);
    pthread_exit(NULL);
}

void* AsciiSort::mInsertionSort(void* This)
{
    int size = ((AsciiSort*)This)->mSize;
    char* list = ((AsciiSort*)This)->mInsChars;
    for(int i=1; i < size; i++){
        char key = list[i];      //Key holds value to be inserted
        for(int j = i; j > 0 && list[j-1] > key; j--){
            pthread_mutex_lock(&((AsciiSort*)This)->sortMutex);
                pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);

                list[j] = list[j-1];  //Sliding over
                list[j-1] = key;

                ((AsciiSort*)This)->activeThreads--;
                if(((AsciiSort*)This)->activeThreads == 0){
                    ((AsciiSort*)This)->mainFlag = true;
                    pthread_cond_signal(&((AsciiSort*)This)->mainCond);
                }
                pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);

                if(!((AsciiSort*)This)->sortFlag){
                    pthread_cond_wait(&((AsciiSort*)This)->sortCond, &((AsciiSort*)This)->sortMutex);
                    ((AsciiSort*)This)->sortFlag = false;
                }
                pthread_mutex_unlock(&((AsciiSort*)This)->sortMutex);

         }
    }
    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);
        ((AsciiSort*)This)->totalThreads--;
        ((AsciiSort*)This)->mainFlag = true;
    pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);
}
