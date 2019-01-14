#include "asciisort.h"
#include <random>
#include <iostream>

AsciiSort::AsciiSort()
    : mSize((int)DEFAULT_NUM_CHARS), mMinChar(DEFAULT_MIN), mMaxChar(DEFAULT_MAX),
      mBubSwapCount(0), mSelSwapCount(0), mInsSwapCount(0)
{
    mAsciiChars = new char[mSize];
    mBubChars = new char[mSize];
    mSelChars = new char[mSize];
    mInsChars = new char[mSize];
    mainFlag = false;
    swapFlag = false;
    contFlag = true;
    exitFlag = false;
    mainMutex = PTHREAD_MUTEX_INITIALIZER;
    sortMutex = PTHREAD_MUTEX_INITIALIZER;
    mainCond = PTHREAD_COND_INITIALIZER;
    sortCond = PTHREAD_COND_INITIALIZER;
    contMutex = PTHREAD_MUTEX_INITIALIZER;
    contCond = PTHREAD_COND_INITIALIZER;
}

AsciiSort::AsciiSort(int size, int minChar, int maxChar)
    : mSize((int)size), mMinChar(minChar), mMaxChar(maxChar),
      mBubSwapCount(0), mSelSwapCount(0), mInsSwapCount(0)
{
    mAsciiChars = new char[mSize];
    mBubChars = new char[mSize];
    mSelChars = new char[mSize];
    mInsChars = new char[mSize];
    mainFlag = false;
    swapFlag = false;
    contFlag = true;
    exitFlag = false;
    mainMutex = PTHREAD_MUTEX_INITIALIZER;
    sortMutex = PTHREAD_MUTEX_INITIALIZER;
    mainCond = PTHREAD_COND_INITIALIZER;
    sortCond = PTHREAD_COND_INITIALIZER;
    contMutex = PTHREAD_MUTEX_INITIALIZER;
    contCond = PTHREAD_COND_INITIALIZER;
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
        std::uniform_int_distribution<int> dist(mMinChar, mMaxChar);   /*Upper and lower limits of*/
        mAsciiChars[i] = char(dist(eng));                              /* Ascii chars allowed     */
    }
    mInitCopy();        //copy random chars into sort arrays
}

//TODO - Update is required to allow these methods to be called
//void AsciiSort::bubbleSort()
//{
//    mCreateCopy(mBubChars);  //Create copy of chars
//    mInitBubbleSort();
//}

//void AsciiSort::selectionSort()
//{
//    mCreateCopy(mSelChars);  //Create copy of chars
//    mInitSelectionSort();
//}

//void AsciiSort::insertionSort()
//{
//    mCreateCopy(mInsChars);  //Create copy of chars
//    mInitInsertionSort();
//}

void AsciiSort::initAllSorts()
{
    this->activeSorts = 3;
    this->activeSwaps = 3;
    mInitBubbleSort();
    mInitSelectionSort();
    mInitInsertionSort();
}

void AsciiSort::waitForBubSort() const
{
   pthread_join(mtBub, NULL);
}

