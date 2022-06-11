#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node *node = (Node *) malloc(sizeof(Node));

    node->left = NULL;
    node->right = NULL;
    node->symbol = symbol;
    node->frequency = frequency;

    return node;
}

void node_delete(Node **n) {
    free(*n);
    *n = NULL;
}

Node *node_join(Node *left, Node *right) {
    Node *parent_node = node_create('$', left->frequency + right->frequency);

    //set left and right child
    parent_node->left = left;
    parent_node->right = right;

    return parent_node;
}

void node_print(Node *n) {
    if (n) {
        printf("symbol = %c\n", n->symbol);
        printf("frequency = %lu\n", n->frequency);
        if (n->left != NULL) {
            printf("left symbol = %c\nleft frequency = %lu\n", n->left->symbol, n->left->frequency);
        } else {
            printf("left node = NULL\n");
        }
        if (n->right != NULL) {
            printf("right symbol = %c\nright frequency = %lu\n", n->right->symbol,
                n->right->frequency);
        } else {
            printf("right node = NULL\n");
        }
    }
}
