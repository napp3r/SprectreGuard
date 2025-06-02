#include "settingswidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QDir>
#include <QSettings>
#include "src/core/settings.h"

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
    
    // Load saved settings
    QSettings settings;
    
    // Obfuscation level
    QString obfuscationLevel = settings.value("obfuscationLevel", "Low").toString();
    if (obfuscationLevel == "Low")
        lowRadioButton->setChecked(true);
    else if (obfuscationLevel == "Medium")
        mediumRadioButton->setChecked(true);
    else if (obfuscationLevel == "High")
        highRadioButton->setChecked(true);
    
    // VM detection
    bool vmDetection = settings.value("vmDetection", false).toBool();
    if (vmDetection)
        onRadioButton->setChecked(true);
    else
        offRadioButton->setChecked(true);
    
    // LLVM obfuscation settings
    bool useLLVMObfuscation = settings.value("useLLVMObfuscation", false).toBool();
    useLLVMObfuscationCheckbox->setChecked(useLLVMObfuscation);
    
    QString llvmPath = settings.value("llvmPath", "").toString();
    llvmPathEdit->setText(llvmPath);
    
    controlFlowFlatteningCheckbox->setChecked(settings.value("controlFlowFlattening", true).toBool());
    instructionSubstitutionCheckbox->setChecked(settings.value("instructionSubstitution", true).toBool());
    bogusControlFlowCheckbox->setChecked(settings.value("bogusControlFlow", true).toBool());
    deadCodeInsertionCheckbox->setChecked(settings.value("deadCodeInsertion", true).toBool());
    stringEncryptionCheckbox->setChecked(settings.value("stringEncryption", true).toBool());

    // Load output directory settings
    outputDirectoryEdit->setText(Settings::instance().getOutputDirectory());
    exeProtectionDirectoryEdit->setText(Settings::instance().getExeProtectionDirectory());
    sourceProtectionDirectoryEdit->setText(Settings::instance().getSourceProtectionDirectory());
}

