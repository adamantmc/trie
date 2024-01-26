#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <chrono>
#include "trie.h"

std::vector<std::string> read_batch(std::ifstream &file, int batch_size) {
    std::vector<std::string> lines;

    for(int i = 0; i < batch_size; i++) {
        std::string line;
        
        if(!std::getline(file, line)) break;
        
        lines.push_back(line);
    }

    return lines;
}


Trie trie_insert_benchmark(std::string words_file, int batch_size = 1000000) {
    Trie trie;

    std::ifstream file(words_file, std::ifstream::in);
    int no_words = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
    int read_words = 0;

    file.clear();
    file.seekg(0);

    double total_duration = 0;

    while(true) {
        std::vector<std::string> batch = read_batch(file, batch_size);

        if(batch.empty()) break;

        auto start = std::chrono::high_resolution_clock::now();
        for(long unsigned int i = 0; i < batch.size(); i++) {
            trie.insert(batch.at(i));
        }
        auto stop = std::chrono::high_resolution_clock::now();

        double duration = (double) std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        total_duration += duration;
        read_words += batch.size();
        
        std::cout << "\33[2K\r" << "Progress: " << 100*read_words / no_words << "%, " << batch.size() / duration << " words/ms";
        std::fflush(stdout);
    }
    
    file.close();

    std::cout << std::endl;
    std::cout << "Inserted " << read_words << " words in " << total_duration << "ms" << std::endl;
    std::cout << "Average inserts per millisecond (across batches): " << read_words / total_duration << std::endl;

    return trie;
}


void trie_search_benchmark(Trie trie, std::string words_file) {
    std::srand(std::time(nullptr));
    std::ifstream file(words_file, std::ifstream::in);
    int no_words = std::count(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), '\n');
    
    file.clear();
    file.seekg(0);

    double total_duration = 0;
    int searched_words = 0;

    while(true) {
        std::vector<std::string> batch = read_batch(file, 1000000);
        if(batch.empty()) break;

        auto start = std::chrono::high_resolution_clock::now();
        for(long unsigned int i = 0; i < batch.size(); i++) {
            std::vector<std::string> res = trie.search(batch.at(i));
        }
        auto stop = std::chrono::high_resolution_clock::now();

        double duration = (double) std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        total_duration += duration;
        searched_words += batch.size();
        
        std::cout << "\33[2K\r" << "Progress: " << 100*searched_words / no_words << "%, " << batch.size() / duration << " words/ms";
        std::fflush(stdout);
    }
    
    file.close();

    std::cout << std::endl;
    std::cout << "Searched for " << no_words << " words in " << total_duration << "ms" << std::endl;
    std::cout << "Average searches per millisecond (across batches): " << no_words / total_duration << std::endl;
}


double current_rss() {
    int tSize = 0, resident = 0, share = 0;
    std::ifstream buffer("/proc/self/statm");
    buffer >> tSize >> resident >> share;
    buffer.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    double rss = resident * page_size_kb;
    return rss / 1024;
}


int main(int argc, char *argv[]) {
    if(argc <= 1) {
        std::cout << "Usage: " << argv[0] << " PATH_TO_WORDLIST" << std::endl;
        return -1;
    }
    
    double start_rss = current_rss();
    Trie trie = trie_insert_benchmark(std::string(argv[1]));
    std::cout << "Memory (RSS) used for Trie tree: " << current_rss() - start_rss << "mb" << std::endl;     
    trie_search_benchmark(trie, std::string(argv[1]));
    return 0;
}
