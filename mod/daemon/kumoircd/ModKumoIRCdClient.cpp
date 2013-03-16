#include "ModKumoIRCdClient.hpp"
#include "ModKumoIRCd.hpp"
#include <QTcpSocket>
#include <QStringList>
#include <QMetaMethod>

// http://tools.ietf.org/html/rfc1459.html

ModKumoIRCdClient::ModKumoIRCdClient(QTcpSocket *sock, ModKumoIRCd *_parent): ClientTcpLine(sock, _parent) {
	qDebug("KumoIRCd: new client id %s", qPrintable(getId()));
	parent = _parent;
//	connect(&timeout, SIGNAL(timeout()), this, SLOT(nextLine()));
	timeout.setSingleShot(false);
	timeout.start(60000);
}

// http://tools.ietf.org/html/rfc1459.html#section-6
void ModKumoIRCdClient::rawReply(enum irc_raw raw, const QList<QByteArray>&reply) {
	switch(raw) {
		case ERR_UNKNOWNCOMMAND: return rawReply(raw, QList<QByteArray>() << reply << "Unknown command", true);
		case ERR_ERRONEUSNICKNAME: return rawReply(raw, QList<QByteArray>() << reply << "Erroneus nickname", true);
		case ERR_NONICKNAMEGIVEN: return rawReply(raw, QList<QByteArray>() << "No nickname given", true);
		default:
			qDebug("Raw %d not handled, to fix asap", raw);
	}
}

void ModKumoIRCdClient::rawReply(enum irc_raw raw, const QList<QByteArray>&reply, bool last_with_space) {
	QByteArray raw_str = QByteArray::number(raw).rightJustified(3, '0');
	sendReply(QList<QByteArray>() << raw_str << reply, ":"+parent->getServerName(), last_with_space);
}

void ModKumoIRCdClient::handleIrcCommand(const QList<QByteArray>&cmd, const QByteArray &prefix) {
	// dirty trick using Qt
//	if (!QMetaObject::invokeMethod(this, QByteArray("irc_cmd_"+cmd.at(0)).toLower(), Qt::DirectConnection, Q_ARG(const QList<QByteArray>&, cmd), Q_ARG(const QByteArray &, prefix))) {
	int m = metaObject()->indexOfMethod(QByteArray("irc_cmd_"+cmd.at(0).toLower()+"(QList<QByteArray>,QByteArray)"));
	if (m == -1) {
		qDebug() << "Method not found: " << prefix << cmd;
		rawReply(ERR_UNKNOWNCOMMAND, QList<QByteArray>() << cmd.at(0));
		return;
	}
	metaObject()->method(m).invoke(this, Qt::DirectConnection, Q_ARG(const QList<QByteArray>&, cmd), Q_ARG(const QByteArray &, prefix));
}

void ModKumoIRCdClient::irc_cmd_nick(const QList<QByteArray>&, const QByteArray &) {
	qDebug("NICK... Todo");
}

void ModKumoIRCdClient::handleLine(const QByteArray &line) {
	if (line.isEmpty()) return; // "Empty  messages  are  silently  ignored" (RFC1459 2.3.1)
	// parse line
	QList<QByteArray> tmp;
	int pos = 0;
	while(pos < line.length()) {
		if ((line[pos] == ':') && (pos != 0)) {
			tmp << line.mid(pos+1);
			break;
		}
		int tpos = line.indexOf(' ', pos);
		if (tpos == -1) {
			tmp << line.mid(pos);
			break;
		}
		tmp << line.mid(pos, tpos-pos);
		pos = tpos+1;
		while(line[pos] == ' ') {
			pos++;
			if (pos >= line.length()) break;
		}
	}
	QByteArray prefix;
	if (tmp.at(0)[0] == ':') {
		prefix = tmp.takeFirst();
	}
	handleIrcCommand(tmp, prefix);
}

void ModKumoIRCdClient::sendReply(const QList<QByteArray>&params, const QByteArray &prefix, bool last_param_space) {
	QByteArray final;
	if (!prefix.isEmpty()) {
		final = prefix+" ";
	}
	for(int i = 0; i < params.size(); i++) {
		if (i == 0) {
			final += params.at(i);
			continue;
		}
		if ((i == (params.size()-1)) && (last_param_space)) {
			final += " :"+params.at(i);
			break;
		}
		final += " "+params.at(i);
	}
	final += "\r\n";

	sock->write(final);
}

