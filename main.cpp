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

class Singleton : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE FutureBase foo() {
        QNetworkAccessManager man;
        QNetworkRequest req;

        auto resp = co_await man.get(req);
        qDebug() << resp->errorString();

        auto foo = co_await timer(2000);

        co_return "i was returned by a coroutine";
    };
};

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine eng;

    qRegisterMetaType<FutureBase>();

    qmlRegisterSingletonType<Singleton>("org.kde.idk", 1, 0, "Singleton", [](QQmlEngine*,QJSEngine*) -> QObject* { return new Singleton; });

    const QUrl url = QUrl::fromLocalFile("main.qml");
    QObject::connect(
        &eng,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if ((obj == nullptr) && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);
    eng.load(url);

    return app.exec();
}

#include "main.moc"
