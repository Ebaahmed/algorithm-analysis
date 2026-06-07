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
