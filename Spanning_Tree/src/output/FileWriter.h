#ifndef FILEWRITER_H_
#define FILEWRITER_H_

#include <iostream>
#include <fstream>

// This class can be updated with:
// Producer–consumer problem

class FileWriter {
    public:
        FileWriter(void);




    private:
        int index;
        double time[10000];
        double networkDelay[10000];
        double cyptoDelay[10000];
        double osDelay[10000];

        int numOfTransactionsCompleted[10000];
        int numOfTransactionsStarted[10000];
        int numOfTransactionsForwarded[10000];




};


#endif
