///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#define MIXED_OPS_ONLY

#include <nil/crypto3/multiprecision/boost_backends/debug_adaptor.hpp>
#include <nil/crypto3/multiprecision/boost_backends/cpp_dec_float.hpp>
#include "test_arithmetic.hpp"

int main() {
    test<boost::multiprecision::number<
        boost::multiprecision::debug_adaptor<boost::multiprecision::cpp_dec_float<50>>>>();
    return boost::report_errors();
}
