#include <ext/ElasticSearch.hpp>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QBuffer>

// Fully async elastic search implementation

ElasticSearch::ElasticSearch(const QString &_prefix, QObject *parent): QObject(parent) {
	qDebug("Loading ElasticSearch, connecting to %s", qPrintable(_prefix));
	prefix = QUrl(_prefix);
	QNetworkRequest r(prefix.resolved(QUrl("/")));
	QNetworkReply *reply = net.get(r);
	while (reply->isRunning()) QCoreApplication::processEvents();
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("error %d while getting ElasticSearch status", reply->error());
		return;
	}
	QByteArray res = reply->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(res);

	qDebug("res: %s", qPrintable(res));
}

bool ElasticSearch::contains(const QString &id, const QString &index, const QString &type) {
	// build url
	QNetworkRequest r(prefix.resolved(QUrl(QString("/") + index + "/" + type + "/" + id)));
	QNetworkReply *reply = net.head(r);
	while (reply->isRunning()) QCoreApplication::processEvents();
	if (reply->error() == QNetworkReply::ContentNotFoundError) {
//		qDebug("ElasticSearch: checking for %s/%s/%s (NOT FOUND)", qPrintable(index), qPrintable(type), qPrintable(id));
		return false; // not found
	}
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("error %d while ElasticSearch::contains()", reply->error());
		return false;
	}
//	qDebug("ElasticSearch: checking for %s/%s/%s (found)", qPrintable(index), qPrintable(type), qPrintable(id));
	return true;
}

QJsonObject ElasticSearch::get(const QString &id, const QString &index, const QString &type) {
	// build url
	qDebug("ElasticSearch: getting for %s/%s/%s", qPrintable(index), qPrintable(type), qPrintable(id));
	QNetworkRequest r(prefix.resolved(QUrl(QString("/") + index + "/" + type + "/" + id)));
	QNetworkReply *reply = net.get(r);
	while (reply->isRunning()) QCoreApplication::processEvents();
	if (reply->error() == QNetworkReply::ContentNotFoundError) return QJsonObject(); // not found
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("error %d while ElasticSearch::get()", reply->error());
		return QJsonObject();
	}
	QByteArray res = reply->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(res);
	return doc.object().value("_source").toObject();
}

QString ElasticSearch::index(const QJsonObject &obj, const QString &_index, const QString &_type) {
	QJsonDocument doc(obj);
	return index(doc.toJson(), _index, _type);
}

QString ElasticSearch::index(const QByteArray &obj, const QString &index, const QString &type) {
	// send POST

	QNetworkRequest r(prefix.resolved(QUrl(QString("/") + index + "/" + type + "/")));
	r.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	QByteArray dup(obj);
	QBuffer buf(&dup);
	QNetworkReply *reply = net.post(r, &buf);
	while (reply->isRunning()) QCoreApplication::processEvents();
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("error %d while ElasticSearch::index()", reply->error());
		return QString();
	}

	QByteArray res = reply->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(res);

//	qDebug("ElasticSearch::index: %s", qPrintable(res));

	return QString();
}

QJsonObject ElasticSearch::search(const QJsonObject &obj, const QString &index) {
	QJsonDocument doc(obj);
	return search(doc.toJson(), index);
}

QJsonObject ElasticSearch::search(const QByteArray &search, const QString &index) {
	qDebug("ElasticSearch: search(%s)", qPrintable(search));
	QNetworkRequest r(prefix.resolved(QUrl(QString("/") + index + "/_search")));
	r.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	QByteArray dup(search);
	QBuffer buf(&dup);
	QNetworkReply *reply = net.sendCustomRequest(r, "GET", &buf);
	while (reply->isRunning()) QCoreApplication::processEvents();
	if (reply->error() != QNetworkReply::NoError) {
		qDebug("error %d while ElasticSearch::search()", reply->error());
		return QJsonObject();
	}
	QByteArray res = reply->readAll();
	QJsonDocument doc = QJsonDocument::fromJson(res);

	qDebug("ElasticSearch::search: %s", qPrintable(res));

	return doc.object();
}

