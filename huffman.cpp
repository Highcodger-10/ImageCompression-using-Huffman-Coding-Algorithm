#include <iostream>
#include <queue>
#include <map>
#include <cstring>
#include <algorithm>
using namespace std;
struct node
{
	node *leftnode;
	node *rightnode;
	int frequency;
	int content;
	string code;
	bool operator<(const node &a) const
	{
		return frequency > a.frequency;
	}
};
class Huffmantree
{
	priority_queue<node> node_array; // data stored as minheap i.e. high frequency data are at last
	vector<float> codetable;				 // we can make this vector int type as it is to store the frequency of each node
	vector<string> symboltable;
	vector<int> image;

public:
	// Huffmantree();
	// returns the rootnode of the huffman tree
	node getHuffmantree()
	{
		while (!node_array.empty())
		{
			node *rootnode = new node;
			node *left_node = new node;
			node *right_node = new node;
			*left_node = node_array.top();
			node_array.pop();
			*right_node = node_array.top();
			node_array.pop();
			rootnode->leftnode = left_node;
			rootnode->rightnode = right_node;
			rootnode->frequency = left_node->frequency + right_node->frequency;
			node_array.push(*rootnode);
			if (node_array.size() == 1)
			{
				break;
			}
		}
		return node_array.top();
	}
	// using BFS to search the symbol for the content in the tree
	void BFS_Search(node *temproot, string s)
	{
		node *root1 = new node;
		root1 = temproot;
		root1->code = s;
		if (root1 == NULL)
		{
		}
		else if (root1->leftnode == NULL && root1->rightnode == NULL)
		{
			short i = root1->content;
			codetable.at(i) = (float)root1->frequency;
			symboltable.at(i) = root1->code;
		}
		else
		{
			root1->leftnode->code = s.append("0");
			s.erase(s.end() - 1);
			root1->rightnode->code = s.append("1");
			s.erase(s.end() - 1);
			BFS_Search(root1->leftnode, s.append("0"));
			s.erase(s.end() - 1);
			BFS_Search(root1->rightnode, s.append("1"));
			s.erase(s.end() - 1);
		}
	}
	// computes and returns the table of symbols
	vector<float> gethuffmancode()
	{
		node root = getHuffmantree();
		codetable.resize(UCHAR_MAX + 1);
		symboltable.resize(UCHAR_MAX + 1);
		BFS_Search(&root, "");
		return codetable;
	}
	// sets up the frequency table to create the huffman tree
	void setfrequencytable(vector<float> f)
	{
		for (unsigned int i = 0; i < f.size(); i++)
			setfrequencytable(i, f[i]);
	}
	void setfrequencytable(int ind, float frequency)
	{
		if (frequency < 0)
		{
			return;
		}
		node temp;
		temp.frequency = (int)frequency;
		temp.content = ind;
		temp.leftnode = NULL;
		temp.rightnode = NULL;
		node_array.push(temp);
	}
	// to encode into 0s and 1s
	string encode(vector<int> e)
	{
		string codedimage = "";
		for (unsigned int i = 0; i < e.size(); i++)
		{
			if (symboltable.at(e.at(i)) == "")
			{
				cout << endl
						 << "Code doesn't exist at the code table" << endl;
			}
			codedimage += symboltable.at(e.at(i));
		}
		return codedimage;
	}
	// to decode the string
	vector<int> decode(string d)
	{
		node root = getHuffmantree();
		serach_content(&root, d);
		return image;
	}
	// used to traverse to decode a particular bit pattern in string im
	void serach_content(node *root, string im)
	{
		node *n = new node;
		n = root;
		size_t imsize = im.length(); // normal unsigned int data type
		image.clear();							 // to clear the data of the vector image
		for (size_t i = 0; i <= imsize; i++)
		{
			if (n->leftnode == NULL && n->rightnode == NULL)
			{
				image.push_back(n->content);
				n = root;
			}
			n = (im[i] == '1') ? n->rightnode : n->leftnode;
		}
		im.clear();
	}
	// virtual ~Huffmantree();
};

