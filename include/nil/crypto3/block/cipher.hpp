//---------------------------------------------------------------------------//
// Copyright (c) 2018-2019 Nil Foundation AG
// Copyright (c) 2018-2019 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//---------------------------------------------------------------------------//

#ifndef CRYPTO3_BLOCK_CIPHER_HPP
#define CRYPTO3_BLOCK_CIPHER_HPP

namespace nil {
    namespace crypto3 {
        namespace block {
            /*!
             * @defgroup block Block Ciphers
             *
             * @brief Block ciphers are a n-bit permutation for some small ```n```,
             * typically 64 or 128 bits. It is a cryptographic primitive used
             * to generate higher level operations such as authenticated encryption.
             */

            /*!
             * @brief
             * @tparam Cipher
             * @tparam Mode
             * @tparam Padding
             */
            template<typename Cipher, typename Mode, typename Padding>
            struct cipher : public Mode::template bind<Cipher, Padding>::type {
                typedef std::size_t size_type;

                typedef Cipher cipher_type;
                typedef Mode mode_type;
                typedef Padding padding_strategy;
            };
        }    // namespace block
    }        // namespace crypto3
}    // namespace nil

#endif
