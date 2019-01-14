# sort_screen
Updates:
1. Bug fix - Key entries are now accepted during display initialisation without causing app to freeze
    
2. update - Input parameters are now accepted to control Amount of chars generated, min & max ascii value generated and display delay time.                  

App Description:

This repository holds a console app that displays ascii characters being sorted by the basic algorithms - Bubble Sort, Selection Sort and Insertion Sort.

The aim is to display a coherent animation of randommly generated ascii characters being sorted in real time.
A count of the number of swaps each algorithm has completed is displayed along with the current sort state.

The app uses the posix multithreading library to control each time a swap will be completed so that it can coordinate with the main thread, which is updating the display.

The AsciiSort class produces the randommly array and contains the individual Sort methods which are executed on their own individual threads. Public conditional variables and mutexes also belonging to the AsciiSort class and are used to ensure that the main function will wait until all threads have completed a swap before updating the display. The same conditional variables are then used to broadcast to the sorting threads that they can proceed with another swap cycle.

After sorting is completed, the parent control loop, which the above functionality is embedded in,starts the cycle again with a newly randommly generated array.

The Display class is used to access the Ncurses library which is utilsed for the animation.

Using GCC the app can be compiled and linked on Linux using the following commands:

g++ -c asciisort.h asciisort.cpp display.h display.cpp main.cpp

g++ -o sort_screen asciisort.o display.o main.o -lncurses -lpthread

chmod a+x sort_screen

./sort_screen

Known Bug:
App locks if user enters key during startup animation 

Updates required:
1. Add input parameters:
    -Control update speed
    -Let user specify range of ascii chars to sort
    -Let user specify how many characters to sort (Max 152)
2. Add colour to characters already sorted so progress is easily tracked






