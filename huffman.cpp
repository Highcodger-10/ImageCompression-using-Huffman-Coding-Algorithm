#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <cstdlib>
#include <algorithm>
using namespace std;

struct node
{
	node *leftChild;
	node *rightChild;
	int frequency;
	int content;
	string code;

	bool operator<(const node &a) const { return (frequency > a.frequency); }
};

class Huffman
{
	priority_queue<node> nodeArray;
	vector<float> codeTable;
	vector<string> symbolTable;
	vector<int> image;

public:
	// Huffman(){};
	node getHuffmanTree()
	{
		while (!nodeArray.empty())
		{
			node *rootNode = new node;
			node *leftNode = new node;
			node *rightNode = new node;

			*leftNode = nodeArray.top();
			nodeArray.pop();
			*rightNode = nodeArray.top();
			nodeArray.pop();

			rootNode->leftChild = leftNode;
			rootNode->rightChild = rightNode;
			rootNode->frequency = leftNode->frequency + rightNode->frequency;

			nodeArray.push(*rootNode);
			if (nodeArray.size() == 1)
			{ // only the root node exsits
				break;
			}
		}

		return nodeArray.top();
	}

	void BFS(node *temproot, string s)
	{
		node *root1 = new node;
		root1 = temproot;

		root1->code = s;

		if (root1 == NULL)
		{
		}
		else if (root1->leftChild == NULL && root1->rightChild == NULL)
		{
			short i = root1->content;
			codeTable.at(i) = (float)root1->frequency;
			symbolTable.at(i) = root1->code;
		}
		else
		{
			root1->leftChild->code = s.append("0");
			s.erase(s.end() - 1);
			root1->rightChild->code = s.append("1");
			s.erase(s.end() - 1);

			BFS(root1->leftChild, s.append("0"));
			s.erase(s.end() - 1);
			BFS(root1->rightChild, s.append("1"));
			s.erase(s.end() - 1);
		}
	}

	vector<float> getHuffmanCode()
	{
		node root = getHuffmanTree(); // construct the huffman tree

		codeTable.resize(UCHAR_MAX + 1);	 // Code table with 256 bins
		symbolTable.resize(UCHAR_MAX + 1); // Code table with 256 bins

		BFS(&root, ""); // Search tree-basead code with BFS algorithm

		return codeTable; // return table of symbols
	}

	void setFrequenceTable(vector<float> f)
	{
		for (unsigned i = 0; i < f.size(); i++)
		{
			setFrequenceTable(i, f[i]);
		}
	}

	void setFrequenceTable(int ind, float frequency)
	{
		if (frequency <= 0)
			return;

		node temp;
		temp.frequency = (int)frequency;
		temp.content = ind;
		temp.leftChild = NULL;
		temp.rightChild = NULL;
		nodeArray.push(temp);
	}

	string encode(vector<int> e)
	{
		string codifiedImage = "";

		for (unsigned i = 0; i < e.size(); i++)
		{
			if (symbolTable.at(e.at(i)) == "")
			{
				cout << "\nError: Code don't exist in CodeTable." << endl;
			}
			codifiedImage += symbolTable.at(e.at(i));
		}
		return codifiedImage;
	}

	vector<int> decode(string d)
	{
		node root = getHuffmanTree();

		searchContent(&root, d);

		return image;
	}

	void searchContent(node *root, string im)
	{
		node *n = new node;
		n = root;

		size_t imSize = im.length();
		image.clear();

		for (size_t i = 0; i <= imSize; i++)
		{
			if (n->leftChild == NULL && n->rightChild == NULL)
			{ // leaf
				image.push_back(n->content);
				n = root;
			}

			n = (im[i] == '1') ? n->rightChild : n->leftChild;
		}
		im.clear();
	}

	// virtual ~Huffman(){};
};
