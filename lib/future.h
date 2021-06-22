// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include "futurebase.h"

template<typename T>
concept Variantable = requires(T a) {
	QVariant::fromValue(a);
};

template<typename T, typename Error>
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

struct Nil {};
Q_DECLARE_METATYPE(Nil)

template<typename T = Nil>
requires (Variantable<T> || std::is_same<T, void>::value) class Future : public FutureBase
{

public:
	void succeed(const T& it) const {
		FutureBase::succeed(QVariant::fromValue(it));
	}
	void fail(const T& it) const {
		FutureBase::succeed(QVariant::fromValue(it));
	}
	T result() const {
		Q_ASSERT(settled());

		return FutureBase::result().template value<T>();
	}
	void then(std::function<void(T)> then, std::function<void(T)> orElse = [](T){}) const {
		auto wrap1 = [then](QVariant r) { then(qvariant_cast<T>(r)); };
		auto wrap2 = [orElse](QVariant r) { orElse(qvariant_cast<T>(r)); };
		FutureBase::then(wrap1, wrap2);
	}
};

template<>
class Future<void> : public FutureBase
{

public:
	void succeed() const {
		FutureBase::succeed(QVariant());
	}
	void fail() const {
		FutureBase::succeed(QVariant());
	}
	void then(std::function<void()> then, std::function<void()> orElse = [](){}) const {
		auto wrap1 = [then](QVariant) { then(); };
		auto wrap2 = [orElse](QVariant) { orElse(); };
		FutureBase::then(wrap1, wrap2);
	}
};

struct Error {
	QString err;
};
Q_DECLARE_METATYPE(Error)

template<typename T = Nil, typename Error = Error>
	requires Variantable<T>&& Variantable<Error> && (!std::is_same<T, Error>::value) class FutureResult : public FutureBase
{

public:
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
	void then(std::function<void(Result<T, Error>)> then) const {
		auto wrap = [then, this](QVariant) { then(this->result()); };
		FutureBase::then(wrap, wrap);
	}
};
