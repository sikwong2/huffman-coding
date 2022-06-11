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

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "  A Huffman decoder.\n"
        "  Decompresses a file using the Huffman coding algorithm.\n"
        "USAGE\n"
        "  %s [-h] [-i infile] [-o outfile]\n"
        "OPTIONS\n"
        "  -h             Program usage and help.\n"
        "  -v             Print compression statistics.\n"
        "  -i infile      Input file to decompress.\n"
        "  -o outfile     Output of decompressed data.\n",
        exec);
}

//i got this is_seekable function from Eugene
bool is_seekable(int infile) {
    return lseek(infile, 0, SEEK_SET) != -1;
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
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); return EXIT_FAILURE;
        default: usage(argv[0]); return EXIT_FAILURE;
        }
    }

    //if read from stdin, copy contents to a temp file
    if (!is_seekable(infile)) {
        int tempfile = open("/tmp/totallyrandomfile2.txt", O_CREAT | O_TRUNC | O_RDWR, 0600);
        unlink("/tmp/totallyrandomfile2.txt");

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

    //read in header
    Header header;
    read_bytes(infile, (uint8_t *) &header, sizeof(header));
    if (header.magic != MAGIC) {
        fprintf(stderr, "Invalid Magic number.\n");
        return -1;
    }

    //set outfile perms
    fchmod(outfile, header.permissions);

    //rebuild tree
    uint8_t tree_dump[header.tree_size];
    read_bytes(infile, tree_dump, header.tree_size);

    Node *root = rebuild_tree(header.tree_size, tree_dump);

    //read bits
    Node *node = root;
    uint64_t decoded = 0;
    uint8_t bit;
    while (decoded != header.file_size) {
        //read bit
        read_bit(infile, &bit);
        //printf("bit = %d\n", bit);
        if (bit == 1 && node) {
            node = node->right;
        }
        if (bit == 0 && node) {
            node = node->left;
        }

        //check if leaf node
        if (node && !(node->left) && !(node->right)) {
            write_bytes(outfile, &node->symbol, sizeof(uint8_t));
            decoded += 1;
            node = root;
        }
    }

    //verbose
    if (verbose && header.file_size > 0) {
        fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
        fprintf(stderr, "Decompressed file size: %lu bytes\n", header.file_size);
        fprintf(stderr, "Space saving: %lu %%\n", 100 * (1 - (bytes_written / header.file_size)));
    }

    delete_tree(&root);
    close(infile);
    close(outfile);
    return 0;
}
