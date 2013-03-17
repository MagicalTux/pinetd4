#include <ext/BitcoinCrypto.hpp>
#include <QtCrypto>
#include <QMutex>

static QMutex qca_mutex;
QCA::Initializer *qca;

bool BitcoinCrypto::init() {
	if (qca == NULL) {
		qca_mutex.lock();
		if (qca == NULL)
			qca = new QCA::Initializer();
		qca_mutex.unlock();
	}

	if (!QCA::isSupported("sha256")) {
		qDebug("BitcoinCrypto: unable to invoke as QCA does not support sha256");
		return false;
	}
	return true;
}

QByteArray BitcoinCrypto::doubleSha256(const QByteArray &input) {
	// compute checksum for data
	// Do not instanciate a sha object each time for faster processing
	static QMutex shamutex;
	shamutex.lock();
	static QCA::Hash sha("sha256");
	sha.clear();
	sha.update(input);
	QByteArray hashResult = sha.final().toByteArray();
	sha.clear();
	sha.update(hashResult);
	hashResult = sha.final().toByteArray();
	shamutex.unlock();
	return hashResult;
}

