#include "sourceprotectionwidget.h"
#include "../../core/settings.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QFileInfo>
#include <QDateTime>
#include <QDir>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextEdit>
#include <QProgressBar>
#include <QRegularExpression>
#include <fstream>
#include <sstream>
#include "../../protection/source_protection.h"
#include <QStandardPaths>
#include <QFile>

SourceProtectionWidget::SourceProtectionWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void SourceProtectionWidget::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel *titleLabel = new QLabel("Source Obfuscation", this);
    titleLabel->setObjectName("pageTitle");
    mainLayout->addWidget(titleLabel);

    createFileSelection();
    createStringSelectionGroup();
    createLogGroup();

    // Protection options
    QGroupBox *optionsGroup = new QGroupBox("Obfuscation Options", this);
    QVBoxLayout *optionsLayout = new QVBoxLayout;

    obfuscateFunctionsCheck = new QCheckBox("Obfuscation", this);
    encryptStringsCheck = new QCheckBox("Strings Encryption", this);
    
    optionsLayout->addWidget(obfuscateFunctionsCheck);
    optionsLayout->addWidget(encryptStringsCheck);
    optionsGroup->setLayout(optionsLayout);
    mainLayout->addWidget(optionsGroup);

    // Apply button
    applyButton = new QPushButton("Apply Obfuscation", this);
    connect(applyButton, &QPushButton::clicked, this, &SourceProtectionWidget::onApplyProtection);
    mainLayout->addWidget(applyButton);

    mainLayout->addStretch();

    // Initially hide string selection
    if (stringSelectionGroup) {
        stringSelectionGroup->setVisible(false);
    }
    
    // Connect encrypt strings checkbox
    connect(encryptStringsCheck, &QCheckBox::toggled, this, &SourceProtectionWidget::onToggleStringEncryption);
    
    // Log initial message
    logMessage("Source Obfuscation ready. Select a file and options to begin.");
}

void SourceProtectionWidget::createFileSelection() {
    QGroupBox *fileGroup = new QGroupBox("Source File", this);
    QHBoxLayout *fileLayout = new QHBoxLayout;

    sourceFileEdit = new QLineEdit(this);
    QPushButton *browseButton = new QPushButton("Browse", this);
    extractStringsButton = new QPushButton("Extract Strings", this);
    
    connect(browseButton, &QPushButton::clicked, this, &SourceProtectionWidget::onBrowseFile);
    connect(extractStringsButton, &QPushButton::clicked, this, &SourceProtectionWidget::onExtractStrings);

    fileLayout->addWidget(sourceFileEdit);
    fileLayout->addWidget(browseButton);
    fileLayout->addWidget(extractStringsButton);
    fileGroup->setLayout(fileLayout);

    mainLayout->addWidget(fileGroup);
}

void SourceProtectionWidget::createStringSelectionGroup() {
    stringSelectionGroup = new QGroupBox("Strings to Encrypt", this);
    QVBoxLayout *stringLayout = new QVBoxLayout;

    // String list
    stringListWidget = new QListWidget(this);
    stringListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    stringLayout->addWidget(stringListWidget);

    // Select All button
    selectAllStringsButton = new QPushButton("Select All", this);
    connect(selectAllStringsButton, &QPushButton::clicked, this, &SourceProtectionWidget::onSelectAllStringsClicked);
    stringLayout->addWidget(selectAllStringsButton);

    stringSelectionGroup->setLayout(stringLayout);
    mainLayout->addWidget(stringSelectionGroup);

    // Connect signals
    connect(stringListWidget, &QListWidget::itemSelectionChanged, this, &SourceProtectionWidget::onStringSelectionChanged);
}

void SourceProtectionWidget::createLogGroup() {
    QGroupBox *logGroup = new QGroupBox("Obfuscation Log", this);
    QVBoxLayout *logLayout = new QVBoxLayout;
    
    // Log text area
    logTextEdit = new QTextEdit(this);
    logTextEdit->setReadOnly(true);
    logTextEdit->setMinimumHeight(150);
    logLayout->addWidget(logTextEdit);
    
    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    logLayout->addWidget(progressBar);
    
    logGroup->setLayout(logLayout);
    mainLayout->addWidget(logGroup);
}

void SourceProtectionWidget::extractStringsFromFile(const QString& filePath) {
    std::ifstream file(filePath.toStdString());
    if (!file.is_open()) {
        logMessage("Error: Could not open file for string extraction");
        return;
    }

    stringListWidget->clear();
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Regular expression to match string literals
    QRegularExpression stringRegex("\"([^\"]*)\"");
    QRegularExpressionMatchIterator it = stringRegex.globalMatch(QString::fromStdString(content));
    
    std::set<QString> uniqueStrings;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString str = match.captured(1);
        if (!str.isEmpty() && str.length() > 1) { // Skip empty strings and single characters
            uniqueStrings.insert(str);
        }
    }

    for (const QString& str : uniqueStrings) {
        QListWidgetItem* item = new QListWidgetItem(str);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
        stringListWidget->addItem(item);
    }

    logMessage(QString("Extracted %1 unique strings").arg(uniqueStrings.size()));
}

