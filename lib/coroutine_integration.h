// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "future.h"

#if defined(__clang__)

#include <experimental/coroutine>
namespace coroutine_namespace = std::experimental;

#elif defined(__GNUC__)

#include <coroutine>
namespace coroutine_namespace = std;

#endif

// responsible for providing the overload of await_transform
template<typename T>
struct transformer {
	static T transform(const T& t) {
		return t;
	}
};

template<typename... Args>
struct coroutine_namespace::coroutine_traits<FutureBase, Args...> {
	struct promise_type {
	private:
		FutureBase t;

	public:
		FutureBase get_return_object() noexcept {
			return t;
		}

		coroutine_namespace::suspend_never initial_suspend() const noexcept { return {}; }
		coroutine_namespace::suspend_never final_suspend() const noexcept { return {}; }

		void return_value(const QVariant& value) noexcept {
			t.succeed(value);
		}
		void return_value(QVariant&& value) noexcept {
			t.succeed(std::move(value));
		}

		template<typename Awaited>
		auto await_transform(const Awaited& t) {
			return transformer<Awaited>::transform(t);
		}

		void unhandled_exception() noexcept {
			Q_ASSERT("unhandled exception");
		}
	};
};

inline auto operator co_await(FutureBase it) noexcept {
	struct Awaiter {
		FutureBase future;

		bool await_ready() const noexcept {
			return future.settled();
		}
		void await_suspend(coroutine_namespace::coroutine_handle<> cont) const {
			future.then([cont](QVariant) mutable { cont(); }, [cont](QVariant) mutable { cont(); });
		}
		QVariant await_resume() {
			return future.result();
		}
	};

	return Awaiter{it};
}

template<typename T, typename... Args>
struct coroutine_namespace::coroutine_traits<Future<T>, Args...> {
	struct promise_type {
	private:
		Future<T> v;

	public:
		Future<T> get_return_object() noexcept {
			return v;
		}

		coroutine_namespace::suspend_never initial_suspend() const noexcept { return {}; }
		coroutine_namespace::suspend_never final_suspend() const noexcept { return {}; }

		void return_value(const T& value) noexcept {
			v.succeed(value);
		}
		void return_value(T&& value) noexcept {
			v.succeed(std::move(value));
		}

		template<typename Awaited>
		auto await_transform(const Awaited& t) {
			return transformer<Awaited>::transform(t);
		}

		void unhandled_exception() noexcept {
			Q_ASSERT("unhandled exception");
		}
	};
};

template<typename... Args>
struct coroutine_namespace::coroutine_traits<Future<void>, Args...> {
	struct promise_type {
	private:
		Future<void> v;

	public:
		Future<void> get_return_object() noexcept {
			return v;
		}

		coroutine_namespace::suspend_never initial_suspend() const noexcept { return {}; }
		coroutine_namespace::suspend_never final_suspend() const noexcept { return {}; }

		void return_void() noexcept {
			v.succeed();
		}

		template<typename Awaited>
		auto await_transform(const Awaited& t) {
			return transformer<Awaited>::transform(t);
		}

		void unhandled_exception() noexcept {
			Q_ASSERT("unhandled exception");
		}
	};
};

template<typename T>
auto operator co_await(Future<T> it) noexcept {
	struct Awaiter {
		Future<T> future;

		bool await_ready() const noexcept {
			return future.settled();
		}
		void await_suspend(coroutine_namespace::coroutine_handle<> cont) const {
			future.then([cont](T) mutable { cont(); }, [cont](T) mutable { cont(); });
		}
		T await_resume() {
			return future.result();
		}
	};

	return Awaiter{it};
}

template<>
inline auto operator co_await(Future<void> it) noexcept {
	struct Awaiter {
		Future<void> future;

		bool await_ready() const noexcept {
			return future.settled();
		}
		void await_suspend(coroutine_namespace::coroutine_handle<> cont) const {
			future.then([cont]() mutable { cont(); }, [cont]() mutable { cont(); });
		}
		void await_resume() {
			return;
		}
	};

	return Awaiter{it};
}

template<typename T, typename Error, typename... Args>
struct coroutine_namespace::coroutine_traits<FutureResult<T, Error>, Args...> {
	struct promise_type {
	private:
		FutureResult<T, Error> v;

	public:
		FutureResult<T, Error> get_return_object() noexcept {
			return v;
		}

		coroutine_namespace::suspend_never initial_suspend() const noexcept { return {}; }
		coroutine_namespace::suspend_never final_suspend() const noexcept { return {}; }

		void return_value(const Result<T, Error>& value) noexcept {
			v.finish(value);
		}
		void return_value(const T& value) noexcept {
			v.succeed(value);
		}
		void return_value(const Error& value) noexcept {
			v.fail(value);
		}
		void return_value(Result<T, Error>&& value) noexcept {
			v.finish(std::move(value));
		}

		template<typename Awaited>
		auto await_transform(const Awaited& t) {
			return transformer<Awaited>::transform(t);
		}

		void unhandled_exception() noexcept {
			Q_ASSERT("unhandled exception");
		}
	};
};

template<typename T, typename Error>
auto operator co_await(FutureResult<T, Error> it) noexcept {
	struct Awaiter {
		FutureResult<T, Error> future;

		bool await_ready() const noexcept {
			return future.settled();
		}
		void await_suspend(coroutine_namespace::coroutine_handle<> cont) const {
			future.then([cont](Result<T ,Error>) mutable { cont(); });
		}
		Result<T, Error> await_resume() {
			return Result<T, Error>{future.result()};
		}
	};

	return Awaiter{it};
}
