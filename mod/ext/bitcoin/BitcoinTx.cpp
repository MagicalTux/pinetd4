#include <ext/BitcoinBlock.hpp>
#include <ext/BitcoinStream.hpp>
#include <ext/BitcoinCrypto.hpp>

BitcoinTx::BitcoinTx() {
	is_valid = false;
}

BitcoinTx::BitcoinTx(const QByteArray &data) {
	raw = data;
	is_valid = true;
	QDataStream stream(data);
	stream.setByteOrder(QDataStream::LittleEndian);
	parseStream(stream);
}

BitcoinTx::BitcoinTx(QDataStream &stream) {
	is_valid = true;
	parseStream(stream);
}

QByteArray BitcoinTx::hash() {
	return BitcoinCrypto::doubleSha256(generate());
}

void BitcoinTx::generate(QDataStream &stream) const {
	if (!is_valid) return;
	stream << version;
	BitcoinStream::writeInt(stream, tx_in.size());
	for(qint64 i = 0; i < tx_in.size(); i++)
		tx_in.at(i).generate(stream);
	BitcoinStream::writeInt(stream, tx_out.size());
	for(qint64 i = 0; i < tx_out.size(); i++)
		tx_out.at(i).generate(stream);
	stream << lock_time;
}

void BitcoinTx::uncache() {
	raw = QByteArray();
}

const QByteArray &BitcoinTx::generate() {
	if (!raw.isEmpty()) return raw;
	if (!is_valid) return raw;
	QDataStream stream(&raw, QIODevice::WriteOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	generate(stream);
	return raw;
}

void BitcoinTx::parseStream(QDataStream &stream) {
	stream >> version;

	quint64 tmp = BitcoinStream::readInt(stream);
	for(quint64 i = 0; i < tmp; i++)
		tx_in.append(BitcoinTxIn(stream));

	tmp = BitcoinStream::readInt(stream);
	for(quint64 i = 0; i < tmp; i++)
		tx_out.append(BitcoinTxOut(stream));

	stream >> lock_time;
}

const QList<BitcoinTxOut> &BitcoinTx::txOut() const {
	return tx_out;
}

const QList<BitcoinTxIn> &BitcoinTx::txIn() const {
	return tx_in;
}

quint32 BitcoinTx::getVersion() const {
	return version;
}

quint32 BitcoinTx::getLockTime() const {
	return lock_time;
}

BitcoinTxIn::BitcoinTxIn(QDataStream&stream) {
	prev_out_hash = BitcoinStream::readData(stream, 32);
	stream >> prev_out_index;
	script = BitcoinTxScript(BitcoinStream::readString(stream));
	stream >> sequence;
}

void BitcoinTxIn::generate(QDataStream&stream) const {
	BitcoinStream::writeData(stream, prev_out_hash);
	stream << prev_out_index;
	script.generate(stream);
	stream << sequence;
}

QByteArray BitcoinTxIn::getPrevOutHash() const {
	return prev_out_hash;
}

quint32 BitcoinTxIn::getPrevOutIndex() const {
	return prev_out_index;
}

BitcoinTxOut::BitcoinTxOut(QDataStream&stream) {
	stream >> value;
	script = BitcoinTxScript(BitcoinStream::readString(stream));
}

void BitcoinTxOut::generate(QDataStream&stream) const {
	stream << value;
	script.generate(stream);
}

quint64 BitcoinTxOut::getValue() const {
	return value;
}

QByteArray BitcoinTxOut::getTxOutAddr() const {
	return script.getTxOutAddr();
}

BitcoinTxScript::BitcoinTxScript() {
}

BitcoinTxScript::BitcoinTxScript(const QByteArray &s) {
	script = s;
}

void BitcoinTxScript::generate(QDataStream&stream) const {
	BitcoinStream::writeString(stream, script);
}

QByteArray BitcoinTxScript::getTxOutAddr() const {
	// if this is a txout, we can get the addr easily
	// format is: 76a910 x 88ac (x being 16 bytes of addr data)
	// OP_DUP OP_HASH160 de26002a186c381cdf320c7ae21e43f99c2a8296 OP_EQUALVERIFY OP_CHECKSIG
	// OP_DUP = 0x76 OP_HASH160 = 0xa9 (16bytes string literal = 0x10+string) OP_EQUALVERIFY = 0x88 OP_CHECKSIG = 0xac
	// total length: 16+3+2 = 21
	// can also be:
	// 04678afdb0fe5548271967f1a67130b7105cd6a828e03909a67962e0ea1f61deb649f6bc3f4cef38c4f35504e51ec112de5c384df7ba0b8d578a4c702b6bf11d5f OP_CHECKSIG
	// 0x41(len) <data> OP_CHECKSIG = 0xac (need to perform hash160 of <data> to get correct addr in that case - too lazy right now
	// if format is not valid, return an empty QByteArray()
	if (script.length() == 21) {
		if (script.left(3) != QByteArray("\x76\xa9\x10", 3)) return QByteArray();
		if (script.right(2) != QByteArray("\x88\xac", 2)) return QByteArray();
		return script.mid(3,16);
	}
	return QByteArray();
}

