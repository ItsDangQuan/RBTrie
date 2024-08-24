#include <string>
#include <vector>

/* Red Black Tree rule:
 * 1. Every node is either red or black.
 * 2. The root is black.
 * 3. Every leaf (NIL) is black.
 * 4. If a node is red, then both its children are black.
 * 5. For each node, all simple paths from the node to descendant leaves contain the same number of black nodes.
 */

template <typename Key, typename Value> class RBTree
{
  private:
	struct Node
	{
		enum Color : char
		{
			RED = 0,
			BLACK
		} color;
		Key key;
		Value value;
		Node *left;
		Node *right;
		Node *parent;
	};

  public:
	class Iterator
	{
	  private:
		RBTree *tree;
		Node *node;

	  public:
		Key *first;
		Value *second;

	  private:
		friend class RBTree;
		Iterator(RBTree *tree, Node *node) : tree(tree), node(node), first(&node->key), second(&node->value){};

	  public:
		Iterator &operator++()
		{
			node = tree->Successor(node);
			first = &node->key;
			second = &node->value;
			return *this;
		}
		friend bool operator==(const Iterator &lhs, const Iterator &rhs)
		{
			return lhs.tree == rhs.tree && lhs.node == rhs.node;
		}
		friend bool operator!=(const Iterator &lhs, const Iterator &rhs)
		{
			return !(rhs == lhs);
		}
	};

  private:
	Node *root;
	Node *nil; // sentinel, stand-in for root->parent and leaves

  private:
	// assume x->right is not nil
	void LeftRotate(Node *x)
	{
		Node *y = x->right;
		x->right = y->left;			   // turn y's left subtree into x's right subtree
		if (y->left != nil)			   // if y's left subtree is not empty...
		{							   //
			y->left->parent = x;	   // ...then x becomes the parent of the subtree's root
		}							   //
		y->parent = x->parent;		   // x's parent becomes y's parent
		if (x->parent == nil)		   // if x is the root...
		{							   //
			root = y;				   // ...then y becomes the root
		}							   //
		else if (x == x->parent->left) // otherwise, if x was a left child...
		{							   //
			x->parent->left = y;	   // ...then y becomes a left child
		}							   //
		else						   // otherwise, x was a right child...
		{							   //
			x->parent->right = y;	   // ...and now y is
		}							   //
		y->left = x;				   // make x become y's left child
		x->parent = y;
	}
	// assume x->left is not nil
	void RightRotate(Node *x)
	{
		Node *y = x->left;
		x->left = y->right;			   // turn y's right subtree into x's left subtree
		if (y->right != nil)		   // if y's right subtree is not empty...
		{							   //
			y->right->parent = x;	   // ...then x becomes the parent of the subtree's root
		}							   //
		y->parent = x->parent;		   // x's parent becomes y's parent
		if (x->parent == nil)		   // if x is the root...
		{							   //
			root = y;				   // ...then y becomes the root
		}							   //
		else if (x == x->parent->left) // otherwise, if x was a left child
		{							   //
			x->parent->left = y;	   // ...then y becomes a left child
		}							   //
		else						   // otherwise, x was a right child...
		{							   //
			x->parent->right = y;	   // ...and now y is
		}							   //
		y->right = x;				   // make x become y's right child
		x->parent = y;
	}
	void InsertFixup(Node *z)
	{
		while (z->parent->color == Node::RED)
		{
			if (z->parent == z->parent->parent->left)
			{
				Node *y = z->parent->parent->right; // uncle node
				if (y->color == Node::RED)
				{
					// case 1, only recolor, loop again due to possible rule 4 violation at z's grandparent
					z->parent->color = Node::BLACK;
					y->color = Node::BLACK;
					z->parent->parent->color = Node::RED;
					z = z->parent->parent;
				}
				else
				{
					if (z == z->parent->right)
					{
						// case 2, rotate put the system into case 3
						z = z->parent;
						LeftRotate(z);
					}
					// case 3, rotate without colors, fixing all violation without creating new one
					z->parent->color = Node::BLACK;
					z->parent->parent->color = Node::RED;
					RightRotate(z->parent->parent);
				}
			}
			else
			{
				// Mirror cases
				Node *y = z->parent->parent->left;
				if (y->color == Node::RED)
				{
					z->parent->color = Node::BLACK;
					y->color = Node::BLACK;
					z->parent->parent->color = Node::RED;
					z = z->parent->parent;
				}
				else
				{
					if (z == z->parent->left)
					{
						z = z->parent;
						RightRotate(z);
					}
					z->parent->color = Node::BLACK;
					z->parent->parent->color = Node::RED;
					LeftRotate(z->parent->parent);
				}
			}
		}
		root->color = Node::BLACK;
	}
	// node should not be nil
	Node *Successor(Node *node) const
	{
		if (node->right != nil)
		{
			return Minimum(node->right);
		}
		else
		{
			Node *ret = node;
			while (ret->parent != nil && ret->parent->left != ret)
			{
				ret = ret->parent;
			}
			return ret->parent;
		}
	}
	// node should not be nil
	Node *Minimum(Node *node) const
	{
		while (node->left != nil)
		{
			node = node->left;
		}
		return node;
	}
	void Transplant(Node *u, Node *v)
	{
		if (u->parent == nil)
		{
			root = v;
		}
		else if (u == u->parent->left)
		{
			u->parent->left = v;
		}
		else
		{
			u->parent->right = v;
		}
		v->parent = u->parent;
	}
	void RemoveFixup(Node *x)
	{
		// if x is red, just set it to black fix the problem, so the loop is not entered
		// if x is the root, just remove that extra black that x hold is fine
		while (x != root && x->color == Node::BLACK)
		{
			if (x == x->parent->left)
			{
				// 4 cases where w is x's right sibling
				Node *w = x->parent->right;
				if (w->color == Node::RED)
				{
					// if w is red, rotate the tree left so that new w is black
					// w and parent exchange color to abide by the rule
					w->color = Node::BLACK;
					x->parent->color = Node::RED;
					LeftRotate(x->parent);
					w = x->parent->right;
				}
				if (w->left->color == Node::BLACK && w->right->color == Node::BLACK)
				{
					// if w only have black child, recolor w and x to red and move the extra black to x's parent
					w->color = Node::RED;
					x = x->parent;
				}
				else
				{
					if (w->right->color == Node::BLACK)
					{
						// if w's inner child is red, rotate so that only new w's outer child is red
						// w and its inner child exchange color to abide by the rule
						w->left->color = Node::BLACK;
						w->color = Node::RED;
						RightRotate(w);
						w = x->parent->right;
					}
					// if only w's outer child is red, recoloring and rotating make the extra black go away
					w->color = x->parent->color;
					x->parent->color = Node::BLACK;
					w->right->color = Node::BLACK;
					LeftRotate(x->parent);
					x = root;
				}
			}
			else
			{
				// 4 mirror cases
				Node *w = x->parent->left;
				if (w->color == Node::RED)
				{
					w->color = Node::BLACK;
					x->parent->color = Node::RED;
					RightRotate(x->parent);
					w = x->parent->left;
				}
				if (w->left->color == Node::BLACK && w->right->color == Node::BLACK)
				{
					w->color = Node::RED;
					x = x->parent;
				}
				else
				{
					if (w->left->color == Node::BLACK)
					{
						w->right->color = Node::BLACK;
						w->color = Node::RED;
						LeftRotate(w);
						w = x->parent->left;
					}
					w->color = x->parent->color;
					x->parent->color = Node::BLACK;
					w->left->color = Node::BLACK;
					RightRotate(x->parent);
					x = root;
				}
			}
		}
		x->color = Node::BLACK;
	}

  public:
	RBTree() : nil(new Node{Node::BLACK})
	{
		nil->parent = nil->left = nil->right = nil;
		root = nil;
	}
	RBTree(RBTree &&other) : nil(other.nil), root(other.root)
	{
		other.nil = new Node{Node::BLACK, Key(), Value(), nullptr, nullptr, nullptr};
		other.nil->left = other.nil->right = other.nil->parent = other.nil;
		other.root = other.nil;
	}
	~RBTree()
	{
		while (root != nil)
		{
			if (root->left != nil)
			{
				root = root->left;
			}
			else if (root->right != nil)
			{
				root = root->right;
			}
			else
			{
				Node *deletee = root;
				root = root->parent;
				if (deletee == root->left)
				{
					root->left = nil;
				}
				else
				{
					root->right = nil;
				}
				delete deletee;
			}
		}
		delete nil;
	}
	void Insert(Key key, Value value)
	{
		Node *x = root;	 // node for key comparison
		Node *y = nil;	 // soon to be new node parent
		while (x != nil) // descend until nil
		{
			y = x;
			if (key == x->key)
			{
				return; // key already exist, stop here
			}
			if (key < x->key)
			{
				x = x->left;
			}
			else
			{
				x = x->right;
			}
		}
		Node *z = new Node{
			Node::RED, key, value, nil, nil, y,
		}; // make new node, assign y as the parent
		if (y == nil)
		{
			root = z; // empty tree case
		}
		else if (key < y->key)
		{
			y->left = z;
		}
		else
		{
			y->right = z;
		}
		InsertFixup(z);
	}
	void Remove(Key key)
	{
		// find the node with such key
		Node *z = root;
		while (z != nil && key != z->key)
		{
			if (key < z->key)
			{
				z = z->left;
			}
			else
			{
				z = z->right;
			}
		}
		// if not found then stop
		if (z == nil)
		{
			return;
		}
		// keep the color, if the deletee node is black then there will be rule violations to fix
		Node *y = z;
		Node::template Color yOriginalColor = y->color;
		// the site of rule violations if there are any
		Node *x;
		if (z->left == nil)
		{
			// if the deletee have as most 1 child, then just push the child up
			x = z->right;
			Transplant(z, z->right);
		}
		else if (z->right == nil)
		{
			// mirror of above
			x = z->left;
			Transplant(z, z->left);
		}
		else
		{
			// swap the deletee with its successor
			y = Minimum(z->right);
			yOriginalColor = y->color;
			// we keep the original destination colors when swapping, so the problematic site is the successor
			x = y->right;
			if (y != z->right)
			{
				// case when y is NOT the direct child of z
				Transplant(y, y->right); // push y's right up
				y->right = z->right;	 // change z's right...
				y->right->parent = y;	 // ...to y's right
			}
			else
			{
				x->parent = y; // x can be nil!!!
			}
			Transplant(z, y);	 // swap z and y
			y->left = z->left;	 // change z's left...
			y->left->parent = y; // ...to y's left
			y->color = z->color; // change y color to z color (in other word, swap y and z without color)
		}
		// finally delete the deletee
		delete z;
		// fix any problems that arise
		if (yOriginalColor == Node::BLACK)
		{
			RemoveFixup(x);
		}
		nil->parent = nil; // return nil back to normal
	}
	int Size() const
	{
		int size = 0;
		Node *cur = Minimum(root);
		while (cur != nil)
		{
			size++;
			cur = Successor(cur);
		}
		return size;
	}
	Iterator Begin() const
	{
		return Iterator((RBTree *)this, Minimum(root));
	}
	Iterator End() const
	{
		return Iterator((RBTree *)this, (Node *)nil);
	}
	Iterator Find(Key key) const
	{
		Node *cur = root;
		while (cur != nil)
		{
			if (key < cur->key)
			{
				cur = cur->left;
			}
			else if (key > cur->key)
			{
				cur = cur->right;
			}
			else
			{
				return Iterator((RBTree *)this, cur);
			}
		}
		return End();
	}
	Value &operator[](Key key)
	{
		Iterator iter = Find(key);
		if (iter != End())
		{
			return *iter.second;
		}
		Insert(key, Value());
		return *Find(key).second;
	}
};
