#include <mod/core/ClientTcp.hpp>

ClientTcp::ClientTcp(QTcpSocket *_sock, Daemon *_parent): Client(_parent) {
	sock = _sock;
}

