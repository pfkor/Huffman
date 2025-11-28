#ifndef CODER
#define CODER

#include "huffman.h"

unsigned int* get_frequencies(char*);

MinHeap* make_min_heap(unsigned int*);
Node* make_huffman_tree(MinHeap*);
HuffCode* get_huffman_codes(Node*);

int compress(char*, char*, HuffCode*);

#endif