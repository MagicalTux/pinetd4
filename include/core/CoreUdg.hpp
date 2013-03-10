#include <QObject>

class Core;
class QUnixDatagramServer;
class DatagramReply;

class CoreUdg: public QObject {
	Q_OBJECT;
public:
	CoreUdg(const QString &socket, Core *parent);
	void setTarget(QObject *, const QString &entry = "main");
	bool isValid() const;

signals:
	void outgoingDatagram(const QByteArray&, DatagramReply*);

public slots:
	void message(const QByteArray&);
	void targetDestroyed(QObject*);

private:
	Core *parent;
	QString entry;
	QUnixDatagramServer *server;
	QObject *receiver;
};

