#include <QObject>
#include <QSettings>
#include <QMap>
#include <QLibrary>
#include <QtGlobal>

class Daemon;
class CoreTcp;
#ifdef Q_OS_UNIX
class CoreUdg;
#endif

class Core: public QObject {
	Q_OBJECT;
public:
	Core();
	Daemon *getDaemon(const QString &);

public slots:
	void reloadSymbols();
	void reloadConfig();
	bool modprobe(const QString &module);

private:
	QSettings settings;
	QMap<QString,Daemon*> daemons;
	QMap<QString,QLibrary*> modules;
	QMap<QString,CoreTcp*> port_tcp;
#ifdef Q_OS_UNIX
	QMap<QString,CoreUdg*> port_udg;
#endif

	QMap<QString,QString> sym_map;
};

