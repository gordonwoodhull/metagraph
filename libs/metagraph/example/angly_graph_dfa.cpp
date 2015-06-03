// Copyright 2011 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <boost/msm/mpl_graph/adjacency_list_graph.hpp>
#include <boost/msm/mpl_graph/depth_first_search.hpp>
#include <boost/metagraph/angly/dfa.hpp>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/identity.hpp>

#include <boost/units/detail/utility.hpp>

namespace mpl = boost::mpl;
namespace mpl_graph = boost::msm::mpl_graph;
namespace angly = boost::metagraph::angly;
template<typename ...>
struct graph {};
template<typename ...>
struct node {};
template<typename ...>
struct edge {};
//bogus
template<typename ...>
struct foo {};

// parser for metadata text of adjacency_list format
// graph< node< node_tag, edge< edge_tag, target_node_tag >, ...> >
// this an example of a graph language parser, not realistic
// define types for states
struct graph_nodes : angly::dfa_state<mpl::true_, mpl::vector<> > {};
struct node_name : angly::dfa_state<> {};
struct node_edges : angly::dfa_state<mpl::true_> {};
struct edge_name : angly::dfa_state<> {};
struct edge_target : angly::dfa_state<> {};
struct edge_done : angly::dfa_state<mpl::true_> {};
// types for transitions
struct graph_node : 
  angly::dfa_transition<angly::match_token<node<> >, 
                           node_name, 
                           mpl::push_back<mpl::_1, mpl::_2> > {}; 
struct node_name_trans : 
  angly::dfa_transition<mpl::always<mpl::true_>, 
                           void, 
                           mpl::pair<mpl::_2, mpl::vector<> > > {};
struct node_edge : 
  angly::dfa_transition<angly::match_token<edge<> >, 
                           edge_name, 
                           mpl::pair<mpl::first<mpl::_1>, mpl::push_back<mpl::second<mpl::_1>, mpl::_2> > > {}; 
struct edge_name_trans : 
  angly::dfa_transition<mpl::always<mpl::true_>, 
                           void, 
                           mpl::_2> {};
struct edge_target_trans : 
  angly::dfa_transition<mpl::always<mpl::true_>, 
                           void, 
                           mpl::pair<mpl::_1, mpl::_2> > {};

typedef mpl_graph::adjacency_list_graph<
  mpl::vector<mpl::pair<graph_nodes, 
                        mpl::vector<mpl::pair<graph_node, graph_nodes> > >,
              mpl::pair<node_name, 
                        mpl::vector<mpl::pair<node_name_trans, node_edges> > >,
              mpl::pair<node_edges, 
                        mpl::vector<mpl::pair<node_edge, node_edges> > >,
              mpl::pair<edge_name,
                        mpl::vector<mpl::pair<edge_name_trans, edge_target> > >,
              mpl::pair<edge_target,
                        mpl::vector<mpl::pair<edge_target_trans, edge_done> > >
              > > graph_dfa;



typedef graph<node<edge<>,edge<graph<> > >, edge<node<> >, edge<> > nonsense;
//typedef typename angly::run_dfa<graph_dfa, nonsense, void>::type fail;

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

typedef typename angly::run_dfa<graph_dfa, graph_nodes, adjacencies>::type graph_data;
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

