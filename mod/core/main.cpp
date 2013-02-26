#include <QtCore/QtGlobal>

extern "C" {
	Q_DECL_EXPORT void pinetd_main();
};

void pinetd_main() {
	qDebug("in core");
}