void SettingsWidget::setupUI()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Title
    titleLabel = new QLabel("Settings");
    titleLabel->setObjectName("pageTitle");
    mainLayout->addWidget(titleLabel);

    // Obfuscation level section
    obfuscationLevelLabel = new QLabel("Obfuscation level");
    obfuscationLevelLabel->setObjectName("sectionLabel");
    mainLayout->addWidget(obfuscationLevelLabel);

    obfuscationLevelGroup = new QButtonGroup(this);

    lowRadioButton = new QRadioButton("Low");
    lowRadioButton->setChecked(true); // Default selected
    obfuscationLevelGroup->addButton(lowRadioButton);
    mainLayout->addWidget(lowRadioButton);

    mediumRadioButton = new QRadioButton("Medium");
    obfuscationLevelGroup->addButton(mediumRadioButton);
    mainLayout->addWidget(mediumRadioButton);

    highRadioButton = new QRadioButton("High");
    obfuscationLevelGroup->addButton(highRadioButton);
    mainLayout->addWidget(highRadioButton);

    mainLayout->addSpacing(20);

    // VM detection section
    vmDetectionLabel = new QLabel("Virtual machine detection");
    vmDetectionLabel->setObjectName("sectionLabel");
    mainLayout->addWidget(vmDetectionLabel);

    vmDetectionGroup = new QButtonGroup(this);

    onRadioButton = new QRadioButton("On");
    vmDetectionGroup->addButton(onRadioButton);
    mainLayout->addWidget(onRadioButton);

    offRadioButton = new QRadioButton("Off");
    offRadioButton->setChecked(true); // Default selected
    vmDetectionGroup->addButton(offRadioButton);
    mainLayout->addWidget(offRadioButton);
    
    // LLVM obfuscation section
    mainLayout->addSpacing(20);
    QLabel *llvmObfuscationLabel = new QLabel("LLVM-based Obfuscation");
    llvmObfuscationLabel->setObjectName("sectionLabel");
    mainLayout->addWidget(llvmObfuscationLabel);
    
    useLLVMObfuscationCheckbox = new QCheckBox("Use LLVM Obfuscation");
    useLLVMObfuscationCheckbox->setChecked(false);
    mainLayout->addWidget(useLLVMObfuscationCheckbox);
    
    QHBoxLayout *llvmPathLayout = new QHBoxLayout();
    llvmPathLabel = new QLabel("LLVM Path:");
    llvmPathEdit = new QLineEdit();
    llvmPathEdit->setPlaceholderText("Path to LLVM bin directory");
    browseLLVMPathButton = new QPushButton("Browse...");
    
    llvmPathLayout->addWidget(llvmPathLabel);
    llvmPathLayout->addWidget(llvmPathEdit, 1);
    llvmPathLayout->addWidget(browseLLVMPathButton);
    mainLayout->addLayout(llvmPathLayout);
    
    // LLVM obfuscation options
    QLabel *llvmOptionsLabel = new QLabel("LLVM Obfuscation Options:");
    mainLayout->addWidget(llvmOptionsLabel);
    
    controlFlowFlatteningCheckbox = new QCheckBox("Control Flow Flattening");
    controlFlowFlatteningCheckbox->setChecked(true);
    mainLayout->addWidget(controlFlowFlatteningCheckbox);
    
    instructionSubstitutionCheckbox = new QCheckBox("Instruction Substitution");
    instructionSubstitutionCheckbox->setChecked(true);
    mainLayout->addWidget(instructionSubstitutionCheckbox);
    
    bogusControlFlowCheckbox = new QCheckBox("Bogus Control Flow");
    bogusControlFlowCheckbox->setChecked(true);
    mainLayout->addWidget(bogusControlFlowCheckbox);
    
    deadCodeInsertionCheckbox = new QCheckBox("Dead Code Insertion");
    deadCodeInsertionCheckbox->setChecked(true);
    mainLayout->addWidget(deadCodeInsertionCheckbox);
    
    stringEncryptionCheckbox = new QCheckBox("String Encryption");
    stringEncryptionCheckbox->setChecked(true);
    mainLayout->addWidget(stringEncryptionCheckbox);
    
    // Output directories section
    mainLayout->addSpacing(20);
    outputPathsLabel = new QLabel("Output Directories");
    outputPathsLabel->setObjectName("sectionLabel");
    mainLayout->addWidget(outputPathsLabel);
    
    // Source protection output directory
    QHBoxLayout *sourceProtectionPathLayout = new QHBoxLayout();
    sourceProtectionPathLabel = new QLabel("Source Protection Output:");
    sourceProtectionPathEdit = new QLineEdit();
    sourceProtectionPathEdit->setPlaceholderText("Select directory for source protection output");
    browseSourceProtectionPathButton = new QPushButton("Browse...");
    
    sourceProtectionPathLayout->addWidget(sourceProtectionPathLabel);
    sourceProtectionPathLayout->addWidget(sourceProtectionPathEdit, 1);
    sourceProtectionPathLayout->addWidget(browseSourceProtectionPathButton);
    mainLayout->addLayout(sourceProtectionPathLayout);
    
    // EXE protection output directory
    QHBoxLayout *exeProtectionPathLayout = new QHBoxLayout();
    exeProtectionPathLabel = new QLabel("EXE Protection Output:");
    exeProtectionPathEdit = new QLineEdit();
    exeProtectionPathEdit->setPlaceholderText("Select directory for EXE protection output");
    browseExeProtectionPathButton = new QPushButton("Browse...");
    
    exeProtectionPathLayout->addWidget(exeProtectionPathLabel);
    exeProtectionPathLayout->addWidget(exeProtectionPathEdit, 1);
    exeProtectionPathLayout->addWidget(browseExeProtectionPathButton);
    mainLayout->addLayout(exeProtectionPathLayout);
    
    // Save button
    saveChangesButton = new QPushButton("Save changes");
    saveChangesButton->setObjectName("saveChangesButton");
    saveChangesButton->setFixedWidth(120);
    mainLayout->addWidget(saveChangesButton, 0, Qt::AlignLeft);

    mainLayout->addStretch();
}

