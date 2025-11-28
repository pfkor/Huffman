#include <stdlib.h>
#include <stdio.h>

#include "huffman.h"
#include "coder.h"

unsigned int* get_frequencies(char *filepath){
    FILE *input = fopen(filepath, "rb");
    if (!input) return NULL;
    
    unsigned int *freqs = malloc(sizeof(unsigned int) * 256);
    if (!freqs){
        fclose(input);
        return NULL;
    }

    for (int i = 0; i < 256; i++){
        freqs[i] = 0;
    }

    unsigned char symbol;
    int c;
    while ((c = fgetc(input)) != EOF){
        symbol = (unsigned char) c;
        freqs[symbol]++;
    }
    
    fclose(input);
    return freqs;
}

MinHeap* make_min_heap(unsigned int *freqs){
    if (!freqs) return NULL;
    
    MinHeap *heap = create_min_heap(256);
    if (!heap) return NULL;
    
    for (int i = 0; i < 256; i++){
        if (freqs[i] > 0){
            if (heap->size >= heap->capacity){
                fprintf(stderr, "Error: Heap capacity exceeded.\n");
                destroy_min_heap(heap);
                return NULL;
            }
            
            Node *node = create_node((unsigned char) i, freqs[i]);
            if (node) min_heap_insert(heap, node);
        }
    }
    return heap;
}

Node* make_huffman_tree(MinHeap *heap){
    if (!heap || heap->size == 0) return NULL;
    while(heap->size > 1){
        Node *first = min_heap_extract_min(heap);
        Node *second = min_heap_extract_min(heap);

        Node *merged = merge_nodes(first, second);
        min_heap_insert(heap, merged);
    }
    return heap->array[0];
}

void generate_codes_rec(Node *node, HuffCode *codes, unsigned int code, unsigned int length){
    if (!node) return;
 
    if (!node->left && !node->right){
        codes[node->symbol].code = code;
        codes[node->symbol].length = length;
        return;
    }

    generate_codes_rec(node->left, codes, code << 1, length + 1);
    generate_codes_rec(node->right, codes, (code << 1) | 1, length + 1);
}

HuffCode* get_huffman_codes(Node *root){
    if (!root) return NULL;

    // post-order traverse
    HuffCode *codes = (HuffCode *)malloc(sizeof(HuffCode) * 256);
    if (!codes) return NULL;

    for (int i = 0; i < 256; i++){
        codes[i].code = 0;
        codes[i].length = 0;
    }
    if (!root->left && !root->right) generate_codes_rec(root, codes, 0, 1);
    else generate_codes_rec(root, codes, 0, 0);
    return codes;
}

int compress(char *source, char *target, HuffCode *codes){
    FILE *input = fopen(source, "rb");
    if (!input) return 0;

    FILE *output = fopen(target, "wb");
    if (!output){
        fclose(input);
        return 0;
    }

    int useful_symbols = 0;
    for (int i = 0; i < 256; i++){
        useful_symbols += (codes[i].length > 0);
    }
    fputc(useful_symbols, output); 

    int c;
    int bit_skip = 0;
    while ((c = fgetc(input)) != EOF) bit_skip = (bit_skip + codes[c].length) % 8;
    bit_skip = (8 - bit_skip) % 8;
    fputc(bit_skip, output); // skip bit_count bites at the begining
    rewind(input);
    
    // Metadata
    for (int c = 0; c < 256; c++){
        int code = codes[c].code;
        int len = codes[c].length;

        if (len == 0) continue;
        
        fputc(c, output);
        fputc(len, output);

        int temp_bit_count = 0;
        unsigned int temp_buffer = 0;

        for (int i = len-1; i >= 0; i--){
            int bit = (code >> i) & 1;
            temp_buffer = (temp_buffer << 1) | bit;
            temp_bit_count++;

            if (temp_bit_count == 8){
                fputc(temp_buffer, output);
                temp_buffer = 0;
                temp_bit_count = 0;
            }
        }
        if (temp_bit_count > 0){
            temp_buffer <<= (8 - temp_bit_count);
            fputc(temp_buffer, output);
        }
    }

    int buffer = 0;
    int bit_count = bit_skip;
    // Text
    while ((c = fgetc(input)) != EOF){
        int code = codes[(unsigned char)c].code;
        int len = codes[(unsigned char)c].length;

        for (int i = len-1; i >= 0; i--){
            int bit = (code >> i) & 1;
            buffer = (buffer << 1) | bit;
            bit_count++;

            if (bit_count == 8){
                fputc(buffer, output);
                buffer = 0;
                bit_count = 0;
            }
        }
    }

    fclose(input);
    fclose(output);
    return 1;
}

