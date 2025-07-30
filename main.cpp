#include "oappcore.h"
#include <QCommandLineParser>

int main(int argc, char *argv[])
{
    OAppCore app(argc, argv);
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption("remote-debugging-port", "Set webEngine debugging port", "3000", "3000"));
    parser.process(app);

    app.openBrowserWindow();
    return app.exec();
}
