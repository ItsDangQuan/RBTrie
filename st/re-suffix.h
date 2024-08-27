#pragma once
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <map> // red black tree
#include <string>
#include <uni_algo/all.h>
#include <vector>

class ReSuffix
{
  private:
	static const int oo = std::numeric_limits<int>::max();
	char32_t delim = std::numeric_limits<char32_t>::max();

	class Node
	{
	  public:
		int start;
		int end;

		int link;

		int dist; // distance to delim

		std::map<char32_t, int> next;

	  public:
		int EdgeLength(const int &pos) const
		{
			return std::min(end, pos) - start;
		}
		bool IsLeaf() const
		{
			return end == oo;
		}
	};

	class Sate
	{
	  public:
		std::string data;
		int keyLen;
		int keyPos;

	  public:
		Sate() : data(), keyLen(0), keyPos(0){};
		Sate(std::string data, int keyLen, int keyPos) : data(data), keyLen(keyLen), keyPos(keyPos){};
	};

  private:
	std::u32string text;
	std::vector<Node> tree;
	std::vector<Sate> satellite;

	int needSL, remainder;

	int aN, aE, aL;

  public:
	int NewNode
};