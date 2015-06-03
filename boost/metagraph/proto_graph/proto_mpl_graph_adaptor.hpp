//  Copyright 2010 Gordon Woodhull. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_PROTO_MPL_GRAPH_ADAPTOR_HPP_INCLUDED
#define BOOST_METAGRAPH_PROTO_MPL_GRAPH_ADAPTOR_HPP_INCLUDED

// implement the MPGL IncidenceGraph concept on the Proto API
// Vertex is Expr; Edge is mpl::pair<Expr, ChildNum>
// Graph is unused

#include <boost/proto/core.hpp>

#include <boost/mpl/pair.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/typeof/typeof.hpp>

#include <boost/metagraph/proto_graph/proto_graph.hpp>

namespace boost {
namespace metagraph {
namespace mpl_graph {    
    
// this probably is not the most awesome way to specialize the mpgl metafunctions
// mpl_graph.hpp should probably be opened up to rely more on the representation tag
// instead of assuming that every impl will have giant maps in it

template<typename Edge>
struct source<Edge, proto_graph::ProtoGraph> : 
    mpl::first<Edge> 
{};
template<typename Edge>
struct target<Edge, proto_graph::ProtoGraph> : 
    proto::result_of::child_c<typename mpl::first<Edge>::type, mpl::second<Edge>::type::value> 
{};
template<typename Vertex>
struct out_edges<Vertex, proto_graph::ProtoGraph> :
    mpl::fold<mpl::range_c<long,0,proto::arity_of<Vertex>::value>,
         mpl::vector<>,
         mpl::push_back<mpl::_1, mpl::pair<Vertex, mpl::_2> > >
{};
template<typename Vertex>
struct out_degree<Vertex, proto_graph::ProtoGraph> : 
    proto::arity_of<Vertex>
{};
   
}
}     
}

#endif // BOOST_METAGRAPH_PROTO_MPL_GRAPH_ADAPTOR_HPP_INCLUDED
