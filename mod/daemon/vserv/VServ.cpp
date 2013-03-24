#include "VServ.hpp"
#include <QUuid>

VServ::VServ(const QString &_name, QObject *parent): QObject(parent) {
	name = _name;

	qemu_exe = "/usr/bin/qemu-system-x86_64";
	bpath = "/tmp/";

	// generate default config
	config.insert("model", "pc-1.0"); // list via qemu-system-x86_64 -M '?'
	config.insert("cpu", "Nehalem"); // list via qemu-system-x86_64 -cpu '?'
	config.insert("memory", "512"); // in MB
	config.insert("smp_count", "1"); // number of CPU
	config.insert("smp_max", "32"); // max number of CPU
	config.insert("system_name", name); // "name"
	config.insert("system_uuid", QUuid::createUuid().toString().mid(1, 36)); // random uuid
	config.insert("rtc_base","utc");
	config.insert("vga", "cirrus");
	config.insert("net_mac", "52:05:99:00:00:01");
	config.insert("net_if", name+"0");

	config.insert("device0", "virtio-blk-pci|bus=pci.0|addr=0x4|drive=drive-device0|id=device0|bootindex=2|file=" + config.value("system_uuid") + ".bin|format=raw");
	config.insert("device1", "ide-drive|bus=ide.1|unit=0|drive=drive-device1|id=ide0-1-0|bootindex=1|media=cdrom|readonly=on|format=raw");

	connect(&p, SIGNAL(started()), this, SLOT(processStarted()));
}

void VServ::start() {
	if (p.state() != QProcess::NotRunning) return; // avoid double start
	p.start(qemu_exe, cmdLine());
}

void VServ::processStarted() {
	// we need to connect to the monitor unix socket now
	s.connectToServer(bpath+config.value("system_uuid")+".monitor");
}

QStringList VServ::cmdLine() const {
	QStringList res;

//	res << qemu_exe;
	res << "-S"; // do not start CPU immediately
	res << "-M" << config.value("model") << "-cpu" << config.value("cpu");
	res << "-enable-kvm";
	res << "-m" << config.value("memory");
	res << "-smp" << config.value("smp_count")+",maxcpus="+config.value("smp_max");
	res << "-name" << config.value("system_name");
	res << "-uuid" << config.value("system_uuid");
	
	if (config.contains("net_mac") && (config.contains("net_if"))) {
		res << "-net" << "nic,macaddr="+config.value("net_mac")+",model=virtio";
		res << "-net" << "tap,name="+config.value("net_if");
	}

	res << "-nodefconfig" << "-nodefaults";

	// control channel
	res << "-chardev" << "socket,id=charmonitor,path="+bpath+config.value("system_uuid")+".monitor,server";
	res << "-mon" << "chardev=charmonitor,id=monitor,mode=control";

	if (config.contains("rtc_base"))
		res << "-rtc" << "base=" << config.value("rtc_base");

	for(int i = 0; config.contains("device"+QString::number(i)); i++) {
		QStringList devinfotmp = config.value("device"+QString::number(i)).split("|");
		QMap<QString,QString> devinfo;
		QString devtype = devinfotmp.at(0);
		for(int j = 1; j < devinfotmp.length(); j++) {
			int k = devinfotmp.at(j).indexOf("=");
			if (k == -1) continue;
			devinfo.insert(devinfotmp.at(j).left(k), devinfotmp.at(j).mid(k+1));
		}
		if (devinfo.contains("bus")) devtype += ",bus="+devinfo.value("bus");
		if (devinfo.contains("unit")) devtype += ",unit="+devinfo.value("unit");
		if (devinfo.contains("addr")) devtype += ",addr="+devinfo.value("addr");
		if (devinfo.contains("drive")) devtype += ",drive="+devinfo.value("drive");
		if (devinfo.contains("id")) devtype += ",id="+devinfo.value("id");
		if (devinfo.contains("bootindex")) devtype += ",bootindex="+devinfo.value("bootindex");
		if (devinfo.contains("drive")) {
			QString driveinfo = "id="+devinfo.value("drive")+",if=none";
			if (devinfo.contains("file")) driveinfo += ",file="+devinfo.value("file");
			if (devinfo.contains("format")) driveinfo += ",format="+devinfo.value("format");
			if (devinfo.contains("media")) driveinfo += ",media="+devinfo.value("media");
			if (devinfo.contains("readonly")) driveinfo += ",readonly="+devinfo.value("readonly");
			res << "-drive" << driveinfo;
		}
		res << "-device" << devtype;
	}

	res << "-usb";
	res << "-device" << "usb-tablet,id=input0";

	res << "-vnc" << "unix:"+bpath+config.value("system_uuid")+".vnc";
	res << "-vga" << config.value("vga");
	res << "-device" << "virtio-balloon-pci,id=balloon0,bus=pci.0,addr=0x5"; // for memory increase

	return res;
}

