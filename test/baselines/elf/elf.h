#pragma once

#include <cstdint>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t elf_encode(double *in, ssize_t len, uint8_t **out, double error);
ssize_t elf_decode(uint8_t *in, ssize_t len, double *out, double error);

#ifdef __cplusplus
}
#endif 
