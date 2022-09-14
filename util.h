#include <iostream>
#include <fstream>
#include <map>
#include <queue>
#include <vector>
#include <functional>
#include <string>
#include "bitstream.h"
#include "hashmap.h"
#include "mymap.h"

#pragma once

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

// functor used for flipping the ordering of the priority queue
struct Increasing {
	bool operator () (HuffmanNode* a, HuffmanNode* b) {
		return a -> count > b -> count;
	}
};

// recursively frees the memory allocated to the Huffman tree
void freeTree(HuffmanNode* node) {
	if(node == nullptr)
		return;

	freeTree(node -> zero);
	freeTree(node -> one);

	delete node;
}

/* helper function which  inserts or increment a charcter into
	the hashmap depending on whether or not hashmap  contains it */
void getMap(char& c, hashmap &map) {
	if(!map.containsKey(c))
		map.put(c, 1);
	else
		map.put(c, map.get(c) + 1);
}

// builds a frequency map using a string or the file
void buildFrequencyMap(string filename, bool isFile, hashmap &map) {
	if(!isFile) {
		for(char c : filename)
			getMap(c, map);
	} else {
		ifstream fs(filename);
		char c;

		while(fs.get(c))
			getMap(c, map);
		fs.close();
	}

	map.put(PSEUDO_EOF, 1);
}

// uses a priority queue in order to create an encoding tree
HuffmanNode* buildEncodingTree(hashmap &map) {
	priority_queue<HuffmanNode*, vector<HuffmanNode*>, Increasing> pq;

	vector<int> keyContainer = map.keys();

	for(size_t t = 0; t < keyContainer.size(); t++) {
		HuffmanNode* newNode = new HuffmanNode();
		newNode -> character = keyContainer[t];
		newNode -> count = map.get(keyContainer[t]);
		newNode -> zero = nullptr;
		newNode -> one = nullptr;
		pq.push(newNode);
	}

	while(pq.size() != 1) {
		HuffmanNode* treeNode = new HuffmanNode();
		treeNode -> zero = pq.top();
		pq.pop();
		treeNode -> one = pq.top();
		pq.pop();
		treeNode -> count = treeNode -> zero -> count + treeNode -> one -> count;
		treeNode -> character = NOT_A_CHAR;

		pq.push(treeNode);
	}

    return pq.top();
}

// helper for buildEncodingMap
void recursiveHelperBuildEncode(HuffmanNode* root, mymap<int, string>& encodingMap, string str) {
	if(root == nullptr)
		return;

	if(root -> character != NOT_A_CHAR)
		encodingMap.put(root -> character, str);

	recursiveHelperBuildEncode(root -> zero, encodingMap, str + "0");
	recursiveHelperBuildEncode(root -> one, encodingMap, str + "1");
}

// encodes all the chars inside the map
mymap<int, string> buildEncodingMap(HuffmanNode* tree) {
    mymap<int, string> encodingMap;
	recursiveHelperBuildEncode(tree, encodingMap, "");

    return encodingMap;
}

// encodes the input returning its bitsream representation
string encode(ifstream& input, mymap<int, string> &encodingMap, ofbitstream& output, int& size, bool makeFile) {
	string str = "";
	char q;

	while(input.get(q))
		str += q;

	string bitstream = "";

	for(char c : str)
		bitstream += encodingMap[c];
	bitstream += encodingMap[PSEUDO_EOF];

	size = bitstream.size();

	if(makeFile)
		for(size_t t = 0; t < bitstream.size(); t++)
			output.writeBit(bitstream[t] - '0');

    return bitstream;
}

// reverses encode and returns a string derivated from the bitsream
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
	HuffmanNode* ptr = encodingTree;
	string str = "";

	while(true) {
		if(ptr -> character != NOT_A_CHAR) {  // if it is a char
			if(ptr -> character == PSEUDO_EOF)
				break;
			str += ptr -> character;
			ptr = encodingTree;
		}

		if(input.readBit() == 0)
			ptr = ptr -> zero;
		else
			ptr = ptr -> one;
	}

	output << str;

    return str;
}

// compresses the file
string compress(string filename) {
	ifstream fs(filename);
	hashmap hashMap;
	int size = 0;

	buildFrequencyMap(filename, true, hashMap);
	HuffmanNode* huffNode = buildEncodingTree(hashMap);
	mymap<int, string> theMap = buildEncodingMap(huffNode);

	ofbitstream output(filename + ".huf");
	output << hashMap;

	string str = encode(fs, theMap, output, size, true);

	fs.close();
	output.close();

	freeTree(huffNode);

    return str;
}

// decompresses the file
string decompress(string filename) {
	int pos = filename.find(".huf");
	if(pos >= 0)
		filename = filename.substr(0, pos);

	pos = filename.find(".");
	string ext = filename.substr(pos, filename.length() - pos);
	filename = filename.substr(0, pos);
	ifbitstream input(filename + ext + ".huf");
	ofstream output(filename + "_unc" + ext);

	hashmap hashMap;
	input >> hashMap;

	HuffmanNode* encodingTree = buildEncodingTree(hashMap);

	string decodeStr  = decode(input, encodingTree, output);

	input.close();
	output.close();
	freeTree(encodingTree);

    return decodeStr;
}
