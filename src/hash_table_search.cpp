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
//   SOURCE file  -> build the hash table (n records)
//   TARGET file  -> keys to search for (same size n)
//
//   For each dataset size n:
//     - Builds hash table from SOURCE (dataset_n.csv)
//     - Searches every key from TARGET (dataset_n.csv is both source and target)
//     - Performs n searches per case (single search too fast to measure)
//     - Writes hash_table_search_dataset_n.txt:
//         Best case time: x seconds
//         Average case time: y seconds
//         Worst case time: z seconds
// *********************************************************

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <climits>
#include <chrono>

using namespace std;
using namespace chrono;

// -------------------------------------------------------
// Record: 10-digit integer key + 5-letter string
// -------------------------------------------------------
struct Record {
    long long integer_val;
    string    str_val;
};

// -------------------------------------------------------
// AVL tree node
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

static int ht(AVLNode* n) { return n ? n->height : 0; }
static void upd(AVLNode* n) { int l=ht(n->left),r=ht(n->right); n->height=1+(l>r?l:r); }

static AVLNode* rotR(AVLNode* y) {
    AVLNode* x=y->left; y->left=x->right; x->right=y; upd(y); upd(x); return x;
}
static AVLNode* rotL(AVLNode* x) {
    AVLNode* y=x->right; x->right=y->left; y->left=x; upd(x); upd(y); return y;
}
static AVLNode* ins(AVLNode* n, long long k, const string& v) {
    if (!n) return new AVLNode(k,v);
    if (k<n->key) n->left=ins(n->left,k,v);
    else if (k>n->key) n->right=ins(n->right,k,v);
    else { n->value=v; return n; }
    upd(n);
    int bf=ht(n->left)-ht(n->right);
    if (bf> 1&&k<n->left->key)  return rotR(n);
    if (bf<-1&&k>n->right->key) return rotL(n);
    if (bf> 1&&k>n->left->key)  { n->left=rotL(n->left);  return rotR(n); }
    if (bf<-1&&k<n->right->key) { n->right=rotR(n->right); return rotL(n); }
    return n;
}
static bool srch(AVLNode* n, long long k, string& out) {
    while(n) {
        if(k==n->key){out=n->value;return true;}
        n=(k<n->key)?n->left:n->right;
    }
    return false;
}
static int depth(AVLNode* n, long long k) {
    int d=0;
    while(n){++d;if(k==n->key)return d;n=(k<n->key)?n->left:n->right;}
    return -1;
}
static void destroy(AVLNode* n){if(!n)return;destroy(n->left);destroy(n->right);delete n;}

// -------------------------------------------------------
// Hash table with AVL chaining
// -------------------------------------------------------
class HashTable {
public:
    explicit HashTable(long long sz): size_(sz), buckets_(sz,nullptr){}
    ~HashTable(){for(auto* b:buckets_)destroy(b);}
    long long hash(long long k)const{return(k%size_+size_)%size_;}
    void insert(long long k,const string& v){long long h=hash(k);buckets_[h]=ins(buckets_[h],k,v);}
    bool search(long long k,string& out)const{return srch(buckets_[hash(k)],k,out);}
    int searchDepth(long long k)const{return depth(buckets_[hash(k)],k);}
private:
    long long size_;
    vector<AVLNode*> buckets_;
};

static bool isPrime(long long x){
    if(x<2)return false;if(x%2==0)return x==2;
    for(long long i=3;i*i<=x;i+=2)if(x%i==0)return false;return true;
}
static long long nextPrime(long long n){long long c=n<2?2:n;while(!isPrime(c))++c;return c;}

// -------------------------------------------------------
// Read CSV (I/O not timed)
// -------------------------------------------------------
static vector<Record> readCSV(const string& fn){
    vector<Record> data; ifstream f(fn);
    if(!f.is_open()){cerr<<"Cannot open "<<fn<<endl;return data;}
    string line;
    while(getline(f,line)){
        while(!line.empty()&&(line.back()=='\r'||line.back()=='\n'||line.back()==' '))line.pop_back();
        if(line.empty()||line[0]=='#'||line[0]=='R')continue;
        size_t c=line.find(',');if(c==string::npos)continue;
        Record r;
        try{r.integer_val=stoll(line.substr(0,c));}catch(...){continue;}
        r.str_val=line.substr(c+1);data.push_back(r);
    }
    return data;
}

static string sizeToken(const string& fn){
    string b=fn;
    size_t s=b.find_last_of("/\\");if(s!=string::npos)b=b.substr(s+1);
    size_t d=b.rfind('.');if(d!=string::npos)b=b.substr(0,d);
    size_t u=b.rfind('_');if(u!=string::npos)return b.substr(u+1);
    return b;
}

