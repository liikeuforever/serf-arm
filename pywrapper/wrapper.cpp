#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "serf/concurrency/tensor_xor_compress_manager.h"
#include "serf/concurrency/tensor_xor_decompress_manager.h"

namespace py = pybind11;

PYBIND11_MODULE(pyserf, m) {
    py::class_<TensorXORCompressManager>(m, "TensorXORCompressTransaction")
            .def(py::init<int, int, int, int>())
            .def("compress_tensor", &TensorXORCompressManager::compress_tensor);

    py::class_<TensorXORDecompressManager>(m, "TensorXORDecompressTransaction")
            .def(py::init<int, int, int, int>())
            .def("decompress_tensor", &TensorXORDecompressManager::decompress_tensor);
}