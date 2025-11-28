#ifndef HUFFMAN
#define HUFFMAN

typedef struct Node{
    unsigned char symbol;
    unsigned int freq; 
    struct Node *left, *right;
} Node; 

typedef struct {
    unsigned int code;
    unsigned int length;
} HuffCode;

Node* create_node(unsigned char, unsigned int);
void destroy_tree(Node*);

Node* merge_nodes(Node*, Node*);

typedef struct {
    int size;
    int capacity;
    Node **array;
} MinHeap;

MinHeap* create_min_heap(int);
void destroy_min_heap(MinHeap*);

void min_heap_insert (MinHeap*, Node*);
Node* min_heap_extract_min(MinHeap*);

#endif