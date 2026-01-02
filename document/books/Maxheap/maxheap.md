# Maxheap - Conceptual Documentation

This document explains **WHAT** a max heap is, **WHY** we use it, and **ALTERNATIVES**. For implementation details, see `working.md`.

---

## Table of Contents

1. [What is a Max Heap?](#1-what-is-a-max-heap)
2. [Why Use Max Heap for Search Results?](#2-why-use-max-heap-for-search-results)
3. [Heap vs Other Data Structures](#3-heap-vs-other-data-structures)
4. [malloc vs new](#4-malloc-vs-new)
5. [Time Complexity Analysis](#5-time-complexity-analysis)
6. [Design Decisions Q&A](#6-design-decisions-qa)

---

## 1. What is a Max Heap?

### Simple Definition

A **max heap** is a **complete binary tree** where every parent node has a value **greater than or equal to** its children.

### Visual Representation

```
        9 (root = maximum)
       / \
      7   8
     / \
    3   5

Heap property: parent >= children
9 >= 7, 9 >= 8
7 >= 3, 7 >= 5
8 has no children
```

### Array Storage

**Binary tree stored in array:**
```
Index:  0   1   2   3   4
Value: [9] [7] [8] [3] [5]

Relationships:
  Parent of index i:     (i-1)/2
  Left child of i:       2*i + 1
  Right child of i:      2*i + 2

Example:
  Node at index 1 (value=7):
    Parent: (1-1)/2 = 0 (value=9) ✅
    Left:   2*1+1 = 3 (value=3) ✅
    Right:  2*1+2 = 4 (value=5) ✅
```

### Heap Properties

1. **Shape property**: Complete binary tree (filled left-to-right)
2. **Heap property**: Parent >= children (max heap)
3. **Root property**: Maximum element always at root (index 0)

### Operations

**Insert:**
1. Add element at end
2. Bubble up until heap property restored
3. Time: O(log n)

**Remove (extract max):**
1. Save root (maximum)
2. Replace root with last element
3. Bubble down until heap property restored
4. Time: O(log n)

**Peek (get max):**
- Return root value
- Time: O(1)

---

## 2. Why Use Max Heap for Search Results?

### Problem: Top-K Document Ranking

**Scenario:**
```
Search query: "machine learning"
100 documents match

User wants: Top 5 documents (k=5)

Naive approach:
  1. Score all 100 documents
  2. Sort all 100 by score
  3. Take first 5
  
Time: O(n log n) where n=100
```

### Max Heap Approach

```
Heap approach:
  1. Create heap of size k=5
  2. For each document:
     - Calculate score
     - If heap not full, insert
     - If heap full and score > minimum:
       - Remove minimum
       - Insert new score
  3. Extract k elements from heap
  
Time: O(n log k) where n=100, k=5
```

### Why This Is Better

**Comparison:**

| Approach | Time | Space | Notes |
|----------|------|-------|-------|
| **Sort all** | O(n log n) | O(n) | 100 log 100 ≈ 664 ops |
| **Heap (k=5)** | O(n log k) | O(k) | 100 log 5 ≈ 232 ops |
| **Savings** | **2.86x faster!** | **20x less space** | k=5 vs n=100 |

**Real-world example:**
```
1,000,000 documents, want top 10:

Sort: 1M log 1M = 19,931,568 operations
Heap: 1M log 10 = 3,321,928 operations

Heap is 6x faster! ✅
```

### Additional Benefits

1. **Space efficient**: Only store k items, not all n
2. **Incremental**: Can stop processing early if needed
3. **Online**: Can add documents dynamically
4. **Simple**: Clear API (insert, remove, peek)

---

## 3. Heap vs Other Data Structures

### Q: Why Not Just Sort?

**Sorting approach:**
```cpp
struct Result {
    double score;
    int docId;
};
Result results[100];
// Fill results...
sort(results, results+100, [](Result a, Result b){
    return a.score > b.score;
});
// Take first k
```

**Disadvantages:**
❌ Must store ALL n documents
❌ O(n log n) time (slower for large n)
❌ Must process all documents before returning results
❌ Wastes memory on low-scoring documents

**Max heap approach:**
```cpp
Maxheap heap(k);
for each document:
    heap.insert(score, docId);  // Automatically keeps top-k
```

**Advantages:**
✅ Only store k documents
✅ O(n log k) time (faster when k << n)
✅ Results available immediately
✅ Minimal memory footprint

### Q: Why Not Use std::priority_queue?

**C++ STL priority_queue:**
```cpp
#include <queue>
std::priority_queue<pair<double, int>> pq;
pq.push({score, docId});
```

**Advantages:**
✅ Standard library (well-tested)
✅ Generic (works with any type)
✅ Exception-safe
✅ Modern C++ style

**Why we DON'T use it:**
❌ **Project constraint**: No STL allowed
❌ **Educational goal**: Learn by implementing
❌ **Simplicity**: Custom version has only features we need
❌ **Control**: Full control over memory and behavior

**In production code:** Use std::priority_queue!

### Q: Why Not Use Min Heap?

**Our max heap:**
```
Root = maximum score
Extract → Get highest score first
```

**Min heap alternative:**
```
Root = minimum score
Need to extract n-k times to get top-k
```

**Problem with min heap for top-k:**
```
Want top 5 from 100 documents

Min heap approach:
  1. Insert all 100
  2. Remove 95 smallest
  3. Remaining 5 are top-k
  
Operations: 100 inserts + 95 removes = 195 ops

Max heap approach:
  1. Maintain heap of size 5
  2. Only replace if new > minimum
  
Operations: ~100 comparisons, ~20 inserts = 120 ops
```

**Max heap is simpler for top-k!**

### Q: Why Not Use Array + Linear Search?

**Array approach:**
```cpp
double scores[5];
int ids[5];
int count = 0;

// Insert new score
if(count < 5){
    scores[count] = score;
    ids[count] = docId;
    count++;
}
else{
    // Find minimum
    int minIdx = 0;
    for(int i=1; i<5; i++){
        if(scores[i] < scores[minIdx])
            minIdx = i;
    }
    // Replace if new > min
    if(score > scores[minIdx]){
        scores[minIdx] = score;
        ids[minIdx] = docId;
    }
}
```

**Disadvantages:**
❌ O(k) to find minimum each time
❌ O(k) to extract in sorted order
❌ Total: O(n*k) insertion + O(k²) extraction

**Heap approach:**
❌ O(log k) to insert
❌ O(log k) to extract
❌ Total: O(n log k) + O(k log k)

**Example (n=100, k=5):**
```
Array: 100*5 + 5*5 = 525 operations
Heap:  100*2.3 + 5*2.3 = 241 operations

Heap is 2x faster! ✅
```

---

## 4. malloc vs new

### Q: Why Does Maxheap Use malloc Instead of new?

**Our implementation:**
```cpp
heap = (double*)malloc(maxnumofscores * sizeof(double));
ids = (int*)malloc(maxnumofscores * sizeof(int));
```

**Why malloc?**
1. **Raw arrays**: Just need contiguous memory blocks
2. **C-style arrays**: Plain Old Data (POD) types
3. **Performance**: Slightly faster (no constructor calls)
4. **Legacy code**: Original implementation style

**Why new would be better:**
```cpp
heap = new double[maxnumofscores];
ids = new int[maxnumofscores];
```

**Advantages of new:**
✅ Type-safe (no casting)
✅ Automatically initializes (for classes)
✅ Modern C++ style
✅ Pairs with delete[] (clearer intent)

### Detailed Comparison

| Feature | malloc/free | new/delete |
|---------|-------------|------------|
| **Language** | C | C++ |
| **Type safety** | Manual cast | Automatic |
| **Constructors** | No | Yes |
| **Destructors** | No | Yes |
| **Syntax** | malloc(size) | new Type[n] |
| **Cleanup** | free(ptr) | delete[] ptr |
| **Failure** | Returns NULL | Throws exception |
| **Usage** | POD types | All types |

### When to Use Each

**Use malloc/free:**
- Plain Old Data (int, double, char)
- C library integration
- Performance-critical raw buffers
- Legacy C code

**Use new/delete:**
- C++ objects (with constructors/destructors)
- Modern C++ code
- Exception handling needed
- Better type safety wanted

**Our case:**
- Storing doubles and ints (POD)
- malloc acceptable ✅
- new would be more idiomatic ✅✅

### Modern C++ Alternative

**Best practice (C++11+):**
```cpp
#include <vector>
std::vector<double> heap;
std::vector<int> ids;

heap.resize(maxnumofscores);
ids.resize(maxnumofscores);

// Automatic cleanup, exception-safe, modern!
```

**Why we don't use it:**
❌ No STL constraint
✅ Learning exercise

---

## 5. Time Complexity Analysis

### Operations Overview

| Operation | Best Case | Average Case | Worst Case | Space |
|-----------|-----------|--------------|------------|-------|
| **Insert** | O(1) | O(log k) | O(log k) | O(k) |
| **Remove** | O(1) | O(log k) | O(log k) | O(k) |
| **Peek** | O(1) | O(1) | O(1) | O(k) |
| **Build heap** | - | O(k) | O(k) | O(k) |

### Insert Analysis

**Best case: O(1)**
```
Insert 1 into heap [5, 3, 2]
New element is smallest → Stays at bottom
No swaps needed
```

**Average case: O(log k)**
```
Heap height: h = log₂(k)
Average swaps: h/2 ≈ log(k)/2
Example k=8: Height=3, avg swaps≈1.5
```

**Worst case: O(log k)**
```
Insert 10 into heap [5, 3, 2]
New element is largest → Bubbles to root
Swaps: log₂(k)
Example k=8: 3 swaps maximum
```

### Remove Analysis

**Always O(log k)**
```
1. Copy root → O(1)
2. Replace root with last → O(1)
3. Bubble down → O(log k)
Total: O(log k)
```

**Bubble down swaps:**
```
Heap height: log₂(k)
Worst case: Element sinks to bottom
Swaps: log₂(k)
```

### Overall Top-K Algorithm

**Given n documents, find top k:**

```cpp
Maxheap heap(k);
for(int i=0; i<n; i++){         // n iterations
    heap.insert(score, id);      // O(log k)
}
for(int i=0; i<k; i++){         // k iterations
    int id = heap.get_id();
    heap.remove();               // O(log k)
}
```

**Time complexity:**
```
Insertion phase: n × O(log k) = O(n log k)
Extraction phase: k × O(log k) = O(k log k)
Total: O(n log k) + O(k log k) = O(n log k)
  (since k << n typically)
```

**Space complexity:**
```
Heap storage: O(k)
Two arrays (scores + ids): 2k × size
Example k=5: ~80 bytes total
```

### Comparison with Alternatives

**For n=1000 documents, k=10 results:**

| Method | Time Complexity | Operations | Space |
|--------|----------------|------------|-------|
| **Full sort** | O(n log n) | 9,965 | O(n) |
| **Partial sort** | O(n log k) | 3,321 | O(n) |
| **Max heap** | O(n log k) | 3,321 | O(k) |
| **Linear scan** | O(nk) | 10,000 | O(k) |

**Winner: Max heap** (fast + space-efficient)

---

## 6. Design Decisions Q&A

### Q: Why Fixed Size Heap (k) Instead of Growing?

**Our design:**
```cpp
Maxheap(int k)  // Fixed capacity
```

**Alternative: Growing heap**
```cpp
Maxheap()  // Unlimited capacity, grows as needed
```

**Why fixed size:**
1. **Top-k problem**: Only need k results
2. **Memory bounded**: Know exact memory usage
3. **Simpler code**: No resizing logic
4. **Performance**: No reallocation overhead
5. **Clear intent**: "Give me top 5" → heap of 5

**Trade-off:** Less flexible, but perfect for our use case.

### Q: Why Store Both Scores and IDs?

**Our approach:**
```cpp
double* heap;   // Scores
int* ids;       // Document IDs
```

**Alternative: Store pairs**
```cpp
struct ScorePair {
    double score;
    int id;
};
ScorePair* heap;
```

**Our choice:**
✅ Separate arrays → Better cache locality when comparing scores
✅ Simpler swap logic (swap each independently)
✅ Clearer intent (scores drive ordering, IDs are payload)

**Alternative choice:**
✅ Single allocation
✅ Data bundled together
✅ More object-oriented

**Both valid!** We chose separate for simplicity.

### Q: Why Recursive Destructor for Scorelist but Not Heap?

**Scorelist (linked list):**
```cpp
~Scorelist(){
    if(next!=NULL) delete(next);  // Recursive
}
```

**Maxheap (arrays):**
```cpp
~Maxheap(){
    free(heap);  // Direct
    free(ids);
}
```

**Reason:**
- Linked list: Chain of objects → Must delete each
- Arrays: Single contiguous block → Single free

**Different data structures → Different cleanup strategies**

### Q: Why Not Heapify (Build Heap Efficiently)?

**Heapify algorithm:**
```cpp
// Build heap from array in O(n) time
for(int i=n/2-1; i>=0; i--){
    bubble_down(i);
}
```

**Why we don't use it:**
1. We insert incrementally (one score at a time)
2. Don't have all scores upfront
3. O(n log k) insertion fine for our use case
4. Simpler to understand and maintain

**When heapify is useful:**
- Have array of elements to turn into heap
- Batch insertion (all at once)
- O(n) vs O(n log n) matters

**Our use case: Incremental insertion → No heapify needed**

### Q: What Happens on Tie Scores?

**Scenario:**
```
Document 3: score = 5.0
Document 7: score = 5.0
Both have same score!
```

**Our behavior:**
- Heap compares scores only
- Ties broken by insertion order
- First inserted stays higher in heap

**Alternative approaches:**
1. **Secondary sort by docId**
   ```cpp
   if(score1 == score2)
       return id1 < id2;
   ```

2. **Stable heap**
   - Track insertion time
   - Maintain FIFO for ties

**Our choice:** Simplest (no tie-breaking)
**Rationale:** Rare in practice (BM25 scores rarely identical)

---

## Summary

### Max Heap Characteristics

**Structure:**
- Complete binary tree (array representation)
- Parent >= children (max heap property)
- Root = maximum element

**Time Complexity:**
- Insert: O(log k)
- Remove: O(log k)
- Peek: O(1)
- Top-k from n: O(n log k)

**Space Complexity:**
- O(k) - only store top k results

**Advantages:**
✅ Fast top-k retrieval
✅ Memory efficient
✅ Simple API
✅ Industry standard for ranking

**Design Choices:**
- Fixed size k (bounded memory)
- malloc for raw arrays (C-style)
- Separate score/ID arrays (cache-friendly)
- No heapify (incremental insertion)

**Use Cases:**
- Search result ranking
- Top-N queries
- Priority queues
- Event scheduling
- Graph algorithms (Dijkstra)

**Alternatives:**
- std::priority_queue (if STL allowed)
- Sorting (if k ≈ n)
- Linear scan (if k very small)

---

**Document Version**: 1.0  
**Created**: January 2, 2026  
**Purpose**: Explain conceptual design of max heap for search ranking  
**Bugs Fixed**: Uninitialized memory (Jan 2), added get_count() method  
**Author**: High-Performance Search Engine Project
