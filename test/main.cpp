// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <QGuiApplication>
#include <QTimer>

#include <QQmlApplicationEngine>

#include "coroutine_integration.h"
#include "coroutine_integration_network.h"

FutureResult<> timer(int duration) {
    FutureResult it;

    QTimer::singleShot(duration, [it]() {
        it.succeed({});
    });

    return it;
}

Future<int> futureMain() {
    auto then = QTime::currentTime();

    co_await timer(1500);

    auto now = QTime::currentTime();

    Q_ASSERT(then.msecsTo(now) > 1000);

    co_return 0;
}

Future<int> something() {
    co_return 0;
}

void anotherThing() {
    something().then([](int i) {

    });
}

FutureResult<int> yetAnotherThing() {
    co_return 0;
}

void wawajete() {
    yetAnotherThing().then([](Result<int, Error> r) {

    });
}

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);

    futureMain().then([&app](QVariant ret) {
        app.exit(ret.toInt());
    });

    return app.exec();
}
