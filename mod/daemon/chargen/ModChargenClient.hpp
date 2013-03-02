#include <core/ClientTcp.hpp>
#include <QTimer>

class ModChargen;

class ModChargenClient: public ClientTcp {
	Q_OBJECT;
public:
	ModChargenClient(QTcpSocket *sock, ModChargen *parent);

public slots:
	void nextLine();

protected:
	void handleBuffer(const QByteArray&);

private:
	QTimer t;
	int pos;
};

