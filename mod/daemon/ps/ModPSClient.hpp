#include <core/ClientTcp.hpp>
#include <QTimer>

class ModPS;

class ModPSClient: public ClientTcp {
	Q_OBJECT;
public:
	ModPSClient(QTcpSocket *sock, ModPS *parent);

protected:
	void handleBuffer(const QByteArray&);
};

