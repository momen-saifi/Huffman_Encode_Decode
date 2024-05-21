## Huffman_Encode_Decode

This project implements the Huffman Coding algorithm, a widely used lossless data compression technique. It reduces file size by assigning shorter codes to frequently occurring characters and longer codes to less frequent characters. This algorithm is employed in various file compression formats like ZIP and JPEG.

## Table of Contents

Project Overview
Data Structures
Functions
Usage
File Formats
Installation (if applicable)
Examples (if applicable)
Contributing (if applicable)
License
## Project Overview

The Huffman Compression Algorithm implemented here comprises two core functionalities: encoding and decoding.

### Encoding

Reads an input file and analyzes individual character frequencies.
Constructs a Huffman tree, optimally assigning shorter codes to frequently occurring characters.
Creates a compressed output file (.huffman extension) containing the encoded data and a header for decompression.
### Decoding

Reads a Huffman-encoded file (.huffman extension).
Uses the accompanying frequency table file (.table extension) to rebuild the Huffman tree.
Decodes the compressed data using the tree and writes the decompressed content to an output file.
## Data Structures

Node Structure: Represents a node in the Huffman tree. It includes:
Pointers to left and right child nodes (if applicable)
Character (for leaf nodes)
Frequency count
## Functions

createNode: Creates a new node with the specified frequency, character, and child nodes.
insertIntoQueue: Inserts a node into a priority queue based on its frequency for efficient tree construction.
removeFromQueue: Removes the node with the lowest frequency from the priority queue, ensuring optimal code assignment.
generateCode: Generates and assigns Huffman codes for each character in the tree, reflecting their relative frequencies.
importData: Reads input file data, calculates character frequencies, and builds the Huffman tree.
importTable: Reads frequency data from a table file (.table) to build the Huffman tree for decoding.
printCodeTable: Prints a table showing character frequencies and corresponding Huffman codes, providing insights into compression effectiveness.
encodeFile: Encodes data from an input file using Huffman codes and saves it to an output file with a header.
decodeFile: Decodes data from a Huffman-encoded file and saves the decompressed data to an output file.
huffmanEncode: Handles the encoding process, taking user input for the file and performing compression.
huffmanDecode: Handles the decoding process, taking user input for the file and performing decompression.
## Usage

Compile the C code using a C compiler (e.g., gcc gcc huffman.c -o huffman).
Run the compiled program (./huffman).
Choose the option:
Encode File: Compress a file using Huffman coding.
Decode File: Decompress a Huffman-encoded file.
Exit: Terminate the program.
Follow on-screen prompts as needed.
## File Formats

Input File: The file to be compressed or decompressed.
Output File: The compressed or decompressed data.
Frequency Table File (.table): Stores character frequencies for decoding (generated during encoding).
Compressed File (.huffman): Stores the Huffman-encoded data along with a header containing information about the compressed data.
Note: Frequency table and compressed files share the same base name (e.g., example.huffman and example.table).

## Installation (if applicable)

(Include instructions for any specific dependencies required by the project. If no dependencies exist, remove this section.)

## Examples (if applicable)

(Provide usage examples or remove this section if not applicable.)

## Contributing (if applicable)

(Include contribution guidelines if you encourage collaboration, otherwise remove this section.)

## License

This project is licensed under the MIT License. You can find a copy of the license in the LICENSE file.
