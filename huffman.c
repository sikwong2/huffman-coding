#include "node.h"
#include "stack.h"
#include "io.h"
#include "code.h"
#include "pq.h"
#include "defines.h"
#include <string.h>
#include <stdio.h>

Node *build_tree(uint64_t hist[static ALPHABET]) {
    PriorityQueue *pq = pq_create(ALPHABET);

    //loop through the hist to find frequencies > 0
    for (int i = 0; i < ALPHABET; i += 1) {
        if (hist[i]) {
            Node *node = node_create(i, hist[i]);
            enqueue(pq, node);
        }
    }
    while (pq_size(pq) > 1) {
        //create left and right node
        Node *left;
        Node *right;

        //dequeue last 2 nodes
        dequeue(pq, &left);
        dequeue(pq, &right);

        // join to make parent node
        Node *parent = node_join(left, right);

        //enqueue the parent node
        enqueue(pq, parent);
    }

    //return last remaining node
    Node *last = 0;
    dequeue(pq, &last);

    //delete priority queue
    pq_delete(&pq);
    return last;
}

void build_codes(Node *root, Code table[static ALPHABET]) {
    static Code c;
    if (c.top == 0) {
        c = code_init();
    }
    uint8_t bit = 0;
    if (root) {
        if (!(root->left) && !(root->right)) {
            table[root->symbol] = c;
        } else {
            code_push_bit(&c, 0);
            build_codes(root->left, table);
            code_pop_bit(&c, &bit);

            code_push_bit(&c, 1);
            build_codes(root->right, table);
            code_pop_bit(&c, &bit);
        }
    }
}
void dump_tree(int outfile, Node *root) {
    if (root) {
        //traverse left and right branches
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);

        //print symbol and 'L' if a leaf node
        if (root->left == NULL && root->right == NULL) {
            write_bytes(outfile, (uint8_t *) "L", sizeof(uint8_t));
            write_bytes(outfile, (uint8_t *) &root->symbol, sizeof(uint8_t));
        } else {
            write_bytes(outfile, (uint8_t *) "I", sizeof(uint8_t));
        }
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[nbytes]) {
    Stack *stack = stack_create(nbytes);

    //iterate over tree_dump
    for (uint16_t i = 0; i < nbytes; i += 1) {

        if (tree[i] == 'I') {
            Node *left;
            Node *right;

            //pop top 2 nodes
            stack_pop(stack, &right);
            stack_pop(stack, &left);

            //create paret
            Node *parent = node_join(left, right);

            //push parent
            stack_push(stack, parent);
        }

        if (tree[i] == 'L') {
            Node *node = node_create(tree[i + 1], 0);
            stack_push(stack, node);
            i += 1;
        }
    }

    //return last node in stack
    Node *root = NULL;
    stack_pop(stack, &root);

    stack_delete(&stack);
    return root;
}

void delete_tree(Node **root) {
    if (*root) {
        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);

        //delete node if no left and right child
        if (!((*root)->left) && !((*root)->right)) {
            node_delete(root);
        }
    }
}
