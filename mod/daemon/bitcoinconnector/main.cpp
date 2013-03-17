#include "ModBitcoinConnector.hpp"
#include <QtCrypto>
#include <QMutex>

static QMutex qca_mutex;
QCA::Initializer *qca;

Q_DECL_EXPORT Daemon *pinetd_instanciate(const QString &modname, const QString &instname) {
	if (qca == NULL) {
		qca_mutex.lock();
		if (qca == NULL)
			qca = new QCA::Initializer();
		qca_mutex.unlock();
	}

	if (!QCA::isSupported("sha256")) {
		qDebug("BitcoinConnector: unable to invoke as QCA does not support sha256");
		return NULL;
	}
	return new ModBitcoinConnector(modname, instname);
}
