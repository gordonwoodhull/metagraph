// Copyright 2011 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_ANGLY_DFA_HPP
#define BOOST_METAGRAPH_ANGLY_DFA_HPP

#include <boost/mpl/back.hpp>
#include <boost/mpl/iter_fold.hpp>
#include <boost/mpl/push_front.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/same_as.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/pop_front.hpp>
#include <boost/mpl/front.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/empty.hpp>
#include <boost/mpl/protect.hpp>
#include <boost/mpl/print.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/msm/mpl_graph/mpl_graph.hpp>

namespace boost {
  namespace metagraph {
    namespace angly {

      namespace detail {

        // extract a variadic template from its instantiation, stripping its args and returning T<>
        template<typename T> struct de_arg {
          // default = didn't work (e.g non-templates, templates with non-type params)
          typedef void type; 
        };

        template<template<typename...> class Template, typename ...Args>
        struct de_arg<Template<Args...> > {
          typedef Template<> type;
        };

        // convert variadic template arguments to mpl sequence
        template<typename ...Args> struct variadic_to_mpl;

        template<typename Head, typename ...Args> 
        struct variadic_to_mpl<Head, Args...> :
          mpl::push_front<typename variadic_to_mpl<Args...>::type, Head> {};

        template<>
        struct variadic_to_mpl<> :
          mpl::vector<> {};

        // return the args of a template instantiation as an mpl sequence
        template<typename T> struct arg_seq;

        template<template<typename...> class Template, typename ...Args>
        struct arg_seq<Template<Args...> > : variadic_to_mpl<Args...> {};

      }

      /* 
         The DFA operates in stack frames which are produced when recursing
         into angle brackets.

         Each frame produces some metadata "pre" and "post" semantic actions

         Each semantic action metafunction takes two parameters: 
         * the parent frame metadata
         * the child frame metadata

         The pre-action only gets invoked when there is a recurse rule.  It
         takes the parent frame's metadata and the start_data of the recurse rule state.

         The post-action gets invoked at the end of every transition.  Its
         first parameter is the current frame data.  If there was a recursion, 
         its second parameter is the resulting frame data from the child,
         otherwise it is the raw value of the item that matched.
         
         Each semantic action returns the current frame metadata, i.e. pre-
         returns the resulting child frame data and post- returns the resulting
         parent frame data.

         It could conceivably input/output a whole stack of current datas
         (for something akin to inherited attributes)
         but let's not get too crazy.
       */

      // derive from these to produce your states & transitions
      // or provide states & transitions with the same access metafunctions
      template<typename IsFinish = mpl::false_, typename StartData = void>
      struct dfa_state {
        typedef IsFinish is_finish;
        typedef StartData start_data;
      };

      template<typename MatchFn, typename RecurseRule = void, typename PostAction = mpl::_1, typename PreAction = mpl::protect<mpl::_2> > 
      struct dfa_transition {
        typedef typename mpl::lambda<MatchFn>::type match_fn;
        typedef typename mpl::lambda<PreAction>::type pre_action;
        typedef typename mpl::lambda<PostAction>::type post_action;
        typedef RecurseRule recurse_rule;
      };

      template<typename T> struct get_transition_match_fn {
        typedef typename T::match_fn type;
      };

      template<typename T> struct get_transition_pre_action {
        typedef typename T::pre_action type;
      };

      template<typename T> struct get_transition_post_action {
        typedef typename T::post_action type;
      };

      template<typename T> struct get_transition_recurse_rule {
        typedef typename T::recurse_rule type;
      };

      // a match_fn for matching the template
      template<typename Token> struct match_token
      {
        template<typename Item> struct apply;
        template<typename Item> struct apply<mpl::protect<Item> >
        : boost::is_same<Token, typename detail::de_arg<Item>::type>
        {};
      };

      template<typename Trans, typename Item> 
      struct check_transition_match :
        mpl::apply<typename get_transition_match_fn<Trans>::type, Item>
      {};

      template<typename T> struct is_finish_state : T::is_finish 
      {};
      
      template<typename T> struct get_state_start_data {
        typedef typename T::start_data type;
      };

