//Real-Time Coursework

#include <fstream>
#include <iostream>
#include <string> 
#include <map>
#include <stdio.h>
#include "gnuplot.h"

using namespace std;


int main() {
    int counter = 0;
    int numTasks = 0;
    char task[5];
    string t;

    ifstream inFile;

    inFile.open("input.txt");
    
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop with an error
    }

    //Read the first 2 strings of the file. Ignore the string 'NumTasks' and convert the number of tasks to an integer
    while (counter < 2 && inFile >> t) {
        if(counter == 1) {
            numTasks = stoi(t);
        }
        counter++;
    }

    //Define 3 arrays on the heap, all big enough to hold the task data of every task
    string* taskNames = new string[numTasks];
    int* runtimes = new int[numTasks];
    int* periods = new int[numTasks];
    int* executions = new int[numTasks];

    //Read in every task and add its data to the appropriate array
    for(int i = 0; i < numTasks; i++) {
        counter = 0;
        executions[i] = 0;
        while (counter < 3 && inFile >> t) {
            if(counter == 0) {
                taskNames[i] = t;
            } else if (counter == 1) {
                runtimes[i] = stoi(t);
            } else {
                periods[i] = stoi(t);
            }
            counter++;
        }
    }

    //The time frame of the entire graph should be the lowest common multiple of the periods. Calculate this value:
    int LCM = 1;
    for(int i = 0; i < numTasks; i++) {
        int n1 = periods[i];
        int n2 = LCM;
        int max;
        
        //The maximum value between n1 and n2 is stored in max
        max = (n1 > n2) ? n1 : n2;

        do {
            if (max % n1 == 0 && max % n2 == 0) {
                LCM = max;
                break;
            }
            else
                ++max;
        } while (true);
    }

    cout << "RMS Schedule" << endl << endl;

    //Open a file to store the results of the RMS data
    ofstream rmsFile;
    rmsFile.open("RMS.txt");

    //These arrays will store the co-ordinates of the rectangles to draw when graphing the results
    int** listOfLists = new int*[numTasks];
    
    for(int i = 0; i < numTasks; i++) {
        listOfLists[i] = new int[LCM];      //Each list is for a different task. In the worst case, 1 task runs every time, hence the size

        for(int x = 0; x < LCM; x++) {
            listOfLists[i][x] = -1;
        }
    }

    //Perform RMS scheduling on the input tasks
    for(int x = 0; x < LCM; x++) {

        //The task with the smallest period (that still has a runtime) is always scheduled. Get the task with the smallest period.
        int lowestPeriod = 9999;
        int importantI = 0;
        bool taskExecutes = false;
        for(int i = 0; i < numTasks; i++) {
            if (periods[i] < lowestPeriod && executions[i] < (runtimes[i] * (1+(int)((x) / periods[i])))) {
                lowestPeriod = periods[i];
                importantI = i;
                taskExecutes = true;
            }
        }

        //Print the current execution number
        cout << x << " ";
        rmsFile << x << " ";

        /*Calcualte any deadline misses
         *Deadline misses occur if the the number of executions of the task is smaller than (the execution time of the task * execution cycle)
         *This does not take into account whether or not the task has executed on this execution number, since it will already have missed the deadline.
        */
        for(int i = 0; i < numTasks; i++) {
            if (x != 0 && executions[i] < (runtimes[i] * ((int)((x) / periods[i])))) {
                cout << taskNames[i] << " Misses" << endl << x << " ";
                rmsFile << taskNames[i] << " Misses" << endl << x << " ";
            }
        }

        //Execute valid task - using 'importantI' which represents the index of the task chosen to execute this time
        if (taskExecutes) {
            executions[importantI] +=1;
            cout << taskNames[importantI] << " Executes" << endl;
            rmsFile << taskNames[importantI] << " Executes" << endl;
            listOfLists[importantI][x] = x;
        }
        

        //State if the task has completed or not (the number of executions = the execution time * execution cycle)
        if((runtimes[importantI] * (1+(int)((x+1) / periods[importantI]))) >= executions[importantI] && (executions[importantI] % runtimes[importantI] == 0) && taskExecutes) {
            cout << x << " " << taskNames[importantI] << " Completes" << endl;
            rmsFile << x << " " << taskNames[importantI] << " Completes" << endl;
        }
    }

    rmsFile.close();

    //RMS scheduling is now finished. We need to reset the number of executions for each task to 0 for EDF schedluling:
    for(int i = 0; i < numTasks; i++) {
        executions[i] = 0;
    }
    
    cout << endl << endl << "EDF Schedule" << endl << endl;

    //Open a file to store the results of the RMS data
    ofstream edfFile;
    edfFile.open("EDF.txt");

    //Permorms EDF scheduling on the input tasks.
    for(int x = 0; x < LCM; x++) {

        //The task with the smallest deadline (that still has a runtime) is always scheduled. Get the task with the smallest deadline.
        int lowestDeadline = 9999;
        int importantI = 0;
        bool taskExecutes = false;
        for(int i = 0; i < numTasks; i++) {
            int deadline = ((periods[i] * (1+(int)((x) / periods[i]))) - x) - (runtimes[i] - executions[i]);
            if (deadline < lowestDeadline && executions[i] < (runtimes[i] * (1+(int)((x) / periods[i])))) {
                lowestDeadline = deadline;
                importantI = i;
                taskExecutes = true;
            }
        }

        //Print the current execution number
        cout << x << " ";
        edfFile << x << " ";

        /*Calcualte any deadline misses
         *Deadline misses occur if the the number of executions of the task is smaller than (the execution time of the task * execution cycle)
         *This does not take into account whether or not the task has executed on this execution number, since it will already have missed the deadline.
        */
        for(int i = 0; i < numTasks; i++) {
            if (x != 0 && executions[i] < (runtimes[i] * ((int)((x) / periods[i])))) {
                cout << taskNames[i] << " Misses" << endl << x << " ";
                edfFile << taskNames[i] << " Misses" << endl << x << " ";
            }
        }

        //Execute valid task - using 'importantI' which represents the index of the task chosen to execute this time
        if(taskExecutes) {
            executions[importantI] +=1;
            cout << taskNames[importantI] << " Executes" << endl;
            edfFile << taskNames[importantI] << " Executes" << endl;
        }
        

        //State if the task has completed or not (the number of executions = the execution time * execution cycle)
        if((runtimes[importantI] * (1+(int)((x+1) / periods[importantI]))) >= executions[importantI] && (executions[importantI] % runtimes[importantI] == 0) && taskExecutes) {
            cout << x << " " << taskNames[importantI] << " Completes" << endl;
            edfFile << x << " " << taskNames[importantI] << " Completes" << endl;
        }
    }

    inFile.close();
    edfFile.close();

    gnuplot p;
    p("set term pngcairo dashed size 800,400");
    p("set output 'boxes.eps'");
    p("set style fill solid");
    p("unset ytics");
    p("set ytics('Processor1' 1.5,'Processor2' 0.5)");
    p("unset key");
    p("set xrange [-1:" + to_string(LCM) + "]");
    p("set yrange [0:" + to_string(numTasks) + "]");
    p("set xlabel 't'");
    
    //Drawing the boxes

    std::cin >> t;

    counter = 1;

    for(int i = 0; i < numTasks; i++) {
        for(int x = 0; x < LCM; x++) {
            if(listOfLists[i][x] == x) {
                string toPlot = "set object " + to_string(counter) + " rectangle from " + to_string(x) + "," + to_string(i) + " to " + to_string(x+1) + "," + to_string(i+0.7) + " fc rgb 'gold'";
                cout << toPlot << endl;
                p("set object " + to_string(counter) + " rectangle from " + to_string(x) + "," + to_string(i) + " to " + to_string(x+1) + "," + to_string(i+0.7) + " fc rgb 'gold'");
                counter++;
            }
        }
    }

    // Enter anything to exit
    std::cin >> t;
    return 0;
}