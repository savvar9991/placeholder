//---------------------------------------------------------------------------//
// Copyright (c) 2011-2018 Dominik Charousset
// Copyright (c) 2017-2020 Mikhail Komarov <nemo@nil.foundation>
//
// Distributed under the terms and conditions of the BSD 3-Clause License or
// (at your option) under the terms and conditions of the Boost Software
// License 1.0. See accompanying files LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt.
//---------------------------------------------------------------------------//

#include <nil/actor/node_id.hpp>

#include <cstring>
#include <ctype.h>
#include <iterator>
#include <random>

#include <nil/crypto3/hash/algorithm/hash.hpp>
#include <nil/crypto3/hash/ripemd.hpp>

#include <nil/actor/binary_deserializer.hpp>
#include <nil/actor/binary_serializer.hpp>
#include <nil/actor/config.hpp>
#include <nil/actor/deserializer.hpp>
#include <nil/actor/detail/append_hex.hpp>
#include <nil/actor/detail/get_mac_addresses.hpp>
#include <nil/actor/detail/get_process_id.hpp>
#include <nil/actor/detail/get_root_uuid.hpp>
#include <nil/actor/detail/parse.hpp>
#include <nil/actor/detail/parser/ascii_to_int.hpp>
#include <nil/actor/expected.hpp>
#include <nil/actor/logger.hpp>
#include <nil/actor/make_counted.hpp>
#include <nil/actor/parser_state.hpp>
#include <nil/actor/sec.hpp>
#include <nil/actor/serializer.hpp>
#include <nil/actor/string_algorithms.hpp>

using namespace nil::crypto3;

namespace nil {
    namespace actor {

        node_id::data::~data() {
            // nop
        }

        node_id::default_data::default_data() : pid_(0) {
            memset(host_.data(), 0, host_.size());
        }

        node_id::default_data::default_data(uint32_t pid, const host_id_type &host) : pid_(pid), host_(host) {
            // nop
        }

        namespace {

            std::atomic<uint8_t> system_id;

        }    // namespace

        node_id node_id::default_data::local(const spawner_config &) {
            ACTOR_LOG_TRACE("");
            auto ifs = detail::get_mac_addresses();
            std::vector<std::string> macs;
            macs.reserve(ifs.size());
            for (auto &i : ifs)
                macs.emplace_back(std::move(i.second));
            auto seeded_hd_serial_and_mac_addr = join(macs, "") + detail::get_root_uuid();
            // By adding 8 random ASCII characters, we make sure to assign a new (random)
            // ID to a node every time we start it. Otherwise, we might run into issues
            // where a restarted node produces identical actor IDs than the node it
            // replaces. Especially when running nodes in a container, because then the
            // process ID is most likely the same.
            std::random_device rd;
            std::minstd_rand gen {rd()};
            std::uniform_int_distribution<> dis(33, 126);
            for (int i = 0; i < 8; ++i)
                seeded_hd_serial_and_mac_addr += static_cast<char>(dis(gen));
            // One final tweak: we add another character that makes sure two actor systems
            // in the same process won't have the same node ID - even if the user
            // manipulates the system to always produce the same seed for its randomness.
            auto sys_seed = static_cast<char>(system_id.fetch_add(1) + 33);
            seeded_hd_serial_and_mac_addr += sys_seed;
            host_id_type hid = hash::hash<hash::ripemd<160>>(seeded_hd_serial_and_mac_addr);
            return make_node_id(detail::get_process_id(), hid);
        }

        bool node_id::default_data::valid(const host_id_type &x) noexcept {
            auto is_zero = [](uint8_t x) { return x == 0; };
            return !std::all_of(x.begin(), x.end(), is_zero);
        }

