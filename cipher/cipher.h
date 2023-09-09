#ifndef CIPHER_H
#define CIPHER_H

void encode(char const * plaintext, char* encryptedtext, int32_t *num_bits_used);
void decode(char const* ciphertext, int32_t num_chars, char* plaintext);

// helper function for debugging
void print_bits(char* buffer, int32_t start_pos, int32_t how_many);

#endif
