#include <core/Daemon.hpp>
#include <QFile>
#include <ext/qamqp/amqp.h>
#include <ext/qamqp/amqp_queue.h>
#include <ext/qamqp/amqp_exchange.h>

class ModMidasInput: public Daemon {
	Q_OBJECT;
public:
	ModMidasInput(const QString &modname, const QString &instname);

public slots:
	void switchOutput();
	void mq_declared();
	void mq_e_declared();
	void mq_messageReceived(QAMQP::Queue*);

private:
	QFile *output;

	QAMQP::Queue* mq_queue;
	QAMQP::Exchange* mq_exchange;

	void c_write(unsigned char type, const QByteArray &data = QByteArray());
	void c_switch(const QString &);
	void c_abort();
};

