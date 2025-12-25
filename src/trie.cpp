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