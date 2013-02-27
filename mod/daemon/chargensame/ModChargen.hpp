#include <mod/core/Daemon.hpp>
#include <QTimer>

class ModChargen: public Daemon {
	Q_OBJECT;
public:
	ModChargen(const QString &modname, const QString &instname);

signals:
	void nextLine(const QByteArray&);

public slots:
	virtual void incomingTcp(const QString &entry, QTcpSocket *sock);
	void makeNextLine();

private:
	QTimer t;
	int pos;
};

