#include "./header/searchengine.hpp"
using namespace std;
//argument count(argc) and argument vector(argv)
int main (int argc, char** argv) {//argv and argc are how command line arguments are passed to main() in C and C++.
    if(argc !=5){
        cout<<"Wrong number of arguments"<<endl;
    }
    else{
        cout<<"Starting search engine with parameters:"<<endl;
    }
    return 0;
}