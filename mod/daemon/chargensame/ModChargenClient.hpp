#include <core/ClientTcp.hpp>

class ModChargen;

class ModChargenClient: public ClientTcp {
	Q_OBJECT;
public:
	ModChargenClient(QTcpSocket *sock, ModChargen *parent);

public slots:
	void nextLine(const QByteArray&);

protected:
	void handleBuffer(const QByteArray&);
};

