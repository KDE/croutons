#pragma once

#include <QNetworkReply>

#include "coroutine_integration.h"

template<>
struct transformer<QNetworkReply*>
{
    static auto transform(QNetworkReply* it) {
        auto future = Future<QNetworkReply*>();

        QObject::connect(it, &QNetworkReply::finished, it, [future, it]() {
            if (it->error() == QNetworkReply::NoError) {
                future.succeed(it);
            } else {
                future.fail(it);
            }
        });

        return future;
    }
};

// auto operator co_await(QNetworkReply* it) noexcept
// {
//     struct Awaiter {
//         FutureBase future;

//         bool await_ready() const noexcept {
//             return future.settled();
//         }
//         void await_suspend(ns::coroutine_handle<> cont) const {
//             future.then([cont](QVariant) mutable {
//                 cont();
//             });
//         }
//         QVariant await_resume() {
//             return future.result();
//         }
//     };

//     return Awaiter{ it };
// }
