// Copyright 2011 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

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

/*
  A - t > B - v > D - x > F
    - u > C - w > E - y > F
                    - z > G
*/
typedef graph<node<A, edge<t, B>, edge<u, C> >,
              node<B, edge<v, D> >,
              node<C, edge<w, E> >,
              node<D, edge<x, F> >,
              node<E, edge<y, F>, edge<z, G> > > adjacencies;

typedef typename angly::run_dfa<typename mpl::first<graph_parser>::type, graph_nodes, adjacencies, mpl::at<typename mpl::second<graph_parser>::type, mpl::_1> >::type graph_data;
typedef typename mpl_graph::adjacency_list_graph<graph_data> parsed_graph;
struct preordering_visitor : mpl_graph::dfs_default_visitor_operations {    
    template<typename Node, typename Graph, typename State>
    struct discover_vertex :
        mpl::push_back<State, Node>
    {};
};

// preordering, default start node (first)
typedef mpl::first<mpl_graph::
                   depth_first_search<parsed_graph,
                                      preordering_visitor, 
                                      mpl::vector<>,
                                      A>::type>::type preorder;

BOOST_MPL_ASSERT(( mpl::equal<preorder, mpl::vector<A,B,D,F,C,E,G> > ));

//boost::metagraph::angly::detail::print<preorder> x;


int main() {
  return 0;
}

