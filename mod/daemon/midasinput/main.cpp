#include "ModMidasInput.hpp"
#include <core/Core.hpp>

Q_DECL_EXPORT bool pinetd_preload(Core *core) {
	if (!core->modprobe("ext/qamqp")) return false;
	return true;
}

Q_DECL_EXPORT Daemon *pinetd_instanciate(const QString &modname, const QString &instname) {
	return new ModMidasInput(modname, instname);
}

