#include "settingswidget.h"
#include "../../core/settings.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QSettings>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();

    // Load saved settings
    QSettings settings;
    QString outputDir = settings.value("output_directory", QDir::homePath() + "/Documents/SpectreGuard").toString();
    outputDirEdit->setText(outputDir);
    updateSubDirs();
}

void SettingsWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Output directory section
    QLabel *outputDirLabel = new QLabel("Base Output Directory:");
    outputDirEdit = new QLineEdit();
    outputDirEdit->setPlaceholderText("Base directory for all outputs");
    QPushButton *browseOutputDirButton = new QPushButton("Browse...");
    QHBoxLayout *outputDirLayout = new QHBoxLayout();
    outputDirLayout->addWidget(outputDirLabel);
    outputDirLayout->addWidget(outputDirEdit, 1);
    outputDirLayout->addWidget(browseOutputDirButton);
    mainLayout->addLayout(outputDirLayout);

    // Source protection output directory (read-only, auto)
    QLabel *sourceProtectionPathLabel = new QLabel("Source Protection Output:");
    sourceProtectionPathEdit = new QLineEdit();
    sourceProtectionPathEdit->setReadOnly(true);
    QHBoxLayout *sourceProtectionPathLayout = new QHBoxLayout();
    sourceProtectionPathLayout->addWidget(sourceProtectionPathLabel);
    sourceProtectionPathLayout->addWidget(sourceProtectionPathEdit, 1);
    mainLayout->addLayout(sourceProtectionPathLayout);

    // EXE protection output directory (read-only, auto)
    QLabel *exeProtectionPathLabel = new QLabel("EXE Protection Output:");
    exeProtectionPathEdit = new QLineEdit();
    exeProtectionPathEdit->setReadOnly(true);
    QHBoxLayout *exeProtectionPathLayout = new QHBoxLayout();
    exeProtectionPathLayout->addWidget(exeProtectionPathLabel);
    exeProtectionPathLayout->addWidget(exeProtectionPathEdit, 1);
    mainLayout->addLayout(exeProtectionPathLayout);

    // Save changes button
    mainLayout->addSpacing(30);
    saveChangesButton = new QPushButton("Save changes");
    saveChangesButton->setObjectName("saveChangesButton");
    saveChangesButton->setFixedWidth(120);
    mainLayout->addWidget(saveChangesButton, 0, Qt::AlignLeft);

    mainLayout->addStretch();

    // Connections for browse and edit
    connect(browseOutputDirButton, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select Base Output Directory",
                                                       outputDirEdit->text().isEmpty() ? QDir::homePath() : outputDirEdit->text(),
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        if (!dir.isEmpty()) {
            outputDirEdit->setText(dir);
            updateSubDirs();
        }
    });
    connect(outputDirEdit, &QLineEdit::textChanged, this, &SettingsWidget::updateSubDirs);
}

void SettingsWidget::setupConnections()
{
    connect(saveChangesButton, &QPushButton::clicked, this, &SettingsWidget::onSaveChanges);
}

void SettingsWidget::updateSubDirs()
{
    QString base = outputDirEdit->text();
    sourceProtectionPathEdit->setText(base + "/source_protection");
    exeProtectionPathEdit->setText(base + "/exe_protection");
}

void SettingsWidget::onSaveChanges()
{
    QString outputDir = outputDirEdit->text();
    Settings::instance().setOutputDirectory(outputDir);
    QMessageBox::information(this, "Settings Saved", "Settings saved successfully.");
} 