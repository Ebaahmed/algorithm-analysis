// *******************
// Program: dataset_generator.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T22L
// Trimester: 2610
// Member_1: 242UC243BE | EBA MOHAMED ABBAS AHMED | +60136734355
// Member_2: 242UC244SN | MOHAMMAD IEMAN BIN ZAHARI | +60183756140
// Member_3: 242UC243B4 | LAMA M. R. SIAM | +60183942734
// Member_4: 242UC244L3 | HAREIN A/L SATHIAMURTHY | +60143653986
// *******************
// Task Distribution
// Member_1: Dataset Generator - generates n unique random records
// Member_2: Radix Sort Step
// Member_3: Hash Table Search
// Member_4: Hash Table Search Step + Conclusion
// *******************

#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

// Generate a random 5-letter lowercase string
string randomString() {
    string s = "";
    for (int i = 0; i < 5; i++) {
        s += (char)('a' + rand() % 26);
    }
    return s;
}

int main(int argc, char* argv[]) {

    // Check command line argument
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <n>" << endl;
        cout << "Example: dataset_generator 1000" << endl;
        return 1;
    }

    long long n = atoll(argv[1]);

    if (n <= 0) {
        cout << "Error: n must be a positive number." << endl;
        return 1;
    }

    // Seed using group leader student ID: 242UC243BE
    // Mapping: A=1 B=2 C=3 D=4 E=5 F=6 G=7 H=8 I=9 J=0
    //          K=1 L=2 M=3 N=4 O=5 P=6 Q=7 R=8 S=9 T=0
    //          U=1 V=2 W=3 X=4 Y=5 Z=6
    // 242UC243BE -> 2 4 2 1 3 2 4 3 2 5 -> 2421324325
    srand((unsigned int)2421324325U);

    // Integer range: 1,000,000,000 to 9,999,999,999
    long long MIN_VAL = 1000000000LL;
    long long MAX_VAL = 9999999999LL;
    long long RANGE   = MAX_VAL - MIN_VAL + 1;

    // Use a set to ensure uniqueness
    set<long long> used;

    // Output filename: dataset_n.csv
    string filename = "dataset_" + to_string(n) + ".csv";
    ofstream outFile(filename);

    if (!outFile.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return 1;
    }

    cout << "Generating " << n << " unique records..." << endl;

    long long count = 0;
    while (count < n) {
        // Generate a random 10-digit integer in range
        long long num = MIN_VAL + ((long long)rand() * rand()) % RANGE;

        // Ensure uniqueness
        if (used.find(num) == used.end()) {
            used.insert(num);
            string str = randomString();
            outFile << num << "," << str << "\n";
            count++;

            // Progress update every 100,000 records
            if (count % 100000 == 0) {
                cout << count << " records generated..." << endl;
            }
        }
    }

    outFile.close();

    cout << "Done! Output saved to: " << filename << endl;

    return 0;
}