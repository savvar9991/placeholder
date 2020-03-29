//---------------------------------------------------------------------------//
// Copyright (c) 2011-2018 Dominik Charousset
// Copyright (c) 2017-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the terms and conditions of the BSD 3-Clause License or
// (at your option) under the terms and conditions of the Boost Software
// License 1.0. See accompanying files LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt.
//---------------------------------------------------------------------------//

#pragma once

#include <nil/actor/meta/annotation.hpp>

namespace nil::actor::meta {

    struct omittable_t : annotation {
        constexpr omittable_t() {
            // nop
        }
    };

    /// Allows an inspector to omit the following data field
    /// unconditionally when producing human-friendly output.
    constexpr omittable_t omittable() {
        return {};
    }

}    // namespace nil::actor::meta