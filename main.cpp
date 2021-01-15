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

    cout << "Plotting RMS Schedule..." << endl;

    //Open a file to store the results of the RMS data
    ofstream rmsFile;
    rmsFile.open("RMS.txt");

    //These arrays will store the co-ordinates of the rectangles to draw when graphing the results
    int** listOfLists = new int*[numTasks]; //To hold all data about when each task executes
    int** listOfComp = new int*[numTasks];  //To hold all data about when each task completes
    int** listOfMiss = new int*[numTasks];  //To hold all data about when each task misses
    
    for(int i = 0; i < numTasks; i++) {
        listOfLists[i] = new int[LCM];      //Each list is for a different task. In the worst case, 1 task runs every single time
        listOfComp[i] = new int[LCM];       //interval of the schedule, hence the size is LCM, the total length of the whole schedule.
        listOfMiss[i] = new int[LCM];

        for(int x = 0; x < LCM; x++) {
            listOfLists[i][x] = -1;
            listOfComp[i][x] = -1;
            listOfMiss[i][x] = -1;
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
        rmsFile << x << " ";

        /*Calcualte any deadline misses
         *Deadline misses occur if the the number of executions of the task is smaller than (the execution time of the task * execution cycle)
         *This does not take into account whether or not the task has executed on this execution number, since it will already have missed the deadline.
        */
        for(int i = 0; i < numTasks; i++) {
            if (x != 0 && executions[i] < (runtimes[i] * ((int)((x) / periods[i])))) {
                rmsFile << taskNames[i] << " Misses" << endl << x << " ";
                listOfMiss[i][x] = x;
            }
        }

        //Execute valid task - using 'importantI' which represents the index of the task chosen to execute this time
        if (taskExecutes) {
            executions[importantI] +=1;
            rmsFile << taskNames[importantI] << " Executes" << endl;
            listOfLists[importantI][x] = x;
        }
        

        //State if the task has completed or not (the number of executions = the execution time * execution cycle)
        if((runtimes[importantI] * (1+(int)((x+1) / periods[importantI]))) >= executions[importantI] && (executions[importantI] % runtimes[importantI] == 0) && taskExecutes) {
            rmsFile << x << " " << taskNames[importantI] << " Completes" << endl;
            listOfComp[importantI][x] = x;
        }
    }

    rmsFile.close();

    //The next 50 lines or so (any line beginning 'p') is passing commands to GNUPlot...
    //which only works if GNUPlot is added to your PATH environment variable for Windows.

    gnuplot p;
    p("set term pngcairo dashed size 800,400");
    p("set output 'RMS.png'");
    p("set style fill solid");
    p("unset ytics");

    string yAxisString = "set ytics(";

    for(int i = 0; i < numTasks; i++) {
        if(i == (numTasks - 1)) {
            yAxisString += ("'" + taskNames[i] + "' " + to_string(i+0.5) + ")");
        } else {
            yAxisString += ("'" + taskNames[i] + "' " + to_string(i+0.5) + ", ");
        }
        
    }

    p(yAxisString);
    p("unset key");
    p("set xrange [0:" + to_string(LCM) + "]");
    p("set yrange [0:" + to_string(numTasks) + "]");
    p("set xlabel 't'");

    //Drawing the graph

    counter = 1;

    for(int i = 0; i < numTasks; i++) {
        for(int x = 0; x < LCM; x++) {

            //Draws a box to show the task executing
            if(listOfLists[i][x] == x) {
                //See report for an explanation of this command
                p("set object " + to_string(counter) + " rectangle from " + to_string(x) + "," + to_string(i+0.3) + " to " + to_string(x+1) + "," + to_string(i+0.7) + " fc rgb 'blue' lw 2");
                counter++;
            }
        }
    }

    for(int i = 0; i < numTasks; i++) {
        for(int x = 0; x < LCM; x++) {
            //Draws a green circle to show the task completing
            if(listOfComp[i][x] == x) {
                //See report for an explanation of this command
                p("set object " + to_string(counter) + " circle at " + to_string(x+1) + "," + to_string(i+0.6) + " radius char 0.2 fillstyle empty border lc rgb 'green' lw 2");
                counter++;
            }

            //Draws a red circle to show the task missing
            if(listOfMiss[i][x] == x) {
                //See report for an explanation of this command
                p("set object " + to_string(counter) + " circle at " + to_string(x+1) + "," + to_string(i+0.4) + " radius char 0.2 fillstyle empty border lc rgb 'red' lw 2");
                counter++;
            }
        }
    }

    p("plot 1 w l lt 2 lc rgb 'white'"); //We must plot SOMETHING to draw the graph, that's just how GNUPlot works.

    //RMS scheduling is now finished. We need to reset the number of executions for each task to 0 for EDF schedluling:
    for(int i = 0; i < numTasks; i++) {
        executions[i] = 0;
    }
    
    cout << endl << "Plotting EDF Schedule..." << endl << endl;

    //Open a file to store the results of the RMS data
    ofstream edfFile;
    edfFile.open("EDF.txt");

    //We also need to reset the values in the lists of lists...
    for(int i = 0; i < numTasks; i++) {
        for(int x = 0; x < LCM; x++) {
            listOfLists[i][x] = -1;
            listOfComp[i][x] = -1;
            listOfMiss[i][x] = -1;
        }
    }

    //Permorms EDF scheduling on the input tasks.
    for(int x = 0; x < LCM; x++) {

        //The task with the smallest deadline (that still has a runtime) is always scheduled. Get the task with the smallest deadline.
        int lowestDeadline = 9999;
        int importantI = 0;
        bool taskExecutes = false;
        for(int i = 0; i < numTasks; i++) {
            int deadline = ((periods[i] * (1+(int)((x) / periods[i]))));
            //if we are deadline missing, the deadline should be earlier
            if (x != 0 && executions[i] < (runtimes[i] * ((int)((x) / periods[i])))) {
                deadline = ((int)(executions[i] / runtimes[i])+1) * periods[i];
            }
            if (deadline < lowestDeadline && executions[i] < (runtimes[i] * (1+(int)((x) / periods[i])))) {
                lowestDeadline = deadline;
                importantI = i;
                taskExecutes = true;
            }
        }

        //Print the current execution number
        edfFile << x << " ";

        /*Calcualte any deadline misses
         *Deadline misses occur if the the number of executions of the task is smaller than (the execution time of the task * execution cycle)
         *This does not take into account whether or not the task has executed on this execution number, since it will already have missed the deadline.
        */
        for(int i = 0; i < numTasks; i++) {
            if (x != 0 && executions[i] < (runtimes[i] * ((int)((x) / periods[i])))) {
                edfFile << taskNames[i] << " Misses" << endl << x << " ";
                listOfMiss[i][x] = x;
            }
        }

        //Execute valid task - using 'importantI' which represents the index of the task chosen to execute this time
        if(taskExecutes) {
            executions[importantI] +=1;
            edfFile << taskNames[importantI] << " Executes" << endl;
            listOfLists[importantI][x] = x;
        }
        

        //State if the task has completed or not (the number of executions = the execution time * execution cycle)
        if((runtimes[importantI] * (1+(int)((x+1) / periods[importantI]))) >= executions[importantI] && (executions[importantI] % runtimes[importantI] == 0) && taskExecutes) {
            edfFile << x << " " << taskNames[importantI] << " Completes" << endl;
            listOfComp[importantI][x] = x;
        }
    }

    inFile.close();
    edfFile.close();

    counter = 0;

    p("set term pngcairo dashed size 800,400");
    p("set output 'EDF.png'");
    p("set style fill solid");
    p("unset ytics");

    yAxisString = "set ytics(";

    for(int i = 0; i < numTasks; i++) {
        if(i == (numTasks - 1)) {
            yAxisString += ("'" + taskNames[i] + "' " + to_string(i+0.5+numTasks+1) + ")");
        } else {
            yAxisString += ("'" + taskNames[i] + "' " + to_string(i+0.5+numTasks+1) + ", ");
        }
        
    }

    p(yAxisString);
    p("unset key");
    p("set xrange [0:" + to_string(LCM) + "]");
    p("set yrange ["+ to_string(numTasks+1) + ":" + to_string(2*numTasks+1) + "]");
    p("set xlabel 't'");
    
    //Drawing the graph

    counter = 1;

    for(int i = 0; i < numTasks; i++) {
        for(int x = 0; x < LCM; x++) {

            //Draws a box to show the task executing
            if(listOfLists[i][x] == x) {
                //See report for an explanation of this command
                p("set object " + to_string(counter) + " rectangle from " + to_string(x) + "," + to_string(i+0.3+numTasks+1) + " to " + to_string(x+1) + "," + to_string(i+0.7+numTasks+1) + " fc rgb 'blue' lw 2");
                counter++;
            }
        }
    }

    for(int i = 0; i < numTasks; i++) {
        for(int x = 0; x < LCM; x++) {
            //Draws a green circle to show the task completing
            if(listOfComp[i][x] == x) {
                //See report for an explanation of this command
                p("set object " + to_string(counter) + " circle at " + to_string(x+1) + "," + to_string(i+0.6+numTasks+1) + " radius char 0.2 fillstyle empty border lc rgb 'green' lw 2");
                counter++;
            }

            //Draws a red circle to show the task missing
            if(listOfMiss[i][x] == x) {
                //See report for an explanation of this command
                p("set object " + to_string(counter) + " circle at " + to_string(x+1) + "," + to_string(i+0.4+numTasks+1) + " radius char 0.2 fillstyle empty border lc rgb 'red' lw 2");
                counter++;
            }
        }
    }

    p("plot 1 w l lt 2 lc rgb 'white'"); //We must plot SOMETHING to draw the graph, that's just how GNUPlot works.

    // Enter anything to exit
    cout << "Program finished, input any string to exit" << endl;
    std::cin >> t;
    return 0;
}