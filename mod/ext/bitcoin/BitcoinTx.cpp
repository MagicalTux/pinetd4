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

BitcoinTxOut::BitcoinTxOut(QDataStream&stream) {
	stream >> value;
	script = BitcoinTxScript(BitcoinStream::readString(stream));
}

void BitcoinTxOut::generate(QDataStream&stream) const {
	stream << value;
	script.generate(stream);
}


BitcoinTxScript::BitcoinTxScript() {
}

BitcoinTxScript::BitcoinTxScript(const QByteArray &s) {
	script = s;
}

void BitcoinTxScript::generate(QDataStream&stream) const {
	BitcoinStream::writeString(stream, script);
}

