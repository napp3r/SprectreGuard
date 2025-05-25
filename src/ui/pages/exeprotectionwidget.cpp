#include "exeprotectionwidget.h"
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
#include <QStandardPaths>
#include <QDebug>
#include <QSettings>
#include <QProcess>
#include <QFile>
#include <QInputDialog>
#include <QCoreApplication>
#include <QSlider>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QMetaObject>

ExeProtectionWidget::ExeProtectionWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void ExeProtectionWidget::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    // Title
    QLabel *titleLabel = new QLabel("Packing", this);
    titleLabel->setObjectName("pageTitle");
    mainLayout->addWidget(titleLabel);

    createFileSelection();
    createOptionsGroup();
    createProgressArea();

    // Apply button
    applyButton = new QPushButton("Apply Packing", this);
    connect(applyButton, &QPushButton::clicked, this, &ExeProtectionWidget::onApplyProtection);
    mainLayout->addWidget(applyButton);

    mainLayout->addStretch();
}

void ExeProtectionWidget::createFileSelection() {
    QGroupBox *fileGroup = new QGroupBox("Executable File", this);
    QHBoxLayout *fileLayout = new QHBoxLayout;

    exeFileEdit = new QLineEdit(this);
    QPushButton *browseButton = new QPushButton("Browse", this);
    connect(browseButton, &QPushButton::clicked, this, &ExeProtectionWidget::onBrowseFile);

    fileLayout->addWidget(exeFileEdit);
    fileLayout->addWidget(browseButton);
    fileGroup->setLayout(fileLayout);

    mainLayout->addWidget(fileGroup);
}

void ExeProtectionWidget::createOptionsGroup() {
    QGroupBox *optionsGroup = new QGroupBox("Packing Options", this);
    QVBoxLayout *optionsLayout = new QVBoxLayout;

    // Only keep UPX checkbox
    packExeCheck = new QCheckBox("Pack with UPX", this);
    
    optionsLayout->addWidget(packExeCheck);
    optionsGroup->setLayout(optionsLayout);
    mainLayout->addWidget(optionsGroup);
}

void ExeProtectionWidget::createProgressArea() {
    QGroupBox *progressGroup = new QGroupBox("Progress", this);
    QVBoxLayout *progressLayout = new QVBoxLayout;
    
    // Status label
    statusLabel = new QLabel("Ready", this);
    progressLayout->addWidget(statusLabel);
    
    // Progress bar
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setFormat("%p% complete");
    progressLayout->addWidget(progressBar);
    
    progressGroup->setLayout(progressLayout);
    mainLayout->addWidget(progressGroup);
}

void ExeProtectionWidget::updateProgress(int value, const QString& status) {
    if (progressBar) {
        progressBar->setValue(value);
        statusLabel->setText(status);
    }
}

void ExeProtectionWidget::onBrowseFile() {
    QString filePath = QFileDialog::getOpenFileName(this,
        "Select Executable File",
        QString(),
        "Executable Files (*.exe);;All Files (*)");

    if (!filePath.isEmpty()) {
        exeFileEdit->setText(filePath);
    }
}

bool ExeProtectionWidget::applyExeProtection(const QString& inputFile, const QString& outputPath) {
    ExeProtection::ProtectionConfig config;
    
    // Only keep UPX option
    config.useUPX = packExeCheck->isChecked();
    
    // Set progress callback
    config.progressCallback = [this](int progress, const std::string& status) {
        // Use QMetaObject::invokeMethod to safely update UI from another thread
        QMetaObject::invokeMethod(this, "updateProgress", 
                                  Qt::QueuedConnection,
                                  Q_ARG(int, progress), 
                                  Q_ARG(QString, QString::fromStdString(status)));
    };
    
    return ExeProtection::protect(
        inputFile.toStdString(),
        outputPath.toStdString(),
        config
    );
}

void ExeProtectionWidget::onApplyProtection() {
    if (exeFileEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select an executable file.");
        return;
    }

    // Reset progress bar
    updateProgress(0, "Starting packing process...");
    
    QString inputFile = exeFileEdit->text();
    QFileInfo fileInfo(inputFile);
    
    // Create output filename with timestamp
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString outputFileName = fileInfo.baseName() + "_packed_" + timestamp + ".exe";
    
    // Get save path from settings
    QString outputPath = Settings::instance().getExeProtectionDirectory() + "/" + outputFileName;

    // Create directory if it doesn't exist
    QDir().mkpath(Settings::instance().getExeProtectionDirectory());

    // Enable logging to console for debugging
    qDebug() << "Starting packing process for file:" << inputFile;
    qDebug() << "Output path:" << outputPath;
    qDebug() << "Packing options:"
             << "UPX:" << packExeCheck->isChecked();
    
    // Apply packing - progress updates will be handled by the callback
    bool success = applyExeProtection(inputFile, outputPath);

    if (success) {
        QMessageBox::information(this, "Success", 
            QString("Packing applied successfully!\nPacked file saved to:\n%1").arg(outputPath));
    } else {
        // Check if UPX tools exist
        QString errorMsg = "Failed to apply packing!\n";
        
        if (packExeCheck->isChecked()) {
            QString upxPath = QDir::current().absoluteFilePath("tools/upx/upx.exe");
            if (!QFileInfo::exists(upxPath)) {
                errorMsg += "\nUPX executable not found at: " + upxPath;
            }
        }
        
        errorMsg += "\n\nCheck console output for more details.";
        QMessageBox::critical(this, "Error", errorMsg);
    }
} 