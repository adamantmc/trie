#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include "trie.h"

namespace py = pybind11;


void Trie::batch_insert(std::vector<std::string> strings) {
    for(std::string s : strings) this->insert(s);
}


PYBIND11_MODULE(trie, m) {
    py::class_<Trie>(m, "Trie")
        .def(py::init<>())
        .def("insert", &Trie::insert)
        .def("exists", &Trie::exists)
        .def("search", &Trie::search)
        .def("batch_insert", &Trie::batch_insert);
}
