# Maxheap - Working Documentation

This document provides **step-by-step implementation** details for the Maxheap class used in BM25 ranking. For conceptual understanding, see `maxheap.md`.

---

## Table of Contents

1. [Complete Source Code](#1-complete-source-code)
2. [Constructor Implementation](#2-constructor-implementation)
3. [insert() Function Breakdown](#3-insert-function-breakdown)
4. [remove() Function Breakdown](#4-remove-function-breakdown)
5. [Helper Functions](#5-helper-functions)
6. [Memory Management](#6-memory-management)
7. [January 2, 2026 Updates](#7-january-2-2026-updates)

---

## 1. Complete Source Code

### Maxheap.hpp (Header File)

```cpp
#include <iostream>
#include <cstdlib>
#include <cstring>
#ifndef MAXHEAP_HPP
#define MAXHEAP_HPP
using namespace std;
class Maxheap
{
    double* heap;
    int *ids;
    int curnumofscores;
    int maxnumofscores;
    void swapscore(int index1,int index2);
    int minindex(int low,int high);
    public:
    Maxheap(int k);
    ~Maxheap(){
        free(heap);
        free(ids);
    }
    void insert(double score, int id);
    int MaxChild(int number1,int number2);
    double remove();
    int get_id(){
        return ids[0];
    }
    int get_count(){
        return curnumofscores;
    }

};
#endif
```

### Maxheap.cpp (Implementation)

```cpp
#include "Maxheap.hpp"
using namespace std;

Maxheap::Maxheap(int k):
    curnumofscores(0),
    maxnumofscores(k)
{
    heap=(double*)malloc(maxnumofscores*sizeof(double));
    ids=(int*)malloc(maxnumofscores*sizeof(int));
    // Initialize to prevent garbage values
    for(int i=0; i<maxnumofscores; i++){
        ids[i] = -1;
        heap[i] = 0.0;
    }
}

int Maxheap::minindex(int low,int high){
    int min=1;
    double minscore=1000000.0;
    for(int i=low;i<high;i++){
        if(heap[i]<minscore){
            min=i;
            minscore=heap[i];
        }
    }
    return min;
}

void Maxheap::swapscore(int index1,int index2){
    double temp=0.0;
    temp=heap[index1];
    int tempid=ids[index1];
    heap[index1]=heap[index2];
    heap[index2]=temp;
    ids[index1]=ids[index2];
    ids[index2]=tempid;
}

void Maxheap::insert(double score,int id){
    int index;
    if(curnumofscores<maxnumofscores){
        index=curnumofscores;
        curnumofscores++;
    }
    else{
        int tempindex=minindex(maxnumofscores/2,maxnumofscores);
        if(score>heap[tempindex]){
            index=tempindex;
        }
        else{
            return ;
        }
    }
    heap[index]=score;
    ids[index]=id;
    while(heap[index]>heap[(index-1)/2]){
        swapscore(index,(index-1)/2);
        index=(index-1)/2;
    }
}

int Maxheap::MaxChild(int number1,int number2){
    if(number1<curnumofscores && number2<curnumofscores){
        if(heap[number1]>heap[number2]){
            return number1;
        }
        else{
            return number2;
        }
    }
    else if(number1<curnumofscores){
        return number1;
    }
    else if(number2<curnumofscores){
        return number2;
    }
    else{
        return -1;
    }
}

double Maxheap::remove(){
    int index=0, chosenchild;
    double Returnvalue=0.0;
    if(curnumofscores>0){
        Returnvalue=heap[0];
        curnumofscores--;
        heap[0]=heap[curnumofscores];
        ids[0]=ids[curnumofscores];
        while(1){
            chosenchild=MaxChild(2*index+1,2*index+2);
            if(chosenchild!=-1 && heap[chosenchild]>heap[index]){
                swapscore(chosenchild,index);
                index=chosenchild;
            }
            else{
                break;
            }
        }
    }
    return Returnvalue;
}
```

---

## 2. Constructor Implementation

```cpp
Maxheap::Maxheap(int k):
    curnumofscores(0),
    maxnumofscores(k)
{
    heap=(double*)malloc(maxnumofscores*sizeof(double));
    ids=(int*)malloc(maxnumofscores*sizeof(int));
    // Initialize to prevent garbage values (Added Jan 2, 2026)
    for(int i=0; i<maxnumofscores; i++){
        ids[i] = -1;
        heap[i] = 0.0;
    }
}
```

### Line-by-Line Breakdown

**Lines 1-3: Initialization List**
```cpp
Maxheap::Maxheap(int k):
    curnumofscores(0),
    maxnumofscores(k)
```
- `k`: Maximum number of results to store (e.g., top 5, top 10)
- `curnumofscores`: Tracks actual number of entries (starts at 0)
- `maxnumofscores`: Capacity of heap

**Lines 5-6: Memory Allocation**
```cpp
heap=(double*)malloc(maxnumofscores*sizeof(double));
ids=(int*)malloc(maxnumofscores*sizeof(int));
```
- Allocates two parallel arrays using `malloc`
- `heap[]`: Stores BM25 scores
- `ids[]`: Stores corresponding document IDs

**Why malloc instead of new?**
- Legacy C-style code
- Works correctly but not modern C++ style
- Paired with `free()` in destructor

**Lines 7-11: Initialization Loop (Jan 2 Fix)**
```cpp
for(int i=0; i<maxnumofscores; i++){
    ids[i] = -1;
    heap[i] = 0.0;
}
```

**Why needed?** 
`malloc` doesn't initialize memory! Contains garbage values from previous usage.

**Bug before fix:**
```
If heap size = 6 but only 1 document scored:
  ids[0] = valid document
  ids[1-5] = GARBAGE! Could be anything!
  
When displaying results, might show:
  Document 5    ✅ (valid)
  Document 1847362  ❌ (garbage!)
```

**After fix:**
```
All unused slots initialized:
  ids[0] = valid
  ids[1-5] = -1 (marked invalid)
  
Can safely check: if(id != -1) → it's valid
```

---

## 3. insert() Function Breakdown

```cpp
void Maxheap::insert(double score,int id){
    int index;
    if(curnumofscores<maxnumofscores){
        index=curnumofscores;
        curnumofscores++;
    }
    else{
        int tempindex=minindex(maxnumofscores/2,maxnumofscores);
        if(score>heap[tempindex]){
            index=tempindex;
        }
        else{
            return ;
        }
    }
    heap[index]=score;
    ids[index]=id;
    while(heap[index]>heap[(index-1)/2]){
        swapscore(index,(index-1)/2);
        index=(index-1)/2;
    }
}
```

### Two Phases of Insertion

**Phase 1: Find Insertion Point**

**Case A: Heap Not Full**
```cpp
if(curnumofscores<maxnumofscores){
    index=curnumofscores;
    curnumofscores++;
}
```
- Simply add to next available slot
- Increment count
- Example: k=5, currently have 3 items → insert at index 3

**Case B: Heap Full**
```cpp
else{
    int tempindex=minindex(maxnumofscores/2,maxnumofscores);
    if(score>heap[tempindex]){
        index=tempindex;
    }
    else{
        return;  // Score too low, reject
    }
}
```

**Why minindex(k/2, k)?**
- Heap property: children indices are `2i+1` and `2i+2`
- Indices k/2 to k-1 are leaf nodes (have no children)
- These are the smallest elements in the heap
- Find minimum among leaves, replace if new score higher

**Example:**
```
Heap (k=5):
Index:  0    1    2    3    4
Score: 3.5  2.1  2.8  1.2  1.5
               ^    ^    ^
             Leaves (indices 2-4)

minindex(2, 5) → returns index 3 (score=1.2)

New score 2.0:
  2.0 > 1.2? YES
  Replace index 3 with new entry
```

**Phase 2: Maintain Heap Property (Bubble Up)**

```cpp
heap[index]=score;
ids[index]=id;
while(heap[index]>heap[(index-1)/2]){
    swapscore(index,(index-1)/2);
    index=(index-1)/2;
}
```

**Heap Property:** Parent >= Children (max-heap)

**Bubble up process:**
```
Insert score=4.0 at index 4:

Before:
       3.5(0)
      /     \
   2.1(1)  2.8(2)
   /   \
1.2(3) 4.0(4)  ← NEW

Step 1: Compare with parent
  4.0 > 2.1? YES → Swap
  
       3.5(0)
      /     \
   4.0(1)  2.8(2)
   /   \
1.2(3) 2.1(4)

Step 2: Compare with parent
  4.0 > 3.5? YES → Swap
  
       4.0(0)
      /     \
   3.5(1)  2.8(2)
   /   \
1.2(3) 2.1(4)

Step 3: At root, STOP ✅
```

---

## 4. remove() Function Breakdown

```cpp
double Maxheap::remove(){
    int index=0, chosenchild;
    double Returnvalue=0.0;
    if(curnumofscores>0){
        Returnvalue=heap[0];
        curnumofscores--;
        heap[0]=heap[curnumofscores];
        ids[0]=ids[curnumofscores];
        while(1){
            chosenchild=MaxChild(2*index+1,2*index+2);
            if(chosenchild!=-1 && heap[chosenchild]>heap[index]){
                swapscore(chosenchild,index);
                index=chosenchild;
            }
            else{
                break;
            }
        }
    }
    return Returnvalue;
}
```

### Removal Process (Bubble Down)

**Step 1: Save Maximum**
```cpp
Returnvalue=heap[0];  // Top element = highest score
```

**Step 2: Replace Root with Last Element**
```cpp
curnumofscores--;
heap[0]=heap[curnumofscores];
ids[0]=ids[curnumofscores];
```

**Example:**
```
Before remove:
       4.0(0)
      /     \
   3.5(1)  2.8(2)
   /   \
1.2(3) 2.1(4)

After replacing root with last:
       2.1(0)  ← Moved from index 4
      /     \
   3.5(1)  2.8(2)
   /
1.2(3)

curnumofscores: 5 → 4
```

**Step 3: Restore Heap Property (Bubble Down)**
```cpp
while(1){
    chosenchild=MaxChild(2*index+1,2*index+2);
    if(chosenchild!=-1 && heap[chosenchild]>heap[index]){
        swapscore(chosenchild,index);
        index=chosenchild;
    }
    else{
        break;
    }
}
```

**Bubble down example:**
```
Current:
       2.1(0)
      /     \
   3.5(1)  2.8(2)
   /
1.2(3)

Step 1: Find max child
  MaxChild(1, 2)
  3.5 > 2.8 → index 1

Step 2: Compare and swap
  3.5 > 2.1? YES → Swap
  
       3.5(0)
      /     \
   2.1(1)  2.8(2)
   /
1.2(3)

Step 3: Continue at index 1
  MaxChild(3, 4)
  Only child 3 exists
  1.2 > 2.1? NO → STOP ✅
```

---

## 5. Helper Functions

### get_id()

```cpp
int get_id(){
    return ids[0];
}
```

**Purpose:** Get document ID of top result (highest score)
**Usage:** Called before `remove()` to extract winner

### get_count() (Added Jan 2, 2026)

```cpp
int get_count(){
    return curnumofscores;
}
```

**Purpose:** Return actual number of valid entries
**Why needed:** Display loop needs to know how many results exist

**Before fix:**
```cpp
// Used resultCount (total documents processed)
for(int j = 0; j < resultCount; j++)  // ❌ Wrong!
```

**After fix:**
```cpp
// Use heap->get_count() (actual entries in heap)
for(int j = 0; j < heap->get_count(); j++)  // ✅ Correct!
```

---

## 6. Memory Management

### Destructor

```cpp
~Maxheap(){
    free(heap);
    free(ids);
}
```

**Cleanup:**
- Frees both dynamically allocated arrays
- Called automatically when Maxheap object deleted
- Paired with `malloc()` in constructor

**Important:** In Search.cpp, must call `delete heap;` to trigger destructor!

---

## 7. January 2, 2026 Updates

### Bug Fix: Uninitialized Memory

**Problem:**
```cpp
// Old constructor
Maxheap::Maxheap(int k){
    heap=(double*)malloc(...);
    ids=(int*)malloc(...);
    // ❌ No initialization! Garbage values!
}
```

**Impact:**
When displaying results with fewer entries than capacity:
```
k=6, but only 1 document matched
ids[0] = 5    (valid)
ids[1] = ???  (garbage)
ids[2] = ???  (garbage)
...

Display loop (using wrong count):
  Document 5    ✅
  Document -1   ❌ or random number
```

**Solution:**
```cpp
for(int i=0; i<maxnumofscores; i++){
    ids[i] = -1;     // Mark as invalid
    heap[i] = 0.0;   // Zero score
}
```

**Result:**
```
k=6, 1 document matched
ids[0] = 5    (valid)
ids[1] = -1   (invalid marker)
ids[2] = -1   (invalid marker)
...

Can now check: if(id != -1) → valid entry
```

### Enhancement: get_count() Method

**Added:** Public method to expose `curnumofscores`

**Why needed:**
```cpp
// OLD approach (WRONG):
int resultCount = 0;  // Count all documents processed
while(currentDoc!=NULL){
    heap->insert(...);
    resultCount++;
}
// Problem: resultCount might be > actual heap size!

// NEW approach (CORRECT):
int actualResults = heap->get_count();
for(int j = 0; j < actualResults; j++){
    // Process only valid entries
}
```

### Testing Results

**Test 1: Full heap**
```
k=5, 10 documents matched
Expected: Display 5 results
Before fix: ✅ Works (heap full)
After fix: ✅ Still works
```

**Test 2: Partial heap**
```
k=5, 1 document matched
Expected: Display 1 result
Before fix: ❌ Shows Document -1 (4 garbage entries)
After fix: ✅ Shows only 1 valid document
```

**Test 3: Empty query**
```
k=5, 0 documents matched
Expected: "No documents found"
Before fix: ❌ Might show garbage
After fix: ✅ Correctly shows message
```

---

## Complete Usage Example

```cpp
// Create heap for top 5 results
Maxheap* heap = new Maxheap(5);

// Insert 3 documents with scores
heap->insert(3.5, 1);  // Doc 1, score 3.5
heap->insert(2.1, 3);  // Doc 3, score 2.1
heap->insert(4.2, 5);  // Doc 5, score 4.2

// Get count of valid entries
int count = heap->get_count();  // Returns 3

// Extract results in order (highest first)
for(int i=0; i<count; i++){
    int docId = heap->get_id();     // Get top document
    cout << "Document " << docId << endl;
    heap->remove();                  // Remove it, next becomes top
}

// Output:
// Document 5  (score 4.2)
// Document 1  (score 3.5)
// Document 3  (score 2.1)

// Cleanup
delete heap;  // Calls destructor, frees memory
```

---

**Document Version**: 1.0  
**Created**: January 2, 2026  
**Purpose**: Document maxheap implementation for BM25 ranking  
**Bug Fixes**: Uninitialized memory fixed, get_count() method added  
**Author**: High-Performance Search Engine Project  
**Status**: Fully operational and bug-free ✅
