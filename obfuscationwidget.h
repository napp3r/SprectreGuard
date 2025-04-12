#ifndef OBFUSCATIONWIDGET_H
#define OBFUSCATIONWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>

class ObfuscationWidget : public QWidget {
    Q_OBJECT

public:
    explicit ObfuscationWidget(QWidget *parent = nullptr);

public slots:
    void openFile(const QString &filePath);

signals:
    void fileProcessed(); // Signal emitted when a file is successfully processed

private slots:
    void browseFile();
    void processFile();
    void tryAgain();
    void saveSettings();

private:
    QLabel *selectLabel; // Для отображения имени выбранного файла
    QComboBox *typeCombo;
    QSlider *maxStringsSlider;
    QString currentFilePath;
    
    void setupUI();
    void setupConnections();
    QString getObfuscationLevel() const;
    bool obfuscateFile(const QString &inputPath, const QString &outputPath);
};

#endif // OBFUSCATIONWIDGET_H
