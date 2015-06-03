// Copyright 2008-2010 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/metagraph/fusion_graph/fusion_graph.hpp>
#include <boost/metagraph/mpl_graph/incidence_list_graph.hpp>

namespace fusion_graph = boost::metagraph::fusion_graph;
namespace fusion = boost::fusion;
namespace mpl_graph = boost::metagraph::mpl_graph;
namespace mpl = boost::mpl;

struct Happy {}; // sample data tag

struct G : fusion_graph::vertex_data<mpl::vector<> > {};
struct N : fusion_graph::vertex_data<mpl::vector<fusion::pair<Happy,bool> > > {};
struct E : fusion_graph::vertex_data<mpl::vector<> > {};

struct G_N : fusion_graph::ptr_list_edge {};
struct N_E : fusion_graph::ptr_list_edge {};
struct E_N : fusion_graph::ptr_edge {};
struct E_T : E_N {};
struct E_S : E_N {};

struct graph_desc : 
    mpl::vector<
        mpl::vector<G_N,G,N>,
        mpl::vector<N_E,N,E>,
        mpl::vector<E_T,E,N>,
        mpl::vector<E_S,E,N> 
        >
{};
struct graph_desc_graph :
    mpl_graph::incidence_list_graph<graph_desc>
{};

typedef fusion_graph::make_fusion_graph<graph_desc_graph>::type graphy;

typedef graphy::vertex_impl<G>::type Graph;
typedef graphy::vertex_impl<N>::type Node;
typedef graphy::vertex_impl<E>::type Edge;

int main(int narg, char *args[]) {
    Graph *g = new Graph;
    Node *n1 = new Node, *n2 = new Node;
    Edge *e1 = new Edge;
    
    fusion::at_key<Happy>(n1->data) = true;
    
    fusion::at_key<G_N>(g->data).push_back(n1);
    fusion::at_key<G_N>(g->data).push_back(n2);
    fusion::at_key<N_E>(n1->data).push_back(e1);
    fusion::at_key<E_S>(e1->data) = n1;
    fusion::at_key<E_T>(e1->data) = n2;
    
    // NO (generates horribly incomprehensible messages
    //fusion::at_key<N_E>(g->edges).push_back(n1);

}