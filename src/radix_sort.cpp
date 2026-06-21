// *********************************************************
// Program: radix_sort.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T22L
// Trimester: 2610
// Member_1: 242UC243BE | EBA MOHAMED ABBAS AHMED    | +60136734355
// Member_2: 242UC244SN | MOHAMMAD IEMAN BIN ZAHARI  | +60183756140
// Member_3: 242UC243B4 | LAMA M. R. SIAM            | +60183942734
// Member_4: 242UC244L3 | HAREIN A/L SATHIAMURTHY    | +60143653986
// *********************************************************
// Task Distribution
// Member_1: Radix Sort — LSD radix sort on integer field, outputs sorted CSV
// Member_2: Radix Sort Step
// Member_3: Hash Table Search
// Member_4: Hash Table Search Step + Conclusion
// *********************************************************
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
using namespace std;

struct Record {
    long long key;   // 10-digit integer (sort key)
    string    label; // 5-letter lowercase string
};

bool readCSV(const string& filename, vector<Record>& data) {
    ifstream in(filename);
    if (!in.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return false;
    }
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        size_t comma = line.find(',');
        if (comma == string::npos) continue;
        Record r;
        r.key   = stoll(line.substr(0, comma));
        r.label = line.substr(comma + 1);
        // Trim any trailing CR/whitespace
        while (!r.label.empty() && (r.label.back() == '\r' || r.label.back() == ' '))
            r.label.pop_back();
        data.push_back(r);
    }
    in.close();
    return true;
}


void countingSortByDigit(vector<Record>& arr, long long exp) {
    int n = (int)arr.size();
    vector<Record> output(n);
    int count[10] = {0};

    // Count occurrences of each digit
    for (int i = 0; i < n; i++) {
        int digit = (int)((arr[i].key / exp) % 10);
        count[digit]++;
    }

    // Cumulative count (prefix sum)
    for (int i = 1; i < 10; i++) {
        count[i] += count[i - 1];
    }

    // Build output array (traverse right-to-left to keep stability)
    for (int i = n - 1; i >= 0; i--) {
        int digit = (int)((arr[i].key / exp) % 10);
        output[--count[digit]] = arr[i];
    }

    // Copy back
    for (int i = 0; i < n; i++) {
        arr[i] = output[i];
    }
}

// ── Full LSD Radix Sort ───────────────────────────────────────
// Integers are all 10 digits (1,000,000,000 – 9,999,999,999)
// so we always do exactly 10 passes: exp = 1, 10, 100, ... 1,000,000,000
void radixSort(vector<Record>& arr) {
    long long exp = 1;
    for (int d = 0; d < 10; d++, exp *= 10) {
        countingSortByDigit(arr, exp);
    }
}

void processFile(const string& inputFile) {
    // ── 1. Read input (I/O excluded from timing) ──────────────
    vector<Record> data;
    if (!readCSV(inputFile, data)) return;

    long long n = (long long)data.size();
    cout << "\n[" << inputFile << "]  " << n << " records loaded." << endl;

    auto startTime = chrono::high_resolution_clock::now();
    radixSort(data);
    auto endTime   = chrono::high_resolution_clock::now();

    double elapsed = chrono::duration<double>(endTime - startTime).count();

    
    string outFilename = "radix_sorted_" + inputFile;
    // If input has a directory prefix, strip it
    size_t slash = outFilename.rfind('/');
    string outBase = (slash == string::npos) ? outFilename : outFilename.substr(slash + 1);

    // ── 5. Write output (I/O excluded from timing) ────────────
    ofstream out(outBase);
    if (!out.is_open()) {
        cerr << "Error: Cannot write to " << outBase << endl;
        return;
    }

    for (const Record& r : data) {
        out << r.key << "/" << r.label << "\n";
    }

    // Append running time to output file
    out << "\nRunning time: " << elapsed << " seconds\n";
    out.close();

    // ── 6. Print running time to console ─────────────────────
    cout << "Running time : " << elapsed << " seconds" << endl;
    cout << "Output saved : " << outBase << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <dataset1.csv> [dataset2.csv ...]" << endl;
        cout << "Example: ./radix_sort dataset_1000.csv" << endl;
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        processFile(argv[i]);
    }

    return 0;
}