# Search Engine Implementation - Code Explanation

This document provides a **detailed step-by-step explanation** of the `Searchengine.cpp` file implementation. It explains how the code works, what each line does, and the execution flow of the main program.

---

## File Information

**File Name**: `Searchengine.cpp`  
**Location**: `src/Searchengine.cpp`  
**Purpose**: Main entry point for the search engine - handles command-line arguments, validates input, and processes document files  
**Header File**: `header/Searchengine.hpp`

---

## Table of Contents

1. [Complete Source Code](#complete-source-code)
2. [Header Files and Dependencies](#header-files-and-dependencies)
3. [Main Function Overview](#main-function-overview)
4. [Command-Line Argument Parsing](#command-line-argument-parsing)
5. [Argument Validation](#argument-validation)
6. [Integer Conversion with Error Handling](#integer-conversion-with-error-handling)
7. [File Processing](#file-processing)
8. [Execution Flow Diagram](#execution-flow-diagram)
9. [Example Usage Scenarios](#example-usage-scenarios)

---

## Complete Source Code

```cpp
#include "Searchengine.hpp"

using namespace std;

// read document/books/searchengine.md for more information
int main(int argc, char** argv) {
    if (argc != 5 ||
        strcmp(argv[1], "-d") != 0 ||
        strcmp(argv[3], "-k") != 0) {
        cout << "Wrong arguments. Usage: -d <file> -k <number>" << endl;
        return -1;
    }

    cout << "Please wait..." << endl;
    int linecounter = 0;
    int maxlength = -1;
    int k;
    try {
        k = stoi(argv[4]); 
    } catch (...) {
        cout << "Invalid value for -k (must be an integer)" << endl;
        return -1;
    }
    
    if(read_sizes(&linecounter, &maxlength, argv[2]) == -1){
        return -1;
    }
    cout << "File read successfully. Lines: " << linecounter 
         << ", Max Length: " << maxlength << endl;
    return 1;
}
```

---

## Header Files and Dependencies

### Searchengine.hpp Contents

```cpp
#include <iostream>
#include <cstring>
#include "./Document_store.hpp"
```

**Line-by-Line Breakdown:**

#### Line 1: `#include <iostream>`

**What it provides:**
- `cout` - Console output stream
- `endl` - End line manipulator
- Input/output stream functionality

**Used in our code:**
```cpp
cout << "Please wait..." << endl;
cout << "Wrong arguments. Usage: -d <file> -k <number>" << endl;
```

---

#### Line 2: `#include <cstring>`

**What it provides:**
- `strcmp()` - String comparison function
- C-style string manipulation functions

**Used in our code:**
```cpp
strcmp(argv[1], "-d") != 0
strcmp(argv[3], "-k") != 0
```

**Why `strcmp()`?**
- `argv` contains C-style strings (`char*`)
- Cannot use `==` to compare C-style strings (compares pointers, not content)
- `strcmp()` compares actual string content

---

#### Line 3: `#include "./Document_store.hpp"`

**What it provides:**
- `read_sizes()` function declaration
- Access to document processing functionality

**Used in our code:**
```cpp
read_sizes(&linecounter, &maxlength, argv[2])
```

---

### Line 1-2: Include and Namespace

```cpp
#include "Searchengine.hpp"

using namespace std;
```

**Line 1: `#include "Searchengine.hpp"`**
- Includes the header file with all dependencies
- Brings in `iostream`, `cstring`, and `Document_store.hpp`

**Line 2: `using namespace std;`**
- Allows using `cout` instead of `std::cout`
- Allows using `endl` instead of `std::endl`
- Makes code more readable

---

## Main Function Overview

### Function Signature

```cpp
int main(int argc, char** argv)
```

**Return Type**: `int`
- Returns `1` on success
- Returns `-1` on failure (wrong arguments, invalid input, file error)

**Parameters:**

| Parameter | Type | Purpose |
|-----------|------|---------|
| `argc` | `int` | **Arg**ument **C**ount - Number of command-line arguments |
| `argv` | `char**` | **Arg**ument **V**ector - Array of C-style strings containing arguments |

---

### Understanding argc and argv

**Visual Representation:**

```
Command: ./searchengine -d doc1.txt -k 10

argc = 5

argv[0] = "./searchengine"  ← Program name
argv[1] = "-d"              ← Flag
argv[2] = "doc1.txt"        ← File name
argv[3] = "-k"              ← Flag
argv[4] = "10"              ← Number (as string)
```

**Memory Layout:**

```
argv (array of pointers):
+--------+--------+--------+--------+--------+
| ptr[0] | ptr[1] | ptr[2] | ptr[3] | ptr[4] |
+--------+--------+--------+--------+--------+
    ↓        ↓        ↓        ↓        ↓
  "prog"   "-d"   "doc.txt"  "-k"    "10"
```

**Important:** 
- `argv` elements are C-style strings (`char*`)
- All arguments are strings (even numbers)
- Must convert "10" to integer using `stoi()`

---

## Command-Line Argument Parsing

### Lines 6-11: Argument Validation

```cpp
if (argc != 5 ||
    strcmp(argv[1], "-d") != 0 ||
    strcmp(argv[3], "-k") != 0) {
    cout << "Wrong arguments. Usage: -d <file> -k <number>" << endl;
    return -1;
}
```

**This is a compound conditional checking THREE things!**

---

### Condition 1: `argc != 5`

```cpp
argc != 5
```

**What it checks:**
- Verifies exactly 5 arguments are provided
- 5 arguments = program name + 4 user arguments

**Example:**

```
✓ VALID:
./searchengine -d doc1.txt -k 10
argc = 5 → Condition is FALSE → Continue

✗ INVALID (too few):
./searchengine -d doc1.txt
argc = 3 → Condition is TRUE → Error!

✗ INVALID (too many):
./searchengine -d doc1.txt -k 10 extra
argc = 6 → Condition is TRUE → Error!
```

**Why exactly 5?**

| Index | Content | Example |
|-------|---------|---------|
| argv[0] | Program name | `./searchengine` |
| argv[1] | `-d` flag | `-d` |
| argv[2] | File path | `doc1.txt` |
| argv[3] | `-k` flag | `-k` |
| argv[4] | Number | `10` |

---

### Condition 2: `strcmp(argv[1], "-d") != 0`

```cpp
strcmp(argv[1], "-d") != 0
```

**What it checks:**
- First argument must be exactly "-d"
- Uses `strcmp()` to compare string content

**How strcmp() works:**

```cpp
strcmp(str1, str2) returns:
    0   if strings are equal
    < 0 if str1 < str2 (alphabetically)
    > 0 if str1 > str2 (alphabetically)
```

**Examples:**

```
✓ VALID:
argv[1] = "-d"
strcmp("-d", "-d") = 0
0 != 0 → FALSE → Continue

✗ INVALID:
argv[1] = "-f"
strcmp("-f", "-d") ≠ 0
non-zero != 0 → TRUE → Error!

✗ INVALID:
argv[1] = "d"
strcmp("d", "-d") ≠ 0
non-zero != 0 → TRUE → Error!
```

**Why not use `==`?**

```cpp
// ❌ WRONG: Compares pointer addresses, not content
if(argv[1] == "-d")  // Always false!

// ✅ CORRECT: Compares string content
if(strcmp(argv[1], "-d") == 0)
```

---

### Condition 3: `strcmp(argv[3], "-k") != 0`

```cpp
strcmp(argv[3], "-k") != 0
```

**What it checks:**
- Third argument must be exactly "-k"
- Same logic as condition 2

**Examples:**

```
✓ VALID:
argv[3] = "-k"
strcmp("-k", "-k") = 0
0 != 0 → FALSE → Continue

✗ INVALID:
argv[3] = "-n"
strcmp("-n", "-k") ≠ 0
TRUE → Error!
```

---

### The OR Logic (||)

```cpp
if (argc != 5 ||
    strcmp(argv[1], "-d") != 0 ||
    strcmp(argv[3], "-k") != 0)
```

**How OR works:**
- If **ANY** condition is TRUE → Enter error block
- If **ALL** conditions are FALSE → Continue execution

**Truth Table:**

| argc != 5 | strcmp argv[1] | strcmp argv[3] | Result | Action |
|-----------|----------------|----------------|--------|--------|
| TRUE | - | - | TRUE | Error |
| FALSE | TRUE | - | TRUE | Error |
| FALSE | FALSE | TRUE | TRUE | Error |
| FALSE | FALSE | FALSE | FALSE | Continue ✓ |

**Visual Flow:**

```
Check argc != 5
    ↓
  TRUE? → Error → Exit
    ↓ NO
Check argv[1] != "-d"
    ↓
  TRUE? → Error → Exit
    ↓ NO
Check argv[3] != "-k"
    ↓
  TRUE? → Error → Exit
    ↓ NO
All checks passed → Continue
```

---

### Lines 9-11: Error Message and Exit

```cpp
cout << "Wrong arguments. Usage: -d <file> -k <number>" << endl;
return -1;
```

**Line 9: Error Message**

```cpp
cout << "Wrong arguments. Usage: -d <file> -k <number>" << endl;
```

**What it does:**
- Prints error message to console
- Shows correct usage format
- Helps user understand how to use the program

**Example Output:**
```
Wrong arguments. Usage: -d <file> -k <number>
```

**Line 10: Return Error Code**

```cpp
return -1;
```

**What it does:**
- Exits `main()` function immediately
- Returns `-1` to operating system (indicates error)
- Program terminates

**Error code convention:**
```
 0 or positive = Success
-1 or negative = Error
```

**Usage by system:**
```bash
./searchengine -d wrong
echo $?  # Prints: 255 (or -1 depending on system)
```

---

## Argument Validation

### Line 13: User Feedback

```cpp
cout << "Please wait..." << endl;
```

**What it does:**
- Informs user that processing has started
- Provides feedback while file is being read
- Good user experience practice

**Why here?**
- Placed **after** argument validation
- Only shown when arguments are correct
- Appears before potentially slow file operations

**Visual Timeline:**

```
User runs: ./searchengine -d large_file.txt -k 10
    ↓
Arguments validated ✓
    ↓
Output: "Please wait..."
    ↓
File processing starts (may take time)
    ↓
Output: "File read successfully..."
```

---

### Lines 14-16: Variable Initialization

```cpp
int linecounter = 0;
int maxlength = -1;
int k;
```

**Line-by-Line Breakdown:**

#### Line 14: `int linecounter = 0;`

**Purpose:**
- Will store the total number of lines in the file
- Initialized to `0` (no lines counted yet)
- Will be modified by `read_sizes()` function

**Why initialize to 0?**
- Safe default value
- Prevents using uninitialized memory
- Makes sense: 0 lines before reading

---

#### Line 15: `int maxlength = -1;`

**Purpose:**
- Will store the maximum line length in the file
- Initialized to `-1` (no maximum found yet)

**Why initialize to -1 instead of 0?**
- `-1` indicates "not yet calculated"
- Valid line lengths are ≥ 0
- Allows detection if file reading failed
- After successful read, will be ≥ 0

**Example:**
```cpp
// Before reading
maxlength = -1  (means: unknown)

// After reading file with lines of length 10, 25, 15
maxlength = 25  (longest line)
```

---

#### Line 16: `int k;`

**Purpose:**
- Will store the number parameter from command line
- Not initialized (will be assigned in try block)

**Why not initialized?**
- Will be assigned immediately in next block
- If assignment fails, program exits (no need for default)

**Will hold:**
- Value from `argv[4]` converted to integer
- Example: If `argv[4] = "10"`, then `k = 10`

---

## Integer Conversion with Error Handling

### Lines 17-22: Try-Catch Block

```cpp
try {
    k = stoi(argv[4]); 
} catch (...) {
    cout << "Invalid value for -k (must be an integer)" << endl;
    return -1;
}
```

**This is EXCEPTION HANDLING in C++!**

---

### Line 17: Try Block

```cpp
try {
```

**What it does:**
- Starts a protected code block
- Code inside is monitored for exceptions (errors)
- If exception occurs, jumps to `catch` block

**Why needed?**
- `stoi()` can throw exceptions
- Prevents program crash
- Allows graceful error handling

---

### Line 18: String to Integer Conversion

```cpp
k = stoi(argv[4]);
```

**Breaking it down:**

**`argv[4]`**
- Fourth command-line argument (index 4)
- Contains the number as a **string**
- Example: `"10"`, `"25"`, `"100"`

**`stoi()`**
- **St**ring **to** **I**nteger conversion
- Converts string to integer
- Example: `"10"` → `10`

**Full syntax:**
```cpp
int stoi(const string& str);
```

**Examples:**

```cpp
// ✓ Valid conversions
stoi("10")    → 10
stoi("0")     → 0
stoi("999")   → 999
stoi("-5")    → -5
stoi("  42 ") → 42 (ignores whitespace)

// ✗ Invalid - throws exception
stoi("abc")       → exception!
stoi("12.5")      → exception! (not an integer)
stoi("10 20")     → exception! (multiple values)
stoi("")          → exception! (empty string)
```

---

### Why stoi() Instead of atoi()?

**Old way (atoi):**
```cpp
k = atoi(argv[4]);  // ❌ No error detection!

// Problems:
atoi("abc")  → 0  (silently fails!)
atoi("10")   → 10 (works)
// Can't tell if "0" is valid or error!
```

**Modern way (stoi):**
```cpp
try {
    k = stoi(argv[4]);  // ✅ Throws exception on error!
} catch(...) {
    // Handle error
}

// Advantages:
stoi("abc")  → throws exception (detectable!)
stoi("10")   → 10 (works)
stoi("0")    → 0 (valid, distinguishable from error)
```

**Comparison:**

| Feature | atoi() | stoi() |
|---------|--------|--------|
| Error detection | ❌ Returns 0 | ✅ Throws exception |
| C++ standard | C-style | Modern C++ |
| Safety | Low | High |
| Distinguishes "0" from error | ❌ No | ✅ Yes |

---

### Lines 19-22: Exception Handling

```cpp
} catch (...) {
    cout << "Invalid value for -k (must be an integer)" << endl;
    return -1;
}
```

**Line 19: `catch (...)`**

**What it does:**
- Catches **any** exception thrown in try block
- `...` means "catch all exception types"

**Alternative (specific exception):**
```cpp
catch (invalid_argument& e) {
    // Only catch invalid_argument exceptions
}
catch (out_of_range& e) {
    // Only catch out_of_range exceptions
}
```

**Our code uses `(...)` because:**
- Simpler
- Catches all possible errors
- Same error message for all cases

---

**Lines 20-21: Error Handling**

```cpp
cout << "Invalid value for -k (must be an integer)" << endl;
return -1;
```

**What happens:**
1. Print error message explaining the problem
2. Exit program with error code `-1`

**Example scenarios:**

```
Scenario 1: User enters non-numeric value
Command: ./searchengine -d doc1.txt -k hello
    ↓
stoi("hello") throws exception
    ↓
Output: "Invalid value for -k (must be an integer)"
    ↓
Program exits with -1

Scenario 2: User enters decimal number
Command: ./searchengine -d doc1.txt -k 12.5
    ↓
stoi("12.5") throws exception
    ↓
Output: "Invalid value for -k (must be an integer)"
    ↓
Program exits with -1

Scenario 3: User enters valid number
Command: ./searchengine -d doc1.txt -k 10
    ↓
k = stoi("10") → k = 10 ✓
    ↓
No exception
    ↓
Continue to next line
```

---

### Complete Flow of Try-Catch

```
try {
    k = stoi(argv[4]);
}
    ↓
Is argv[4] valid integer string?
    ↓
  YES → k = integer value → Continue to line 24
    ↓
   NO → Exception thrown
        ↓
      catch (...) {
          Print error
          return -1
      }
        ↓
      Program exits
```

---

## File Processing

### Lines 24-26: Call read_sizes()

```cpp
if(read_sizes(&linecounter, &maxlength, argv[2]) == -1){
    return -1;
}
```

**This is where the actual file processing happens!**

---

### Line 24: Function Call and Return Check

```cpp
if(read_sizes(&linecounter, &maxlength, argv[2]) == -1)
```

**Breaking it down:**

#### Part 1: Function Call

```cpp
read_sizes(&linecounter, &maxlength, argv[2])
```

**Function: `read_sizes()`**
- Defined in `document_store.cpp`
- Reads file and counts lines

**Parameters:**

| Argument | Type | Value | Purpose |
|----------|------|-------|---------|
| `&linecounter` | `int*` | Address of `linecounter` | Output: stores line count |
| `&maxlength` | `int*` | Address of `maxlength` | Output: stores max length |
| `argv[2]` | `char*` | File path string | Input: file to process |

**Why `&` (address-of) operator?**
- `read_sizes()` expects pointers (`int*`)
- We have variables (`int`)
- `&` gets the memory address of variable
- Allows function to modify original variables

**Visual:**

```
Before call:
    linecounter = 0
    maxlength = -1

Call read_sizes(&linecounter, &maxlength, "doc1.txt")
    ↓
Function modifies values through pointers
    ↓
After call:
    linecounter = 150  (file has 150 lines)
    maxlength = 85     (longest line is 85 chars)
```

---

#### Part 2: Return Value Check

```cpp
... == -1
```

**What it checks:**
- `read_sizes()` returns `-1` on error
- `read_sizes()` returns `1` on success

**Possible scenarios:**

```
Return -1 → Error occurred
    - File not found
    - File empty
    - Permission denied

Return 1 → Success
    - File read successfully
    - linecounter and maxlength updated
```

---

#### Part 3: Conditional Logic

```cpp
if(... == -1)
```

**If condition is TRUE (return value is -1):**
- Enter the if block
- Execute line 25

**If condition is FALSE (return value is 1):**
- Skip the if block
- Continue to line 27

---

### Line 25: Error Exit

```cpp
return -1;
```

**What happens:**
- Exits `main()` function immediately
- Returns error code to operating system
- Program terminates

**Why no error message here?**
- `read_sizes()` already printed error message
- Examples:
  - "Cannot open file: doc1.txt"
  - "File is empty: doc1.txt"
- No need to print duplicate message

---

### Complete Flow Example

**Scenario 1: File Not Found**

```
Command: ./searchengine -d missing.txt -k 10

Line 24: read_sizes(&linecounter, &maxlength, "missing.txt")
    ↓
Inside read_sizes():
    fopen("missing.txt") returns NULL
    Prints: "Cannot open file: missing.txt"
    Returns -1
    ↓
Line 24: return value == -1 → TRUE
    ↓
Line 25: return -1
    ↓
Program exits
```

**Scenario 2: File Empty**

```
Command: ./searchengine -d empty.txt -k 10

Line 24: read_sizes(&linecounter, &maxlength, "empty.txt")
    ↓
Inside read_sizes():
    File opens successfully
    fgetc() returns EOF (empty file)
    Prints: "File is empty: empty.txt"
    Returns -1
    ↓
Line 24: return value == -1 → TRUE
    ↓
Line 25: return -1
    ↓
Program exits
```

**Scenario 3: Success**

```
Command: ./searchengine -d doc1.txt -k 10

Line 24: read_sizes(&linecounter, &maxlength, "doc1.txt")
    ↓
Inside read_sizes():
    File opens successfully
    Reads 150 lines
    Maximum line length: 85
    linecounter = 150
    maxlength = 85
    Returns 1
    ↓
Line 24: return value == 1 → 1 == -1 → FALSE
    ↓
Skip line 25
    ↓
Continue to line 27
```

---

### Lines 27-28: Success Output

```cpp
cout << "File read successfully. Lines: " << linecounter 
     << ", Max Length: " << maxlength << endl;
```

**What it does:**
- Prints success message
- Shows statistics about the file
- Provides useful information to user

**Example Outputs:**

```
File read successfully. Lines: 150, Max Length: 85
File read successfully. Lines: 42, Max Length: 120
File read successfully. Lines: 1, Max Length: 10
```

**Breaking down the output:**

```cpp
cout << "File read successfully. Lines: "  // Static text
     << linecounter                         // Variable (e.g., 150)
     << ", Max Length: "                    // Static text
     << maxlength                           // Variable (e.g., 85)
     << endl;                               // New line
```

**Why this output is useful:**
- Confirms file was processed
- Shows number of lines (for validation)
- Shows longest line (for buffer allocation planning)
- Helps debug if values seem wrong

---

### Line 29: Success Return

```cpp
return 1;
```

**What it does:**
- Exits `main()` function
- Returns success code `1` to operating system
- Program terminates normally

**Why `1` instead of `0`?**

**Convention varies:**
```cpp
// Standard C/C++ convention:
return 0;   // Success
return 1;   // Error

// This program's convention:
return 1;   // Success
return -1;  // Error
```

**Our program uses:**
- `1` for success (positive result)
- `-1` for error (negative result)

**Both are valid!** What matters is consistency within the program.

---

## Execution Flow Diagram

### Complete Flow Chart

```
START
  ↓
[1] main(argc, argv) called
  ↓
[2] Check: argc == 5?
  ├─ NO → Print error → return -1 → END
  └─ YES → Continue
  ↓
[3] Check: argv[1] == "-d"?
  ├─ NO → Print error → return -1 → END
  └─ YES → Continue
  ↓
[4] Check: argv[3] == "-k"?
  ├─ NO → Print error → return -1 → END
  └─ YES → Continue
  ↓
[5] Print "Please wait..."
  ↓
[6] Initialize: linecounter=0, maxlength=-1
  ↓
[7] TRY: k = stoi(argv[4])
  ↓
[8] Success?
  ├─ NO → CATCH → Print error → return -1 → END
  └─ YES → Continue
  ↓
[9] Call: read_sizes(&linecounter, &maxlength, argv[2])
  ↓
[10] Return value == -1?
  ├─ YES → return -1 → END
  └─ NO → Continue
  ↓
[11] Print success message with statistics
  ↓
[12] return 1
  ↓
END
```

---

### Step-by-Step Execution Example

**Command:**
```bash
./searchengine -d data/doc1.txt -k 10
```

**Execution Trace:**

```
Step 1: Program starts
    argc = 5
    argv[0] = "./searchengine"
    argv[1] = "-d"
    argv[2] = "data/doc1.txt"
    argv[3] = "-k"
    argv[4] = "10"

Step 2: Check argc != 5
    5 != 5 → FALSE ✓

Step 3: Check strcmp(argv[1], "-d") != 0
    strcmp("-d", "-d") = 0
    0 != 0 → FALSE ✓

Step 4: Check strcmp(argv[3], "-k") != 0
    strcmp("-k", "-k") = 0
    0 != 0 → FALSE ✓

Step 5: All conditions FALSE → Skip error block
    Continue to line 13

Step 6: Print "Please wait..."
    Output: Please wait...

Step 7: Initialize variables
    linecounter = 0
    maxlength = -1

Step 8: TRY block - Convert string to integer
    k = stoi("10")
    k = 10 ✓

Step 9: No exception thrown
    Skip CATCH block

Step 10: Call read_sizes()
    read_sizes(&linecounter, &maxlength, "data/doc1.txt")
    
    Inside read_sizes():
        Opens file successfully
        Reads lines:
            Line 1: "Hello World" (11 chars)
            Line 2: "This is a test document" (23 chars)
            Line 3: "Testing" (7 chars)
        linecounter = 3
        maxlength = 23
        Returns 1
    
    Back in main():
        linecounter = 3
        maxlength = 23

Step 11: Check return value == -1
    1 == -1 → FALSE
    Skip return -1

Step 12: Print success message
    Output: File read successfully. Lines: 3, Max Length: 23

Step 13: return 1
    Program exits with success code

END

Final Output:
    Please wait...
    File read successfully. Lines: 3, Max Length: 23
```

---

## Example Usage Scenarios

### Scenario 1: Correct Usage ✓

**Command:**
```bash
./searchengine -d documents/report.txt -k 5
```

**Execution:**
```
Arguments validated ✓
    argc = 5
    argv[1] = "-d"
    argv[2] = "documents/report.txt"
    argv[3] = "-k"
    argv[4] = "5"

Output:
    Please wait...
    File read successfully. Lines: 234, Max Length: 156

Exit code: 1 (success)
```

---

### Scenario 2: Wrong Number of Arguments ✗

**Command:**
```bash
./searchengine -d myfile.txt
```

**Execution:**
```
argc = 3
Check: argc != 5
    3 != 5 → TRUE
    
Output:
    Wrong arguments. Usage: -d <file> -k <number>

Exit code: -1 (error)
```

---

### Scenario 3: Wrong Flag Order ✗

**Command:**
```bash
./searchengine -k 10 -d myfile.txt
```

**Execution:**
```
argc = 5 ✓
Check: argv[1] == "-d"
    argv[1] = "-k"
    strcmp("-k", "-d") ≠ 0 → TRUE
    
Output:
    Wrong arguments. Usage: -d <file> -k <number>

Exit code: -1 (error)
```

---

### Scenario 4: Invalid Number ✗

**Command:**
```bash
./searchengine -d myfile.txt -k abc
```

**Execution:**
```
Arguments validated ✓

Output: Please wait...

TRY: k = stoi("abc")
    Exception thrown!

CATCH:
    Output: Invalid value for -k (must be an integer)

Exit code: -1 (error)
```

---

### Scenario 5: File Not Found ✗

**Command:**
```bash
./searchengine -d missing.txt -k 10
```

**Execution:**
```
Arguments validated ✓
Number parsed ✓ (k = 10)

Output: Please wait...

read_sizes() called
    fopen("missing.txt") returns NULL
    Output: Cannot open file: missing.txt
    Returns -1

Back in main():
    Return value == -1 → TRUE
    return -1

Exit code: -1 (error)
```

---

### Scenario 6: Empty File ✗

**Command:**
```bash
./searchengine -d empty.txt -k 10
```

**Execution:**
```
Arguments validated ✓
Number parsed ✓

Output: Please wait...

read_sizes() called
    File opened successfully
    fgetc() returns EOF (empty file)
    Output: File is empty: empty.txt
    Returns -1

Back in main():
    Return value == -1 → TRUE
    return -1

Exit code: -1 (error)
```

---

### Scenario 7: Large Number ✓

**Command:**
```bash
./searchengine -d bigfile.txt -k 1000000
```

**Execution:**
```
Arguments validated ✓
k = stoi("1000000") = 1000000 ✓

Output: Please wait...

read_sizes() processes file successfully
    linecounter = 50000
    maxlength = 2048
    Returns 1

Output:
    File read successfully. Lines: 50000, Max Length: 2048

Exit code: 1 (success)
```

---

### Scenario 8: Negative Number ✓

**Command:**
```bash
./searchengine -d data.txt -k -5
```

**Execution:**
```
Arguments validated ✓
k = stoi("-5") = -5 ✓

Output: Please wait...
File read successfully. Lines: 100, Max Length: 75

Exit code: 1 (success)

Note: Program accepts negative numbers!
      Validation logic for k value can be added if needed
```

---

## Error Handling Summary

### All Error Cases

| Error Type | Detection Point | Error Message | Exit Code |
|------------|----------------|---------------|-----------|
| Wrong argument count | Line 6 | "Wrong arguments. Usage: -d <file> -k <number>" | -1 |
| Missing `-d` flag | Line 7 | "Wrong arguments. Usage: -d <file> -k <number>" | -1 |
| Missing `-k` flag | Line 8 | "Wrong arguments. Usage: -d <file> -k <number>" | -1 |
| Invalid integer | Line 19 | "Invalid value for -k (must be an integer)" | -1 |
| File not found | `read_sizes()` | "Cannot open file: <filename>" | -1 |
| File empty | `read_sizes()` | "File is empty: <filename>" | -1 |

---

### Error Detection Flow

```
Command-Line Errors (Caught immediately)
    ↓
    ├─ Argument count wrong
    ├─ Flag position wrong
    └─ Flag spelling wrong
    ↓
Integer Conversion Errors (Caught in try-catch)
    ↓
    ├─ Non-numeric input
    ├─ Decimal numbers
    └─ Empty string
    ↓
File Processing Errors (Caught in read_sizes)
    ↓
    ├─ File doesn't exist
    ├─ File empty
    └─ Permission denied
```

---

## Variable Tracking Table

### Variable States Throughout Execution

**Successful Execution Example:**

| Line | linecounter | maxlength | k | Notes |
|------|-------------|-----------|---|-------|
| 14 | 0 | - | - | Initialized |
| 15 | 0 | -1 | - | Initialized |
| 16 | 0 | -1 | <uninitialized> | Declared |
| 18 | 0 | -1 | 10 | Converted from argv[4] |
| 24 (after) | 150 | 85 | 10 | Updated by read_sizes() |
| 27 | 150 | 85 | 10 | Used in output |

---

## Program Structure Analysis

### Code Organization

**Layers:**
```
main() [Searchengine.cpp]
    ↓
Argument Validation
    ↓
User Interface (cout messages)
    ↓
read_sizes() [document_store.cpp]
    ↓
File I/O Operations (fopen, getline, etc.)
```

**Separation of Concerns:**

| File | Responsibility |
|------|----------------|
| `Searchengine.cpp` | Command-line interface, argument parsing, user interaction |
| `document_store.cpp` | File processing, line counting, low-level I/O |

**Benefits:**
- Clean separation
- Easy to test each component
- Reusable `read_sizes()` function
- Main function stays simple and readable

---

## Key Takeaways

### Important Concepts

1. **Command-Line Arguments**
   - `argc` = argument count
   - `argv` = argument vector (array of strings)
   - Always includes program name as argv[0]

2. **String Comparison**
   - Use `strcmp()` for C-style strings
   - Cannot use `==` (compares addresses, not content)
   - Return value: 0 = equal, non-zero = different

3. **Exception Handling**
   - `try-catch` blocks protect against runtime errors
   - `stoi()` throws exceptions on invalid input
   - Better than `atoi()` which silently fails

4. **Pointer Parameters**
   - Use `&` to pass address of variable
   - Allows function to modify original variable
   - Common pattern for "returning" multiple values

5. **Error Codes**
   - `-1` indicates error
   - `1` indicates success
   - Consistent throughout program

---

### Program Flow Summary

```
1. Validate command-line arguments (argc, flags)
2. Notify user processing started
3. Initialize counters
4. Convert string parameter to integer (with error handling)
5. Call read_sizes() to process file
6. Check for errors from read_sizes()
7. Display results
8. Return success code
```

---

### Current Functionality

**What the program does:**
- ✅ Validates command-line arguments
- ✅ Parses file path and number parameter
- ✅ Counts lines in file
- ✅ Finds maximum line length
- ✅ Displays results

**What's next (future development):**
- 🔲 Use `k` parameter (currently unused!)
- 🔲 Implement search functionality
- 🔲 Read search queries
- 🔲 Return top k results
- 🔲 Index document for fast searching

---

### Code Quality

**Strengths:**
- ✅ Clear error messages
- ✅ Comprehensive validation
- ✅ Exception handling
- ✅ Good user feedback
- ✅ Separation of concerns

**Potential Improvements:**
```cpp
// Add validation for k value
if(k <= 0) {
    cout << "Error: k must be positive" << endl;
    return -1;
}

// Use k parameter (currently unused)
// Future: implement actual search with k results

// Add more specific error handling
catch(invalid_argument& e) {
    cout << "Invalid integer format" << endl;
}
catch(out_of_range& e) {
    cout << "Number too large" << endl;
}
```

---

## Summary

This `Searchengine.cpp` file serves as the **main entry point** for the search engine application. It handles:

1. **Command-line interface** - Parses and validates user input
2. **Error handling** - Comprehensive checks at multiple levels
3. **File processing coordination** - Calls `read_sizes()` from document_store
4. **User feedback** - Clear messages for all scenarios

**Current State:**
- Foundation for search engine
- Argument parsing complete
- File reading complete
- Ready for search implementation

**Key Functions:**
- `main()` - Entry point
- `strcmp()` - String comparison
- `stoi()` - String to integer conversion
- `read_sizes()` - File processing (from document_store)

---

**Document Version**: 1.0  
**Last Updated**: December 23, 2025  
**Author**: High-Performance Search Engine Project  
**Repository**: github.com/adarshpheonix2810/high-performance-search-engine-cpp