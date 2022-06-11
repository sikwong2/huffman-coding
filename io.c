#include "code.h"
#include "defines.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

static uint8_t write_buffer[BLOCK];
static uint32_t write_index = 0;

int read_bytes(int infile, uint8_t *buf, int nbytes) {
    ssize_t rbytes = 0;
    ssize_t total = 0;

    //Loop calls to read() to read nbytes. Increment buf by total to
    //offset where in the buffer the bytes are stored as to not overwrite
    //read bytes. Decrement nbytes by the number of bytes read to not read
    //more than nbytes.
    while ((rbytes = read(infile, buf + total, nbytes - total)) > 0) {
        total += rbytes;
    }
    bytes_read += total;
    return total;
}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {
    ssize_t wbytes = 0;
    ssize_t total = 0;

    //Loop calls to write() to write nbytes. Increment buf by total to
    //offset where in the buffer the bytes are stored as to not overwrite
    //written bytes. Decrement nbytes by the number of bytes written to
    //not write more than nbytes.
    while ((wbytes = write(outfile, buf + total, nbytes - total)) > 0) {
        total += wbytes;
    }
    bytes_written += total;
    return total;
}

bool read_bit(int infile, uint8_t *bit) {
    static uint8_t buffer[BLOCK];
    static uint32_t index = 0;
    static uint8_t end = 0;
    static uint8_t bytes = 0;

    //fill buffer with BLOCK # of bytes
    if (index == 0) {
        bytes = read_bytes(infile, buffer, BLOCK);

        //index for EOF
        //only ever less than sizeof(buffer) if read_bytes returns less than
        //a BLOCK of bytes or when EOF
        end = 8 * bytes + 1;
    }

    //set the bit at index
    if ((buffer[index / 8] & (0x1 << (index % 8))) == 0) {
        *bit = 0;
    } else {
        *bit = 1;
    }
    //increment index
    index += 1;

    //reading all bits in buffer
    if (index == 8 * sizeof(buffer)) {
        index = 0;
    }
    if (index != end) {
        return true;
    } else {
        return false;
    }
}

void write_code(int outfile, Code *c) {
    uint32_t size = code_size(c);

    //size is the number of bits in *c
    //write that bit into buffer at write_index
    //if the write_buffer is full, write the contents of the buffer to outfile
    //reset write_index to 0 if end of buffer
    for (uint32_t i = 0; i < size; i += 1) {
        if (write_index == 8 * sizeof(write_buffer)) {
            write_bytes(outfile, write_buffer, sizeof(write_buffer));
            memset(write_buffer, 0, sizeof(write_buffer));
            write_index = 0;
        }

        if (code_get_bit(c, i)) {
            write_buffer[write_index / 8] |= (0x1 << (write_index % 8));
            write_index += 1;
        } else {
            write_buffer[write_index / 8] |= (0 << (write_index % 8));
            write_index += 1;
        }
    }
}

void flush_codes(int outfile) {
    //fill up index/8 byte with 0
    //write 8 - (index % 8) bits
    //then write index / 8 bytes
    for (uint32_t i = 0; i < (write_index % 8); i += 1) {
        write_buffer[write_index / 8] |= (0 << (write_index % 8));
        write_index += 1;
    }
    write_bytes(outfile, write_buffer, (write_index + 7) / 8);

    //zero out buffer
    memset(write_buffer, 0, sizeof(write_buffer));
    write_index = 0;
}
