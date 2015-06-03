// Copyright 2011 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50

#include <iostream>
#include <boost/msm/mpl_graph/adjacency_list_graph.hpp>
#include <boost/msm/mpl_graph/depth_first_search.hpp>
#include <boost/metagraph/angly/dfa_lang.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/joint_view.hpp>

#include <boost/units/detail/utility.hpp>

namespace mpl = boost::mpl;
namespace mpl_graph = boost::msm::mpl_graph;
namespace angly = boost::metagraph::angly;


// ****
// great we have a language for DFAs.  now test it by building a DFA for graphs

template<typename ...>
struct graph {};
template<typename ...>
struct node {};
template<typename ...>
struct edge {};

struct graph_nodes {}; struct node_name {}; struct node_edges {}; 
struct edge_name {}; struct edge_target {}; struct edge_done {};
struct graph_node {}; struct node_name_trans {}; struct node_edge {};
struct edge_name_trans {}; struct edge_target_trans {};

typedef angly::dfa<angly::state<graph_nodes, mpl::true_, mpl::vector<>,
                                angly::transition<graph_node, graph_nodes, 
                                                  angly::match_token<node<> >, node_name, 
                                                  mpl::push_back<mpl::_1, mpl::_2> > >,
                   angly::state<node_name, 
                                angly::transition<node_name_trans, node_edges,
                                                  mpl::always<mpl::true_>, void, 
                                                  mpl::pair<mpl::_2, mpl::vector<> > > >,
                   angly::state<node_edges, mpl::true_,
                                angly::transition<node_edge, node_edges,
                                                  angly::match_token<edge<> >, edge_name, 
                                                  mpl::pair<mpl::first<mpl::_1>, 
                                                            mpl::push_back<mpl::second<mpl::_1>, 
                                                                           mpl::_2> > > >,
                   angly::state<edge_name,
                                angly::transition<edge_name_trans, edge_target,
                                                  mpl::always<mpl::true_>, void, mpl::_2> >,
                   angly::state<edge_target,
                                angly::transition<edge_target_trans, edge_done,
                                                  mpl::always<mpl::true_>, void, 
                                                  mpl::pair<mpl::_1, mpl::_2> > >,
                   angly::state<edge_done, mpl::true_>
                   > graph_dfa_desc;

typedef typename angly::create_parser<graph_dfa_desc>::type graph_parser;

//boost::metagraph::angly::detail::print<mpl::second<graph_parser_and_stmap>::type> blah_blah;

struct A {}; struct B {}; struct C {}; struct D {}; struct E {}; struct F {}; struct G {};
struct t {}; struct u {}; struct v {}; struct w {}; struct x {}; struct y {}; struct z {};
struct A2 {}; struct B2 {}; struct C2 {}; struct D2 {}; struct E2 {}; struct F2 {}; struct G2 {};
struct t2 {}; struct u2 {}; struct v2 {}; struct w2 {}; struct x2 {}; struct y2 {}; struct z2 {};
struct A3 {}; struct B3 {}; struct C3 {}; struct D3 {}; struct E3 {}; struct F3 {}; struct G3 {};
struct t3 {}; struct u3 {}; struct v3 {}; struct w3 {}; struct x3 {}; struct y3 {}; struct z3 {};
struct A4 {}; struct B4 {}; struct C4 {}; struct D4 {}; struct E4 {}; struct F4 {}; struct G4 {};
struct t4 {}; struct u4 {}; struct v4 {}; struct w4 {}; struct x4 {}; struct y4 {}; struct z4 {};
struct A5 {}; struct B5 {}; struct C5 {}; struct D5 {}; struct E5 {}; struct F5 {}; struct G5 {};
struct t5 {}; struct u5 {}; struct v5 {}; struct w5 {}; struct x5 {}; struct y5 {}; struct z5 {};
struct A6 {}; struct B6 {}; struct C6 {}; struct D6 {}; struct E6 {}; struct F6 {}; struct G6 {};
struct t6 {}; struct u6 {}; struct v6 {}; struct w6 {}; struct x6 {}; struct y6 {}; struct z6 {};
struct A7 {}; struct B7 {}; struct C7 {}; struct D7 {}; struct E7 {}; struct F7 {}; struct G7 {};
struct t7 {}; struct u7 {}; struct v7 {}; struct w7 {}; struct x7 {}; struct y7 {}; struct z7 {};
struct A8 {}; struct B8 {}; struct C8 {}; struct D8 {}; struct E8 {}; struct F8 {}; struct G8 {};
struct t8 {}; struct u8 {}; struct v8 {}; struct w8 {}; struct x8 {}; struct y8 {}; struct z8 {};
struct A9 {}; struct B9 {}; struct C9 {}; struct D9 {}; struct E9 {}; struct F9 {}; struct G9 {};
struct t9 {}; struct u9 {}; struct v9 {}; struct w9 {}; struct x9 {}; struct y9 {}; struct z9 {};

