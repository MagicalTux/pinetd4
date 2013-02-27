#include <mod/core/ClientTcp.hpp>

class ModEcho;

class ModEchoClient: public ClientTcp {
	Q_OBJECT;
public:
	ModEchoClient(QTcpSocket *sock, ModEcho *parent);
};

