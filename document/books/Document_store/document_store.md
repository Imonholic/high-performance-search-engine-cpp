# Document Store - C File I/O Functions Documentation

This document provides comprehensive explanations of all C file I/O functions and concepts used in the `document_store.cpp` implementation of our high-performance search engine.

---

## Table of Contents
1. [fopen() - Opening Files](#topic-1-fopen---opening-files)
2. [fclose() - Closing Files](#topic-2-fclose---closing-files)
3. [fgetc() - Reading Characters](#topic-3-fgetc---reading-characters)
4. [eof() - End-of-File Detection](#topic-4-eof---end-of-file-detection)
5. [ungetc() - Pushing Characters Back](#topic-5-ungetc---pushing-characters-back)
6. [size_t - Unsigned Integer Type](#topic-6-size_t---unsigned-integer-type)
7. [getline() - Reading Lines (POSIX)](#topic-7-getline---reading-lines-posix)
8. [getline() Windows Problem & Solutions](#topic-8-getline-windows-problem--solutions)
9. [free() - Deallocating Memory](#topic-9-free---deallocating-memory)

---

## Topic 1: fopen() - Opening Files

### What is fopen()?
`fopen()` is a C standard library function used to **open a file** and establish a connection between the program and the file. It returns a pointer to a `FILE` object that controls the file stream.

### Syntax
```c
FILE* fopen(const char* filename, const char* mode);
```

### Parameters
- **filename**: Path to the file (relative or absolute)
- **mode**: String specifying the file access mode

### Common File Modes

| Mode | Description | File Must Exist? | Overwrites? |
|------|-------------|------------------|-------------|
| `"r"` | Read only | Yes | No |
| `"w"` | Write only | No | Yes |
| `"a"` | Append | No | No |
| `"r+"` | Read and write | Yes | No |
| `"w+"` | Read and write | No | Yes |
| `"a+"` | Read and append | No | No |
| `"rb"` | Read binary | Yes | No |
| `"wb"` | Write binary | No | Yes |

### Return Value
- **Success**: Returns a pointer to `FILE` object
- **Failure**: Returns `NULL` (file doesn't exist, permission denied, etc.)

### Practical Example from document_store.cpp
```cpp
FILE *file = fopen(file_name, "r");
if (file == NULL) {
    cout << "Error: Cannot open file " << file_name << endl;
    return -1;
}
```

### Visual Representation
```
Program Memory                    Disk Storage
+-----------+                    +-------------+
|  FILE*    |  fopen("r")       |   doc1.txt  |
|   file    | ----------------> |   Content   |
| (pointer) | <---------------- |   Lines...  |
+-----------+   Read Access     +-------------+
```

### Key Points
- Always check if `fopen()` returns `NULL` before using the file pointer
- Use `"r"` mode when you only need to read the file
- Use `"b"` suffix for binary files on Windows to prevent newline translation
- File pointer must be closed with `fclose()` after use

---

## Topic 2: fclose() - Closing Files

### What is fclose()?
`fclose()` is a C standard library function used to **close an open file stream** and release all resources associated with it. It also flushes any buffered data to the file.

### Syntax
```c
int fclose(FILE* stream);
```

### Parameters
- **stream**: Pointer to the `FILE` object to be closed

### Return Value
- **Success**: Returns `0`
- **Failure**: Returns `EOF` (typically -1) if an error occurs

### What Happens When You Call fclose()?
1. **Flushes Buffers**: Writes any unwritten data from buffer to file
2. **Releases Resources**: Frees memory allocated for the file stream
3. **Disassociates Stream**: Breaks the connection between program and file
4. **Makes Pointer Invalid**: Using the pointer after `fclose()` causes undefined behavior

### Practical Example from document_store.cpp
```cpp
FILE *file = fopen(file_name, "r");
// ... read from file ...
fclose(file);
```

### Why fclose() is Critical?

**Problem Without fclose():**
```cpp
FILE *file = fopen("data.txt", "w");
fprintf(file, "Important data");
// Program crashes or exits without fclose()
// ❌ Data might be lost! (still in buffer)
```

**Solution With fclose():**
```cpp
FILE *file = fopen("data.txt", "w");
fprintf(file, "Important data");
fclose(file);  // ✅ Data safely written to disk
```

### Memory Leak Example
```cpp
// ❌ BAD: Memory leak
for(int i = 0; i < 1000; i++) {
    FILE *f = fopen("temp.txt", "r");
    // ... use file ...
    // Missing fclose(f) - leaks resources!
}

// ✅ GOOD: Properly closes files
for(int i = 0; i < 1000; i++) {
    FILE *f = fopen("temp.txt", "r");
    // ... use file ...
    fclose(f);  // Releases resources
}
```

### Visual Representation
```
Before fclose():              After fclose():
+-------------+              +-------------+
|   Program   |              |   Program   |
|             |              |             |
|   FILE*  ---|---> [Buffer] |   FILE* = ? |
|             |       ↓      |             |
+-------------+   [Disk File]+-------------+
                              [File Closed]
```

### Key Points
- **Always** call `fclose()` for every successful `fopen()`
- Check the return value to detect write errors
- Using a closed file pointer results in undefined behavior
- OS limits the number of open files - closing prevents resource exhaustion

---

## Topic 3: fgetc() - Reading Characters

### What is fgetc()?
`fgetc()` is a C standard library function that **reads the next character** from an input stream and returns it as an integer. It's commonly used for character-by-character file reading.

### Syntax
```c
int fgetc(FILE* stream);
```

### Parameters
- **stream**: Pointer to the `FILE` object from which to read

### Return Value
- **Success**: Returns the character as an `unsigned char` cast to `int`
- **End of File or Error**: Returns `EOF` (typically -1)

### Why Return Type is int (Not char)?
```c
// EOF is typically -1 (negative value)
// char range: -128 to 127 (signed) or 0 to 255 (unsigned)
// int range: -2,147,483,648 to 2,147,483,647

int c = fgetc(file);
if (c == EOF) {  // Can distinguish EOF from valid character 255
    // End of file or error
}
```

**Problem with char return type:**
- Character with value 255 might be confused with EOF (-1)
- `int` safely holds all possible `char` values PLUS special `EOF` value

### Practical Example from document_store.cpp
```cpp
// Check if file is empty
int c = fgetc(file);
if(c == EOF){
    cout << "File is empty: " << file_name << endl;
    fclose(file);
    return -1;
}
ungetc(c, file);  // Put character back
```

### Reading File Character by Character
```cpp
FILE *file = fopen("document.txt", "r");
int c;

while((c = fgetc(file)) != EOF) {
    printf("%c", c);  // Print each character
}

fclose(file);
```

### Detecting Errors vs EOF
```cpp
int c = fgetc(file);
if (c == EOF) {
    if (feof(file)) {
        printf("End of file reached\n");
    } else if (ferror(file)) {
        printf("Read error occurred\n");
    }
}
```

### Visual Representation
```
File: "Hello"           Memory
+---+---+---+---+---+
| H | e | l | l | o |
+---+---+---+---+---+
  ↑
  File Position Indicator

1st fgetc() → Returns 'H' (72)
2nd fgetc() → Returns 'e' (101)
3rd fgetc() → Returns 'l' (108)
4th fgetc() → Returns 'l' (108)
5th fgetc() → Returns 'o' (111)
6th fgetc() → Returns EOF (-1)
```

### Performance Consideration
```cpp
// ❌ SLOW: Character-by-character reading
while((c = fgetc(file)) != EOF) {
    process(c);
}

// ✅ FASTER: Buffer reading
char buffer[4096];
while(fgets(buffer, sizeof(buffer), file) != NULL) {
    process(buffer);
}
```

### Key Points
- Returns `int` to accommodate `EOF` special value
- Advances file position indicator by 1 byte
- Can be used to peek at the first character (with `ungetc()`)
- Slower than buffered reading for large files
- Use `feof()` and `ferror()` to distinguish EOF from errors

---

## Topic 4: eof() - End-of-File Detection

### What is eof()?
`eof()` is a function used to check whether the **End-of-File (EOF) condition** has been reached during file input operations. In C, the equivalent functions are `feof()` for `FILE*` streams.

### Two Types of EOF Functions

#### 1. C++ Stream Version (std::ifstream)
```cpp
#include <fstream>

std::ifstream file("data.txt");
if (file.eof()) {
    std::cout << "End of file reached\n";
}
```

#### 2. C File Version (FILE*)
```cpp
#include <stdio.h>

FILE *file = fopen("data.txt", "r");
if (feof(file)) {
    printf("End of file reached\n");
}
```

### Important: When is eof() True?

**Critical Concept**: `eof()` returns `true` **ONLY AFTER** a read operation attempts to read **BEYOND** the end of the file.

### Common Misconception Example
```cpp
// ❌ WRONG LOGIC
FILE *file = fopen("data.txt", "r");
while(!feof(file)) {  // Checks EOF before reading
    char buffer[100];
    fgets(buffer, 100, file);  // Might read garbage on last iteration
    printf("%s", buffer);
}
```

**Problem**: `feof()` is false even at the last valid character. The loop reads one extra time, processing garbage or duplicating the last line.

### Correct Pattern
```cpp
// ✅ CORRECT LOGIC
FILE *file = fopen("data.txt", "r");
char buffer[100];

while(fgets(buffer, 100, file) != NULL) {  // Check return value
    printf("%s", buffer);
}

fclose(file);
```

### Visual Representation
```
File Content: "AB"
Position:      0  1  2
               +--+--+
               |A |B |EOF
               +--+--+
               
Read #1: fgetc() → 'A', feof() → false
Read #2: fgetc() → 'B', feof() → false
Read #3: fgetc() → EOF, feof() → TRUE ✓
```

### Detecting Read Errors vs EOF
```cpp
FILE *file = fopen("data.txt", "r");
int c;

while((c = fgetc(file)) != EOF) {
    putchar(c);
}

// Now determine WHY we stopped
if (feof(file)) {
    printf("End of file reached normally\n");
} else if (ferror(file)) {
    printf("Error occurred during reading\n");
}

fclose(file);
```

### EOF Value
```cpp
// EOF is a macro defined in <stdio.h>
#define EOF (-1)

// It's a negative integer value that cannot be a valid character
int c = fgetc(file);
if (c == EOF) {
    // Could be end-of-file OR read error
}
```

### Comparison Table

| Aspect | feof() | Return Value Check |
|--------|--------|-------------------|
| Timing | After read fails | During read |
| Best for | Error distinction | Normal reading |
| Usage | `if(feof(file))` | `if(fgets(...) != NULL)` |
| Precision | Less precise | More precise |

### Practical Example: Reading Until EOF
```cpp
FILE *file = fopen("numbers.txt", "r");
int number;

// ✅ CORRECT: Check fscanf return value
while(fscanf(file, "%d", &number) == 1) {
    printf("Read: %d\n", number);
}

// Now check why we stopped
if (feof(file)) {
    printf("Finished reading all numbers\n");
} else {
    printf("Error: Invalid data format\n");
}

fclose(file);
```

### Key Points
- `feof()` returns `true` ONLY AFTER attempting to read past EOF
- Never use `while(!feof(file))` as a loop condition
- Always check read function return values instead
- Use `feof()` and `ferror()` together to diagnose why reading stopped
- EOF is typically defined as -1

---

## Topic 5: ungetc() - Pushing Characters Back

### What is ungetc()?
`ungetc()` is a C standard library function that **pushes a character back** into an input stream, allowing it to be read again by the next read operation. It's like "un-reading" a character.

### Syntax
```c
int ungetc(int ch, FILE* stream);
```

### Parameters
- **ch**: Character to push back (as `int`)
- **stream**: Pointer to the `FILE` object

### Return Value
- **Success**: Returns the pushed character `ch`
- **Failure**: Returns `EOF` (if `ch` is `EOF` or operation fails)

### How ungetc() Works

**Visual Representation:**
```
Stream: "Hello"
Position: ↓
         H e l l o

Step 1: int c = fgetc(file);  // Reads 'H'
        Position: ↓
                 H e l l o
                   ↑

Step 2: ungetc(c, file);      // Pushes 'H' back
        Position: ↓
         H e l l o
         ↑ (back to start)

Step 3: c = fgetc(file);      // Reads 'H' again
```

### Practical Example from document_store.cpp

**Empty File Detection:**
```cpp
// Check if file is empty by peeking at first character
int c = fgetc(file);      // Read first character
if(c == EOF){             // File is empty
    cout << "File is empty: " << file_name << endl;
    fclose(file);
    return -1;
}
ungetc(c, file);          // Put character back
// Now file position is back at start
```

**Why This Pattern?**
- We want to check if file is empty WITHOUT consuming its content
- `fgetc()` tells us if there's data (returns char) or not (returns EOF)
- `ungetc()` restores the file to its original state
- Subsequent `getline()` can read from the beginning

### Another Use Case: Token Lookahead

**Parsing Numbers:**
```cpp
FILE *file = fopen("data.txt", "r");
int c;

while((c = fgetc(file)) != EOF) {
    if(isdigit(c)) {
        ungetc(c, file);  // Put digit back
        int number;
        fscanf(file, "%d", &number);  // Read full number
        printf("Found number: %d\n", number);
    }
}

fclose(file);
```

### Important Rules and Limitations

#### 1. Only One Pushback Guaranteed
```cpp
int c1 = fgetc(file);     // Read 'A'
int c2 = fgetc(file);     // Read 'B'

ungetc(c2, file);         // ✅ Push 'B' back - OK
ungetc(c1, file);         // ❌ Push 'A' back - NOT GUARANTEED
                          // Only 1 character pushback is portable
```

#### 2. Cannot Push EOF
```cpp
ungetc(EOF, file);        // ❌ Does nothing, returns EOF
```

#### 3. Affects File Position
```cpp
FILE *file = fopen("test.txt", "r");
long pos1 = ftell(file);      // Position: 0

int c = fgetc(file);          // Position: 1
long pos2 = ftell(file);      

ungetc(c, file);              // Position: back to 0
long pos3 = ftell(file);      

printf("Pos: %ld → %ld → %ld\n", pos1, pos2, pos3);
// Output: Pos: 0 → 1 → 0
```

### Comparison with Other Methods

| Method | Purpose | Modifies File? | Efficiency |
|--------|---------|----------------|------------|
| `ungetc()` | Push back 1 char | No | Fast |
| `fseek(file, -1, SEEK_CUR)` | Move back 1 byte | No | Medium |
| Read into buffer | Store multiple chars | No | Slow (memory) |

### Alternative: Buffered Reading
```cpp
// Instead of ungetc(), use larger buffer
char buffer[100];
fgets(buffer, 100, file);  // Read line into buffer
// Process buffer without needing to push back
```

### Common Pitfall: Binary Files
```cpp
// ⚠️ WARNING: ungetc() on binary files
FILE *file = fopen("image.png", "rb");
int byte = fgetc(file);
ungetc(byte, file);  // Works, but use fseek() for binary
                      // fseek() is more reliable for binary data
```

### Key Points
- `ungetc()` allows "peeking" at next character without consuming it
- Only **one character** pushback is guaranteed by C standard
- Cannot push back `EOF`
- Perfect for checking file emptiness before reading
- Updates file position indicator backwards
- Alternative: use `fseek(file, -1, SEEK_CUR)` for moving back

---

## Topic 6: size_t - Unsigned Integer Type

### What is size_t?
`size_t` is an **unsigned integer data type** defined in the C/C++ standard library specifically designed to represent the **size of objects** in bytes. It's guaranteed to be large enough to hold the size of the largest possible object on the system.

### Declaration
```cpp
#include <cstddef>  // C++
#include <stddef.h> // C

size_t size = sizeof(int);
```

### Why size_t Exists?

**Portability Problem:**
```cpp
// ❌ NON-PORTABLE: Assumes int can hold any size
int size = sizeof(large_array);  
// On 64-bit systems, arrays can be > 4GB
// int max: 2,147,483,647 (~ 2GB)
// This will overflow!

// ✅ PORTABLE: size_t adjusts to platform
size_t size = sizeof(large_array);
// On 32-bit: size_t is 32-bit (max 4GB)
// On 64-bit: size_t is 64-bit (max 16 exabytes)
```

### Platform Sizes

| Platform | size_t Size | Maximum Value |
|----------|-------------|---------------|
| 32-bit | 4 bytes | 4,294,967,295 (~ 4GB) |
| 64-bit | 8 bytes | 18,446,744,073,709,551,615 (~ 16 EB) |

### Common Uses of size_t

#### 1. sizeof Operator
```cpp
size_t int_size = sizeof(int);          // 4 bytes
size_t array_size = sizeof(int) * 100;  // 400 bytes
```

#### 2. String Length
```cpp
#include <cstring>

char str[] = "Hello";
size_t length = strlen(str);  // Returns size_t (5)
```

#### 3. Array Indexing
```cpp
int numbers[100];
for(size_t i = 0; i < 100; i++) {
    numbers[i] = i;
}
```

#### 4. Memory Allocation
```cpp
#include <cstdlib>

size_t num_bytes = 1024;
char *buffer = (char*)malloc(num_bytes);
free(buffer);
```

### Practical Example from document_store.cpp

```cpp
size_t len = 0;  // Used with getline()
char *line = NULL;
ssize_t current_length;

// getline() expects size_t* for buffer size
while((current_length = getline(&line, &len, file)) != -1) {
    // len is updated by getline() with buffer size
    // current_length is the actual line length read
}
```

### size_t vs int: Key Differences

| Aspect | size_t | int |
|--------|--------|-----|
| Signed | No (unsigned) | Yes (signed) |
| Negative values | Cannot represent | Can represent |
| Size | Platform-dependent | Usually 4 bytes |
| Purpose | Sizes, counts | General numbers |
| Example | `100` elements | `-50` temperature |

### Common size_t Mistake: Underflow

```cpp
// ❌ DANGER: Unsigned underflow
size_t a = 5;
size_t b = 10;
size_t result = a - b;  // Wraps around!
// result = 18,446,744,073,709,551,611 (not -5!)

printf("%zu\n", result);  // Huge positive number
```

**Visual Representation:**
```
Unsigned Integer Wheel (size_t):
    MAX VALUE
        ↑
  ...  │  ...
   2   │   MAX-2
   1   │   MAX-1
   0 ──┴── MAX
   
5 - 10 wraps around:
5 → 4 → 3 → 2 → 1 → 0 → MAX → MAX-1 → ... → MAX-4
                        (wraps to huge positive)
```

### Safe Comparison with Signed Integers

```cpp
int signed_num = -5;
size_t unsigned_num = 10;

// ❌ WRONG: Dangerous comparison
if(signed_num < unsigned_num) {  
    // signed_num is converted to size_t
    // -5 becomes huge positive number!
    // Condition is FALSE (unexpected)
}

// ✅ CORRECT: Cast size_t to int
if(signed_num < (int)unsigned_num) {
    // Now both are signed
    // Condition is TRUE (expected)
}
```

### Related Types

```cpp
#include <cstddef>

size_t   // Unsigned size type
ssize_t  // Signed size type (POSIX, not standard C++)
ptrdiff_t // Signed type for pointer differences

// Example:
char *start = buffer;
char *end = buffer + 100;
ptrdiff_t distance = end - start;  // 100
```

### Printf/Scanf Format Specifiers

```cpp
size_t value = 42;

// ✅ CORRECT format specifiers
printf("%zu\n", value);   // C99/C++11: size_t
printf("%zx\n", value);   // Hexadecimal size_t

// ❌ WRONG (common mistake)
printf("%d\n", value);    // Treats size_t as int - unsafe!
printf("%lu\n", value);   // Treats size_t as long - not portable!
```

### Practical Size Limits

```cpp
#include <cstddef>
#include <limits>

// Maximum value of size_t
size_t max = std::numeric_limits<size_t>::max();

// 32-bit system: 4,294,967,295 (~ 4 GB)
// 64-bit system: 18,446,744,073,709,551,615 (~ 16 EB)
```

### When NOT to Use size_t

```cpp
// ❌ Don't use for negative values
size_t temperature = -10;  // Wraps to huge positive!

// ❌ Don't use for loop counts that go backwards
for(size_t i = 10; i >= 0; i--) {  // Infinite loop!
    // i never goes negative, wraps to MAX at i-1 when i=0
}

// ✅ Use int for these cases
int temperature = -10;  // OK
for(int i = 10; i >= 0; i--) {  // OK
```

### Key Points
- `size_t` is **unsigned** - cannot represent negative values
- Automatically adjusts size on different platforms (32-bit vs 64-bit)
- Used by `sizeof`, `strlen`, `malloc`, and similar functions
- Beware of underflow: subtracting larger from smaller wraps around
- Use `%zu` format specifier with `printf`
- Use `int` or `ptrdiff_t` if negative values are possible

---

## Topic 7: getline() - Reading Lines (POSIX)

### What is getline()?
`getline()` is a **POSIX function** (not standard C) that reads an entire line from a file stream, including the newline character. It automatically allocates memory for the line, making it convenient for reading lines of unknown length.

### Syntax
```c
ssize_t getline(char **lineptr, size_t *n, FILE *stream);
```

### Parameters
- **lineptr**: Pointer to buffer (allocated automatically by `getline()`)
- **n**: Pointer to buffer size (updated by `getline()`)
- **stream**: File stream to read from

### Return Value
- **Success**: Returns number of characters read (including newline, excluding null terminator)
- **EOF or Error**: Returns -1

### How getline() Works

**Memory Management:**
```cpp
char *line = NULL;    // Start with NULL pointer
size_t len = 0;       // Start with 0 size
ssize_t read;

// First call: getline() allocates memory
read = getline(&line, &len, file);
// After: line points to malloc'd buffer, len = buffer size

// Subsequent calls: getline() reuses or expands buffer
read = getline(&line, &len, file);

// Must free when done
free(line);
```

### Visual Representation
```
Before getline():          After getline():
line = NULL               line ──→ +---+---+---+---+---+---+
len = 0                            | H | e | y |\n | \0|   |
                                   +---+---+---+---+---+---+
                          len = 6 (buffer capacity)
                          read = 4 (actual characters including \n)
```

### Practical Example from document_store.cpp

```cpp
FILE *file = fopen(file_name, "r");
char *line = NULL;
size_t len = 0;
ssize_t current_length;

// Read lines until EOF
while((current_length = getline(&line, &len, file)) != -1) {
    *linecounter = *linecounter + 1;  // Count lines
    
    // Remove newline from length calculation
    if(line[current_length-1] == '\n') {
        current_length--;
    }
    
    // Track maximum line length
    if(current_length > *maxlength) {
        *maxlength = current_length;
    }
}

free(line);   // Must free allocated memory
fclose(file);
```

### Key Features

#### 1. Automatic Memory Allocation
```cpp
// ✅ getline() manages memory automatically
char *line = NULL;
size_t len = 0;
getline(&line, &len, file);  // Allocates memory

// ❌ fgets() requires pre-allocated buffer
char buffer[256];
fgets(buffer, 256, file);  // Limited to 256 chars
```

#### 2. Handles Arbitrary Line Length
```cpp
// getline() can read lines of ANY length
// It automatically expands buffer if needed

// Read 10-character line: allocates 10+ bytes
// Read 10,000-character line: expands to 10,000+ bytes
```

#### 3. Includes Newline Character
```cpp
char *line = NULL;
size_t len = 0;
ssize_t read = getline(&line, &len, file);

// If file contains: "Hello\n"
// line = "Hello\n\0"  (newline included)
// read = 6

// To remove newline:
if(read > 0 && line[read-1] == '\n') {
    line[read-1] = '\0';  // Replace \n with \0
}
```

### getline() vs fgets()

| Feature | getline() | fgets() |
|---------|-----------|---------|
| Standard | POSIX only | C Standard |
| Buffer size | Auto-managed | Fixed size |
| Line length | Unlimited | Limited by buffer |
| Newline | Included | Included |
| Memory | Auto malloc() | Pre-allocated |
| Portability | Linux/Unix/MinGW | All platforms |

### Comparison Example

**Using fgets():**
```cpp
char buffer[100];
if(fgets(buffer, 100, file) != NULL) {
    // Can only read 99 characters
    // Long lines are truncated
}
```

**Using getline():**
```cpp
char *line = NULL;
size_t len = 0;
if(getline(&line, &len, file) != -1) {
    // Can read lines of any length
    // No truncation
}
free(line);
```

### Important Usage Rules

#### 1. Initialize Pointers Correctly
```cpp
// ✅ CORRECT
char *line = NULL;
size_t len = 0;
getline(&line, &len, file);

// ❌ WRONG: Uninitialized
char *line;        // Contains garbage
size_t len;        // Contains garbage
getline(&line, &len, file);  // Undefined behavior!
```

#### 2. Always Free Memory
```cpp
char *line = NULL;
size_t len = 0;

while(getline(&line, &len, file) != -1) {
    // Process line
}

free(line);  // ✅ Must free to avoid memory leak
```

#### 3. Reusing the Buffer
```cpp
char *line = NULL;
size_t len = 0;

// First getline: allocates new buffer
getline(&line, &len, file);  // Allocates 64 bytes

// Second getline: reuses buffer if big enough
getline(&line, &len, file);  // Reuses 64 bytes

// Third getline: expands if line is longer
getline(&line, &len, file);  // Expands to 128 bytes

free(line);  // Free once at the end
```

### Return Value Explained

```cpp
ssize_t read = getline(&line, &len, file);

// read = 5  → Read 5 characters (including \n)
// read = 0  → Empty line (just \n)
// read = -1 → EOF or error

if(read == -1) {
    if(feof(file)) {
        // End of file
    } else if(ferror(file)) {
        // Read error
    }
}
```

### Complete Working Example

```cpp
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *file = fopen("document.txt", "r");
    if(file == NULL) {
        perror("fopen");
        return 1;
    }
    
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_number = 1;
    
    while((read = getline(&line, &len, file)) != -1) {
        printf("Line %d (%zd chars): %s", line_number, read, line);
        line_number++;
    }
    
    free(line);
    fclose(file);
    return 0;
}
```

### Key Points
- `getline()` is a POSIX function (available on Linux, Unix, Windows with MinGW)
- Automatically allocates and manages memory for the line buffer
- Returns number of characters read (including newline)
- Returns -1 on EOF or error
- Must initialize `lineptr` to `NULL` and `n` to `0` for first call
- Must `free()` the allocated memory when done
- Includes newline character in the returned string
- Can read lines of arbitrary length (no truncation)

---

## Topic 8: getline() Windows Problem & Solutions

### The Problem: getline() Hangs on Empty Files (Windows)

#### What Happens?
On Windows systems with POSIX compatibility layers (like MinGW, Cygwin), `getline()` has a **critical bug**: it **hangs indefinitely** when called on an **empty file** instead of returning -1.

```cpp
// ❌ PROBLEMATIC CODE (Windows)
FILE *file = fopen("empty.txt", "r");  // File has 0 bytes
char *line = NULL;
size_t len = 0;

while(getline(&line, &len, file) != -1) {  // HANGS HERE! 
    // Never executes
}
// Program freezes forever
```

### Why This Happens?

**Expected Behavior (POSIX Standard):**
```
Empty file → fgetc() returns EOF → getline() returns -1 ✓
```

**Actual Behavior (Windows POSIX layer):**
```
Empty file → getline() waits for input → never returns ❌
```

**Root Cause**: Windows implementation of `getline()` doesn't properly check for EOF condition before attempting to read, causing it to wait for data that will never arrive.

### Visual Representation

```
Linux/Unix:                      Windows (MinGW):
+-----------+                    +-----------+
| Empty File|                    | Empty File|
+-----------+                    +-----------+
      ↓                                ↓
  getline()                        getline()
      ↓                                ↓
  Check EOF                         Wait for data...
      ↓                                ↓
  Return -1 ✓                      [HANGS] ❌
```

### Real-World Impact

```cpp
// This code works fine on Linux but hangs on Windows
int read_sizes(int *linecounter, int *maxlength, char *file_name){
    *linecounter = 0;
    *maxlength = 0;
    
    FILE *file = fopen(file_name, "r");
    if(file == NULL) {
        return -1;
    }
    
    char *line = NULL;
    size_t len = 0;
    ssize_t current_length;
    
    // ❌ If file is empty, program freezes here on Windows
    while((current_length = getline(&line, &len, file)) != -1) {
        // ...
    }
    
    free(line);
    fclose(file);
    return 0;
}
```

---

## Solutions to Empty File Problem

### Solution 1: Check File Empty Before getline() ⭐ RECOMMENDED

**Method A: Using fgetc() and ungetc() (Simplest)**

```cpp
FILE *file = fopen(file_name, "r");
if(file == NULL) {
    return -1;
}

// Check if file is empty
int c = fgetc(file);
if(c == EOF){
    cout << "File is empty: " << file_name << endl;
    fclose(file);
    return -1;
}
ungetc(c, file);  // Put character back

// Now safe to use getline()
char *line = NULL;
size_t len = 0;
while(getline(&line, &len, file) != -1) {
    // Process line
}

free(line);
fclose(file);
```

**Why This Works:**
- `fgetc()` reliably returns `EOF` on empty files (even on Windows)
- `ungetc()` puts the character back so file position is unchanged
- Only 3 lines of simple code
- **Beginner-friendliness: 10/10**

---

### Solution 2: Use fseek() and ftell()

```cpp
FILE *file = fopen(file_name, "r");
if(file == NULL) {
    return -1;
}

// Check file size
fseek(file, 0, SEEK_END);    // Go to end
long size = ftell(file);     // Get position (= file size)
rewind(file);                // Go back to start

if(size == 0){
    cout << "File is empty: " << file_name << endl;
    fclose(file);
    return -1;
}

// Now safe to use getline()
char *line = NULL;
size_t len = 0;
while(getline(&line, &len, file) != -1) {
    // Process line
}

free(line);
fclose(file);
```

**Pros:**
- Reliable on all platforms
- Gets exact file size

**Cons:**
- More complex (4 function calls)
- **Beginner-friendliness: 6/10**

---

### Solution 3: Use stat() System Call

```cpp
#include <sys/stat.h>

FILE *file = fopen(file_name, "r");
if(file == NULL) {
    return -1;
}

// Get file information
struct stat file_stat;
if(stat(file_name, &file_stat) == 0) {
    if(file_stat.st_size == 0) {
        cout << "File is empty: " << file_name << endl;
        fclose(file);
        return -1;
    }
}

// Now safe to use getline()
char *line = NULL;
size_t len = 0;
while(getline(&line, &len, file) != -1) {
    // Process line
}

free(line);
fclose(file);
```

**Pros:**
- Most efficient (no file reading needed)
- Provides additional file information

**Cons:**
- Requires understanding of structs and system calls
- **Beginner-friendliness: 4/10**

---

### Solution 4: Replace getline() with fgets() ⭐ BEST for Portability

```cpp
FILE *file = fopen(file_name, "r");
if(file == NULL) {
    return -1;
}

char buffer[4096];  // Fixed-size buffer

// fgets() returns NULL on empty file (no hang)
while(fgets(buffer, sizeof(buffer), file) != NULL) {
    int current_length = strlen(buffer);
    
    // Remove newline
    if(current_length > 0 && buffer[current_length-1] == '\n') {
        current_length--;
    }
    
    *linecounter = *linecounter + 1;
    if(current_length > *maxlength) {
        *maxlength = current_length;
    }
}

fclose(file);
```

**Pros:**
- `fgets()` is standard C (works everywhere)
- No empty file bug
- No memory allocation needed

**Cons:**
- Limited buffer size (truncates very long lines)
- **Beginner-friendliness: 9/10**

---

### Solution 5: Check EOF Before Loop

```cpp
FILE *file = fopen(file_name, "r");
if(file == NULL) {
    return -1;
}

char *line = NULL;
size_t len = 0;
ssize_t current_length;

// Check if file is empty using feof()
if(feof(file)) {
    cout << "File is empty: " << file_name << endl;
    fclose(file);
    return -1;
}

// Note: This doesn't work! feof() is false until AFTER read fails
// Including for completeness only
```

**Why This Doesn't Work:**
- `feof()` returns `false` until AFTER a read fails
- Empty file has `feof() == false` before first read
- **Not a solution** - included to explain common misconception

---

### Comparison Table

| Solution | Lines of Code | Complexity | Portability | Beginner-Friendly |
|----------|---------------|------------|-------------|-------------------|
| fgetc/ungetc | 3 | Low | High | ⭐⭐⭐⭐⭐ (10/10) |
| fseek/ftell | 4 | Medium | High | ⭐⭐⭐ (6/10) |
| stat() | 5+ | High | Medium | ⭐⭐ (4/10) |
| fgets() instead | Replace function | Low | Very High | ⭐⭐⭐⭐ (9/10) |

### Our Implementation Choice

**We chose fgetc()/ungetc() method because:**
1. ✅ Simplest code (3 lines)
2. ✅ Easy to understand for beginners
3. ✅ No complex system calls or structs
4. ✅ Reliable on all platforms
5. ✅ Doesn't change file position after check
6. ✅ Perfect for our use case

```cpp
// Final implementation in document_store.cpp
int c = fgetc(file);
if(c == EOF){
    cout << "File is empty: " << file_name << endl;
    fclose(file);
    return -1;
}
ungetc(c, file);
```

### Platform Compatibility Summary

| Platform | getline() Bug | fgetc/ungetc Works | fgets() Works |
|----------|---------------|-------------------|---------------|
| Linux | ❌ No bug | ✅ Yes | ✅ Yes |
| macOS | ❌ No bug | ✅ Yes | ✅ Yes |
| Windows (MinGW) | ✅ **HAS BUG** | ✅ Yes | ✅ Yes |
| Windows (Cygwin) | ✅ **HAS BUG** | ✅ Yes | ✅ Yes |
| Windows (MSVC) | ⚠️ No getline() | N/A | ✅ Yes |

### Key Points
- `getline()` hangs on empty files in Windows POSIX implementations
- Always check for empty files BEFORE calling `getline()` on Windows
- **Best simple solution**: Use `fgetc()` to peek, then `ungetc()` to restore
- **Best portable solution**: Replace `getline()` with `fgets()`
- `feof()` doesn't work for this check (returns false before first read)
- The bug only affects Windows - Linux/Unix work correctly

---

## Topic 9: free() - Deallocating Memory

### What is free()?
`free()` is a C standard library function used to **deallocate memory** that was previously allocated dynamically using `malloc()`, `calloc()`, or `realloc()`. It returns the memory to the system for reuse.

### Syntax
```c
void free(void *ptr);
```

### Parameters
- **ptr**: Pointer to memory block to deallocate

### Return Value
- **None** (`void`)

### Why free() is Critical?

**Memory Leak Without free():**
```cpp
// ❌ MEMORY LEAK
for(int i = 0; i < 1000000; i++) {
    char *buffer = (char*)malloc(1024);  // Allocate 1 KB
    // ... use buffer ...
    // Missing free(buffer)
    // Leaks 1 KB per iteration = 1 GB total!
}
```

**Proper Memory Management:**
```cpp
// ✅ CORRECT
for(int i = 0; i < 1000000; i++) {
    char *buffer = (char*)malloc(1024);  // Allocate 1 KB
    // ... use buffer ...
    free(buffer);  // Deallocate 1 KB
    // Memory is reused
}
```

### Practical Example from document_store.cpp

```cpp
char *line = NULL;
size_t len = 0;

// getline() allocates memory for line
while(getline(&line, &len, file) != -1) {
    // line points to dynamically allocated memory
    // ... process line ...
}

// Must free the allocated memory
free(line);  // Returns memory to system
fclose(file);
```

### How Memory Allocation Works

**Visual Representation:**
```
Program Memory:
+----------------+
|  Stack         | ← Local variables (automatic)
|  int x;        |
|  char c;       |
+----------------+
|  ↓ grows down  |
|                |
|  ↑ grows up    |
+----------------+
|  Heap          | ← Dynamic memory (manual)
|  malloc()      |
|  getline()     |
+----------------+
|  Global/Static | ← Global variables
+----------------+
|  Code          | ← Program instructions
+----------------+
```

**malloc() and free():**
```
Step 1: char *ptr = malloc(100);
        Heap: [■■■■■ allocated 100 bytes ■■■■■]
              ptr → [___|___|___| ... |___]
              
Step 2: ... use ptr ...
        ptr[0] = 'A';
        ptr → [_A_|___|___| ... |___]

Step 3: free(ptr);
        Heap: [_____ freed 100 bytes _____]
        ptr still contains address but is INVALID
```

### Important Rules for free()

#### 1. Only Free Dynamically Allocated Memory

```cpp
// ✅ CORRECT: Free malloc'd memory
char *p1 = (char*)malloc(100);
free(p1);

// ❌ WRONG: Don't free stack memory
char buffer[100];
free(buffer);  // ❌ CRASH! Not from malloc()

// ❌ WRONG: Don't free string literals
char *str = "Hello";
free(str);  // ❌ CRASH! Not from malloc()
```

#### 2. Don't Use Pointer After free()

```cpp
char *buffer = (char*)malloc(100);
buffer[0] = 'A';  // ✅ OK

free(buffer);     // Memory deallocated

buffer[0] = 'B';  // ❌ UNDEFINED BEHAVIOR!
// Memory might be reused by other code
printf("%c", buffer[0]);  // ❌ UNDEFINED BEHAVIOR!
```

#### 3. Don't free() Twice

```cpp
char *buffer = (char*)malloc(100);
free(buffer);     // ✅ First free - OK
free(buffer);     // ❌ DOUBLE FREE - CRASH!
```

#### 4. free(NULL) is Safe

```cpp
char *ptr = NULL;
free(ptr);  // ✅ Does nothing, safe
```

### Complete malloc/free Lifecycle

```cpp
// 1. Allocate memory
char *line = NULL;
size_t len = 0;

// 2. getline() internally calls malloc
ssize_t read = getline(&line, &len, file);
// Now: line → [memory allocated by getline]

// 3. Use the memory
if(read != -1) {
    printf("%s", line);
}

// 4. Free when done
free(line);  // Return memory to system
line = NULL; // Good practice: set to NULL after free
```

### Memory Leak Detection Example

**Without free():**
```cpp
void process_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    char *line = NULL;
    size_t len = 0;
    
    while(getline(&line, &len, file) != -1) {
        // Process line
    }
    
    fclose(file);
    // ❌ Forgot free(line) - MEMORY LEAK!
}

// Call this 100 times → 100 memory leaks
for(int i = 0; i < 100; i++) {
    process_file("document.txt");
}
```

**With free():**
```cpp
void process_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    char *line = NULL;
    size_t len = 0;
    
    while(getline(&line, &len, file) != -1) {
        // Process line
    }
    
    free(line);  // ✅ Properly deallocate
    fclose(file);
}

// Call this 100 times → no memory leaks
for(int i = 0; i < 100; i++) {
    process_file("document.txt");
}
```

### Common Patterns

#### Pattern 1: Single Allocation
```cpp
char *buffer = (char*)malloc(256);
if(buffer != NULL) {
    // Use buffer
    strcpy(buffer, "Hello");
}
free(buffer);
```

#### Pattern 2: Reusing Buffer (getline)
```cpp
char *line = NULL;
size_t len = 0;

// Multiple calls reuse/expand the same buffer
getline(&line, &len, file);  // Allocates
getline(&line, &len, file);  // Reuses or expands
getline(&line, &len, file);  // Reuses or expands

free(line);  // Free once at end
```

#### Pattern 3: Array of Pointers
```cpp
char **lines = (char**)malloc(100 * sizeof(char*));

for(int i = 0; i < 100; i++) {
    lines[i] = (char*)malloc(256);  // Allocate each line
}

// Must free each line AND the array
for(int i = 0; i < 100; i++) {
    free(lines[i]);  // Free each line
}
free(lines);  // Free the array
```

### Best Practices

#### 1. Set Pointer to NULL After free()
```cpp
free(buffer);
buffer = NULL;  // Prevents accidental use after free
```

#### 2. Check malloc() Return Value
```cpp
char *buffer = (char*)malloc(1024);
if(buffer == NULL) {
    fprintf(stderr, "Out of memory!\n");
    return -1;
}
// Use buffer
free(buffer);
```

#### 3. Match Every malloc with free
```cpp
// ✅ CORRECT: Balanced malloc/free
void function() {
    char *p = malloc(100);  // Allocate
    // ... use p ...
    free(p);                // Deallocate
}

// ❌ WRONG: Missing free
void function() {
    char *p = malloc(100);  // Allocate
    // ... use p ...
    // Missing free(p) - MEMORY LEAK!
}
```

### C++ Alternative: Smart Pointers

In modern C++, prefer smart pointers over manual `malloc`/`free`:

```cpp
// C++ way (automatic memory management)
#include <memory>
#include <string>

void function() {
    auto buffer = std::make_unique<char[]>(100);
    // Use buffer
    // Automatically freed when function ends - no free() needed!
}
```

### Memory Leak Tools

**Linux:**
```bash
# Valgrind - detects memory leaks
valgrind --leak-check=full ./program

# Example output:
# 100 bytes in 1 blocks are definitely lost
# at malloc() (in /lib/libc.so)
# by getline() (in /lib/libc.so)
# by main() (program.cpp:42)
```

**Windows:**
```cpp
// Visual Studio: CRT Debug Heap
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

int main() {
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    
    // Your code
    
    // Leaks reported at program exit
}
```

### Key Points
- `free()` deallocates memory allocated by `malloc()`, `calloc()`, or `realloc()`
- Always `free()` dynamically allocated memory to prevent memory leaks
- Only `free()` pointers that came from dynamic allocation
- Don't use memory after calling `free()` (undefined behavior)
- Don't call `free()` twice on the same pointer (double free error)
- `free(NULL)` is safe and does nothing
- `getline()` allocates memory that **must** be freed with `free()`
- Set pointers to `NULL` after freeing (best practice)
- In C++, prefer smart pointers (`std::unique_ptr`, `std::shared_ptr`)

---

## Summary

This document covered all essential C file I/O functions used in `document_store.cpp`:

1. **fopen()**: Opens files with various modes (read, write, append)
2. **fclose()**: Closes files and flushes buffers to disk
3. **fgetc()**: Reads one character at a time from files
4. **eof()**: Detects end-of-file condition (after read fails)
5. **ungetc()**: Pushes character back to stream (peek functionality)
6. **size_t**: Unsigned integer type for sizes and counts
7. **getline()**: POSIX function for reading entire lines dynamically
8. **Windows Bug**: getline() hangs on empty files - solution: check first with fgetc()
9. **free()**: Deallocates dynamically allocated memory

### Key Takeaways for Beginners

- Always check return values (`NULL`, `-1`, `EOF`) before using results
- Match every `fopen()` with `fclose()`
- Match every `malloc()`/`getline()` with `free()`
- Initialize pointers to `NULL` before use
- Use `size_t` for sizes and counts (not `int`)
- Check for empty files before `getline()` on Windows
- Understand the difference between stack and heap memory

---

**Document Version**: 1.0  
**Last Updated**: December 23, 2025  
**Author**: High-Performance Search Engine Project  
**Repository**: github.com/adarshpheonix2810/high-performance-search-engine-cpp