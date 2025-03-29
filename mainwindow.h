#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QComboBox>
#include <QFileDialog>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browseFile();

private:
    QLabel *inputLabel, *outputLabel;
    QPushButton *browseButton, *submitButton, *retryButton, *saveSettingsButton;
    QComboBox *hashTypeCombo;
    QSlider *maxStringsSlider;
};

#endif // MAINWINDOW_H