void SourceProtectionWidget::onExtractStrings() {
    QString filePath = sourceFileEdit->text();
    if (filePath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a source file first");
        return;
    }
    extractStringsFromFile(filePath);
}

void SourceProtectionWidget::onToggleStringEncryption(bool checked) {
    if (stringSelectionGroup) {
        stringSelectionGroup->setVisible(checked);
    }
}

void SourceProtectionWidget::onStringSelectionChanged() {
    // Update UI based on string selection if needed
}

void SourceProtectionWidget::onBrowseFile() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select Source File", "", "C++ Files (*.cpp *.h);;All Files (*)");
    if (!filePath.isEmpty()) {
        sourceFileEdit->setText(filePath);
        extractStringsFromFile(filePath);
    }
}

void SourceProtectionWidget::onApplyProtection() {
    QString inputFile = sourceFileEdit->text();
    if (inputFile.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a source file first");
        return;
    }

    // Получаем путь для сохранения из настроек
    QString outputDir = Settings::instance().getSourceProtectionDirectory();

    logMessage(QString("Creating output directory: %1").arg(outputDir));
    if (!QDir().mkpath(outputDir)) {
        logMessage("Error: Failed to create output directory");
        QMessageBox::critical(this, "Error", "Failed to create output directory!");
        return;
    }

    QFileInfo fileInfo(inputFile);
    QString outputPath = outputDir + "/" + fileInfo.baseName() + "_protected." + fileInfo.suffix();

    logMessage(QString("Input file: %1").arg(inputFile));
    logMessage(QString("Output path: %1").arg(outputPath));

    // Check if input file exists
    if (!QFile::exists(inputFile)) {
        logMessage("Error: Input file does not exist");
        QMessageBox::critical(this, "Error", "Input file does not exist!");
        return;
    }

    if (applySourceProtection(inputFile, outputPath)) {
        // Verify that the output file was created
        if (QFile::exists(outputPath)) {
            QFileInfo outputFileInfo(outputPath);
            logMessage(QString("Obfuscation completed successfully. Output saved to: %1").arg(outputPath));
            logMessage(QString("File size: %1 bytes").arg(outputFileInfo.size()));
            QMessageBox::information(this, "Success", 
                QString("File has been successfully obfuscated!\n\nProtected file saved to:\n%1")
                .arg(outputPath));
        } else {
            logMessage("Error: Output file was not created");
            QMessageBox::critical(this, "Error", "Output file was not created!");
        }
    } else {
        logMessage("Failed to apply obfuscation");
        QMessageBox::critical(this, "Error", "Failed to obfuscate file!\n\nPlease check the log for details.");
    }
}

bool SourceProtectionWidget::applySourceProtection(const QString& inputFile, const QString& outputPath) {
    logMessage("Starting obfuscation process...");
    
    SourceProtection::ProtectionConfig config;
    
    // Set protection options
    config.obfuscateNames = obfuscateFunctionsCheck->isChecked();
    config.encryptStrings = encryptStringsCheck->isChecked();
    
    logMessage(QString("Obfuscation options:"));
    logMessage(QString("- Obfuscation: %1").arg(config.obfuscateNames ? "enabled" : "disabled"));
    logMessage(QString("- Strings Encryption: %1").arg(config.encryptStrings ? "enabled" : "disabled"));
    
    // Collect selected strings for encryption
    if (config.encryptStrings) {
        logMessage("Collecting strings to encrypt...");
        for (int i = 0; i < stringListWidget->count(); ++i) {
            QListWidgetItem* item = stringListWidget->item(i);
            if (item->checkState() == Qt::Checked) {
                config.stringsToEncrypt.push_back(item->text().toStdString());
                logMessage(QString("  - Added string: %1").arg(item->text()));
            }
        }
        logMessage(QString("Total strings to encrypt: %1").arg(config.stringsToEncrypt.size()));
    }

    // Set progress callback
    config.progressCallback = [this](int progress, const std::string& status) {
        updateProgress(progress, QString::fromStdString(status));
        logMessage(QString("Progress: %1% - %2").arg(progress).arg(QString::fromStdString(status)));
    };

    logMessage("Calling protection function...");
    bool result = SourceProtection::protect(inputFile.toStdString(), outputPath.toStdString(), config);
    logMessage(QString("Protection function returned: %1").arg(result ? "success" : "failure"));
    
    return result;
}

void SourceProtectionWidget::logMessage(const QString& message) {
    if (logTextEdit) {
        QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
        logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(message));
        QTextCursor cursor = logTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        logTextEdit->setTextCursor(cursor);
    }
}

void SourceProtectionWidget::updateProgress(int value, const QString& status) {
    if (progressBar) {
        progressBar->setValue(value);
        progressBar->setFormat(QString("%1% - %2").arg(value).arg(status));
    }
}

void SourceProtectionWidget::onSelectAllStringsClicked() {
    for (int i = 0; i < stringListWidget->count(); ++i) {
        QListWidgetItem* item = stringListWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
} 