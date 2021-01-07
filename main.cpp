//Real-Time Coursework

#include <fstream>
#include <iostream>
using namespace std;

int main() {
    int sum = 0;
    int x;
    char num[1];
    ifstream inFile;

    inFile.open("input.txt");
    
    if (!inFile) {
        cerr << "Unable to open file datafile.txt";
        exit(1);   // call system to stop
    }

    while (inFile >> x) {
        sum = sum + x;
    }
    
    // prints the contents of the file
    cout << sum << endl;
    cin >> num[0];
    return 0;
}