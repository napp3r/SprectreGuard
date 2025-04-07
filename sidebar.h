#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class SidebarButton : public QPushButton {
    Q_OBJECT
public:
    explicit SidebarButton(const QString &text, const QString &iconPath = QString(), QWidget *parent = nullptr);
    void setActive(bool active);
};

class Sidebar : public QWidget {
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    SidebarButton *homeButton;
    SidebarButton *obfuscationButton;
    SidebarButton *encryptionButton;
    SidebarButton *settingsButton;
    SidebarButton *messagesButton;

    void setupUI();
    void setActiveButton(SidebarButton *button);

private slots:
    void handleButtonClick();
};

#endif // SIDEBAR_H
