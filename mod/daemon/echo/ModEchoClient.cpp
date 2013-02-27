#include "ModEchoClient.hpp"
#include "ModEcho.hpp"

ModEchoClient::ModEchoClient(QTcpSocket *sock, ModEcho *parent): ClientTcp(sock, parent) {
	qDebug("client init ok");
}