        bool node_id::default_data::can_parse(string_view str) noexcept {
            // Our format is "<20-byte-hex>#<pid>". With 2 characters per byte, this means
            // a valid node ID has at least 42 characters.
            if (str.size() < 42)
                return false;
            string_parser_state ps {str.begin(), str.end()};
            for (size_t i = 0; i < 40; ++i)
                if (!ps.consume_strict_if(isxdigit))
                    return false;
            if (!ps.consume_strict('#'))
                return false;
            // We don't care for the value, but we invoke the actual number parser to make
            // sure the value is in bounds.
            uint32_t dummy;
            detail::parse(ps, dummy);
            return ps.code == pec::success;
        }

        bool node_id::default_data::valid() const noexcept {
            return pid_ != 0 && valid(host_);
        }

        size_t node_id::default_data::hash_code() const noexcept {
            // XOR the first few bytes from the node ID and the process ID.
            auto x = static_cast<size_t>(pid_);
            size_t y;
            memcpy(&y, host_.data(), sizeof(size_t));
            return x ^ y;
        }

        uint8_t node_id::default_data::implementation_id() const noexcept {
            return class_id;
        }

        int node_id::default_data::compare(const data &other) const noexcept {
            if (this == &other)
                return 0;
            auto other_id = other.implementation_id();
            if (class_id != other_id)
                return class_id < other_id ? -1 : 1;
            auto &x = static_cast<const default_data &>(other);
            if (pid_ != x.pid_)
                return pid_ < x.pid_ ? -1 : 1;
            return memcmp(host_.data(), x.host_.data(), host_.size());
        }

        void node_id::default_data::print(std::string &dst) const {
            if (!valid()) {
                dst += "invalid-node";
                return;
            }
            detail::append_hex(dst, host_);
            dst += '#';
            dst += std::to_string(pid_);
        }

        error node_id::default_data::serialize(serializer &sink) const {
            return sink(pid_, host_);
        }

        error node_id::default_data::deserialize(deserializer &source) {
            return source(pid_, host_);
        }

        error_code<sec> node_id::default_data::serialize(binary_serializer &sink) const {
            return sink(pid_, host_);
        }

        error_code<sec> node_id::default_data::deserialize(binary_deserializer &source) {
            return source(pid_, host_);
        }

        node_id::uri_data::uri_data(uri value) : value_(std::move(value)) {
            // nop
        }

        bool node_id::uri_data::valid() const noexcept {
            return !value_.empty();
        }

        size_t node_id::uri_data::hash_code() const noexcept {
            std::hash<uri> f;
            return f(value_);
        }

        uint8_t node_id::uri_data::implementation_id() const noexcept {
            return class_id;
        }

        int node_id::uri_data::compare(const data &other) const noexcept {
            if (this == &other)
                return 0;
            auto other_id = other.implementation_id();
            if (class_id != other_id)
                return class_id < other_id ? -1 : 1;
            return value_.compare(static_cast<const uri_data &>(other).value_);
        }

        void node_id::uri_data::print(std::string &dst) const {
            if (!valid()) {
                dst += "invalid-node";
                return;
            }
            dst += to_string(value_);
        }

        error node_id::uri_data::serialize(serializer &sink) const {
            return sink(value_);
        }

        error node_id::uri_data::deserialize(deserializer &source) {
            return source(value_);
        }

        error_code<sec> node_id::uri_data::serialize(binary_serializer &sink) const {
            return sink(value_);
        }

        error_code<sec> node_id::uri_data::deserialize(binary_deserializer &source) {
            return source(value_);
        }

        node_id &node_id::operator=(const none_t &) {
            data_.reset();
            return *this;
        }

        node_id::node_id(intrusive_ptr<data> data) : data_(std::move(data)) {
            // nop
        }

        node_id::~node_id() {
            // nop
        }

        node_id::operator bool() const {
            return static_cast<bool>(data_);
        }

        int node_id::compare(const node_id &other) const noexcept {
            if (this == &other || data_ == other.data_)
                return 0;
            if (data_ == nullptr)
                return other.data_ == nullptr ? 0 : -1;
            return other.data_ == nullptr ? 1 : data_->compare(*other.data_);
        }

        void node_id::swap(node_id &x) {
            data_.swap(x.data_);
        }

