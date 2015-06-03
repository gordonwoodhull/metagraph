// Copyright 2012 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_MPL_BINOMIAL_HEAP_HPP_INCLUDED
#define BOOST_METAGRAPH_MPL_BINOMIAL_HEAP_HPP_INCLUDED

// adapted from Chris Okasaki's Purely Functional Data Structures

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/less_equal.hpp>
#include <boost/mpl/less.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/next_prior.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/reverse.hpp>

namespace boost {
  namespace metagraph {
    namespace mpl_graph {
      
      template<typename Rank, typename Element, typename Children>
      struct node {
        typedef Rank rank;
        typedef Element element;
        typedef Children children;
      };

      template<typename Node>
      struct rank {
        typedef typename Node::rank type;
      };
      template<typename Node>
      struct root {
        typedef typename Node::element type;
      };
      template<typename Node>
      struct children {
        typedef typename Node::children type;
      };
      
      template<typename Node1, typename Node2>
      struct link : // assumes elements are integral constant wrappers
        mpl::if_<typename mpl::less_equal<typename root<Node1>::type, 
                                          typename root<Node2>::type>::type, 
                 node<typename mpl::next<typename rank<Node1>::type>::type,
                      typename root<Node1>::type,
                      typename mpl::push_front<typename children<Node1>::type,
                                               Node2>::type >, 
                 node<typename mpl::next<typename rank<Node1>::type>::type,
                      typename root<Node2>::type,
                      typename mpl::push_front<typename children<Node2>::type,
                                               Node1>::type > > {};

      template<typename Tree, typename Heap> struct ins_tree;

      template<typename Tree, typename Heap>
      struct ins_tree_helper :
        mpl::eval_if<typename mpl::less<typename rank<Tree>::type,
                                        typename rank<typename mpl::front<Heap>::type>::type>,
                     mpl::push_front<Heap, Tree>,
                     ins_tree<typename link<Tree, typename mpl::front<Heap>::type>::type,
                              typename mpl::pop_front<Heap>::type> > {};

      template<typename Tree, typename Heap>
      struct ins_tree :
        mpl::eval_if<typename mpl::empty<Heap>::type,
                     mpl::push_front<Heap, Tree>,
                     ins_tree_helper<Tree, Heap> > {};

      template<typename Element, typename Heap>
      struct insert : ins_tree<node<mpl::int_<0>, Element, mpl::list<> >, Heap> {};

      template<typename Heap1, typename Heap2> struct merge;

      template<typename Heap1, typename Heap2>
      struct merge_helper {
        typedef typename mpl::front<Heap1>::type head1;
        typedef typename mpl::front<Heap2>::type head2;
        typedef typename mpl::eval_if<mpl::less<typename rank<head1>::type, typename rank<head2>::type>,
                              mpl::push_front<typename merge<typename mpl::pop_front<Heap1>::type, Heap2>::type,
                                              head1>,
                              mpl::eval_if<mpl::less<typename rank<head2>::type, typename rank<head1>::type>,
                                           mpl::push_front<typename merge<Heap1, typename mpl::pop_front<Heap2>::type>::type,
                                                           head2>,
                                           ins_tree<typename link<head1, head2>::type, 
                                                    typename merge<typename mpl::pop_front<Heap1>::type,
                                                                   typename mpl::pop_front<Heap2>::type>::type> > >::type type;
      };

      template<typename Heap1, typename Heap2>
      struct merge :
        mpl::eval_if<typename mpl::empty<Heap1>::type,
                     mpl::identity<Heap2>,
                     mpl::eval_if<typename mpl::empty<Heap2>::type,
                                  mpl::identity<Heap1>,
                                  merge_helper<Heap1, Heap2> > > {};
      
      template<typename Heap> struct remove_min_tree;

      template<typename Heap>
      struct remove_min_tree_helper {
        typedef typename mpl::front<Heap>::type head;
        typedef typename mpl::pop_front<Heap>::type tail;
        typedef typename remove_min_tree<tail>::type rest;
        typedef typename mpl::first<rest>::type rest_tree;
        typedef typename mpl::second<rest>::type rest_list;
        typedef typename mpl::if_<typename mpl::less_equal<typename root<head>::type, 
                                                           typename root<rest_tree>::type>,
                                  mpl::pair<head, tail>,
                                  mpl::pair<rest_tree, typename mpl::push_front<rest_list, head>::type> >::type type;
      };
      template<typename Heap>
      struct remove_min_tree {
        typedef typename mpl::front<Heap>::type head;
        typedef typename mpl::pop_front<Heap>::type tail;
        typedef typename mpl::eval_if<typename mpl::empty<tail>::type,
                                      mpl::pair<head, mpl::list<> >,
                                      remove_min_tree_helper<Heap> >::type type;
      };

      template<typename Heap>
      struct find_min : root<typename mpl::first<typename remove_min_tree<Heap>::type>::type> {};

      template<typename Heap>
      struct delete_min {
        typedef typename remove_min_tree<Heap>::type rmt;
        typedef typename merge<typename mpl::reverse<typename children<typename mpl::first<rmt>::type>::type>::type,
                               typename mpl::second<rmt>::type>::type type;
      };

    } // namespace mpl_graph
  } // namespace metagraph
} // namespace boost

#endif // BOOST_METAGRAPH_MPL_BINOMIAL_HEAP_HPP_INCLUDED
