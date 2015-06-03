//  Copyright 2010 Gordon Woodhull. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_PROTO_GRAPH_HPP_INCLUDED
#define BOOST_METAGRAPH_PROTO_GRAPH_HPP_INCLUDED

// dummy type telling mpl_graph and fusion_graph to use these specializations
// neither needs a graph object/type to implement the graph concept(s)

namespace boost {
namespace metagraph {
namespace proto_graph {    

struct ProtoGraph {};

}
}
}

#endif // BOOST_METAGRAPH_PROTO_GRAPH_HPP_INCLUDED
