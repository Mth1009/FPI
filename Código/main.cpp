#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Laboratorio de Imagens");
    QApplication::setOrganizationName("UFRGS");

    MainWindow janela;
    janela.show();

    return app.exec();
}
