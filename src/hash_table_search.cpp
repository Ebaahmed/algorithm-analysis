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
// Member_1: Dataset Generator + Radix Sort
// Member_2: Radix Sort Step + Heap Sort
// Member_3: Hash Table Search + Heap Sort Step
// Member_4: Hash Table Search Step + Conclusion
// *********************************************************

// -------------------------------------------------------
// Hash Table Search with a SEPARATE TARGET FILE.
//   - The dataset file is loaded into a hash table (AVL-tree buckets).
//   - A separate, smaller target file supplies the keys to search for.
//   - Best / Average / Worst case search time is measured, performing
//     n searches per case (n = dataset size).
//   - Output: hash_table_search_dataset_<n>.txt
// -------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <climits>
#include <chrono>

using namespace std;
using namespace chrono;

struct Record {
    long long integer_val;
    string    str_val;
};

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

static AVLNode* avlInsert(AVLNode* node, long long key, const string& value) {
    if (!node) return new AVLNode(key, value);

    if (key < node->key)
        node->left = avlInsert(node->left, key, value);
    else if (key > node->key)
        node->right = avlInsert(node->right, key, value);
    else {
        node->value = value;
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

static bool avlSearch(AVLNode* node, long long key, string& out_value) {
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
    avlDestroy(n->left);
    avlDestroy(n->right);
    delete n;
}

class HashTable {
public:
    explicit HashTable(long long table_size) : size_(table_size), buckets_(table_size, nullptr) {}
    ~HashTable() { for (auto* b : buckets_) avlDestroy(b); }

    long long hash(long long key) const {
        return (key % size_ + size_) % size_;
    }
    void insert(long long key, const string& value) {
        long long h = hash(key);
        buckets_[h] = avlInsert(buckets_[h], key, value);
    }
    bool search(long long key, string& out_value) const {
        return avlSearch(buckets_[hash(key)], key, out_value);
    }
    int searchDepth(long long key) const {
        return avlSearchDepth(buckets_[hash(key)], key);
    }
    long long tableSize() const { return size_; }

private:
    long long          size_;
    vector<AVLNode*>   buckets_;
};

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

static vector<Record> readCSV(const string& filename) {
    vector<Record> data;
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: cannot open dataset file " << filename << endl;
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

static void runAndTime(const string& dataset_file, const string& target_file) {
    cout << "\n--- Processing: " << dataset_file
         << "  (targets: " << target_file << ") ---" << endl;

    vector<Record> data = readCSV(dataset_file);
    if (data.empty()) { cerr << "  No dataset read. Skipping." << endl; return; }

    // Read the target file as full records (integer + string), then
    // derive the list of keys to search. Reading as records lets us
    // later write the (integer, string) rows to the output file.
    vector<Record> targets_records = readCSV(target_file);
    if (targets_records.empty()) { cerr << "  No targets read. Skipping." << endl; return; }

    vector<long long> targets;
    targets.reserve(targets_records.size());
    for (const auto& r : targets_records) targets.push_back(r.integer_val);

    long long n = (long long)data.size();
    cout << "  Dataset size: " << n << ",  targets: " << targets.size() << endl;

    long long table_size = nextPrime(n);
    HashTable table(table_size);
    for (const auto& r : data)
        table.insert(r.integer_val, r.str_val);

    int min_depth = INT_MAX, max_depth = -1;
    long long best_key = targets[0], worst_key = targets[0];
    bool any_found = false;
    for (long long t : targets) {
        int d = table.searchDepth(t);
        if (d < 0) continue;
        any_found = true;
        if (d < min_depth) { min_depth = d; best_key = t; }
        if (d > max_depth) { max_depth = d; worst_key = t; }
    }
    if (!any_found) { best_key = worst_key = targets[0]; min_depth = max_depth = -1; }

    string sink;
    volatile bool guard = false;

    auto b0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i) guard ^= table.search(best_key, sink);
    auto b1 = high_resolution_clock::now();
    double best_time = duration<double>(b1 - b0).count() / (double)n;

    size_t tsz = targets.size();
    auto a0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i) guard ^= table.search(targets[i % tsz], sink);
    auto a1 = high_resolution_clock::now();
    double avg_time = duration<double>(a1 - a0).count() / (double)n;

    auto w0 = high_resolution_clock::now();
    for (long long i = 0; i < n; ++i) guard ^= table.search(worst_key, sink);
    auto w1 = high_resolution_clock::now();
    double worst_time = duration<double>(w1 - w0).count() / (double)n;

    (void)guard;

    cout.setf(ios::scientific);
    cout << "  Best case time:    " << best_time  << " seconds (depth " << min_depth << ")" << endl;
    cout << "  Average case time: " << avg_time   << " seconds" << endl;
    cout << "  Worst case time:   " << worst_time << " seconds (depth " << max_depth << ")" << endl;

    string token   = extractSizeToken(dataset_file);
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

int main(int argc, char* argv[]) {
    vector<pair<string,string>> jobs;

    if (argc >= 3) {
        for (int i = 1; i + 1 < argc; i += 2)
            jobs.push_back({argv[i], argv[i + 1]});
    } else {
        jobs = {
            {"dataset_1000.csv",    "dataset_100.csv"},
            {"dataset_5000.csv",    "dataset_1000.csv"},
            {"dataset_10000.csv",   "dataset_5000.csv"},
            {"dataset_50000.csv",   "dataset_10000.csv"},
            {"dataset_100000.csv",  "dataset_50000.csv"},
            {"dataset_250000.csv",  "dataset_100000.csv"},
            {"dataset_500000.csv",  "dataset_250000.csv"},
            {"dataset_750000.csv",  "dataset_500000.csv"},
            {"dataset_1000000.csv", "dataset_750000.csv"},
            {"dataset_2000000.csv", "dataset_1000000.csv"}
        };
    }

    for (const auto& job : jobs)
        runAndTime(job.first, job.second);

    cout << "\nDone." << endl;
    return 0;
}
