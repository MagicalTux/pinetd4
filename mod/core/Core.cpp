#include <core/Core.hpp>
#include <core/CoreTcp.hpp>
#include <core/Daemon.hpp>
#include <QStringList>
#include <QSet>
#include <QFile>
#include <QRegExp>
#include <QThread>
#include <QCoreApplication>
#ifdef Q_OS_UNIX
#include <core/CoreUdg.hpp>
#endif

static Core *core_v;

Core::Core(): settings("pinetd.ini", QSettings::IniFormat) {
	qDebug("Core: Loaded pInetd version %s", qPrintable(version()));
	core_v = this;
	reloadSymbols();
	reloadConfig();
}

Core *Core::get() {
	return core_v;
}

QString Core::version() {
	return PINETD_VERSION;
}

void Core::reloadSymbols() {
	qDebug("Core: reloading symbols table");

	sym_map.clear();

	QStringList sym;
	sym << "mod/ext/symbols.txt";

	for(int i = 0; i < sym.size(); i++) {
		// read file
		QFile f(sym.at(i));
		if (!f.exists()) {
			qDebug("Core: file not found: %s", qPrintable(sym.at(i)));
			continue;
		}
		if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
			qDebug("Core: could not open file %s for reading", qPrintable(sym.at(i)));
			continue;
		}
		while(1) {
			QByteArray line = f.readLine();
			if (line.isEmpty()) break;
			QStringList dat = QString::fromLatin1(line.trimmed()).split(' ');
			QString mod = dat.takeFirst();
			QString sym = dat.takeFirst();
			sym_map.insert(sym, mod);
		}
		f.close();
	}
}

void Core::reloadConfig() {
	qDebug("Core: reloading configuration");
	settings_lock.lock();
	do {
		settings.beginGroup("preload");
		QStringList k = settings.allKeys();

		for(int i = 0; i < k.size(); i++) {
			QString mod_name = settings.value(k.at(i)).toString();
			qDebug("Core: loading module %s", qPrintable(mod_name));
			modprobe(mod_name);
		}

		settings.endGroup();
	} while(0);

	do {
		settings.beginGroup("daemons");
		QStringList k = settings.allKeys();

		QSet<QString> cur_daemons;
		// iterate on currently loaded daemons
		for(auto i = daemons.constBegin(); i != daemons.constEnd(); i++)
			cur_daemons.insert(i.key());

		for(int i = 0; i < k.size(); i++) {
			if (daemons.contains(k.at(i))) {
				// already loaded and still in config, ignore
				// TODO: we may want to check the module is still the same
				cur_daemons.remove(k.at(i));
				continue;
			}
			QString mod_name = settings.value(k.at(i)).toString();
			qDebug("Core: loading daemon: %s module %s", qPrintable(k.at(i)), qPrintable(mod_name));
			if (!modprobe(mod_name)) continue;

			// _Z18pinetd_instanciateRK7QStringS1_ = pinetd_instanciate(QString const&, QString const&)
			Daemon *(*callback)(const QString &, const QString &);
			callback = (Daemon*(*)(const QString &, const QString &))modules.value(mod_name)->resolve("_Z18pinetd_instanciateRK7QStringS1_");
			if (callback == NULL) {
				qDebug("Core: failed to load daemon %s as it has no instanciate method", qPrintable(k.at(i)));
				continue;
			}
			Daemon *d = callback(mod_name, k.at(i));
			if (d == NULL) {
				qDebug("Core: failed to instanciate daemon %s", qPrintable(k.at(i)));
				continue;
			}
			QThread *t = new QThread(d);
			d->moveToThread(t);
			connect(this, SIGNAL(reloadDaemons()), d, SLOT(reload()));
			t->start();
			daemons.insert(k.at(i), d);
		}

		for(auto i = cur_daemons.constBegin(); i != cur_daemons.constEnd(); i++) {
			Daemon *d = daemons.take(*i);
			// we may want to set a signal on d
			d->deleteLater();
		}

		settings.endGroup();
	} while(0);

	do {
		settings.beginGroup("tcp");
		QStringList k = settings.allKeys();

		QSet<QString> cur_tcp;
		// iterate on currently routed tcp ports
		for(auto i = port_tcp.constBegin(); i != port_tcp.constEnd(); i++)
			cur_tcp.insert(i.key());

		for(int i = 0; i < k.size(); i++) {
			QStringList tgt = settings.value(k.at(i)).toString().split(":");
			if (tgt.size() == 1) tgt.append("main");

			if (port_tcp.contains(k.at(i))) {
				// already linked to somewhere, remove and re-add with new value
				port_tcp.value(k.at(i))->setTarget(getDaemon(tgt.at(0)), tgt.at(1));
				cur_tcp.remove(k.at(i));
				continue;
			}
			CoreTcp *t = new CoreTcp(this);
			t->setTarget(getDaemon(tgt.at(0)), tgt.at(1));
			QStringList nfo = k.at(i).split(':');
			if (nfo.size() != 2) {
				qDebug("Failed to listen on %s, bad syntax", qPrintable(k.at(i)));
				continue;
			}
			if (!t->listen(QHostAddress(nfo.at(0)), nfo.at(1).toInt())) {
				qDebug("Failed to listen on %s, giving up", qPrintable(k.at(i)));
				continue;
			}
			port_tcp.insert(k.at(i), t);
		}

		for(auto i = cur_tcp.constBegin(); i != cur_tcp.constEnd(); i++)
			port_tcp.take(*i)->deleteLater();

		settings.endGroup();
	} while(0);

#ifdef Q_OS_UNIX
	do {
		settings.beginGroup("udg");
		QStringList k = settings.allKeys();

		QSet<QString> cur_udg;
		// iterate on currently routed udg paths
		for(auto i = port_udg.constBegin(); i != port_udg.constEnd(); i++)
			cur_udg.insert(i.key());

		for(int i = 0; i < k.size(); i++) {
			QStringList tgt = settings.value(k.at(i)).toString().split(":");
			while(tgt.size() < 3) tgt.append("main");

			if (port_udg.contains(k.at(i))) {
				// already linked to somewhere, remove and re-add with new value
				port_udg.value(k.at(i))->setTarget(getDaemon(tgt.at(1)), tgt.at(2));
				cur_udg.remove(k.at(i));
				continue;
			}
			CoreUdg *t = new CoreUdg(tgt.at(0), this);
			if (!t->isValid()) {
				qDebug("Failed to listen on %s, giving up", qPrintable(k.at(i)));
				continue;
			}
			t->setTarget(getDaemon(tgt.at(1)), tgt.at(2));
			port_udg.insert(k.at(i), t);
		}

		for(auto i = cur_udg.constBegin(); i != cur_udg.constEnd(); i++)
			port_udg.take(*i)->deleteLater();

		settings.endGroup();
	} while(0);
#endif
	settings_lock.unlock();

	// tell daemons to load their configs (should happen in different threads)
	reloadDaemons();
}

