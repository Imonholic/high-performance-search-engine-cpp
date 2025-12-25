# Trie Implementation - Code Explanation

This document provides a **detailed step-by-step explanation** of the `trie.cpp` file implementation. It explains how the code works, what each line does, and the execution flow.

---

## File Information

**File Name**: `trie.cpp`  
**Location**: `src/trie.cpp`  
**Purpose**: Implements the TrieNode class for efficient word storage and future search functionality  
**Header File**: `header/Trie.hpp`

---

## Table of Contents

1. [Complete Source Code](#complete-source-code)
2. [Header Dependencies](#header-dependencies)
3. [Constructor Implementation](#constructor-implementation)
4. [Destructor Implementation](#destructor-implementation)
5. [Insert Function - Detailed Breakdown](#insert-function---detailed-breakdown)
6. [Execution Flow Examples](#execution-flow-examples)
7. [Memory Management](#memory-management)
8. [December 26 Updates](#december-26-updates)

---

## Complete Source Code

```cpp
#include "Trie.hpp"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

TrieNode::TrieNode():value(-1), sibling(nullptr), child(nullptr)
{
    //list = nullptr;
};

TrieNode::~TrieNode()
{
    // if(list!=nullptr){
    //     delete list;
    // }
    if(sibling!=nullptr){
        delete sibling;
    }
    if(child!=nullptr){
        delete child;
    }

};

void TrieNode::insert(char* token, int id){
    if(value ==-1 || value ==token[0]){
        value = token[0];
        if(strlen(token)==1){
            //initialise list
            return;
        }
        else{
            if(child == nullptr){
                child = new TrieNode();
            }
            child->insert(token+1,id);
        }
    }
    else{
        if(sibling==nullptr){
            sibling=new TrieNode();
        }
        sibling->insert(token,id);
    }
    return ;
}
```

---

## Header Dependencies

### Include Files

```cpp
#include "Trie.hpp"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
```

**Line-by-Line:**

**Line 1:** `#include "Trie.hpp"`
- Includes class declaration
- Provides TrieNode class definition

**Line 2:** `#include <iostream>`
- Currently unused in this file
- May be used for future debug output

**Line 3:** `#include <string>`
- Provides `strlen()` function
- String manipulation utilities

**Line 4:** `#include <fstream>`
- Currently unused
- May be needed for future file operations

**Line 5:** `using namespace std;`
- Allows using `cout`, `endl` without `std::` prefix

---

## Constructor Implementation

### Constructor Code

```cpp
TrieNode::TrieNode():value(-1), sibling(nullptr), child(nullptr)
{
    //list = nullptr;
};
```

### Line-by-Line Breakdown

**Signature:** `TrieNode::TrieNode()`
- No parameters - default constructor
- Creates a new, empty TrieNode

**Member Initializer List:**
```cpp
:value(-1), sibling(nullptr), child(nullptr)
```

This initializes member variables **before** the constructor body executes.

**Why this matters:**
- More efficient than assignment in constructor body
- Required for const members and references
- Better performance

**Initialization Details:**

**1. value(-1)**
- Sets character value to -1
- Special marker meaning "empty node"
- Root node and uninitialized nodes have value = -1

**Why -1?**
- char can hold -128 to 127 (signed)
- -1 is not a valid printable character
- Easy to check: `if(value == -1)` means empty
- Once set to actual character ('a'-'z'), value is positive

**2. sibling(nullptr)**
- Initializes sibling pointer to null
- Means no alternative characters at this level yet
- Will be created when needed during insert

**3. child(nullptr)**
- Initializes child pointer to null
- Means no next character in sequence yet
- Will be created when needed during insert

**Commented Out Code:**
```cpp
//list = nullptr;
```
- This is for future listnode integration
- Will store document IDs and frequencies
- Currently commented because not implemented yet

### Visual Representation

**New node created:**
```
+----------------+
| value: -1      |  ← Empty marker
| sibling: null  |  ← No alternatives
| child: null    |  ← No next char
| list: (future) |  ← Will track documents
+----------------+
```

---

## Destructor Implementation

### Destructor Code

```cpp
TrieNode::~TrieNode()
{
    // if(list!=nullptr){
    //     delete list;
    // }
    if(sibling!=nullptr){
        delete sibling;
    }
    if(child!=nullptr){
        delete child;
    }
};
```

### How It Works

The destructor is **recursive** - deleting one node triggers deletion of all connected nodes.

**Deletion Order:**
1. Check if sibling exists → delete sibling (recursive)
2. Check if child exists → delete child (recursive)
3. Current node gets deleted automatically

**Why This Order?**
- Siblings and children must be deleted before parent
- `delete sibling` calls sibling's destructor
- Sibling's destructor deletes ITS siblings and children
- Creates a chain reaction deleting entire subtree

### Visual Example

**Deleting this structure:**
```
    Root
     ↓
     h
     ↓
   e → w
   ↓   ↓
   l   o
```

**Execution Flow:**
```
1. delete Root
   → Root destructor checks child
   → delete h
   
2. delete h
   → h destructor checks child  
   → delete e
   
3. delete e
   → e destructor checks child
   → delete l
   → e destructor checks sibling
   → delete w
   
4. delete l
   → l has no children or siblings
   → l deleted
   
5. delete w
   → w destructor checks child
   → delete o
   
6. delete o
   → o has no children or siblings
   → o deleted

All nodes deleted recursively!
```

### Commented Out Code

```cpp
// if(list!=nullptr){
//     delete list;
// }
```

**Future Implementation:**
- Will delete the listnode chain
- listnode destructor is recursive too
- Will free all document tracking data

**Why Commented:**
- list member not yet added to class
- Will uncomment when search features implemented

### Memory Safety

✅ **NULL Check Before Delete:**
```cpp
if(sibling != nullptr){
    delete sibling;  // Only delete if exists
}
```

**Why check?**
- `delete nullptr` is safe in C++, but explicit check is clearer
- Shows intentional design
- Makes code more readable

✅ **No Memory Leaks:**
- All dynamically allocated nodes get freed
- Recursive deletion ensures no orphaned nodes
- Proper RAII (Resource Acquisition Is Initialization)

---

## Insert Function - Detailed Breakdown

### Function Signature

```cpp
void TrieNode::insert(char* token, int id)
```

**Parameters:**
- `token` - Pointer to the word (or remaining part of word)
- `id` - Document ID where this word appears

**Return:** void (modifies tree structure)

### Complete Function

```cpp
void TrieNode::insert(char* token, int id){
    if(value ==-1 || value ==token[0]){
        value = token[0];
        if(strlen(token)==1){
            //initialise list
            return;
        }
        else{
            if(child == nullptr){
                child = new TrieNode();
            }
            child->insert(token+1,id);
        }
    }
    else{
        if(sibling==nullptr){
            sibling=new TrieNode();
        }
        sibling->insert(token,id);
    }
    return ;
}
```

### Logic Flow - Decision Tree

```
Insert "token" into current node:

Is current node empty (value==-1) OR does value match token[0]?
│
├─ YES → Set value = token[0]
│        │
│        Is this the last character (strlen==1)?
│        │
│        ├─ YES → Word complete! (initialize list - future)
│        │        return
│        │
│        └─ NO → More characters remaining
│                 │
│                 Does child exist?
│                 │
│                 ├─ NO → Create new child node
│                 │
│                 └─ Call child->insert(token+1, id)
│
└─ NO → Character doesn't match
         │
         Does sibling exist?
         │
         ├─ NO → Create new sibling node
         │
         └─ Call sibling->insert(token, id)  [same token!]
```

### Line-by-Line Explanation

**Lines 27-28: Main Condition**
```cpp
if(value ==-1 || value ==token[0]){
    value = token[0];
```

**What it does:**
- Checks if current node is empty (-1) OR character matches
- If true, this node will store token[0]

**Why `||` (OR)?**
- Empty node: Can be assigned any character
- Matching node: Continue with this path

**Setting value:**
```cpp
value = token[0];
```
- If empty, this is first assignment
- If matching, reassigning same value (harmless)

**Example:**
```
token = "hello"
Empty node: value = -1 → set to 'h'
'h' node:   value = 'h' → stays 'h' (matches)
```

---

**Lines 29-31: Check for Word Completion**
```cpp
if(strlen(token)==1){
    //initialise list
    return;
}
```

**What it checks:**
- Is this the last character of the word?
- `strlen(token)==1` means only one character left

**What should happen (future):**
```cpp
if(strlen(token)==1){
    if(list == NULL){
        list = new listnode(id);
    }
    list->add(id);  // Track document and frequency
    return;
}
```

**Why return?**
- Word is complete, no more characters to process
- Ends the recursion

---

**Lines 32-37: Continue with Child**
```cpp
else{
    if(child == nullptr){
        child = new TrieNode();
    }
    child->insert(token+1,id);
}
```

**What it does:**
- More characters remaining in word
- Need to move to next level (child)

**Step 1: Create child if needed**
```cpp
if(child == nullptr){
    child = new TrieNode();
}
```
- First time reaching this character? Create child
- Already exists? Use existing child

**Step 2: Recursive call**
```cpp
child->insert(token+1, id);
```

**Key point:** `token+1`
- Pointer arithmetic!
- Moves to next character in string
- "hello" → "ello" → "llo" → "lo" → "o"

**Example:**
```
token = "hello"
token[0] = 'h'
token+1 = "ello"  (pointer moved by 1)
token+2 = "llo"   (pointer moved by 2)
```

---

**Lines 39-44: Character Doesn't Match - Try Sibling**
```cpp
else{
    if(sibling==nullptr){
        sibling=new TrieNode();
    }
    sibling->insert(token,id);
}
```

**When does this execute?**
- Current node has a value
- That value doesn't match token[0]
- Need to try alternative character (sibling)

**Step 1: Create sibling if needed**
```cpp
if(sibling==nullptr){
    sibling=new TrieNode();
}
```

**Step 2: Recursive call**
```cpp
sibling->insert(token, id);
```

**Key point:** `token` (NOT token+1)
- Still trying to insert same character
- Just at different node (sibling)

**Example:**
```
Current node: value = 'h'
Trying to insert: "world"
token[0] = 'w' ≠ 'h'
→ Create sibling with value = 'w'
```

---

## Execution Flow Examples

### Example 1: Inserting "hello" into Empty Trie

**Initial State:** Empty root node (value = -1)

**Step-by-Step:**

```
Call: root->insert("hello", 1)

Step 1: At root
        value = -1 (empty)
        -1 == -1? YES
        Set value = 'h'
        strlen("hello") == 1? NO
        child == nullptr? YES → Create child
        Call: child->insert("ello", 1)

Result: Root('h')

---

Step 2: At root's child (new node)
        value = -1 (empty)
        Set value = 'e'
        strlen("ello") == 1? NO
        child == nullptr? YES → Create child
        Call: child->insert("llo", 1)

Result: 'h' → 'e'

---

Step 3: At 'e's child
        value = -1
        Set value = 'l'
        strlen("llo") == 1? NO
        Create child
        Call: child->insert("lo", 1)

Result: 'h' → 'e' → 'l'

---

Step 4: At 'l's child
        value = -1
        Set value = 'l'
        strlen("lo") == 1? NO
        Create child
        Call: child->insert("o", 1)

Result: 'h' → 'e' → 'l' → 'l'

---

Step 5: At second 'l's child
        value = -1
        Set value = 'o'
        strlen("o") == 1? YES
        //initialize list (future)
        return

Final: 'h' → 'e' → 'l' → 'l' → 'o' [END]
```

### Example 2: Inserting "help" After "hello" Exists

**Initial State:** 'h' → 'e' → 'l' → 'l' → 'o'

**Inserting "help":**

```
Call: root->insert("help", 2)

Step 1: At 'h'
        value = 'h'
        'h' == 'h'? YES (matches!)
        strlen("help") == 1? NO
        child exists? YES (points to 'e')
        Call: child->insert("elp", 2)

---

Step 2: At 'e'
        value = 'e'
        'e' == 'e'? YES (matches!)
        strlen("elp") == 1? NO
        child exists? YES (points to 'l')
        Call: child->insert("lp", 2)

---

Step 3: At first 'l'
        value = 'l'
        'l' == 'l'? YES (matches!)
        strlen("lp") == 1? NO
        child exists? YES (points to second 'l')
        Call: child->insert("p", 2)

---

Step 4: At second 'l'
        value = 'l'
        'l' == 'p'? NO (doesn't match!)
        sibling exists? NO
        Create sibling
        Call: sibling->insert("p", 2)

---

Step 5: At new sibling
        value = -1
        Set value = 'p'
        strlen("p") == 1? YES
        //initialize list
        return

Final Structure:
    'h' → 'e' → 'l' → 'l' → 'o' [END]
                      ↓
                      'p' [END]
```

### Example 3: Inserting "world" After "hello"

**Initial State:** 'h' → 'e' → 'l' → 'l' → 'o'

```
Call: root->insert("world", 3)

Step 1: At 'h'
        value = 'h'
        'h' == 'w'? NO (doesn't match!)
        sibling exists? NO
        Create sibling
        Call: sibling->insert("world", 3)

Step 2: At 'h's new sibling
        value = -1
        Set value = 'w'
        strlen("world") == 1? NO
        Create child
        Call: child->insert("orld", 3)

... continues building 'w' → 'o' → 'r' → 'l' → 'd'

Final Structure:
    'h' → 'e' → 'l' → 'l' → 'o'
    ↓
    'w' → 'o' → 'r' → 'l' → 'd'
```

---

## Memory Management

### Memory Allocation Pattern

**When does allocation happen?**

1. **Creating root:**
```cpp
TrieNode *trie = new TrieNode();  // Explicit allocation
```

2. **Creating child:**
```cpp
if(child == nullptr){
    child = new TrieNode();  // Automatic during insert
}
```

3. **Creating sibling:**
```cpp
if(sibling == nullptr){
    sibling = new TrieNode();  // Automatic during insert
}
```

### Memory Footprint

**Per Node:**
```
sizeof(TrieNode) = 
    1 byte  (char value)
    8 bytes (TrieNode* sibling)
    8 bytes (TrieNode* child)
    8 bytes (listnode* list - future)
≈ 32 bytes (with padding)
```

**Example: "hello"**
- 5 nodes created
- 5 × 32 bytes = 160 bytes

**Example: "hello" + "help"**
- Shares h→e→l (3 nodes)
- Adds l→o and p (2 nodes)
- Total: 5 nodes (not 10!)
- Memory saved: 50%

### Cleanup

**Automatic with delete:**
```cpp
delete trie;  // Deletes entire tree recursively
```

**What happens:**
1. trie destructor called
2. Deletes all children recursively
3. Deletes all siblings recursively
4. All memory freed automatically

**No memory leaks:** ✅

---

## December 26 Updates

### Current State - Incomplete Implementation

Today we confirmed that Trie implementation is in **foundation phase**. The basic structure works, but search functionality is not yet implemented.

### What Works ✅

1. **Basic Structure**
   - TrieNode class defined
   - Constructor initializes properly
   - Destructor handles cleanup

2. **Insert Operation**
   - Correctly builds Trie structure
   - Handles siblings for branching
   - Handles children for sequences
   - Memory management safe

3. **Memory Management**
   - Proper allocation in insert()
   - Proper cleanup in destructor
   - No leaks detected

### What's Pending ⏳

1. **Listnode Integration**
   ```cpp
   // Currently commented:
   // listnode* list;
   
   // Future:
   listnode* list;  // ✅ Uncomment
   ```

2. **Word Completion Handling**
   ```cpp
   // Currently:
   if(strlen(token)==1){
       //initialise list  // ❌ Empty comment
       return;
   }
   
   // Future:
   if(strlen(token)==1){
       if(list == NULL){
           list = new listnode(id);
       }
       list->add(id);  // ✅ Track document
       return;
   }
   ```

3. **Search Functions**
   ```cpp
   // To be implemented:
   int tfsearchword(int id, char* word, int curr);
   int dsearchword(char* word, int curr);
   void dsearchall(char* buffer, int curr);
   void search(char* word, int curr, Scorelist* scorelist);
   ```

### Why Not Implemented Yet?

**Current Focus:** Building solid foundation
- ✅ Map class complete
- ✅ Document store complete
- ✅ Trie structure complete
- ✅ All memory management safe

**Next Phase:** Search functionality
- Requires Listnode integration
- Requires Scorelist class
- Requires Maxheap class (for ranking)
- Requires BM25 algorithm implementation

### Reference Implementation

From the complete version (`5.4 Heap Implementation`), we can see:

**listnode Integration:**
```cpp
if(strlen(token)==1){
    if(list==NULL)
        list=new listnode(id);
    list->add(id);  // Increments times if same doc, adds new node otherwise
}
```

**This enables:**
- Document frequency: Count how many docs contain word
- Term frequency: Count occurrences in specific doc
- BM25 scoring: Use TF and DF for relevance ranking

---

## Summary

### Current Implementation Status

✅ **Complete:**
- Constructor initialization
- Destructor cleanup
- Basic insert operation
- Sibling-child navigation
- Memory management

⏳ **Pending:**
- Listnode member uncommented
- Word completion handling
- Search functions
- TF/DF calculations
- BM25 integration

### Code Quality

**Strengths:**
- ✅ Clean recursive design
- ✅ Proper memory management
- ✅ Efficient sibling-child structure
- ✅ Foundation ready for expansion

**Improvements Needed:**
- Add listnode integration
- Implement search functions
- Add error handling for edge cases
- Consider adding iterative insert option

---

**Document Version**: 1.0  
**Last Updated**: December 26, 2025  
**Author**: High-Performance Search Engine Project  
**Repository**: github.com/adarshpheonix2810/high-performance-search-engine-cpp
