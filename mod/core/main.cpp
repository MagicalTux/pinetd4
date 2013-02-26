#include <QtCore/QtGlobal>
#include <mod/core/Core.hpp>

extern "C" {
	Q_DECL_EXPORT void pinetd_main();
};

void pinetd_main() {
	new Core();
}

