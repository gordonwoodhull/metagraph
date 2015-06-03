//  Copyright 2010 Gordon Woodhull. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_FUSION_GRAPH_HPP_INCLUDED
#define BOOST_METAGRAPH_FUSION_GRAPH_HPP_INCLUDED

// fusion_graph - simple implementation of a 
// heterogeneous typed graph data structure

#include <boost/metagraph/mpl_graph/mpl_graph.hpp>

#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/insert.hpp>
#include <boost/fusion/include/as_map.hpp>
#include <boost/fusion/include/mpl.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/mpl/joint_view.hpp>

#include <list>


namespace boost {
namespace metagraph {
namespace fusion_graph {

// input: an mpl_graph of vertex-types and edge-relations annotated with
// instructions on the web of types-that-refer-to-each-other to generate

// note a fusion_graph's nodes and edges are all different types, although
// there may be many instances of a particular type.  as a particular example,
// one might specify a minimal graph adt as a fusion_graph of three types G, N, and E
// G -> N <-> E  "A graph contains containers of pointers to nodes, which contain
// containers of pointers to edges, which contain pointers to nodes."

// output: a type generator for the requested interlinked types  - look up 
// the implementation using tags of input graph with fig::vertex_impl<vertex_tag>::type
// or fig::edge_impl<edge_tag>::type.  note edge data is all stored in source nodes
// and accessed using fig::access_edge<edge_tag>(vertex)

// this should all be abstracted better (another level is planned), but here's a start
template<typename InputGraph>
struct make_fusion_graph {
    struct type { 
        template<typename VertexTag> struct vertex_impl;
        template<typename EdgeTag>
        struct edge_impl :
            EdgeTag::template link_container<typename vertex_impl<typename mpl_graph::target<EdgeTag,InputGraph>::type>::type>
        {};
            
        template<typename VertexTag>
        struct vertex_impl {
            struct type {
                typedef typename mpl::transform<typename mpl_graph::out_edges<VertexTag,InputGraph>::type,
                                                fusion::pair<mpl::_1,
                                                             edge_impl<mpl::_1> >
                                               >::type tag_n_impl_sequence;
                typedef typename mpl::joint_view<tag_n_impl_sequence,typename VertexTag::data_type>::type all_vertex_data;
                typedef typename boost::fusion::result_of::as_map<all_vertex_data>::type
                                    data_type;
                data_type data;
            };
        };
    };
};

// some vertex and edge data specification classes

template<typename Data>
struct vertex_data {
    typedef Data data_type; // a sequence of tag/data fusion pairs to be concatenated with the edge data
};

// tells fusion_graph to use a pointer to store zero/one link in edge
struct ptr_edge {	
    template<typename VertexImpl>
    struct link_container {
        typedef VertexImpl* type;
    };
};
// tells fusion_graph to use a list to store zero or more links in edge
struct ptr_list_edge {
    template<typename VertexImpl>
    struct link_container {
        typedef std::list<VertexImpl*> type;
    };
};
// etc.  special provision will be made for intrusive containers and their
// magical polymorphic qualities in the next level...


} // fusion_graph
} // metagraoh
} // boost

#endif // BOOST_METAGRAPH_FUSION_GRAPH_HPP_INCLUDED
