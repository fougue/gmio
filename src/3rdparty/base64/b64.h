#ifndef BASE64_B64_H
#define BASE64_B64_H

#include <stddef.h>

void b64_encodeblock(const unsigned char* in, unsigned char* out, size_t len);
void b64_decodeblock(const unsigned char* in, unsigned char* out);

#endif /* BASE64_B64_H */
