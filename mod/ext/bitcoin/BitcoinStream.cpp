#include <ext/BitcoinStream.hpp>
#include <ext/BitcoinNetAddr.hpp>

void BitcoinStream::writeAddress(QDataStream &stream, const BitcoinNetAddr &addr) {
	writeData(stream, addr.getBin());
}

void BitcoinStream::writeString(QDataStream &stream, const QByteArray &string) {
	writeInt(stream, string.length());
	writeData(stream, string);
}

void BitcoinStream::writeInt(QDataStream &stream, quint64 i) {
	if (i < 253) {
		stream << (quint8)i;
		return;
	}
	if (i < 0xffff) {
		stream << (quint8)253 << (quint16)i;
		return;
	}
	if (i < 0xffffffff) {
		stream << (quint8)254 << (quint32)i;
		return;
	}
	stream << (quint8)255 << i;
}

void BitcoinStream::writeData(QDataStream &stream, const QByteArray &data) {
	stream.writeRawData(data.constData(), data.length());
}

quint64 BitcoinStream::readInt(QDataStream &stream) {
	quint8 i;
	stream >> i;
	switch(i) {
		case 253:
			{
				quint16 j;
				stream >> j;
				return j;
			}
		case 254:
			{
				quint32 j;
				stream >> j;
				return j;
			}
		case 255:
			{
				quint64 j;
				stream >> j;
				return j;
			}
		default:
			return i;
	}
}

QByteArray BitcoinStream::readString(QDataStream &stream) {
	qint64 len = readInt(stream);
	return readData(stream, len);
}

QByteArray BitcoinStream::readData(QDataStream &stream, int len) {
	QByteArray final(len, '\0');
	int res = stream.readRawData(final.data(), len);
	if (res < len) final.truncate(res);
	return final;
}

