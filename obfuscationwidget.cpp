#include "obfuscationwidget.h"
#include "filehistory.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QFileDialog>
#include <QStyle>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <random>

ObfuscationWidget::ObfuscationWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void ObfuscationWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Title
    QLabel *titleLabel = new QLabel("Obfuscation");
    titleLabel->setObjectName("pageTitle");
    mainLayout->addWidget(titleLabel);

    // File selection section
    QHBoxLayout *fileLayout = new QHBoxLayout();
    
    selectLabel = new QLabel("No file selected");
    selectLabel->setObjectName("fileLabel");
    fileLayout->addWidget(selectLabel);
    
    QPushButton *browseButton = new QPushButton("Browse");
    browseButton->setObjectName("browseButton");
    fileLayout->addWidget(browseButton);
    
    mainLayout->addLayout(fileLayout);

    // Obfuscation type selection
    typeCombo = new QComboBox();
    typeCombo->addItems({"String Obfuscation", "Control Flow Obfuscation", "Data Obfuscation"});
    mainLayout->addWidget(typeCombo);

    // Max strings slider
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    QLabel *sliderLabel = new QLabel("Max Strings:");
    maxStringsSlider = new QSlider(Qt::Horizontal);
    maxStringsSlider->setRange(10, 100);
    maxStringsSlider->setValue(50);
    sliderLayout->addWidget(sliderLabel);
    sliderLayout->addWidget(maxStringsSlider);
    mainLayout->addLayout(sliderLayout);

    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *submitButton = new QPushButton("Submit");
    submitButton->setObjectName("submitButton");
    buttonLayout->addWidget(submitButton);
    
    QPushButton *tryAgainButton = new QPushButton("Try Again");
    tryAgainButton->setObjectName("tryAgainButton");
    buttonLayout->addWidget(tryAgainButton);
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
}

void ObfuscationWidget::setupConnections() {
    connect(findChild<QPushButton*>("browseButton"), &QPushButton::clicked, this, &ObfuscationWidget::browseFile);
    connect(findChild<QPushButton*>("submitButton"), &QPushButton::clicked, this, &ObfuscationWidget::processFile);
    connect(findChild<QPushButton*>("tryAgainButton"), &QPushButton::clicked, this, &ObfuscationWidget::tryAgain);
}

void ObfuscationWidget::browseFile() {
    QString fileName = QFileDialog::getOpenFileName(this,
        "Select File to Obfuscate", "", "All Files (*.*)");
    
    if (!fileName.isEmpty()) {
        currentFilePath = fileName;
        selectLabel->setText(QFileInfo(fileName).fileName());
    }
}

void ObfuscationWidget::openFile(const QString &filePath) {
    if (QFile::exists(filePath)) {
        currentFilePath = filePath;
        selectLabel->setText(QFileInfo(filePath).fileName());
    } else {
        QMessageBox::warning(this, "Error", "File not found: " + filePath);
    }
}

QString ObfuscationWidget::getObfuscationLevel() const {
    QSettings settings;
    return settings.value("obfuscation/level", "Low").toString();
}

bool ObfuscationWidget::obfuscateFile(const QString &inputPath, const QString &outputPath) {
    QFile inputFile(inputPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QFile outputFile(outputPath);
    if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        inputFile.close();
        return false;
    }

    QTextStream in(&inputFile);
    QTextStream out(&outputFile);
    
    QString obfuscationLevel = getObfuscationLevel();
    int maxStrings = maxStringsSlider->value();
    
    QString content = in.readAll();
    
    // String obfuscation using regex iterator
    QRegularExpression stringPattern("\"[^\"]*\"");
    QRegularExpressionMatchIterator matchIterator = stringPattern.globalMatch(content);
    
    int stringCount = 0;
    QVector<QPair<int, int>> positions;
    QStringList replacements;
    
    // First collect all matches and calculate replacements
    while (matchIterator.hasNext() && stringCount < maxStrings) {
        QRegularExpressionMatch match = matchIterator.next();
        QString str = match.captured();
        QString obfuscated = "QString::fromUtf8(QByteArray::fromBase64(\"" + 
                           str.toUtf8().toBase64() + "\"))";
        
        positions.append(qMakePair(match.capturedStart(), match.capturedLength()));
        replacements.append(obfuscated);
        stringCount++;
    }
    
    // Apply replacements in reverse order to avoid invalidating positions
    for (int i = positions.size() - 1; i >= 0; --i) {
        int pos = positions[i].first;
        int len = positions[i].second;
        content.replace(pos, len, replacements[i]);
    }
    
    out << content;
    
    inputFile.close();
    outputFile.close();
    return true;
}

void ObfuscationWidget::processFile() {
    if (currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a file first.");
        return;
    }

    QString outputPath = currentFilePath + ".obfuscated";
    if (obfuscateFile(currentFilePath, outputPath)) {
        // Add to file history
        FileHistory::addEntry(currentFilePath, getObfuscationLevel(), "Obfuscation");
        
        QMessageBox::information(this, "Success", 
            "File obfuscated successfully!\nSaved as: " + outputPath);
            
        // Emit signal that file was processed
        emit fileProcessed();
    } else {
        QMessageBox::critical(this, "Error", 
            "Failed to obfuscate file. Please check file permissions.");
    }
}

void ObfuscationWidget::tryAgain() {
    currentFilePath.clear();
    selectLabel->setText("No file selected");
    typeCombo->setCurrentIndex(0);
    maxStringsSlider->setValue(50);
}

void ObfuscationWidget::saveSettings() {
    QSettings settings;
    settings.setValue("obfuscation/type", typeCombo->currentText());
    settings.setValue("obfuscation/maxStrings", maxStringsSlider->value());
}
