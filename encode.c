#include "pq.h"
#include "node.h"
#include "stack.h"
#include "code.h"
#include "io.h"
#include "defines.h"
#include "huffman.h"
#include "header.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

//i got this is_seekable function from Eugene
bool is_seekable(int infile) {
    return lseek(infile, 0, SEEK_SET) != -1;
}

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "  A Huffman encoder.\n"
        "  Compresses a file using the Huffman coding algorithm.\n"
        "USAGE\n"
        "  %s [-h] [-i infile] [-o outfile]\n"
        "OPTIONS\n"
        "  -h             Program usage and help.\n"
        "  -v             Print compression statistics.\n"
        "  -i infile      Input file to compress.\n"
        "  -o outfile     Output of compressed data.\n",
        exec);
}
int main(int argc, char **argv) {
    int infile = STDIN_FILENO;
    int outfile = STDOUT_FILENO;

    int opt = 0;

    bool verbose = false;

    while ((opt = getopt(argc, argv, "i:o:hv")) != -1) {
        switch (opt) {
        case 'i': infile = open(optarg, O_RDONLY); break;
        case 'o': outfile = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0600); break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        case 'v': verbose = true; break;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    //if read from stdin, copy contents to a temp file
    if (!is_seekable(infile)) {
        int tempfile = open("/tmp/totallyrandomfile.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);
        unlink("/tmp/totallyrandomfile.txt");

        uint8_t buffer[BLOCK];
        int bytes = 0;

        //copy infile contents to tempfile
        while ((bytes = read_bytes(infile, buffer, BLOCK)) > 0) {
            write_bytes(tempfile, buffer, bytes);
        }
        lseek(tempfile, 0, SEEK_SET);
        close(infile);
        infile = tempfile;
    }

    //zero out histogram
    uint64_t histogram[ALPHABET];
    memset(histogram, 0, sizeof(histogram));
    uint8_t buffer[BLOCK];

    //read in BLOCK bytes into a buffer
    int bytes = 0;
    while ((bytes = read_bytes(infile, buffer, BLOCK)) > 0) {
        //go through the buffer
        for (int i = 0; i < bytes; i += 1) {
            int c = buffer[i];
            histogram[c] += 1;
        }
    }

    //increment element 0 and element 255
    histogram[0] += 1;
    histogram[255] += 1;

    //find the count of unique symbols
    uint16_t symbols = 0;
    for (int i = 0; i < ALPHABET; i += 1) {
        if (histogram[i]) {
            symbols += 1;
        }
    }

    Node *root = build_tree(histogram);

    //generating the code table
    Code table[ALPHABET];

    build_codes(root, table);

    //construct a header
    struct stat stats;
    fstat(infile, &stats);
    Header header;
    header.magic = MAGIC;
    header.permissions = stats.st_mode;
    header.tree_size = (3 * symbols) - 1;
    header.file_size = stats.st_size;

    fchmod(outfile, header.permissions);
    //write header to outfile
    write_bytes(outfile, (uint8_t *) &header, sizeof(header));

    //write huffman tree to outfile
    dump_tree(outfile, root);

    //write_code and flush_code
    lseek(infile, 0, SEEK_SET);

    while ((bytes = read_bytes(infile, buffer, BLOCK)) > 0) {
        for (int i = 0; i < bytes; i += 1) {
            uint8_t symbol = buffer[i];
            write_code(outfile, &table[symbol]);
        }
    }
    flush_codes(outfile);

    if (verbose) {
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", header.file_size);
        fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %+ld %%\n", 100 * (1 - (bytes_written / header.file_size)));
    }

    delete_tree(&root);
    close(infile);
    close(outfile);
    return 0;
}
