#include "ModVps.hpp"
#include "ModVpsInst.hpp"
#include <QJsonDocument>

ModVps::ModVps(const QString &modname, const QString &instname): Daemon(modname, instname) {
	qDebug("ModVps: new instance");

	mq_client = NULL;
}

void ModVps::reload() {
	if (mq_client != NULL) {
		return;
	}
	QMap<QString,QVariant> conf = getConfig();
	if (!conf.contains("mq")) return;

	qDebug("ModVps: initializing connection to MQ");
	mq_client = new QAMQP::Client(this);
	mq_client->open(QUrl(conf.value("mq").toString()));

	mq_queue = mq_client->createQueue();
	mq_queue->declare(conf.value("mq_queue").toString(), QAMQP::Queue::Durable);
	connect(mq_queue, SIGNAL(declared()), this, SLOT(mq_declared()));
	connect(mq_queue, SIGNAL(messageReceived(QAMQP::Queue*)), this, SLOT(mq_messageReceived(QAMQP::Queue*)));
}


void ModVps::mq_declared() {
	qDebug("ModVps: Queue ready");
	mq_queue->setQOS(0,1);
	mq_queue->consume();
}

void ModVps::mq_messageReceived(QAMQP::Queue*q) {
	QAMQP::MessagePtr message = q->getMessage();

	qDebug("Msg: %s", qPrintable(message->payload));
	QJsonDocument doc = QJsonDocument::fromJson(message->payload);

	q->ack(message);
}

