#include <algorithm>           // std::for_each
#include <boost/format.hpp>    // only needed for printing
#include <boost/histogram.hpp> // make_histogram, regular, weight, indexed
#include <cassert>             // assert
#include <functional>          // std::ref
#include <iostream>            // std::cout, std::cout, std::flush
#include <sstream>             // std::ostringstream
#include <typeinfo>
#include <type_traits>

using namespace boost::histogram; // strip the boost::histogram prefix

void oneDHisto()
{

  /*
    Create a 1d-histogram with a regular axis that has 6 equidistant bins on
    the real line from -1.0 to 2.0, and label it as "x". A family of overloaded
    factory functions called `make_histogram` makes creating histograms easy.

    A regular axis is a sequence of semi-open bins. Extra under- and overflow
    bins extend the axis by default (this can be turned off).

    index    :      -1  |  0  |  1  |  2  |  3  |  4  |  5  |  6
    bin edges:  -inf  -1.0  -0.5   0.0   0.5   1.0   1.5   2.0   inf
  */
  auto h = make_histogram(axis::regular<>(6, -1.0, 2.0, "x"));

  /*
    Let's fill a histogram with data, typically this happens in a loop.

    STL algorithms are supported. std::for_each is very convenient to fill a
    histogram from an iterator range. Use std::ref in the call, if you don't
    want std::for_each to make a copy of your histogram.
  */
  auto data = {-0.5, 1.1, 0.3, 1.7};
  std::for_each(data.begin(), data.end(), std::ref(h));
  h(-1.5); // is placed in underflow bin -1
  h(-1.0); // is placed in bin 0, bin interval is semi-open
  h(2.0);  // is placed in overflow bin 6, bin interval is semi-open
  h(20.0); // is placed in overflow bin 6

  /*
    This does a weighted fill using the `weight` function as an additional
    argument. It may appear at the beginning or end of the argument list. C++
    doesn't have keyword arguments like Python, this is the next-best thing.
  */
  h(0.1, weight(1.0));

  /*
    Iterate over bins with the `indexed` range generator, which provides a
    special accessor object, that can be used to obtain the current bin index,
    and the current bin value by dereferncing (it acts like a pointer to the
    value). Using `indexed` is convenient and gives you better performance than
    looping over the histogram cells with hand-written for loops. By default,
    under- and overflow bins are skipped. Passing `coverage::all` as the
    optional second argument iterates over all bins.

    - Access the value with the dereference operator.
    - Access the current index with `index(d)` method of the accessor.
    - Access the corresponding bin interval view with `bin(d)`.

    The return type of `bin(d)` depends on the axis type (see the axis reference
    for details). It usually is a class that represents a semi-open interval.
    Edges can be accessed with methods `lower()` and `upper()`.
  */

  std::ostringstream os;
  for (auto x : indexed(h, coverage::all)) {
    os << boost::format("bin %2i [%4.1f, %4.1f): %i\n") % x.index() % x.bin().lower() %
              x.bin().upper() % *x;
  }

  std::cout << os.str() << std::flush;

  //decltype(h) whatTypeIsH = 1;

  //std::cout << "type of boost histogram: " << decltype(h) << std::endl;
  //std::cout << "type of boost histogram: " << typeid(h).name() << std::endl;

  assert(os.str() == "bin -1 [-inf, -1.0): 1\n"
                     "bin  0 [-1.0, -0.5): 1\n"
                     "bin  1 [-0.5, -0.0): 1\n"
                     "bin  2 [-0.0,  0.5): 2\n"
                     "bin  3 [ 0.5,  1.0): 0\n"
                     "bin  4 [ 1.0,  1.5): 1\n"
                     "bin  5 [ 1.5,  2.0): 1\n"
                     "bin  6 [ 2.0,  inf): 2\n");
}

