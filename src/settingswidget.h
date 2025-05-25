class SettingsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    
private slots:
    void onSaveSettings();
    void onBrowseThemida();
    void onBrowseVMProtect();
    void onBrowseOutputDir();
    void onBrowseSourceDir();
    void onBrowseExeDir();
    void onBrowseUPX();
    void browseUpxPath();
    
private:
    void setupUI();
    void createPathsGroup();
    void createDirectoriesGroup();
    void createProtectionGroup();
    void saveSettings();
    void loadSettings();

    QVBoxLayout *mainLayout;
    QLineEdit *themidaPathEdit;
    QLineEdit *vmprotectPathEdit;
    QLineEdit *outputDirEdit;
    QLineEdit *sourceDirEdit;
    QLineEdit *exeDirEdit;
    QLineEdit *upxPathEdit;
    QSpinBox *junkAmountSpin;
    QPushButton *sourceBrowseButton;
    QPushButton *exeBrowseButton;
    QPushButton *saveButton;
    QCheckBox *m_useUpxCheck;
}; 