QMap<QString,QVariant> Core::getConfig(const QString &daemon) {
	settings_lock.lock();
	QMap<QString,QVariant> res;

	settings.beginGroup(QString("conf-")+daemon);
	QStringList k = settings.allKeys();
	for(int i = 0; i < k.size(); i++) {
		res.insert(k.at(i), settings.value(k.at(i)));
	}
	settings.endGroup();
	settings_lock.unlock();
	return res;
}

bool Core::modprobe(const QString &name) {
	if (modules.contains(name)) return true;

	qDebug("Core: loading module %s", qPrintable(name));

	QLibrary *lib = new QLibrary(QCoreApplication::applicationDirPath() + QString("/mod/")+name+QString(".so"), this); // TODO make this more neutral - QLibrary::load has a problem on linux

	if (name.left(4) == "ext/") {
		// if an extension, export symbols to other modules
		lib->setLoadHints(QLibrary::ExportExternalSymbolsHint);
	} else {
		lib->setLoadHints(0);
	}

	while(1) {
		if (!lib->load()) {
			QString err = lib->errorString();
			qDebug("Core: got error while loading lib: %s", qPrintable(err));
			// Cannot load library mod/daemon/broadcast: (mod/daemon/libbroadcast.so: undefined symbol: _ZTI13ClientTcpLine)
			QRegExp rx(".*: \\(.*: undefined symbol: (_Z.*)\\)");
			if (rx.exactMatch(err)) {
				QString sym = rx.cap(1);
				if (sym_map.contains(sym)) {
					QString mod = sym_map.value(sym);
					if (!modules.contains(mod)) {
//						qDebug("Core: missing symbol: %s - loading %s", qPrintable(sym), qPrintable(mod));
						if (modprobe(mod)) {
							// need to call "unload" so it'll try again
							lib->unload(); // so we can unload
							continue;
						}
					}
				}
			}
			qDebug("Core: failed to load module: %s", qPrintable(err));
			delete lib;
			return false;
		}
		qDebug("Core: Managed to load %s", qPrintable(name));
		break;
	}

	// _Z14pinetd_preloadP4Core = pinetd_preload(Core*)
	bool (*callback_preload)(Core*);
	callback_preload = (bool(*)(Core*))lib->resolve("_Z14pinetd_preloadP4Core");
	if (callback_preload != NULL) {
		if (!callback_preload(this)) {
			qDebug("Core: failed to load %s due to preload error", qPrintable(name));
			lib->unload();
			delete lib;
			return false;
		}
	}


	modules.insert(name, lib);
	return true;
}

Daemon *Core::getDaemon(const QString &key) {
	if (!daemons.contains(key)) return NULL;
	return daemons.value(key);
}

