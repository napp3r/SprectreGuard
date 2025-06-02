#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include "../../protection/exe_protection.h"

class QLineEdit;
class QCheckBox;
class QPushButton;
class QGroupBox;
class QProgressBar;
class QLabel;

class ExeProtectionWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExeProtectionWidget(QWidget *parent = nullptr);

private slots:
    void onApplyProtection();
    void onBrowseFile();
    void updateProgress(int value, const QString& status);

private:
    void setupUI();
    void createFileSelection();
    void createOptionsGroup();
    void createProgressArea();
    bool applyExeProtection(const QString& inputFile, const QString& outputPath);

    QVBoxLayout *mainLayout;
    QLineEdit *exeFileEdit;
    QPushButton *applyButton;
    
    // Progress bar elements
    QProgressBar *progressBar;
    QLabel *statusLabel;
    
    // Only keep UPX option
    QCheckBox *packExeCheck;
}; 