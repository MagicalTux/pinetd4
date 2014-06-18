#include "ModNetflow.hpp"
#include <QDateTime>
#include <QDataStream>
#include <QFile>

ModNetflow::ModNetflow(const QString &modname, const QString &instname): Daemon(modname, instname) {
//	qDebug("ModNetflow: new instance");
}

void ModNetflow::incomingDatagram(const QByteArray&dat, DatagramReply*r) {
	if (r != NULL) delete r;
	// one file per minute
	quint64 t = QDateTime::currentMSecsSinceEpoch() / 1000 / 60;
	QString fn = QString("/var/spool/netflow/nf_%1.raw").arg(t);
	QFile f(fn);
	if (!f.open(QIODevice::Append)) {
		qDebug("failed to write data");
	}

	QDataStream s(&f);
	s.writeBytes(dat.constData(), dat.length()); // The len is serialized as a quint32, followed by len bytes from s.
	f.close();
}

