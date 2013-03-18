#include <QByteArray>
#include "BitcoinTx.hpp"

class BitcoinBlock {
public:
	BitcoinBlock();
	BitcoinBlock(const QByteArray &data, quint32 height);

	const QByteArray &getParent() const;
	const QByteArray &getHash() const;
	QByteArray getHexHash() const;
	const QByteArray &getRaw() const;
	bool isValid() const;
	QList<BitcoinTx> getTransactions() const;
	quint32 getHeight() const;
	void setHeight(quint32);

private:
	bool is_valid;

	QByteArray hash;
	QByteArray parent;
	QByteArray merkle_root;
	QByteArray raw;
	QByteArray txns;
	quint32 version;
	quint32 timestamp;
	quint32 bits;
	quint32 nonce;
	quint32 height;
};

