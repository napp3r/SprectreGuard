#include <QFile>
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"

void loadStyleSheet(QApplication &app) {
    QFile file(":/styles.qss"); // Загружаем из ресурсов
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = file.readAll();
        app.setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Не удалось загрузить QSS из ресурсов!";
    }
}


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    loadStyleSheet(app);

    MainWindow window;
    window.show();
    return app.exec();
}
