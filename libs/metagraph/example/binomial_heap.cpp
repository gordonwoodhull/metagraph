#include <boost/mpl/list.hpp>
#include <boost/metagraph/mpl_graph/binomial_heap.hpp>
#include <boost/mpl/equal.hpp>

namespace mpl_graph = boost::metagraph::mpl_graph;
namespace mpl = boost::mpl;

typedef mpl_graph::insert<mpl::int_< 9>, mpl::list<> >::type heap0;
typedef mpl_graph::insert<mpl::int_<17>, heap0 >::type heap1;
typedef mpl_graph::insert<mpl::int_< 2>, heap1 >::type heap2;
typedef mpl_graph::insert<mpl::int_<23>, heap2 >::type heap3;
typedef mpl_graph::insert<mpl::int_< 0>, heap3 >::type heap4;
typedef mpl_graph::insert<mpl::int_<-1>, heap4 >::type heap5;
typedef mpl_graph::insert<mpl::int_<99>, heap5 >::type heap6;
typedef mpl_graph::insert<mpl::int_<38>, heap6 >::type heap7;

typedef mpl_graph::find_min<heap7>::type val0;
typedef mpl_graph::delete_min<heap7>::type heapD1;
BOOST_MPL_ASSERT(( boost::is_same<val0, mpl::int_<-1> > ));
typedef mpl_graph::find_min<heapD1>::type val1;
typedef mpl_graph::delete_min<heapD1>::type heapD2;
BOOST_MPL_ASSERT(( boost::is_same<val1, mpl::int_<0> > ));
typedef mpl_graph::find_min<heapD2>::type val2;
typedef mpl_graph::delete_min<heapD2>::type heapD3;
BOOST_MPL_ASSERT(( boost::is_same<val2, mpl::int_<2> > ));
typedef mpl_graph::find_min<heapD3>::type val3;
typedef mpl_graph::delete_min<heapD3>::type heapD4;
BOOST_MPL_ASSERT(( boost::is_same<val3, mpl::int_<9> > ));
typedef mpl_graph::find_min<heapD4>::type val4;
typedef mpl_graph::delete_min<heapD4>::type heapD5;
BOOST_MPL_ASSERT(( boost::is_same<val4, mpl::int_<17> > ));
typedef mpl_graph::find_min<heapD5>::type val5;
typedef mpl_graph::delete_min<heapD5>::type heapD6;
BOOST_MPL_ASSERT(( boost::is_same<val5, mpl::int_<23> > ));
typedef mpl_graph::find_min<heapD6>::type val6;
typedef mpl_graph::delete_min<heapD6>::type heapD7;
BOOST_MPL_ASSERT(( boost::is_same<val6, mpl::int_<38> > ));
typedef mpl_graph::find_min<heapD7>::type val7;
typedef mpl_graph::delete_min<heapD7>::type heapD8;
BOOST_MPL_ASSERT(( boost::is_same<val7, mpl::int_<99> > ));

int main() {
  return 0;
}

