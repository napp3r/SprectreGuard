#include "sidebar.h"
#include <QIcon>
#include <QStyle>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QFontDatabase>

SidebarButton::SidebarButton(const QString &text, const QString &iconPath, QWidget *parent)
    : QPushButton(text, parent)
{
    setObjectName("sidebarButton");
    setCursor(Qt::PointingHandCursor);
    setCheckable(true);
    
    // Set fixed height for consistent Material Design look
    setFixedHeight(48);
    
    // Use consistent icons with Material style
    QIcon icon;
    if (iconPath.isEmpty()) {
        // Material-like icons
        if (text == "Home")
            icon = QIcon(":/icons/home.svg");  // Using SVG for better quality
        else if (text == "Obfuscation")
            icon = QIcon(":/icons/code.svg");
        else if (text == "Encryption")
            icon = QIcon(":/icons/lock.svg");
        else if (text == "Settings")
            icon = QIcon(":/icons/settings.svg");
        else if (text == "Messages")
            icon = QIcon(":/icons/message.svg");
            
        // Fallback to system icons if resources are missing
        if (icon.isNull()) {
            if (text == "Home")
                icon = style()->standardIcon(QStyle::SP_DirHomeIcon);
            else if (text == "Obfuscation")
                icon = style()->standardIcon(QStyle::SP_FileIcon);
            else if (text == "Encryption")
                icon = style()->standardIcon(QStyle::SP_DriveFDIcon);
            else if (text == "Settings")
                icon = style()->standardIcon(QStyle::SP_FileDialogDetailedView);
            else if (text == "Messages")
                icon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
        }
    } else {
        icon = QIcon(iconPath);
    }
    
    setIcon(icon);
    setIconSize(QSize(24, 24));  // Material Design icon size
    
    // Set text alignment with proper padding for Material look
    setStyleSheet("text-align: left; padding-left: 16px;");
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
    setFixedWidth(240);  // Material Design navigation drawer width
    setupUI();
}

void Sidebar::setupUI()
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // App header/brand area with elevation
    QWidget *headerWidget = new QWidget();
    headerWidget->setObjectName("sidebarHeader");
    headerWidget->setMinimumHeight(64);  // Material spec for app bar height
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(16, 0, 16, 0);
    
    // App logo (can be replaced with your actual logo)
    QLabel *logoLabel = new QLabel();
    logoLabel->setFixedSize(32, 32);
    QPixmap logoPix(32, 32);
    logoPix.fill(Qt::transparent);
    QPainter painter(&logoPix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor("#6200EE"));  // Material Design primary color
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0, 0, 32, 32, 8, 8);
    logoLabel->setPixmap(logoPix);
    
    // Title with Material Design typography
    QLabel *titleLabel = new QLabel("SpectreGuard");
    titleLabel->setObjectName("sidebarTitle");
    
    headerLayout->addWidget(logoLabel);
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    
    mainLayout->addWidget(headerWidget);
    
    // Divider
    QFrame *divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setFrameShadow(QFrame::Plain);
    divider->setObjectName("sidebarDivider");
    mainLayout->addWidget(divider);
    
    // Add some spacing
    mainLayout->addSpacing(8);

    // Create navigation buttons
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

    // Add buttons to layout with proper spacing
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
