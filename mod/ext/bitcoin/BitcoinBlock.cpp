#include <ext/BitcoinBlock.hpp>
#include <ext/BitcoinStream.hpp>
#include <ext/BitcoinCrypto.hpp>

BitcoinBlock::BitcoinBlock() {
	is_valid = false;
}

BitcoinBlock::BitcoinBlock(const QByteArray &data, quint32 _height) {
	is_valid = true;
	height = _height;
	hash = BitcoinCrypto::doubleSha256(data.left(80));
	raw = data.left(80);
	size = data.length();
	if (data.length() > 80) {
		txns = data.mid(80);
	}
	QDataStream data_r(raw);
	data_r.setByteOrder(QDataStream::LittleEndian);
	data_r >> version;
	parent = BitcoinStream::readData(data_r, 32);
	merkle_root = BitcoinStream::readData(data_r, 32);
	data_r >> timestamp >> bits >> nonce;
}

void BitcoinBlock::setHeight(quint32 h) {
	height = h;
}

quint32 BitcoinBlock::getHeight() const {
	return height;
}

QList<BitcoinTx> BitcoinBlock::getTransactions() const {
	if (txns.isEmpty()) return QList<BitcoinTx>();
	QList<BitcoinTx> res;
	QDataStream txns_r(txns);
	txns_r.setByteOrder(QDataStream::LittleEndian);
	quint64 count = BitcoinStream::readInt(txns_r);
	for(quint64 i = 0; i < count; i++)
		res.append(BitcoinTx(txns_r));
	return res;
}

const QByteArray &BitcoinBlock::getRaw() const {
	return raw;
}

QByteArray BitcoinBlock::getHexHash() const {
	QByteArray res;
	for(int i = hash.length() - 1; i >= 0; i--)
		res.append(hash.at(i));
	return res.toHex();
}

const QByteArray &BitcoinBlock::getHash() const {
	return hash;
}

const QByteArray &BitcoinBlock::getParent() const {
	return parent;
}

bool BitcoinBlock::isValid() const {
	return is_valid;
}

quint32 BitcoinBlock::getVersion() const {
	return version;
}

const QByteArray &BitcoinBlock::getMerkleRoot() const {
	return merkle_root;
}

quint32 BitcoinBlock::getTimestamp() const {
	return timestamp;
}

quint32 BitcoinBlock::getBits() const {
	return bits;
}

quint32 BitcoinBlock::getNonce() const {
	return nonce;
}

quint32 BitcoinBlock::getSize() const {
	return size;
}