      namespace detail {
        template<typename DFA, typename State, typename Item, typename PropFn>
        struct match_item {
          typedef typename msm::mpl_graph::out_edges<State, DFA>::type edges;
          typedef typename mpl::find_if<edges, 
                                        check_transition_match<mpl::apply_wrap1<PropFn,mpl::_1>, mpl::protect<Item> >
                                        >::type miter;
          static_assert(!boost::is_same<miter, typename mpl::end<edges>::type>::value, "no match for input token");
          typedef typename mpl::deref<miter>::type type;
        };
        /* state stack: since we need to avoid recursion, we need to store 
           the entire state of the parser in an explicit stack, where each frame corresponds
           to a level of angle bracket in the input.
           each frame contains:
           - the current state of the DFA 
           - the current and 
           - end iterator of the current input sequence 
           - the semantic action which must be executed upon returning from a frame
           - the output data
       
        */

        // the actual format is what's best for printing; this order is convenient for use (?)
        template<typename Data, typename State, typename CurrIter, typename EndIter, typename Action = mpl::_1>
        struct create_frame :
          mpl::vector<State, CurrIter, EndIter, typename mpl::lambda<Action>::type, Data>        
        {};

        template<class T>
        struct print
        {
          typedef T type;
          unsigned : 80;
        };

        template<typename Frame>
        struct get_frame_state : mpl::at_c<Frame,0> {};
        template<typename Frame>
        struct get_frame_curr_iter : mpl::at_c<Frame,1> {};
        template<typename Frame>
        struct get_frame_end_iter : mpl::at_c<Frame,2> {};
        template<typename Frame>
        struct get_frame_action : mpl::at_c<Frame,3> {};
        template<typename Frame>
        struct get_frame_data : mpl::at_c<Frame,4> {};
        
        template<typename Frame, typename Data>
        struct set_data :
          create_frame<Data, 
                       typename get_frame_state<Frame>::type, 
                       typename get_frame_curr_iter<Frame>::type, 
                       typename get_frame_end_iter<Frame>::type,
                       typename get_frame_action<Frame>::type>
        {};


        template<typename Stack>
        struct stack_states :
          mpl::transform<Stack, get_frame_state<mpl::_1> >
        {};
      
        // all these metafunctions return the new state stack
        template<typename Stack, typename PropFn>
        struct finish_frame {
          typedef typename mpl::front<Stack>::type frame;

          static_assert(is_finish_state<typename PropFn::template apply<typename get_frame_state<frame>::type>::type>::value, 
                        "must be in finishing state when input ends");

          typedef typename get_frame_data<frame>::type result;
          typedef typename mpl::pop_front<Stack>::type popt;
          typedef typename mpl::front<popt>::type frame2;
          typedef typename mpl::pop_front<popt>::type popt2;
          typedef typename 
          mpl::push_front<popt2,
                          typename 
                          set_data<frame2, 
                                   typename 
                                   mpl::apply<typename get_frame_action<frame2>::type,
                                              typename get_frame_data<frame2>::type, 
                                              result>::type>::type>::type type;
        };

        template<typename DFA, typename Stack, typename Match, typename Item, typename PropFn>
        struct dfa_follow_no_recurse {
          typedef typename mpl::front<Stack>::type frame;
          typedef typename mpl::pop_front<Stack>::type popt;
          typedef typename 
          mpl::push_front<popt, 
                          typename 
                          create_frame<typename 
                                       mpl::apply<typename get_transition_post_action<typename PropFn::template apply<Match>::type>::type,
                                                  typename get_frame_data<frame>::type, 
                                                  Item>::type,
                                       typename msm::mpl_graph::target<Match, DFA>::type, 
                                       typename mpl::next<typename get_frame_curr_iter<frame>::type>::type, 
                                       typename get_frame_end_iter<frame>::type>::type>::type type;
        };

        template<typename DFA, typename Stack, typename RecurseRule, typename Match, typename Item, typename PropFn>
        struct dfa_follow_recurse {
          typedef typename mpl::front<Stack>::type frame;
          typedef typename mpl::pop_front<Stack>::type popt;
          typedef typename 
          mpl::push_front<popt, 
                          typename 
                          create_frame<typename get_frame_data<frame>::type,
                                       typename msm::mpl_graph::target<Match, DFA>::type, 
                                       typename mpl::next<typename get_frame_curr_iter<frame>::type>::type, 
                                       typename get_frame_end_iter<frame>::type,
                                       typename get_transition_post_action<typename PropFn::template apply<Match>::type>::type>::type>::type parent;

