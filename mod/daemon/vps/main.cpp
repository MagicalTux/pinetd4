#include "ModVps.hpp"

Q_DECL_EXPORT Daemon *pinetd_instanciate(const QString &modname, const QString &instname) {
	return new ModVps(modname, instname);
}
