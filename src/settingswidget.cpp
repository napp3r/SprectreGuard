void SettingsWidget::setupUi()
{
    // ... existing code ...
    
    // UPX Settings Group
    QGroupBox* upxGroup = new QGroupBox(tr("UPX Settings"), this);
    QVBoxLayout* upxLayout = new QVBoxLayout(upxGroup);
    
    QHBoxLayout* upxPathLayout = new QHBoxLayout();
    m_upxPathEdit = new QLineEdit(this);
    QPushButton* browseUpxButton = new QPushButton(tr("Browse..."), this);
    upxPathLayout->addWidget(new QLabel(tr("UPX Path:"), this));
    upxPathLayout->addWidget(m_upxPathEdit);
    upxPathLayout->addWidget(browseUpxButton);
    
    m_useUpxCheck = new QCheckBox(tr("Enable UPX Compression"), this);
    
    upxLayout->addLayout(upxPathLayout);
    upxLayout->addWidget(m_useUpxCheck);
    
    connect(browseUpxButton, &QPushButton::clicked, this, &SettingsWidget::browseUpxPath);
    
    mainLayout->addWidget(upxGroup);
    
    // ... existing code ...
}

void SettingsWidget::browseUpxPath()
{
    QString path = QFileDialog::getOpenFileName(this,
        tr("Select UPX Executable"),
        m_upxPathEdit->text(),
        tr("Executable Files (*.exe);;All Files (*.*)"));
        
    if (!path.isEmpty()) {
        m_upxPathEdit->setText(path);
    }
}

void SettingsWidget::saveSettings()
{
    // ... existing code ...
    settings.setValue("upx/path", m_upxPathEdit->text());
    settings.setValue("upx/enabled", m_useUpxCheck->isChecked());
    // ... existing code ...
}

void SettingsWidget::loadSettings()
{
    QSettings settings;
    // Set default UPX path if not set or file does not exist
    QString defaultUpxPath = QDir::current().absoluteFilePath("tools/upx/upx.exe");
    QString upxPath = settings.value("upx/path", defaultUpxPath).toString();
    if (upxPath.isEmpty() || !QFileInfo::exists(upxPath)) {
        upxPath = defaultUpxPath;
        settings.setValue("upx/path", upxPath);
    }
    m_upxPathEdit->setText(upxPath);
    m_useUpxCheck->setChecked(settings.value("upx/enabled", true).toBool());
    junkAmountSpin->setValue(settings.value("protection/junk_amount", 5000).toInt());
    // ... existing code ...
}

// ... existing code ... 

void SettingsWidget::createProtectionGroup() {
    QGroupBox* protectionGroup = new QGroupBox("Protection Settings", this);
    QVBoxLayout* protectionLayout = new QVBoxLayout(protectionGroup);

    // UPX settings
    QHBoxLayout* upxLayout = new QHBoxLayout();
    QLabel* upxLabel = new QLabel("UPX Path:", this);
    upxPathEdit = new QLineEdit(this);
    QPushButton* upxBrowseButton = new QPushButton("Browse", this);
    connect(upxBrowseButton, &QPushButton::clicked, this, &SettingsWidget::browseUpxPath);
    upxLayout->addWidget(upxLabel);
    upxLayout->addWidget(upxPathEdit);
    upxLayout->addWidget(upxBrowseButton);
    protectionLayout->addLayout(upxLayout);

    // UPX enable checkbox
    m_useUpxCheck = new QCheckBox("Enable UPX Compression", this);
    protectionLayout->addWidget(m_useUpxCheck);

    // Junk code settings
    QHBoxLayout* junkLayout = new QHBoxLayout();
    QLabel* junkLabel = new QLabel("Junk Code Amount:", this);
    junkAmountSpin = new QSpinBox(this);
    junkAmountSpin->setRange(1000, 10000);
    junkAmountSpin->setValue(5000);
    junkAmountSpin->setSingleStep(1000);
    junkLayout->addWidget(junkLabel);
    junkLayout->addWidget(junkAmountSpin);
    protectionLayout->addLayout(junkLayout);

    mainLayout->addWidget(protectionGroup);
}

void SettingsWidget::browseUpxPath() {
    QString path = QFileDialog::getOpenFileName(this,
        "Select UPX Executable",
        upxPathEdit->text(),
        "Executable Files (*.exe);;All Files (*)");
        
    if (!path.isEmpty()) {
        upxPathEdit->setText(path);
    }
}

void SettingsWidget::saveSettings() {
    QSettings settings;
    settings.setValue("upx/path", upxPathEdit->text());
    settings.setValue("upx/enabled", m_useUpxCheck->isChecked());
    settings.setValue("protection/junk_amount", junkAmountSpin->value());
    // ... existing code ...
}

void SettingsWidget::loadSettings() {
    QSettings settings;
    upxPathEdit->setText(settings.value("upx/path").toString());
    m_useUpxCheck->setChecked(settings.value("upx/enabled", false).toBool());
    junkAmountSpin->setValue(settings.value("protection/junk_amount", 5000).toInt());
    // ... existing code ...
}

// ... existing code ... 