// *********************************************************
// Program: hash_table_search.cpp
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
// Member_1: Dataset Generator
// Member_2: Radix Sort Step
// Member_3: Hash Table Search
// Member_4: Hash Table Search Step + Conclusion
// *********************************************************
//
// DESCRIPTION:
//   Builds a hash table (separate chaining with AVL trees) from
//   datasetPath (n records). Searches keys from queryPath.
//
//   Output file hash_table_search_dataset_n.txt contains:
//     - Best case section:    all n searches listed (query,found,index)
//                             + Best case time
//     - Average case section: all n searches listed (query,found,index)
//                             + Average case time
//     - Worst case section:   all n searches listed (query,found,index)
//                             + Worst case time
//
//   A single search is too fast to measure, so n searches are
//   performed per case and divided by n for per-search time.
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
// AVL tree node.
// -------------------------------------------------------
struct AVLNode {
    long long key;
    string    value;
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
    AVLNode* x = y->left; AVLNode* t = x->right;
    x->right = y; y->left = t;
    updateHeight(y); updateHeight(x); return x;
}
static AVLNode* rotateLeft(AVLNode* x) {
    AVLNode* y = x->right; AVLNode* t = y->left;
    y->left = x; x->right = t;
    updateHeight(x); updateHeight(y); return y;
}

static AVLNode* avlInsert(AVLNode* node, long long key, const string& value) {
    if (!node) return new AVLNode(key, value);
    if      (key < node->key) node->left  = avlInsert(node->left,  key, value);
    else if (key > node->key) node->right = avlInsert(node->right, key, value);
    else { node->value = value; return node; }
    updateHeight(node);
    int bf = balanceFactor(node);
    if (bf >  1 && key < node->left->key)   return rotateRight(node);
    if (bf < -1 && key > node->right->key)  return rotateLeft(node);
    if (bf >  1 && key > node->left->key)   { node->left  = rotateLeft(node->left);  return rotateRight(node); }
    if (bf < -1 && key < node->right->key)  { node->right = rotateRight(node->right); return rotateLeft(node); }
    return node;
}

static bool avlSearch(AVLNode* node, long long key, string& out_value, long long& out_index_unused) {
    while (node) {
        if      (key == node->key) { out_value = node->value; return true; }
        else if (key <  node->key) node = node->left;
        else                       node = node->right;
    }
    return false;
}

static int avlSearchDepth(AVLNode* node, long long key) {
    int d = 0;
    while (node) {
        ++d;
        if      (key == node->key) return d;
        else if (key <  node->key) node = node->left;
        else                       node = node->right;
    }
    return -1;
}

static void avlDestroy(AVLNode* n) {
    if (!n) return;
    avlDestroy(n->left); avlDestroy(n->right); delete n;
}

// -------------------------------------------------------
// Hash table: array of AVL-tree bucket roots.
// -------------------------------------------------------
class HashTable {
public:
    explicit HashTable(long long sz) : size_(sz), buckets_(sz, nullptr) {}
    ~HashTable() { for (auto* b : buckets_) avlDestroy(b); }

    long long hash(long long key) const { return (key % size_ + size_) % size_; }

    void insert(long long key, const string& value) {
        long long h = hash(key);
        buckets_[h] = avlInsert(buckets_[h], key, value);
    }

    // search: returns found, fills out_value and out_index (bucket index)
    bool search(long long key, string& out_value, long long& out_index) const {
        out_index = hash(key);
        long long dummy = 0;
        return avlSearch(buckets_[out_index], key, out_value, dummy);
    }

    int searchDepth(long long key) const {
        return avlSearchDepth(buckets_[hash(key)], key);
    }

private:
    long long        size_;
    vector<AVLNode*> buckets_;
};

