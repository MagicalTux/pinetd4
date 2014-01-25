#include <ext/ElasticSearch.hpp>
#include <QTcpSocket>
#include <QJsonDocument>

// Fully async elastic search implementation

ElasticSearch::ElasticSearch(const QString &_prefix, QObject *parent): QObject(parent) {
	qDebug("Loading ElasticSearch");
	prefix = _prefix;
}

void ElasticSearch::index(const QJsonObject &obj) {
	qDebug("ElasticSearch: index() todo");
}

QJsonObject ElasticSearch::search(const QJsonObject &obj) {
	QJsonDocument doc(obj);
	return search(doc.toJson());
}

QJsonObject ElasticSearch::search(const QByteArray &search) {
	qDebug("ElasticSearch: search() todo");
	return QJsonObject();
}

