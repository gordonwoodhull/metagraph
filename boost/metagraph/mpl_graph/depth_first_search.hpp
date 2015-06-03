// Copyright 2008-2010 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_MPL_GRAPH_DEPTH_FIRST_SEARCH_HPP_INCLUDED
#define BOOST_METAGRAPH_MPL_GRAPH_DEPTH_FIRST_SEARCH_HPP_INCLUDED

#include <boost/metagraph/mpl_graph/mpl_graph.hpp>

#include <boost/mpl/has_key.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/quote.hpp>
#include <boost/mpl/bool.hpp>

#include "search_colors.hpp"

namespace boost {
  namespace metagraph {
    namespace mpl_graph {

      // dfs takes a visitor which has all the bgl-like metafunctions encapsulated in an 
      // "operations" member class, and also a state.  the operations are expected to return a new state
      // in addition, the visitor operations are expected to update the colors of vertices
      // and need to support a new metafunction get_color<Vertex, State>

      struct dfs_default_visitor_operations {
        template<typename Vertex, typename Graph, typename State>
        struct initialize_vertex {
          typedef State type;       
        };
    
        template<typename Vertex, typename Graph, typename State>
        struct discover_vertex {
          typedef State type;       
        };
    
        template<typename Vertex, typename Graph, typename State>
        struct finish_vertex {
          typedef State type;       
        };
        
        template<typename Edge, typename Graph, typename State>
        struct tree_edge {
          typedef State type;
        };
    
        template<typename Edge, typename Graph, typename State>
        struct back_edge {
          typedef State type;
        };
    
        template<typename Edge, typename Graph, typename State>
        struct forward_or_cross_edge {
          typedef State type;
        };  
      };

      namespace detail {
        template<typename Item, typename IsVertex, typename IsNew>
        struct dfs_frame {
          typedef Item item;
          typedef IsVertex is_vertex;
          typedef IsNew is_new;
        };
        template<typename Item, typename IsVertex, typename IsNew>
        struct make_dfs_frame {
          typedef dfs_frame<Item, IsVertex, IsNew> type; // surely there's an easier way to do this?
        };

        template<typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap, typename Vertex>
        struct dfs_step_in_vertex {
          // enter vertex
          typedef typename VisitorOps::template 
          discover_vertex<Vertex, Graph, VisitorState>::type 
          discovered_state;
          // color it
          typedef typename search_color_map_ops::template 
          set_color<Vertex, search_colors::Gray, ColorMap>::type 
          discovered_colors;

          // put exit frame on stack
          typedef typename mpl::push_front<Stack, dfs_frame<Vertex, mpl::true_, mpl::false_> >::type stack;
            
          // loop over out edges
          typedef mpl::vector<typename 
                              mpl::reverse_fold<typename 
                                        mpl_graph::out_edges<Vertex, Graph>::type, 
                                        stack,
                                        mpl::push_front<mpl::_1, make_dfs_frame<mpl::_2, mpl::false_, mpl::false_> > >::type, 
                              discovered_state, discovered_colors> type;
        };
        template<typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap, typename Vertex>
        struct dfs_step_out_vertex {
          typedef mpl::vector<Stack,
                              typename VisitorOps::template finish_vertex<Vertex, Graph, VisitorState>::type,
                              typename search_color_map_ops::template set_color<Vertex, search_colors::Black, ColorMap>::type> type;
        };
        template<typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap, typename Edge, typename Target>
        struct dfs_unseen_target :
          mpl::vector<typename mpl::push_front<Stack, dfs_frame<Target, mpl::true_, mpl::true_> >::type,
                      typename VisitorOps::template tree_edge<Edge, Graph, VisitorState >::type, 
                      ColorMap> {};
        template<typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap, typename Edge, typename Color>
        struct dfs_seen_target :
          mpl::vector<Stack,
                      typename
                      mpl::eval_if<typename boost::is_same<Color, search_colors::Gray>::type,
                                   typename VisitorOps::template back_edge<Edge, Graph, VisitorState >,
                                   typename VisitorOps::template forward_or_cross_edge<Edge, Graph, VisitorState > // Black
                                   >::type, 
                      ColorMap> {};
        template<typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap, typename Edge>
        struct dfs_step_edge {
          typedef typename mpl_graph::target<Edge, Graph>::type target;
          typedef typename search_color_map_ops::get_color<target, ColorMap >::type color;
          typedef typename 
          mpl::eval_if<typename boost::is_same<color, search_colors::White>::type,
                       // unseen target: put it on stack
                       dfs_unseen_target<Graph, VisitorOps, Stack, VisitorState, ColorMap, Edge, target>,
                       // seen: back or forward edge
                       dfs_seen_target<Graph, VisitorOps, Stack, VisitorState, ColorMap, Edge, color>
                        >::type type;
        };
                                     
          
        template<typename T> struct err_it;
         
