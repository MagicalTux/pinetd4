#include <core/ClientTcp.hpp>

class ModEcho;

class ModEchoClient: public ClientTcp {
	Q_OBJECT;
public:
	ModEchoClient(QTcpSocket *sock, ModEcho *parent);

protected:
	void handleBuffer(const QByteArray&);
};

