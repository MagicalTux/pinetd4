#include <QObject>

class Daemon: public QObject {
	Q_OBJECT;
public:
	Daemon(const QString &modname, const QString &instname);

protected:
	QString modname;
	QString instname;
};

