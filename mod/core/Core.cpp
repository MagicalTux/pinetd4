#include <mod/core/Core.hpp>

Core::Core(): settings("pinetd.ini", QSettings::IniFormat) {
	qDebug("In core::core");
}

