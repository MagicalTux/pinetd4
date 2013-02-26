#include <QObject>
#include <QSettings>

class Core: public QObject {
	Q_OBJECT;
public:
	Core();
private:
	QSettings settings;
};

