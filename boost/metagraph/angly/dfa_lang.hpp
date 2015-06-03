// Copyright 2011 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/metagraph/angly/dfa.hpp>
#include <boost/metagraph/angly/detail/apply_seq.hpp>

// pull a DFA language up by its bootstraps 
// by directly building a DFA to parse the language


namespace boost {
namespace metagraph {
namespace angly {

  template<typename ...>
  struct dfa {};
  template<typename ...>
  struct state {};
  template<typename ...>
  struct transition {};
  // we'd prefer true attributes but are settling for crude semantic actions for now
  // this special one records a map of names to data, a common requirement for graph parsers
  // key lambda and value lambda are applied to _1 parent data and _2 child data as all other lambdas
  template<typename KeyLambda, typename ValueLambda>
  struct record {};

  // parser for descriptions of parsers
  // dfa<state<name, [is_finish, [data,]] [transition<name, target, [match_fn, [recurse_rule, [post_action, [pre_action]]]]>], 
  //                                  transition<...> ] >,
  //     state<...> >
  // it's pretty unwieldy to define a parser this way!!  
  // so this parser makes defining new parsers easier
  struct dfa_states : dfa_state<mpl::true_, mpl::pair<mpl::vector<>, mpl::vector<> > > {};
  struct state_name : dfa_state<> {};
  struct state_finish : dfa_state<mpl::true_> {};
  struct state_data : dfa_state<mpl::true_> {};
  struct state_transitions : dfa_state<mpl::true_> {};
  struct transition_name : dfa_state<> {};
  struct transition_target : dfa_state<> {};
  struct transition_match : dfa_state<mpl::true_> {};
  struct transition_recurse : dfa_state<mpl::true_> {};
  struct transition_post_action : dfa_state<mpl::true_> {};
  struct transition_pre_action : dfa_state<mpl::true_> {};
  struct transition_record : dfa_state<mpl::true_> {};
  struct transition_done : dfa_state<mpl::true_> {};

  // there are two horrible thing about building at DFA this way:
  // - the states, transitions, and graph are all specified separately
  // - the data-building actions have to work with sequences of sequences, which is tedious
  // the aim here is to solve problem #1.  #2 might be fixed with semantic action edsl?

  // anyway, we have to build 
  // - a sequence of states, each of which contains a sequnce of transitions
  // - a map from state/transition to data
  // luckily we can build this all from bottom up.  so, 
  // - dfa level returns pair <state sequence, stmap>
  // - state returns sequence <name, args, transitions, stmap>
  // - transition returns the sequence of its args
  // where stmap is sequence of pairs <state/transition name, value>

  // because at_c doesn't work in lambdas (clarity too i guess)
  template<typename S> struct s_name : mpl::at_c<S, 0> {};
  template<typename S> struct s_args : mpl::at_c<S, 1> {};
  template<typename S> struct s_transitions : mpl::at_c<S, 2> {};
  template<typename S> struct s_stmap : mpl::at_c<S, 3> {};

  template<typename T> struct t_name : mpl::at_c<T, 0> {};
  template<typename T> struct t_target : mpl::at_c<T, 1> {};
  template<typename T> struct t_args : mpl::pop_front<typename mpl::pop_front<T>::type> {};

  // types for transitions
  struct dfa_a_state : 
    dfa_transition<match_token<state<> >, 
                   state_name, 
                   // rebuild dfa data, pushing state onto both state seq and stmap
                   mpl::pair<mpl::push_back<mpl::first<mpl::_1>, 
                                            mpl::pair<s_name<mpl::_2>,
                                                      s_transitions<mpl::_2> > >,
                             mpl::push_back<mpl::copy<s_stmap<mpl::_2>,
                                                      mpl::back_inserter<mpl::second<mpl::_1> > >, 
                                            mpl::pair<s_name<mpl::_2>,
                                                      detail::apply_seq_q<dfa_state>
                                                      ::apply<s_args<mpl::_2> > > > > > {}; 
  struct state_name_trans : 
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::vector4<mpl::_2, mpl::vector<>, mpl::vector<>, mpl::vector<> > > {};
  struct state_finish_trans : 
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::vector4<s_name<mpl::_1>, mpl::vector1<mpl::_2>, mpl::vector<>, mpl::vector<> > > {};

