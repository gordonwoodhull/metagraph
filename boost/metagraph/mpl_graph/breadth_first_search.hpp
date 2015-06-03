// Copyright 2008-2010 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_MPL_GRAPH_BREADTH_FIRST_SEARCH_HPP_INCLUDED
#define BOOST_METAGRAPH_MPL_GRAPH_BREADTH_FIRST_SEARCH_HPP_INCLUDED

#include <boost/metagraph/mpl_graph/mpl_graph.hpp>

#include <boost/mpl/has_key.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/remove.hpp>

#include "search_colors.hpp"

namespace boost {
  namespace metagraph {
    namespace mpl_graph {

      // bfs takes a visitor which has all the bgl-like metafunctions encapsulated in an 
      // "operations" member class, and also a state.  the operations are expected to return a new state
      struct bfs_default_visitor_operations {
        template<typename Vertex, typename Graph, typename State>
        struct initialize_vertex {
          typedef State type;       
        };
    
        template<typename Vertex, typename Graph, typename State>
        struct discover_vertex {
          typedef State type;       
        };
    
        template<typename Vertex, typename Graph, typename State>
        struct examine_vertex {
          typedef State type;       
        };
    
        template<typename Vertex, typename Graph, typename State>
        struct examine_edge {
          typedef State type;       
        };
        
        template<typename Edge, typename Graph, typename State>
        struct tree_edge {
          typedef State type;
        };
    
        template<typename Edge, typename Graph, typename State>
        struct non_tree_edge {
          typedef State type;
        };
    
        template<typename Edge, typename Graph, typename State>
        struct gray_target {
          typedef State type;
        };  
    
        template<typename Edge, typename Graph, typename State>
        struct black_target {
          typedef State type;
        };  
    
        template<typename Vertex, typename Graph, typename State>
        struct finish_vertex {
          typedef State type;       
        };
      };

      namespace detail {

        template<typename Graph, typename VisitorOps, typename VCQState, typename Edge>
        struct bfs_run_queue_examine_edge {
          typedef typename VisitorOps::template examine_edge<Edge, Graph, typename mpl::at_c<VCQState, 0>::type>::type visitor_state;
          typedef typename mpl::at_c<VCQState, 1>::type color_state;
          typedef typename mpl::at_c<VCQState, 2>::type vertex_queue;
          typedef typename mpl_graph::target<Edge, Graph>::type target;
          typedef typename search_color_map_ops::template get_color<target, color_state>::type color;

          typedef typename 
          mpl::if_<typename boost::is_same<color, search_colors::White>::type,
                   // unseen target: tree edge, discover target, paint it gray, and enqueue
                   mpl::vector<typename VisitorOps::template 
                               discover_vertex<target, Graph,
                                               typename VisitorOps::template 
                                               tree_edge<Edge, Graph, visitor_state>::type>::type,
                               typename search_color_map_ops::template 
                               set_color<target, search_colors::Gray, color_state>::type,
                               typename mpl::push_back<vertex_queue, target >::type >,
                   // seen
                   mpl::vector<typename mpl::if_<typename boost::is_same<color, search_colors::Gray>::type,
                                                 typename VisitorOps::template 
                                                 gray_target<Edge, Graph, visitor_state>::type,
                                                 typename VisitorOps::template 
                                                 black_target<Edge, Graph, visitor_state>::type>::type,
                               color_state,
                               vertex_queue> 
                   >::type type; 
        };
        
        // processes the front vertex from the queue
        // returns <new_queue, visitor_state, color_state>
        template<typename Graph, typename VertexQueue, typename VisitorOps, typename VisitorState, typename ColorMap>
        struct bfs_process_node {
          // enter vertex
          typedef typename mpl::front<VertexQueue>::type Vertex;
          typedef typename mpl::pop_front<VertexQueue>::type Tail;
          typedef typename VisitorOps::template examine_vertex<Vertex, Graph, VisitorState>::type examined_state;
    
          // loop over out edges
          typedef typename mpl::template 
          fold<typename mpl_graph::out_edges<Vertex, Graph>::type, 
               mpl::vector<examined_state, ColorMap, Tail>,
               bfs_run_queue_examine_edge<Graph, VisitorOps, mpl::_1, mpl::_2>
               >::type did_edges;
            
          typedef typename VisitorOps::template 
          finish_vertex<Vertex, Graph, typename mpl::at_c<did_edges, 0>::type>::type 
          finished_vertex; 
          // does map insert always overwrite?  i seem to remember this not working on msvc once
          typedef typename search_color_map_ops::template 
          set_color<Vertex, search_colors::Black, typename mpl::at_c<did_edges, 1>::type>::type 
          colored_vertex;
          typedef typename mpl::at_c<did_edges, 2>::type queued_targets;

          typedef mpl::vector<queued_targets, finished_vertex, colored_vertex> type;
        };
        
        struct bfs_end {};
        template<typename Graph, typename VertexQueue, typename VisitorOps, typename VisitorState, typename ColorMap>
        struct bfs_iter {
          typedef typename bfs_process_node<Graph, VertexQueue, VisitorOps, VisitorState, ColorMap>::type eval;
          typedef typename mpl::at_c<eval, 0>::type queue;
          typedef typename mpl::at_c<eval, 1>::type visitor_state;
          typedef typename mpl::at_c<eval, 2>::type color_map;
          
          typedef typename mpl::if_<typename mpl::empty<queue>::type,
                           bfs_end,
                           bfs_iter<Graph, queue, VisitorOps, visitor_state, color_map> >::type next;
          typedef mpl::pair<visitor_state, color_map> type;
        };
          
        template<typename BeginIter>
        struct bfs_sequence {
          typedef BeginIter begin;
          typedef bfs_end end;
        };
    } // namespace detail

    template<typename Graph, typename VisitorOps, typename VisitorState, 
             typename Vertex, 
             typename ColorMap = create_search_color_map::type >
    struct breadth_first_search {
      typedef typename VisitorOps::template 
      discover_vertex<Vertex, Graph, VisitorState>::type 
      discovered_state;
      typedef typename search_color_map_ops::template 
      set_color<Vertex, search_colors::Gray, ColorMap>::type 
      discovered_colors;
      typedef typename mpl::fold<detail::bfs_sequence<detail::bfs_iter<Graph, mpl::vector<Vertex>, VisitorOps, discovered_state, discovered_colors> >,
                                 void,
                                 mpl::_2>::type type;
};

template<typename Graph, typename VisitorOps, typename VisitorState,
         typename FirstVertex = typename mpl::front<typename mpl_graph::vertices<Graph>::type>::type,
         typename ColorMap = create_search_color_map::type>
struct breadth_first_search_all : // visit "first" first, then visit any still white
        mpl::fold<typename mpl_graph::vertices<Graph>::type,
                  typename breadth_first_search<Graph, VisitorOps, VisitorState, FirstVertex, ColorMap>::type,
                  mpl::if_<boost::is_same<search_color_map_ops::template get_color<mpl::_2, mpl::second<mpl::_1> >,
                                          search_colors::White>,
                           breadth_first_search<Graph, VisitorOps, mpl::first<mpl::_1>,
                                                mpl::_2, mpl::second<mpl::_1> >,
                           mpl::_1> >   
{};

    } // namespace mpl_graph
  } // namespace metagraph
} // namespace boost


#endif // BOOST_METAGRAPH_MPL_GRAPH_BREADTH_FIRST_SEARCH_HPP_INCLUDED
