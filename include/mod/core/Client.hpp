#include <QObject>

class Daemon;

class Client: public QObject {
	Q_OBJECT;
public:
	Client(Daemon *parent);
	const QString &getId() const;

protected:
	Daemon *parent;

private:
	QString id;
};

