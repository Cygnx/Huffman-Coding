#include <string>
#include <string.h>
#include <iostream>

using namespace std;
string dictionary[256];
int totalFreq = 0;
void print_bytes(const void *num)
{
	for (int i = 0; i < 4; i++)
		cout << std::hex << ((const unsigned char *)num)[i];
}
unsigned int byteToInt(unsigned char *myBytes)
{
	int sum = 0;
	for (int i = 3; i >= 0; i--)
		sum = (sum << 8) + myBytes[i];
	return sum;
}
void BitAsByte(unsigned char *b)
{
	int indexValue = 1;
	char sum = 0;
	for (int i = 0; i < 8; i++)
	{
		if (b[i] == '1')
			sum += indexValue;
		indexValue *= 2;
	}
	cout << hex << sum;
}
void ByteToBit(char byte, char* s)
{
	for (int i = 0; i < 8; ++i) {
		if ((byte & (1 << i) ? 1 : 0) == 1)
			s[i] = '1';
		else
			s[i] = '0';
	}
	s[8] = '\0';
}
class Node
{
public:
	string code;
	unsigned char myChar;
	unsigned int myFreq;
	Node *left,
		*right,
		*root;

	Node(int myFreq) : myFreq(myFreq), root(NULL), left(NULL), right(NULL){}	// for making parents
	Node() : myFreq(0), root(NULL), left(NULL), right(NULL){}	//for creating iniatial list
	~Node(){}

	bool operator <(const Node& right)
	{
		if (myFreq < right.myFreq)
			return true;
		else if (myFreq > right.myFreq)
			return false;
		else if (myChar < right.myChar)
			return true;
		else
			return false;
	}
};
void buildCode(Node *root)
{
	if (root->left)
	{
		root->left->code = root->code + '0';
		buildCode(root->left);
	}
	if (root->right)
	{
		root->right->code = root->code + '1';
		buildCode(root->right);
	}
	else
		dictionary[root->myChar] = root->code;
}
bool codeToChar(string &b)
{
	for (int i = 0; i < 256; i++)
	{
		if (!strcmp(dictionary[i].c_str(), b.c_str()))
		{
			cout << (unsigned char)i;
			return true;
		}
	}
	return false;
}
void print_tree(Node *root)
{
	if (!root)
		return;
	print_tree(root->left);
	if (!root->left && !root->right)
		cout << endl << root->myChar << " has code:" << root->code;
	print_tree(root->right);
}
void makeNodeLowestFreq(Node **freqList)
{
	Node *parentNode = new Node(freqList[0]->myFreq + freqList[1]->myFreq);

	freqList[0]->root = parentNode;
	freqList[1]->root = parentNode;

	parentNode->left = freqList[1];
	parentNode->right = freqList[0];

	if (parentNode->left->myChar < parentNode->right->myChar)
		parentNode->myChar = parentNode->left->myChar;
	else
		parentNode->myChar = parentNode->right->myChar;
	freqList++;
	freqList[0] = parentNode;
}
void sortfreq(Node **freqList, int left, int right) // sorts from left to right w/ left being the smallest and right being the largest
{
	if (left < right)
	{
		int i = left,
			j = right + 1;
		Node pivot = *freqList[left];
		do
		{
			do i++; while (*freqList[i] < pivot);
			do j--; while (pivot < *freqList[j]);
			if (i < j)
				swap(*freqList[i], *freqList[j]);
		} while (i < j);
		swap(*freqList[left], *freqList[j]);

		sortfreq(freqList, left, j - 1);
		sortfreq(freqList, j + 1, right);
	}
}
class HuffmanTree
{
public:
	Node **freqList;
	unsigned int asciiFreqSortedCount;
	unsigned char *buffer = new unsigned char;
	int length;

