#pragma once

#include <iostream>
#include <vector>

// basically:
//   KeyType == file hash == 64-128 bits
//   ValueType == file contents

template <class KeyType, class ValueType>
class merkle_tree
{
protected:
	class node
	{
	public:
		node()
			: _empty(true)
			, _leaf(false)
		{}

		~node()
		{
			if (!_empty & !_leaf && _node.subtree != NULL)
			{
				delete _node.subtree;
				_node.subtree = NULL;
			}
		}

		ValueType set(const KeyType& key, const ValueType& value)
		{
			if (_empty)
			{
				_empty = false;
				_leaf = true;
				_node.key = key;
				_value = value;
				return value;
			}

			if (_leaf && _node.key == key)
				return _value = value;

			if (_leaf)
			{
				merkle_tree* subtree = new merkle_tree();
				subtree->set(_node.key, _value);
				_leaf = false;
				_node.subtree = subtree;
			}
			return _value ^= _node.subtree->set(key, value);
		}

		void print(int level = 0) const
		{
			if (_empty)
				;
			else if (_leaf)
				std::cout << _node.key << ":" << _value << " ";
			else
			{
				std::cout << "(" << _value << ") ";
				_node.subtree->print(level+1);
			}
		}

	protected:
		ValueType _value;
		union leaf_or_tree {
			KeyType key;
			merkle_tree* subtree;
		} _node;
		bool _empty;
		bool _leaf;
	};

public:
	merkle_tree()
	{
		_nodes.resize(64);
	}

	ValueType set(const KeyType& key, const ValueType& value)
	{
		unsigned index = key & 0x3F;
		std::cout << "setting (" << key << "," << value << ") into bucket " << index << std::endl;
		_nodes[index].set(key >> 6, value);
	}

	/*const ValueType& operator[](const KeyType& key) const
	{
		return
	}*/

	void print(int level = 0) const
	{
		std::cout << std::endl;
		for (int i = 0; i < level; ++i)
			std::cout << "  ";
		for (typename std::vector<node>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it)
			it->print(level);
		std::cout << std::endl;
	}

protected:
	std::vector<node> _nodes;
};
