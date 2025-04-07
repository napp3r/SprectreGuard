#include "settingswidget.h"
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>

SettingsWidget::SettingsWidget(QWidget *parent) : QWidget(parent)
{
    setupUI();
    setupConnections();
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

    // Profile section
    QHBoxLayout *profileLayout = new QHBoxLayout();
    
    // Profile photo (circle)
    profilePhotoLabel = new QLabel();
    profilePhotoLabel->setFixedSize(70, 70);
    profilePhotoLabel->setObjectName("profilePhoto");
    
    // Create a default circular avatar
    QPixmap avatar(":/icons/default_avatar.png"); // We'll update the resources later
    if (avatar.isNull()) {
        // If no avatar image, create a solid color circle
        avatar = QPixmap(70, 70);
        avatar.fill(Qt::lightGray);
    }
    
    // Make the avatar circular
    QPixmap circularAvatar(70, 70);
    circularAvatar.fill(Qt::transparent);
    QPainter painter(&circularAvatar);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QBrush(avatar));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(circularAvatar.rect());
    
    profilePhotoLabel->setPixmap(circularAvatar);
    profileLayout->addWidget(profilePhotoLabel);
    
    // Profile info
    QVBoxLayout *profileInfoLayout = new QVBoxLayout();
    
    nameLabel = new QLabel("Helena Hills");
    nameLabel->setObjectName("nameLabel");
    profileInfoLayout->addWidget(nameLabel);
    
    changePhotoButton = new QPushButton("Change profile photo");
    changePhotoButton->setObjectName("changePhotoButton");
    changePhotoButton->setCursor(Qt::PointingHandCursor);
    changePhotoButton->setFlat(true);
    profileInfoLayout->addWidget(changePhotoButton);
    
    profileLayout->addLayout(profileInfoLayout);
    profileLayout->addStretch();
    
    mainLayout->addLayout(profileLayout);
    mainLayout->addSpacing(20);

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

    // Save changes button
    mainLayout->addSpacing(30);
    saveChangesButton = new QPushButton("Save changes");
    saveChangesButton->setObjectName("saveChangesButton");
    saveChangesButton->setFixedWidth(120);
    mainLayout->addWidget(saveChangesButton, 0, Qt::AlignLeft);
    
    mainLayout->addStretch();
}

void SettingsWidget::setupConnections()
{
    connect(saveChangesButton, &QPushButton::clicked, this, &SettingsWidget::onSaveChanges);
    connect(changePhotoButton, &QPushButton::clicked, this, &SettingsWidget::onChangePhoto);
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
    
    // Here you would normally save these settings
    // For now, just show a message
    QMessageBox::information(this, "Settings Saved", 
                            "Settings saved successfully.\n"
                            "Obfuscation Level: " + obfuscationLevel + "\n"
                            "VM Detection: " + (vmDetection ? "On" : "Off"));
}

void SettingsWidget::onChangePhoto()
{
    QString fileName = QFileDialog::getOpenFileName(this, 
        "Select Profile Picture", "", "Image Files (*.png *.jpg *.jpeg *.bmp)");
    
    if (!fileName.isEmpty()) {
        QPixmap newPhoto(fileName);
        if (!newPhoto.isNull()) {
            // Resize and make circular
            newPhoto = newPhoto.scaled(70, 70, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            
            QPixmap circularPhoto(70, 70);
            circularPhoto.fill(Qt::transparent);
            QPainter painter(&circularPhoto);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setBrush(QBrush(newPhoto));
            painter.setPen(Qt::NoPen);
            painter.drawEllipse(circularPhoto.rect());
            
            profilePhotoLabel->setPixmap(circularPhoto);
        }
    }
} 