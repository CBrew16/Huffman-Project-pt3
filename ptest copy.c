#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffmann_tree.h"  // Include your header file

#define MAX 256

int main() {
    // Step 1: Define filename and output buffers
    const char *filename = "/Users/cameronbrewster/Desktop/Starter-kit 3/text.txt";  // Absolute path to your test file
    char decompressed[100];  // Buffer to store decompressed output
    char compressed[1000];   // Buffer to store compressed data
    int uniqueCharCount = 0; // To store the number of unique characters
    node_t nodes[256];       // Frequency table

    // Step 2: Count the frequencies from the file
    countFrequencies(filename, nodes, &uniqueCharCount);

    // Step 3: Build Huffman Tree
    Queue queue;
    initQueue(&queue);
    for (int i = 0; i < uniqueCharCount; i++) {
        // Create a leaf node for each character
        HuffmanNode* leafNode = createLeafNode(nodes[i].index, nodes[i].weight);
        // Enqueue the leaf node
        priorityEnqueue(&queue, leafNode);
    }
    HuffmanNode *root = buildHuffmanTree(&queue);

    // Step 4: Generate Huffman codes
    char codes[256][MAX] = {0};  // Store the Huffman codes
    HuffmanCodes(root, codes);

    // Step 5: Print the frequency table
    printf("\nFrequency Table:\n");
    for (int i = 0; i < uniqueCharCount; i++) {
        printf("Character: '%c', Frequency: %d\n", nodes[i].index, nodes[i].weight);
    }

    // Step 6: Compress the file
    printf("\nCompressing the file...\n");
    compress(filename, compressed, root, codes);
    printf("Compressed data: %s\n", compressed);  // For debugging

    // Step 7: Decompress the file
    printf("\nDecompressing the file...\n");
    decompress(compressed, decompressed, root);

    // Step 8: Output the decompressed data to verify correctness
    printf("\nDecompressed output: %s\n", decompressed);

    // Verify if the decompressed text matches the original input
    if (strcmp(decompressed, "aaaaa") == 0) {
        printf("\nTest Passed: Decompression is successful.\n");
    } else {
        printf("\nTest Failed: Decompression didn't match the original.\n");
    }

    return 0;
}