        bool node_id::can_parse(string_view str) noexcept {
            return default_data::can_parse(str) || uri::can_parse(str);
        }

        namespace {

            template<class Serializer>
            auto serialize_data(Serializer &sink, const intrusive_ptr<node_id::data> &ptr) {
                if (ptr && ptr->valid()) {
                    if (auto err = sink(ptr->implementation_id()))
                        return err;
                    return ptr->serialize(sink);
                }
                uint8_t zero = 0;
                return sink(zero);
            }

            template<class Deserializer>
            auto deserialize_data(Deserializer &source, intrusive_ptr<node_id::data> &ptr) {
                using result_type = typename Deserializer::result_type;
                uint8_t impl = 0;
                if (auto err = source(impl))
                    return err;
                if (impl == 0) {
                    ptr.reset();
                    return result_type {};
                }
                if (impl == node_id::default_data::class_id) {
                    if (ptr == nullptr || ptr->implementation_id() != node_id::default_data::class_id)
                        ptr = make_counted<node_id::default_data>();
                    return ptr->deserialize(source);
                } else if (impl == node_id::uri_data::class_id) {
                    if (ptr == nullptr || ptr->implementation_id() != node_id::uri_data::class_id)
                        ptr = make_counted<node_id::uri_data>();
                    return ptr->deserialize(source);
                }
                return result_type {sec::unknown_type};
            }

        }    // namespace

        error inspect(serializer &sink, node_id &x) {
            return serialize_data(sink, x.data_);
        }

        error inspect(deserializer &source, node_id &x) {
            return deserialize_data(source, x.data_);
        }

        error_code<sec> inspect(binary_serializer &sink, node_id &x) {
            return serialize_data(sink, x.data_);
        }

        error_code<sec> inspect(binary_deserializer &source, node_id &x) {
            return deserialize_data(source, x.data_);
        }

        void append_to_string(std::string &str, const node_id &x) {
            if (x != none)
                x->print(str);
            else
                str += "invalid-node";
        }

        std::string to_string(const node_id &x) {
            std::string result;
            append_to_string(result, x);
            return result;
        }

        node_id make_node_id(uri from) {
            auto ptr = make_counted<node_id::uri_data>(std::move(from));
            return node_id {std::move(ptr)};
        }

        node_id make_node_id(uint32_t process_id, const node_id::default_data::host_id_type &host_id) {
            auto ptr = make_counted<node_id::default_data>(process_id, host_id);
            return node_id {std::move(ptr)};
        }

        optional<node_id> make_node_id(uint32_t process_id, string_view host_hash) {
            using node_data = node_id::default_data;
            if (host_hash.size() != node_data::host_id_size * 2)
                return none;
            detail::parser::ascii_to_int<16, uint8_t> xvalue;
            node_data::host_id_type host_id;
            auto in = host_hash.begin();
            for (auto &byte : host_id) {
                if (!isxdigit(*in))
                    return none;
                auto first_nibble = (xvalue(*in++) << 4);
                if (!isxdigit(*in))
                    return none;
                byte = static_cast<uint8_t>(first_nibble | xvalue(*in++));
            }
            if (!node_data::valid(host_id))
                return none;
            return make_node_id(process_id, host_id);
        }

        error parse(string_view str, node_id &dest) {
            if (node_id::default_data::can_parse(str)) {
                ACTOR_ASSERT(str.size() >= 42);
                auto host_hash = str.substr(0, 40);
                auto pid_str = str.substr(41);
                uint32_t pid_val = 0;
                if (auto err = detail::parse(pid_str, pid_val))
                    return err;
                if (auto nid = make_node_id(pid_val, host_hash)) {
                    dest = std::move(*nid);
                    return none;
                }
                ACTOR_LOG_ERROR("make_node_id failed after can_parse returned true");
                return sec::invalid_argument;
            }
            if (auto nid_uri = make_uri(str)) {
                dest = make_node_id(std::move(*nid_uri));
                return none;
            } else {
                return std::move(nid_uri.error());
            }
        }

    }    // namespace actor
}    // namespace nil
