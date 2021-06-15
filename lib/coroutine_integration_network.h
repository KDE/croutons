// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

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
