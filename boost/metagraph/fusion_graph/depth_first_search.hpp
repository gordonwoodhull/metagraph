//  Copyright 2010 Gordon Woodhull. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_FUSION_GRAPH_DEPTH_FIRST_SEARCH_HPP_INCLUDED
#define BOOST_METAGRAPH_FUSION_GRAPH_DEPTH_FIRST_SEARCH_HPP_INCLUDED

namespace boost {
namespace metagraph {
namespace fusion_graph {
 
// default operations for writing a fusion_graph dfs visitor

// state of the visitor could also be polymorphic 
// (built up as the search progresses)
// then each member would also be polymorphic, 
// returning the new state of whatever computed type

void dfs_default_visitor_operations {
    template<typename Vertex, typename Graph>
    void initialize_vertex(Vertex &v, Graph &g) {}
    
    template<typename Vertex, typename Graph>
    void discover_vertex(Vertex &v, Graph &g) {}
    
    template<typename Vertex, typename Graph>
    void finish_vertex(Vertex &v, Graph &g) {}
        
    template<typename Edge, typename Graph>
    void tree_edge(Edge &e, Graph &g) {}
    
    template<typename Edge, typename Graph>
    void back_edge(Edge &e, Graph &g) {}
    
    template<typename Edge, typename Graph>
    void forward_or_cross_edge(Edge &e, Graph &g) {}  
};
// simplification of depth_first_search for trees: no color map needed, and back_edge & forward_or_cross_edge will not 
template<typename Graph, typename DFSVisitor, typename Vertex>
void depth_first_search_tree(const Graph& g, DFSVisitor vis, Vertex &v);
        
template<typename Graph, typename DFSVisitor, typename Edge>
void depth_first_search_tree_edge(const Graph& g, DFSVisitor vis, Edge &e)
{
    vis.tree_edge(e, g);
    depth_first_search_tree(g, vis, target(e));
};

// simplification of depth_first_search for trees: no color map needed, and back_edge & forward_or_cross_edge will not 
template<typename Graph, typename DFSVisitor, typename Vertex>
void depth_first_search_tree(const Graph& g, DFSVisitor vis, Vertex &v) {
    using namespace boost::phoenix::arg_names;
    vis.start_vertex(v);
    fusion::for_each(out_edges(v), phoenix::bind(depth_first_search_tree_edge, g, vis, arg1));
}

}
}
}

#endif //BOOST_METAGRAPH_FUSION_GRAPH_DEPTH_FIRST_SEARCH_HPP_INCLUDED
