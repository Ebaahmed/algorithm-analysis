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
#include <unordered_set>
#include <string>
#include <random>
using namespace std;

// Generate a random 5-letter lowercase string using the given engine
string randomString(mt19937_64& rng) {
    string s(5, ' ');
    uniform_int_distribution<int> letterDist(0, 25);
    for (int i = 0; i < 5; i++) {
        s[i] = (char)('a' + letterDist(rng));
    }
    return s;
}

int main(int argc, char* argv[]) {
    // ── Argument check ────────────────────────────────────────
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <n>" << endl;
        cout << "Example: ./dataset_generator 1000" << endl;
        return 1;
    }

    long long n = atoll(argv[1]);
    if (n <= 0) {
        cout << "Error: n must be a positive integer." << endl;
        return 1;
    }

    // ── Seed: group leader student ID 242UC243BE → 2421324325 ─
    const unsigned long long SEED = 2421324325ULL;
    mt19937_64 rng(SEED);

    // ── Integer range: 1,000,000,000 to 9,999,999,999 ─────────
    const long long MIN_VAL = 1000000000LL;
    const long long MAX_VAL = 9999999999LL;
    uniform_int_distribution<long long> intDist(MIN_VAL, MAX_VAL);

    // ── Output file ───────────────────────────────────────────
    string filename = "dataset_" + to_string(n) + ".csv";
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cout << "Error: Could not open output file: " << filename << endl;
        return 1;
    }

    // ── Generate unique records ───────────────────────────────
    unordered_set<long long> used;
    used.reserve(n * 2);   // pre-allocate to reduce rehashing

    cout << "Generating " << n << " unique records to " << filename << " ..." << endl;

    long long count = 0;
    while (count < n) {
        long long num = intDist(rng);

        if (used.count(num)) continue;

        used.insert(num);
        outFile << num << "," << randomString(rng) << "\n";
        count++;

        if (count % 100000 == 0) {
            cout << "  " << count << " / " << n << " records written..." << endl;
        }
    }

    outFile.close();
    cout << "Done! Saved to: " << filename << endl;
    return 0;
}