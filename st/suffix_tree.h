#pragma once
#include "red_black_tree.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <uni_algo/all.h>
#include <vector>

//
// Additional variables:
// - `active point` - a triple (`active node`, `active edge`, `active length`), showing from where we must start
// inserting a new suffix.
// - `remainder` - shows the number of suffixes we must add `explicitly`. For instance, if our word is `abcaabca`, and
// `remainder` = 3, it means we must process 3 last suffixes: `bca`, `ca`, and `a`.
//
// Let all the nodes, except the `root` and `leaves`, be `internal nodes`.
//
// Observation 1:
// When the final suffix we need to insert is found to exist in the tree already, the tree itself is not changed at all
// (we only update the `active point` and `remainder`).
//
// Observation 2:
// If at some point `active length` is greater or equal to the length of current edge (`edge length`), we move our
// `active point` down until `edge length` is strictly greater than `active_length`.
//
// Rule 1:
// If after an insertion from the `active node` = `root`, the `active length` is greater than 0, then:
// - `active node` is not changed
// - `active length` is decremented
// - `active edge` is shifted right (to the first character of the next suffix we must insert)
//
// Rule 2:
// If we create a new `internal node` OR make an inserter from an `internal node`, and this is not the first SUCH
// `internal node` at current step, then we link the previous SUCH node with THIS one through a `suffix link`.
//
// Rule 3:
// After an insert from the `active node` which is not the `root` node, we must follow the `suffix link` and set the
// `active node` to the node it points to. If there is not a `suffix link`, set the `active node` to the `root` node.
// Either way, `active edge` and `active length` stay unchanged.
//
// Observation 3:
// When the symbol we want to add to the tree is already on the edge, we, according to `Observation 1`, update only
// `active point` and `remainder`, leaving the tree unchanged. BUT if there is an `internal node` marked as `needing
// suffix link`, we must connect that node with our current `active node` through a `suffix link`.
//
class SuffixTreeRB
{
  private:
	// Define infinity constant, useful for canonization.
	static const int oo = std::numeric_limits<int>::max();
	static const char32_t delim = 36;

	//
	// A node consists of a parent-node edge, all children and (maybe) a suffix link.
	// An edge is specified by the start index and end index of the string.
	//
	class Node
	{
	  public:
		// The parent-node edge.
		int start;
		int end;

		// The suffix link, represented by the destination node index.
		int link;

		// The node-child edges.
		// This map the start character of the edge to the child node index.
		RBTree<char32_t, int> next;

	  public:
		Node() : start(-1), end(-1), link(0){};
		Node(int start, int end, int link) : start(start), end(end), link(link){};
		int EdgeLength(const int &pos) const
		{
			return std::min(end, pos + 1) - start;
		}
		bool IsLeaf() const
		{
			return end == oo;
		}
	};

	class Satellite
	{
	  public:
		std::string data;
		int keyLen;
		int keyPos;

	  public:
		Satellite() : data(), keyLen(0), keyPos(0){};
		Satellite(std::string data, int keyLen, int keyPos) : data(data), keyLen(keyLen), keyPos(keyPos){};
	};

  public:
	class KeyValue
	{
	  public:
		std::string key;
		std::string value;

	  public:
		KeyValue(const Satellite &sat, const std::u32string &text)
		{
			value = sat.data;
			key = una::norm::to_nfc_utf8(
				una::utf32to8(std::u32string_view(text.begin() + sat.keyPos, text.begin() + sat.keyPos + sat.keyLen)));
		}
	};

  private:
	std::u32string text;
	std::vector<Node> tree;
	std::vector<Satellite> satellite;

	int root, needSL, remainder;

	// The active point, represented by a triple.
	int activeNode, activeEdge, activeLength;

  private:
	int NewNode(int start, int end = oo, int satelliteLink = 0)
	{
		tree.emplace_back(start, end, -satelliteLink);
		return tree.size() - 1;
	}

	char32_t ActiveEdge()
	{
		return text[activeEdge];
	}

	void AddLink(int node)
	{
		if (needSL > 0)
		{
			tree[needSL].link = node;
		}
		needSL = node;
	}

	bool WalkDown(int node)
	{
		if (activeLength >= tree[node].EdgeLength(text.size() - 1))
		{
			activeEdge += tree[node].EdgeLength(text.size() - 1);
			activeLength -= tree[node].EdgeLength(text.size() - 1);
			activeNode = node;
			return true;
		}
		return false;
	}

