#include "ModBitcoinConnector.hpp"
#include <QMutex>
#include <ext/BitcoinCrypto.hpp>

Q_DECL_EXPORT Daemon *pinetd_instanciate(const QString &modname, const QString &instname) {
	if (!BitcoinCrypto::init()) return NULL;
	return new ModBitcoinConnector(modname, instname);
}