	HuffmanTree() : asciiFreqSortedCount(0)
	{}
	~HuffmanTree()
	{}
	void getInput()
	{
		unsigned int asciiFreq[256] = { 0 },
			asciiFreqSorted[256] = { 0 };

		cin.seekg(0, ios::end);
		length = cin.tellg();
		cin.seekg(0, cin.beg);
		for (int i = 0; i < length; i++)
		{
			cin.read((char*)buffer, 1);
			if (!asciiFreq[*buffer]++)
				asciiFreqSorted[asciiFreqSortedCount++] = *buffer;
		}

		for (int i = 0; i < 256; i++)
			print_bytes(&asciiFreq[i]);

		// setup tree
		freqList = new Node*[asciiFreqSortedCount + 1];
		for (int i = 0; i <= asciiFreqSortedCount; i++)
			freqList[i] = new Node;

		for (int i = 0; i < asciiFreqSortedCount; i++)
		{
			freqList[i]->myChar = asciiFreqSorted[i];
			freqList[i]->myFreq = asciiFreq[asciiFreqSorted[i]];
		}
		/*	for (int i = 0; i < 256; i++)
		cout << dec << i <<" --- " <<asciiFreq[i] << endl;*/
	}
	void buildTree()
	{
		freqList[asciiFreqSortedCount]->myFreq = 2147483647;
		sortfreq(freqList, 0, asciiFreqSortedCount);
		//		printTree();
		while (asciiFreqSortedCount >= 2)
		{
			makeNodeLowestFreq(freqList);
			freqList++;
			asciiFreqSortedCount--;

			freqList[asciiFreqSortedCount]->myFreq = 2147483647;
			sortfreq(freqList, 0, asciiFreqSortedCount);

			//				printTree();
		}
		freqList[0]->code = ""; // initialize the root of the tree to have a h/c of blank
		buildCode(freqList[0]);
		//	print_tree(freqList[0]);
		//return;
	}
	void compressData()
	{
		string compressedData;
		string subString;
		unsigned char *substring2 = new unsigned char[8];
		int padding;

		cin.seekg(0, cin.beg);
		for (int i = 0; i < length; i++)
		{
			cin.read((char*)buffer, 1);
			compressedData += dictionary[*buffer];
			if (i == length - 1)
			{
				padding = (compressedData.size() % 8);
				if (padding)
				{
					padding = 8 - padding;

					for (int i = 0; i < padding; i++)
						compressedData += '0';
				}
			}
			if (compressedData.size() >= 8)
			{
				subString = string(&compressedData[0], &compressedData[8]);
				substring2 = (unsigned char*)subString.c_str();
				BitAsByte(substring2);
				compressedData.erase(0, 8);
			}
		}
	}
	void printTree()
	{
		for (int i = 0; i < asciiFreqSortedCount; i++)
			cout << freqList[i]->myChar << " " << freqList[i]->myFreq << endl;
		cout << endl;
	}
	void d_getInput()
	{
		unsigned int asciiFreq[256] = { 0 },
			asciiFreqSorted[256] = { 0 };
		unsigned char bytes[4];
		for (int i = 0; i < 8; i++)
			cin.get();
		for (int i = 0; i < 256; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				cin.read((char*)buffer, 1);
				bytes[j] = *buffer;
			}
			int x = byteToInt(bytes);
			if (x)
			{
				totalFreq += x;
				asciiFreq[i] = x;
				asciiFreqSorted[asciiFreqSortedCount++] = i;
			}
		}

		// setup tree
		freqList = new Node*[asciiFreqSortedCount + 1];
		for (int i = 0; i <= asciiFreqSortedCount; i++)
			freqList[i] = new Node;

		for (int i = 0; i < asciiFreqSortedCount; i++)
		{
			freqList[i]->myChar = asciiFreqSorted[i];
			freqList[i]->myFreq = asciiFreq[asciiFreqSorted[i]];
		}
	}
	void decompressData()
	{
		//	print_tree(freqList[0]);
		string myString, subString;
		char cString[9];
		while (cin.read((char*)buffer, 1))
		{
			ByteToBit(*buffer, cString);
			myString += cString;
			int i = 0;
			for (i = 1; i <= myString.size(); i++)
			{
				subString = string(&myString[0], &myString[i]);
				if (codeToChar(subString) == true)
				{
					totalFreq--;
					if (totalFreq == 0)
						return;
					myString.erase(0, i);
					i = 0;
				}
			}
		}
	}
};
int main(int argc, char *argv[])
{
	HuffmanTree myTree;
	if (argc == 2 && argv[1][1] == 'd')
	{
		myTree.d_getInput();
		myTree.buildTree();
		myTree.decompressData();
	}
	else
	{
		cout << "HUFFMA5";
		cout << '\0';
		myTree.getInput();
		myTree.buildTree();
		myTree.compressData();
	}
	return 0;
}