	void Extend(char32_t c, int satelliteLink)
	{
		text.push_back(c);
		needSL = 0;
		remainder++;
		while (remainder > 0)
		{
			if (activeLength == 0)
			{
				activeEdge = text.size() - 1;
			}
			if (tree[activeNode].next.Find(ActiveEdge()) == tree[activeNode].next.End())
			{
				int leaf = NewNode(text.size() - 1, oo, satelliteLink);
				tree[activeNode].next[ActiveEdge()] = leaf;
				AddLink(activeNode); // rule 2
			}
			else
			{
				int next = tree[activeNode].next[ActiveEdge()];
				if (WalkDown(next)) // observation 2
				{
					continue;
				}
				if (text[tree[next].start + activeLength] == c) // observation 1
				{
					activeLength++;
					AddLink(activeNode); // observation 3
					break;
				}
				int split = NewNode(tree[next].start, tree[next].start + activeLength);
				tree[activeNode].next[ActiveEdge()] = split;
				int leaf = NewNode(text.size() - 1, oo, satelliteLink);
				tree[split].next[c] = leaf;
				tree[next].start += activeLength;
				tree[split].next[text[tree[next].start]] = next;
				AddLink(split); // rule 2
			}
			remainder--;
			if (activeNode == root && activeLength > 0) // rule 1
			{
				activeLength--;
				activeEdge = text.size() - remainder;
			}
			else
			{
				activeNode = tree[activeNode].link > 0 ? tree[activeNode].link : root; // rule 3
			}
		}
	}

	void List(std::u32string &u32str, int node) const
	{
		if (tree[node].end == oo)
		{
			for (int i = tree[node].start; i < text.size(); ++i)
			{
				u32str.push_back(text[i]);
			}
			std::cout << una::norm::to_nfc_utf8(una::utf32to8(u32str)) << '\n';
			return;
		}
		for (int i = tree[node].start; i < tree[node].end; ++i)
		{
			u32str.push_back(text[i]);
		}
		for (auto next = tree[node].next.Begin(); next != tree[node].next.End(); ++next)
		{
			auto orig = u32str.size();
			List(u32str, *next.second);
			u32str.resize(orig);
		}
	}

  public:
	SuffixTreeRB()
	{
		needSL = 0;
		remainder = 0, activeNode = 0, activeEdge = 0, activeLength = 0;
		root = activeNode = NewNode(-1, -1);
	}