/*
  A - t > B - v > D - x > F
    - u > C - w > E - y > F
                    - z > G
*/
typedef graph<node<A, edge<t, B>, edge<u, C> >,
              node<B, edge<v, D> >,
              node<C, edge<w, E> >,
              node<D, edge<x, F> >,
              node<E, edge<y, F>, edge<z, G> >

             ,node<A2, edge<t2, B2>, edge<u2, C2> >,
              node<B2, edge<v2, D2> >,
              node<C2, edge<w2, E2> >,
              node<D2, edge<x2, F2> >,
              node<E2, edge<y2, F2>, edge<z2, G2> >

             ,node<A3, edge<t3, B3>, edge<u3, C3> >,
              node<B3, edge<v3, D3> >,
              node<C3, edge<w3, E3> >,
              node<D3, edge<x3, F3> >,
              node<E3, edge<y3, F3>, edge<z3, G3> >

             ,node<A4, edge<t4, B4>, edge<u4, C4> >,
              node<B4, edge<v4, D4> >,
              node<C4, edge<w4, E4> >,
              node<D4, edge<x4, F4> >,
              node<E4, edge<y4, F4>, edge<z4, G4> >

             ,node<A5, edge<t5, B5>, edge<u5, C5> >,
              node<B5, edge<v5, D5> >,
              node<C5, edge<w5, E5> >,
              node<D5, edge<x5, F5> >,
              node<E5, edge<y5, F5>, edge<z5, G5> >

             ,node<A6, edge<t6, B6>, edge<u6, C6> >,
              node<B6, edge<v6, D6> >,
              node<C6, edge<w6, E6> >,
              node<D6, edge<x6, F6> >,
              node<E6, edge<y6, F6>, edge<z6, G6> >

             ,node<A7, edge<t7, B7>, edge<u7, C7> >,
              node<B7, edge<v7, D7> >,
              node<C7, edge<w7, E7> >,
              node<D7, edge<x7, F7> >,
              node<E7, edge<y7, F7>, edge<z7, G7> >

             ,node<A8, edge<t8, B8>, edge<u8, C8> >,
              node<B8, edge<v8, D8> >,
              node<C8, edge<w8, E8> >,
              node<D8, edge<x8, F8> >,
              node<E8, edge<y8, F8>, edge<z8, G8> >

             ,node<A9, edge<t9, B9>, edge<u9, C9> >,
              node<B9, edge<v9, D9> >,
              node<C9, edge<w9, E9> >,
              node<D9, edge<x9, F9> >,
              node<E9, edge<y9, F9>, edge<z9, G9> >
#if 0
#endif
> adjacencies;

typedef typename angly::run_dfa<typename mpl::first<graph_parser>::type, graph_nodes, adjacencies, mpl::at<typename mpl::second<graph_parser>::type, mpl::_1> >::type graph_data;
typedef typename mpl_graph::adjacency_list_graph<graph_data> parsed_graph;
struct preordering_visitor : mpl_graph::dfs_default_visitor_operations {    
    template<typename Node, typename Graph, typename State>
    struct discover_vertex :
        mpl::push_back<State, Node>
    {};
};
#if 0
// preordering, default start node (first)
typedef mpl::first<mpl_graph::
                   depth_first_search_all<parsed_graph,
                                      preordering_visitor, 
                                      mpl::vector<>,
                                      A>::type>::type preorder;

BOOST_MPL_ASSERT(( mpl::equal<preorder, mpl::vector<
                     A,B,D,F,C,E,G,
                     A2,B2,D2,F2,C2,E2,G2,
                     A3,B3,D3,F3,C3,E3,G3,
                     A4,B4,D4,F4,C4,E4,G4,
                     A5,B5,D5,F5,C5,E5,G5
> > ));
#endif
//boost::metagraph::angly::detail::print<preorder> x;


int main() {
  return 0;
}

