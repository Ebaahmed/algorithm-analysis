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
#include <chrono>

using namespace std;
using namespace chrono;


struct Record {
    long long integer_val;
    string    str_val;
};


vector<Record> readCSV(const string& filename) {
    vector<Record> data;
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: cannot open file " << filename << endl;
        return data;
    }

    string line;
    while (getline(fin, line)) {
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


void heapSort(vector<Record>& arr) {
    int n = (int)arr.size();

    // Phase 1: build max-heap
    for (int i = n / 2 - 1; i >= 0; --i)
        heapify(arr, n, i);

    // Phase 2: extract elements one by one
    for (int i = n - 1; i > 0; --i) {
        swap(arr[0], arr[i]);      // move current max to end
        heapify(arr, i, 0);        // restore heap property for reduced heap
    }
}

// -------------------------------------------------------
// Extract dataset size token from filename for output name
// -------------------------------------------------------
string extractSizeToken(const string& csv_filename) {
    string base = csv_filename;
    size_t slash = base.find_last_of("/\\");
    if (slash != string::npos) base = base.substr(slash + 1);
    size_t dot = base.rfind('.');
    if (dot != string::npos) base = base.substr(0, dot);
    return base;  // e.g. "dataset_1000000"
}

// -------------------------------------------------------
// Write sorted array to CSV  (integer/string per row)
// -------------------------------------------------------
void writeSortedCSV(const string& filename, const vector<Record>& arr) {
    ofstream fout(filename);
    if (!fout.is_open()) {
        cerr << "Error: cannot write " << filename << endl;
        return;
    }
    for (const auto& rec : arr)
        fout << rec.integer_val << "/" << rec.str_val << "\n";
    fout.close();
}


void runAndTime(const string& csv_filename) {
    cout << "\n--- Processing: " << csv_filename << " ---" << endl;

    // Read (I/O excluded from timing)
    vector<Record> arr = readCSV(csv_filename);
    if (arr.empty()) {
        cerr << "  No data read. Skipping." << endl;
        return;
    }
    size_t n = arr.size();
    cout << "  Dataset size: " << n << endl;

    // ---- START TIMING ----
    auto t_start = high_resolution_clock::now();
    heapSort(arr);
    auto t_end   = high_resolution_clock::now();
    // ---- STOP TIMING ----

    double elapsed = duration<double>(t_end - t_start).count();

    // Console output
    cout << "  Running time: " << elapsed << " seconds" << endl;

    // Build output filenames
    string token   = extractSizeToken(csv_filename);
    string out_csv = "heap_sorted_" + token + ".csv";
    string out_txt = "heap_sort_"   + token + ".txt";

    // Write sorted CSV
    writeSortedCSV(out_csv, arr);

    // Write running time to txt (also include in sorted file)
    ofstream ftxt(out_txt);
    if (ftxt.is_open()) {
        ftxt << "Dataset: "      << csv_filename << "\n";
        ftxt << "Size: "         << n            << "\n";
        ftxt << "Running time: " << elapsed      << " seconds\n";
        ftxt.close();
    }

    // Also append running time at end of sorted CSV
    ofstream fcsv(out_csv, ios::app);
    if (fcsv.is_open()) {
        fcsv << "# Running time: " << elapsed << " seconds\n";
        fcsv.close();
    }

    cout << "  Sorted output : " << out_csv << endl;
    cout << "  Time log      : " << out_txt << endl;
}


int main() {
    // ==============================================================
    // INPUT CONFIGURATION  (comment/uncomment as needed)
    // ==============================================================

    vector<string> datasets = {
        // --- experiment sizes (at least 10) ---
        //  "dataset_1000.csv",
        //  "dataset_5000.csv",
        //  "dataset_10000.csv",
        //  "dataset_50000.csv",
        //  "dataset_100000.csv",
        //  "dataset_500000.csv",
        //  "dataset_1000000.csv",
        //  "dataset_5000000.csv",
        //  "dataset_10000000.csv",
        //  "dataset_50000000.csv",
        //  "dataset_100000000.csv",

        // --- demo default ---
        "dataset_1000.csv"
    };

    // ==============================================================

    for (const auto& f : datasets)
        runAndTime(f);

    cout << "\nDone." << endl;
    return 0;
}
