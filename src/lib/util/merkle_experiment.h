/* This code is subject to the terms of the Mozilla Public License, v.2.0. http://mozilla.org/MPL/2.0/. */
#pragma once

#include <bitset>
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
			: _value(0)
			, _empty(true)
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

		ValueType insert(const KeyType& key, const ValueType& value)
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
				subtree->insert(_node.key, _value);
				_leaf = false;
				_node.subtree = subtree;
			}
			return _value = _node.subtree->insert(key, value);
		}

		ValueType remove(const KeyType& key)
		{
			if (_empty)
				return 0;

			if (!_leaf)
			{
				_value = _node.subtree->remove(key);
				if (_node.subtree->count() == 1)
				{
					merkle_tree* subtree = _node.subtree;
					subtree->findKey(_node.key);
					_leaf = true;
					delete subtree;
				}
				return _value;
			}

			if (_node.key != key)
				return 0;

			_leaf = false;
			_empty = true;
			return 0;
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

		bool empty() const
		{
			return _empty;
		}

		const KeyType& key() const
		{
			return _node.key;
		}

		const ValueType& value() const
		{
			return _value;
		}

		const merkle_tree* subtree() const
		{
			if (_empty || _leaf)
				return NULL;
			return _node.subtree;
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
		: _value(0)
	{
		_nodes.resize(64);
	}

	unsigned count() const
	{
		return _bitset.count();
	}

	ValueType value() const
	{
		return _value;
	}

	ValueType insert(const KeyType& key, const ValueType& value)
	{
		unsigned index = key & 0x3F;
		setBit(index);

		node& node(_nodes[index]);
		_value ^= node.value();
		return _value ^= node.insert(key >> 6, value);
	}

	ValueType remove(const KeyType& key)
	{
		unsigned index = key & 0x3F;
		node& node(_nodes[index]);
		_value ^= node.value();
		ValueType val = node.remove(key >> 6);
		if (node.empty())
			clearBit(index);
		return _value ^= val;
	}

	/*const ValueType& operator[](const KeyType& key) const
	{
		return
	}*/

	// how to interate?
	const merkle_tree* subtree(const KeyType& key, unsigned level = 1) const
	{
		unsigned index = key & 0x3F;
		const merkle_tree* tree = _nodes[index].subtree();
		if (tree == NULL || --level == 0)
			return tree;
		return tree->subtree(key >> 6, level);
	}

	// how does "level" work?
	std::vector<ValueType> values() const
	{
		std::vector<ValueType> values;
		values.reserve(_nodes.size());
		for (typename std::vector<node>::const_iterator it = _nodes.begin(); it != _nodes.end(); ++it)
			values.push_back(it->empty()? 0 : it->value());
		return values;
	}

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
	bool findKey(KeyType& key)
	{
		for (unsigned i = 0; i < 64; ++i)
		{
			if (_bitset.test(i))
			{
				key = _nodes[i].key() << 6;
				key |= i;
				return true;
			}
		}
		return false;
	}

	void setBit(unsigned index)
	{
		_bitset |= std::bitset<64>(1ULL << index);
	}

	void clearBit(unsigned index)
	{
		_bitset ^= std::bitset<64>(1ULL << index);
	}

protected:
	ValueType _value;
	std::bitset<64> _bitset;
	std::vector<node> _nodes;
};
