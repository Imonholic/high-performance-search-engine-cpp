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