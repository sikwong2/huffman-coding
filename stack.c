#include "stack.h"
#include "node.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>

struct Stack {
    uint32_t top;
    uint32_t capacity;
    Node **items;
};

Stack *stack_create(uint32_t capacity) {
    //allocate mem for stack struct
    Stack *stack = (Stack *) malloc(sizeof(Stack));

    //set capacity of stack
    stack->capacity = capacity;

    //allocate mem for capacity # of nodes
    stack->items = (Node **) malloc(capacity * sizeof(Node *));

    //set top to 0
    stack->top = 0;
    return stack;
}

void stack_delete(Stack **s) {

    //free memory of items
    free((*s)->items);
    (*s)->items = NULL;
    free(*s);
    *s = NULL;
}

bool stack_empty(Stack *s) {
    if (s->top == 0) {
        return true;
    } else {
        return false;
    }
}

bool stack_full(Stack *s) {
    if (s->top == s->capacity) {
        return true;
    } else {
        return false;
    }
}

uint32_t stack_size(Stack *s) {
    return s->top;
}

bool stack_push(Stack *s, Node *n) {
    if (stack_full(s)) {
        return false;
    } else {
        //set top to n
        s->items[s->top] = n;

        //move top
        s->top += 1;

        return true;
    }
}

bool stack_pop(Stack *s, Node **n) {
    if (stack_empty(s)) {
        return false;
    } else {
        //return node before top to n
        *n = s->items[s->top - 1];

        //move top down 1
        s->top -= 1;

        return true;
    }
}

void stack_print(Stack *s) {
    for (uint32_t i = 0; i < s->top; i += 1) {
        printf("%c\n", s->items[i]->symbol);
    }
}
