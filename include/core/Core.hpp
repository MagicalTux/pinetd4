#include <QObject>
#include <QSettings>
#include <QMap>
#include <QLibrary>
#include <QtGlobal>
#include <QMutex>

#define PINETD_VERSION "0.1.0-dev"

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
	QMap<QString,QVariant> getConfig(const QString &daemon);

	static Core *get();
	QString version();

signals:
	void reloadDaemons();

public slots:
	void reloadSymbols();
	void reloadConfig();
	bool modprobe(const QString &module);

private:
	QSettings settings;
	QMutex settings_lock;
	QMap<QString,Daemon*> daemons;
	QMap<QString,QLibrary*> modules;
	QMap<QString,CoreTcp*> port_tcp;
#ifdef Q_OS_UNIX
	QMap<QString,CoreUdg*> port_udg;
#endif

	QMap<QString,QString> sym_map;
};

