# 🐍️Python Interface of Serf

## Dependency

- Pybind11 - can be installed by `pip install pybind11[global]`

## How to build

1. In the root directory of this project, run `cmake -B build` to construct the CMake building system
2. Change the working directory as build, run `cd build`
3. Build python shared library of Serf, run `make pyserf`

## How to use

Just put the shared library together with your scripts in the same directory and import it by `import pyserf`

### Example

```python
from pyserf import PySerfXORCompressor, PySerfXORDecompressor

WIN_SIZE = 1000
ERROR_BOUND = 0.001
ADJUST = 0

# Compression
compressor = PySerfXORCompressor(WIN_SIZE, ERROR_BOUND, ADJUST)
compressor.add_value(3.1415)
compressor.add_value(3.1788)
# ...
compressor.close()
pack = compressor.get()

# Decompression
decompressor = PySerfXORDecompressor(ADJUST)
values = decompressor.decompress(pack)
# values = [3.1415, ...]
```

## PyTorch Example

Just run `python -u pywrapper/example/main.py` 