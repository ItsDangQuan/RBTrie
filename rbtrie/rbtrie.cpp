#include "rbtrieRB.h"
#include "red_black_tree.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string_view>

#include <windows.h>

int main()
{
	SetConsoleOutputCP(CP_UTF8);

	RBTrieRB trie;
	trie.Insert((const char *)u8"thử nghiệm", (const char *)u8"experiment");
	trie.Insert((const char *)u8"cây hậu tố", (const char *)u8"suffix tree");
	trie.Insert((const char *)u8"cây nhị phân", (const char *)u8"binary tree");
	trie.Insert((const char *)u8"nhi đồng", (const char *)u8"children");
	trie.Insert((const char *)u8"abcababd", (const char *)u8"random bulllshit go");
	trie.Insert((const char *)u8"abc", (const char *)u8"random bulllshit go the second");

	trie.Remove((const char *)u8"abc");

	for (const auto &str : trie.PrefixSearch((const char *)u8"a"))
	{
		std::cout << str << '\n';
	}

	return 0;
}
