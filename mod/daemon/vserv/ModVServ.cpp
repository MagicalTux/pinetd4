#include "ModVServ.hpp"
#include "ModVServClient.hpp"
#include "VServ.hpp"
#include <QDebug>

ModVServ::ModVServ(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModVServ: new instance");
	// test
	VServ v("test");
	qDebug() << v.cmdLine();
}

void ModVServ::incomingTcp(const QString &, QTcpSocket *sock) {
	clientAdd(new ModVServClient(sock, this));
}

