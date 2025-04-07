#include "obfuscationwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QFileDialog>
#include <QStyle>

ObfuscationWidget::ObfuscationWidget(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    // --- Центральная область ---
    QWidget *centralContent = new QWidget;
    centralContent->setObjectName("centralContent");
    QVBoxLayout *centralLayout = new QVBoxLayout(centralContent);

    // Раздел "Input"
    QFrame *inputFrame = new QFrame;
    inputFrame->setObjectName("inputFrame");
    QVBoxLayout *inputLayout = new QVBoxLayout(inputFrame);
    QLabel *inputTitle = new QLabel("Input");
    inputTitle->setAlignment(Qt::AlignCenter);
    QFont boldFont = inputTitle->font();
    boldFont.setBold(true);
    inputTitle->setFont(boldFont);
    selectLabel = new QLabel("Select your file");
    selectLabel->setAlignment(Qt::AlignCenter);
    QPushButton *browseButton = new QPushButton("+ Browse");
    browseButton->setObjectName("browseButton");
    connect(browseButton, &QPushButton::clicked, this, &ObfuscationWidget::browseFile);
    inputLayout->addWidget(inputTitle);
    inputLayout->addWidget(selectLabel);
    inputLayout->addWidget(browseButton);
    inputLayout->setAlignment(browseButton, Qt::AlignCenter);
    centralLayout->addWidget(inputFrame);

    // Раздел "Output"
    QFrame *outputFrame = new QFrame;
    outputFrame->setObjectName("outputFrame");
    QVBoxLayout *outputLayout = new QVBoxLayout(outputFrame);
    QLabel *outputTitle = new QLabel("Output");
    outputTitle->setAlignment(Qt::AlignCenter);
    outputTitle->setFont(boldFont);
    QLabel *downloadLabel = new QLabel("Download");
    downloadLabel->setAlignment(Qt::AlignCenter);
    downloadLabel->setFont(boldFont);
    QLabel *arrowLabel = new QLabel;
    arrowLabel->setPixmap(style()->standardIcon(QStyle::SP_ArrowDown).pixmap(32, 32));
    arrowLabel->setAlignment(Qt::AlignCenter);
    outputLayout->addWidget(outputTitle);
    outputLayout->addWidget(downloadLabel);
    outputLayout->addWidget(arrowLabel);
    centralLayout->addWidget(outputFrame);

    // Нижние кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *submitButton = new QPushButton("Submit");
    submitButton->setObjectName("submitButton");
    QPushButton *tryAgainButton = new QPushButton("Try again");
    tryAgainButton->setObjectName("tryAgainButton");
    buttonLayout->addWidget(submitButton);
    buttonLayout->addWidget(tryAgainButton);
    centralLayout->addLayout(buttonLayout);

    mainLayout->addWidget(centralContent, 1);

    // --- Правая боковая панель ---
    QWidget *rightSidebar = new QWidget;
    rightSidebar->setObjectName("rightSidebar");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightSidebar);

    // Инструкции
    QLabel *instructions = new QLabel("We recommend doing encryption first before obfuscation for better anti-reversing.");
    instructions->setWordWrap(true);
    rightLayout->addWidget(instructions);

    // Выбор типа
    QLabel *typeLabel = new QLabel("Type:");
    QComboBox *typeCombo = new QComboBox;
    typeCombo->addItems({"SHA-256", "MD5", "AES"});
    typeCombo->setCurrentText("SHA-256");
    rightLayout->addWidget(typeLabel);
    rightLayout->addWidget(typeCombo);

    // Слайдер "Max strings"
    QLabel *maxStringsLabel = new QLabel("Max strings:");
    QSlider *maxStringsSlider = new QSlider(Qt::Horizontal);
    maxStringsSlider->setRange(0, 1024);
    maxStringsSlider->setValue(256);
    rightLayout->addWidget(maxStringsLabel);
    rightLayout->addWidget(maxStringsSlider);

    // Кнопка "Save settings"
    QPushButton *saveSettingsButton = new QPushButton("Save settings");
    saveSettingsButton->setObjectName("saveSettingsButton");
    rightLayout->addWidget(saveSettingsButton);

    rightLayout->addStretch();
    mainLayout->addWidget(rightSidebar);

    // Настройка отступов и расстояний
    mainLayout->setContentsMargins(10, 10, 10, 10);
    centralLayout->setSpacing(10);
    rightLayout->setSpacing(10);
}

void ObfuscationWidget::browseFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select File");
    if (!fileName.isEmpty()) {
        selectLabel->setText(fileName);
    }
}
