// Copyright 2011 Gordon Woodhull
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_METAGRAPH_PATTERNS_TOKENS_HPP
#define BOOST_METAGRAPH_PATTERNS_TOKENS_HPP

// all sorts of pattern languages use the names "pattern", "role", and "relation" 
// for graph, node, and edge respectively

namespace boost {
namespace metagraph {
namespace patterns {

  template<typename ...>
  struct pattern {};
  template<typename ...>
  struct role {};
  template<typename ...>
  struct relation {};

}
}
}

#endif // BOOST_METAGRAPH_PATTERNS_TOKENS_HPP
