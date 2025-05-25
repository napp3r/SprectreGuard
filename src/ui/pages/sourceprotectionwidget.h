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
    void onStringSelectionChanged();
    void onToggleStringEncryption(bool checked);
    void onSelectAllStringsClicked();

private:
    void setupUI();
    void createFileSelection();
    void createStringSelectionGroup();
    void createLogGroup();
    bool applySourceProtection(const QString& inputFile, const QString& outputPath);
    void extractStringsFromFile(const QString& filePath);
    void logMessage(const QString& message);
    void updateProgress(int value, const QString& status);

    QVBoxLayout *mainLayout;
    QLineEdit *sourceFileEdit;
    QCheckBox *obfuscateFunctionsCheck;
    QCheckBox *encryptStringsCheck;
    QPushButton *applyButton;
    QPushButton *extractStringsButton;
    QPushButton *selectAllStringsButton;

    // String encryption UI elements
    QWidget *stringSelectionGroup;
    QListWidget *stringListWidget;
    
    // Log and progress elements
    QTextEdit *logTextEdit;
    QProgressBar *progressBar;
}; 