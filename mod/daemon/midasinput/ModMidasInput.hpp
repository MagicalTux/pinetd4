#include <core/Daemon.hpp>
#include <QFile>

class ModMidasInput: public Daemon {
	Q_OBJECT;
public:
	ModMidasInput(const QString &modname, const QString &instname);

public slots:
	void switchOutput();

private:
	QFile *output;

	void c_write(unsigned char type, const QByteArray &data = QByteArray());
	void c_switch(const QString &);
	void c_abort();
};

