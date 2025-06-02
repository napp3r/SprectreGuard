#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>

class SettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget* parent = nullptr);

private slots:
    void onSaveChanges();
    void updateSubDirs();

private:
    void setupUI();
    void setupConnections();

    QLineEdit *outputDirEdit;
    QLineEdit *sourceProtectionPathEdit;
    QLineEdit *exeProtectionPathEdit;
    QPushButton *saveChangesButton;
};

#endif // SETTINGSWIDGET_H 