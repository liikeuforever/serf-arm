# Serf - Streaming Error-bounded Floating-Point Compression

```
   _____           ____
  / ___/___  _____/ __/
  \__ \/ _ \/ ___/ /_  
 ___/ /  __/ /  / __/  
/____/\___/_/  /_/
```

Serf is a fast and robust compression algorithm with high compression performance, which is designed specifically 
for streaming application and floating-point data.

## Guides on building Serf

CMake is the building system of Serf. You can build Serf using the following command:

`cmake -DCMAKE_BUILD_TYPE=release -B build && cmake --build build`

_Notes: By default, baselines will be compiled altogether with Serf. If you do not need this, you can comment 
`add_subdirectory()` for baselines in `CMakeLists.txt`._

## Baselines

- Buff
- Chimp128
- Deflate
- Elf
- FPC
- Gorilla
- LZ4
- LZ77
- Machete
- Snappy
- SZ2.0
- Zstd

## Python interface

We port Serf to many language so people can use Serf in their projects.

For Python, You can install pybind11 using pip and copy Serf shared library together with its Python module wrapper, 
then import `pyserf`. Easy, isn't it?

## Code Format Check
Serf keeps Google C++ Style as Code of Conduct, which is accepted widely in industry. You should use `cpplint` to 
check your code before contributing your code to this project.
