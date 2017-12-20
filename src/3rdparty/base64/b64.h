#pragma once

#include <cstddef>

void b64_encodeblock(const unsigned char* in, unsigned char* out, size_t len);
void b64_decodeblock(const unsigned char* in, unsigned char* out);
