#include <ext/ElasticSearch.hpp>
#include <QTcpSocket>

// Fully async elastic search implementation

ElasticSearch::ElasticSearch(const QString &_prefix, QObject *parent): QObject(parent) {
	prefix = _prefix;
}

