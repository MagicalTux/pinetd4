#include <mod/core/Daemon.hpp>

class ModEcho: public Daemon {
	Q_OBJECT;
public:
	ModEcho(const QString &modname, const QString &instname);
};

