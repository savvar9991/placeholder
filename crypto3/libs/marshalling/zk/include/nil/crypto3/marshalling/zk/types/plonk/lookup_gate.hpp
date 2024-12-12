//---------------------------------------------------------------------------//
// Copyright (c) 2022-2023 Elena Tatuzova <e.tatuzova@nil.foundation>
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

#ifndef CRYPTO3_MARSHALLING_ZK_PLONK_LOOKUP_GATE_HPP
#define CRYPTO3_MARSHALLING_ZK_PLONK_LOOKUP_GATE_HPP

#include <type_traits>

#include <nil/marshalling/types/bundle.hpp>
#include <nil/marshalling/types/array_list.hpp>
#include <nil/marshalling/types/integral.hpp>
#include <nil/marshalling/status_type.hpp>
#include <nil/marshalling/options.hpp>

#include <nil/crypto3/marshalling/math/types/term.hpp>
#include <nil/crypto3/marshalling/math/types/expression.hpp>

#include <nil/crypto3/marshalling/zk/types/plonk/lookup_constraint.hpp>

namespace nil {
    namespace crypto3 {
        namespace marshalling {
            namespace types {
                template<typename TTypeBase, typename PlonkGate>
                using plonk_lookup_gate = nil::crypto3::marshalling::types::bundle<
                    TTypeBase, std::tuple<
                        // std::size_t selector_index
                        nil::crypto3::marshalling::types::integral<TTypeBase, std::size_t>,
                        // std::vector<plonk_lookup_constraint<FieldType>> constraints
                        nil::crypto3::marshalling::types::standard_array_list<
                            TTypeBase,
                            plonk_lookup_constraint<TTypeBase, typename PlonkGate::constraint_type>
                        >
                    >
                >;

                template<typename Endianness, typename PlonkGate>
                plonk_lookup_gate<nil::crypto3::marshalling::field_type<Endianness>, PlonkGate> fill_plonk_lookup_gate(const PlonkGate &gate) {
                    using TTypeBase = nil::crypto3::marshalling::field_type<Endianness>;
                    using result_type = plonk_lookup_gate<TTypeBase, PlonkGate>;
                    using size_t_marshalling_type = nil::crypto3::marshalling::types::integral<TTypeBase, std::size_t>;

                    using constraint_marshalling_type = plonk_lookup_constraint<TTypeBase, typename PlonkGate::constraint_type>;
                    using constraint_vector_marshalling_type = nil::crypto3::marshalling::types::standard_array_list<
                        TTypeBase, constraint_marshalling_type>;

                    constraint_vector_marshalling_type filled_constraints;
                    for (const auto &constr : gate.constraints) {
                        filled_constraints.value().push_back(
                            fill_plonk_lookup_constraint<Endianness, typename PlonkGate::constraint_type>(constr)
                        );
                    }

                    return result_type(std::make_tuple(size_t_marshalling_type(gate.tag_index), filled_constraints));
                }

                template<typename Endianness, typename PlonkGate>
                PlonkGate make_plonk_lookup_gate(
                    const plonk_lookup_gate<nil::crypto3::marshalling::field_type<Endianness>, PlonkGate> &filled_gate)
                {
                    std::size_t selector_index = std::get<0>(filled_gate.value()).value();
                    std::vector<typename PlonkGate::constraint_type> constraints;

                    for (std::size_t i = 0; i < std::get<1>(filled_gate.value()).value().size(); i++) {
                        constraints.emplace_back(make_plonk_lookup_constraint<Endianness, typename PlonkGate::constraint_type>(
                            std::get<1>(filled_gate.value()).value().at(i)));
                    }

                    return {selector_index, constraints};
                }

                template<typename TTypeBase, typename PlonkGate>
                using plonk_lookup_gates =
                    nil::crypto3::marshalling::types::standard_array_list<
                        TTypeBase, plonk_lookup_gate<TTypeBase, PlonkGate>
                    >;

                template<typename Endianness, typename PlonkGate, typename InputRange>
                plonk_lookup_gates<nil::crypto3::marshalling::field_type<Endianness>, PlonkGate>
                    fill_plonk_lookup_gates(const InputRange &gates) {
                    using TTypeBase = nil::crypto3::marshalling::field_type<Endianness>;
                    using result_type = nil::crypto3::marshalling::types::standard_array_list<
                        TTypeBase, plonk_lookup_gate<TTypeBase, PlonkGate>>;

                    result_type filled_gates;
                    for (const auto &gate : gates) {
                        filled_gates.value().push_back(fill_plonk_lookup_gate<Endianness, PlonkGate>(gate));
                    }

                    return filled_gates;
                }

                template<typename Endianness, typename PlonkGate>
                std::vector<PlonkGate> make_plonk_lookup_gates(
                    const plonk_lookup_gates<nil::crypto3::marshalling::field_type<Endianness>, PlonkGate> &filled_gates)
                {
                    std::vector<PlonkGate> gates;
                    gates.reserve(filled_gates.value().size());
                    for (std::size_t i = 0; i < filled_gates.value().size(); i++) {
                        gates.emplace_back(make_plonk_lookup_gate<Endianness, PlonkGate>(filled_gates.value().at(i)));
                    }
                    return gates;
                }
            }    // namespace types
        }        // namespace marshalling
    }            // namespace crypto3
}    // namespace nil

#endif    // CRYPTO3_MARSHALLING_ZK_PLONK_LOOKUP_GATE_HPP
