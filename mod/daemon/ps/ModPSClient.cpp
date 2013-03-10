#include "ModPSClient.hpp"
#include "ModPS.hpp"
#include <QTcpSocket>

ModPSClient::ModPSClient(QTcpSocket *sock, ModPS *_parent): ClientTcp(sock, _parent) {
	parent = _parent;
//	handleSubscribe(QByteArray::fromHex(getId().toLatin1().replace("-","")));
}

ModPSClient::~ModPSClient() {
	for(auto i = channels.begin(); i != channels.end(); i++)
		parent->channelDelRef(*i);
	channels.clear();
}

void ModPSClient::channelPacket(const QByteArray &dat, const QByteArray &chan, int) {
	if (!channels.contains(chan)) return;
	write(dat);
}

void ModPSClient::handlePacket(const QByteArray &packet) {
	// we got a packet here, length is guaranteed to be fine (checked in handleBuffer())
	int type = (unsigned char)packet[2];
	QByteArray channel = packet.mid(3,16);

	if (!(type & 0x80)) {
		qDebug("ModPSClient::handlePacket: received an invalid packet type from client");
		sock->close();
		deleteLater();
		return;
	}

	switch(type) {
		case 0x80:
			write(packet); // echo
			break;
		case 0x81:
			handleSubscribe(channel);
			break;
		case 0x82:
			handleUnsubscribe(channel);
			break;
	}
}

void ModPSClient::handleSubscribe(const QByteArray &channel) {
	if (channels.contains(channel)) return;
//	qDebug("ModPSClient::handleSubscribe: client subscribes to channel %s", qPrintable(channel.toHex()));
	channels.insert(channel);
	parent->channelAddRef(channel);
}

void ModPSClient::handleUnsubscribe(const QByteArray &channel) {
	if (!channels.contains(channel)) return;
//	qDebug("ModPSClient::handleUnsubscribe: client unsubscribes to channel %s", qPrintable(channel.toHex()));
	channels.remove(channel);
	parent->channelDelRef(channel);
}

void ModPSClient::handleBuffer(const QByteArray &t) {
	buf_in.append(t);
	if (buf_in.size() > (100*1024)) {
		// more than 100kB pending read, drop! (shouldn't happen)
		buf_in.clear(); // free memory asap
		sock->close();
		deleteLater();
	}

	while(true) {
		if (buf_in.size() < 2) return; // not enough data
		int len = (((unsigned char)buf_in[0] << 8) | (unsigned char)buf_in[1]) + 2;
		if (len > buf_in.size()) return; // packet not fully received yet
		QByteArray packet = buf_in.left(len);
		buf_in.remove(0, len);
		handlePacket(packet);
	}
}
