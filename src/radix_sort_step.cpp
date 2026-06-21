// *********************************************************
// Program: dataset_generator.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T22L
// Trimester: 2610
// Member_1: 242UC243BE | EBA MOHAMED ABBAS AHMED  |eba.mohamed.abbas@student.mmu.edu.my| +60136734355
// Member_2: 242UC244SN | MOHAMMAD IEMAN BIN ZAHARI |MOHAMMAD.IEMAN.ZAHARI@student.mmu.edu.my| +60183756140
// Member_3: 242UC244L3 | HAREIN A/L SATHIAMURTHY  |HAREIN.SATHIAMURTHY@student.mmu.edu.my| +60143653986
// Member_4: 242UC243B4 | LAMA M. R. SIAM     |LAMA.M.R@student.mmu.edu.my| +60183942734

// *********************************************************
// Task Distribution
// Member_1: Dataset Generator — generates n unique random (integer, string) records
// Member_2: Radix Sort Step
// Member_3: Hash Table Search
// Member_4: Hash Table Search Step + Conclusion
// *********************************************************

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;


struct Record {
    long long integer_val;
    string    str_val;
};


vector<Record> readCSV(const string& filename, int start_row, int end_row) {
    vector<Record> data;
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: cannot open file " << filename << endl;
        return data;
    }

    string line;
    int current_row = 0;
    while (getline(fin, line)) {
        ++current_row;
        if (current_row < start_row) continue;
        if (current_row > end_row)   break;

        // trim trailing whitespace / carriage return
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' '))
            line.pop_back();
        if (line.empty()) continue;

        size_t comma = line.find(',');
        if (comma == string::npos) continue;

        Record rec;
        rec.integer_val = stoll(line.substr(0, comma));
        rec.str_val     = line.substr(comma + 1);
        data.push_back(rec);
    }
    fin.close();
    return data;
}


void writeState(ofstream& fout, const vector<Record>& arr, const string& label) {
    fout << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        fout << arr[i].integer_val << "/" << arr[i].str_val;
        if (i + 1 < arr.size()) fout << ", ";
    }
    fout << "] " << label << "\n";
}

// -------------------------------------------------------
// Counting sort on a specific decimal digit position.
// digit_pos: 1 = ones place, 2 = tens place, etc.
// -------------------------------------------------------
void countingSortByDigit(vector<Record>& arr, long long digit_pos) {
    const int BASE = 10;
    int n = (int)arr.size();

    vector<Record> output(n);
    int count[BASE] = {0};

    // Divisor to extract the digit
    long long divisor = 1;
    for (int i = 1; i < digit_pos; ++i) divisor *= 10;

    // Count occurrences
    for (int i = 0; i < n; ++i) {
        int digit = (int)((arr[i].integer_val / divisor) % BASE);
        count[digit]++;
    }

    // Cumulative count
    for (int i = 1; i < BASE; ++i)
        count[i] += count[i - 1];

    // Build output (traverse right-to-left for stability)
    for (int i = n - 1; i >= 0; --i) {
        int digit = (int)((arr[i].integer_val / divisor) % BASE);
        output[--count[digit]] = arr[i];
    }

    arr = output;
}

// -------------------------------------------------------
// Radix Sort Step:
//   - Sorts by integer field using LSD radix sort
//   - Processes from the rightmost digit (d=10 down to d=1
//     for 10-digit numbers)
//   - Writes array state after every digit pass
// -------------------------------------------------------
void radixSortStep(vector<Record>& arr, ofstream& fout) {
    const int NUM_DIGITS = 10;   // integers have 10 digits (1,000,000,000 – 9,999,999,999)

    // Write original state
    writeState(fout, arr, "original");

    // Process from rightmost digit (position 10) to leftmost (position 1)
    for (int d = NUM_DIGITS; d >= 1; --d) {
        int digit_pos = NUM_DIGITS - d + 1;   // digit_pos 1 = ones, 2 = tens, ...
        countingSortByDigit(arr, digit_pos);
        writeState(fout, arr, "d=" + to_string(d));
    }
}

// -------------------------------------------------------
// Build output filename from inputs
// e.g.  dataset_1000_radix_sorted_step_1_7.txt
// -------------------------------------------------------
string buildOutputFilename(const string& csv_filename, int start_row, int end_row) {
    // Extract size token from filename (e.g. "1000" from "dataset_1000.csv")
    string base = csv_filename;
    size_t slash = base.find_last_of("/\\");
    if (slash != string::npos) base = base.substr(slash + 1);
    size_t dot = base.rfind('.');
    if (dot != string::npos) base = base.substr(0, dot);   // strip .csv

    return base + "_radix_sorted_step_" +
           to_string(start_row) + "_" + to_string(end_row) + ".txt";
}


int main() {
    
    // --- dataset filename ---
    //const string DATASET = "dataset_100.csv";
    const string DATASET = "dataset_1000.csv";
    //const string DATASET = "dataset_10000.csv";

    // --- row range (1-based, inclusive) ---
    //const int START_ROW = 1;  const int END_ROW = 100;
    const int START_ROW = 1;  const int END_ROW = 7;
    //const int START_ROW = 1;  const int END_ROW = 1000;

    // ==============================================================

    // Read data
    vector<Record> arr = readCSV(DATASET, START_ROW, END_ROW);
    if (arr.empty()) {
        cerr << "No data read. Check filename and row range." << endl;
        return 1;
    }

    // Open output file
    string outfile = buildOutputFilename(DATASET, START_ROW, END_ROW);
    ofstream fout(outfile);
    if (!fout.is_open()) {
        cerr << "Error: cannot open output file " << outfile << endl;
        return 1;
    }

    // Run radix sort step
    radixSortStep(arr, fout);

    fout.close();
    cout << "Radix sort step output written to: " << outfile << endl;
    return 0;
}
