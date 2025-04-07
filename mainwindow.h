#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QStackedWidget>

// Forward declarations
class HistoryWidget;
class ObfuscationWidget;
class EncryptionWidget;
class Sidebar;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchPage(int index);

private:
    QStackedWidget *stackedWidget;
    Sidebar *sidebar;
};

#endif // MAINWINDOW_H
