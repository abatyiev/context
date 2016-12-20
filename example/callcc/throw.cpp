
//          Copyright Oliver Kowalke 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/context/continuation.hpp>

namespace ctx = boost::context;

struct my_exception : public std::runtime_error {
    my_exception( std::string const& what) :
        std::runtime_error{ what } {
    }
};

int main() {
    ctx::continuation c = ctx::callcc< void >([](boost::context::continuation && c) {
        for (;;) {
            try {
                    std::cout << "entered" << std::endl;
                    c = ctx::callcc< void >( std::move( c) );
            } catch ( boost::context::ontop_error const& e) {
                try {
                    std::rethrow_if_nested( e);
                } catch ( my_exception const& ex) {
                    std::cerr << "my_exception: " << ex.what() << std::endl;
                }
                return e.get_continuation();
            }
        }
        return std::move( c);
    });
    c = ctx::callcc< void >( std::move( c) );
    c = ctx::callcc< void >( std::move( c) );
    c = ctx::callcc< void >( std::move( c), ctx::exec_ontop_arg, []() { throw my_exception("abc"); });

    std::cout << "main: done" << std::endl;

    return EXIT_SUCCESS;
}