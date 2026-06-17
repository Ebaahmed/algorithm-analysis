// *********************************************************
// Program: hash_table_search_step.cpp
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
// Member_1: Dataset Generator + Radix Sort
// Member_2: Radix Sort Step + Heap Sort
// Member_3: Hash Table Search + Heap Sort Step
// Member_4: Hash Table Search Step + Conclusion
// *********************************************************
//
// This program traces a SINGLE search through the hash table and
// writes the full search path to a file. It reuses the same
// structure as hash_table_search.cpp: SEPARATE CHAINING where each
// bucket is an AVL (height-balanced) binary search tree keyed on the
// 10-digit integer.
//
// For a given target the program prints, step by step:
//   1. the hash index (which bucket the target maps to)
//   2. every node it compares along the AVL tree path, and the
//      decision taken (go left / go right / match)
//   3. the final result line, exactly in the assignment's format:
//        found     ->  <target> = <target>/<string>
//        not found ->  -1 != <target>
//
// Output file: dataset_<n>_hash_table_search_step_<target>.txt
// *********************************************************

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// -------------------------------------------------------
// Data record: 10-digit integer key + 5-letter string.
// -------------------------------------------------------
struct Record {
    long long integer_val;
    string    str_val;
};

// -------------------------------------------------------
// AVL tree node (one per stored record). Identical to the node
// used in hash_table_search.cpp so the trees are built the same way.
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

    if (bf > 1 && key < node->left->key)  return rotateRight(node);              // Left Left
    if (bf < -1 && key > node->right->key) return rotateLeft(node);              // Right Right
    if (bf > 1 && key > node->left->key)  { node->left = rotateLeft(node->left); return rotateRight(node); }  // Left Right
    if (bf < -1 && key < node->right->key) { node->right = rotateRight(node->right); return rotateLeft(node); } // Right Left

    return node;
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

    AVLNode* bucketRoot(long long key) const {
        return buckets_[hash(key)];
    }

    long long tableSize() const { return size_; }

private:
    long long          size_;
    vector<AVLNode*>   buckets_;
};

// -------------------------------------------------------
// Smallest prime >= n (used to size the table so load factor ~1).
// Same sizing rule as hash_table_search.cpp.
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
// Read all rows from a CSV file (I/O — not part of the search step).
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
// Trace one search and record every step into `steps`.
//
// Walking the AVL tree of the target's bucket:
//   - if target == node->key  -> match, record the found line
//   - if target <  node->key  -> go left
//   - if target >  node->key  -> go right
//   - if we fall off the tree (nullptr) -> not found
// -------------------------------------------------------
static void searchStep(const HashTable& table, long long target, vector<string>& steps) {
    long long h = table.hash(target);

    {
        ostringstream oss;
        oss << "Searching for target: " << target;
        steps.push_back(oss.str());
    }
    {
        ostringstream oss;
        oss << "Hash index = " << target << " % " << table.tableSize()
            << " = " << h;
        steps.push_back(oss.str());
    }

    AVLNode* node = table.bucketRoot(target);

    if (!node) {
        steps.push_back("Bucket " + to_string(h) + " is empty (no AVL tree).");
    }

    int step_no = 0;
    bool found = false;
    string found_value;

    while (node) {
        ++step_no;
        ostringstream oss;
        oss << "Step " << step_no << ": compare with " << node->key
            << "/" << node->value << " -> ";

        if (target == node->key) {
            oss << "match";
            steps.push_back(oss.str());
            found = true;
            found_value = node->value;
            break;
        } else if (target < node->key) {
            oss << "target < key, go LEFT";
            steps.push_back(oss.str());
            node = node->left;
        } else {
            oss << "target > key, go RIGHT";
            steps.push_back(oss.str());
            node = node->right;
        }
    }

    if (!found && step_no > 0) {
        steps.push_back("Step " + to_string(step_no + 1) +
                        ": reached empty subtree (nullptr) -> not found");
    }

    // ---- Final result line, exactly in the assignment's format ----
    ostringstream result;
    if (found)
        result << target << " = " << target << "/" << found_value;   // found
    else
        result << "-1 != " << target;                                // not found
    steps.push_back(result.str());
}

// -------------------------------------------------------
// Main
// -------------------------------------------------------
int main() {
    // ==============================================================
    // INPUT CONFIGURATION (comment/uncomment; one active block).
    //   *dataset_n.csv
    //   *a found target     (tutor specifies in the code file)
    //   *a not-found target (tutor specifies in the code file)
    // ==============================================================

    string input_file  = "dataset_1000.csv";
    long long target   = 8138274075LL;   // FOUND  (depth 3 in its bucket)
    // long long target = 123456789LL;    // NOT FOUND (only 9 digits, never in dataset)

    // ==============================================================

    vector<Record> data = readCSV(input_file);
    if (data.empty()) {
        cerr << "No data read. Check the filename." << endl;
        return 1;
    }

    // Build the same hash table as hash_table_search.cpp
    long long table_size = nextPrime((long long)data.size());
    HashTable table(table_size);
    for (const auto& r : data)
        table.insert(r.integer_val, r.str_val);

    // Trace the search
    vector<string> steps;
    searchStep(table, target, steps);

    // Output filename: dataset_<n>_hash_table_search_step_<target>.txt
    string token   = extractSizeToken(input_file);
    string out_txt = "dataset_" + token + "_hash_table_search_step_"
                   + to_string(target) + ".txt";

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
