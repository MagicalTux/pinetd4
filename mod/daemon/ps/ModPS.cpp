#include "ModPS.hpp"
#include "ModPSClient.hpp"

ModPS::ModPS(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModPS: new instance");
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

void ModPS::incomingDatagram(const QByteArray&dat, DatagramReply*) {
	pushPacket(dat);
}

void ModPS::channelAddRef(const QByteArray &channel) {
	channel_refcount_lock.lock();
	if (!channel_refcount.contains(channel)) {
		channel_refcount.insert(channel, 1);
		// TODO: subscribe to any master
		qDebug("ModPS: should subscribe to %s", qPrintable(channel.toHex()));
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
		qDebug("ModPS: should unsubscribe from %s", qPrintable(channel.toHex()));
		channel_refcount_lock.unlock();
		return;
	}
	channel_refcount.insert(channel, channel_refcount.value(channel)-1);
	channel_refcount_lock.unlock();
}

