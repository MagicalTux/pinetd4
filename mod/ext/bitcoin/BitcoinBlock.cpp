#include <ext/BitcoinBlock.hpp>
#include <ext/BitcoinStream.hpp>
#include <ext/BitcoinCrypto.hpp>

BitcoinBlock::BitcoinBlock() {
	is_valid = false;
}

BitcoinBlock::BitcoinBlock(const QByteArray &data) {
	is_valid = true;
	hash = BitcoinCrypto::doubleSha256(data.left(80));
	raw = data;
	QDataStream data_r(data);
	data_r >> version;
	parent = BitcoinStream::readData(data_r, 32);
	merkle_root = BitcoinStream::readData(data_r, 32);
	data_r >> timestamp >> bits >> nonce;
	txn_count = BitcoinStream::readInt(data_r);
}

const QByteArray &BitcoinBlock::getRaw() const {
	return raw;
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

