#include "ModBroadcast.hpp"

Q_DECL_EXPORT Daemon *pinetd_instanciate(const QString &modname, const QString &instname) {
	return new ModBroadcast(modname, instname);
}
