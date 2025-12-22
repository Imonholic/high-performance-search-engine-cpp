# Search Engine Project Documentation

---

**PROJECT:** Command-Line Search Engine  
**AUTHOR:** Adarsh Singh  
**DATE:** December 2025

---

## 1. Understanding Command Line Arguments in C++

### 1.1 Introduction

This search engine project accepts input through command-line arguments. To understand how the program receives and processes these inputs, we need to understand `argc` and `argv`.

### 1.2 The main() Function Signature

**Syntax:**
```cpp
int main(int argc, char *argv[])
```

**Where:**
- `argc` = **Argument Count** (integer)
- `argv` = **Argument Vector** (array of character pointers)

### 1.3 What is argc?

`argc` represents the **NUMBER** of command-line arguments passed to the program, including the program name itself.

**Example:**
```bash
./searchengine word1 word2 word3
```
In this case: `argc = 4` (program name + 3 arguments)

### 1.4 What is argv?

`argv` is an **ARRAY** of strings (character pointers) that holds all command-line arguments.

**Structure:**
- `argv[0]` = Program name (e.g., `"./searchengine"`)
- `argv[1]` = First argument (e.g., `"word1"`)
- `argv[2]` = Second argument (e.g., `"word2"`)
- ...and so on

### 1.5 Technical Details

The parameter `argv` has type `char**` (pointer to pointer to char).

These two declarations are **EQUIVALENT:**
```cpp
int main(int argc, char *argv[]);  // Array notation
int main(int argc, char **argv);   // Pointer notation
```

**Why?** Because in C/C++, array parameters decay to pointers.

**Memory Structure:**
- `argv` points to the first element of an array of `char*` pointers
- Each `argv[i]` points to a null-terminated C-string
- Each string represents one command-line argument

#### Visual Diagram

![argc and argv memory structure](../pic/argv+argc.png)

*Figure 1: Memory layout of argc and argv showing how command-line arguments are stored*

### 1.6 Naming Convention

While `argc` and `argv` are standard conventions, you can use any valid identifiers:

```cpp
int main(int num_args, char** arg_strings)  // Valid alternative
```

However, using `argc`/`argv` is **HIGHLY RECOMMENDED** for code readability.

### 1.7 How This Applies to Our Search Engine

Our search engine uses command-line arguments to:

1. Accept search queries from the user
2. Specify which documents to search
3. Configure search options (if implemented)

**Example Usage:**
```bash
./searchengine "search term" doc1.txt doc2.txt
```

In this case:
- `argc = 4`
- `argv[0] = "./searchengine"`
- `argv[1] = "search term"`
- `argv[2] = "doc1.txt"`
- `argv[3] = "doc2.txt"`

---

## 2. Understanding String Comparison with strcmp()

### 2.1 Introduction

When working with C-style strings in C++, comparing strings is a fundamental operation. Our search engine needs to compare user input with stored data, and understanding how string comparison works is crucial for correct functionality.

### 2.2 The Problem: Why Can't We Use == ?

In C++, when dealing with C-style strings (character arrays), you might be tempted to compare them using the `==` operator:

```cpp
char* string_a = "hello";
char* string_b = "hello";

if (string_a == string_b) {  // ❌ This doesn't work as expected!
    // This compares memory addresses, not content!
}
```

**The Issue:**  
The `==` operator between two `char*` pointers compares **memory addresses**, not the actual string content.

### 2.3 Memory Addresses vs String Content

When you use `==` with pointers, you're asking:  
> "Do these two pointers point to the **same memory location**?"

When you want to compare strings, you're actually asking:  
> "Do these two strings have the **same characters** in the same order?"

#### Visual Example

```
Memory Layout:
┌─────────────────────────────────────┐
│ Address: 0x1000                     │
│ Content: 'h' 'e' 'l' 'l' 'o' '\0'   │  ← string_a points here
└─────────────────────────────────────┘

┌─────────────────────────────────────┐
│ Address: 0x2000                     │
│ Content: 'h' 'e' 'l' 'l' 'o' '\0'   │  ← string_b points here
└─────────────────────────────────────┘

string_a == string_b  →  0x1000 == 0x2000  →  false ❌
strcmp(string_a, string_b)  →  compares content  →  0 (equal) ✓
```

### 2.4 The Solution: strcmp()

The `strcmp()` function compares two C-style strings **character by character**, examining their actual content rather than memory addresses.

**Function Signature:**
```cpp
#include <cstring>

int strcmp(const char* str1, const char* str2);
```

**Return Values:**
- **0** → Strings are **equal**
- **< 0** (negative) → str1 comes **before** str2 (lexicographically)
- **> 0** (positive) → str1 comes **after** str2 (lexicographically)

### 2.5 How strcmp() Works

