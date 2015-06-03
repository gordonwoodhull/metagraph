// Copyright 2008-2010 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSEmpl::_1_0.txt or copy at http://www.boost.org/LICENSEmpl::_1_0.txt)

// adjacency_list_graph100.cpp: test dfs & bfs on a large "random" graph
//

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 100

#include <boost/metagraph/mpl_graph/adjacency_list_graph.hpp>
#include <boost/metagraph/mpl_graph/mpl_utils.hpp>
#include <boost/metagraph/mpl_graph/depth_first_search.hpp>
#include <boost/metagraph/mpl_graph/breadth_first_search.hpp>

#include <boost/mpl/fold.hpp>

#include <boost/mpl/print.hpp>

#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/range_c.hpp>

#include <boost/mpl/equal.hpp>
#include <boost/mpl/modulus.hpp>
#include <boost/mpl/times.hpp>
#include <boost/mpl/plus.hpp>


namespace mpl_graph = boost::metagraph::mpl_graph;
namespace mpl_utils = mpl_graph::mpl_utils;
namespace mpl = boost::mpl;


#if !defined(SEARCH_BFS) && !defined(SEARCH_DFS)
#define SEARCH_BFS
#endif


struct preordering_visitor : mpl_graph::
#ifdef SEARCH_DFS
    dfs_default_visitor_operations
#elif defined(SEARCH_BFS)
    bfs_default_visitor_operations
#endif
{    
    template<typename Vertex, typename Graph, typename State>
    struct discover_vertex :
        mpl::push_back<State, Vertex>
    {};
};


template<int ID>
    struct vertex {};
    
template<int ID>
    struct edge {};
    
template<typename ID>
    struct make_vertex {
        typedef vertex<ID::value> type;
    };
    


// 100 prime numbers
typedef mpl::vector_c<int, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 
    1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 
    1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 
    1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 
    1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 
    1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 
    1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931>
    primes_vc;

// generate EPV edges per vertex
// prob not a realistic graph, just a stress test

// is this psuedorandom enough?
// P = primes[EI%NP]  
// target V = (EI+1)*P % NV 

template<typename EI>
struct get_prime :
    mpl::at<primes_vc, typename mpl::modulus<EI,mpl::size<primes_vc>::type>::type > {};

template<typename EI, typename NV>
struct gen_edge :
    mpl::pair<edge<EI::value>, vertex<mpl::
        modulus<typename mpl::
            times<typename mpl::next<EI>::type, 
                  typename get_prime<EI>::type>::type, NV>::value > >
{};

template<typename V, typename E0, typename NV, typename EPV>
struct gen_edges :
    mpl::fold<mpl::range_c<int, E0::value, mpl::plus<E0,EPV>::value >,
         mpl::vector<>,
         mpl::push_back<mpl::_1, gen_edge<mpl::_2, NV> > > {};
         
template<typename NV, typename EPV>
struct gen_graph :
    mpl::fold<mpl::range_c<int, 0, NV::value>,
    mpl::vector<>,
    mpl::push_back<mpl::_1, mpl::pair<make_vertex<mpl::_2>, gen_edges<mpl::_2, mpl::times<mpl::_2, EPV>, NV, EPV> > > > {};

typedef mpl_graph::adjacency_list_graph<gen_graph<mpl::int_<10>, mpl::int_<5> >::type > some_graph;

BOOST_MPL_ASSERT(( boost::is_same<mpl::int_<17>, some_graph> ));

BOOST_MPL_ASSERT(( boost::is_same<mpl::int_<17>, 
    mpl::first<mpl_graph::
#ifdef SEARCH_DFS
    depth_first_search
#elif defined(SEARCH_BFS)
    breadth_first_search
#endif
#ifdef SEARCH_ALL
    ##_all
#endif
    <some_graph, preordering_visitor, mpl::vector<>
#ifndef SEARCH_ALL
    ,vertex<0>
#endif
     >::type>::type> ));

int main(int argc, char* argv[])
{
    return 0;
}

