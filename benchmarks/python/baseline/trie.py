class Trie:
    __slots__ = "character", "children", "word"

    def __init__(self):
        self.children = dict()
        self.word = None
        self.character = None

    def insert(self, s: str):
        node = self

        for char in s:
            if char in node.children:
                node = node.children[char]
            else:
                new_node = Trie()
                new_node.character = char
                node.children[char] = new_node
                node = new_node
        
        node.word = s

    def bfs(self, node):
        if node is None:
            node = self

        results = []

        if node.word is not None:
            results.append(node.word)
        
        for child_char, child_node in node.children.items():
            results.extend(self.bfs(child_node))
        
        return results

    def search(self, query: str, match_whole_query: bool = False) -> list[str]:
        node = self
        matching_chars = 0

        for char in query:
            if char in node.children:
                node = node.children[char]
                matching_chars += 1
            else:
                break
        
        if matching_chars == 0 or (match_whole_query and len(query) != matching_chars):
            return []

        return self.bfs(node)

    def exists(self, s: str):
        pass


import time


def read_chunks(words_file: str, batch_size: int = 1000000):
    lines = []

    with open(words_file, "r", encoding="latin-1") as f:
        for line in f:
            lines.append(line.strip())

            if len(lines) == batch_size:
                yield lines
                lines.clear()
    
    if len(lines) != 0:
        yield lines


def trie_insert_benchmark(words_file: str, batch_size: int = 1000000):
    tree = Trie()
    
    duration = 0
    no_words = 0
    read_words = 0

    with open(words_file, "r", encoding="latin-1") as f:
        for line in f:
            no_words += 1
    
    for batch in read_chunks(words_file, batch_size=batch_size):
        t1 = time.time()
        for line in batch:
            tree.insert(line)
        t2 = time.time()
        
        duration += (t2 - t1) * 1000
        read_words += len(batch)

        print(f"\33[2K\rProgress: {int(100*read_words/no_words)}%, {read_words / duration} words/ms")
    
    print(f"Inserted {no_words} in {duration}ms")
    print(f"Average words per millisecond (across batches): {no_words / duration}")


if __name__ == "__main__":
    trie_insert_benchmark("../python_bindings/rockyou.txt")
