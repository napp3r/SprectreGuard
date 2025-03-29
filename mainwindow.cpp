#include "mainwindow.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QFileDialog>
#include <QFile>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    // Главное окно
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // --- Боковая панель ---
    QVBoxLayout *sidebarLayout = new QVBoxLayout;
    QStringList menuItems = {"Home", "Obfuscation", "Encryption", "Settings", "Messages"};
    for (const QString &item : menuItems) {
        QPushButton *button = new QPushButton(item);
        sidebarLayout->addWidget(button);
    }
    sidebarLayout->addStretch();

    QWidget *sidebar = new QWidget;
    sidebar->setLayout(sidebarLayout);
    sidebar->setFixedWidth(200);
    sidebar->setObjectName("sidebar");

    // --- Основная часть ---
    QVBoxLayout *mainLayout = new QVBoxLayout;
    QLabel *titleLabel = new QLabel("Obfuscation");
    titleLabel->setObjectName("titleLabel");

    // Ввод файла
    inputLabel = new QLabel("Select your file");
    browseButton = new QPushButton("+ Browse");
    browseButton->setObjectName("browseButton");
    connect(browseButton, &QPushButton::clicked, this, &MainWindow::browseFile);

    QVBoxLayout *inputLayout = new QVBoxLayout;
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(browseButton);
    inputLayout->setAlignment(Qt::AlignCenter);
    QWidget *inputWidget = new QWidget;
    inputWidget->setLayout(inputLayout);
    inputWidget->setObjectName("inputWidget");

    // Вывод
    outputLabel = new QLabel("Download");
    outputLabel->setAlignment(Qt::AlignCenter);
    QVBoxLayout *outputLayout = new QVBoxLayout;
    outputLayout->addWidget(outputLabel);
    QWidget *outputWidget = new QWidget;
    outputWidget->setLayout(outputLayout);
    outputWidget->setObjectName("outputWidget");

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(inputWidget);
    mainLayout->addWidget(outputWidget);

    // --- Правая панель (настройки) ---
    QVBoxLayout *settingsLayout = new QVBoxLayout;
    QLabel *instructionsLabel = new QLabel("We recommend doing encryption first before obfuscation for better anti-reversing");
    instructionsLabel->setWordWrap(true);
    settingsLayout->addWidget(instructionsLabel);

    QLabel *typeLabel = new QLabel("Type");
    hashTypeCombo = new QComboBox;
    hashTypeCombo->addItems({"SHA-256", "MD5", "AES"});
    settingsLayout->addWidget(typeLabel);
    settingsLayout->addWidget(hashTypeCombo);

    QLabel *maxStringsLabel = new QLabel("Max strings");
    maxStringsSlider = new QSlider(Qt::Horizontal);
    settingsLayout->addWidget(maxStringsLabel);
    settingsLayout->addWidget(maxStringsSlider);

    saveSettingsButton = new QPushButton("Save settings");
    saveSettingsButton->setObjectName("saveSettingsButton");
    settingsLayout->addWidget(saveSettingsButton);
    settingsLayout->addStretch();

    QWidget *settingsWidget = new QWidget;
    settingsWidget->setLayout(settingsLayout);
    settingsWidget->setFixedWidth(250);
    settingsWidget->setObjectName("settingsWidget");

    // --- Нижние кнопки ---
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    submitButton = new QPushButton("Submit");
    retryButton = new QPushButton("Try again");
    submitButton->setObjectName("submitButton");
    retryButton->setObjectName("retryButton");
    buttonsLayout->addWidget(submitButton);
    buttonsLayout->addWidget(retryButton);

    // --- Общий макет ---
    QHBoxLayout *mainContainer = new QHBoxLayout;
    mainContainer->addWidget(sidebar);
    mainContainer->addLayout(mainLayout, 1);
    mainContainer->addWidget(settingsWidget);

    QVBoxLayout *globalLayout = new QVBoxLayout;
    globalLayout->addLayout(mainContainer);
    globalLayout->addLayout(buttonsLayout);

    centralWidget->setLayout(globalLayout);
}

// --- Выбор файла ---
void MainWindow::browseFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select File");
    if (!fileName.isEmpty()) {
        inputLabel->setText(fileName);
    }
}

MainWindow::~MainWindow() {}
