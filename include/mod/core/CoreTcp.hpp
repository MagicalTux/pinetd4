#include <QTcpServer>

class Core;

class CoreTcp: public QTcpServer {
	Q_OBJECT;
public:
	CoreTcp(Core *parent);
	void setTarget(const QString &);

protected:
	virtual void incomingConnection(int socketDescriptor);

private:
	Core *parent;
	QString target;
	QString entry;
};

