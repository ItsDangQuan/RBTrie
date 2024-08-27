﻿#include "suffix_tree.h"
#include "suffix-tree.h"
#include <chrono>
#include <fstream>
#include <Windows.h>

// void StressBuildTest()
//{
//	SuffixTree st;
//
//	std::ifstream fin("data/ee.csv");
//
//	auto start = std::chrono::steady_clock::now();
//	while (fin)
//	{
//		std::string buf;
//		std::getline(fin, buf);
//		st.Add(buf);
//	}
//	auto finish = std::chrono::steady_clock::now();
//	auto elapsed = (finish - start).count();
//	std::cout << "Build:\n";
//	std::cout << st.Count() << " suffices\n";
//	std::cout << st.Size() << " nodes\n";
//	std::cout << elapsed << " ns\n";
//	std::u32string test = una::utf8to32u(una::norm::to_nfd_utf8((const char *)u8"Validate"));
//	std::cout << (st.Contain(test) ? "Found: Validate\n" : "Not found: Validate\n");
// }
//
// void StressSaveTest()
//{
//	SuffixTree st;
//	std::ifstream fin("data/ee.csv");
//	while (fin)
//	{
//		std::string buf;
//		std::getline(fin, buf);
//		st.Add(buf);
//	}
//
//	auto start = std::chrono::steady_clock::now();
//	st.Serialize((const char *)u8"./seri", (const char *)u8"st");
//	auto finish = std::chrono::steady_clock::now();
//	auto elapsed = (finish - start).count();
//	std::cout << "Save:\n";
//	std::cout << st.Count() << " suffices\n";
//	std::cout << st.Size() << " nodes\n";
//	std::cout << elapsed << " ns\n";
//	std::u32string test = una::utf8to32u(una::norm::to_nfd_utf8((const char *)u8"Validate"));
//	std::cout << (st.Contain(test) ? "Found: Validate\n" : "Not found: Validate\n");
// }
//
// void StressLoadTest()
//{
//	SuffixTree st;
//	auto start = std::chrono::steady_clock::now();
//	st.Deserialize((const char *)u8"./seri", (const char *)u8"st");
//	auto finish = std::chrono::steady_clock::now();
//	auto elapsed = (finish - start).count();
//	std::cout << "Load:\n";
//	std::cout << st.Count() << " suffices\n";
//	std::cout << st.Size() << " nodes\n";
//	std::cout << elapsed << " ns\n";
//	std::u32string test = una::utf8to32u(una::norm::to_nfd_utf8((const char *)u8"Validate"));
//	std::cout << (st.Contain(test) ? "Found: Validate\n" : "Not found: Validate\n");
// }

void SmallTest()
{
	SuffixTreeRB st;
	st.Add((const char *)u8"thử nghiệm", (const char *)u8"experiment");
	st.Add((const char *)u8"cây hậu tố", (const char *)u8"suffix tree");
	st.Add((const char *)u8"cây nhị phân", (const char *)u8"binary tree");
	st.Add((const char *)u8"nhi đồng", (const char *)u8"children");
	st.Add((const char *)u8"abcababd", (const char *)u8"random bulllshit go");
	st.Add((const char *)u8"abc", (const char *)u8"random bulllshit go the second");
	st.Add((const char *)u8"- (máy tính) số hạng thứ hai", (const char *)u8"augend");
	st.Add((const char *)u8"sự gần đúng, phép xấp xỉ, cách tiếp cận; radial a. gl. ghép xấp xỉ theo tia ",
		   (const char *)u8"approach");
	std::cout << "Build validity:\n";

	std::u32string test = una::utf8to32u(una::norm::to_nfd_utf8((const char *)u8"thử"));
	std::cout << (st.Contain(test) ? (const char *)u8"Found: thử\n" : (const char *)u8"Not found: thử\n");

	std::string query = (const char *)u8"nghiệm";
	std::vector<SuffixTreeRB::KeyValue> kv = st.Find(query);

	std::cout << "Search result(s) for " << query << ":\n";
	for (const auto &kvPair : kv)
	{
		std::cout << "key: " << kvPair.key << "; value: " << kvPair.value << '\n';
	}

	std::cout << "All suffices:\n";
	st.List();

	std::cout << (st.Validate() ? "Valid suffix tree\n" : "Invalid suffix tree\n");

	st.Serialize((const char *)u8"./seri", (const char *)u8"small-strb");
}

void SmallLoadTest()
{
	SuffixTreeRB st;
	st.Deserialize((const char *)u8"./seri", (const char *)u8"small-strb");
	std::cout << "Load validity:\n";

	std::u32string test = una::utf8to32u(una::norm::to_nfd_utf8((const char *)u8"thử"));
	std::cout << (st.Contain(test) ? (const char *)u8"Found: thử\n" : (const char *)u8"Not found: thử\n");

	std::string query = (const char *)u8"nhi";
	std::vector<SuffixTreeRB::KeyValue> kv = st.Find(query);

	std::cout << "Search result(s) for " << query << ":\n";
	for (const auto &kvPair : kv)
	{
		std::cout << "key: " << kvPair.key << "; value: " << kvPair.value << '\n';
	}

	std::cout << "All suffices:\n";
	st.List();

	std::cout << (st.Validate() ? "Valid suffix tree\n" : "Invalid suffix tree\n");
}

#include "suffix-arr.h"
#include <chrono>
#include <fstream>
#include <random>
#include <string>

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	old::SuffixArray sa;

	std::fstream fin("data/anh_viet.txt");
	std::string line, word, def;
	while (std::getline(fin, line))
	{
		if (line[0] == '@')
		{
			if (!word.empty())
			{
				sa.Add(def, word);
			}
			line.erase(0, 1);
			word = line;
			def.clear();
		}
		else if (line[0] == '-')
		{
			line.erase(0, 1);
			def += line + '\n';
		}
	}

	auto start = std::chrono::steady_clock::now();
	sa.Build();
	auto end = std::chrono::steady_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';

	for (char c = 'a'; c <= 'z'; ++c)
	{
		start = std::chrono::steady_clock::now();
		std::vector<std::string> collection = sa.Find(std::string() + c);
		end = std::chrono::steady_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
	}

	return 0;
}
