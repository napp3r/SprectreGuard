#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);

private:
    // UI components
    QLabel *titleLabel;
    
    // Profile section
    QLabel *profilePhotoLabel;
    QLabel *nameLabel;
    QPushButton *changePhotoButton;
    
    // Obfuscation level section
    QLabel *obfuscationLevelLabel;
    QRadioButton *lowRadioButton;
    QRadioButton *mediumRadioButton;
    QRadioButton *highRadioButton;
    QButtonGroup *obfuscationLevelGroup;
    
    // VM detection section
    QLabel *vmDetectionLabel;
    QRadioButton *onRadioButton;
    QRadioButton *offRadioButton;
    QButtonGroup *vmDetectionGroup;
    
    // Save button
    QPushButton *saveChangesButton;
    
    void setupUI();
    void setupConnections();
    
private slots:
    void onSaveChanges();
    void onChangePhoto();
};

#endif // SETTINGSWIDGET_H 