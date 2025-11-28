#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
    #define strcmp stricmp
    #include <io.h>
    #define access _access 
    #define F_OK 0
    #define R_OK 4
    #define W_OK 2
#else 
    #include <unistd.h>
#endif

#include "huffman.h"
#include "coder.h"
#include "decoder.h"

#define INPUT_FILE argv[2]
#define OUTPUT_FILE argv[3]

#include <stdio.h>
#include "huffman.h"

int main(int argn, char *argv[]){
    if (argn != 4){
        fprintf(stderr, "Usage: %s [encode|decode] input output.\n", argv[0]);
        return 1;
    }

    if (access(INPUT_FILE, F_OK | R_OK) != 0){
        fprintf(stderr, "Error: Cannot access input file '%s'\n", INPUT_FILE);
        return 1;
    }

    if (access(OUTPUT_FILE, F_OK) == 0 && access(OUTPUT_FILE, W_OK) != 0){
        fprintf(stderr, "Error: Cannot access output file '%s'\n", OUTPUT_FILE);
        return 1;
    }

    if (strcmp(argv[1], "encode") == 0){
        // encoding regime
        printf("Compressing '%s' to '%s'...\n", INPUT_FILE, OUTPUT_FILE);

        unsigned int *freqs = get_frequencies(INPUT_FILE);
        if (!freqs){
            fprintf(stderr, "Error: Failed to calculate frequencies.\n");
            return 1;
        }

        MinHeap *heap = make_min_heap(freqs);
        free(freqs);
        if (!heap){
            fprintf(stderr, "Error: Failed to create min-heap.\n");
            return 1;
        }

        Node *tree = make_huffman_tree(heap);
        destroy_min_heap(heap);
        if (!tree){
            fprintf(stderr, "Error: Failed to build Huffman tree.\n");
            return 1;
        }
        
        HuffCode *codes = get_huffman_codes(tree);
        destroy_tree(tree);
        if (!codes){
            fprintf(stderr, "Error: Failed to calculate codes.\n");
            return 1;
        }

        if (!compress(INPUT_FILE, OUTPUT_FILE, codes)){
            fprintf(stderr, "Error: Compression failed.\n");
            free(codes);
            return 1;
        }
        free(codes);
        printf("Compressing completed successfully.\n");

    } else if (strcmp(argv[1], "decode") == 0){
        // decoding regime
        printf("Decompressing '%s' to '%s'...\n", INPUT_FILE, OUTPUT_FILE);

        Node *root = get_coding_tree(INPUT_FILE);
        if (!root){
            fprintf(stderr, "Error: Failed to build Huffman tree.\n");
            return 1;
        }

        if (!decompress(INPUT_FILE, OUTPUT_FILE, root)){
            fprintf(stderr, "Error: Decompression failed.\n");
            destroy_tree(root);
            return 1;
        }
        destroy_tree(root);
        printf("Decompressing completed successfully.\n");

    } else {
        fprintf(stderr, "Unknown mode: %s.\n", argv[1]);
        return 1;
    }
    return 0;
}