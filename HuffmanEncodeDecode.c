
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//node structure
typedef struct Node {
    struct Node *left, *right;
    int frequency;
    char character;
} *NodePtr;

// global variables
int nodeCount = 0, queueEnd = 1; //variables for counting node and tracking end of queue
struct Node pool[256] = {{0}};//pool of nodes
NodePtr queue[255], *q = queue - 1; //priority queue
char *codes[128] = {0}, buffer[1024];//string array contain huffman codes for each letter
int inputData = 0, outputData = 0;

//function used to create new node
NodePtr createNode(int freq, char c, NodePtr left, NodePtr right) {
    NodePtr node = pool + nodeCount++;
    if (freq != 0) {
        node->character = c;
        node->frequency = freq;
    } else {
        node->left = left;
        node->right = right;
        node->frequency = left->frequency + right->frequency;
    }
    return node;
}

//function used to insert a node into the priority queue
void insertIntoQueue(NodePtr node) {
    int j, i = queueEnd++;
    while ((j = i / 2)) {
        if (queue[j]->frequency <= node->frequency) break;
        queue[i] = queue[j];
        i = j;
    }
    queue[i] = node;
}

//function used to remove a node from the priority queue
NodePtr removeFromQueue() {
    int i, l;
    NodePtr node = queue[i = 1];

    if (queueEnd < 2) return 0;
    queueEnd--;
    while ((l = i * 2) < queueEnd) {
        if (l + 1 < queueEnd && queue[l + 1]->frequency < queue[l]->frequency) l++;
        queue[i] = queue[l];
        i = l;
    }
    queue[i] = queue[queueEnd];
    return node;
}

//generate and assign the code for each character using huffman tree
void generateCode(NodePtr node, char *s, int len) {
    static char *out = buffer;
    if (node->character) {
        s[len] = 0;
        strcpy(out, s);
        codes[(int)node->character] = out;//code is assign the relevant character.
        out += len + 1;//increment the pointer to point next memory location
        return;
    }

    s[len] = '0';
    generateCode(node->left, s, len + 1);
    s[len] = '1';
    generateCode(node->right, s, len + 1);
}

//function used to retrieve data form file and build tree
void importData(FILE *inputFile, unsigned long long *frequency) {
    char c, s[16] = {0};
    int i = 0;
    printf("File Read:\n");
    while ((c = fgetc(inputFile)) != EOF) {
        frequency[(int)c]++;//read the file character by character and increment the particular frequency
        putchar(c);
    }
    for (i = 0; i < 128; i++)
        if (frequency[i]) insertIntoQueue(createNode(frequency[i], i, 0, 0));
    while (queueEnd > 2)
        insertIntoQueue(createNode(0, 0, removeFromQueue(), removeFromQueue()));
    generateCode(queue[1], s, 0);//build code
}

//to get data from table file and build tree
void importTable(FILE *tableFile, unsigned long long *frequency){
	char c;						//temporary variable
	int i = 0;
	char s[16]={0};

	fread(frequency, sizeof(unsigned long long), 128, tableFile);//get the freqency of char from the .table file to build  huffman tree

	for (i = 0; i < 128; i++)
		if (frequency[i]) insertIntoQueue(createNode(frequency[i], i, 0, 0));	//insert new nodes into the que if there is a frequency
	while (queueEnd > 2)
		insertIntoQueue(createNode(0, 0, removeFromQueue(), removeFromQueue()));		//build the tree
}

//print the char and their freq and huffman code
void printCodeTable(unsigned long long *frequency) {
    int i;
    printf("\n---------CODE TABLE---------\n----------------------------\nCHAR  FREQ  CODE\n----------------------------\n");
    for (i = 0; i < 128; i++) {
        if (isprint((char)i) && codes[i] != NULL && i != ' ') {
            printf("%-4c  %-4d  %16s\n", i, frequency[i], codes[i]);
        } else if (codes[i] != NULL) {
            switch (i) {
                case '\n':
                    printf("\\n  ");
                    break;
                case ' ':
                    printf("\' \' ");
                    break;
                case '\t':
                    printf("\\t  ");
                    break;
                default:
                    printf("%0X  ", (char)i);
                    break;
            }
            printf("  %-4d  %16s\n", frequency[i], codes[i]);
        }
    }
    printf("----------------------------\n");
}

//function used to encode the data and save in file
void encodeFile(FILE *inputFile, FILE *outputFile, unsigned long long *frequency) {
    char in, c,temp[20] = {0};
    int i, j = 0, k = 0, lim = 0;

    rewind(inputFile);//reset the position to the begnning of file

    for (i = 0; i < 128; i++) {
        if (frequency[i]) lim += (frequency[i] * strlen(codes[i]));// length of bitstreem
    }
    outputData = lim;
    fprintf(outputFile, "%04d\n", lim);
    printf("\nEncoded:\n");

    for (i = 0; i < lim; i++) {
        if (temp[j] == '\0') {
            in = fgetc(inputFile);
            strcpy(temp, codes[in]);
            printf("%s", codes[in]);
            j = 0;
        }
        if (temp[j] == '1')
            c = c | (1 << (7 - k));//shifts 1 to relevant position and OR with the temporary char
        else if (temp[j] == '0')
            c = c | (0 << (7 - k));//shifts 0 to relevant position and OR with the temporary char
        else
            printf("ERROR: Wrong input!\n");
        k++;// k is used to divide the string into 8 bit chunks and save
        j++;
        if (((i + 1) % 8 == 0) || (i == lim - 1)) {
            k = 0;//reset k
            fputc(c, outputFile);//save the character
            c = 0;//reset character
        }
    }
    putchar('\n');
}

