#include <QByteArray>
#include <QList>

class BitcoinTxScript {
public:
	BitcoinTxScript();
	BitcoinTxScript(const QByteArray&);
	void generate(QDataStream&) const;
	QByteArray getTxOutAddr() const;
private:
	QByteArray script;
};

class BitcoinTxOut {
public:
	BitcoinTxOut(QDataStream&);
	void generate(QDataStream&) const;
	quint64 getValue() const;
	QByteArray getTxOutAddr() const;
private:
	quint64 value;
	BitcoinTxScript script;
};

class BitcoinTxIn {
public:
	BitcoinTxIn(QDataStream&);
	void generate(QDataStream&) const;
	QByteArray getPrevOutHash() const;
	quint32 getPrevOutIndex() const;
private:
	QByteArray prev_out_hash;
	quint32 prev_out_index;
	BitcoinTxScript script;
	quint32 sequence;
};

class BitcoinTx {
public:
	BitcoinTx();
	BitcoinTx(const QByteArray&);
	BitcoinTx(QDataStream&);

	QByteArray hash();
	const QByteArray &generate();
	void generate(QDataStream&) const;
	void uncache();

	quint32 getVersion() const;
	quint32 getLockTime() const;

	const QList<BitcoinTxOut> &txOut() const;
	const QList<BitcoinTxIn> &txIn() const;

private:
	bool is_valid;
	void parseStream(QDataStream&);

	QByteArray raw;
	quint32 version;
	QList<BitcoinTxOut> tx_out;
	QList<BitcoinTxIn> tx_in;
	quint32 lock_time;
};

