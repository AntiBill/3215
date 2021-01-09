//Real-Time Coursework

#include <fstream>
#include <iostream>
#include <string> 
#include <map>
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
        
        // maximum value between n1 and n2 is stored in max
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

        /*Calcualte any deadline misses
         *Deadline misses occur if the the number of executions of the task is smaller than (the execution time of the task * execution cycle)
         *This does not take into account whether or not the task has executed on this execution number, since it will already have missed the deadline.
        */
        for(int i = 0; i < numTasks; i++) {
            if (x != 0 && executions[i] < (runtimes[i] * ((int)((x) / periods[i])))) {
                cout << taskNames[i] << " Misses" << endl << x << " ";
            }
        }

        //Execute valid task - using 'importantI' which represents the index of the task chosen to execute this time
        if (taskExecutes) {
            executions[importantI] +=1;
            cout << taskNames[importantI] << " Executes" << endl;
        }
        

        //State if the task has completed or not (the number of executions = the execution time * execution cycle)
        if((runtimes[importantI] * (1+(int)((x+1) / periods[importantI]))) >= executions[importantI] && (executions[importantI] % runtimes[importantI] == 0) && taskExecutes) {
            cout << x << " " << taskNames[importantI] << " Completes" << endl;
        }
    }

    //RMS scheduling is now finished. We need to reset the number of executions for each task to 0 for EDF schedluling:
    for(int i = 0; i < numTasks; i++) {
        executions[i] = 0;
    }
    
    cout << endl << endl << "EDF Schedule" << endl << endl;

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

        /*Calcualte any deadline misses
         *Deadline misses occur if the the number of executions of the task is smaller than (the execution time of the task * execution cycle)
         *This does not take into account whether or not the task has executed on this execution number, since it will already have missed the deadline.
        */
        for(int i = 0; i < numTasks; i++) {
            if (x != 0 && executions[i] < (runtimes[i] * ((int)((x) / periods[i])))) {
                cout << taskNames[i] << " Misses" << endl << x << " ";
            }
        }

        //Execute valid task - using 'importantI' which represents the index of the task chosen to execute this time
        if(taskExecutes) {
            executions[importantI] +=1;
            cout << taskNames[importantI] << " Executes" << endl;
        }
        

        //State if the task has completed or not (the number of executions = the execution time * execution cycle)
        if((runtimes[importantI] * (1+(int)((x+1) / periods[importantI]))) >= executions[importantI] && (executions[importantI] % runtimes[importantI] == 0) && taskExecutes) {
            cout << x << " " << taskNames[importantI] << " Completes" << endl;
        }
    }


    // Enter anything to exit
    cin >> t;
    return 0;
}