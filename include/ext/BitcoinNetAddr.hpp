#include <QHostAddress>

class BitcoinNetAddr {
public:
	BitcoinNetAddr();
	BitcoinNetAddr(const QHostAddress &, quint16, quint64 services = 0);
	BitcoinNetAddr(const QByteArray&);

	QString getKey() const;
	const QByteArray &getBin() const;
	const QHostAddress &getAddr() const;
	quint16 getPort() const;
	quint64 getServices() const;
	bool isValid() const;

	static const BitcoinNetAddr &null();

private:
	bool is_valid;
	QHostAddress addr;
	quint16 port;
	quint64 services;
	QByteArray bin;

	void decode_bin();
	void decode_hp();
};

