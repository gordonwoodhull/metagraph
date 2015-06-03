//  Copyright 2010 Gordon Woodhull. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_PROTO_FUSION_GRAPH_ADAPTOR_HPP_INCLUDED
#define BOOST_METAGRAPH_PROTO_FUSION_GRAPH_ADAPTOR_HPP_INCLUDED

// Example of adapting Proto trees into a Fusion/BGL hybrid graph interface
// allowing traversal of the tree using generic tree & graph algorithms

#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>

#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/include/make_vector.hpp>

#include <boost/metagraph/proto_graph/proto_graph.hpp>
#include <boost/metagraph/fusion_graph/fusion_graph_result_of_sigs.hpp>

#include <boost/spirit/home/phoenix/core.hpp>
#include <boost/spirit/home/phoenix/function.hpp>
#include <boost/spirit/home/phoenix/bind.hpp>

namespace boost {
namespace metagraph {
namespace fusion_graph {
    
namespace result_of {
        
template<typename Edge>
struct source<Edge, proto_graph::ProtoGraph> 
    : fusion::result_of::second<Edge>
{};

template<typename Edge>
struct target<Edge, proto_graph::ProtoGraph> 
    : proto::result_of::child_c<typename Edge::second_type, Edge::first_type::value>
{};

// yes i'm aware that this would better return a lazily-generated fusion sequence,
// because it's duplicating the same reference A times.  just a first try..
template<typename Vertex>
struct out_edges<Vertex, proto_graph::ProtoGraph> :
    fusion::result_of::make_vector<
        typename mpl::fold<mpl::range_c<long,0,proto::arity_of<Vertex>::value>,
                  mpl::vector<>,
                  mpl::push_back<mpl::_1, fusion::pair<mpl::_2, Vertex&> > >::type >
{};

}
    
// i know, i am probably not dealing with references right 
template<typename Edge>
typename result_of::source<Edge, proto_graph::ProtoGraph>::type &source(Edge &e, proto_graph::ProtoGraph &) {
    return e.second;
}
template<typename Edge>
typename result_of::target<Edge, proto_graph::ProtoGraph>::type &target(Edge &e, proto_graph::ProtoGraph &) {
    return proto::child_c<Edge::first_type::value>(e);
}
// hopefully not necessary - just couldn't figure out how else to extract the type from fusion::fold's _2 
template<typename Vertex>
struct edge_pair {
    Vertex &v;
    edge_pair(Vertex &v) : v(v) {}
    
    template<typename> struct result;
    
    template<class Self, typename I>
    struct result<Self(I)> : fusion::pair<I,Vertex> {};
    
    template<typename I>
    fusion::pair<I,Vertex> operator()(I) {
        return fusion::make_pair<I>(v);
    }
};
template<typename Vertex>
typename result_of::out_edges<Vertex, proto_graph::ProtoGraph>::type out_edges(Vertex &v, proto_graph::ProtoGraph &) {
    using namespace boost::phoenix::arg_names;
    return fusion::fold(mpl::range_c<long,0,proto::arity_of<Vertex>::value>(), 
            fusion::vector<>(), 
            phoenix::bind(fusion::push_back, arg1, phoenix::bind(edge_pair<Vertex>(v), arg2)));
}

}
}
}

#endif // BOOST_METAGRAPH_PROTO_FUSION_GRAPH_ADAPTOR_HPP_INCLUDED
