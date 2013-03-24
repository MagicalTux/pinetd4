#include <QObject>
#include <QProcess>
#include <QMap>
#include <QLocalSocket>

class VServ: public QObject {
	Q_OBJECT;
public:
	VServ(const QString &name, QObject *parent = 0);

	void setPath(const QString &path);
	void setConfig(const QMap<QString,QString> &config);

	QStringList cmdLine() const;

public slots:
	void start();
	void processStarted();

private:
	QMap<QString,QString> config;
	QString qemu_exe;
	QString name;
	QString bpath;
	QProcess p;
	QLocalSocket s;
};

