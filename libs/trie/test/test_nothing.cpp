// Copyright 2013 Hardy.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#include <boost/trie.hpp>

#include <boost/test/included/test_exec_monitor.hpp>

int test_main(int, char* [])
{
    boost::trie::trie<int> object;
    int x = 0;
    BOOST_CHECK_EQUAL(object.get_nothing_count(),0);
    object.nothing(x);
    BOOST_CHECK_EQUAL(object.get_nothing_count(),1);
    
    return 0;
};