          typedef typename detail::arg_seq<Item>::type inner_seq;
          typedef typename
          mpl::push_front<parent,
                          typename 
                          create_frame<typename mpl::apply<typename get_transition_pre_action<typename PropFn::template apply<Match>::type>::type,
                                                           typename get_frame_data<frame>::type,
                                                           typename get_state_start_data<typename PropFn::template apply<RecurseRule>::type>::type>::type,
                                       RecurseRule,
                                       typename mpl::begin<inner_seq>::type,
                                       typename mpl::end<inner_seq>::type>::type>::type type;
        };

          
        template<typename DFA, typename Stack, typename PropFn>
        struct dfa_follow {
          typedef typename mpl::front<Stack>::type frame;
        
          typedef typename get_frame_curr_iter<frame>::type curr_iter;
          typedef typename mpl::deref<curr_iter>::type item;         
          typedef typename match_item<DFA, typename get_frame_state<frame>::type, item, PropFn>::type match;

          typedef typename get_transition_recurse_rule<typename PropFn::template apply<match>::type>::type recurse_rule;
          typedef typename mpl::eval_if<
            typename boost::is_same<void, recurse_rule>::type,
            dfa_follow_no_recurse<DFA, Stack, match, item, PropFn>,
            dfa_follow_recurse<DFA, Stack, recurse_rule, match, item, PropFn> >::type type;
        };


        template<typename DFA, typename Stack, typename PropFn>
        struct dfa_step {
          typedef typename mpl::front<Stack>::type frame;
          typedef typename get_frame_curr_iter<frame>::type curr_iter;
          typedef typename get_frame_end_iter<frame>::type end_iter;
  
          typedef typename 
          mpl::eval_if<typename boost::is_same<curr_iter, end_iter>::type,
                       finish_frame<Stack, PropFn>,
                       dfa_follow<DFA, Stack, PropFn>
                       >::type type;
        }; 

        struct utterly_done_state {};
        // create a stack with a sentinel end-frame
        template<typename Data, typename StartState, typename CurrIter, typename EndIter>
        struct create_stack {
          typedef mpl::vector<typename 
                              create_frame<Data, 
                                                   StartState, 
                                                   CurrIter,
                                                   EndIter>::type,
                              typename //end-frame
                              create_frame<void,utterly_done_state, void, void, mpl::_2>::type
                              > type;
        };

        template<typename Stack>
        struct stack_is_done :
          boost::is_same<typename get_frame_state<typename mpl::front<Stack>::type>::type, utterly_done_state>
        {};

        struct dfa_end {};

        template<typename DFA, typename Stack, typename PropFn>
        struct dfa_iter {
          //print<typename stack_states<Stack>::type> x;
          static_assert(!stack_is_done<Stack>::value, "iterate past end of DFA sequence");
          typedef typename dfa_step<DFA, Stack, PropFn>::type eval;
  
          typedef typename 
          mpl::if_<typename stack_is_done<eval>::type,
                   dfa_end,
                   dfa_iter<DFA, eval, PropFn> >::type next;
          typedef typename get_frame_data<typename mpl::front<eval>::type>::type type;
        };
          
      } // namespace detail

      // a DFA sequence is a pair of iterators, where an iterator represents the running
      // state of the DFA and a position in the input.  currently only the data manipulated
      // by the semantic actions is accessible (by dereferencing the iterator); if access
      // to the state stack is desired, we'll have to talk about the best interface - it's
      // currently (undocumented ;) in dfa_iter<>::state
      template<typename DFA, typename StartState, typename Input, typename PropFn>
      struct dfa_sequence {
        typedef typename detail::de_arg<Input>::type token; // unchecked; we should actually work with single_view<Input> as starting sequence and add start state to parsers
        typedef typename detail::arg_seq<Input>::type inner_seq;
        typedef typename detail::create_stack<typename get_state_start_data<typename PropFn::template apply<StartState>::type>::type, StartState, typename mpl::begin<inner_seq>::type, typename mpl::end<inner_seq>::type>::type stack;
        typedef detail::dfa_iter<DFA, stack, PropFn> begin;
        typedef detail::dfa_end end;
      };

      template<typename DFA, typename StartState, typename Input, typename PropFn = mpl::_1>
      struct run_dfa :
        mpl::fold<dfa_sequence<DFA, StartState, Input, typename mpl::lambda<PropFn>::type>, 
                  void,
                  mpl::_2 > // not really a fold, just iterate until done and return final data
      {};
   
    } 
  } 
} // namespaces

#endif //BOOST_METAGRAPH_ANGLY_DFA_HPP
