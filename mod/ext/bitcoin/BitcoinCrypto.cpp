#include <ext/BitcoinCrypto.hpp>
#include <QCryptographicHash>

bool BitcoinCrypto::init() {
	// no more QCA - no need to do much
	return true;
}

QByteArray BitcoinCrypto::doubleSha256(const QByteArray &input) {
	// compute checksum for data
	QCryptographicHash sha(QCryptographicHash::Sha256);
	sha.addData(input);
	QByteArray hashResult = sha.result();
	sha.reset();
	sha.addData(hashResult);
	hashResult = sha.result();
	return hashResult;
}

