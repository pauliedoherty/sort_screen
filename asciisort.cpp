#include "asciisort.h"
#include <random>
#include <iostream>


AsciiSort::AsciiSort():
    mSize(10)
{
    mAsciiChars = new char[mSize];
    mBubChars = new char[mSize];
    mSelChars = new char[mSize];
    mInsChars = new char[mSize];
    mBubSwapCount = 0;
    mSelSwapCount = 0;
    mInsSwapCount = 0;
    mbubComplete = false;
    mSelComplete = false;
    totalThreads = 3;
    activeThreads = 3;
    mainMutex = PTHREAD_MUTEX_INITIALIZER;
    sortMutex = PTHREAD_MUTEX_INITIALIZER;
    mainCond = PTHREAD_COND_INITIALIZER;
    sortCond = PTHREAD_COND_INITIALIZER;
    contMutex = PTHREAD_MUTEX_INITIALIZER;
    contCond = PTHREAD_COND_INITIALIZER;
    mainFlag = false;
    sortFlag = false;
    contFlag = true;
}

AsciiSort::AsciiSort(int size):
    mSize(size)
{
    mAsciiChars = new char[mSize];
    mBubChars = new char[mSize];
    mSelChars = new char[mSize];
    mInsChars = new char[mSize];
    mBubSwapCount = 0;
    mSelSwapCount = 0;
    mInsSwapCount = 0;
    mbubComplete = false;
    mSelComplete = false;
    totalThreads = 3;
    activeThreads = 3;
    mainMutex = PTHREAD_MUTEX_INITIALIZER;
    sortMutex = PTHREAD_MUTEX_INITIALIZER;
    mainCond = PTHREAD_COND_INITIALIZER;
    sortCond = PTHREAD_COND_INITIALIZER;
    contMutex = PTHREAD_MUTEX_INITIALIZER;
    contCond = PTHREAD_COND_INITIALIZER;
    mainFlag = false;
    sortFlag = false;
    contFlag = true;
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
    mInitCopy();
}


void AsciiSort::bubbleSort()
{
    mCreateCopy(mBubChars);  //Create copy of chars
    mInitBubbleSort();
}

void AsciiSort::selectionSort()
{
    mCreateCopy(mSelChars);  //Create copy of chars
    mInitSelectionSort();
}

void AsciiSort::insertionSort()
{
    mCreateCopy(mInsChars);  //Create copy of chars
    mInitInsertionSort();
}

void AsciiSort::runAllSorts()
{
    mInitBubbleSort();
    mInitSelectionSort();
    mInitInsertionSort();
}

void AsciiSort::contSorts()
{
    mContBubSort();
    mContSelSort();
    mContInsSort();
}

void AsciiSort::waitForBubSort() const
{
   (void) pthread_join(mtBub, NULL);
}

void AsciiSort::waitForSelSort() const
{
   (void) pthread_join(mtSel, NULL);      //Compare (void) in other wait method
}

void AsciiSort::waitForInsSort() const
{
   (void) pthread_join(mtIns, NULL);
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


int AsciiSort::getBubSwapCount() const
{
    return mBubSwapCount;
}

int AsciiSort::getSelSwapCount() const
{
    return mSelSwapCount;
}

int AsciiSort::getInsSwapCount() const
{
    return mInsSwapCount;
}

void AsciiSort::setBubSwapCount(int count)
{
    mBubSwapCount = count;
}

void AsciiSort::setSelSwapCount(int count)
{
    mSelSwapCount = count;
}

void AsciiSort::setInsSwapCount(int count)
{
    mInsSwapCount = count;
}
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

void AsciiSort::mInitCopy()
{
    mCreateCopy(mBubChars);  //Create copy of chars
    mCreateCopy(mSelChars);
    mCreateCopy(mInsChars);     //Populates mInsChars w/AsciiSort
}

void AsciiSort::mInitBubbleSort()
{
    pthread_create(&mtBub, NULL, mBubbleSort, this);
}

void AsciiSort::mInitSelectionSort()
{
    pthread_create(&mtSel, NULL, mSelectionSort, this);
}

void AsciiSort::mInitInsertionSort()
{
    pthread_create(&mtIns, NULL, mInsertionSort, this);
}

void AsciiSort::mContBubSort()
{
    mBubbleSort(this);
}

void AsciiSort::mContSelSort()
{
    mSelectionSort(this);
}
void AsciiSort::mContInsSort()
{
    mInsertionSort(this);
}

/*Private static member functions that get*
 *executed within their own threads       */
void* AsciiSort::mBubbleSort(void* This)
{
    int size = ((AsciiSort*)This)->mSize;
    char* list = ((AsciiSort*)This)->getBubChars();

    for(int i=1; i<size; i++){
        for(int j=1; j <= size-i; j++){

            if(list[j-1] > list[j]){

                pthread_mutex_lock(&((AsciiSort*)This)->sortMutex);  //lock Thread mutex so list can be safely accessed
                    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex); //locks main Mutex

                    ((AsciiSort*)This)->mSwap(list[j-1], list[j]);
                    ((AsciiSort*)This)->mBubSwapCount++;
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
    //if(((AsciiSort*)This)->totalThreads==0){
        ((AsciiSort*)This)->mainFlag = true;
        pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    //}
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);

    pthread_mutex_lock(&((AsciiSort*)This)->contMutex);

    if(!((AsciiSort*)This)->contFlag){  //Check if user has requested to exit program
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        pthread_exit(NULL);
    }else
        pthread_cond_wait(&((AsciiSort*)This)->contCond, &((AsciiSort*)This)->contMutex);
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        ((AsciiSort*)This)->mContBubSort(); //Restart the cycle



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
             ((AsciiSort*)This)->mSelSwapCount++;

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
        //if(((AsciiSort*)This)->totalThreads==0){
            ((AsciiSort*)This)->mainFlag = true;
            pthread_cond_signal(&((AsciiSort*)This)->mainCond);
        //}
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);
    //pthread_exit(NULL);

    pthread_mutex_lock(&((AsciiSort*)This)->contMutex);

    if(!((AsciiSort*)This)->contFlag){  //Check if user has requested to exit program
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        pthread_exit(NULL);
    }else
        pthread_cond_wait(&((AsciiSort*)This)->contCond, &((AsciiSort*)This)->contMutex);
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        ((AsciiSort*)This)->mContSelSort(); //Restart the cycle

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
                ((AsciiSort*)This)->mInsSwapCount++;
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
    //if(((AsciiSort*)This)->totalThreads==0){
        ((AsciiSort*)This)->mainFlag = true;
        pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    //}
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);
    //pthread_exit(NULL);

    pthread_mutex_lock(&((AsciiSort*)This)->contMutex);

    if(!((AsciiSort*)This)->contFlag){  //Check if user has requested to exit program
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        pthread_exit(NULL);
    }else
        pthread_cond_wait(&((AsciiSort*)This)->contCond, &((AsciiSort*)This)->contMutex);
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        ((AsciiSort*)This)->mContInsSort(); //Restart the cycle

}
