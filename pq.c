#include "pq.h"
#include "node.h"
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

struct PriorityQueue {
    Node *head;
    uint32_t capacity;
    uint32_t elements;
    Node **nodes;
};

void sort_queue(PriorityQueue *q, uint32_t n) {
    uint32_t j = n - 1;
    Node *temp = q->nodes[j];

    while (j > 0 && temp->frequency > q->nodes[j - 1]->frequency) {
        q->nodes[j] = q->nodes[j - 1];
        j -= 1;
    }
    q->nodes[j] = temp;
}

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *pq = (PriorityQueue *) malloc(sizeof(PriorityQueue));
    pq->capacity = capacity;
    pq->elements = 0;
    pq->nodes = (Node **) malloc(capacity * sizeof(Node *));
    return pq;
}

void pq_delete(PriorityQueue **q) {

    free((*q)->nodes);
    (*q)->nodes = NULL;
    free(*q);
    *q = NULL;
}

bool pq_empty(PriorityQueue *q) {
    if (q->elements == 0) {
        return true;
    } else {
        return false;
    }
}

uint32_t pq_size(PriorityQueue *q) {
    return q->elements;
}

bool pq_full(PriorityQueue *q) {
    if (q->elements == q->capacity) {
        return true;
    } else {
        return false;
    }
}

bool enqueue(PriorityQueue *q, Node *n) {
    if (pq_full(q)) {
        return false;
    } else {
        q->nodes[q->elements] = n;
        q->elements += 1;
        sort_queue(q, q->elements);
        q->head = q->nodes[q->elements - 1];
        return true;
    }
}

bool dequeue(PriorityQueue *q, Node **n) {
    if (pq_empty(q)) {
        return false;
    } else {
        *n = q->head;

        //shift head pointer up
        q->elements -= 1;

        //set new head
        if (q->elements == 0) {
            q->head = q->nodes[q->elements];
        } else {
            q->head = q->nodes[q->elements - 1];
        }
        return true;
    }
}

void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->elements; i += 1) {
        printf("%c %lu\n", q->nodes[i]->symbol, q->nodes[i]->frequency);
    }
}
