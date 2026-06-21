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
#include <climits>
#include <chrono>

using namespace std;
using namespace chrono;

// -------------------------------------------------------
// Data record: 10-digit integer key + 5-letter string.
// -------------------------------------------------------
struct Record {
    long long integer_val;
    string    str_val;
};

// -------------------------------------------------------
// AVL tree node (one per stored record).
// -------------------------------------------------------
struct AVLNode {
    long long key;       // integer_val
    string    value;     // str_val
    int       height;
    AVLNode*  left;
    AVLNode*  right;
    AVLNode(long long k, const string& v)
        : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
};

static int nodeHeight(AVLNode* n) { return n ? n->height : 0; }
static int balanceFactor(AVLNode* n) { return n ? nodeHeight(n->left) - nodeHeight(n->right) : 0; }
static void updateHeight(AVLNode* n) {
    int hl = nodeHeight(n->left), hr = nodeHeight(n->right);
    n->height = 1 + (hl > hr ? hl : hr);
}

static AVLNode* rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* t = x->right;
    x->right = y;
    y->left  = t;
    updateHeight(y);
    updateHeight(x);
    return x;
}

static AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* t = y->left;
    y->left  = x;
    x->right = t;
    updateHeight(x);
    updateHeight(y);
    return y;
}

// Insert (key,value) into the AVL tree, returning the new subtree root.
static AVLNode* avlInsert(AVLNode* node, long long key, const string& value) {
    if (!node) return new AVLNode(key, value);

    if (key < node->key)
        node->left = avlInsert(node->left, key, value);
    else if (key > node->key)
        node->right = avlInsert(node->right, key, value);
    else {
        node->value = value;   // duplicate key (dataset is unique, but safe)
        return node;
    }

    updateHeight(node);
    int bf = balanceFactor(node);

    if (bf > 1 && key < node->left->key)  return rotateRight(node);
    if (bf < -1 && key > node->right->key) return rotateLeft(node);
    if (bf > 1 && key > node->left->key)  { node->left = rotateLeft(node->left); return rotateRight(node); }
    if (bf < -1 && key < node->right->key) { node->right = rotateRight(node->right); return rotateLeft(node); }

    return node;
}

// Search the AVL tree for key. On success, out_value receives the
// string and the function returns true; comparisons counts probes.
static bool avlSearch(AVLNode* node, long long key, string& out_value) {
    while (node) {
        if      (key == node->key) { out_value = node->value; return true; }
        else if (key <  node->key) node = node->left;
        else                       node = node->right;
    }
    return false;
}

// Number of comparisons to reach a key (depth + 1). Used to find
// best / worst representative targets for timing.
static int avlSearchDepth(AVLNode* node, long long key) {
    int d = 0;
    while (node) {
        ++d;
        if      (key == node->key) return d;
        else if (key <  node->key) node = node->left;
        else                       node = node->right;
    }
    return -1;  // not found
}

static void avlDestroy(AVLNode* n) {
    if (!n) return;
    avlDestroy(n->left);
    avlDestroy(n->right);
    delete n;
}

// -------------------------------------------------------
// Hash table: array of AVL-tree bucket roots.
// -------------------------------------------------------
class HashTable {
public:
    explicit HashTable(long long table_size) : size_(table_size), buckets_(table_size, nullptr) {}
    ~HashTable() { for (auto* b : buckets_) avlDestroy(b); }

    // Hash function: key % table_size (table_size is a prime > n).
    long long hash(long long key) const {
        return (key % size_ + size_) % size_;
    }

    void insert(long long key, const string& value) {
        long long h = hash(key);
        buckets_[h] = avlInsert(buckets_[h], key, value);
    }

    // Returns true if found, filling out_value with the string field.
    bool search(long long key, string& out_value) const {
        long long h = hash(key);
        return avlSearch(buckets_[h], key, out_value);
    }

    int searchDepth(long long key) const {
        long long h = hash(key);
        return avlSearchDepth(buckets_[h], key);
    }

    long long tableSize() const { return size_; }

private:
    long long              size_;
    vector<AVLNode*>       buckets_;
};

// -------------------------------------------------------
// Smallest prime >= n (used to size the table so load factor ~1).
// -------------------------------------------------------
static bool isPrime(long long x) {
    if (x < 2) return false;
    if (x % 2 == 0) return x == 2;
    for (long long i = 3; i * i <= x; i += 2)
        if (x % i == 0) return false;
    return true;
}
static long long nextPrime(long long n) {
    long long c = (n < 2) ? 2 : n;
    while (!isPrime(c)) ++c;
    return c;
}