        template<typename Frame, typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap>
        struct dispatch_frame;


        template<typename Item, typename Graph, typename VisitorOps, typename Stack,
               typename VisitorState, typename ColorMap>
        struct dispatch_frame<dfs_frame<Item, mpl::true_, mpl::true_>, 
                              Graph, VisitorOps, Stack,
                              VisitorState, ColorMap> :
        dfs_step_in_vertex<Graph, VisitorOps, Stack,
                           VisitorState, ColorMap, Item> {};
 

        template<typename Item, typename Graph, typename VisitorOps, typename Stack,
               typename VisitorState, typename ColorMap>
        struct dispatch_frame<dfs_frame<Item, mpl::true_, mpl::false_>, 
                              Graph, VisitorOps, Stack,
                              VisitorState, ColorMap> :
        dfs_step_out_vertex<Graph, VisitorOps, Stack,
                            VisitorState, ColorMap, Item> {};
        

        template<typename Item, typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap, typename Unused>
        struct dispatch_frame<dfs_frame<Item, mpl::false_, Unused>, 
                              Graph, VisitorOps, Stack,
                              VisitorState, ColorMap> :
        dfs_step_edge<Graph, VisitorOps, Stack,
                      VisitorState, ColorMap, Item> {};
 
        template<typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap>
        struct dfs_step {
          typedef typename mpl::pop_front<Stack>::type stack;
          typedef typename mpl::front<Stack>::type frame;
          //typedef typename err_it<frame>::type foo;
          typedef typename dispatch_frame<frame, Graph, VisitorOps, stack, VisitorState, ColorMap>::type type;
        };
        
        struct dfs_end {};
        template<typename Graph, typename VisitorOps, typename Stack,
                 typename VisitorState, typename ColorMap>
        struct dfs_iter {
          typedef typename dfs_step<Graph, VisitorOps, Stack, VisitorState, ColorMap>::type eval;
          typedef typename mpl::at_c<eval, 0>::type stack;
          typedef typename mpl::at_c<eval, 1>::type visitor_state;
          typedef typename mpl::at_c<eval, 2>::type color_map;

          typedef typename mpl::if_<typename mpl::empty<stack>::type,
                                    dfs_end,
                                    dfs_iter<Graph, VisitorOps, stack, visitor_state, color_map> >::type next;
          typedef mpl::pair<visitor_state, color_map> type;
        };
        template<typename BeginIter>
        struct dfs_sequence {
          typedef BeginIter begin;
          typedef dfs_end end;
        };
      }
      // requires IncidenceGraph
      // returns pair<VisitorState, ColorMap>
      template<typename Graph, typename VisitorOps, typename VisitorState,
               typename Vertex, 
               typename ColorMap = create_search_color_map::type >
      struct depth_first_search {
        typedef mpl::vector<detail::dfs_frame<Vertex, mpl::true_, mpl::true_> > stack;
        typedef typename mpl::fold<detail::dfs_sequence<detail::dfs_iter<Graph, VisitorOps, stack, VisitorState, ColorMap> >,
                                   void,
                                   mpl::_2>::type type;
      };

      // requires IncidenceGraph, VertexListGraph
      template<typename Graph, typename VisitorOps, typename VisitorState,
               typename FirstVertex = typename mpl::front<typename mpl_graph::vertices<Graph>::type>::type,
               typename ColorMap = create_search_color_map::type>
      struct depth_first_search_all : // visit first then rest
      mpl::fold<typename mpl_graph::vertices<Graph>::type,
                typename depth_first_search<Graph, 
                                            VisitorOps, VisitorState, 
                                            FirstVertex, 
                                            ColorMap>::type, 
                mpl::if_<boost::is_same<search_color_map_ops::get_color<mpl::_2, mpl::second<mpl::_1> >,
                                        search_colors::White>, // visit any yet unvisited
                         depth_first_search<Graph,
                                            VisitorOps, mpl::first<mpl::_1>,
                                            mpl::_2,
                                            mpl::second<mpl::_1> >,
                         mpl::_1> >   
      {};

    } // namespace mpl_graph
  } // namespace metagraph
} // namespace boost


#endif // BOOST_METAGRAPH_MPL_GRAPH_DEPTH_FIRST_SEARCH_HPP_INCLUDED
