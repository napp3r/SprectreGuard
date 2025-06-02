#include "sidebar.h"
#include <QStyle>

Sidebar::Sidebar(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
}

void Sidebar::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create buttons with emoji icons
    homeButton = new QPushButton("🏠 Home", this);
    sourceProtectionButton = new QPushButton("🔒 Source Protection", this);
    exeProtectionButton = new QPushButton("🛡️ EXE Protection", this);
    settingsButton = new QPushButton("⚙️ Settings", this);

    styleButton(homeButton);
    styleButton(sourceProtectionButton);
    styleButton(exeProtectionButton);
    styleButton(settingsButton);

    mainLayout->addWidget(homeButton);
    mainLayout->addWidget(sourceProtectionButton);
    mainLayout->addWidget(exeProtectionButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addStretch();

    setFixedWidth(200);

    // Set sidebar style
    setStyleSheet(
        "QPushButton { "
        "   text-align: left;"
        "   padding: 15px;"
        "   border: none;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #3D3D3D; }"
        "QPushButton:checked { background-color: #4D4D4D; }"
    );
}

void Sidebar::styleButton(QPushButton *button) {
    button->setMinimumHeight(50);
    button->setCursor(Qt::PointingHandCursor);
    button->setCheckable(true);
}

void Sidebar::setupConnections() {
    connect(homeButton, &QPushButton::clicked, [this]() {
        emit pageSelected(0);
        homeButton->setChecked(true);
        sourceProtectionButton->setChecked(false);
        exeProtectionButton->setChecked(false);
        settingsButton->setChecked(false);
    });

    connect(sourceProtectionButton, &QPushButton::clicked, [this]() {
        emit pageSelected(1);
        homeButton->setChecked(false);
        sourceProtectionButton->setChecked(true);
        exeProtectionButton->setChecked(false);
        settingsButton->setChecked(false);
    });

    connect(exeProtectionButton, &QPushButton::clicked, [this]() {
        emit pageSelected(2);
        homeButton->setChecked(false);
        sourceProtectionButton->setChecked(false);
        exeProtectionButton->setChecked(true);
        settingsButton->setChecked(false);
    });

    connect(settingsButton, &QPushButton::clicked, [this]() {
        emit pageSelected(3);
        homeButton->setChecked(false);
        sourceProtectionButton->setChecked(false);
        exeProtectionButton->setChecked(false);
        settingsButton->setChecked(true);
    });

    // Set home as default
    homeButton->setChecked(true);
} 