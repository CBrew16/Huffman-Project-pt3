//
// Created by Cameron Brewster on 2024-11-20.
//
#include "huffmann_tree.h"



HuffmanNode *createLeafNode(char data, int frequency) {
    HuffmanNode * newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode)); // Allocating memory
    if(newNode == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE); //Edge case if memory allocation fails
    }

    newNode -> frequency = frequency;
    newNode -> data = data;
    newNode -> left = NULL;
    newNode -> right =NULL; // Initializing leaf node
    return newNode;

}


HuffmanNode *createInternalNodes(HuffmanNode *left, HuffmanNode *right) {
    HuffmanNode *newNode = (HuffmanNode*)malloc(sizeof(HuffmanNode)); // allocating memory for internal node
    if(newNode == NULL) {
        printf("Memory allocation failed");
        exit(EXIT_FAILURE);  //Edge case if memory allocation fails
    }
    newNode -> frequency = left->frequency + right->frequency; // Setting the frequency to the sum of both its child nodes
    newNode -> data = '\0'; // Internal nodes do not hold a character
    newNode -> left = left; // Point to children
    newNode -> right = right;

    return newNode;
}


HuffmanNode* buildHuffmanTree(Queue* queue) {
    while (queue->front != NULL && queue->front->next != NULL) {  // While more than one node in the queue

        // Dequeue the two nodes with the smallest frequencies
        HuffmanNode* left = priorityDequeue(queue);
        HuffmanNode* right = priorityDequeue(queue);

        // Create a new internal node
        HuffmanNode* internalNode = createInternalNodes(left, right);

        // Enqueue the new internal node back into the queue
        priorityEnqueue(queue, internalNode);
    }

    // The final node left in the queue is the root of the Huffman tree
    return priorityDequeue(queue);
}

void freeHuffmanTree(HuffmanNode* root) {
    if(root == NULL) {
        return; // Edge case if tree DNE
    }
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right); // Recursive calls

    free(root);
}
void getCode(HuffmanNode *root, char codes[256][MAX], char* currentCode, int depth){
    if(root == NULL) {
        return; // Edge case if the tree doesn't exist
    }

    if(root -> left == NULL && root-> right == NULL) { // The base case to stop recustion; Tells it to read a character
        currentCode[depth] = '\0';
        strcpy(codes[(unsigned char)root->data], currentCode);  // Store the code for this character
        printf("Character :%c, Code: %s\n", root->data, currentCode);
        return;
    }
    currentCode[depth] = '0'; // Appending 0 to the string after the character has been added at the left node
    getCode(root->left, codes, currentCode, depth+1); // pre-order Recursively travesering
    currentCode[depth] = '1'; // Appending 1 to the string after the character has been added and right node reached
    getCode(root->right, codes, currentCode, depth+1); // recursively calling left subtree

}


// Function declarations in huffmann_tree.h
void HuffmanCodes(HuffmanNode *root, char codes[256][MAX])
{
     // initializing the array to store codes to zero
    char currentCode[MAX] = {0}; // Initializing the array that keeps track of the codes in the traversal

    getCode(root, codes, currentCode, 0); // Calling the getCode function to append the codes to the string
}

void countFrequencies(const char *filename, node_t *nodes, int *uniqueCharCount) {
    // Step 1: Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        exit(1);
    }

    // Step 2: Initialize frequency array
    int frequencies[256] = {0}; // One entry for each ASCII character

    // Step 3: Read characters and count occurrences
    int c; // Variable to store each character read from the file
    while ((c = fgetc(file)) != EOF) {
        frequencies[c]++;
    }

    // Step 4: Populate the nodes array with non-zero frequencies
    int nodeIndex = 0; // Keeps track of how many nodes we've added
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            nodes[nodeIndex].index = i;          // ASCII value of the character
            nodes[nodeIndex].weight = frequencies[i]; // Frequency of the character
            nodeIndex++;
        }
    }

    // Step 5: Store the number of unique characters
    *uniqueCharCount = nodeIndex;

    // Step 6: Close the file
    fclose(file);
}

