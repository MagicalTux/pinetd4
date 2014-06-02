#include <QTcpServer>

class Core;

class CoreTcp: public QTcpServer {
	Q_OBJECT;
public:
	CoreTcp(Core *parent);
	void setTarget(QObject *, const QString &entry = "main");

public slots:
	void targetDestroyed(QObject*);

signals:
	void outgoingTcp(const QString &entry, QTcpSocket *socket);

protected:
	virtual void incomingConnection(int socketDescriptor);
#if QT_VERSION >= 0x050000
	virtual void incomingConnection(qintptr socketDescriptor);
#endif

private:
	Core *parent;
	QString entry;
	QObject *receiver;
};

