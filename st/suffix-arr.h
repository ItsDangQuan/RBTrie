#pragma once

#include "red_black_tree.h"
#include "uni_algo/all.h"
#include <iostream>
#include <string>
#include <vector>

namespace old
{
class SuffixArray
{
  private:
	struct Rank
	{
		int leftRank;
		int rightRank;
		int index;

		friend bool operator<(const Rank &lhs, const Rank &rhs)
		{
			return lhs.leftRank < rhs.leftRank || lhs.leftRank == rhs.leftRank && lhs.rightRank < rhs.rightRank;
		}
	};

  private:
	std::u32string str;
	std::vector<int> sa;
	RBTree<int, std::string> sate;

  private:
	void MakeRanks(std::vector<Rank> &substrRank, std::vector<int> &rank)
	{
		int r = 1;
		rank[substrRank[0].index] = r;
		for (int i = 1; i < str.size(); ++i)
		{
			if (substrRank[i].leftRank != substrRank[i - 1].leftRank ||
				substrRank[i].rightRank != substrRank[i - 1].rightRank)
			{
				r = r + 1;
			}
			rank[substrRank[i].index] = r;
		}
	}
	void SortLeft(std::vector<Rank> &substrRank, int b)
	{
		int max = 0;
		for (const Rank &elem : substrRank)
		{
			if (max < elem.leftRank)
			{
				max = elem.leftRank;
			}
		}
		std::function<void(int)> countSort = [&substrRank, &b](int exp) {
			std::vector<Rank> output(substrRank.size());
			std::vector<int> count(b, 0);
			for (int i = 0; i < substrRank.size(); ++i)
			{
				count[(substrRank[i].leftRank / exp) % b]++;
			}
			for (int i = 1; i < count.size(); ++i)
			{
				count[i] += count[i - 1];
			}
			for (int i = substrRank.size() - 1; i >= 0; --i)
			{
				output[count[(substrRank[i].leftRank / exp) % b] - 1] = substrRank[i];
				count[(substrRank[i].leftRank / exp) % b]--;
			}
			substrRank = output;
		};
		for (int exp = 1; max / exp > 0; exp *= b)
		{
			countSort(exp);
		}
	}
	void SortRight(std::vector<Rank> &substrRank, int b)
	{
		int max = 0;
		for (const Rank &elem : substrRank)
		{
			if (max < elem.rightRank)
			{
				max = elem.rightRank;
			}
		}
		std::function<void(int)> countSort = [&substrRank, &b](int exp) {
			std::vector<Rank> output(substrRank.size());
			std::vector<int> count(b, 0);
			for (int i = 0; i < substrRank.size(); ++i)
			{
				count[(substrRank[i].rightRank / exp) % b]++;
			}
			for (int i = 1; i < count.size(); ++i)
			{
				count[i] += count[i - 1];
			}
			for (int i = substrRank.size() - 1; i >= 0; --i)
			{
				output[count[(substrRank[i].rightRank / exp) % b] - 1] = substrRank[i];
				count[(substrRank[i].rightRank / exp) % b]--;
			}
			substrRank = output;
		};
		for (int exp = 1; max / exp > 0; exp *= b)
		{
			countSort(exp);
		}
	}
	void Sort(std::vector<Rank> &substrRank)
	{
		int b = 10000;
		SortRight(substrRank, b);
		SortLeft(substrRank, b);
	}
	void Collect(int lower, int upper, int size, std::vector<std::string> &collection)
	{
		std::vector<int> collected;
		for (int i = lower; i < upper; ++i)
		{
			int j = sa[i] + size;
			while (str[j] != U'\0')
			{
				j = j + 1;
			}
			auto iter = sate.Find(j);
			if (iter != sate.End() && (*iter.second).back() != 1)
			{
				collected.push_back(j);
				collection.push_back(*iter.second);
				(*iter.second).push_back(1);
			}
		}
		for (const auto j : collected)
		{
			sate[j].pop_back();
		}
	}

  public:
	void Build()
	{
		std::vector<Rank> substrRank(str.size());
		std::vector<int> rank(str.size());
		sa.resize(str.size());
		for (int i = 0; i < str.size(); ++i)
		{
			substrRank[i].leftRank = static_cast<int>(str[i]);
			if (i < str.size() - 1)
			{
				substrRank[i].rightRank = static_cast<int>(str[i + 1]);
			}
			else
			{
				substrRank[i].rightRank = 0;
			}
			substrRank[i].index = i;
		}
		Sort(substrRank);
		for (int l = 2; l < str.size(); l = l * 2)
		{
			MakeRanks(substrRank, rank);
			for (int i = 0; i < str.size(); ++i)
			{
				substrRank[i].leftRank = rank[i];
				if (i + l < str.size())
				{
					substrRank[i].rightRank = rank[i + l];
				}
				else
				{
					substrRank[i].rightRank = 0;
				}
				substrRank[i].index = i;
			}
			Sort(substrRank);
		}
		for (int i = 0; i < str.size(); ++i)
		{
			sa[i] = substrRank[i].index;
		}
	}
	void Add(std::string key, std::string value)
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return;
		}
		str += una::utf8to32u(una::norm::to_nfd_utf8(key));
		sate[str.size()] = value;
		str += U'\0';
	}
	std::vector<std::string> Find(std::string key)
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return {};
		}
		int lower = 0;
		int upper = sa.size();
		std::u32string u32str = una::utf8to32u(una::norm::to_nfd_utf8(key));
		for (int i = 0; i < u32str.size(); ++i)
		{
			lower = [this, &i, &u32str](int l, int h) -> int {
				int ans = h--;
				while (l <= h)
				{
					int m = l + (h - l) / 2;
					if (str[sa[m] + i] >= u32str[i])
					{
						ans = m;
						h = m - 1;
					}
					else
					{
						l = m + 1;
					}
				}
				return ans;
			}(lower, upper);
			if (lower >= sa.size() || str[sa[lower] + i] != u32str[i])
			{
				return {};
			}
			upper = [this, &i, &u32str](int l, int h) -> int {
				int ans = h--;
				while (l <= h)
				{
					int m = l + (h - l) / 2;
					if (str[sa[m] + i] > u32str[i])
					{
						ans = m;
						h = m - 1;
					}
					else
					{
						l = m + 1;
					}
				}
				return ans;
			}(lower, upper);
			if (lower > upper)
			{
				return {};
			}
		}
		std::vector<std::string> collection;
		Collect(lower, upper, u32str.size(), collection);
		return collection;
	}
	void Print()
	{
		for (int i : sa)
		{
			std::cout << una::norm::to_nfc_utf8(una::utf32to8(std::u32string_view(str.begin() + i, str.end()))) << '\n';
		}
	}
	void Validate()
	{
		std::u32string_view prev(str.begin() + sa[0], str.end());
		for (int i = 1; i < sa.size(); ++i)
		{
			std::u32string_view cur(str.begin() + sa[i], str.end());
			if (cur < prev)
			{
				std::cerr << "Oh no\n";
				throw;
			}
			prev = cur;
		}
	}
	int Size()
	{
		return str.size();
	}
};
} // namespace old

class SuffixArray
{
};
