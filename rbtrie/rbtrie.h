#include <string>
#include <uni_algo/all.h>
#include <vector>

/* Adapt red - black balancing rule to ternary search tree:
 * 1. Every node is either red or black.
 * 2. Root is black.
 * 3. Every subroot is black.
 * 4. Every leaf (NIL) is black.
 * 5. If a node is red, then all of its children are black.
 * 6. For each node, all simple branching paths (paths that contains no subroot) from the node to descendant leaves
 * contain the same number of black nodes.
 */
class RBTrie
{
  private:
	struct Node
	{
		enum Color : char
		{
			RED = 0,
			BLACK
		} color;
		bool subroot;

		bool end;
		char32_t codepoint;
		std::string value;

		Node *lo;
		Node *eq;
		Node *hi;
		Node *pa;
	};

  private:
	Node *root;
	Node *const nil;

  private:
	// it is neccessary to have an iterative method for postorder traversal,
	// as recursive method can't handle long string.
	Node *PostOrderBegin(Node *node) const
	{
		while (node != nil && (node->lo != nil || node->eq != nil || node->hi != nil))
		{
			node = node->lo != nil ? node->lo : node->eq != nil ? node->eq : node->hi;
		}
		return node;
	}
	Node *PostOrderSuccessor(Node *node) const
	{
		Node *pa = node->pa;
		if (pa == nil)
		{
			return nil;
		}
		if (pa->hi == node)
		{
			return pa;
		}
		if (pa->eq == node)
		{
			if (pa->hi != nil)
			{
				return PostOrderBegin(pa->hi);
			}
			return pa;
		}
		if (pa->eq != nil)
		{
			return PostOrderBegin(pa->eq);
		}
		if (pa->hi != nil)
		{
			return PostOrderBegin(pa->hi);
		}
		return pa;
	}
	// to collect all strings in a substree in sorted order, we have to do inorder traversal
	Node *InOrderBegin(Node *node, std::u32string &str) const
	{
		while (node != nil && node->lo != nil)
		{
			node = node->lo;
		}
		if (node != nil)
		{
			str += node->codepoint;
		}
		return node;
	}
	Node *InOrderSuccessor(Node *node, std::u32string &str) const
	{
		if (node->eq != nil)
		{
			return InOrderBegin(node->eq, str);
		}
		if (node->hi != nil)
		{
			return InOrderBegin(node->hi, str);
		}
		while (node != nil)
		{
			if (node->pa->lo == node)
			{
				str.back() = node->pa->codepoint;
				return node->pa;
			}
			if (node->pa->eq == node)
			{
				str.pop_back();
				if (node->pa->hi != nil)
				{
					str.pop_back();
					return InOrderBegin(node->pa->hi, str);
				}
			}
			node = node->pa;
		}
		str.clear();
		return nil;
	}
	// to get the inorder successor for Remove method
	Node *Minimum(Node *node) const
	{
		while (node->lo != nil)
		{
			node = node->lo;
		}
		return node;
	}
	void Deallocate(Node *subtree)
	{
		if (subtree == nil)
		{
			return;
		}
		Node *node = PostOrderBegin(subtree);
		while (node != subtree)
		{
			Node *next = PostOrderSuccessor(node);
			delete node;
			node = next;
		}
		delete node;
	}
	// aka left rotate
	// root can be changed, so we return the new root
	Node *RotateWithHi(Node *rt, Node *node) const
	{
		Node *kid = node->hi;
		node->hi = kid->lo;
		if (kid->lo != nil)
		{
			kid->lo->pa = node;
		}
		kid->pa = node->pa;
		if (node->subroot)
		{
			rt = kid;
			kid->subroot = true;
			node->subroot = false;
		}
		else if (node == node->pa->lo)
		{
			node->pa->lo = kid;
		}
		else
		{
			node->pa->hi = kid;
		}
		kid->lo = node;
		node->pa = kid;
		return rt;
	}
	// aka right rotate
	// root can be changed, so we return the new root
	Node *RotateWithLo(Node *rt, Node *node) const
	{
		Node *kid = node->lo;
		node->lo = kid->hi;
		if (kid->hi != nil)
		{
			kid->hi->pa = node;
		}
		kid->pa = node->pa;
		if (node->subroot)
		{
			rt = kid;
			kid->subroot = true;
			node->subroot = false;
		}
		else if (node == node->pa->lo)
		{
			node->pa->lo = kid;
		}
		else
		{
			node->pa->hi = kid;
		}
		kid->hi = node;
		node->pa = kid;
		return rt;
	}
	// this algorithm is taken from clrs book
	// assume subtree is detached
	// root can be changed, so we return the new root
	Node *Transplant(Node *rt, Node *dest, Node *node) const
	{
		if (dest->pa == nil)
		{
			rt = node;
			node->subroot = true;
		}
		else if (dest == dest->pa->lo)
		{
			dest->pa->lo = node;
		}
		else
		{
			dest->pa->hi = node;
		}
		node->pa = dest->pa;
		return rt;
	}
	// this algorithm is taken from clrs book
	// assume the subtree is detached
	// root can be changed, so we return the new root
	Node *InsertFixup(Node *rt, Node *node) const
	{
		while (node->pa->color == Node::RED)
		{
			if (node->pa == node->pa->pa->lo)
			{
				Node *uncle = node->pa->pa->hi;
				if (uncle->color == Node::RED)
				{
					node->pa->color = Node::BLACK;
					uncle->color = Node::BLACK;
					node->pa->pa->color = Node::RED;
					node = node->pa->pa;
				}
				else
				{
					if (node == node->pa->hi)
					{
						node = node->pa;
						rt = RotateWithHi(rt, node);
					}
					node->pa->color = Node::BLACK;
					node->pa->pa->color = Node::RED;
					rt = RotateWithLo(rt, node->pa->pa);
				}
			}
			else
			{
				Node *uncle = node->pa->pa->lo;
				if (uncle->color == Node::RED)
				{
					node->pa->color = Node::BLACK;
					uncle->color = Node::BLACK;
					node->pa->pa->color = Node::RED;
					node = node->pa->pa;
				}
				else
				{
					if (node == node->pa->lo)
					{
						node = node->pa;
						rt = RotateWithLo(rt, node);
					}
					node->pa->color = Node::BLACK;
					node->pa->pa->color = Node::RED;
					rt = RotateWithHi(rt, node->pa->pa);
				}
			}
		}
		rt->color = Node::BLACK;
		return rt;
	}
	// this algorithm is taken from clrs book
	// assume subtree is detached
	// root can be changed, so we return the new root
	Node *RemoveFixup(Node *rt, Node *node) const
	{
		while (node != rt && node->color == Node::BLACK)
		{
			if (node == node->pa->lo)
			{
				Node *sibling = node->pa->hi;
				if (sibling->color == Node::RED)
				{
					sibling->color = Node::BLACK;
					node->pa->color = Node::RED;
					rt = RotateWithHi(rt, node->pa);
					sibling = node->pa->hi;
				}
				if (sibling->lo->color == Node::BLACK && sibling->hi->color == Node::BLACK)
				{
					sibling->color = Node::RED;
					node = node->pa;
				}
				else
				{
					if (sibling->hi->color == Node::BLACK)
					{
						sibling->lo->color = Node::BLACK;
						sibling->color = Node::RED;
						rt = RotateWithLo(rt, sibling);
						sibling = node->pa->hi;
					}
					sibling->color = node->pa->color;
					node->pa->color = Node::BLACK;
					sibling->hi->color = Node::BLACK;
					rt = RotateWithHi(rt, node->pa);
					node = rt;
				}
			}
			else
			{
				Node *sibling = node->pa->lo;
				if (sibling->color == Node::RED)
				{
					sibling->color = Node::BLACK;
					node->pa->color = Node::RED;
					rt = RotateWithLo(rt, node->pa);
					sibling = node->pa->lo;
				}
				if (sibling->lo->color == Node::BLACK && sibling->hi->color == Node::BLACK)
				{
					sibling->color = Node::RED;
					node = node->pa;
				}
				else
				{
					if (sibling->lo->color == Node::BLACK)
					{
						sibling->hi->color = Node::BLACK;
						sibling->color = Node::RED;
						rt = RotateWithHi(rt, sibling);
						sibling = node->pa->lo;
					}
					sibling->color = node->pa->color;
					node->pa->color = Node::BLACK;
					sibling->lo->color = Node::BLACK;
					rt = RotateWithLo(rt, node->pa);
					node = rt;
				}
			}
		}
		node->color = Node::BLACK;
		return rt;
	}
	// this algorithm is taken from clrs book
	// assume subtree is detached
	// root can be changed, so we return the new root
	Node *RemoveUpdate(Node *rt, Node *node) const
	{
		Node *del = node;
		Node::Color del_original_color = del->color;
		Node *violation;
		if (node->lo == nil)
		{
			violation = node->hi;
			rt = Transplant(rt, node, node->hi);
		}
		else if (node->hi == nil)
		{
			violation = node->lo;
			rt = Transplant(rt, node, node->lo);
		}
		else
		{
			del = Minimum(node->hi);
			del_original_color = del->color;
			violation = del->hi;
			if (del != node->hi)
			{
				rt = Transplant(rt, del, del->hi);
				del->hi = node->hi;
				del->hi->pa = del;
			}
			else
			{
				violation->pa = del;
			}
			rt = Transplant(rt, node, del);
			del->lo = node->lo;
			del->lo->pa = del;
			del->color = node->color;
		}
		delete node;
		if (del_original_color == Node::BLACK)
		{
			rt = RemoveFixup(rt, violation);
		}
		nil->pa = nil;
		return rt;
	}
	// deattach the subtree to update it, then reattach
	// fixup function need the subtree to be detached
	void InsertUpdate(Node *rt, Node *node)
	{
		if (rt == root)
		{
			root = InsertFixup(root, node);
			return;
		}
		Node *pa = rt->pa;
		rt->pa = nil;
		if (pa->lo == rt)
		{

			pa->lo = InsertFixup(pa->lo, node);
			pa->lo->pa = pa;
		}
		else if (pa->hi == rt)
		{
			pa->hi = InsertFixup(pa->hi, node);
			pa->hi->pa = pa;
		}
		else
		{
			pa->eq = InsertFixup(pa->eq, node);
			pa->eq->pa = pa;
		}
	}
	Node *AddTail(Node *node, std::u32string &str, int pos, std::string &value)
	{
		while (pos < str.size())
		{
			node->eq = new Node{Node::BLACK, true, false, str[pos], "", nil, nil, nil, node};
			node = node->eq;
			pos += 1;
		}
		node->end = true;
		node->value = value;
		return node;
	}
	// iterative method, as recursive can't handle long string
	long long Count(Node *node) const
	{
		long long cnt = 0;
		Node *iter = PostOrderBegin(node);
		while (iter != node)
		{
			cnt += 1;
			iter = PostOrderSuccessor(iter);
		}
		cnt += (node != nil);
		return cnt;
	}
	// this is just to test the correctness of the tree, will be Removed
	// int Validate(Node *node) const
	//{
	//	if (node == nil)
	//	{
	//		return 0;
	//	}
	//	if (node->pa == nil || node->pa->eq == node)
	//	{
	//		if (!node->subroot)
	//		{
	//			throw "Subroot not marked";
	//		}
	//	}
	//	if (node->lo != nil && node->lo->pa != node)
	//	{
	//		throw "Lo kid wrong parent";
	//	}
	//	if (node->eq != nil && node->eq->pa != node)
	//	{
	//		throw "Eq kid wrong parent";
	//	}
	//	if (node->hi != nil && node->hi->pa != node)
	//	{
	//		throw "Hi kid wrong parent";
	//	}
	//	if (node->subroot && node->color != Node::BLACK)
	//	{
	//		throw "Subroot wrong color";
	//	}
	//	else if (!node->subroot && node->color == Node::RED)
	//	{
	//		if (!node->lo->color == Node::BLACK)
	//		{
	//			throw "Lo kid wrong color";
	//		}
	//		else if (!node->hi->color == Node::BLACK)
	//		{
	//			throw "Hi kid wrong color";
	//		}
	//	}
	//	int lobh = Validate(node->lo);
	//	int hibh = Validate(node->hi);
	//	if (lobh != hibh)
	//	{
	//		throw "Black height differ";
	//	}
	//	Validate(node->eq);
	//	return lobh + (node->color == Node::BLACK);
	//}
	void Collect(Node *node, std::u32string &str, std::vector<std::string> &collection)
	{
		Node *pa = node->pa;
		node->pa = nil; // detach subtree for traversal
		Node *cur = InOrderBegin(node, str);
		while (cur != nil)
		{
			if (cur->end)
			{
				collection.push_back(una::norm::to_nfc_utf8(una::utf32to8(str)));
			}
			cur = InOrderSuccessor(cur, str);
		}
		node->pa = pa;
	}