// -------------------------------------------------------
// Read all rows from a CSV file (I/O — excluded from timing).
// -------------------------------------------------------
static vector<Record> readCSV(const string& filename) {
    vector<Record> data;
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: cannot open file " << filename << endl;
        return data;
    }
    string line;
    while (getline(fin, line)) {
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

static string extractSizeToken(const string& csv_filename) {
    string base = csv_filename;
    size_t slash = base.find_last_of("/\\");
    if (slash != string::npos) base = base.substr(slash + 1);
    size_t dot = base.rfind('.');
    if (dot != string::npos) base = base.substr(0, dot);
    size_t us = base.rfind('_');
    if (us != string::npos) return base.substr(us + 1);
    return base;
}

// -------------------------------------------------------
// Run best / average / worst timing for one dataset file.
// A single search is too fast to time, so we perform n searches
// per case and divide by n to get the per-search time.
// -------------------------------------------------------
static void runAndTime(const string& csv_filename) {
    cout << "\n--- Processing: " << csv_filename << " ---" << endl;

    vector<Record> data = readCSV(csv_filename);   // I/O, not timed
    if (data.empty()) {
        cerr << "  No data read. Skipping." << endl;
        return;
    }
    long long n = (long long)data.size();
    cout << "  Dataset size: " << n << endl;

    // Build the table (prime size > n keeps the load factor near 1).
    long long table_size = nextPrime(n);
    HashTable table(table_size);
    for (const auto& r : data)
        table.insert(r.integer_val, r.str_val);

   
    int min_depth = INT_MAX, max_depth = 0;
    for (const auto& r : data) {
        int d = table.searchDepth(r.integer_val);
        if (d < min_depth) min_depth = d;
        if (d > max_depth) max_depth = d;
    }

    vector<long long> best_keys, worst_keys;
    for (const auto& r : data) {
        int d = table.searchDepth(r.integer_val);
        if (d == min_depth) best_keys.push_back(r.integer_val);
        if (d == max_depth) worst_keys.push_back(r.integer_val);
    }

    string sink;                 // receives the found string
    volatile bool guard = false; // prevents the search loops being optimised away

    // ---- BEST CASE : n searches cycling through the shallowest keys ----
    size_t bsz = best_keys.size();
    auto b0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i)
        guard ^= table.search(best_keys[i % bsz], sink);
    auto b1 = high_resolution_clock::now();
    double best_time = duration<double>(b1 - b0).count() / (double)n;

    // ---- AVERAGE CASE : search every key in the dataset once ----
    auto a0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i)
        guard ^= table.search(data[i].integer_val, sink);
    auto a1 = high_resolution_clock::now();
    double avg_time = duration<double>(a1 - a0).count() / (double)n;

    // ---- WORST CASE : n searches cycling through the deepest keys ----
    size_t wsz = worst_keys.size();
    auto w0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i)
        guard ^= table.search(worst_keys[i % wsz], sink);
    auto w1 = high_resolution_clock::now();
    double worst_time = duration<double>(w1 - w0).count() / (double)n;

    (void)guard;

    int best_depth = min_depth, worst_depth = max_depth;

    cout.setf(ios::scientific);
    cout << "  Best case time:    " << best_time  << " seconds (depth " << best_depth  << ")" << endl;
    cout << "  Average case time: " << avg_time   << " seconds" << endl;
    cout << "  Worst case time:   " << worst_time << " seconds (depth " << worst_depth << ")" << endl;

    // Output file: hash_table_search_dataset_<n>.txt
    string token   = extractSizeToken(csv_filename);
    string out_txt = "hash_table_search_dataset_" + token + ".txt";
    ofstream fout(out_txt);
    if (fout.is_open()) {
        fout.setf(ios::scientific);
        fout << "Best case time: "    << best_time  << " seconds\n";
        fout << "Average case time: " << avg_time   << " seconds\n";
        fout << "Worst case time: "   << worst_time << " seconds\n";
        fout.close();
        cout << "  Written: " << out_txt << endl;
    }
}


int main() {
   
    vector<string> datasets = {
        "dataset_1000.csv",
        "dataset_5000.csv",
        "dataset_10000.csv",
        "dataset_50000.csv",
        "dataset_100000.csv",
        "dataset_250000.csv",
        "dataset_500000.csv",
        "dataset_750000.csv",
        "dataset_1000000.csv",
        "dataset_2000000.csv"
    };
    // ==============================================================

    for (const auto& f : datasets)
        runAndTime(f);

    cout << "\nDone." << endl;
    return 0;
}
