#ifndef TRIE_H
#define TRIE_H

#include <map>
#include <string>
#include <vector>


class Trie {
    public:
        Trie();
        Trie(char c);
        void insert(std::string);
        void batch_insert(std::vector<std::string>);
        bool exists(std::string);
        std::vector<std::string> search(std::string, bool match_exact=false);
    private:
        char character;
        std::string word;
        std::map<char, Trie *> children;
        bool has_child(char c) { return this->children.find(c) != this->children.end(); };
        std::vector<std::string> bfs(Trie *node = NULL);

};

#endif
