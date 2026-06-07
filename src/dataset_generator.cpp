// *********************************************************
// Program: dataset_generator.cpp
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
// Member_1: Dataset Generator — generates n unique random (integer, string) records
// Member_2: Radix Sort Step
// Member_3: Hash Table Search
// Member_4: Hash Table Search Step + Conclusion
// *********************************************************
//
// Seed derivation for group leader ID: 242UC243BE
// Mapping table (A=1 B=2 C=3 D=4 E=5 F=6 G=7 H=8 I=9 J=0
//                K=1 L=2 M=3 N=4 O=5 P=6 Q=7 R=8 S=9 T=0
//                U=1 V=2 W=3 X=4 Y=5 Z=6):
//   2  4  2  U  C  2  4  3  B  E
//   2  4  2  1  3  2  4  3  2  5
//   => seed = 2421324325
//
// Usage:
//   g++ dataset_generator.cpp -o dataset_generator -std=c++11
//   ./dataset_generator 1000
//   ./dataset_generator 10000
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
    // unordered_set gives O(1) average lookup — much faster than set for large n
    unordered_set<long long> used;
    used.reserve(n * 2);   // pre-allocate to reduce rehashing

    cout << "Generating " << n << " unique records to " << filename << " ..." << endl;

    long long count = 0;
    while (count < n) {
        long long num = intDist(rng);

        // Skip if already used (collision → just try again)
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