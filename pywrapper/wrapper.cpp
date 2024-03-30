#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "serf/concurrency/TensorXORCompressTransaction.h"
#include "serf/concurrency/TensorXORDecompressTransaction.h"

namespace py = pybind11;

PYBIND11_MODULE(pyserf, m) {
    py::class_<TensorXORCompressTransaction>(m, "TensorXORCompressTransaction")
            .def(py::init<int, int, int, int>())
            .def("compress_tensor", &TensorXORCompressTransaction::compress_tensor);

    py::class_<TensorXORDecompressTransaction>(m, "TensorXORDecompressTransaction")
            .def(py::init<int, int, int, int>())
            .def("decompress_tensor", &TensorXORDecompressTransaction::decompress_tensor);
}