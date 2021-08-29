// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "coroutine_integration.h"

#include <functional>
#include <type_traits>

#define perform co_await

template<typename T>
class Effect {
	struct Holder {
		T t;
	};
	QSharedPointer<Holder> val;

public:
	Effect() {
		val.reset(new Holder);
	}
	Effect(const Effect& other) {
		val = other.val;
	}
	operator T() const {
		return val->t;
	}

	struct promise_type {
	private:
		Effect<T> t;

	public:
		promise_type() {
			t = Effect<T>{};
		}

		Effect<T> get_return_object() noexcept {
			return t;
		}

		coroutine_namespace::suspend_never initial_suspend() const noexcept { return {}; }
		coroutine_namespace::suspend_never final_suspend() const noexcept { return {}; }

		void return_value(const T& value) noexcept {
			t.val->t = value;
		}
		void return_value(T&& value) noexcept {
			t.val->t = std::move(value);
		}

		void unhandled_exception() noexcept {
			Q_ASSERT("unhandled exception");
		}
	};
};

template<>
class Effect<void> {
};

template<typename ...Args>
struct coroutine_namespace::coroutine_traits<Effect<void>, Args...>
{
	struct promise_type {
	private:
		Effect<void> t;

	public:
		promise_type() {
			t = Effect<void>{};
		}

		Effect<void> get_return_object() noexcept {
			return t;
		}

		coroutine_namespace::suspend_never initial_suspend() const noexcept { return {}; }
		coroutine_namespace::suspend_never final_suspend() const noexcept { return {}; }

		void return_void() noexcept {
		}

		void unhandled_exception() noexcept {
			Q_ASSERT("unhandled exception");
		}
	};
};

template<typename Fn>
class EffectVoidFun {
	static thread_local QList<Fn> items;

public:

	template<typename... Args>
	EffectVoidFun(Args&&... args) {
		items.last()(std::forward<Args>(args)...);
	}

	static auto handler(Fn fn) {
		items << fn;
		struct Deleter {
			~Deleter() {
				items.pop_back();
			}
		};
		return Deleter{};
	}

	bool await_ready() const noexcept {
		return true;
	}
	void await_suspend(coroutine_namespace::coroutine_handle<> cont) const {
		cont();
	}
	void await_resume() {
		return;
	}
};

template<typename T>
inline thread_local QList<T> EffectVoidFun<T>::items;

template<typename Fn>
class EffectFun {
	static thread_local QList<Fn> items;
	typename Fn::result_type ret;

public:

	template<typename... Args>
	EffectFun(Args&&... args) {
		ret = items.last()(std::forward<Args>(args)...);
	}

	static auto handler(Fn fn) {
		items << fn;
		struct Deleter {
			~Deleter() {
				items.pop_back();
			}
		};
		return Deleter{};
	}

	bool await_ready() const noexcept {
		return true;
	}
	void await_suspend(coroutine_namespace::coroutine_handle<> cont) const {
		cont();
	}
	typename Fn::result_type await_resume() {
		return ret;
	}
};

template<typename T>
inline thread_local QList<T> EffectFun<T>::items;
