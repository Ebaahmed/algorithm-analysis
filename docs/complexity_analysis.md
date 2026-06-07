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
