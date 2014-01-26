#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QUrl>

class ElasticSearch: public QObject {
	Q_OBJECT
public:
	ElasticSearch(const QString &prefix, QObject *parent = 0); // new ElasticSearch("http://127.0.0.1:9200"); but you really should use config for that
	QString index(const QJsonObject &obj, const QString &index, const QString &type); // doesn't works same way the PHP api works. index({"index":index,"type":type,"op_type":"create","replication":"async","body":{"var":val}})
	QString index(const QByteArray &obj, const QString &index, const QString &type);
	QJsonObject search(const QJsonObject &search, const QString &index); // index can be index/type
	QJsonObject search(const QByteArray &search, const QString &index); // index can be index/type

	bool contains(const QString &id, const QString &index, const QString &type);
	QJsonObject get(const QString &id, const QString &index, const QString &type);

protected:
	QUrl prefix;
	QNetworkAccessManager net;
};

