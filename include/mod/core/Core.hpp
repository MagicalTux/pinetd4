#include <QObject>
#include <QSettings>
#include <QMap>
#include <QLibrary>

class Daemon;

class Core: public QObject {
	Q_OBJECT;
public:
	Core();

public slots:
	void reloadConfig();
	bool modprobe(const QString &module);

private:
	QSettings settings;
	QMap<QString,Daemon*> daemons;
	QMap<QString,QLibrary*> modules;
};

