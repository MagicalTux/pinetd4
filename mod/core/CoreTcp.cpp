#include <mod/core/CoreTcp.hpp>
#include <mod/core/Core.hpp>

CoreTcp::CoreTcp(Core *_parent): QTcpServer(_parent) {
	parent = _parent;
}

void CoreTcp::setTarget(const QString &t) {
	target = t;
}