HuffmanNode* priorityDequeue(Queue* q) {
    if (q->front == NULL) {
        fprintf(stderr, "Queue is empty! Cannot dequeue.\n");
        return NULL;  // Return NULL if the queue is empty
    }

    // Store the data of the front node to return
    QueueNode* temp = q->front;
    HuffmanNode* dequeuedNode = temp->node;

    // Move the front pointer to the next node
    q->front = q->front->next;

    // If the queue is now empty, set rear to NULL
    if (q->front == NULL) {
        q->rear = NULL;
    }

    // Free the memory of the dequeued node
    free(temp);

    return dequeuedNode;
}




void freeQueue(Queue* q){
    QueueNode* current = q->front;
    while (current != NULL) {
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    q->front = NULL;
    q->rear = NULL;
}

void priorityEnqueue(Queue* q, HuffmanNode* newNode) {
    // Allocate memory for the new QueueNode
    QueueNode* newQueueNode = (QueueNode*)malloc(sizeof(QueueNode));
    if (!newQueueNode) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);  // Exit if memory allocation fails
    }
    newQueueNode->node = newNode;
    newQueueNode->next = NULL;

    // If the queue is empty, insert at the front
    if (q->front == NULL) {
        q->front = q->rear = newQueueNode;
        return;
    }

    // If the new node has a smaller frequency than the front node, insert at the front
    if (newNode->frequency < q->front->node->frequency) {
        newQueueNode->next = q->front;
        q->front = newQueueNode;
        return;
    }

    // Traverse to find the correct insertion point (based on frequency)
    QueueNode* current = q->front;
    while (current->next != NULL && current->next->node->frequency <= newNode->frequency) {
        current = current->next;
    }

    // Insert the new node
    newQueueNode->next = current->next;
    current->next = newQueueNode;

    // If inserted at the end, update the rear pointer
    if (newQueueNode->next == NULL) {
        q->rear = newQueueNode;
    }
}



void initQueue(Queue* q) {
    q->front = NULL;
    q->rear = NULL;
}

void compress(const char *filename, char *compressed, HuffmanNode *root, char codes[256][MAX]) {
    // Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file '%s'\n", filename);
        exit(1);  // Exit if the file can't be opened
    }

    int i, j = 0;  // 'i' for reading characters, 'j' for writing to compressed output
    char currentByte = 0;  // Temporary variable to store a byte of compressed data

    while (1) {
        char c = fgetc(file);  // Read one character from the file
        if (c == EOF) break;  // Stop if end of file is reached

        // Get the Huffman code for the current character
        char *code = codes[(unsigned char)c];

        // Append the Huffman code to the current byte (using bitwise operations)
        for (int k = 0; code[k] != '\0'; k++) {
            // Shift the bits to make space for the next bit in the byte
            currentByte = currentByte << 1;

            // If the code is '1', set the least significant bit (LSB) of the byte
            if (code[k] == '1') {
                currentByte |= 1;
            }

            // If the byte is full (8 bits), store it and reset
            if (k % 8 == 7) {
                compressed[j++] = currentByte;
                currentByte = 0;  // Reset byte
            }
        }
    }

    // If there are remaining bits in the currentByte, store them
    if (j % 8 != 0) {
        compressed[j++] = currentByte;
    }

    fclose(file);  // Close the file
    compressed[j] = '\0';  // Null terminate the compressed output (optional)
}




void decompress(const char *compressed, char *decompressed, HuffmanNode *root) {
    HuffmanNode *currentNode = root;
    int j = 0;  // Index for the decompressed output

    // Iterate through each character in the compressed bitstream
    for (int i = 0; compressed[i] != '\0'; i++) {
        // Traverse the tree based on the bit ('0' or '1')
        if (compressed[i] == '0') {
            currentNode = currentNode->left;  // Go left for '0'
        } else if (compressed[i] == '1') {
            currentNode = currentNode->right;  // Go right for '1'
        }

        // If a leaf node is reached, output the character and reset to root
        if (currentNode->left == NULL && currentNode->right == NULL) {
            decompressed[j++] = currentNode->data;  // Add character to decompressed output
            currentNode = root;  // Reset to root of the tree for the next character
        }
    }

    decompressed[j] = '\0';  // Null-terminate the decompressed string
}