void SettingsWidget::setupConnections()
{
    connect(saveChangesButton, &QPushButton::clicked, this, &SettingsWidget::onSaveChanges);
    connect(browseLLVMPathButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseLLVMPath);
    connect(browseSourceProtectionPathButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseSourceProtectionPath);
    connect(browseExeProtectionPathButton, &QPushButton::clicked, this, &SettingsWidget::onBrowseExeProtectionPath);
    
    // Enable/disable LLVM options based on checkbox state
    connect(useLLVMObfuscationCheckbox, &QCheckBox::toggled, [this](bool checked) {
        llvmPathLabel->setEnabled(checked);
        llvmPathEdit->setEnabled(checked);
        browseLLVMPathButton->setEnabled(checked);
        controlFlowFlatteningCheckbox->setEnabled(checked);
        instructionSubstitutionCheckbox->setEnabled(checked);
        bogusControlFlowCheckbox->setEnabled(checked);
        deadCodeInsertionCheckbox->setEnabled(checked);
        stringEncryptionCheckbox->setEnabled(checked);
    });
    
    // Initialize the enabled state
    useLLVMObfuscationCheckbox->toggled(useLLVMObfuscationCheckbox->isChecked());

    // Connect output directory changes to update subdirectories
    connect(outputDirectoryEdit, &QLineEdit::textChanged, [this](const QString &text) {
        // Only update subdirectories if they follow the default pattern
        QString currentOutputDir = Settings::instance().getOutputDirectory();
        QString exeProtectionDir = Settings::instance().getExeProtectionDirectory();
        QString sourceProtectionDir = Settings::instance().getSourceProtectionDirectory();
        
        if (exeProtectionDir == currentOutputDir + "/exe_protection") {
            exeProtectionDirectoryEdit->setText(text + "/exe_protection");
        }
        
        if (sourceProtectionDir == currentOutputDir + "/source_protection") {
            sourceProtectionDirectoryEdit->setText(text + "/source_protection");
        }
    });
}

void SettingsWidget::onSaveChanges()
{
    // Collect settings
    QString obfuscationLevel;
    if (lowRadioButton->isChecked())
        obfuscationLevel = "Low";
    else if (mediumRadioButton->isChecked())
        obfuscationLevel = "Medium";
    else
        obfuscationLevel = "High";

    bool vmDetection = onRadioButton->isChecked();
    
    // LLVM obfuscation settings
    bool useLLVMObfuscation = useLLVMObfuscationCheckbox->isChecked();
    QString llvmPath = llvmPathEdit->text();
    bool controlFlowFlattening = controlFlowFlatteningCheckbox->isChecked();
    bool instructionSubstitution = instructionSubstitutionCheckbox->isChecked();
    bool bogusControlFlow = bogusControlFlowCheckbox->isChecked();
    bool deadCodeInsertion = deadCodeInsertionCheckbox->isChecked();
    bool stringEncryption = stringEncryptionCheckbox->isChecked();

    // Save settings
    QSettings settings;
    settings.setValue("obfuscationLevel", obfuscationLevel);
    settings.setValue("vmDetection", vmDetection);
    settings.setValue("useLLVMObfuscation", useLLVMObfuscation);
    settings.setValue("llvmPath", llvmPath);
    settings.setValue("controlFlowFlattening", controlFlowFlattening);
    settings.setValue("instructionSubstitution", instructionSubstitution);
    settings.setValue("bogusControlFlow", bogusControlFlow);
    settings.setValue("deadCodeInsertion", deadCodeInsertion);
    settings.setValue("stringEncryption", stringEncryption);

    // Save output directory settings
    settings.setValue("sourceProtectionPath", sourceProtectionPathEdit->text());
    settings.setValue("exeProtectionPath", exeProtectionPathEdit->text());
    
    // Create directories if they don't exist
    QDir dir;
    dir.mkpath(sourceProtectionPathEdit->text());
    dir.mkpath(exeProtectionPathEdit->text());
    
    QMessageBox::information(this, "Settings", "Settings saved successfully.");
}

void SettingsWidget::onBrowseLLVMPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select LLVM bin Directory", 
                                                   QDir::homePath(),
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        llvmPathEdit->setText(dir);
    }
}

void SettingsWidget::onBrowseSourceProtectionPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select Source Protection Output Directory", 
                                                   QDir::homePath(),
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        sourceProtectionPathEdit->setText(dir);
    }
}

void SettingsWidget::onBrowseExeProtectionPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Select EXE Protection Output Directory", 
                                                   QDir::homePath(),
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty()) {
        exeProtectionPathEdit->setText(dir);
    }
}
