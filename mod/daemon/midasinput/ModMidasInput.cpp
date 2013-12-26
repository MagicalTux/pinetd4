#include "ModMidasInput.hpp"
#include <QDateTime>
#include <ext/qamqp/amqp.h>
#include <ext/qamqp/amqp_queue.h>

ModMidasInput::ModMidasInput(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModMidasInput: initialize new file and checkpoint");
	
	output = NULL;
	switchOutput();
}

void ModMidasInput::switchOutput() {
	// start outputting to a new file. 1. Generate filename 2. open file for writing 3. output new file info in current output, 4. finish
	
	QString now = QDateTime::currentDateTimeUtc().toString("yyyyMMddhhmmsszzz");
	QString newfile = "/var/run/midas/data_" + now + ".dat";
	QString checkpoint_name = "/var/run/midas/checkpoint_"+now+".dat";

	QFile *newout = new QFile(newfile);
	newout->open(QIODevice::WriteOnly | QIODevice::Truncate);
	QFile *checkpoint = new QFile(checkpoint_name);
	checkpoint->open(QIODevice::WriteOnly | QIODevice::Truncate);

	if ((output) && (output->isOpen())) {
		c_switch(newfile);
		c_abort();
		output->close();
	}
	output = checkpoint;
	c_switch(newfile);
	c_abort();
	output->close();

	// QFile::rename() won't remove existing files, so use C rename instead
	rename(checkpoint_name.toLatin1().constData(), "/var/run/midas/checkpoint.dat"); // from stdio.h
//	QFile::remove("/var/run/midas/checkpoint.dat");
//	checkpoint.rename("/var/run/midas/checkpoint.dat");

	output = newout;
}

void ModMidasInput::c_write(unsigned char type, const QByteArray &data) {
	QByteArray buf;
	buf.append((char)type);
	buf.append(data.length() & 0xff);
	buf.append((data.length() >> 8) & 0xff);
	buf.append(data);
	output->write(buf);
}

void ModMidasInput::c_switch(const QString &s) {
	c_write(0x00, s.toLatin1());
}

void ModMidasInput::c_abort() {
	c_write(0xff);
}

