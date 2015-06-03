#include <boost/msm/mpl_graph/incidence_list_graph.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/msm/mpl_graph/print_mpl_graph.hpp>

#include <iostream>

namespace mpl_graph = boost::msm::mpl_graph;
namespace mpl = boost::mpl;

// vertices
struct A{}; struct B{}; struct C{}; struct D{}; struct E{}; struct F{}; struct G{};

// edges
struct A_B{}; struct B_C{}; struct C_D{}; struct C_E{}; struct C_F{}; struct B_F{};



/* 
    incidence list test graph:
    A -> B -> C -\--> D
           \     |--> E
            \    \--> F
             \-----/
    G
*/           

typedef mpl::vector<mpl::vector<A_B,A,B>,
               mpl::vector<B_C,B,C>,
               mpl::vector<C_D,C,D>,
               mpl::vector<C_E,C,E>,
               mpl::vector<C_F,C,F>,
               mpl::vector<B_F,B,F> >
    some_incidence_list;

typedef mpl_graph::incidence_list_graph<some_incidence_list> some_incidence_list_graph;

int main() {
  boost::msm::mpl_graph::print_graph<some_incidence_list_graph>(std::cout);
}

