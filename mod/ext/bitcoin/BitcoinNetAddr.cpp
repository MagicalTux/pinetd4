#include <ext/BitcoinNetAddr.hpp>
#include <QDataStream>

// https://en.bitcoin.it/wiki/Protocol_specification#Network_address

BitcoinNetAddr::BitcoinNetAddr() {
	is_valid = false;
	port = 0;
	services = 0;
}

BitcoinNetAddr::BitcoinNetAddr(const QHostAddress &_addr, quint16 _port, quint64 _services) {
	is_valid = false;
	addr = _addr;
	port = _port;
	services = _services;
	decode_hp();
}

BitcoinNetAddr::BitcoinNetAddr(const QByteArray&_bin) {
	is_valid = false;
	bin = _bin;
	decode_bin();
}

const BitcoinNetAddr &BitcoinNetAddr::null() {
	static BitcoinNetAddr nullval;
	return nullval;
}

bool BitcoinNetAddr::isValid() const {
	return is_valid;
}

const QByteArray &BitcoinNetAddr::getBin() const {
	if (!is_valid) {
		static QByteArray null_addr(26,'\0');
		return null_addr;
	}
	return bin;
}

const QHostAddress &BitcoinNetAddr::getAddr() const {
	return addr;
}

quint16 BitcoinNetAddr::getPort() const {
	return port;
}

quint64 BitcoinNetAddr::getServices() const {
	return services;
}

void BitcoinNetAddr::decode_bin() {
	if (bin.length() != 26) {
		is_valid = false;
		return;
	}
	QDataStream bin_r(bin);
	bin_r.setByteOrder(QDataStream::LittleEndian);
	bin_r >> services;
	if (bin.mid(8, 12) == QByteArray("\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF", 12)) {
		// ipv4
		bin_r.skipRawData(12);
		quint32 ipv4;
		bin_r >> ipv4;
		addr.setAddress(ipv4);
	} else {
		Q_IPV6ADDR ipv6;
		for(int i = 0; i < 16; i++)
			bin_r >> ipv6[i];
		addr.setAddress(ipv6);
	}
	bin_r >> port;
	is_valid = true;
}

void BitcoinNetAddr::decode_hp() {
	// transform hostaddr+port to bin
	bin = "";
	if (addr.isNull()) {
		is_valid = false;
		return; // invalid
	}
	QDataStream bin_w(&bin, QIODevice::WriteOnly);
	bin_w.setByteOrder(QDataStream::LittleEndian);

	bin_w << services;
	switch(addr.protocol()) {
		case QAbstractSocket::IPv4Protocol:
			bin_w.setByteOrder(QDataStream::BigEndian);
			bin_w << (quint32)0x00000000 << (quint32)0x00000000 << (quint32)0x0000ffff;
			bin_w << addr.toIPv4Address();
			bin_w.setByteOrder(QDataStream::LittleEndian);
			break;
		case QAbstractSocket::IPv6Protocol:
			{
				Q_IPV6ADDR ipv6 = addr.toIPv6Address();
				for(int i = 0; i < 16; i++)
					bin_w << (quint8)ipv6[i];
			}
			break;
		case QAbstractSocket::UnknownNetworkLayerProtocol:
			is_valid = false;
			return;
	}
	bin_w.setByteOrder(QDataStream::BigEndian);
	bin_w << port;
	is_valid = true;
}