  public:
	// nil's attributes can only be changed during update process
	// after update process, nil's attributes must be restored
	RBTrie() : nil(new Node{Node::BLACK, false, false, 0, "", nullptr, nullptr, nullptr, nullptr})
	{
		nil->lo = nil->eq = nil->hi = nil->pa = nil;
		root = nil;
	}
	~RBTrie()
	{
		Deallocate(root);
		delete nil;
	}
	// iterative method, as recursive can't handle long string
	void Clear()
	{
		Deallocate(root);
		root = nil;
	}
	// return the number of node in the tree
	long long Count() const
	{
		return Count(root);
	}
	// iterative method, as recursive can't handle long string
	Node *Insert(std::string key, std::string value)
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return nil;
		}
		Node *end = nil;
		std::u32string str = una::utf8to32u(una::norm::to_nfd_utf8(key));
		if (root == nil)
		{
			end = AddTail(nil, str, 0, value);
			root = nil->eq;
			nil->eq = nil;
			return end;
		}
		Node *node = root;
		Node *rt = root;
		int pos = 0;
		while (pos < str.size())
		{
			while (str[pos] != node->codepoint)
			{
				if (str[pos] < node->codepoint)
				{
					if (node->lo != nil)
					{
						node = node->lo;
					}
					else
					{
						node->lo = new Node{Node::RED, false, false, str[pos], "", nil, nil, nil, node};
						end = AddTail(node->lo, str, pos + 1, value);
						InsertUpdate(rt, node->lo);
						return end;
					}
				}
				else
				{
					if (node->hi != nil)
					{
						node = node->hi;
					}
					else
					{
						node->hi = new Node{Node::RED, false, false, str[pos], "", nil, nil, nil, node};
						end = AddTail(node->hi, str, pos + 1, value);
						InsertUpdate(rt, node->hi);
						return end;
					}
				}
			}
			pos += 1;
			if (node->eq != nil && pos < str.size())
			{
				node = node->eq;
				rt = node;
			}
			else
			{
				end = AddTail(node, str, pos, value);
				return end;
			}
		}
	}
	// iterative method, as recursive can't handle long string
	void Remove(std::string key)
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return;
		}
		std::u32string str = una::utf8to32u(una::norm::to_nfd_utf8(key));
		int pos = -1;
		nil->eq = root;
		Node *node = nil;
		while (pos < str.size())
		{
			node = node->eq;
			pos += 1;
			while (node != nil && str[pos] != node->codepoint)
			{
				if (str[pos] < node->codepoint)
				{
					node = node->lo;
				}
				else
				{
					node = node->hi;
				}
			}
			if (node == nil)
			{
				nil->eq = nil;
				return;
			}
		}
		nil->eq = nil;
		node->end = false;
		while (node->subroot && !node->end && node->eq == nil && node->lo == nil && node->hi == nil)
		{
			Node *pa = node->pa;
			delete node;
			node = pa;
			node->eq = nil;
		}
		if (node == nil)
		{
			root = nil;
			return;
		}
		if (node->end || node->eq != nil)
		{
			return;
		}
		Node *rt = node;
		while (!rt->subroot)
		{
			rt = rt->pa;
		}
		if (rt == root)
		{
			root = RemoveUpdate(root, node);
		}
		else
		{
			Node *pa = rt->pa;
			rt->pa = nil;
			pa->eq = RemoveUpdate(rt, node);
			pa->eq->pa = pa;
		}
	}
	// i believe we should not return error code string, instead
	// we should return an empty string and an error code elsewhere
	std::string Search(std::string key) const
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return (const char *)u8"Invalid key";
		}
		std::u32string str = una::utf8to32u(una::norm::to_nfd_utf8(key));
		Node *node = root;
		int pos = 0;
		while (pos < str.size())
		{
			while (node != nil && str[pos] != node->codepoint)
			{
				if (str[pos] < node->codepoint)
				{
					node = node->lo;
				}
				else
				{
					node = node->hi;
				}
			}
			if (node == nil)
			{
				return (const char *)u8"Key not found";
			}
			pos += 1;
			if (pos < str.size())
			{
				node = node->eq;
			}
			else if (!node->end)
			{
				return (const char *)u8"Key not found";
			}
			else
			{
				return node->value;
			}
		}
	}
	// find all string that have certain prefix
	std::vector<std::string> PrefixSearch(std::string key)
	{
		if (key.empty() || !una::is_valid_utf8(key))
		{
			return {};
		}
		std::u32string str = una::utf8to32u(una::norm::to_nfd_utf8(key));
		Node *node = root;
		int pos = 0;
		while (pos < str.size())
		{
			while (node != nil && str[pos] != node->codepoint)
			{
				if (str[pos] < node->codepoint)
				{
					node = node->lo;
				}
				else
				{
					node = node->hi;
				}
			}
			if (node == nil)
			{
				return {};
			}
			pos += 1;
			if (pos < str.size())
			{
				node = node->eq;
			}
			else
			{

				std::vector<std::string> collection;
				if (node->end)
				{
					collection.push_back(una::norm::to_nfc_utf8(una::utf32to8(str)));
				}
				Collect(node->eq, str, collection);
				return collection;
			}
		}
	}
	// get the k-th string in the tree
	std::string GetKthWord(int k)
	{
		std::u32string word;
		Node *cur = InOrderBegin(root, word);
		if (cur->end)
		{
			k--;
		}
		while (cur != nil && k > 0)
		{
			cur = InOrderSuccessor(cur, word);
			if (cur->end)
			{
				k--;
			}
		}
		return una::norm::to_nfc_utf8(una::utf32to8(word));
	}

	// this is just to test the correctness of the tree, will be removed
	// void Validate() const
	//{
	//	Validate(root);
	//}
};
