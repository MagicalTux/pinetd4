#include <QByteArray>

class BitcoinBlock {
public:
	BitcoinBlock();
	BitcoinBlock(const QByteArray &data);

	const QByteArray &getParent() const;
	const QByteArray &getHash() const;
	const QByteArray &getRaw() const;
	bool isValid() const;

private:
	bool is_valid;

	QByteArray hash;
	QByteArray parent;
	QByteArray merkle_root;
	QByteArray raw;
	quint32 version;
	quint32 timestamp;
	quint32 bits;
	quint32 nonce;
	quint64 txn_count;
};

