// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#include <QDebug>
#include "futurebase.h"

class FutureListener : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QJSValue value READ value NOTIFY valueChanged)

public:
    QJSValue _value;
    QJSValue value() const {
        return _value;
    }
    Q_SIGNAL void valueChanged();
};

QJSValue FutureBase::valueOr(const QJSValue& it)
{
    auto eng = it.engine();
    auto fn = eng->evaluate("(function(foo) { return Qt.binding(() => foo.value ) })");

    if (d->valueOrObject != nullptr) {
        return fn.call({eng->newQObject(d->valueOrObject)});
    }

    auto lis = new FutureListener;
    d->valueOrObject = lis;
    if (d->settled && d->succeeded) {
        lis->_value = eng->toScriptValue(d->result);
        return fn.call({eng->newQObject(d->valueOrObject)});
    }

    lis->_value = it;
    d->valueOr = [lis, eng](QVariant it) {
        lis->_value = eng->toScriptValue(it);
        Q_EMIT lis->valueChanged();
    };
    return fn.call({eng->newQObject(d->valueOrObject)});
}

#include "futurebase_p.moc"
