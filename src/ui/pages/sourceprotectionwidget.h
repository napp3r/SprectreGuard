#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QProgressBar>
#include "../../protection/source_protection.h"

class QLineEdit;
class QCheckBox;
class QPushButton;
class QListWidget;
class QListWidgetItem;
class QTextEdit;
class QProgressBar;

class SourceProtectionWidget : public QWidget {
    Q_OBJECT

public:
    explicit SourceProtectionWidget(QWidget *parent = nullptr);

private slots:
    void onApplyProtection();
    void onBrowseFile();
    void onExtractStrings();
    void onToggleStringEncryption(bool checked);
    void onToggleAESStringEncryption(bool checked);
    void onSelectAllStringsClicked();
    void onSelectAllAESStringsClicked();

private:
    void setupUI();
    void createFileSelection();
    void createStringSelectionGroup();
    void createAESStringSelectionGroup();
    void createLogGroup();
    bool applySourceProtection(const QString& inputFile, const QString& outputPath);
    void extractStringsFromFile(const QString& filePath);
    void logMessage(const QString& message);
    void updateProgress(int value, const QString& status);

    QVBoxLayout *mainLayout;
    QLineEdit *sourceFileEdit;
    QCheckBox *obfuscateFunctionsCheck;
    QCheckBox *encryptStringsCheck;
    QCheckBox *aesEncryptStringsCheck;
    QPushButton *applyButton;
    QPushButton *extractStringsButton;
    QPushButton *selectAllStringsButton;
    QPushButton *selectAllAESStringsButton;

    // String encryption UI elements
    QWidget *stringSelectionGroup;
    QListWidget *stringListWidget;
    QListWidget *aesStringListWidget;
    QWidget *aesStringSelectionGroup;
    
    // Log and progress elements
    QTextEdit *logTextEdit;
    QProgressBar *progressBar;
}; 