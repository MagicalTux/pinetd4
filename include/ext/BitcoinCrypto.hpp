#include <QByteArray>

class BitcoinCrypto {
public:
	static bool init();
	static QByteArray doubleSha256(const QByteArray &input);
};

