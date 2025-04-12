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
    void openFileFromHistory(const QString &filePath, const QString &processType);

private:
    QStackedWidget *stackedWidget;
    Sidebar *sidebar;
    HistoryWidget *historyPage;
    ObfuscationWidget *obfuscationPage;
    EncryptionWidget *encryptionPage;
};

#endif // MAINWINDOW_H
