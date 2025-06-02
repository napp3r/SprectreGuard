#pragma once

#include <QMainWindow>
#include <QStackedWidget>

class Sidebar;
class HomeWidget;
class SourceProtectionWidget;
class ExeProtectionWidget;
class SettingsWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchPage(int index);

private:
    void setupUI();
    void setupConnections();

    QStackedWidget *stackedWidget;
    Sidebar *sidebar;
    HomeWidget *homePage;
    SourceProtectionWidget *sourceProtectionPage;
    ExeProtectionWidget *exeProtectionPage;
    SettingsWidget *settingsPage;
}; 