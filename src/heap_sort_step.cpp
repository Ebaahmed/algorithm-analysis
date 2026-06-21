// *********************************************************
// Program: heap_sort_step.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T22L
// Trimester: 2610
// Member_1: 242UC243BE | EBA MOHAMED ABBAS AHMED    | +60136734355
// Member_2: 242UC244SN | MOHAMMAD IEMAN BIN ZAHARI  | +60183756140
// Member_3: 242UC244L3 | HAREIN A/L SATHIAMURTHY    | +60143653986
// Member_4: 242UC243B4 | LAMA M. R. SIAM            | +60183942734
// *********************************************************
// Task Distribution
// Member_1: Dataset Generator + Radix Sort
// Member_2: Radix Sort Step + Heap Sort
// Member_3: Hash Table Search + Heap Sort Step
// Member_4: Hash Table Search Step + Conclusion
// *********************************************************

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// -------------------------------------------------------
// Data record: a 10-digit integer key paired with a 5-letter string.
// Sorting is performed on integer_val (the integer field).
// -------------------------------------------------------
struct Record {
    long long integer_val;
    string    str_val;
};


vector<Record> readCSVRange(const string& filename, int start_row, int end_row) {
    vector<Record> data;
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: cannot open file " << filename << endl;
        return data;
    }

    string line;
    int row = 0;
    while (getline(fin, line)) {
        ++row;
        if (row < start_row) continue;   // skip until we reach start
        if (row > end_row)   break;       // stop once past end

        // Trim trailing whitespace / carriage returns
        while (!line.empty() &&
              (line.back() == '\r' || line.back() == '\n' || line.back() == ' '))
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


void heapify(vector<Record>& arr, int heap_size, int i) {
    while (true) {
        int largest = i;
        int left    = 2 * i + 1;
        int right   = 2 * i + 2;

        if (left  < heap_size && arr[left].integer_val  > arr[largest].integer_val) largest = left;
        if (right < heap_size && arr[right].integer_val > arr[largest].integer_val) largest = right;

        if (largest == i) break;   // heap property satisfied
        swap(arr[i], arr[largest]);
        i = largest;               // continue sifting down
    }
}

// -------------------------------------------------------
// Format and append one array snapshot, e.g.:
// [9686532590/ajgug, 8138274075/cmdkc, ...] initial
// -------------------------------------------------------
string snapshot(const vector<Record>& arr, const string& label) {
    ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        oss << arr[i].integer_val << "/" << arr[i].str_val;
        if (i + 1 < arr.size()) oss << ", ";
    }
    oss << "] " << label;
    return oss.str();
}


void heapSortSteps(vector<Record>& arr, vector<string>& steps) {
    int n = (int)arr.size();
    if (n == 0) return;

    // Phase 1: build max-heap
    for (int i = n / 2 - 1; i >= 0; --i)
        heapify(arr, n, i);

    // The "initial" line is the heap-built state (NOT the input order)
    steps.push_back(snapshot(arr, "initial"));

    // Phase 2: extract max one at a time, recording each step
    for (int i = n - 1; i > 0; --i) {
        swap(arr[0], arr[i]);          // move current max to end
        heapify(arr, i, 0);            // restore heap on reduced range
        steps.push_back(snapshot(arr, "i = " + to_string(i)));
    }
}


string extractSizeToken(const string& csv_filename) {
    string base = csv_filename;
    size_t slash = base.find_last_of("/\\");
    if (slash != string::npos) base = base.substr(slash + 1);
    size_t dot = base.rfind('.');
    if (dot != string::npos) base = base.substr(0, dot);
    // base is like "dataset_1000"; pull the trailing number
    size_t us = base.rfind('_');
    if (us != string::npos) return base.substr(us + 1);
    return base;
}


int main() {
    

    // string input_file = "dataset_1000.csv"; int start_row = 100; int end_row = 110;
    string input_file = "dataset_1000.csv";  int start_row = 1;   int end_row = 7;

    // ==============================================================

    vector<Record> arr = readCSVRange(input_file, start_row, end_row);
    if (arr.empty()) {
        cerr << "No data read in the given row range. Check filename and rows." << endl;
        return 1;
    }

    vector<string> steps;
    heapSortSteps(arr, steps);

    // Output filename: dataset_<n>_heap_sorted_step_<start>_<end>.txt
    string token   = extractSizeToken(input_file);
    string out_txt = "dataset_" + token + "_heap_sorted_step_"
                   + to_string(start_row) + "_" + to_string(end_row) + ".txt";

    ofstream fout(out_txt);
    if (!fout.is_open()) {
        cerr << "Error: cannot write " << out_txt << endl;
        return 1;
    }
    for (const auto& s : steps) fout << s << "\n";
    fout.close();

    // Echo to console for the demo
    for (const auto& s : steps) cout << s << "\n";
    cout << "\nWritten to: " << out_txt << endl;
    return 0;
}
