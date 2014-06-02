#include <core/ClientTcp.hpp>
#include <QTimer>

class ModRConnect;

class ModRConnectClient: public ClientTcp {
	Q_OBJECT;
public:
	ModRConnectClient(QTcpSocket *sock, ModRConnect *parent);
	~ModRConnectClient();

protected:
	void handleBuffer(const QByteArray&);

	QByteArray raw_id;
	QByteArray buf_in;
};

