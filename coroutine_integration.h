#pragma once

#include "future.h"

#if defined(__clang__)

#include <experimental/coroutine>
namespace ns = std::experimental;

#elif defined(__GNUC__)

#include <coroutine>
namespace ns = std;

#endif

template<typename ...Args>
struct ns::coroutine_traits<FutureBase, Args...> {
    struct promise_type {
        private: FutureBase t;
        public: promise_type() {
            t = FutureBase {};
        }

        FutureBase get_return_object() noexcept {
            return t;
        }

        ns::suspend_never initial_suspend() const noexcept { return {}; }
        ns::suspend_never final_suspend() const noexcept { return {}; }

        void return_value(const QVariant& value) noexcept
        {
            t.succeed(value);
        }
        void return_value(QVariant &&value) noexcept
        {
            t.succeed(std::move(value));
        }

        void unhandled_exception() noexcept {
            Q_ASSERT("unhandled exception");
        }
    };
};

template<typename T, typename Error, typename ...Args>
struct ns::coroutine_traits<Future<T, Error>, Args...> {
    struct promise_type {
        private: Future<T, Error> v;
        public: promise_type() {
            v = Future<T, Error> {};
        }

        Future<T, Error> get_return_object() noexcept {
            return v;
        }

        ns::suspend_never initial_suspend() const noexcept { return {}; }
        ns::suspend_never final_suspend() const noexcept { return {}; }

        void return_value(const Res<T, Error>& value) noexcept
        {
            v.finish(value);
        }
        void return_value(Res<T, Error> &&value) noexcept
        {
            v.finish(std::move(value));
        }

        void unhandled_exception() noexcept {
            Q_ASSERT("unhandled exception");
        }
    };
};

auto operator co_await(FutureBase it) noexcept
{
    struct Awaiter {
        FutureBase future;

        bool await_ready() const noexcept {
            return future.settled();
        }
        void await_suspend(ns::coroutine_handle<> cont) const {
            future.then([cont](QVariant) mutable {
                cont();
            });
        }
        QVariant await_resume() {
            return future.result();
        }
    };

    return Awaiter{ it };
}

template<typename T, typename Error> auto operator co_await(Future<T, Error> it) noexcept
{
    struct Awaiter {
        Future<T, Error> future;

        bool await_ready() const noexcept {
            return future.settled();
        }
        void await_suspend(ns::coroutine_handle<> cont) const {
            future.then([cont](QVariant) mutable {
                cont();
            });
        }
        Result<T, Error> await_resume() {
            return Result<T, Error>{future.result()};
        }
    };

    return Awaiter{ it };
}