void AsciiSort::waitForSelSort() const
{
   pthread_join(mtSel, NULL);
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

void AsciiSort::resetBubSwapCount()
{
    mBubSwapCount = 0;
}

void AsciiSort::resetSelSwapCount()
{
    mSelSwapCount = 0;
}

void AsciiSort::resetInsSwapCount()
{
    mInsSwapCount = 0;
}
int AsciiSort::getNumElements() const
{
    return mSize;
}

void AsciiSort::shutdownRoutine()
{
    this->exitFlag = true;                   //flag to exit app
    this->contFlag = false;                  //flag to not to start another sort iteration

    /*set flags and signal to conditional variables where threads may be waiting*/
    this->mainFlag = true;                      //set flag and signal to main thread
    pthread_cond_signal(&this->mainCond);
    pthread_mutex_unlock(&this->mainMutex);

    pthread_mutex_lock(&this->sortMutex);    //set flag and broadcast to sort cond vars
    this->swapFlag = true;
    pthread_cond_broadcast(&this->sortCond);
    pthread_mutex_unlock(&this->sortMutex);

    pthread_mutex_lock(&this->contMutex);     //Broadcast to continue conditional vars
    pthread_cond_broadcast(&this->contCond);
    pthread_mutex_unlock(&this->contMutex);
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
    pthread_create(&mtBub, NULL, mBubbleSort, this);    //start BubbleSort thread
}

void AsciiSort::mInitSelectionSort()
{
    pthread_create(&mtSel, NULL, mSelectionSort, this); //start SelectionSort thread
}

void AsciiSort::mInitInsertionSort()
{
    pthread_create(&mtIns, NULL, mInsertionSort, this); //start InsertionSort thread
}

void AsciiSort::mRestartBubSort()
{
    mBubbleSort(this);              //continue bubble sort thread
}

void AsciiSort::mRestartSelSort()
{
    mSelectionSort(this);           //continue selection sort thread
}
void AsciiSort::mRestartInsSort()
{
    mInsertionSort(this);           //continue insertion sort thread
}

/*Private static member functions that get*/
/*executed within their own threads       */
void* AsciiSort::mBubbleSort(void* This)
{
    int size = ((AsciiSort*)This)->getNumElements();                           //Declatations
    char* arr = ((AsciiSort*)This)->getBubChars();
    bool* exitFlag = &((AsciiSort*)This)->exitFlag;
    bool* mainFlag = &((AsciiSort*)This)->mainFlag;
    bool* swapFlag = &((AsciiSort*)This)->swapFlag;
    bool* contFlag = &((AsciiSort*)This)->contFlag;

    for(int i=1; i<size; i++){                                      //Start BubSort cycle
        for(int j=1; j <= size-i; j++){

            if(arr[j-1] > arr[j]){                                  //compare arr elements

                if(*exitFlag){                                      //Check if exit flag has been set
                    pthread_exit(NULL);
                }

                pthread_mutex_lock(&((AsciiSort*)This)->sortMutex); //lock sort mutex
                pthread_mutex_lock(&((AsciiSort*)This)->mainMutex); //locks main Mutex

                ((AsciiSort*)This)->mSwap(arr[j-1], arr[j]);        //swaps char positions

                ((AsciiSort*)This)->mBubSwapCount++;                //increment counter
                ((AsciiSort*)This)->activeSwaps--;                  //decrements counter
                                                                    /*when counter reaches zero*/
                if(((AsciiSort*)This)->activeSwaps==0){             /*all sorts have finished  */
                    *mainFlag = true;                               /*swap cycle - tell Main   */
                    pthread_cond_signal(&((AsciiSort*)This)->mainCond);
                }

                pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);   //Unlock main mutex

                /*check if Main raised Flag to start Swap cycle again? if not, wait*/
                if(!*swapFlag){
                    pthread_cond_wait(&((AsciiSort*)This)->sortCond, &((AsciiSort*)This)->sortMutex);
                    if(!*exitFlag){                                 /*if exit has NOT been raised*/
                        *swapFlag = false;                          /*lower sort flag to and     */
                    }                                               /*continue swap cycle        */
                }
                pthread_mutex_unlock(&((AsciiSort*)This)->sortMutex);   //Unlock Sort Mutex
            }
        }
    }

    /*  After Sort cycle has complete.....   */
    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);     //lock Main mutex

    ((AsciiSort*)This)->activeSorts--;                      //Decrement numOf Sort cycles active
    ((AsciiSort*)This)->activeSwaps--;                      //Decrements numOf Swap cycles active
    if(((AsciiSort*)This)->activeSwaps==0){
        *mainFlag = true;                                   //Update Main
        pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    }
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);   //Unlock Main mutex

    pthread_mutex_lock(&((AsciiSort*)This)->contMutex);
    if(!*contFlag){                                         //Check if continue flag lowered
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        pthread_exit(NULL);                                 //Kill thread if so
    }else                                                   //Otherwise wait for new sort cycle
        pthread_cond_wait(&((AsciiSort*)This)->contCond, &((AsciiSort*)This)->contMutex);
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        if(!*contFlag){                                     /*Checks if continue flag was  */
            pthread_exit(NULL);                             /*lowered by Main while waiting*/
        }
        ((AsciiSort*)This)->mRestartBubSort();              //Restart Sort cycle
}

