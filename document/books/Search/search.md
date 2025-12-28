# Search - Concepts Documentation

This document explains the **concepts and theory** behind search operations and query handling. For code implementation details, see `working.md`.

---

## Table of Contents

1. [What is Search Module?](#1-what-is-search-module)
2. [What is TF (Term Frequency)?](#2-what-is-tf-term-frequency)
3. [What is DF (Document Frequency)?](#3-what-is-df-document-frequency)
4. [What is strtok?](#4-what-is-strtok)
5. [What is isdigit?](#5-what-is-isdigit)
6. [What is atoi?](#6-what-is-atoi)
7. [Why Not Use stoi?](#7-why-not-use-stoi)
8. [Query Command System](#8-query-command-system)

---

## 1. What is Search Module?

### Definition

The **Search module** is responsible for executing user queries and calculating relevance scores for documents. It provides three main operations:

1. **`/search`** - Search for documents containing query words
2. **`/tf`** - Get Term Frequency (word count in specific document)
3. **`/df`** - Get Document Frequency (how many documents contain word)

### Purpose

The search module bridges the **user interface** (command-line queries) with the **backend data structures** (Trie, Map, Listnode) to provide meaningful search results.

---

## 2. What is TF (Term Frequency)?

### Definition

**Term Frequency (TF)** is the count of how many times a specific word appears in a specific document.

### Example

```
Document 0: "hello world hello earth hello"

TF("hello", doc=0) = 3
TF("world", doc=0) = 1
TF("earth", doc=0) = 1
TF("python", doc=0) = 0  (not present)
```

### Why TF Matters

**Relevance Scoring:**
- Document with word appearing 5 times is MORE relevant than document with word appearing once
- Higher TF → Higher BM25 score

**Query:**
```bash
/tf 0 hello
# Returns: 3 (appears 3 times in document 0)
```

### Formula in BM25

```
TF Component = (TF × (k1 + 1)) / (TF + k1 × (1 - b + b × (doc_len / avg_doc_len)))

Where:
- k1 = 1.5 (tuning parameter)
- b = 0.75 (length normalization)
```

---

## 3. What is DF (Document Frequency)?

### Definition

**Document Frequency (DF)** is the count of how many documents contain a specific word.

### Example

```
Document 0: "hello world"
Document 1: "hello earth"
Document 2: "python code"

DF("hello") = 2  (in doc 0 and 1)
DF("world") = 1  (only in doc 0)
DF("python") = 1  (only in doc 2)
DF("java") = 0   (not present)
```

### Why DF Matters

**Inverse Relationship:**
- **Common words** (high DF) → Less important (e.g., "the", "is")
- **Rare words** (low DF) → More important (e.g., "quantum", "blockchain")

**IDF Calculation:**
```
IDF = log((N - DF + 0.5) / (DF + 0.5))

Where:
- N = total number of documents
- DF = document frequency
```

**Query:**
```bash
/df hello
# Returns: 2 (present in 2 documents)
```

### Example Impact

```
Query: "quantum physics"

DF("quantum") = 5 out of 1000 documents  → High IDF (rare, important!)
DF("the") = 998 out of 1000 documents   → Low IDF (common, less important)
```

---

## 4. What is strtok?

### Definition

`strtok` is a C function that **tokenizes** (splits) a string into smaller tokens based on delimiters.

### Syntax

```cpp
char* strtok(char* str, const char* delimiters);
```

### How It Works

```cpp
char input[] = "/tf 0 hello";
char* token = strtok(input, " \t\n");  // First call: returns "/tf"

token = strtok(NULL, " \t\n");  // Second call: returns "0"
token = strtok(NULL, " \t\n");  // Third call: returns "hello"
token = strtok(NULL, " \t\n");  // Fourth call: returns NULL (end)
```

### Key Points

**1. Modifies Original String:**
```cpp
char input[] = "hello world";
strtok(input, " ");
// input is now: "hello\0world"  (inserted null terminator)
```

**2. Returns Pointer to Original String:**
```cpp
char* token = strtok(input, " ");
// token points to somewhere inside input[]
// NOT a new allocated memory!
```

**3. Stateful Function:**
```cpp
strtok(input, " ");   // First call with string
strtok(NULL, " ");    // Subsequent calls with NULL
// Remembers position internally (static variable)
```

### Critical Memory Rule

**NEVER call free() on strtok result:**
```cpp
char* token = strtok(input, " ");
free(token);  // ❌ CRASH! token points to input[], not malloc'd memory
```

### Alternatives

| Function | Thread-Safe | Modifies String | Allocates Memory |
|----------|-------------|-----------------|------------------|
| `strtok` | ❌ No | ✅ Yes | ❌ No |
| `strtok_r` | ✅ Yes | ✅ Yes | ❌ No |
| `strsep` | ✅ Yes | ✅ Yes | ❌ No |
| Manual parsing | ✅ Yes | ❌ No | ✅ Optional |

---

## 5. What is isdigit?

### Definition

`isdigit` checks if a character is a decimal digit (0-9).

### Syntax

```cpp
#include <cctype>
int isdigit(int ch);
```

### How It Works

```cpp
isdigit('5')   // Returns: non-zero (true)
isdigit('a')   // Returns: 0 (false)
isdigit(' ')   // Returns: 0 (false)
isdigit('\n')  // Returns: 0 (false)
```

### Use Case in Our Code

**Validating document ID:**
```cpp
char* id_str = "123";
for(int i = 0; i < strlen(id_str); i++){
    if(!isdigit(id_str[i])){
        cout << "Error: ID must be a number" << endl;
        return -1;
    }
}
// All characters are digits ✅
int id = atoi(id_str);
```

### Why Not Just atoi?

```cpp
atoi("123")    // Returns: 123 ✅
atoi("abc")    // Returns: 0 (silent failure) ❌
atoi("12abc")  // Returns: 12 (partial parse) ❌
```

**Problem:** `atoi` doesn't tell you if input was valid!

**Solution:** Validate first with `isdigit`:
```cpp
// Step 1: Validate
if(!isdigit(str[0])){
    return -1;  // Invalid input
}

// Step 2: Convert
int num = atoi(str);  // Safe now
```

### Alternatives

| Function | Purpose | Example |
|----------|---------|---------|
| `isdigit` | Check if digit | `isdigit('5')` |
| `isalpha` | Check if letter | `isalpha('a')` |
| `isalnum` | Check if alphanumeric | `isalnum('a')` |
| `isspace` | Check if whitespace | `isspace(' ')` |

---

## 6. What is atoi?

### Definition

`atoi` converts a string to an integer (ASCII to Integer).

### Syntax

```cpp
#include <cstdlib>
int atoi(const char* str);
```

### How It Works

```cpp
atoi("123")      // Returns: 123
atoi("-456")     // Returns: -456
atoi("  789")    // Returns: 789 (skips leading spaces)
atoi("12.34")    // Returns: 12 (stops at non-digit)
atoi("abc")      // Returns: 0 (invalid input)
atoi("")         // Returns: 0 (empty string)
```

### Conversion Process

```
String: "123"
  ↓
'1' → 1
'2' → 10 + 2 = 12
'3' → 120 + 3 = 123
  ↓
Integer: 123
```

### Problems with atoi

**1. No Error Reporting:**
```cpp
int x = atoi("abc");  // x = 0 (no error!)
int y = atoi("0");    // y = 0 (valid zero)
// Can't tell the difference! ❌
```

**2. Silent Partial Conversion:**
```cpp
int x = atoi("123abc");  // x = 123 (stops at 'a')
// No warning about "abc" ❌
```

**3. No Overflow Detection:**
```cpp
int x = atoi("999999999999999");  // Undefined behavior!
```

### When to Use atoi

✅ **Good for:**
- Trusted input (already validated)
- Simple cases where error handling not critical
- Quick prototypes

❌ **Bad for:**
- User input validation
- Production code requiring error handling
- Large numbers (use strtol)

---

## 7. Why Not Use stoi?

### Comparison

| Feature | atoi | stoi |
|---------|------|------|
| Language | C | C++ |
| Header | `<cstdlib>` | `<string>` |
| Input | `const char*` | `std::string` |
| Error handling | None | Throws exception |
| Overflow | Undefined | Throws exception |

### Example: stoi

```cpp
#include <string>
try {
    int x = stoi("123");        // ✅ Works
    int y = stoi("abc");        // ❌ Throws invalid_argument
    int z = stoi("999999999");  // ❌ Throws out_of_range
} catch(const std::exception& e) {
    cout << "Error: " << e.what() << endl;
}
```

### Why We Use atoi Instead

**1. Our Code Uses C-Style Strings:**
```cpp
char* token = strtok(input, " ");
// token is char*, not std::string
// Would need conversion: std::string(token)
```

**2. Already Validated Input:**
```cpp
// We validate BEFORE converting
for(int i=0; i<strlen(token); i++){
    if(!isdigit(token[i])){
        return -1;  // Validated ✅
    }
}
int id = atoi(token);  // Safe to use atoi
```

**3. Simpler Error Handling:**
```cpp
// atoi approach (our code)
if(!isdigit(token[0])) return -1;
int x = atoi(token);

// stoi approach
try {
    int x = stoi(std::string(token));
} catch(...) {
    return -1;
}
// More code, same result
```

**4. C-Style Project:**
- Using `char*`, `strtok`, `malloc/free`
- Mixing C++ std::string would be inconsistent
- atoi fits the C-style design

### When stoi is Better

✅ **Use stoi when:**
- Working with `std::string` throughout
- Need strong exception-based error handling
- Using modern C++ (C++11+)
- Want automatic overflow detection

---

## 8. Query Command System

### Available Commands

```bash
/search <query>     # Search for documents (not implemented)
/tf <doc_id> <word> # Get term frequency
/df <word>          # Get document frequency  
/exit               # Exit program
```

### Command Format

**TF Query:**
```bash
/tf 0 hello
│   │ │
│   │ └─ word to search
│   └─ document ID (must be number)
└─ command
```

**DF Query:**
```bash
/df hello
│   │
│   └─ word to search
└─ command
```

### Input Flow

```
User types: "/tf 0 hello\n"
    ↓
getline() reads entire line
    ↓
strtok splits: ["/tf", "0", "hello"]
    ↓
inputmanager() identifies command
    ↓
tf() function executes
    ↓
Result displayed
```

### Error Handling

**Unknown Command:**
```bash
> hello
Unknown command: hello
Available commands: /search, /df, /tf, /exit
```

**Missing Arguments:**
```bash
> /tf 0
Error: Missing word. Usage: /tf <doc_id> <word>
```

**Invalid Document ID:**
```bash
> /tf abc hello
Error: Document ID must be a number
```

---

## Summary

### Key Concepts

1. **TF** - Word count in specific document (relevance indicator)
2. **DF** - Document count containing word (rarity indicator)
3. **strtok** - String tokenizer (NEVER free its result!)
4. **isdigit** - Validates numeric input (prevents atoi errors)
5. **atoi** - Converts string to int (simple, no error reporting)
6. **stoi** - Modern C++ alternative (exceptions, slower for C-style code)

### Why These Choices?

✅ **strtok** - Fast, simple, fits C-style design  
✅ **isdigit** - Validates before conversion  
✅ **atoi** - Simple conversion after validation  
✅ **Command system** - User-friendly query interface  

### Integration with Other Modules

```
User Query → Search.cpp → Trie → Listnode → TF/DF values
                      ↓
                   Map → Document content
```

---

**Document Version**: 1.0  
**Last Updated**: December 28, 2025  
**Changes**: Initial creation with search concepts and command system  
**Author**: High-Performance Search Engine Project
