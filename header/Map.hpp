#include <iostream>
#include <cstdlib>
#include <cstring>
#ifndef MAP_HPP
#define MAP_HPP
using namespace std;
class Mymap
{
    int size;         /// the number of documents
    int buffersize;   // the length of the biggest document
    char **documents; // each document
    int *doc_lengths; // lengths of each document
public:
    // Constructor
    Mymap(int size, int buffersize);
    ~Mymap();
    int insert(char* line,int i);
    const int get_size() const { return size;  }
    const int get_buffersize() const { return buffersize; }
};
#endif