import argparse
import psutil
import time
import os


def read_chunks(words_file: str, words_file_encoding="utf-8", batch_size: int = 1000000):
    lines = []

    with open(words_file, "r", encoding=words_file_encoding) as f:
        for line in f:
            lines.append(line.strip())

            if len(lines) == batch_size:
                yield lines
                lines.clear()
    
    if len(lines) != 0:
        yield lines


def trie_insert_benchmark(words_file: str, implementation: str, batch_size: int = 1000000, words_file_encoding="utf-8", use_batch_insert=False):
    if implementation == "cpp":
        import trie
        tree = trie.Trie()
    else:
        from baseline.trie import Trie
        tree = Trie()

    duration = 0
    no_words = 0
    read_words = 0

    with open(words_file, "r", encoding=words_file_encoding) as f:
        for line in f:
            no_words += 1
    
    for batch in read_chunks(words_file, words_file_encoding=words_file_encoding, batch_size=batch_size):
        if use_batch_insert:
            t1 = time.time()
            tree.batch_insert(batch)
        else:
            t1 = time.time()
            for line in batch:
                tree.insert(line)

        t2 = time.time()
        
        batch_duration = (t2 - t1) * 1000
        duration += batch_duration
        read_words += len(batch)

        print(f"\33[2K\rProgress: {int(100*read_words/no_words)}%, {len(batch) / batch_duration} words/ms", end="")
    
    print("")
    print(f"Inserted {read_words} words in {duration}ms")
    print(f"Average inserts per millisecond (across batches): {read_words / duration}")

    return tree


def trie_search_benchmark(tree, words_file: str, words_file_encoding="utf-8"):
    duration = 0
    no_words = 0
    searched_words = 0

    with open(words_file, "r", encoding=words_file_encoding) as f:
        for line in f:
            no_words += 1
    
    for batch in read_chunks(words_file, words_file_encoding=words_file_encoding, batch_size=1000000):
        t1 = time.time()
        for line in batch:
            res = tree.search(line, False)
        t2 = time.time()
        
        batch_duration = (t2 - t1) * 1000
        duration += batch_duration
        searched_words += len(batch)

        print(f"\33[2K\rProgress: {int(100*searched_words/no_words)}%, {len(batch) / batch_duration} words/ms", end="")
    
    print("")
    print(f"Searched for {searched_words} words in {duration}ms")
    print(f"Average searches per millisecond (across batches): {searched_words / duration}")
                

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog="Trie Benchmark")
    parser.add_argument("--words", dest="words", help="Path to word list (single word per line)", required=True)
    parser.add_argument(
        "--implementation", dest="impl", choices=["cpp", "python"], 
        help="Implementation to use - either CPP via PyBind, or pure Python", default="python"
    )
    parser.add_argument("--words-encoding", dest="enc", help="Encoding to use when reading words file", default="utf-8")
    parser.add_argument("--use-batch-insert", dest="use_batch_insert", action="store_true", default=False)
    
    args = parser.parse_args()

    process = psutil.Process()
    start_mem = process.memory_info().rss
    tree = trie_insert_benchmark(args.words, args.impl, words_file_encoding=args.enc, use_batch_insert=args.use_batch_insert)
    cur_mem = process.memory_info().rss
    print(f"Memory (RSS) used for Trie tree: {(cur_mem - start_mem) / 1024 / 1024}mb")
    trie_search_benchmark(tree, args.words, words_file_encoding=args.enc)
