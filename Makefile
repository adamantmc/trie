.PHONY: clean trie_lib_so all

CPPC = g++
CPP_CFLAGS = -O3 -Wall -Werror -Wextra -std=c++11
python = python3.11
pybind_includes = $(shell ${python} -m pybind11 --includes)
ext = $(shell ${python}-config --extension-suffix)

clean:
	@rm -rf build

build/libtrie.a: trie/trie.cpp 
	@mkdir -p build
	@echo "Building trie static library"
	$(CPPC) $(CPP_CFLAGS) -c trie/trie.cpp -o build/trie.o
	@ar -rc $@ build/trie.o
	@echo ""

build/include/trie.h: trie/trie.h
	@mkdir -p build/include
	@cp trie/trie.h $@

trie_lib_so: build/libtrie.a build/include/trie.h
	@echo "Building bindings shared library"
	$(CPPC) $(CPP_CFLAGS) -shared bindings/trie.cpp build/libtrie.a -I build/include/ $(pybind_includes) -fPIC -o build/trie$(ext)
	@echo ""

build/bin/benchmark: build/libtrie.a build/include/trie.h
	@echo "Building benchmark executable"
	@mkdir -p build/bin/
	$(CPPC) $(CPP_CFLAGS) benchmarks/cpp/benchmark.cpp build/libtrie.a -I build/include/ -o $@
	@echo ""

rockyou.txt:
	@wget https://github.com/kkrypt0nn/wordlists/raw/main/wordlists/famous/rockyou.zip -O rockyou.zip
	@unzip -o rockyou.zip
	@rm rockyou.zip

cpp_benchmark: rockyou.txt build/bin/benchmark
	@echo "====== CPP benchmark ======"
	@build/bin/benchmark rockyou.txt
	@echo ""
	
python_bindings_benchmark:
	@echo "====== Python benchmark - bindings ======"
	@export PYTHONPATH=build/:${PYTHONPATH} && $(python) benchmarks/python/benchmark.py --words rockyou.txt --implementation cpp --words-encoding latin-1
	@echo ""

python_bindings_batch_insert_benchmark:
	@echo "====== Python benchmark - bindings, batch insert ======"
	@export PYTHONPATH=build/:${PYTHONPATH} && $(python) benchmarks/python/benchmark.py --words rockyou.txt --implementation cpp --words-encoding latin-1 --use-batch-insert
	@echo ""

pure_python_benchmark:
	@echo "====== Python benchmark - Python baseline ======"
	@export PYTHONPATH=build/:${PYTHONPATH} && $(python) benchmarks/python/benchmark.py --words rockyou.txt --implementation python --words-encoding latin-1
	@echo ""

run_benchmarks: cpp_benchmark python_bindings_benchmark pure_python_benchmark

build: trie_lib_so

all: trie_lib_so build/bin/benchmark run_benchmarks 