#include <QDataStream>

class BitcoinNetAddr;

class BitcoinStream {
public:
	static void writeAddress(QDataStream &stream, const BitcoinNetAddr &addr);
	static void writeString(QDataStream &stream, const QByteArray &string);
	static void writeInt(QDataStream &stream, quint64 i);
	static void writeData(QDataStream &stream, const QByteArray &data);

	static quint64 readInt(QDataStream &stream);
	static QByteArray readString(QDataStream &stream);
	static QByteArray readData(QDataStream &stream, int len);
	static BitcoinNetAddr readAddress(QDataStream &stream);
};

