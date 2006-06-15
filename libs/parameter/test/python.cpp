// Copyright Daniel Wallin 2006. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/parameter/preprocessor.hpp>
#include <boost/parameter/keyword.hpp>
#include <boost/parameter/python.hpp>
#include <boost/python.hpp>
#include <boost/utility/enable_if.hpp>

namespace test {

BOOST_PARAMETER_KEYWORD(tag, x)
BOOST_PARAMETER_KEYWORD(tag, y)
BOOST_PARAMETER_KEYWORD(tag, z)

struct Xbase
{
    // We need the disable_if part for VC7.1/8.0.
    template <class Args>
    Xbase(
        Args const& args
      , typename boost::disable_if<
            boost::is_base_and_derived<Xbase, Args>
        >::type* = 0
    )
      : value(std::string(args[x | "foo"]) + args[y | "bar"])
    {}

    std::string value;
};

struct X : Xbase
{
    BOOST_PARAMETER_CONSTRUCTOR(X, (Xbase), tag,
        (optional
         (x, *)
         (y, *)
        )
    )

    BOOST_PARAMETER_BASIC_MEMBER_FUNCTION((int), f, tag,
        (required
         (x, *)
         (y, *)
        )
        (optional
         (z, *)
        )
    )
    {
        return args[x] + args[y] + args[z | 0];
    }

    BOOST_PARAMETER_BASIC_MEMBER_FUNCTION((std::string), g, tag,
        (optional
         (x, *)
         (y, *)
        )
    )
    {
        return std::string(args[x | "foo"]) + args[y | "bar"];
    }
};

} // namespace test

struct f_fwd
{
    template <class R, class T, class A0, class A1, class A2>
    R operator()(boost::type<R>, T& self, A0 const& a0, A1 const& a1, A2 const& a2)
    {
        return self.f(a0,a1,a2);
    }
};

struct g_fwd
{
    template <class R, class T, class A0, class A1>
    R operator()(boost::type<R>, T& self, A0 const& a0, A1 const& a1)
    {
        return self.g(a0,a1);
    }
};

BOOST_PYTHON_MODULE(python_parameter)
{
    namespace mpl = boost::mpl;
    using namespace test;
    using namespace boost::python;

    class_<X>("X")
        .def(
            boost::parameter::python::init<
                mpl::vector<
                    tag::x*(std::string), tag::y*(std::string)
                >
            >()
        )
        .def(
            "f"
          , boost::parameter::python::function<
                f_fwd
              , mpl::vector<
                    int, tag::x(int), tag::y(int), tag::z*(int)
                >
            >()
        )
        .def(
            "g"
          , boost::parameter::python::function<
                g_fwd
              , mpl::vector<
                    std::string, tag::x*(std::string), tag::y*(std::string)
                >
            >()
        )
        .def_readonly("value", &X::value);
}

