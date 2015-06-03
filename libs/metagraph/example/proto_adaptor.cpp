//  Example of adapting Proto trees into MPGL (aka mpl_graph) graphs
//  Copyright 2010 Gordon Woodhull.  
//  Based on Proto example Calc1
//  Copyright 2008 Eric Niebler. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Start with the simple example of how to build an arithmetic expression
// evaluator with placeholders, and then traverse the ASTs using DFS & BFS

// This does compile-time traversal of the AST, which is not really the whole
// problem.  Instead it should be a Fusion/BGL hybrid

#include <iostream>
#include <boost/mpl/int.hpp>
#include <boost/proto/core.hpp>
#include <boost/proto/context.hpp>

#include <boost/metagraph/mpl_graph/depth_first_search.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/typeof/typeof.hpp>


namespace mpl = boost::mpl;
namespace proto = boost::proto;
namespace mpgl = boost::metagraph::mpl_graph;
using proto::_;

template<int I> struct placeholder {};

// Define some placeholders
proto::terminal< placeholder< 1 > >::type const _1 = {{}};
proto::terminal< placeholder< 2 > >::type const _2 = {{}};

// Define a calculator context, for evaluating arithmetic expressions
struct calculator_context
  : proto::callable_context< calculator_context const >
{
    // The values bound to the placeholders
    double d[2];

    // The result of evaluating arithmetic expressions
    typedef double result_type;

    explicit calculator_context(double d1 = 0., double d2 = 0.)
    {
        d[0] = d1;
        d[1] = d2;
    }

    // Handle the evaluation of the placeholder terminals
    template<int I>
    double operator ()(proto::tag::terminal, placeholder<I>) const
    {
        return d[ I - 1 ];
    }
};

template<typename Expr>
double evaluate( Expr const &expr, double d1 = 0., double d2 = 0. )
{
    // Create a calculator context with d1 and d2 substituted for _1 and _2
    calculator_context const ctx(d1, d2);

    // Evaluate the calculator expression with the calculator_context
    return proto::eval(expr, ctx);
}

// implement the MPGL IncidenceGraph concept on the Proto API
// Vertex is Expr; Edge is mpl::pair<Expr, ChildNum>
// Graph is unused

// dummy graph telling mpgl to use these specializations
struct proto_graph_rep;
struct ProtoGraph {
    typedef proto_graph_rep representation;
};

namespace boost {
namespace metagraph {
namespace mpl_graph {    
    
// this probably is not the most awesome way to specialize the mpgl metafunctions
// mpl_graph.hpp should probably be opened up to rely more on the representation tag
// instead of assuming that every impl will have giant maps in it

template<typename Edge>
struct source<Edge, ProtoGraph> : 
    mpl::first<Edge> 
{};
template<typename Edge>
struct target<Edge, ProtoGraph> : 
    proto::result_of::child_c<typename mpl::first<Edge>::type, mpl::second<Edge>::type::value> 
{};
template<typename Vertex>
struct out_edges<Vertex, ProtoGraph> :
    mpl::fold<mpl::range_c<long,0,proto::arity_of<Vertex>::value>,
         mpl::vector<>,
         mpl::push_back<mpl::_1, mpl::pair<Vertex, mpl::_2> > >
{};
template<typename Vertex>
struct out_degree<Vertex, ProtoGraph> : 
    proto::arity_of<Vertex>
{};
        
}
}
}

struct preordering_visitor : mpgl::dfs_default_visitor_operations {    
    template<typename Node, typename Graph, typename State>
    struct discover_vertex :
        mpl::push_back<State, Node>
    {};
};

// dummy Graph (unneeded)
struct None {};

template<typename Expr>
struct proto_preorder_dfs :
    mpl::first<typename mpgl::depth_first_search<ProtoGraph, preordering_visitor, mpl::vector<>, Expr>::type >
{};

typedef BOOST_TYPEOF(_1*12/_2) expr_type;
typedef proto_preorder_dfs<expr_type>::type pre_expr_travs;

// print a nice error message showing that the traversal worked.  
// will show the preorder traversal *in reverse order*, because push_back is lazy
BOOST_MPL_ASSERT(( mpl::equal<pre_expr_travs, void > ));

int main()
{
    
    return 0;
}
