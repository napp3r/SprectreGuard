#ifndef ENCRYPTIONWIDGET_H
#define ENCRYPTIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QComboBox>
#include <QSlider>

class EncryptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EncryptionWidget(QWidget *parent = nullptr);

private:
    // Main layouts
    QHBoxLayout *mainLayout;
    QVBoxLayout *contentLayout;
    QVBoxLayout *sidebarLayout;

    // Content widgets
    QLabel *titleLabel;
    QFrame *inputFrame;
    QFrame *outputFrame;
    QLabel *inputHeaderLabel;
    QLabel *outputHeaderLabel;
    QLabel *selectFileLabel;
    QPushButton *browseButton;
    QLabel *downloadLabel;
    QLabel *downloadIcon;
    QPushButton *submitButton;
    QPushButton *tryAgainButton;

    // Sidebar widgets
    QWidget *rightSidebar;
    QLabel *instructionsHeaderLabel;
    QLabel *instructionsTextLabel;
    QLabel *typeLabel;
    QComboBox *typeComboBox;
    QLabel *maxStringsLabel;
    QSlider *maxStringsSlider;
    QLabel *maxStringsValueLabel;
    QPushButton *saveSettingsButton;

    void setupUI();
    void setupContent();
    void setupSidebar();
};

#endif // ENCRYPTIONWIDGET_H 