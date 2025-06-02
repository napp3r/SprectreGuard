#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "encryptionwidget.h"
#include "historywidget.h"
#include "obfuscationwidget.h"
#include "settingswidget.h"
#include "sidebar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // Load stylesheet
    QFile styleFile(":/styles.qss");
    styleFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(styleFile.readAll());
    qApp->setStyleSheet(styleSheet);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create custom sidebar
    sidebar = new Sidebar(this);

    // Main content (QStackedWidget)
    stackedWidget = new QStackedWidget;

    // Home page (History)
    historyPage = new HistoryWidget;
    stackedWidget->addWidget(historyPage);
    connect(historyPage, &HistoryWidget::openFile, this, &MainWindow::openFileFromHistory);

    // Ensure history is loaded at startup
    QTimer::singleShot(100, historyPage, &HistoryWidget::refreshTable);

    // Obfuscation page
    obfuscationPage = new ObfuscationWidget;
    stackedWidget->addWidget(obfuscationPage);
    connect(obfuscationPage, &ObfuscationWidget::fileProcessed, [this]() {
        historyPage->refreshTable();
    });

    // Encryption page
    encryptionPage = new EncryptionWidget;
    stackedWidget->addWidget(encryptionPage);

    // Settings page
    SettingsWidget *settingsPage = new SettingsWidget;
    stackedWidget->addWidget(settingsPage);

    // Placeholder for Messages page
    QWidget *messagesPage = new QWidget;
    QLabel *messagesLabel = new QLabel("Messages Page (Placeholder)");
    QVBoxLayout *messagesLayout = new QVBoxLayout(messagesPage);
    messagesLayout->addWidget(messagesLabel);
    stackedWidget->addWidget(messagesPage);

    stackedWidget->setCurrentIndex(0);

    // Connect sidebar buttons to page switching
    QList<SidebarButton *> buttons;
    buttons.append(sidebar->findChild<SidebarButton *>("homeButton"));
    buttons.append(sidebar->findChild<SidebarButton *>("obfuscationButton"));
    buttons.append(sidebar->findChild<SidebarButton *>("encryptionButton"));
    buttons.append(sidebar->findChild<SidebarButton *>("settingsButton"));
    buttons.append(sidebar->findChild<SidebarButton *>("messagesButton"));

    for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]) {
            connect(buttons[i], &QPushButton::clicked, this, [this, i]() { switchPage(i); });
        }
    }

    // Main layout
    QHBoxLayout *mainContainer = new QHBoxLayout;
    mainContainer->setContentsMargins(0, 0, 0, 0);
    mainContainer->setSpacing(0);
    mainContainer->addWidget(sidebar);
    mainContainer->addWidget(stackedWidget, 1);

    centralWidget->setLayout(mainContainer);

    // Set window properties
    setWindowTitle("SpectreGuard");
    resize(1000, 700);
}

void MainWindow::switchPage(int index)
{
    stackedWidget->setCurrentIndex(index);

    // Refresh history when switching to the home page
    if (index == 0 && historyPage) {
        historyPage->refreshTable();
    }
}

void MainWindow::openFileFromHistory(const QString &filePath, const QString &processType)
{
    if (processType == "Obfuscation") {
        switchPage(1); // Switch to obfuscation page
        // Now we need to tell the obfuscation page to open this file
        if (obfuscationPage) {
            QMetaObject::invokeMethod(obfuscationPage, "openFile", Q_ARG(QString, filePath));
        }
    } else if (processType == "Encryption") {
        switchPage(2); // Switch to encryption page
        // Tell the encryption page to open this file (if we've implemented that)
        // This would be similar to the obfuscation page code
    }
}

MainWindow::~MainWindow() {}
