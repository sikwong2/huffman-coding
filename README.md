# huffman-coding
An implementation of Huffman coding in C. The implementation is able to encode/decode files and from stdin and output to stdout or an output file. 

# Make
To make the the encode and decode executables type `make` or `make all` in the terminal.

# Usage
**encode**
A Huffman encoder.  
Compresses a file using the Huffman coding algorithm.  
**USAGE**  
`./encode [-h] [-i infile] [-o outfile]`
`Flags`
    * `-h` Program usage and help.  
    * `-i [infile]` Input file to compress (default is stdin).  
    * `-o [outfile]` Output of compressed data (default is stdout).

**decode**
A Huffman decoder.  
Decompress an encoded file using the Huffman coding algorithm.  
**USAGE**  

`./encode [-h] [-i infile] [-o outfile]`  
`Flags`
    * `-h` Program usage and help.  
    * `-i [infile]` Input file to decompress (default is stdin).  
    * `-o [outfile]` Output of decompressed data (default is stdin).  
  
# Cleanup
To cleanup all created files, type `make clean` into the command line.
