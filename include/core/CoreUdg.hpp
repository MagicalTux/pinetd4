#include <QObject>

class Core;
class QUnixDatagramServer;

class CoreUdg: public QObject {
	Q_OBJECT;
public:
	CoreUdg(const QString &socket, Core *parent);
	void setTarget(const QString &);
	bool isValid() const;

public slots:
	void message(const QByteArray&);

private:
	Core *parent;
	QString target;
	QString entry;
	QUnixDatagramServer *server;
};

