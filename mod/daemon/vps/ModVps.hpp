#include <core/Daemon.hpp>
#include <ext/qamqp/amqp.h>
#include <ext/qamqp/amqp_queue.h>

class ModVps: public Daemon {
	Q_OBJECT;
public:
	ModVps(const QString &modname, const QString &instname);

public slots:
	void switchOutput();
	void mq_declared();
	void mq_e_declared();
	void mq_messageReceived(QAMQP::Queue*);
	virtual void reload();

private:
	QAMQP::Client *mq_client;
	QAMQP::Queue *mq_queue;
};

