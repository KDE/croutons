// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "futurebase.h"

namespace Croutons
{

// anything in here should not be considered part of the public API, and
// may break at any time
namespace detail
{

template<typename T>
class NonVariantFuture
{

};

};

template<typename T>
concept Variantable = QMetaTypeId2<T>::Defined != 0;

template<typename T, typename Error>
requires (!std::is_same_v<T, Error>)
struct Result {
	std::variant<T, Error> it;

	T value() const {
		return std::get<T>(it);
	}
	Error error() const {
		return std::get<Error>(it);
	}

	bool ok() const {
		return std::holds_alternative<T>(it);
	}
};

template<typename T = void, typename ImplClass = std::conditional_t<Variantable<T> || std::is_same_v<T, void>, FutureBase, detail::NonVariantFuture<T>>>
class Future : public ImplClass
{

public:
	using Kind = T;

	Future() : ImplClass() {
	}
	Future(const ImplClass& other) : ImplClass(other) {
	}

	void succeed(const T& it) const {
		ImplClass::succeed(it);
	}
	void fail(const T& it) const {
		ImplClass::fail(it);
	}
	T result() const {
		Q_ASSERT(ImplClass::settled());

		return ImplClass::template get<T>(ImplClass::result());
	}
	void then(std::function<void(T)> callback, std::function<void(T)> orElse = [](T){}) const {
		auto wrap1 = [callback](auto r) { callback(ImplClass::template get<T>(r)); };
		auto wrap2 = [orElse](auto r) { orElse(ImplClass::template get<T>(r)); };

		ImplClass::then(wrap1, wrap2);
	}
	template<typename Function>
	Future<typename std::result_of_t<Function(T)>::Kind>
	flatMap(Function callback) const {
		using NewT = typename std::result_of_t<Function(T)>::Kind;

		Future<NewT> ret;

		auto wrap1 = [callback, ret](QVariant r) {
			auto bret = callback(ImplClass::template get<T>(r));

			bret.then([ret](NewT t) { ret.succeed(t); }, [ret](NewT t) { ret.fail(t); });
		};

		ImplClass::then(wrap1);

		return ret;
	}
	template<typename Function>
	Future<typename std::result_of_t<Function(T)>>
	map(Function callback) const {
		using NewT = std::result_of_t<Function(T)>;
		Future<NewT> ret;

		auto wrap1 = [callback, ret](auto r) {
			ret.succeed(callback(ImplClass::template get<T>(r)));
		};

		ImplClass::then(wrap1);

		return ret;
	}
};

template<>
class Future<void, FutureBase> : public FutureBase
{

public:
	void succeed() const {
		FutureBase::succeed(QVariant());
	}
	void fail() const {
		FutureBase::succeed(QVariant());
	}
	void then(std::function<void()> callback, std::function<void()> orElse = [](){}) const {
		auto wrap1 = [callback](QVariant) { callback(); };
		auto wrap2 = [orElse](QVariant) { orElse(); };
		FutureBase::then(wrap1, wrap2);
	}
};

struct Error {
	QString err;
};

struct Nil {};

template<typename T = Nil, typename Error = Error>
	requires Variantable<T>&& Variantable<Error> && (!std::is_same_v<T, Error>) class FutureResult : public FutureBase
{

public:
	FutureResult() : FutureBase() {
	}
	FutureResult(const FutureBase& other) : FutureBase(other) {
	}
	void succeed(const T& it) const {
		FutureBase::succeed(QVariant::fromValue(it));
	}
	void fail(const Error& it) const {
		FutureBase::fail(QVariant::fromValue(it));
	}
	void finish(const Result<T, Error>& it) const {
		if (it.ok()) {
			succeed(it.value());
		} else {
			fail(it.error());
		}
	}

	Result<T, Error> result() const {
		Q_ASSERT(settled());

		Result<T, Error> res;

		if (!success()) {
			res = Result<T, Error> { .it = FutureBase::result().template value<Error>() };
		} else {
			res = Result<T, Error> { .it = FutureBase::result().template value<T>() };
		}

		return res;
	}
	void then(std::function<void(Result<T, Error>)> callback) const {
		auto wrap = [callback, *this](QVariant) { callback(this->result()); };
		FutureBase::then(wrap, wrap);
	}
	Future<T> toFutureT() {
		Future<T> ret;

		then([ret](Result<T, Error> res) mutable {
			if (res.ok()) {
				ret.succeed(res.value());
			}
		});

		return ret;
	}
	Future<Result<T, Error>> toFutureResultT() {
		Future<Result<T, Error>> ret;

		then([ret](Result<T, Error> res) mutable {
			ret.succeed(res);
		});

		return ret;
	}
};

} // namespace Croutons

Q_DECLARE_METATYPE(Croutons::Error)
Q_DECLARE_METATYPE(Croutons::Nil)
