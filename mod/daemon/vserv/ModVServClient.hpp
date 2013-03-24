#include <core/ClientTcp.hpp>

class ModVServ;

class ModVServClient: public ClientTcp {
	Q_OBJECT;
public:
	ModVServClient(QTcpSocket *sock, ModVServ *parent);

protected:
	void handleBuffer(const QByteArray&);

private:
};