//function used to decode the data and save in file
void decodeFile(FILE *huffmanFile, FILE *outputFile) {
    int i = 0, lim = 0, j = 0;
    char c;
    NodePtr node = queue[1];

    fscanf(huffmanFile, "%d", &lim);//get the length of the bit stream from header
    fseek(huffmanFile, 2, SEEK_CUR);//seek one position to avoid new line character of the header

    printf("Decoded:\n");
    for (i = 0; i < lim; i++) {
        if (j == 0)
            c = fgetc(huffmanFile);

        if (c & 128)//if the anding of the character with b1000 0000 is true then,
            node = node->right;//1 go right
        else
            node = node->left;//else go left

        if (node->character) { //until a leaf (node with a character) meets
            putchar(node->character); //spit that character out and
            fputc(node->character, outputFile);//save the character in file
            node = queue[1];//reset the que
        }

        c = c << 1;//shift the character by 1
        if (++j > 7)
            j = 0;
    }
    putchar('\n');
    if (queue[1] != node)
        printf("Garbage input.\n");
}

//function used to handle encoding
void huffmanEncode() {
    FILE *inputFile, *outputFile;
    char fileName[50] = {0};
    unsigned long long frequency[128] = {0}, i;//frequency array
    printf("Enter the file to be compressed: ");
    scanf("%s", fileName);

    if (strlen(fileName) >= 50) {
        printf("ERROR: Enter a file name less than 50 chars.\n");
        return;
    }

    if ((inputFile = fopen(fileName, "r")) == NULL) {
        printf("ERROR: No such file.\n");
        return;
    }

    importData(inputFile, frequency);//import the file and fills frequency array
    printCodeTable(frequency);// print code for data

    strcat(fileName, ".huffman");
    outputFile = fopen(fileName, "w");
    encodeFile(inputFile, outputFile, frequency);

    fclose(inputFile);
    fclose(outputFile);

    strcat(fileName, ".table");//append .table with file name
    outputFile = fopen(fileName, "wb");

    fwrite(frequency, sizeof(unsigned long long), 128, outputFile);//write the frequency of char, table in .table extension

    for (i = 0; i < 128; i++) {
        inputData += frequency[i];//calculate input bytes
    }

    fclose(outputFile);

    printf("\nInput bytes: %d\n", inputData);
    outputData = (outputData % 8) ? (outputData / 8) + 1 : (outputData / 8);
    printf("Output bytes: %d\n", outputData);

    printf("\nCompression ratio: %.2f%%\n\n", ((double)(inputData - outputData) / inputData) * 100);
    return 0;
}

//function used to handle encoding
void huffmanDecode() {
    FILE *tableFile, *huffmanFile, *outputFile;
    char fileName[50] = {0}, temp[50] = {0};
    unsigned long long frequency[128] = {0};

    printf("Enter the file to be decompressed: ");
    scanf("%s", fileName);

    if (strstr(fileName, "huffman") == NULL) {
        printf("ERROR: Wrong file format!\n");
        return ;
    }

    if ((huffmanFile = fopen(fileName, "r")) == NULL) {
        printf("ERROR: No such file.\n");
        return;
    }

    strcat(fileName, ".table");//file pointer for .table file
    if ((tableFile = fopen(fileName, "rb")) == NULL) {//open in read binary mode
        printf("ERROR: Frequency table cannot be found.\n");
        return;
    }

    importTable(tableFile, frequency);//import the file and fills frequency array


    *strstr(fileName, ".huffman") = '\0';//search for .huffman and remove it
    strcpy(temp, "mkdir ");//concatenating strings for the command
    strcat(temp, fileName);//to make a directory with the file name
    system(strcat(temp, ".decoded"));//with .decoded at the end

    strcpy(temp, "./");//concatenating string to save the file with the original name in the .decoded derectory
    strcat(temp, fileName);
    strcat(temp, ".decoded/");
    if((outputFile = fopen(strcat(temp, fileName), "w"))==NULL){
       printf("ERROR:Creating decoded file failed\n");	//if any error occured durig creating file
       return ;
    }
    decodeFile(huffmanFile, outputFile);//decode the file and save

    fclose(huffmanFile);
    fclose(tableFile);
    fclose(outputFile);
    return 0;
}

int main() {
    int choice = 0;


        printf("\n1. Encode File\n2. Decode File\n3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                huffmanEncode();
                break;
            case 2:
                huffmanDecode();
                break;
            case 3:
                return 0;
            default:
                printf("Wrong choice. Please try again.\n");
        }


    return 0;
}

