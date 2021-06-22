// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <QGuiApplication>
#include <QTimer>

#include <QQmlApplicationEngine>

#include "coroutine_integration.h"
#include "coroutine_integration_network.h"

#include "effects.h"

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

Future<void> voidSomething() {
    co_return;
}

FutureResult<int> yetAnotherThing() {
    co_return 0;
}

void wawajete() {
    yetAnotherThing().then([](Result<int, Error> r) {

    });
}

class Singleton : public QObject
{

    Q_OBJECT

public:
    Q_INVOKABLE FutureBase wait() {
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

    qRegisterMetaType<FutureBase>();
    qmlRegisterSingletonType<Singleton>("org.kde.croutons", 1, 0, "Singleton", [](QQmlEngine*, QJSEngine*) -> QObject* { return new Singleton; });

    QQmlApplicationEngine eng;
    eng.load("main.qml");

    return app.exec();
}

#include "main.moc"