// -------------------------------------------------------
// Process one source+target pair
// SOURCE: file that fills the hash table
// TARGET: file whose every key is searched (n searches per case)
// -------------------------------------------------------
static void process(const string& sourcePath, const string& targetPath){
    cout << "\n--- Processing: " << sourcePath << " ---" << endl;

    // Read both files (I/O not timed)
    vector<Record> source = readCSV(sourcePath);
    vector<Record> target = readCSV(targetPath);

    if(source.empty()||target.empty()){cerr<<"  Empty file. Skipping.\n";return;}

    long long n = (long long)source.size(); // n = dataset size = number of searches
    long long m = (long long)target.size();

    cout << "  Source size: " << n << "  |  Target size: " << m << endl;

    // Build hash table from SOURCE (not timed)
    HashTable table(nextPrime(n));
    for(const auto& r : source)
        table.insert(r.integer_val, r.str_val);

    // Classify target keys by AVL depth -> best and worst case pools
    int minD=INT_MAX, maxD=0;
    for(const auto& r : target){
        int d=table.searchDepth(r.integer_val);
        if(d==-1)d=maxD;
        if(d<minD)minD=d;
        if(d>maxD)maxD=d;
    }
    if(minD==INT_MAX)minD=1;

    vector<Record> bestPool, worstPool;
    for(const auto& r : target){
        int d=table.searchDepth(r.integer_val);
        if(d==-1)d=maxD;
        if(d==minD)bestPool.push_back(r);
        if(d==maxD)worstPool.push_back(r);
    }
    if(bestPool.empty())  bestPool.push_back(target[0]);
    if(worstPool.empty()) worstPool.push_back(target[m-1]);

    size_t bsz=bestPool.size(), wsz=worstPool.size();
    string val;
    volatile bool guard=false;

    // ----------------------------------------------------------
    // BEST CASE: n searches on shallowest keys (depth = minD)
    // ----------------------------------------------------------
    auto b0=high_resolution_clock::now();
    for(long long i=0;i<n;++i)
        guard^=table.search(bestPool[i%bsz].integer_val, val);
    auto b1=high_resolution_clock::now();
    double bestTime=duration<double>(b1-b0).count()/(double)n;

    // ----------------------------------------------------------
    // AVERAGE CASE: n searches cycling through ALL target keys
    // ----------------------------------------------------------
    auto a0=high_resolution_clock::now();
    for(long long i=0;i<n;++i)
        guard^=table.search(target[i%m].integer_val, val);
    auto a1=high_resolution_clock::now();
    double avgTime=duration<double>(a1-a0).count()/(double)n;

    // ----------------------------------------------------------
    // WORST CASE: n searches on deepest keys (depth = maxD)
    // ----------------------------------------------------------
    auto w0=high_resolution_clock::now();
    for(long long i=0;i<n;++i)
        guard^=table.search(worstPool[i%wsz].integer_val, val);
    auto w1=high_resolution_clock::now();
    double worstTime=duration<double>(w1-w0).count()/(double)n;

    (void)guard;

    // Print to console
    cout.setf(ios::scientific);
    cout << "  Best case time:    " << bestTime  << " seconds (depth " << minD << ")" << endl;
    cout << "  Average case time: " << avgTime   << " seconds" << endl;
    cout << "  Worst case time:   " << worstTime << " seconds (depth " << maxD << ")" << endl;

    // Write output file: hash_table_search_dataset_n.txt
    // Lists ALL n search results then timing at the bottom
    string outFile = "hash_table_search_dataset_" + sizeToken(sourcePath) + ".txt";
    ofstream fout(outFile);
    if(fout.is_open()){
        fout.setf(ios::scientific);

        // Write all n search results (searching every element in target)
        for(long long i = 0; i < n; ++i){
            const Record& r = target[i % m];
            long long idx = table.hash(r.integer_val);
            bool found = table.search(r.integer_val, val);
            fout << r.integer_val << "," << (found ? "yes" : "no") << "," << idx << "\n";
        }

        // Timing at the bottom
        fout << "Best case time: "    << bestTime  << " seconds\n";
        fout << "Average case time: " << avgTime   << " seconds\n";
        fout << "Worst case time: "   << worstTime << " seconds\n";
        fout.close();
        cout << "  Written: " << outFile << endl;
    }
}

// -------------------------------------------------------
// main
// SOURCE = dataset_n.csv  (hash table is built from this)
// TARGET = dataset_n.csv  (every key in this file is searched)
// Both are the same file — every element searches itself,
// giving n searches where n = dataset size, as required.
// -------------------------------------------------------
int main(){
    // SOURCE and TARGET are the same file for each size.
    // The hash table is built from source, then all n target
    // keys are searched -> n searches per case, per the spec.
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

    for(const auto& ds : datasets)
        process(ds, ds);  // source and target = same file -> n searches

    cout << "\nDone." << endl;
    return 0;
}