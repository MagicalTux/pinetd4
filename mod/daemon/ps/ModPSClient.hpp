#include <core/ClientTcp.hpp>
#include <QTimer>
#include <QSet>

class ModPS;

class ModPSClient: public ClientTcp {
	Q_OBJECT;
public:
	ModPSClient(QTcpSocket *sock, ModPS *parent);
	virtual ~ModPSClient();

public slots:
	void channelPacket(const QByteArray &dat, const QByteArray &chan, int type);

protected:
	void handleBuffer(const QByteArray&);

private:
	QByteArray buf_in;
	void handlePacket(const QByteArray &packet);

	void handleSubscribe(const QByteArray &channel);
	void handleUnsubscribe(const QByteArray &channel);

	QSet<QByteArray> channels;
	ModPS *parent;
};

