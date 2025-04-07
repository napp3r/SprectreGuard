#include "encryptionwidget.h"
#include <QFont>

EncryptionWidget::EncryptionWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void EncryptionWidget::setupUI()
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Setup main content and sidebar
    setupContent();
    setupSidebar();

    // Add content and sidebar to main layout
    mainLayout->addLayout(contentLayout, 7);
    mainLayout->addWidget(rightSidebar, 3);

    setLayout(mainLayout);
}

void EncryptionWidget::setupContent()
{
    contentLayout = new QVBoxLayout();
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(20);

    // Title
    titleLabel = new QLabel("Encryption");
    titleLabel->setObjectName("pageTitle");

    // Input section
    inputHeaderLabel = new QLabel("Input");
    inputFrame = new QFrame();
    inputFrame->setObjectName("inputFrame");
    inputFrame->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout *inputLayout = new QVBoxLayout(inputFrame);
    inputLayout->setAlignment(Qt::AlignCenter);
    
    selectFileLabel = new QLabel("Select your file");
    selectFileLabel->setAlignment(Qt::AlignCenter);
    
    browseButton = new QPushButton("+ Browse");
    browseButton->setObjectName("browseButton");
    browseButton->setFixedSize(120, 40);
    
    inputLayout->addWidget(selectFileLabel);
    inputLayout->addSpacing(10);
    inputLayout->addWidget(browseButton, 0, Qt::AlignCenter);

    // Output section
    outputHeaderLabel = new QLabel("Output");
    outputFrame = new QFrame();
    outputFrame->setObjectName("outputFrame");
    outputFrame->setFrameShape(QFrame::StyledPanel);

    QVBoxLayout *outputLayout = new QVBoxLayout(outputFrame);
    outputLayout->setAlignment(Qt::AlignCenter);
    
    downloadLabel = new QLabel("Download");
    downloadLabel->setAlignment(Qt::AlignCenter);
    
    // Use text instead of image for download arrow
    downloadIcon = new QLabel("↓");
    QFont iconFont = downloadIcon->font();
    iconFont.setPointSize(24);
    downloadIcon->setFont(iconFont);
    downloadIcon->setAlignment(Qt::AlignCenter);
    
    outputLayout->addWidget(downloadLabel);
    outputLayout->addSpacing(10);
    outputLayout->addWidget(downloadIcon, 0, Qt::AlignCenter);

    // Action buttons
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(10);
    
    submitButton = new QPushButton("Submit");
    submitButton->setObjectName("submitButton");
    submitButton->setFixedSize(100, 40);
    
    tryAgainButton = new QPushButton("Try again");
    tryAgainButton->setObjectName("tryAgainButton");
    tryAgainButton->setFixedSize(100, 40);
    
    actionLayout->addWidget(submitButton);
    actionLayout->addWidget(tryAgainButton);
    actionLayout->addStretch();

    // Add all sections to content layout
    contentLayout->addWidget(titleLabel);
    contentLayout->addWidget(inputHeaderLabel);
    contentLayout->addWidget(inputFrame);
    contentLayout->addWidget(outputHeaderLabel);
    contentLayout->addWidget(outputFrame);
    contentLayout->addLayout(actionLayout);
    contentLayout->addStretch();
}

void EncryptionWidget::setupSidebar()
{
    rightSidebar = new QWidget();
    rightSidebar->setObjectName("rightSidebar");
    
    sidebarLayout = new QVBoxLayout(rightSidebar);
    sidebarLayout->setContentsMargins(20, 20, 20, 20);
    sidebarLayout->setSpacing(15);

    // Instructions section
    instructionsHeaderLabel = new QLabel("Instructions");
    
    instructionsTextLabel = new QLabel("We recommend doing encryption first before obfuscation for better anti-reversing");
    instructionsTextLabel->setWordWrap(true);
    
    // Type section
    typeLabel = new QLabel("Type");
    
    typeComboBox = new QComboBox();
    typeComboBox->addItem("SHA-256");
    typeComboBox->addItem("SHA-512");
    typeComboBox->addItem("MD5");
    typeComboBox->setFixedHeight(40);
    
    // Max strings section
    QHBoxLayout *maxStringsHeaderLayout = new QHBoxLayout();
    maxStringsLabel = new QLabel("Max strings");
    maxStringsValueLabel = new QLabel("256");
    maxStringsValueLabel->setAlignment(Qt::AlignRight);
    maxStringsHeaderLayout->addWidget(maxStringsLabel);
    maxStringsHeaderLayout->addWidget(maxStringsValueLabel);
    
    maxStringsSlider = new QSlider(Qt::Horizontal);
    maxStringsSlider->setRange(64, 512);
    maxStringsSlider->setValue(256);
    maxStringsSlider->setFixedHeight(20);
    connect(maxStringsSlider, &QSlider::valueChanged, [this](int value) {
        maxStringsValueLabel->setText(QString::number(value));
    });
    
    // Save settings button
    saveSettingsButton = new QPushButton("Save settings");
    saveSettingsButton->setObjectName("saveSettingsButton");
    saveSettingsButton->setFixedHeight(40);

    // Add all sections to sidebar layout
    sidebarLayout->addWidget(instructionsHeaderLabel);
    sidebarLayout->addWidget(instructionsTextLabel);
    sidebarLayout->addSpacing(10);
    sidebarLayout->addWidget(typeLabel);
    sidebarLayout->addWidget(typeComboBox);
    sidebarLayout->addSpacing(10);
    sidebarLayout->addLayout(maxStringsHeaderLayout);
    sidebarLayout->addWidget(maxStringsSlider);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(saveSettingsButton);
} 