  struct state_data_trans : 
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::vector4<s_name<mpl::_1>,
                                mpl::push_back<s_args<mpl::_1>, mpl::_2>, 
                                mpl::vector<>, mpl::vector<> > > {};
  struct state_transition : 
    dfa_transition<match_token<transition<> >, 
                   transition_name, 
                   // rebuild state data, pushing new transition onto transitions and stmap
                   mpl::vector4<s_name<mpl::_1>, s_args<mpl::_1>,
                                mpl::push_back<s_transitions<mpl::_1>, 
                                               mpl::pair<t_name<mpl::_2>, t_target<mpl::_2> > >,
                                mpl::push_back<s_stmap<mpl::_1>,
                                               mpl::pair<t_name<mpl::_2>,
                                                         detail::apply_seq_q<dfa_transition>
                                                         ::apply<t_args<mpl::_2> > > > >
                   > {};
  struct state_name_transition : state_transition {};
  struct state_finish_transition : state_transition {};
  struct transition_name_trans : 
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::vector1<mpl::_2> > {};
  struct transition_target_trans : 
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::push_back<mpl::_1,mpl::_2> > {};
  struct transition_match_trans :
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::push_back<mpl::_1,mpl::_2> > {};
  struct transition_recurse_trans :
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::push_back<mpl::_1,mpl::_2> > {};
  struct transition_post_action_trans :
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::push_back<mpl::_1,mpl::lambda<mpl::_2> > > {};
  struct transition_pre_action_trans :
    dfa_transition<mpl::always<mpl::true_>, 
                   void, 
                   mpl::push_back<mpl::_1,mpl::lambda<mpl::_2> > > {};

  typedef boost::msm::mpl_graph::adjacency_list_graph<
    mpl::vector<mpl::pair<dfa_states, 
                          mpl::vector<mpl::pair<dfa_a_state, dfa_states> > >,
                mpl::pair<state_name, 
                          mpl::vector<mpl::pair<state_name_trans, state_finish> > >,
                mpl::pair<state_finish, 
                          mpl::vector<mpl::pair<state_name_transition, state_transitions>,
                                      mpl::pair<state_finish_trans, state_data> > >,
                mpl::pair<state_data, 
                          mpl::vector<mpl::pair<state_finish_transition, state_transitions>,
                                      mpl::pair<state_data_trans, state_transitions> > >,
                mpl::pair<state_transitions, 
                          mpl::vector<mpl::pair<state_transition, state_transitions> > >,
                mpl::pair<transition_name,
                          mpl::vector<mpl::pair<transition_name_trans, transition_target> > >,
                mpl::pair<transition_target,
                          mpl::vector<mpl::pair<transition_target_trans, transition_match> > >,
                mpl::pair<transition_match,
                          mpl::vector<mpl::pair<transition_match_trans, transition_recurse> > >,
                mpl::pair<transition_recurse,
                          mpl::vector<mpl::pair<transition_recurse_trans, transition_post_action> > >,
                mpl::pair<transition_post_action,
                          mpl::vector<mpl::pair<transition_post_action_trans, transition_pre_action> > >,
                mpl::pair<transition_pre_action,
                          mpl::vector<mpl::pair<transition_pre_action_trans, transition_done> > >
                > > dfa_dfa;

  template<typename DFADesc>
  struct create_parser {
    typedef typename run_dfa<dfa_dfa, dfa_states, DFADesc>::type adjacencies_and_mappings;
    typedef typename boost::msm::mpl_graph::
    adjacency_list_graph<typename mpl::first<adjacencies_and_mappings>::type> graph_dfa;
    typedef typename boost::msm::mpl_graph::mpl_utils::
    as_map<typename mpl::second<adjacencies_and_mappings>::type>::type graph_st_map;
    
    typedef mpl::pair<graph_dfa, graph_st_map> type;
  };

}
}
}
