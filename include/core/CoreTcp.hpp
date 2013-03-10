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

private:
	Core *parent;
	QString target;
	QString entry;
	QObject *receiver;
};

