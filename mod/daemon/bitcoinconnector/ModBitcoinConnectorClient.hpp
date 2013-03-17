#include <core/ClientTcp.hpp>
#include <QTimer>

class ModBitcoinConnector;
class QHostAddress;
class BitcoinNetAddr;

class ModBitcoinConnectorClient: public ClientTcp {
	Q_OBJECT;
public:
	ModBitcoinConnectorClient(QTcpSocket *sock, ModBitcoinConnector *parent);
	virtual ~ModBitcoinConnectorClient();
	void sendVersion();

public slots:
	void bitcoin_packet_verack(const QByteArray&);
	void bitcoin_packet_version(const QByteArray&);
	void bitcoin_packet_inv(const QByteArray&);
	void bitcoin_packet_tx(const QByteArray&);
	void bitcoin_packet_addr(const QByteArray&);
	void bitcoin_packet_block(const QByteArray&);
	void bitcoin_packet_getdata(const QByteArray&);

	void newInventory(quint32, const QByteArray&);

protected:
	void handleBuffer(const QByteArray&);

private:
	int bitcoin_version; // Starting 2012-02-20 00:00:00 default version number is 209 - this forces version packet to be checksummed
	bool bitcoin_got_version;
	bool bitcoin_sent_version;
	QByteArray bitcoin_nonce;
	ModBitcoinConnector *parent;

	qint32 remote_version;
	qint64 remote_services;
	qint64 remote_timestamp;
	QByteArray remote_user_agent;
	qint32 remote_height;

	void sendPacket(const QByteArray &type, const QByteArray &data);

	QByteArray in_buf;
};

