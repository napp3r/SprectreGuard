#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>

class Sidebar : public QWidget {
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

signals:
    void pageSelected(int index);

private:
    void setupUI();
    void styleButton(QPushButton *button);
    void setupConnections();

    QVBoxLayout *mainLayout;
    QPushButton *homeButton;
    QPushButton *sourceProtectionButton;
    QPushButton *exeProtectionButton;
    QPushButton *settingsButton;
}; 