#include <stdlib.h>
#include <stdio.h>

#include "huffman.h"

Node* create_node(unsigned char symbol, unsigned int freq){
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) return NULL;

    node->symbol = symbol;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;

    return node;
}

void destroy_tree(Node *root){
    if (!root) return;
    destroy_tree(root->left);
    destroy_tree(root->right);
    free(root);
    return;
}

Node* merge_nodes(Node *left, Node *right){
    if (!left || !right) return NULL;
    Node *merged = create_node(0, left->freq + right->freq);
    if (!merged) return NULL;
    merged->left = left;
    merged->right = right;
    return merged;
}

MinHeap* create_min_heap(int capacity){
    if (capacity == 0) return NULL;

    MinHeap *heap = (MinHeap *)malloc(sizeof(MinHeap));
    if (!heap) return NULL;

    heap->size = 0;
    heap->capacity = capacity;
    heap->array = malloc(sizeof(Node*) * capacity);
    if (!heap->array){
        free(heap);
        return NULL;
    }
    return heap;
}

void destroy_min_heap(MinHeap *heap){
    if (!heap) return;

    free(heap->array);
    free(heap);
    return;
}

void min_heap_insert(MinHeap *heap, Node *node){
    if (heap->size >= heap->capacity) return;

    int pos = heap->size;
    heap->array[pos] = node;
    heap->size++;

    while (pos > 0){
        int parent_pos = (pos - 1) / 2;
        if (heap->array[parent_pos]->freq <= heap->array[pos]->freq) break;
    
        Node *temp = heap->array[parent_pos];
        heap->array[parent_pos] = heap->array[pos];
        heap->array[pos] = temp;

        pos = parent_pos;
    }   
}

Node* min_heap_extract_min(MinHeap *heap){
    if (!heap || heap->size == 0) return NULL;
    
    Node *min = heap->array[0];
    heap->array[0] = heap->array[heap->size-1];
    heap->size--;

    int pos = 0;

    while (2 * pos + 1 < heap->size){
        int left_pos = 2 * pos + 1;
        int right_pos = 2 * pos + 2;
        int smallest = pos;

        if (heap->array[left_pos]->freq < heap->array[smallest]->freq) smallest = left_pos;
        if (right_pos < heap->size && heap->array[right_pos]->freq < heap->array[smallest]->freq) smallest = right_pos;
        
        if (smallest == pos) break;

        Node *temp = heap->array[pos];
        heap->array[pos] = heap->array[smallest];
        heap->array[smallest] = temp;

        pos = smallest;   
    }
    return min;
}
