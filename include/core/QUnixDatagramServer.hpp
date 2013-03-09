#include <QSocketNotifier>
#include <QObject>

class QUnixDatagramServer: public QObject {
	Q_OBJECT;

public:
	QUnixDatagramServer(const QString &socket, QObject *parent = 0);
	~QUnixDatagramServer();
	bool isValid() const;

public slots:
	void activated(int sock);

signals:
	void message(const QByteArray&);

private:
	int sock;
	QString socket_path;
	QSocketNotifier *n;
};

