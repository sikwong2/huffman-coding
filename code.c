#include "code.h"
#include "defines.h"
#include "stack.h"
#include <inttypes.h>
#include <assert.h>
#include <stdio.h>

Code code_init(void) {
    Code code;
    code.top = 0;

    //zero out array of bits
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i += 1) {
        code.bits[i] = 0;
    }

    return code;
}

uint32_t code_size(Code *c) {
    return c->top;
}

bool code_empty(Code *c) {
    return code_size(c) == 0;
}

bool code_full(Code *c) {
    return c->top == ALPHABET;
}

bool code_set_bit(Code *c, uint32_t i) {
    if (c) {
        if (i >= ALPHABET) {
            return false;
        }
        //You do i / 8 to figure out which byte the bit belongs to
        //and 0x1 << (i % 8) is to figure out how much to shift the bit by
        c->bits[i / 8] = c->bits[i / 8] | 0x1 << (i % 8);

        return true;
    } else {
        return false;
    }
}

bool code_get_bit(Code *c, uint32_t i) {
    if (c) {
        if (i >= ALPHABET || (c->bits[i / 8] & (0x1 << (i % 8))) == 0) {
            return false;
        } else {

            return true;
        }
    } else {
        return false;
    }
}

bool code_clr_bit(Code *c, uint32_t i) {
    if (c) {
        if (i >= ALPHABET) {
            return false;
        }
        //Bitwise AND the ith index with ~0x1 to clear a bit
        c->bits[i / 8] = c->bits[i / 8] & ~(0x1 << (i % 8));
        return true;
    } else {
        return false;
    }
}

bool code_push_bit(Code *c, uint8_t bit) {
    if (c) {
        if (code_full(c) || bit < 0 || bit > 1) {
            return false;
        }
        //Bitwise OR the top index with bit to push a bit and increment top
        c->bits[c->top / 8] = c->bits[c->top / 8] | bit << (c->top % 8);
        c->top += 1;

        return true;
    } else {
        return false;
    }
}

bool code_pop_bit(Code *c, uint8_t *bit) {
    if (c) {
        if (code_empty(c)) {
            return false;
        }
        //pass bit popped bit to the pointer
        *bit = (c->bits[(c->top - 1) / 8] >> (c->top - 1)) & 0x1;
        code_clr_bit(c, c->top - 1);

        //moving top
        c->top -= 1;

        return true;
    } else {
        return false;
    }
}

void code_print(Code *c) {
    for (uint32_t i = 0; i < c->top; i += 1) {
        if ((i % 8 == 0) && i > 0) {
            printf("\n");
        }
        if (code_get_bit(c, i)) {
            printf("1");
        } else {
            printf("0");
        }
    }
    printf("\n");
}