void threeDHisto()
{
  /*
    Create a histogram which can be configured dynamically at run-time. The axis
    configuration is first collected in a vector of axis::variant type, which
    can hold different axis types (those in its template argument list). Here,
    we use a variant that can store a regular and a category axis.
  */
  using reg = axis::regular<>;
  using cat = axis::category<std::string>;
  using variant = axis::variant<axis::regular<>, axis::category<std::string>>;
  std::vector<variant> axes;
  axes.emplace_back(cat({"red", "blue"}));
  axes.emplace_back(reg(3, 0.0, 1.0, "x"));
  axes.emplace_back(reg(3, 0.0, 1.0, "y"));
  // passing an iterator range also works here
  auto h = make_histogram(std::move(axes));

  // fill histogram with data, usually this happens in a loop
  h("red", 0.1, 0.2);
  h("blue", 0.7, 0.3);
  h("red", 0.3, 0.7);
  h("red", 0.7, 0.7);

  /*
    Print histogram by iterating over bins.
    Since the [bin type] of the category axis cannot be converted into a double,
    it cannot be handled by the polymorphic interface of axis::variant. We use
    axis::get to "cast" the variant type to the actual category type.
  */

  // get reference to category axis, performs a run-time checked static cast
  const auto& cat_axis = axis::get<cat>(h.axis(0));
  std::ostringstream os;
  for (auto x : indexed(h)) {
    os << boost::format("(%i, %i, %i) %4s [%3.1f, %3.1f) [%3.1f, %3.1f) %3.0f\n") %
              x.index(0) % x.index(1) % x.index(2) % cat_axis.bin(x.index(0)) %
              x.bin(1).lower() % x.bin(1).upper() % x.bin(2).lower() % x.bin(2).upper() %
              *x;
  }

  std::cout << os.str() << std::flush;
  assert(os.str() == "(0, 0, 0)  red [0.0, 0.3) [0.0, 0.3)   1\n"
                     "(1, 0, 0) blue [0.0, 0.3) [0.0, 0.3)   0\n"
                     "(0, 1, 0)  red [0.3, 0.7) [0.0, 0.3)   0\n"
                     "(1, 1, 0) blue [0.3, 0.7) [0.0, 0.3)   0\n"
                     "(0, 2, 0)  red [0.7, 1.0) [0.0, 0.3)   0\n"
                     "(1, 2, 0) blue [0.7, 1.0) [0.0, 0.3)   1\n"
                     "(0, 0, 1)  red [0.0, 0.3) [0.3, 0.7)   0\n"
                     "(1, 0, 1) blue [0.0, 0.3) [0.3, 0.7)   0\n"
                     "(0, 1, 1)  red [0.3, 0.7) [0.3, 0.7)   0\n"
                     "(1, 1, 1) blue [0.3, 0.7) [0.3, 0.7)   0\n"
                     "(0, 2, 1)  red [0.7, 1.0) [0.3, 0.7)   0\n"
                     "(1, 2, 1) blue [0.7, 1.0) [0.3, 0.7)   0\n"
                     "(0, 0, 2)  red [0.0, 0.3) [0.7, 1.0)   1\n"
                     "(1, 0, 2) blue [0.0, 0.3) [0.7, 1.0)   0\n"
                     "(0, 1, 2)  red [0.3, 0.7) [0.7, 1.0)   0\n"
                     "(1, 1, 2) blue [0.3, 0.7) [0.7, 1.0)   0\n"
                     "(0, 2, 2)  red [0.7, 1.0) [0.7, 1.0)   1\n"
                     "(1, 2, 2) blue [0.7, 1.0) [0.7, 1.0)   0\n");
}

void oneDProfile()
{
  /*
    Create a profile. Profiles does not only count entries in each cell, but
    also compute the mean of a sample value in each cell.
  */
  auto p = make_profile(axis::regular<>(5, 0.0, 1.0));

  /*
    Fill profile with data, usually this happens in a loop. You pass the sample
    with the `sample` helper function. The sample can be the first or last
    argument.
  */
  p(0.1, sample(1));
  p(0.15, sample(3));
  p(0.2, sample(4));
  p(0.9, sample(5));

  /*
    Iterate over bins and print profile.
  */
  std::ostringstream os;
  for (auto x : indexed(p)) {
    os << boost::format("bin %i [%3.1f, %3.1f) count %i mean %g\n") % x.index() %
              x.bin().lower() % x.bin().upper() % x->count() % x->value();
  }

  std::cout << os.str() << std::flush;
  assert(os.str() == "bin 0 [0.0, 0.2) count 2 mean 2\n"
                     "bin 1 [0.2, 0.4) count 1 mean 4\n"
                     "bin 2 [0.4, 0.6) count 0 mean 0\n"
                     "bin 3 [0.6, 0.8) count 0 mean 0\n"
                     "bin 4 [0.8, 1.0) count 1 mean 5\n");
}

void testBoostHistos()
{
  oneDHisto();
  threeDHisto();
  oneDProfile();

}
