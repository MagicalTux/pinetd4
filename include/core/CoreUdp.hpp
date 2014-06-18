#include <QUdpSocket>

class Core;
class DatagramReply;

class CoreUdp: public QUdpSocket {
	Q_OBJECT;
public:
	CoreUdp(Core *parent);
	void setTarget(QObject *, const QString &entry = "main");

signals:
	void outgoingDatagram(const QByteArray&, DatagramReply*);

public slots:
	void receiveDatagram();
	void targetDestroyed(QObject*);

private:
	Core *parent;
	QString entry;
	QObject *receiver;
};