	void Add(std::string key, std::string value)
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return;
		}
		std::u32string u32str = una::utf8to32u(una::norm::to_nfd_utf8(key));
		satellite.emplace_back(value, u32str.size(), text.size());
		for (const char32_t &c : u32str)
		{
			Extend(c, satellite.size() - 1);
		}
		Extend(delim, satellite.size() - 1);
	}

	void List() const
	{
		std::u32string u32str;
		List(u32str, 0);
	}

	// Save the data into files
	bool Serialize(std::filesystem::path directory, std::string filename) const
	{
		if (!std::filesystem::exists(directory))
		{
			std::filesystem::create_directories(directory);
		}
		else if (!std::filesystem::is_directory(directory))
		{
			return false;
		}

		std::ofstream textFileOut(directory / (const char8_t *)(filename + ".text").c_str(),
								  std::ios::out | std::ios::binary);
		std::ofstream treeFileOut(directory / (const char8_t *)(filename + ".tree").c_str(),
								  std::ios::out | std::ios::binary);
		std::ofstream sateFileOut(directory / (const char8_t *)(filename + ".sate").c_str(),
								  std::ios::out | std::ios::binary);
		if (!textFileOut || !treeFileOut || !sateFileOut)
		{
			return false;
		}

		int textSize = text.size();
		textFileOut.write((const char *)&textSize, sizeof(textSize));
		textFileOut.write((const char *)text.c_str(), textSize * sizeof(char32_t));

		int satCnt = satellite.size();
		sateFileOut.write((const char *)&satCnt, sizeof(satCnt));
		for (const auto &sat : satellite)
		{
			int dataSize = sat.data.size();
			sateFileOut.write((const char *)&dataSize, sizeof(dataSize));
			sateFileOut.write((const char *)sat.data.c_str(), dataSize * sizeof(char));
			sateFileOut.write((const char *)&sat.keyLen, sizeof(sat.keyLen));
			sateFileOut.write((const char *)&sat.keyPos, sizeof(sat.keyPos));
		}

		// This maybe is not needed
		treeFileOut.write((const char *)&root, sizeof(root));
		treeFileOut.write((const char *)&needSL, sizeof(needSL));
		treeFileOut.write((const char *)&remainder, sizeof(remainder));
		//

		treeFileOut.write((const char *)&activeNode, sizeof(activeNode));
		treeFileOut.write((const char *)&activeEdge, sizeof(activeEdge));
		treeFileOut.write((const char *)&activeLength, sizeof(activeLength));

		int treeSize = tree.size();
		treeFileOut.write((const char *)&treeSize, sizeof(treeSize));
		for (const auto &node : tree)
		{
			treeFileOut.write((const char *)&node.start, sizeof(node.start));
			treeFileOut.write((const char *)&node.end, sizeof(node.end));
			treeFileOut.write((const char *)&node.link, sizeof(node.link));
			int mapSize = node.next.Size();
			treeFileOut.write((const char *)&mapSize, sizeof(mapSize));
			for (auto next = node.next.Begin(); next != node.next.End(); ++next)
			{
				treeFileOut.write((const char *)next.first, sizeof(*next.first));
				treeFileOut.write((const char *)next.second, sizeof(*next.second));
			}
		}

		return true;
	}

	// Load the data from files
	bool Deserialize(std::filesystem::path directory, std::string filename)
	{
		if (!std::filesystem::is_directory(directory))
		{
			return false;
		}

		std::ifstream textFileIn(directory / (const char8_t *)(filename + ".text").c_str(),
								 std::ios::in | std::ios::binary);
		std::ifstream treeFileIn(directory / (const char8_t *)(filename + ".tree").c_str(),
								 std::ios::in | std::ios::binary);
		std::ifstream sateFileIn(directory / (const char8_t *)(filename + ".sate").c_str(),
								 std::ios::in | std::ios::binary);
		if (!textFileIn || !treeFileIn || !sateFileIn)
		{
			return false;
		}

		int textSize;
		textFileIn.read((char *)&textSize, sizeof(textSize));
		text.resize(textSize);
		textFileIn.read((char *)text.data(), textSize * sizeof(char32_t));

		int satCnt;
		sateFileIn.read((char *)&satCnt, sizeof(satCnt));
		satellite.resize(satCnt);
		for (auto &sat : satellite)
		{
			int dataSize;
			sateFileIn.read((char *)&dataSize, sizeof(dataSize));
			sat.data.resize(dataSize);
			sateFileIn.read((char *)sat.data.data(), dataSize * sizeof(char));
			sateFileIn.read((char *)&sat.keyLen, sizeof(sat.keyLen));
			sateFileIn.read((char *)&sat.keyPos, sizeof(sat.keyPos));
		}

		// This maybe is not needed
		treeFileIn.read((char *)&root, sizeof(root));
		treeFileIn.read((char *)&needSL, sizeof(needSL));
		treeFileIn.read((char *)&remainder, sizeof(remainder));
		//

		treeFileIn.read((char *)&activeNode, sizeof(activeNode));
		treeFileIn.read((char *)&activeEdge, sizeof(activeEdge));
		treeFileIn.read((char *)&activeLength, sizeof(activeLength));

		int treeSize = tree.size();
		treeFileIn.read((char *)&treeSize, sizeof(treeSize));
		tree.resize(treeSize);
		for (auto &node : tree)
		{
			treeFileIn.read((char *)&node.start, sizeof(node.start));
			treeFileIn.read((char *)&node.end, sizeof(node.end));
			treeFileIn.read((char *)&node.link, sizeof(node.link));
			int mapSize = node.next.Size();
			treeFileIn.read((char *)&mapSize, sizeof(mapSize));
			for (int i = 0; i < mapSize; ++i)
			{
				char32_t c;
				int child;
				treeFileIn.read((char *)&c, sizeof(c));
				treeFileIn.read((char *)&child, sizeof(child));
				node.next[c] = child;
			}
		}

		return true;
	}

	size_t Count() const
	{
		return text.size();
	}

	size_t Size() const
	{
		return tree.size();
	}

	bool Contain(const std::u32string_view &u32strv) const
	{
		int curNode = 0, curLength = 0;
		for (int i = 0; i < u32strv.size(); ++i)
		{
			if (curLength == tree[curNode].EdgeLength(text.size() - 1))
			{
				auto child = tree[curNode].next.Find(u32strv[i]);
				if (child == tree[curNode].next.End())
				{
					return false;
				}
				curNode = *child.second;
				curLength = 1;
			}
			else if (u32strv[i] == text[tree[curNode].start + curLength])
			{
				curLength++;
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	std::vector<KeyValue> Find(std::string key)
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return {};
		}
		std::u32string u32key = una::utf8to32u(una::norm::to_nfd_utf8(key));
		int curNode = 0, curLength = 0;
		for (int i = 0; i < u32key.size(); ++i)
		{
			if (curLength == tree[curNode].EdgeLength(text.size() - 1))
			{
				auto child = tree[curNode].next.Find(u32key[i]);
				if (child == tree[curNode].next.End())
				{
					return {};
				}
				curNode = *child.second;
				curLength = 1;
			}
			else if (u32key[i] == text[tree[curNode].start + curLength])
			{
				curLength++;
			}
			else
			{
				return {};
			}
		}
		std::vector<KeyValue> keyValue;
		std::vector<int> collected;
		Collect(curNode, keyValue, collected);
		for (const auto &i : collected)
		{
			satellite[i].keyPos = -(satellite[i].keyPos + 1);
		}
		return keyValue;
	}

	void Collect(int curNode, std::vector<KeyValue> &keyValue, std::vector<int> &collected)
	{
		if (tree[curNode].IsLeaf())
		{
			int i = -tree[curNode].link;
			if (satellite[i].keyPos >= 0)
			{
				keyValue.emplace_back(satellite[i], text);
				satellite[i].keyPos = -satellite[i].keyPos - 1; // minus 1 to ensure marked value is negative
				collected.push_back(i);
			}
			return;
		}
		for (auto child = tree[curNode].next.Begin(); child != tree[curNode].next.End(); ++child)
		{
			Collect(*child.second, keyValue, collected);
		}
	}

	bool Validate() const
	{
		std::u32string_view u32strv(text.begin(), text.end());
		for (int i = 0; i < text.size(); ++i)
		{
			if (!Contain(u32strv))
			{
				return false;
			}
			u32strv.remove_prefix(1);
		}
		return true;
	}
};
