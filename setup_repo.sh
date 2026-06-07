#!/bin/bash
# ============================================================
#  CCP6214 Algorithm Analysis — GitHub Repo Setup Script
#  Run this once inside your cloned (empty) repo folder
#  Usage: bash setup_repo.sh
# ============================================================

echo "Setting up ccp6214-algorithm-analysis repository..."

# ── Folders ──────────────────────────────────────────────────
mkdir -p src data output report docs

# Keep empty folders tracked by Git
touch data/.gitkeep
touch output/.gitkeep
touch report/.gitkeep

# ── Placeholder C++ source files ─────────────────────────────
FILES=(
  "dataset_generator"
  "radix_sort"
  "radix_sort_step"
  "heap_sort"
  "heap_sort_step"
  "hash_table_search"
  "hash_table_search_step"
)

MEMBERS=(
  "242UC243BE | EBA MOHAMED ABBAS AHMED    | +60136734355"
  "242UC244SN | MOHAMMAD IEMAN BIN ZAHARI  | +60183756140"
  "242UC243B4 | LAMA M. R. SIAM            | +60183942734"
  "242UC244L3 | HAREIN A/L SATHIAMURTHY    | +60143653986"
)

for FILE in "${FILES[@]}"; do
cat > src/${FILE}.cpp << EOF
// *********************************************************
// Program: ${FILE}.cpp
// Course: CCP6214 Algorithm Design and Analysis
// Lecture Class: TC6L
// Tutorial Class: T22L
// Trimester: 2610
// Member_1: ${MEMBERS[0]}
// Member_2: ${MEMBERS[1]}
// Member_3: ${MEMBERS[2]}
// Member_4: ${MEMBERS[3]}
// *********************************************************

#include <iostream>
using namespace std;

int main() {
    // TODO: implement ${FILE}
    return 0;
}
EOF
done

echo "C++ source files created in src/"

# ── .gitignore ───────────────────────────────────────────────
cat > .gitignore << 'EOF'
# Compiled binaries
*.exe
*.out
*.o
*.a

# Large dataset files (upload to OneDrive instead)
data/dataset_10000.csv
data/dataset_100000.csv
data/dataset_1000000.csv
data/dataset_*.csv
!data/dataset_1000.csv

# OS junk
.DS_Store
Thumbs.db

# IDE folders
.vscode/
.idea/
EOF

echo ".gitignore created"

# ── README.md ────────────────────────────────────────────────
cat > README.md << 'EOF'
# CCP6214 Algorithm Design and Analysis

> **Multimedia University** · Trimester 2610  
> Lecture: TC6L · Tutorial: T22L · Group: G04

## Team

| # | Student ID   | Name                          | Role           |
|---|--------------|-------------------------------|----------------|
| 1 | 242UC243BE   | EBA MOHAMED ABBAS AHMED *(leader)* | Dataset Generator + Radix Sort |
| 2 | 242UC244SN   | MOHAMMAD IEMAN BIN ZAHARI     | Radix Sort Step + Heap Sort    |
| 3 | 242UC243B4   | LAMA M. R. SIAM               | Hash Table Search              |
| 4 | 242UC244L3   | HAREIN A/L SATHIAMURTHY       | Hash Table Search Step + Conclusion |

## Algorithms Implemented

| Algorithm         | File                        | Complexity     |
|-------------------|-----------------------------|----------------|
| Dataset Generator | `dataset_generator.cpp`     | O(n)           |
| Radix Sort        | `radix_sort.cpp`            | O(d·n)         |
| Radix Sort Step   | `radix_sort_step.cpp`       | O(d·n)         |
| Heap Sort         | `heap_sort.cpp`             | O(n log n)     |
| Heap Sort Step    | `heap_sort_step.cpp`        | O(n log n)     |
| Hash Table Search | `hash_table_search.cpp`     | O(1) avg       |
| Hash Table Step   | `hash_table_search_step.cpp`| O(1) avg       |

## Repository Structure

```
ccp6214-algorithm-analysis/
├── src/          ← All C++ source files
├── data/         ← Sample dataset (dataset_1000.csv)
├── output/       ← Step-by-step output .txt files
├── report/       ← Final .docx report
└── docs/         ← Complexity notes and charts
```

## How to Compile & Run

```bash
# Compile any file (example)
g++ src/dataset_generator.cpp -o dataset_generator

# Generate a dataset of 1000 records
./dataset_generator 1000

# Run radix sort on the dataset
./radix_sort dataset_1000.csv
```

## Submission Deadline

**22 June 2026, 11:59 PM** (Week 13)
EOF

echo "README.md created"

# ── docs/complexity_analysis.md ──────────────────────────────
cat > docs/complexity_analysis.md << 'EOF'
# Complexity Analysis Notes

## Radix Sort
- **Time:** O(d · n) where d = 10 (fixed digits), so effectively O(n)
- **Space:** O(n + k) where k = 10 (base-10 buckets)
- Stable sort using counting sort as subroutine

## Heap Sort
- **Time:** O(n log n) — build heap O(n) + n extractions each O(log n)
- **Space:** O(1) — in-place sort
- Uses max-heap; not stable

## Hash Table Search
- **Time:** O(1) average, O(n) worst case (all keys collide)
- **Space:** O(n)
- Collision resolution: chaining

## Radix vs Heap Comparison
At large n, Radix Sort (O(n)) should outperform Heap Sort (O(n log n)).
The two largest input sizes must show a difference of at least 60 seconds.
EOF

echo "docs/complexity_analysis.md created"

# ── Done ─────────────────────────────────────────────────────
echo ""
echo "✅ Repository structure ready! Next steps:"
echo ""
echo "  1. git add ."
echo "  2. git commit -m 'Initial project structure'"
echo "  3. git push origin main"
echo "  4. Go to GitHub → Settings → Collaborators → invite teammates"
echo ""
echo "GitHub usernames to invite (ask teammates for theirs):"
echo "  - MOHAMMAD IEMAN BIN ZAHARI"
echo "  - LAMA M. R. SIAM"
echo "  - HAREIN A/L SATHIAMURTHY"
