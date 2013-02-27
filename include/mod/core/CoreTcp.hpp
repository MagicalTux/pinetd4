#include <QTcpServer>

class Core;

class CoreTcp: public QTcpServer {
	Q_OBJECT;
public:
	CoreTcp(Core *parent);
	void setTarget(const QString &);

private:
	Core *parent;
	QString target;
};

