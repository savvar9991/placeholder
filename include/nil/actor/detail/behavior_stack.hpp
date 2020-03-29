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

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include <nil/actor/behavior.hpp>
#include <nil/actor/config.hpp>

#include <nil/actor/mailbox_element.hpp>
#include <nil/actor/message_id.hpp>
#include <nil/actor/optional.hpp>

namespace nil::actor::detail {

    struct behavior_stack_mover;

    class BOOST_SYMBOL_VISIBLE behavior_stack {
    public:
        friend struct behavior_stack_mover;

        behavior_stack(const behavior_stack &) = delete;
        behavior_stack &operator=(const behavior_stack &) = delete;

        behavior_stack() = default;

        // erases the last (asynchronous) behavior
        void pop_back();

        void clear();

        inline bool empty() const {
            return elements_.empty();
        }

        inline behavior &back() {
            ACTOR_ASSERT(!empty());
            return elements_.back();
        }

        inline void push_back(behavior &&what) {
            elements_.emplace_back(std::move(what));
        }

        template<class... Ts>
        inline void emplace_back(Ts &&... xs) {
            elements_.emplace_back(std::forward<Ts>(xs)...);
        }

        inline void cleanup() {
            erased_elements_.clear();
        }

    private:
        std::vector<behavior> elements_;
        std::vector<behavior> erased_elements_;
    };

}    // namespace nil::actor::detail