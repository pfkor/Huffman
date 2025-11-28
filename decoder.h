#ifndef DECODER
#define DECODER

#include "huffman.h"

Node* get_coding_tree(char*);
int decompress(char*, char*, Node*);

#endif