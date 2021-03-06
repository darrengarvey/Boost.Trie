#ifndef BOOST_TRIE_HPP
#define BOOST_TRIE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif


#include <map>
#include <iterator>
#include <utility>
#include <cstdio>
#include <memory>

namespace boost { namespace tries {

namespace detail {

template <typename Key, typename Value,
		 class Compare>
struct trie_node {
//protected:
	typedef Key key_type;
	typedef key_type* key_ptr;
	typedef Value value_type;
	typedef value_type* value_ptr;
	typedef trie_node<key_type, value_type, Compare> node_type;
	typedef node_type* node_ptr;
	typedef std::map<key_type, node_ptr, Compare> children_type;
	typedef typename children_type::iterator child_iter;
	typedef std::allocator<Value> value_allocator; // is it necessary here?

	children_type child;

//public:
	value_ptr value; // use unique_ptr here?
	node_ptr parent;
	//child_iter child_iter_of_parent;
	size_t node_count;
	size_t value_count;

	value_ptr new_value()
	{
		return value = value_allocator::allocate();
	}

	value_ptr new_value(const value_type& value)
	{
		if (!value)
			value = value_allocator::allocate();
		construct(value, value);
		return value;
	}

	void delete_value()
	{
		value_allocator::deallocate();
	}

	trie_node() : value(0), parent(0), node_count(0), value_count(0) 
	{
		child.clear();
	}

	~trie_node()
	{
	}
};

template <typename Key, typename Value, class Compare>
struct trie_iterator
{
	//typedef std::bidirectional_iterator_tag iterator_category;
	typedef Key key_type;
	typedef Value value_type;
	typedef value_type& ref;
	typedef value_type* ptr;
	typedef trie_iterator<Key, Value, Compare> iterator;
	typedef trie_node<Key, Value, Compare> node_type;
	typedef iterator self;
	typedef node_type* node_ptr;

	node_ptr node;

	trie_iterator() : node(0)
	{
	}

	trie_iterator(node_ptr x) : node(x)
	{
	}

	trie_iterator(const iterator &it): node(it.node)
	{
	}

	ref operator*() const 
	{
		return node->value;
	}
	ptr operator->() const
	{
		return &(operator*()); 
	}

	self& operator++() 
	{
		// increment
		return *this;
	}
	self& operator++(int)
	{
		self tmp = *this;
		// increment
		return tmp;
	}

	self& operator--()
	{
		// decrement
		return *this;
	}
	self& operator--(int)
	{
		self tmp = *this;
		// decrement
		return tmp;
	}
}; 

} // namespace detail

template <typename Key, typename Value,
		 class Compare>
class trie {
public:
	typedef Key key_type;
	typedef Value value_type;
	typedef trie<key_type, value_type, Compare> trie_type;
	typedef typename detail::trie_node<key_type, value_type, Compare> node_type;
	typedef key_type * key_ptr;
	typedef node_type * node_ptr;
	typedef std::allocator< node_type > trie_node_allocator;
	typedef size_t size_type;

	node_ptr root;
	size_type node_count;
	size_type value_count;

	node_ptr new_node() 
	{
		return trie_node_allocator::allocate(sizeof(node_type));
	}

	void delete_node(node_ptr p)
	{
		// 
		destroy(p);
		trie_node_allocator::deallocate(p);
	}

	node_type& leftmost() const
	{
		// to be written
		return NULL;
	}

	node_type& rightmost() const
	{
		// to be written
		return NULL;
	}

public:
	// iterators still unavailable here
	
	trie() {
		root = new_node();
	}

	typedef detail::trie_iterator<Key, Value, Compare> iterator;

	iterator begin()
	{
		return leftmost();
	}

	iterator end()
	{
		return root;
	}

	template<typename Iter>
	iterator __insert(Iter first, Iter last,
			const value_type& value)
	{
		node_ptr cur = root;
		for (; first != last; ++first)
		{
			const key_type& cur_key = *first;
			typename node_type::child_iter ci = cur->child.find(cur->child[cur_key]);
			if (ci == cur->child.end())
			{
				node_ptr new_node = new_node();
				ci = cur->child.insert(std::make_pair(cur_key, new_node)).first;
				new_node->parent = cur;
			}
			cur = *ci;
		}
		// this should be changed to adapt to single value
		if (!cur->value)
			++value_count;
		cur->new_value(value);

		return cur;
	}

	template<typename Container>
	iterator insert(Container &container)
	{
		return __insert(container.begin(), container.end());
	}

	static void clear(node_ptr cur)
	{
		typename node_type::child_iter ci = cur->child.begin();
		// end() may take time to calc
		typename node_type::child_iter ci_end = cur->child.end();
		for (; ci != ci_end; ++ci)
		{
			node_ptr c = ci->second;
			clear(c);
			delete_node(c);
		}
	}

	size_t size() const
	{
		return value_count;
	}
	
	bool empty() const {
		return value_count == 0;
	}

	bool destroy()
	{
		clear(root);
		delete_node(root);
	}

	~trie()
	{
		destroy();
	}

};

template<typename Key, typename Value,
		class Compare = std::less<Key> >
class trie_map
{
public:
	typedef Key key_type;
	typedef Value value_type;
	typedef trie<key_type, value_type, Compare> trie_type;
	typedef typename trie_type::iterator iterator;
	typedef size_t size_type;

protected:
	trie_type t;

public:
	trie_map()
	{
	}


	template<typename Container>
	value_type& operator [] (const Container& container)
	{
		return *(insert(container, value_type()));
	}

	template<typename Container>
	iterator insert(const Container& container)
	{
		return t.insert(container);
	}

	~trie_map()
	{
		t.destroy();
	}

};

} // tries
} // boost
#endif // BOOST_TRIE_HPP
