#include "ModEcho.hpp"

ModEcho::ModEcho(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModEcho: new instance");
}

