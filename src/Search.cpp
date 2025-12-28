#include "Search.hpp"
using namespace std;

void search(char* token, TrieNode *trie, Mymap *map, int k){
    cout<<"not implemented yet"<<endl;
}

void df(TrieNode* trie){
    cout<<"not implemented yet"<<endl;
}

int tf(char* token, TrieNode* trie){
    // Get document ID
    char* token2 = strtok(NULL, " \t\n");
    if(token2 == NULL){
        cout<<"Error: Missing document ID. Usage: /tf <doc_id> <word>"<<endl;
        return -1;
    }
    
    // Validate document ID is a number
    for(int i=0; i<strlen(token2); i++){
        if(!isdigit(token2[i])){
            cout<<"Error: Document ID must be a number"<<endl;
            return -1;
        }
    }
    int id = atoi(token2);
    
    // Get the word to search
    token2 = strtok(NULL, " \t\n");
    if(token2 == NULL){
        cout<<"Error: Missing word. Usage: /tf <doc_id> <word>"<<endl;
        return -1;
    }
    
    cout<<"not implemented yet"<<endl;
    return 0;
}