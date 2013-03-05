#include <ext/ClientTcpLine.hpp>

class ModBroadcast;

class ModBroadcastClient: public ClientTcpLine {
	Q_OBJECT;
public:
	ModBroadcastClient(QTcpSocket *sock, ModBroadcast *parent);

signals:
	void recv(const QByteArray&);

public slots:
	void send(const QByteArray&);

protected:
	void handleLine(const QByteArray&);
};

