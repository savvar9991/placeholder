///////////////////////////////////////////////////////////////
//  Copyright 2012 John Maddock. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt

//[safe_prime

#include <nil/crypto3/multiprecision/boost_backends/cpp_int_modular.hpp>
#include <nil/crypto3/multiprecision/boost_backends/miller_rabin.hpp>
#include <iostream>
#include <iomanip>

int main()
{
   using namespace boost::random;
   using namespace nil::crypto3::multiprecision;

   typedef cpp_int int_type;
   mt11213b base_gen(clock());
   independent_bits_engine<mt11213b, 256, int_type> gen(base_gen);
   //
   // We must use a different generator for the tests and number generation, otherwise
   // we get false positives.
   //
   mt19937 gen2(clock());

   for(unsigned i = 0; i < 100000; ++i)
   {
      int_type n = gen();
      if(miller_rabin_test(n, 25, gen2))
      {
         // Value n is probably prime, see if (n-1)/2 is also prime:
         std::cout << "We have a probable prime with value: " << std::hex << std::showbase << n << std::endl;
         if(miller_rabin_test((n-1)/2, 25, gen2))
         {
            std::cout << "We have a safe prime with value: " << std::hex << std::showbase << n << std::endl;
            return 0;
         }
      }
   }
   std::cout << "Ooops, no safe primes were found - probably a bad choice of seed values!" << std::endl;
   return 0;
}

//]
