#pragma once

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QCheckBox>
#include <QFileDialog>

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);

private slots:
    void onSaveChanges();
    void onBrowseLLVMPath();
    void onBrowseSourceProtectionPath();
    void onBrowseExeProtectionPath();

private:
    void setupUI();
    void setupConnections();

    // Title
    QLabel *titleLabel;

    // Obfuscation level
    QLabel *obfuscationLevelLabel;
    QButtonGroup *obfuscationLevelGroup;
    QRadioButton *lowRadioButton;
    QRadioButton *mediumRadioButton;
    QRadioButton *highRadioButton;

    // VM detection
    QLabel *vmDetectionLabel;
    QButtonGroup *vmDetectionGroup;
    QRadioButton *onRadioButton;
    QRadioButton *offRadioButton;
    
    // LLVM obfuscation settings
    QCheckBox *useLLVMObfuscationCheckbox;
    QLabel *llvmPathLabel;
    QLineEdit *llvmPathEdit;
    QPushButton *browseLLVMPathButton;
    QCheckBox *controlFlowFlatteningCheckbox;
    QCheckBox *instructionSubstitutionCheckbox;
    QCheckBox *bogusControlFlowCheckbox;
    QCheckBox *deadCodeInsertionCheckbox;
    QCheckBox *stringEncryptionCheckbox;
    
    // Output directories settings
    QLabel *outputPathsLabel;
    QLabel *sourceProtectionPathLabel;
    QLineEdit *sourceProtectionPathEdit;
    QPushButton *browseSourceProtectionPathButton;
    QLabel *exeProtectionPathLabel;
    QLineEdit *exeProtectionPathEdit;
    QPushButton *browseExeProtectionPathButton;

    // Save button
    QPushButton *saveChangesButton;
};
