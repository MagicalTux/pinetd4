#include "ModMidasInput.hpp"
#include <QDateTime>

ModMidasInput::ModMidasInput(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModMidasInput: initialize new file and checkpoint, and MQ connection");

	QAMQP::Client *client = new QAMQP::Client(this);
	client->open(QUrl("qamqp://guest:guest@localhost/"));
	mq_queue = client->createQueue();
	mq_queue->declare("midas1", QAMQP::Queue::Durable);
	mq_exchange = client->createExchange("midas");
	connect(mq_queue, SIGNAL(declared()), this, SLOT(mq_declared()));
	connect(mq_queue, SIGNAL(messageReceived(QAMQP::Queue*)), this, SLOT(mq_messageReceived(QAMQP::Queue*)));

	output = NULL;
	switchOutput();
}

void ModMidasInput::mq_declared() {
	qDebug("ModMidasInput: Queue ready, configuring exchange");
	mq_queue->setQOS(0,1);
	mq_queue->consume();
	// start exchange declaration
	connect(mq_exchange, SIGNAL(declared()), this, SLOT(mq_e_declared()));
	mq_exchange->declare("fanout", QAMQP::Exchange::Durable);
}

void ModMidasInput::mq_e_declared() {
	qDebug("ModMidasInput: Exchange ready, binding...");
	mq_exchange->bind(mq_queue);
}

void ModMidasInput::mq_messageReceived(QAMQP::Queue*q) {
	QAMQP::MessagePtr message = q->getMessage();

	output->write(message->payload);
	output->flush();

	q->ack(message);
}

void ModMidasInput::switchOutput() {
	// start outputting to a new file. 1. Generate filename 2. open file for writing 3. output new file info in current output, 4. finish
	
	QString now = QDateTime::currentDateTimeUtc().toString("yyyyMMddhhmmsszzz");
	QString newfile = "/opt/midas/data_" + now + ".dat";
	QString checkpoint_name = "/opt/midas/checkpoint_"+now+".dat";

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
	rename(checkpoint_name.toLatin1().constData(), "/opt/midas/checkpoint.dat"); // from stdio.h
//	QFile::remove("/opt/midas/checkpoint.dat");
//	checkpoint.rename("/opt/midas/checkpoint.dat");

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

