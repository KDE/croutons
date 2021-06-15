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
using Res = std::variant<T, Error>;

template<typename T, typename Error>
struct Result {
    Res<T, Error> it;

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

template<typename T = Nil> requires Variantable<T>
class Future : public FutureBase
{

public:
    void succeed(const T& it) const {
        FutureBase::succeed( QVariant::fromValue(it) );
    }
    void fail(const T& it) const {
        FutureBase::succeed( QVariant::fromValue(it) );
    }
    T result() const {
        Q_ASSERT(settled());

        return FutureBase::result().template value<T>();
    }
};

struct Error {
    QString err;
};
Q_DECLARE_METATYPE(Error)

template<typename T = Nil, typename Error = Error> requires Variantable<T> && Variantable<Error> && (!std::is_same<T, Error>::value)
class FutureResult : public FutureBase
{

public:
    void succeed(const T& it) const {
        FutureBase::succeed( QVariant::fromValue(it) );
    }
    void fail(const Error& it) const {
        FutureBase::fail( QVariant::fromValue(it) );
    }
    void finish(const Res<T, Error>& it) const {
        if (std::holds_alternative<T>(it)) {
            succeed(std::get<T>(it));
        } else {
            fail(std::get<Error>(it));
        }
    }

    Res<T, Error> result() const {
        Q_ASSERT(settled());

        Res<T, Error> res;

        if (!success()) {
            res = FutureBase::result().template value<Error>();
        } else {
            res = FutureBase::result().template value<T>();
        }

        return res;
    }
};
