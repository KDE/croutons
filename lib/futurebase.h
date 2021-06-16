// SPDX-FileCopyrightText: 2021 Carson Black <uhhadd@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QJSEngine>
#include <QJSValue>
#include <QSharedPointer>
#include <QVariant>

class FutureBase
{

	Q_GADGET

	struct Shared {
		QVariant result;

		// ok, err
		std::function<void(QVariant)> onSucc = [](QVariant) {};
		std::function<void(QVariant)> onFail = [](QVariant) {};

		std::function<void(QVariant)> valueOr = [](QVariant) {};
		QObject* valueOrObject = nullptr;

		bool settled = false;
		bool succeeded = false;
	};
	QSharedPointer<Shared> d;

public:
	FutureBase() {
		d.reset(new Shared);
	}
	FutureBase(const FutureBase& other) {
		this->d = other.d;
	}
	~FutureBase() {
	}

	void succeed(const QVariant& value) const {
		if (d->settled) {
			return;
		}
		d->settled = true;
		d->succeeded = true;

		d->result = value;
		d->onSucc(d->result);
		d->valueOr(d->result);
	}
	void fail(const QVariant& value) const {
		if (d->settled) {
			return;
		}
		d->settled = true;
		d->succeeded = false;

		d->result = value;
		d->onFail(d->result);
	}
	QVariant result() const {
		return d->result;
	}
	void then(std::function<void(QVariant)> then, std::function<void(QVariant)> fail = nullptr) const {
		d->onSucc = then;
		if (fail != nullptr) {
			d->onFail = fail;
		}
		if (d->settled && d->succeeded) {
			then(result());
		} else if (d->settled && !d->succeeded && fail != nullptr) {
			fail(result());
		}
	}
	bool settled() const {
		return d->settled;
	}
	bool success() const {
		return d->succeeded;
	}
	Q_INVOKABLE void then(const QJSValue& it) {
		d->onSucc = [va = it](QVariant n) mutable {
			va.call({va.engine()->toScriptValue(n)});
		};
		d->onFail = [va = it](QVariant n) mutable {
			va.call({QJSValue(), va.engine()->toScriptValue(n)});
		};

		if (d->settled && d->succeeded) {
			d->onSucc(result());
			d->valueOr(result());
		} else if (d->settled && !d->succeeded) {
			d->onFail(result());
		}
	}
	Q_INVOKABLE QJSValue valueOr(const QJSValue& it);
};