The function compares strings **character by character** using their ASCII values:

```cpp
strcmp("apple", "apple")   // Returns 0  (equal)
strcmp("apple", "banana")  // Returns <0 (apple < banana)
strcmp("zebra", "apple")   // Returns >0 (zebra > apple)
```

**Algorithm:**
1. Start at the first character of both strings
2. Compare character by character
3. If a difference is found, return the difference
4. If one string ends (reaches '\0'), it's considered "less than"
5. If both strings end simultaneously, they're equal (return 0)

### 2.6 Practical Examples

#### Example 1: Basic String Comparison
```cpp
#include <cstring>
#include <iostream>

int main() {
    char str1[] = "hello";
    char str2[] = "hello";
    char str3[] = "world";
    
    // Compare strings with identical content
    if (strcmp(str1, str2) == 0) {
        std::cout << "str1 and str2 are equal\n";  // ✓ This will execute
    }
    
    // Compare different strings
    if (strcmp(str1, str3) != 0) {
        std::cout << "str1 and str3 are different\n";  // ✓ This will execute
    }
    
    return 0;
}
```

#### Example 2: Why == Fails with String Literals
```cpp
char* string_a = "search";
char* string_b = "search";

// These are two SEPARATE string literals in memory
// Even though content is same, they may have different addresses

if (string_a == string_b) {
    std::cout << "Same address\n";  // ❌ Might not execute!
}

if (strcmp(string_a, string_b) == 0) {
    std::cout << "Same content\n";  // ✓ Will execute!
}
```

### 2.7 How This Applies to Our Search Engine

Our search engine uses `strcmp()` in several critical operations:

#### 1. **Comparing Search Queries**
```cpp
// Check if user's search term matches a keyword
if (strcmp(argv[1], "search") == 0) {
    // Execute search operation
}
```

#### 2. **Matching File Names**
```cpp
// Verify which document to search
if (strcmp(filename, "doc1.txt") == 0) {
    // Process doc1.txt
}
```

#### 3. **Finding Exact Matches**
```cpp
// Compare search term with words in documents
if (strcmp(search_term, word_from_file) == 0) {
    // Match found!
    match_count++;
}
```

### 2.8 Common Pitfalls to Avoid

#### ❌ Wrong: Using == for string comparison
```cpp
if (str1 == str2) {  // Compares addresses, not content!
    // Unreliable behavior
}
```

#### ✓ Correct: Using strcmp()
```cpp
if (strcmp(str1, str2) == 0) {  // Compares content
    // Reliable comparison
}
```

#### ⚠️ Important Notes:
1. **Case Sensitivity:** `strcmp()` is case-sensitive
   - `strcmp("Hello", "hello")` ≠ 0
   - Use `strcasecmp()` or `_stricmp()` for case-insensitive comparison

2. **Null Termination:** Both strings must be null-terminated (`'\0'`)
   - Otherwise, undefined behavior occurs

3. **NULL Pointers:** Never pass NULL pointers to `strcmp()`
   - This causes a crash

### 2.9 Alternative: C++ std::string

Modern C++ offers `std::string` which allows direct comparison:

```cpp
#include <string>

std::string str1 = "hello";
std::string str2 = "hello";

if (str1 == str2) {  // ✓ This works with std::string!
    std::cout << "Strings are equal\n";
}
```

However, our search engine uses C-style strings (char arrays) because:
- Command-line arguments come as `char*` arrays
- Lower-level control for educational purposes
- Demonstrates fundamental C/C++ concepts

### 2.10 Quick Reference Table

| Operation | C-Style Strings (char*) | C++ std::string |
|-----------|------------------------|-----------------|
| Declaration | `char str[] = "text";` | `std::string str = "text";` |
| Comparison | `strcmp(str1, str2) == 0` | `str1 == str2` |
| Address Check | `str1 == str2` | N/A |
| Case-Insensitive | `strcasecmp(str1, str2)` | Custom function needed |
| Length | `strlen(str)` | `str.length()` |

### 2.11 Summary

- **`strcmp()`** compares string **content**, not memory addresses
- **`==`** operator compares **memory addresses** for pointers
- `strcmp()` returns **0** when strings are equal
- This is essential for our search engine to match search terms with document content
- Always use `strcmp()` for C-style string comparisons

---

## References

For more information, visit:  
- [What does int argc, char *argv[] mean?](https://stackoverflow.com/questions/3024197/what-does-int-argc-char-argv-mean)
- [C++ strcmp() Reference](https://cplusplus.com/reference/cstring/strcmp/)
- [String Comparison in C/C++](https://en.cppreference.com/w/cpp/string/byte/strcmp)

---

*Documentation created for educational purposes to help understand the Search Engine project implementation.*
