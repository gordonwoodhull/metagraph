#ifndef BOOST_METAGRAPH_EXAMPLE_APPLY_SEQ_HPP
#define BOOST_METAGRAPH_EXAMPLE_APPLY_SEQ_HPP

#include <boost/mpl/empty.hpp>
#include <boost/mpl/pop_back.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/vector.hpp>

// from https://github.com/scientific-coder/Computer-Languages/blob/master/interpreting/apply.hxx

namespace boost {
namespace metagraph {
namespace angly {
namespace detail {
template<template<typename...> class T, bool empty, typename C, typename... Types> struct apply_seq_helper {
  typedef typename boost::mpl::pop_back<C>::type rest;
  typedef typename apply_seq_helper<T, boost::mpl::empty<rest>::value, rest, typename boost::mpl::back<C>::type, Types...>::type type;
};
template<template<typename...> class T,typename C, typename... Types> struct apply_seq_helper<T, true, C, Types...> {
  typedef T<Types...> type;
};

template<template<typename...> class T,typename C> struct apply_seq : detail::apply_seq_helper<T, boost::mpl::empty<C>::value, C> {};

// for lambdas
template<template<typename...> class T> struct apply_seq_q {
  template<typename C>
  struct apply : apply_seq<T, C>
  {};
};
}
}
}
}

#endif // BOOST_METAGRAPH_EXAMPLE_APPLY_SEQ_HPP