// -------------------------------------------------------
// Smallest prime >= n.
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
// Read CSV file (I/O, not timed).
// -------------------------------------------------------
static vector<Record> readCSV(const string& filename) {
    vector<Record> data;
    ifstream fin(filename);
    if (!fin.is_open()) { cerr << "  Error: cannot open " << filename << endl; return data; }
    string line;
    while (getline(fin, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' '))
            line.pop_back();
        if (line.empty() || line[0] == '#' || line[0] == 'R') continue;
        size_t comma = line.find(',');
        if (comma == string::npos) continue;
        Record rec;
        try { rec.integer_val = stoll(line.substr(0, comma)); } catch (...) { continue; }
        rec.str_val = line.substr(comma + 1);
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
// Main processing function.
// Compares two files: datasetPath (hash table) vs queryPath (search targets).
// Output file lists ALL searches for best, average, and worst case,
// each as: query,found,index  -- then the timing at the end of each section.
// -------------------------------------------------------
static void runAndTime(const string& datasetPath, const string& queryPath) {
    cout << "\n--- Processing: " << datasetPath << " ---" << endl;

    // I/O — not timed
    vector<Record> data  = readCSV(datasetPath);
    vector<Record> query = readCSV(queryPath);

    if (data.empty())  { cerr << "  No data in dataset. Skipping.\n"; return; }
    if (query.empty()) { cerr << "  No data in query file. Skipping.\n"; return; }

    long long n = (long long)data.size();
    long long m = (long long)query.size();

    cout << "  Dataset size: " << n << ", Queries: " << m << endl;

    // Build hash table from dataset (not timed)
    HashTable table(nextPrime(n));
    for (const auto& r : data)
        table.insert(r.integer_val, r.str_val);

    // Find min/max AVL depth among query keys -> best/worst case key sets
    int min_depth = INT_MAX, max_depth = 0;
    for (const auto& q : query) {
        int d = table.searchDepth(q.integer_val);
        if (d == -1) d = max_depth;
        if (d < min_depth) min_depth = d;
        if (d > max_depth) max_depth = d;
    }
    if (min_depth == INT_MAX) min_depth = 1;

    vector<Record> best_queries, worst_queries;
    for (const auto& q : query) {
        int d = table.searchDepth(q.integer_val);
        if (d == -1) d = max_depth;
        if (d == min_depth) best_queries.push_back(q);
        if (d == max_depth) worst_queries.push_back(q);
    }
    if (best_queries.empty())  best_queries.push_back(query[0]);
    if (worst_queries.empty()) worst_queries.push_back(query[m-1]);

    size_t bsz = best_queries.size();
    size_t wsz = worst_queries.size();

    // Output file
    string token   = extractSizeToken(datasetPath);
    string txt_out = "hash_table_search_dataset_" + token + ".txt";
    ofstream fout(txt_out);
    if (!fout.is_open()) { cerr << "  Error: cannot write " << txt_out << endl; return; }
    fout.setf(ios::scientific);

    string val;
    long long idx;
    volatile bool guard = false;

    // ==============================================================
    // BEST CASE: n searches cycling through shallowest-depth keys
    // List ALL n searches: query,found,index
    // ==============================================================
    fout << "=== BEST CASE (depth " << min_depth << ") ===\n";
    fout << "query,found,index\n";

    auto b0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i) {
        const Record& q = best_queries[i % bsz];
        bool found = table.search(q.integer_val, val, idx);
        guard ^= found;
        fout << q.integer_val << "," << (found ? "yes" : "no") << "," << idx << "\n";
    }
    auto b1 = high_resolution_clock::now();
    double best_time = duration<double>(b1 - b0).count() / (double)n;
    fout << "Best case time: " << best_time << " seconds\n\n";

    // ==============================================================
    // AVERAGE CASE: n searches cycling through ALL query keys
    // List ALL n searches: query,found,index
    // ==============================================================
    fout << "=== AVERAGE CASE ===\n";
    fout << "query,found,index\n";

    auto a0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i) {
        const Record& q = query[i % m];
        bool found = table.search(q.integer_val, val, idx);
        guard ^= found;
        fout << q.integer_val << "," << (found ? "yes" : "no") << "," << idx << "\n";
    }
    auto a1 = high_resolution_clock::now();
    double avg_time = duration<double>(a1 - a0).count() / (double)n;
    fout << "Average case time: " << avg_time << " seconds\n\n";

    // ==============================================================
    // WORST CASE: n searches cycling through deepest-depth keys
    // List ALL n searches: query,found,index
    // ==============================================================
    fout << "=== WORST CASE (depth " << max_depth << ") ===\n";
    fout << "query,found,index\n";

    auto w0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i) {
        const Record& q = worst_queries[i % wsz];
        bool found = table.search(q.integer_val, val, idx);
        guard ^= found;
        fout << q.integer_val << "," << (found ? "yes" : "no") << "," << idx << "\n";
    }
    auto w1 = high_resolution_clock::now();
    double worst_time = duration<double>(w1 - w0).count() / (double)n;
    fout << "Worst case time: " << worst_time << " seconds\n";

    (void)guard;
    fout.close();

    // Print summary to console
    cout.setf(ios::scientific);
    cout << "  Best case time:    " << best_time  << " seconds (depth " << min_depth << ")" << endl;
    cout << "  Average case time: " << avg_time   << " seconds" << endl;
    cout << "  Worst case time:   " << worst_time << " seconds (depth " << max_depth << ")" << endl;
    cout << "  Written: " << txt_out << endl;
}

// -------------------------------------------------------
// main: runs all 10 dataset sizes.
// datasetPath = large file loaded into hash table.
// queryPath   = smaller file whose keys are searched.
// -------------------------------------------------------
int main() {
    const string queryPath = "dataset_1000.csv";  // fixed query pool (the smaller file)

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

    for (const auto& ds : datasets)
        runAndTime(ds, queryPath);

    cout << "\nDone." << endl;
    return 0;
}

