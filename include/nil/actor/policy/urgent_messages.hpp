//---------------------------------------------------------------------------//
// Copyright (c) 2011-2017 Dominik Charousset
// Copyright (c) 2017-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the terms and conditions of the BSD 3-Clause License or
// (at your option) under the terms and conditions of the Boost Software
// License 1.0. See accompanying files LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt.
//---------------------------------------------------------------------------//

#pragma once


#include <nil/actor/fwd.hpp>
#include <nil/actor/mailbox_element.hpp>
#include <nil/actor/unit.hpp>

namespace nil::actor::policy {

    /// Configures a cached DRR queue for holding asynchronous messages with
    /// default priority.
    class BOOST_SYMBOL_VISIBLE urgent_messages {
    public:
        // -- member types -----------------------------------------------------------

        using mapped_type = mailbox_element;

        using task_size_type = size_t;

        using deficit_type = size_t;

        using unique_pointer = mailbox_element_ptr;

        // -- constructors, destructors, and assignment operators --------------------

        urgent_messages() = default;

        urgent_messages(const urgent_messages &) = default;

        urgent_messages &operator=(const urgent_messages &) = default;

        constexpr urgent_messages(unit_t) {
            // nop
        }

        // -- interface required by drr_queue ----------------------------------------

        static inline task_size_type task_size(const mailbox_element &) noexcept {
            return 1;
        }
    };

}    // namespace nil::actor::policy