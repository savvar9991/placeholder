//---------------------------------------------------------------------------//
// Copyright (c) 2018-2021 Mikhail Komarov <nemo@nil.foundation>
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

#pragma once

#include <nil/actor/core/circular_buffer.hh>
#include <nil/actor/core/future.hh>
#include <queue>
#include <nil/actor/detail/std-compat.hh>

namespace nil {
    namespace actor {

        /// Asynchronous single-producer single-consumer queue with limited capacity.
        /// There can be at most one producer-side and at most one consumer-side operation active at any time.
        /// Operations returning a future are considered to be active until the future resolves.
        template<typename T>
        class queue {
            std::queue<T, circular_buffer<T>> _q;
            size_t _max;
            std::optional<promise<>> _not_empty;
            std::optional<promise<>> _not_full;
            std::exception_ptr _ex = nullptr;

        private:
            void notify_not_empty();
            void notify_not_full();

        public:
            explicit queue(size_t size);

            /// \brief Push an item.
            ///
            /// Returns false if the queue was full and the item was not pushed.
            bool push(T &&a);

            /// \brief Pop an item.
            ///
            /// Popping from an empty queue will result in undefined behavior.
            T pop();

            /// \brief access the front element in the queue
            ///
            /// Accessing the front of an empty queue will result in undefined
            /// behaviour.
            T &front();

            /// Consumes items from the queue, passing them to \c func, until \c func
            /// returns false or the queue it empty
            ///
            /// Returns false if func returned false.
            template<typename Func>
            bool consume(Func &&func);

            /// Returns true when the queue is empty.
            bool empty() const;

            /// Returns true when the queue is full.
            bool full() const;

            /// Returns a future<> that becomes available when pop() or consume()
            /// can be called.
            /// A consumer-side operation. Cannot be called concurrently with other consumer-side operations.
            future<> not_empty();

            /// Returns a future<> that becomes available when push() can be called.
            /// A producer-side operation. Cannot be called concurrently with other producer-side operations.
            future<> not_full();

            /// Pops element now or when there is some. Returns a future that becomes
            /// available when some element is available.
            /// If the queue is, or already was, abort()ed, the future resolves with
            /// the exception provided to abort().
            /// A consumer-side operation. Cannot be called concurrently with other consumer-side operations.
            future<T> pop_eventually();

            /// Pushes the element now or when there is room. Returns a future<> which
            /// resolves when data was pushed.
            /// If the queue is, or already was, abort()ed, the future resolves with
            /// the exception provided to abort().
            /// A producer-side operation. Cannot be called concurrently with other producer-side operations.
            future<> push_eventually(T &&data);

            /// Returns the number of items currently in the queue.
            size_t size() const {
                return _q.size();
            }

            /// Returns the size limit imposed on the queue during its construction
            /// or by a call to set_max_size(). If the queue contains max_size()
            /// items (or more), further items cannot be pushed until some are popped.
            size_t max_size() const {
                return _max;
            }

            /// Set the maximum size to a new value. If the queue's max size is reduced,
            /// items already in the queue will not be expunged and the queue will be temporarily
            /// bigger than its max_size.
            void set_max_size(size_t max) {
                _max = max;
                if (!full()) {
                    notify_not_full();
                }
            }

            /// Destroy any items in the queue, and pass the provided exception to any
            /// waiting readers or writers - or to any later read or write attempts.
            void abort(std::exception_ptr ex) {
                while (!_q.empty()) {
                    _q.pop();
                }
                _ex = ex;
                if (_not_full) {
                    _not_full->set_exception(ex);
                    _not_full = std::nullopt;
                }
                if (_not_empty) {
                    _not_empty->set_exception(std::move(ex));
                    _not_empty = std::nullopt;
                }
            }

            /// \brief Check if there is an active consumer
            ///
            /// Returns true if another fiber waits for an item to be pushed into the queue
            bool has_blocked_consumer() const {
                return bool(_not_empty);
            }
        };

        template<typename T>
        inline queue<T>::queue(size_t size) : _max(size) {
        }

        template<typename T>
        inline void queue<T>::notify_not_empty() {
            if (_not_empty) {
                _not_empty->set_value();
                _not_empty = std::optional<promise<>>();
            }
        }

        template<typename T>
        inline void queue<T>::notify_not_full() {
            if (_not_full) {
                _not_full->set_value();
                _not_full = std::optional<promise<>>();
            }
        }

        template<typename T>
        inline bool queue<T>::push(T &&data) {
            if (_q.size() < _max) {
                _q.push(std::move(data));
                notify_not_empty();
                return true;
            } else {
                return false;
            }
        }

        template<typename T>
        inline T &queue<T>::front() {
            return _q.front();
        }

        template<typename T>
        inline T queue<T>::pop() {
            if (_q.size() == _max) {
                notify_not_full();
            }
            T data = std::move(_q.front());
            _q.pop();
            return data;
        }

        template<typename T>
        inline future<T> queue<T>::pop_eventually() {
            if (_ex) {
                return make_exception_future<T>(_ex);
            }
            if (empty()) {
                return not_empty().then([this] {
                    if (_ex) {
                        return make_exception_future<T>(_ex);
                    } else {
                        return make_ready_future<T>(pop());
                    }
                });
            } else {
                return make_ready_future<T>(pop());
            }
        }

        template<typename T>
        inline future<> queue<T>::push_eventually(T &&data) {
            if (_ex) {
                return make_exception_future<>(_ex);
            }
            if (full()) {
                return not_full().then([this, data = std::move(data)]() mutable {
                    _q.push(std::move(data));
                    notify_not_empty();
                });
            } else {
                _q.push(std::move(data));
                notify_not_empty();
                return make_ready_future<>();
            }
        }

        template<typename T>
        template<typename Func>
        inline bool queue<T>::consume(Func &&func) {
            if (_ex) {
                std::rethrow_exception(_ex);
            }
            bool running = true;
            while (!_q.empty() && running) {
                running = func(std::move(_q.front()));
                _q.pop();
            }
            if (!full()) {
                notify_not_full();
            }
            return running;
        }

        template<typename T>
        inline bool queue<T>::empty() const {
            return _q.empty();
        }

        template<typename T>
        inline bool queue<T>::full() const {
            return _q.size() >= _max;
        }

        template<typename T>
        inline future<> queue<T>::not_empty() {
            if (_ex) {
                return make_exception_future<>(_ex);
            }
            if (!empty()) {
                return make_ready_future<>();
            } else {
                _not_empty = promise<>();
                return _not_empty->get_future();
            }
        }

        template<typename T>
        inline future<> queue<T>::not_full() {
            if (_ex) {
                return make_exception_future<>(_ex);
            }
            if (!full()) {
                return make_ready_future<>();
            } else {
                _not_full = promise<>();
                return _not_full->get_future();
            }
        }

    }    // namespace actor
}    // namespace nil

