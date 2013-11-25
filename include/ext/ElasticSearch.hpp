#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>

class ElasticSearch: public QObject {
	Q_OBJECT;
public:
	ElasticSearch(const QString &prefix, QObject *parent = 0); // new ElasticSearch("http://127.0.0.1:9200"); but you really should use config for that
	void index(const QJsonObject &obj); // works same way the PHP api works. index({"index":index,"type":type,"op_type":"create","replication":"async","body":{"var":val}})

protected:
	QString prefix;
	QNetworkAccessManager net;
};

