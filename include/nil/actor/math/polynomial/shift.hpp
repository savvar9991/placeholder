//---------------------------------------------------------------------------//
// Copyright (c) 2021 Mikhail Komarov <nemo@nil.foundation>
// Copyright (c) 2021 Nikita Kaskov <nbering@nil.foundation>
//
// MIT License
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//---------------------------------------------------------------------------//

#ifndef ACTOR_MATH_POLYNOMIAL_SHIFT_HPP
#define ACTOR_MATH_POLYNOMIAL_SHIFT_HPP

#include <nil/actor/math/polynomial/polynomial.hpp>
#include <nil/actor/math/polynomial/polynomial_dfs.hpp>

namespace nil {
    namespace actor {
        namespace math {
            template<typename FieldValueType>
            static inline polynomial<FieldValueType>
                polynomial_shift(const polynomial<FieldValueType> &f,
                                 const FieldValueType &x) {
                polynomial<FieldValueType> f_shifted(f);
                FieldValueType x_power = x;
                for (int i = 1; i < f.size(); i++) {
                    f_shifted[i] = f_shifted[i] * x_power;
                    x_power *= x;
                }

                return f_shifted;
            }

            template<typename FieldValueType>
            static inline polynomial_dfs<FieldValueType>
                polynomial_shift(const polynomial_dfs<FieldValueType> &f,
                                 const int shift,
                                 std::size_t domain_size = 0) {
                if ((domain_size == 0) && (f.size() > 0)) {
                    domain_size = f.size() - 1;
                }

                assert(domain_size <= f.size() - 1);

                polynomial_dfs<FieldValueType> f_shifted(f.degree(), f.size());

                for (std::size_t index = 0; index < f.size(); index++){
                    f_shifted[index] = f[(domain_size + 1 + index + shift) % (domain_size + 1)];
                }

                return f_shifted;
            }
        }    // namespace math
    }        // namespace actor
}    // namespace nil

#endif    // CRYPTO3_ZK_PLONK_REDSHIFT_POLYNOMIAL_SHIFT_HPP