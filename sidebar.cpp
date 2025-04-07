#include "sidebar.h"
#include <QIcon>
#include <QStyle>
#include <QLabel>

SidebarButton::SidebarButton(const QString &text, const QString &iconPath, QWidget *parent)
    : QPushButton(text, parent)
{
    setObjectName("sidebarButton");
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);
    
    QIcon icon;
    if (iconPath.isEmpty()) {
        // Use minimal system icons
        if (text == "Home")
            icon = style()->standardIcon(QStyle::SP_CommandLink);  // Simple arrow icon
        else if (text == "Obfuscation")
            icon = style()->standardIcon(QStyle::SP_FileIcon);
        else if (text == "Encryption")
            icon = style()->standardIcon(QStyle::SP_BrowserStop);  // Lock-like icon
        else if (text == "Settings")
            icon = style()->standardIcon(QStyle::SP_CommandLink);
        else if (text == "Messages")
            icon = style()->standardIcon(QStyle::SP_MessageBoxQuestion);
    } else {
        icon = QIcon(iconPath);
    }
    
    setIcon(icon);
    setIconSize(QSize(16, 16));  // Smaller icon size for minimalistic look
}

void SidebarButton::setActive(bool active)
{
    setChecked(active);
    setProperty("active", active);
    style()->unpolish(this);
    style()->polish(this);
}

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("sidebar");
    setFixedWidth(200);
    setupUI();
}

void Sidebar::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add SpectreGuard title
    QLabel *titleLabel = new QLabel("SpectreGuard");
    titleLabel->setObjectName("sidebarTitle");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    titleLabel->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(titleLabel);

    // Create buttons
    homeButton = new SidebarButton("Home");
    homeButton->setObjectName("homeButton");
    
    obfuscationButton = new SidebarButton("Obfuscation");
    obfuscationButton->setObjectName("obfuscationButton");
    
    encryptionButton = new SidebarButton("Encryption");
    encryptionButton->setObjectName("encryptionButton");
    
    settingsButton = new SidebarButton("Settings");
    settingsButton->setObjectName("settingsButton");
    
    messagesButton = new SidebarButton("Messages");
    messagesButton->setObjectName("messagesButton");

    // Add buttons to layout
    mainLayout->addWidget(homeButton);
    mainLayout->addWidget(obfuscationButton);
    mainLayout->addWidget(encryptionButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addWidget(messagesButton);
    mainLayout->addStretch();

    // Connect signals
    connect(homeButton, &QPushButton::clicked, this, &Sidebar::handleButtonClick);
    connect(obfuscationButton, &QPushButton::clicked, this, &Sidebar::handleButtonClick);
    connect(encryptionButton, &QPushButton::clicked, this, &Sidebar::handleButtonClick);
    connect(settingsButton, &QPushButton::clicked, this, &Sidebar::handleButtonClick);
    connect(messagesButton, &QPushButton::clicked, this, &Sidebar::handleButtonClick);

    // Set initial active button
    setActiveButton(homeButton);
}

void Sidebar::setActiveButton(SidebarButton *button)
{
    homeButton->setActive(button == homeButton);
    obfuscationButton->setActive(button == obfuscationButton);
    encryptionButton->setActive(button == encryptionButton);
    settingsButton->setActive(button == settingsButton);
    messagesButton->setActive(button == messagesButton);
}

void Sidebar::handleButtonClick()
{
    SidebarButton *button = qobject_cast<SidebarButton*>(sender());
    if (button) {
        setActiveButton(button);
    }
}
