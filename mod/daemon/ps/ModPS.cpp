#include "ModPS.hpp"
#include "ModPSClient.hpp"
#include <QStringList>
#include <QVariant>
#include <QSet>
#include <QTcpSocket>

ModPS::ModPS(const QString &modname, const QString &instname): Daemon(modname, instname) {
//	qDebug("ModPS: new instance");
	connect(&masters_check, SIGNAL(timeout()), this, SLOT(checkMasters()));
	masters_check.setSingleShot(false);
	masters_check.start(10000);
}

void ModPS::reload() {
	QMap<QString,QVariant> conf = getConfig();
	QStringList conf_masters;

	if (conf.contains("master"))
		conf_masters = conf.value("master").toStringList();

	subscribe_limit.clear();
	if (conf.contains("subscribe_limit")) {
		QStringList tmp = conf.value("subscribe_limit").toStringList();
		for(int i = 0; i < tmp.length(); i++) {
			subscribe_limit.insert(QByteArray::fromHex(tmp.at(i).toAscii().replace("-", "")));
		}
	}

	// make list of current masters
	QSet<QString> cur_masters;

	for(auto i = masters.constBegin(); i != masters.constEnd(); i++)
		cur_masters.insert(i.key());

	for(int i = 0; i < conf_masters.size(); i++) {
		if (masters.contains(conf_masters.at(i))) {
			// already connected and still in config, ignore
			cur_masters.remove(conf_masters.at(i));
			continue;
		}
		masters.insert(conf_masters.at(i), NULL);
	}

	for(auto i = cur_masters.constBegin(); i != cur_masters.constEnd(); i++) {
		QTcpSocket *t = masters.take(*i);
		if (t != NULL) {
			t->close();
			delete t;
		}
	}
	checkMasters();
}

void ModPS::checkMasters() {
	qulonglong now = time(NULL);

	for(auto i = masters.begin(); i != masters.end(); i++) {
		if (i.value() == NULL) {
			i.value() = new QTcpSocket(this);
			connect(i.value(), SIGNAL(readyRead()), this, SLOT(readFromMaster()));
			connect(i.value(), SIGNAL(connected()), this, SLOT(masterConnected()));
			i.value()->setProperty("stamp", now);
		}

		if (i.value()->property("stamp").toULongLong() < (now-60)) {
			i.value()->abort();
		}

		switch(i.value()->state()) {
			case QAbstractSocket::HostLookupState:
			case QAbstractSocket::ConnectingState:
			case QAbstractSocket::ConnectedState:
				break;
			default:
			{
				i.value()->setProperty("stamp", (qulonglong)time(NULL));
				QStringList tmp = i.key().split(":");
				if (tmp.size() != 2) break; // can't help it
				qDebug("ModPS: connecting to master %s", qPrintable(i.key()));
				i.value()->connectToHost(tmp.at(0), tmp.at(1).toInt());
			}
		}

		// send a ping to the master
		if (i.value()->state() == QAbstractSocket::ConnectedState) {
			QByteArray ping("\x00\x19\x80\x50\x49\x4e\x47\x5f\x54\x43\x50\x5f\x52\x45\x51\x55\x45\x53\x54\x5f\x50\x49\x4e\x45\x54\x44\x34", 27);
			i.value()->write(ping);
		}
	}
}

void ModPS::readFromMaster() {
	QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
	if (s == NULL) return;

	s->setProperty("stamp", (qulonglong)time(NULL));

	while(true) {
		if (s->bytesAvailable() < 2) return; // not enough
		// read packet size
		QByteArray dat = s->peek(2);
		int length = (((unsigned char)dat[0] << 8) | (unsigned char)dat[1]) + 2;
		if (s->bytesAvailable() < length) return; // still not enough
		pushPacket(s->read(length));
	}
}

void ModPS::masterConnected() {
	QTcpSocket *s = qobject_cast<QTcpSocket*>(sender());
	if (s == NULL) return;
	QByteArray packet(3, '\0');
	packet[1] = 17; // packet size
	packet[2] = 0x81; // subscribe
	for(auto i = channel_refcount.constBegin(); i != channel_refcount.constEnd(); i++)
		s->write(packet+i.key());
}

bool ModPS::canSubscribe(const QByteArray &channel) {
	if (subscribe_limit.isEmpty()) return true;
	if (subscribe_limit.contains(channel)) return true;
	return false;
}

void ModPS::doSubscribe(const QByteArray &channel) {
	QByteArray packet(3, '\0');
	packet[1] = 17; // packet size
	packet[2] = 0x81; // subscribe
	packet.append(channel); // +16 bytes
	for(auto i = masters.begin(); i != masters.end(); i++) {
		i.value()->write(packet);
	}
}

void ModPS::doUnsubscribe(const QByteArray &channel) {
	QByteArray packet(3, '\0');
	packet[1] = 17; // packet size
	packet[2] = 0x82; // unsubscribe
	packet.append(channel); // +16 bytes
	for(auto i = masters.begin(); i != masters.end(); i++) {
		i.value()->write(packet);
	}
}

void ModPS::pushPacket(const QByteArray &dat) {
	// check length
	int length = (((unsigned char)dat[0] << 8) | (unsigned char)dat[1]) + 2;
	if (length < 17) {
		qDebug("ModPS::pushPacket: dropping packet because too small");
		return;
	}
	if (dat.size() < length) {
		qDebug("ModPS::pushPacket: Dropping incomplete packet");
		return;
	}
	if (dat.size() > length) {
		qDebug("ModPS::pushPacket: Dropping extra data at end of packet");
		// can't modify buffer, so need to create a new one. This case should never happen anyway
		return pushPacket(dat.left(length));
	}
	int type = (unsigned char)dat[2];
	QByteArray channel = dat.mid(3,16);

	if (type == 0x80) return; // ping reply, ignore safely

	if (type & 0x80) {
		qDebug("ModPS::pushPacket: dropping control packet");
		return;
	}

	if (!channel_refcount.contains(channel)) return; // no listener for this message, drop it

	channelPacket(dat, channel, type);
}

void ModPS::incomingTcp(const QString &, QTcpSocket *sock) {
	ModPSClient *t = new ModPSClient(sock, this);
	connect(this, SIGNAL(channelPacket(const QByteArray &, const QByteArray &, int)), t, SLOT(channelPacket(const QByteArray &, const QByteArray &, int)));
	clientAdd(t);
}

void ModPS::incomingDatagram(const QByteArray&dat, DatagramReply*r) {
	if (r != NULL) delete r;
	pushPacket(dat);
}

void ModPS::channelAddRef(const QByteArray &channel) {
	channel_refcount_lock.lock();
	if (!channel_refcount.contains(channel)) {
		channel_refcount.insert(channel, 1);
		// TODO: subscribe to any master
		doSubscribe(channel);
		channel_refcount_lock.unlock();
		return;
	}
	channel_refcount.insert(channel, channel_refcount.value(channel)+1);
	channel_refcount_lock.unlock();
}

void ModPS::channelDelRef(const QByteArray &channel) {
	channel_refcount_lock.lock();
	if (!channel_refcount.contains(channel)) abort(); // can't happen
	if (channel_refcount.value(channel) <= 1) {
		channel_refcount.remove(channel);
		// TODO: unsubscribe from any master
		doUnsubscribe(channel);
		channel_refcount_lock.unlock();
		return;
	}
	channel_refcount.insert(channel, channel_refcount.value(channel)-1);
	channel_refcount_lock.unlock();
}

