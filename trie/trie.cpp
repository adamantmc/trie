#include "trie.h"
#include <iostream>


Trie::Trie() {
    this->character = '\0';
}


Trie::Trie(char c) {
    this->character = c;
}


void Trie::insert(std::string s) {
    Trie *node = this;

    int noChars = s.length();

    for(int i = 0; i < noChars; i++) {
        char c = s[i];

        if(node->has_child(c)) {
            node = node->children[c];
        }
        else {
            Trie *newNode = new Trie(c);
            node->children[c] = newNode;
            node = newNode;
        }
    }

    node -> word = s;
}


bool Trie::exists(std::string s) {
    Trie *node = this;
    
    int noChars = s.length();

    for(int i = 0; i < noChars; i++) {
        char c = s[i];

        if(node->has_child(c)) node = node->children[c];
        else return false;
    }

    return !node->word.empty();
}


std::vector<std::string> Trie::bfs(Trie *node) {
    std::vector<std::string> results;

    if(node == NULL) node = this;

    if(!node->word.empty()) results.push_back(node->word);

    for(std::map<char, Trie *>::iterator it = node->children.begin(); it != node->children.end(); it++) {
        std::vector<std::string> child_results = node->bfs(it->second);
        
        results.insert(results.end(), child_results.begin(), child_results.end());
    }

    return results;
}


std::vector<std::string> Trie::search(std::string query, bool match_exact) {
    Trie *node = this;
    
    int noChars = query.length();
    int lastMatch = 0;

    for(int i = 0; i < noChars; i++) {
        char c = query[i];
        if(node->has_child(c)) {
            node = node->children[c];
            lastMatch += 1;
        }
        else {
            break;
        }
    }

    if(lastMatch == 0 || (lastMatch != noChars && match_exact)) return std::vector<std::string>();

    // found deepest match to input string - bfs to get results
    return this->bfs(node);
}
