//============================================================================
// tests.cpp
// Copyright (c) 2011 Pete Goodliffe. All rights reserved
//============================================================================

#include "skip_list.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using goodliffe::skip_list;

TEST_CASE( "skip_list/smoketest", "" )
{
    //REQUIRE(false);
}

//============================================================================

struct Struct
{
    int i;
    float f;

    Struct() : i(0), f(0) {}
    Struct(int i_, float f_) : i(i_), f(f_) {}
};

inline
bool operator==(const Struct &lhs, const Struct &rhs)
    { return lhs.i == rhs.i && lhs.f == rhs.f; }
inline
bool operator!=(const Struct &lhs, const Struct &rhs)
    { return !operator==(lhs,rhs); }

template <typename T = int>
struct TestingAllocator : Struct
{
    TestingAllocator() : Struct() {}
    TestingAllocator(int i, float f) : Struct(i,f) {}

    template <typename OTHER>
    struct rebind { typedef TestingAllocator<OTHER> other; };
    
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef T*        pointer;
    typedef const T*  const_pointer;
};

//============================================================================
// simple construction tests

TEST_CASE( "skip_list/can be constructed and destroyed", "" )
{
    skip_list<int>    sl_int;
    skip_list<float>  sl_float;
    skip_list<Struct> sl_struct;

    REQUIRE(true);
}

TEST_CASE( "skip_list/default construction gets default allocator", "" )
{
    REQUIRE(skip_list<int>().get_allocator() == std::allocator<int>());
    REQUIRE((skip_list<int,std::less<int>,TestingAllocator<int> >().get_allocator()) == TestingAllocator<int>());
}

TEST_CASE( "skip_list/construction with allocator returns copy of that allocator", "" )
{
    REQUIRE((skip_list<int,std::less<int>,TestingAllocator<int> >(TestingAllocator<int>(10,4)).get_allocator()) == TestingAllocator<int>(10,4));
}

TEST_CASE( "skip_list/constructed list returns empty()", "" )
{
    const skip_list<int> list;
    REQUIRE(list.empty());
}

TEST_CASE( "skip_list/max_size() does something sensible", "" )
{
    const skip_list<int> list;
    REQUIRE(list.max_size() > 10000);
}

TEST_CASE( "skip_list/empty list does not count() a value", "" )
{
    const skip_list<int> list;
    REQUIRE(list.count(10) == 0);
    REQUIRE(list.count(1) == 0);
    REQUIRE(list.count(0) == 0);
}

//============================================================================
// iterators

TEST_CASE( "skip_list/default iterator equality", "" )
{
    skip_list<int>::iterator a, b;
    REQUIRE(a == b);
}

TEST_CASE( "skip_list/default iterator inequality", "" )
{
    skip_list<int>::iterator a, b;
    REQUIRE_FALSE(a != b);
}

TEST_CASE( "skip_list/default const_iterator equality", "" )
{
    skip_list<int>::const_iterator a, b;
    REQUIRE(a == b);
}

TEST_CASE( "skip_list/default const_iterator inequality", "" )
{
    skip_list<int>::const_iterator a, b;
    REQUIRE_FALSE(a != b);
}

TEST_CASE( "skip_list/default iterator/const_iterator equality", "" )
{
    skip_list<int>::iterator a;
    skip_list<int>::const_iterator b;
    REQUIRE(a == b);
    REQUIRE(b == a);
}

TEST_CASE( "skip_list/default iterator/const_iterator inequality", "" )
{
    skip_list<int>::iterator a;
    skip_list<int>::const_iterator b;
    REQUIRE_FALSE(a != b);
    REQUIRE_FALSE(b != a);
}

TEST_CASE( "skip_list/empty list begin() is end()", "" )
{
    skip_list<int> list;
    
    REQUIRE(list.begin() == list.end());
    (list.cbegin() == list.cend());
    
    const skip_list<int> &clist(list);
    REQUIRE(clist.cbegin() == clist.cend());
}

//============================================================================
// Checking code compiles

TEST_CASE( "skip_list/iterator converts to const iterator", "" )
{
    skip_list<int> list;
    skip_list<int>::iterator i = list.begin();
    skip_list<int>::const_iterator ci = i;
    (void)ci;
    REQUIRE(true);
}

//============================================================================
// inserting one item

TEST_CASE( "skip_list/inserting one item is not empty()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE_FALSE(list.empty());
}

TEST_CASE( "skip_list/inserting one item begin() is not end()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.begin() != list.end());
    REQUIRE(list.cbegin() != list.cend());
    REQUIRE_FALSE(list.begin() == list.end());
    REQUIRE_FALSE(list.cbegin() == list.cend());

    const skip_list<int> &clist(list);
    REQUIRE(clist.cbegin() != clist.cend());
    REQUIRE_FALSE(clist.cbegin() == clist.cend());
}

TEST_CASE( "skip_list/inserting one item returned from front()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.front() == 10);
    
    const skip_list<int> &clist(list);
    REQUIRE(clist.front() == 10);
}

TEST_CASE( "skip_list/inserting one item returned from begin()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.front() == 10);
    
    const skip_list<int> &clist(list);
    REQUIRE(clist.front() == 10);
}

TEST_CASE( "skip_list/inserting one item returns a count()", "" )
{
    skip_list<int> list;
    list.insert(10);
    REQUIRE(list.count(0) == 0);
    REQUIRE(list.count(1) == 0);
    REQUIRE(list.count(10) == 1);
    REQUIRE(list.count(11) == 0);
}

//============================================================================
// random level selection

TEST_CASE( "skip_list/inserting one item returned from begin()", "" )
{
    skip_list<int> list;
    std::vector<unsigned> levels(skip_list<int>::max_levels, 0);
    for (unsigned n = 0; n < 10000; ++n)
    {
        unsigned random = list.random_level();
        REQUIRE(random < skip_list<int>::max_levels);
        levels[random]++;
    }
    //for (unsigned n = 0; n < skip_list<int>::max_levels; ++n)
    //    fprintf(stderr, "Level[%u]=%u\n", n, levels[n]);
    
    for (unsigned n = 0; n < skip_list<int>::max_levels-1; ++n)
    {
        if (levels[n+1]) break;
        REQUIRE(levels[n] > levels[n+1]);
    }
}

//============================================================================
// iterating through a series of items

TEST_CASE( "skip_list/inserting an ordered series and iterating through", "" )
{
    skip_list<int> list;
    list.insert(10);
    list.insert(20);
    list.insert(30);
    list.insert(40);
    
    skip_list<int>::iterator i = list.begin();

    REQUIRE(i != list.end()); REQUIRE(*i == 10); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 20); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 30); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 40); ++i;
    REQUIRE(i == list.end());
}

TEST_CASE( "skip_list/inserting an unordered series and iterating through", "" )
{
    skip_list<int> list;
    list.insert(30);
    list.insert(10);
    list.insert(40);
    list.insert(20);
    list.insert(0);
    list.dump();
    
    skip_list<int>::iterator i = list.begin();
    
    REQUIRE(i != list.end()); REQUIRE(*i == 0); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 10); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 20); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 30); ++i;
    REQUIRE(i != list.end()); REQUIRE(*i == 40); ++i;
    REQUIRE(i == list.end());
}
