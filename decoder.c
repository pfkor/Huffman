#include <stdlib.h>
#include <stdio.h>

#include "huffman.h"
#include "decoder.h"

Node* get_coding_tree(char *filepath){
    FILE *input = fopen(filepath, "rb");
    if (!input) return NULL;
    
    int useful_symbols = fgetc(input);
    int bit_skip = fgetc(input);
    if (useful_symbols == EOF || bit_skip == EOF) {
        fclose(input);
        return NULL;
    }
    
    Node *root = create_node(0, 0);
    if (!root){
        fclose(input);
        return NULL;
    }

    // Metadata
    for (int i = 0; i < useful_symbols; i++){
        int symbol = fgetc(input);
        int code_length = fgetc(input);
        
        if (symbol == EOF || code_length == EOF) {
            destroy_tree(root);
            fclose(input);
            return NULL;
        }
        
        unsigned int code = 0;
        int bits_read = 0;
        
        while (bits_read < code_length) {
            int byte = fgetc(input);
            if (byte == EOF) {
                destroy_tree(root);
                fclose(input);
                return NULL;
            }
            
            for (int bit_pos = 7; bit_pos >= 0 && bits_read < code_length; bit_pos--) {
                int bit = (byte >> bit_pos) & 1;
                code = (code << 1) | bit;
                bits_read++;
            }
        }
        
        Node *current = root;
        for (int j = code_length - 1; j >= 0; j--) {
            int bit = (code >> j) & 1;
            
            if (bit == 0) {
                if (!current->left) {
                    current->left = create_node(0, 0);
                    if (!current->left) {
                        destroy_tree(root);
                        fclose(input);
                        return NULL;
                    }
                }
                current = current->left;
            } else {
                if (!current->right) {
                    current->right = create_node(0, 0);
                    if (!current->right) {
                        destroy_tree(root);
                        fclose(input);
                        return NULL;
                    }
                }
                current = current->right;
            }
        }
        current->symbol = (unsigned char)symbol;
    }
    
    fclose(input);
    return root;
}

int decompress(char *source, char *target, Node *root){
    if (!root) return 0;

    FILE *input = fopen(source, "rb");
    if (!input) return 0;
    
    FILE *output = fopen(target, "wb");
    if (!output){
        fclose(input);
        return 0;
    }

    long text_start = ftell(input);
    fseek(input, 0, SEEK_END);
    long file_size_in_bytes = ftell(input);
    if (file_size_in_bytes == -1L){
        fprintf(stderr, "Something went wrong! Failed to get input file size\n");
        fclose(input);
        fclose(output);
        return 0;
    }
    fseek(input, text_start, SEEK_SET);
    long file_size_in_bits = file_size_in_bytes * 8;
    

    int useful_symbols = fgetc(input);
    int bit_skip = fgetc(input);
    
    for (int i = 0; i < useful_symbols; i++){
        fgetc(input); // symbol
        int code_length = fgetc(input);
        
        int code_skip = (code_length + 7) / 8;
        for (int j = 0; j < code_skip; j++) fgetc(input);
    }
    
    if (!root->left && !root->right){
        printf("%d\n", root->symbol);
        for (long i = 0; i < file_size_in_bits - bit_skip; i++){
            fputc(root->symbol, output);
        }

    } else {
        Node *current = root;
        int buffer = 0;
        int bit_count = 0;
        int bits_processed = 0;
        
        for (int i = 0; i < bit_skip; i++) {
            if (bit_count == 0) {
                buffer = fgetc(input);
                if (buffer == EOF) break;
                bit_count = 8;
            }
            buffer <<= 1;
            bit_count--;
            bits_processed++;
        }
        
        while (bits_processed < file_size_in_bits) {
            if (bit_count == 0) {
                buffer = fgetc(input);
                if (buffer == EOF) break;
                bit_count = 8;
            }
            
            int bit = (buffer >> 7) & 1;
            buffer <<= 1;
            bit_count--;
            bits_processed++;
            
            if (bit == 0) {
                current = current->left;
            } else {
                current = current->right;
            }
            
            if (!current) {
                fprintf(stderr, "Something went wrong! Invalid code\n");
                fclose(input);
                fclose(output);
                return 0;
            }
            
            if (!current->left && !current->right) {
                fputc(current->symbol, output);
                current = root;
            }
        }
    }
    
    fclose(input);
    fclose(output);
    return 1;
}