// Copyright 2012 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_MPL_PRIORITY_SEARCH_QUEUE_HPP_INCLUDED
#define BOOST_METAGRAPH_MPL_PRIORITY_SEARCH_QUEUE_HPP_INCLUDED

// translated from Ralf Hinze / Scott Dillard PSQueue.hs

namespace boost {
  namespace metagraph {
    namespace mpl_graph {

      template<typename K, typename P>
      struct binding {
        typedef K key;
        typedef P prio;
      };

      template<typename Binding>
      struct get_key {
        typedef typename Binding::key type;
      };

      template<typename Binding>
      struct get_prio {
        typedef typename Binding::prio type;
      };

      template<typename Size, typename Key, typename Prio, typename Left, typename Split, typename Right>
      struct node {
        typedef Size size;
        typedef Key key;
        typedef Prio prio;
        typedef Left left;
        typedef Split split;
        typedef Right right;
      };

      template<typename Binding>
      struct get_size {
        typedef typename Binding::size type;
      };

      template<typename Binding>
      struct get_left {
        typedef typename Binding::left type;
      };

      template<typename Binding>
      struct get_split {
        typedef typename Binding::split type;
      };

      template<typename Binding>
      struct get_right {
        typedef typename Binding::right type;
      };

      template<typename Key, typename Queue>
      struct lookup
    

    } // namespace mpl_graph
  } // namespace metagraph
} // namespace boost

#endif // BOOST_METAGRAPH_MPL_PRIORITY_SEARCH_QUEUE_HPP_INCLUDED
