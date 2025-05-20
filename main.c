#include <stdio.h>
#include <string.h>
#include "BabyKyber.h"

// Convert a string to binary bits
void string_to_bits(const char* input, int* output_bits, int* bit_len) {
    int index = 0;
    while (*input) {
        for (int i = 7; i >= 0; i--) {
            output_bits[index++] = (*input >> i) & 1;
        }
        input++;
    }
    *bit_len = index;
}

// Convert binary bits to string
void bits_to_string(const int* bits, int bit_len, char* output) {
    int byte_count = bit_len / 8;
    for (int i = 0; i < byte_count; i++) {
        char c = 0;
        for (int j = 0; j < 8; j++) {
            c = (c << 1) | bits[i * 8 + j];
        }
        output[i] = c;
    }
    output[byte_count] = '\0';
}
// Assuming public key is stored in an array or matrix as `public_key[2][4]`
void PrintPublicKey(int public_key[][4], int length) {
    printf("Public Key:\n");
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d ", public_key[i][j]);
        }
        printf("\n");
    }
}

int main() {
    // Sample message
    const char* message = "Hi";

    // Step 1: Convert to bits and pad
    int message_bits[256];
    int bit_len = 0;
    string_to_bits(message, message_bits, &bit_len);

    while (bit_len % 4 != 0) message_bits[bit_len++] = 0;
    int num_blocks = bit_len / 4;

    // Kyber parameters
    int A[4][4] = { {11, 16, 16, 6}, {3, 6, 4, 9}, {1, 10, 3, 5}, {15, 9, 1, 6} };
    int S[2][4] = { {0, 1, -1, -1}, {0, -1, 0, -1} };
    int e[2][4] = { {0, 0, 1, 0}, {0, -1, 1, 0} };
    int t[2][4] = {0};

    GenerateT(A, S, e, t);

    // Output buffers
    int u_blocks[64][2][4] = {{{0}}};
    int v_blocks[64][4] = {{0}};

    // Encrypt
    for (int i = 0; i < num_blocks; i++) {
        int data[4];
        for (int j = 0; j < 4; j++) {
            data[j] = message_bits[i * 4 + j];
        }

        int r[2][4] = { {0, 0, 1, -1}, {-1, 0, 1, 1} };
        int e1[2][4] = { {0, 1, 1, 0}, {0, 0, 1, 0} };
        int e2[4] = {0, 0, -1, -1};

        Encrypt(A, t, r, e1, e2, 4, u_blocks[i], v_blocks[i], data);
    }

    // Decrypt
    int recovered_bits[256];
    int bit_index = 0;
    for (int i = 0; i < num_blocks; i++) {
        int out[4];
        Decrypt(S, u_blocks[i], v_blocks[i], out);
        for (int j = 0; j < 4; j++) {
            recovered_bits[bit_index++] = out[j];
        }
    }

    // Convert bits back to string
    char decrypted_message[64];
    bits_to_string(recovered_bits, bit_len, decrypted_message);

    // Print results
    printf("Original message: %s\n", message);
    printf("Decrypted message: %s\n", decrypted_message);

    return 0;
}
