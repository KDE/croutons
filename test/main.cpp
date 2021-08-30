// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <QGuiApplication>
#include <QTimer>

#include <QQmlApplicationEngine>

#include "coroutine_integration.h"
#include "coroutine_integration_network.h"

#include "effects.h"

Croutons::FutureResult<> timer(int duration) {
    Croutons::FutureResult it;

    QTimer::singleShot(duration, [it]() {
        it.succeed({});
    });

    return it;
}

Croutons::Future<int> returnFuture(int number) {
    co_await timer(200);

    co_return number;
}

Croutons::Future<int> flatMapTest() {
    using namespace Croutons;

    auto transformed = co_await returnFuture(5)
    .flatMap([](int i) -> Future<qreal> {
        co_await timer(100);

        co_return i + 1.0;
    })
    .flatMap([](qreal i) -> Future<int> {
        return returnFuture(i + 1.0);
    })
    .map([](int i) -> qreal {
        return i + 1.0;
    });

    Q_ASSERT(transformed == 8.0);

    co_return 1.0;
}

Croutons::Future<int> futureMain() {
    auto then = QTime::currentTime();

    co_await timer(1500);
    co_await flatMapTest();

    auto now = QTime::currentTime();

    Q_ASSERT(then.msecsTo(now) > 1000);

    co_return 0;
}

Croutons::Future<int> something() {
    co_return 0;
}

void anotherThing() {
    something().then([](int i) {

    });
}

Croutons::Future<void> voidSomething() {
    co_return;
}

Croutons::FutureResult<int> yetAnotherThing() {
    co_return 0;
}

void wawajete() {
    yetAnotherThing().then([](Croutons::Result<int, Croutons::Error> r) {

    });
}

class Singleton : public QObject
{

    Q_OBJECT

public:
    Q_INVOKABLE Croutons::FutureBase wait() {
        co_await voidSomething();

        co_await timer(1000);

        co_return "hi";
    }

};

using Maths = EffectFun<std::function<int(int, int)>>;
using Log = EffectVoidFun<std::function<void(QString)>>;

Effect<int> contextDependentArithmetic() {
    co_return perform Maths(1, 2);
}

Effect<void> contextDependentLogging() {
    perform Log("hi");

    co_return;
}

void mu() {
    {
        auto handler = Maths::handler([](int i, int ii) -> int {
            return i + ii;
        });

        qDebug() << contextDependentArithmetic();
    }
    {
        auto handler = Maths::handler([](int i, int ii) -> int {
            return i * ii;
        });

        qDebug() << contextDependentArithmetic();
    }
}

int main(int argc, char* argv[]) {
    mu();

    QGuiApplication app(argc, argv);

    futureMain();

    qRegisterMetaType<Croutons::FutureBase>();
    qmlRegisterSingletonType<Singleton>("org.kde.croutons", 1, 0, "Singleton", [](QQmlEngine*, QJSEngine*) -> QObject* { return new Singleton; });

    QQmlApplicationEngine eng;
    eng.load("main.qml");

    return app.exec();
}

#include "main.moc"
