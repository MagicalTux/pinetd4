#include "ModBitcoinConnector.hpp"
#include <core/Core.hpp>

Q_DECL_EXPORT bool pinetd_preload(Core *core) {
	if (!core->modprobe("ext/bitcoin")) return false;
	if (!core->modprobe("ext/elasticsearch")) return false;
	return true;
}

Q_DECL_EXPORT Daemon *pinetd_instanciate(const QString &modname, const QString &instname) {
	qDebug("hi");
	return new ModBitcoinConnector(modname, instname);
}
