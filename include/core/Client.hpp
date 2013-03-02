#include <QObject>

class Daemon;

class Client: public QObject {
	Q_OBJECT;
public:
	Client(Daemon *parent);
	virtual ~Client();
	const QString &getId() const;

protected:
	Daemon *parent;

private:
	QString id;
};

