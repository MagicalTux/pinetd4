#include <QObject>
#include <QSettings>
#include <QMap>
#include <QLibrary>

class Daemon;
class CoreTcp;

class Core: public QObject {
	Q_OBJECT;
public:
	Core();
	Daemon *getDaemon(const QString &);

public slots:
	void reloadConfig();
	bool modprobe(const QString &module);

private:
	QSettings settings;
	QMap<QString,Daemon*> daemons;
	QMap<QString,QLibrary*> modules;
	QMap<QString,CoreTcp*> port_tcp;
};

