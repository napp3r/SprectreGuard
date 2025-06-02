#include "homewidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

HomeWidget::HomeWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
}

void HomeWidget::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    createWelcomeSection();
    createDescriptionSection();
    createFeaturesSection();
    createGettingStartedSection();
}

void HomeWidget::createWelcomeSection() {
    QLabel *titleLabel = new QLabel("Welcome to SpectreGuard", this);
    titleLabel->setObjectName("welcomeTitle");
    titleLabel->setAlignment(Qt::AlignCenter);

    QLabel *subtitleLabel = new QLabel(
        "Advanced Software Protection System",
        this
    );
    subtitleLabel->setObjectName("welcomeSubtitle");
    subtitleLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
}

void HomeWidget::createDescriptionSection() {
    QFrame *descriptionFrame = new QFrame(this);
    descriptionFrame->setObjectName("descriptionFrame");
    QVBoxLayout *descriptionLayout = new QVBoxLayout(descriptionFrame);
    
    QLabel *descriptionTitle = new QLabel("About SpectreGuard", this);
    descriptionTitle->setObjectName("sectionTitle");
    
    QLabel *descriptionText = new QLabel(
        "SpectreGuard is a comprehensive software protection solution designed to safeguard your applications "
        "from reverse engineering, tampering, and unauthorized access. Our multi-layered protection system "
        "combines advanced obfuscation techniques with robust encryption to ensure your intellectual property "
        "remains secure.",
        this
    );
    descriptionText->setObjectName("descriptionText");
    descriptionText->setWordWrap(true);
    
    descriptionLayout->addWidget(descriptionTitle);
    descriptionLayout->addWidget(descriptionText);
    
    mainLayout->addWidget(descriptionFrame);
}

void HomeWidget::createFeaturesSection() {
    QFrame *featuresFrame = new QFrame(this);
    featuresFrame->setObjectName("featuresFrame");
    QVBoxLayout *featuresLayout = new QVBoxLayout(featuresFrame);
    
    QLabel *featuresTitle = new QLabel("Key Features", this);
    featuresTitle->setObjectName("sectionTitle");
    
    QStringList features = {
        "• Source Level Protection - Advanced code obfuscation and anti-debugging techniques to protect your source code",
        "• EXE Level Protection - Binary hardening and anti-tampering measures for compiled executables",
        "• String Encryption - Runtime protection of sensitive data and strings",
        "• Resource Protection - Secure your application resources from unauthorized access"
    };

    for (const QString &feature : features) {
        QLabel *featureLabel = new QLabel(feature, this);
        featureLabel->setObjectName("featureItem");
        featuresLayout->addWidget(featureLabel);
    }

    mainLayout->addWidget(featuresTitle);
    mainLayout->addWidget(featuresFrame);
}

void HomeWidget::createGettingStartedSection() {
    QFrame *gettingStartedFrame = new QFrame(this);
    gettingStartedFrame->setObjectName("gettingStartedFrame");
    QVBoxLayout *gettingStartedLayout = new QVBoxLayout(gettingStartedFrame);
    
    QLabel *gettingStartedTitle = new QLabel("Getting Started", this);
    gettingStartedTitle->setObjectName("sectionTitle");
    
    QLabel *gettingStartedText = new QLabel(
        "To begin protecting your software:\n\n"
        "1. Select the appropriate protection method from the sidebar\n"
        "2. Configure your protection settings\n"
        "3. Add your files for processing\n"
        "4. Apply protection and save your secured files",
        this
    );
    gettingStartedText->setObjectName("gettingStartedText");
    gettingStartedText->setWordWrap(true);
    
    gettingStartedLayout->addWidget(gettingStartedTitle);
    gettingStartedLayout->addWidget(gettingStartedText);
    
    mainLayout->addWidget(gettingStartedFrame);
    mainLayout->addStretch();
} 