void* AsciiSort::mSelectionSort(void* This)
{
    int size = ((AsciiSort*)This)->getNumElements();        //declarations
    char* arr = ((AsciiSort*)This)->getSelChars();
    bool* exitFlag = &((AsciiSort*)This)->exitFlag;
    bool* mainFlag = &((AsciiSort*)This)->mainFlag;
    bool* swapFlag = &((AsciiSort*)This)->swapFlag;
    bool* contFlag = &((AsciiSort*)This)->contFlag;
    int min;                                                //Holds minimum value found in each iter

    for(int i=0; i < size-1; i++){                          //Start SelecionSort cycle
         min = i;                                           //Set min value
         for(int j=i+1; j < size; j++){           
             if(*exitFlag){                                 //Check if Main raised exit flag
                 pthread_exit(NULL);
             }

             if(arr[j] < arr[min]){                         //Update min value if new min found
                min=j;
             }
         }
         pthread_mutex_lock(&((AsciiSort*)This)->sortMutex);    //lock Sort mutex
         pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);    //locks main Mutex
         ((AsciiSort*)This)->mSwap(arr[min], arr[i]);           //Swap values
         ((AsciiSort*)This)->activeSwaps--;                     //Decrement active swap cycles active
         ((AsciiSort*)This)->mSelSwapCount++;                   //Increment swap count
         if(((AsciiSort*)This)->activeSwaps==0){                /*Check num active swap cycles is 0*/
             *mainFlag = true;                                  /*If so, tell Main flag to continue*/
             pthread_cond_signal(&((AsciiSort*)This)->mainCond);
         }
         pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);  //Unlock main mutex
         if(!*swapFlag){                                        //Wait for Main Mutex to finish
             pthread_cond_wait(&((AsciiSort*)This)->sortCond, &((AsciiSort*)This)->sortMutex);
             if(!*exitFlag){                            /*Check if exit flag is raised while waiting*/
                 *swapFlag = false;                     /*lower sort flag and start new Swap cycle  */
             }
         }
         pthread_mutex_unlock(&((AsciiSort*)This)->sortMutex);  //Unlock Sort mutex
    }
    /*  After Sort cycle has complete.....   */
    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);         //Lock main mutex
    ((AsciiSort*)This)->activeSorts--;                          //Decrements active Sort cycles
    ((AsciiSort*)This)->activeSwaps--;                          //Decrements numOf threads active
    if(((AsciiSort*)This)->activeSwaps==0){                     //Check if all swaps are inactive
        *mainFlag = true;                                       //Tell Main thread to continue if so
        pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    }
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);       //unlock Main mutex
    pthread_mutex_lock(&((AsciiSort*)This)->contMutex);         //lock Continue mutex
    if(!*contFlag){                                 //Check if user has requested to exit program
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        pthread_exit(NULL);                                     //Kill thread if so
    }else
        pthread_cond_wait(&((AsciiSort*)This)->contCond, &((AsciiSort*)This)->contMutex);
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        if(!*contFlag){                                     /*This checks if continue flag lowered*/
            pthread_exit(NULL);                             /*after sort cycle has finished       */
        }
        ((AsciiSort*)This)->mRestartSelSort();              //Restart the cycle
}

void* AsciiSort::mInsertionSort(void* This)
{
    int size = ((AsciiSort*)This)->getNumElements();        //Declarations
    char* arr = ((AsciiSort*)This)->getInsChars();
    bool* exitFlag = &((AsciiSort*)This)->exitFlag;
    bool* mainFlag = &((AsciiSort*)This)->mainFlag;
    bool* swapFlag = &((AsciiSort*)This)->swapFlag;
    bool* contFlag = &((AsciiSort*)This)->contFlag;

    for(int i=1; i < size; i++){                        //Start InsSort cycle
        char key = arr[i];                              //key holds value to be inserted
        if(*exitFlag){                                  //Check exit flag
            pthread_exit(NULL);
        }
        for(int j = i; j > 0 && arr[j-1] > key; j--){

            pthread_mutex_lock(&((AsciiSort*)This)->sortMutex);
            pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);
            arr[j] = arr[j-1];                          //Sliding over
            arr[j-1] = key;                             //Inserting
            ((AsciiSort*)This)->mInsSwapCount++;        //Increment Count
            ((AsciiSort*)This)->activeSwaps--;          //Decrement active Swap cycles
            if(((AsciiSort*)This)->activeSwaps == 0){   //Check if its last alive swap cycle
                *mainFlag = true;                       //Signal to Main if so
                pthread_cond_signal(&((AsciiSort*)This)->mainCond);
            }
            pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);
            if(!*swapFlag){                             //Wait for main to signal swap flag/condVar
                pthread_cond_wait(&((AsciiSort*)This)->sortCond, &((AsciiSort*)This)->sortMutex);

                if(!*exitFlag){                         //Check that exit flag has not been raised
                    *swapFlag = false;                  //Lower swap flag
                }
            }
            pthread_mutex_unlock(&((AsciiSort*)This)->sortMutex);   //Unlock swap mutex
        }
    }
    pthread_mutex_lock(&((AsciiSort*)This)->mainMutex);
    ((AsciiSort*)This)->activeSorts--;
    ((AsciiSort*)This)->activeSwaps--;                          //Decrements numOf threads active
    if(((AsciiSort*)This)->activeSwaps==0){
        *mainFlag = true;                                       //Tell Main thread the good news
        pthread_cond_signal(&((AsciiSort*)This)->mainCond);
    }
    pthread_mutex_unlock(&((AsciiSort*)This)->mainMutex);       //Unlock main mutex
    pthread_mutex_lock(&((AsciiSort*)This)->contMutex);
    if(!*contFlag){                                             //Check if continue flag lowered, if so-
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);   //Unlock cont mutex
        pthread_exit(NULL);                                     //Kill thread
    }else
        pthread_cond_wait(&((AsciiSort*)This)->contCond, &((AsciiSort*)This)->contMutex);
        pthread_mutex_unlock(&((AsciiSort*)This)->contMutex);
        if(!*contFlag){   //This check is place in the event that the user exited the program after sort had finished
            pthread_exit(NULL);
        }
        ((AsciiSort*)This)->mRestartInsSort(); //Restart the cycle
}
