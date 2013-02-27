#include <QCoreApplication>
#include <QLibrary>

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);

	QLibrary lib("mod/core");
	lib.setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
	if (!lib.load()) {
		qDebug("Failed to load core module: %s", qPrintable(lib.errorString()));
		return 1;
	}
	void (*ptr)() = (void(*)())lib.resolve("pinetd_main");
	if (!ptr) {
		qDebug("Failed to initialize core module, giving up");
		return 2;
	}
	ptr();

	return app.exec();
}

