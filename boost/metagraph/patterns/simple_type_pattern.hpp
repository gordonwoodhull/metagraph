// Copyright 2011 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_PATTERNS_SIMPLE_TYPE_PATTERN_HPP
#define BOOST_METAGRAPH_PATTERNS_SIMPLE_TYPE_PATTERN_HPP

#include <boost/metagraph/angly/dfa_lang.hpp>
#include <boost/metagraph/patterns/tokens.hpp>

// a language and a system for defining and using Simple Type Patterns,
// where the graph specifies the relations 

namespace boost {
namespace metagraph {
namespace patterns {

  // sorry, starting with a search-and-replace of the graph in angly_graph_parser.
  // pattern hierarchies will add large-scale customizability but all these things
  // that produce an MPL.Graph with just different data?  hmm

  struct pattern_roles {}; struct role_name {}; struct role_relations {}; 
  struct relation_name {}; struct relation_target {}; struct relation_done {};
  struct pattern_role {}; struct role_name_trans {}; struct role_relation {};
  struct relation_name_trans {}; struct relation_target_trans {};

  typedef angly::dfa<angly::state<pattern_roles, mpl::true_, mpl::vector<>,
                                  angly::transition<pattern_role, pattern_roles, 
                                                    angly::match_token<role<> >, role_name, 
                                                    mpl::push_back<mpl::_1, mpl::_2> > >,
                     angly::state<role_name, 
                                  angly::transition<role_name_trans, role_relations,
                                                    mpl::always<mpl::true_>, void, 
                                                    mpl::pair<mpl::_2, mpl::vector<> > > >,
                     angly::state<role_relations, mpl::true_,
                                  angly::transition<role_relation, role_relations,
                                                    angly::match_token<relation<> >, relation_name, 
                                                    mpl::pair<mpl::first<mpl::_1>, 
                                                              mpl::push_back<mpl::second<mpl::_1>, 
                                                                             mpl::_2> > > >,
                     angly::state<relation_name,
                                  angly::transition<relation_name_trans, relation_target,
                                                    mpl::always<mpl::true_>, void, mpl::_2> >,
                     angly::state<relation_target,
                                  angly::transition<relation_target_trans, relation_done,
                                                    mpl::always<mpl::true_>, void, 
                                                    mpl::pair<mpl::_1, mpl::_2> > >,
                     angly::state<relation_done, mpl::true_>
                     > pattern_dfa_desc;

  typedef typename angly::create_parser<pattern_dfa_desc>::type pattern_parser;

}
}
}
#endif // BOOST_METAGRAPH_PATTERNS_SIMPLE_TYPE_PATTERN_HPP
