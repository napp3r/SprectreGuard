#include "mainwindow.h"
#include "historywidget.h"
#include "obfuscationwidget.h"
#include "encryptionwidget.h"
#include "sidebar.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
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
    HistoryWidget *historyPage = new HistoryWidget;
    stackedWidget->addWidget(historyPage);

    // Obfuscation page
    ObfuscationWidget *obfuscationPage = new ObfuscationWidget;
    stackedWidget->addWidget(obfuscationPage);

    // Encryption page
    EncryptionWidget *encryptionPage = new EncryptionWidget;
    stackedWidget->addWidget(encryptionPage);

    // Placeholder for Settings page
    QWidget *settingsPage = new QWidget;
    QLabel *settingsLabel = new QLabel("Settings Page (Placeholder)");
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsPage);
    settingsLayout->addWidget(settingsLabel);
    stackedWidget->addWidget(settingsPage);

    // Placeholder for Messages page
    QWidget *messagesPage = new QWidget;
    QLabel *messagesLabel = new QLabel("Messages Page (Placeholder)");
    QVBoxLayout *messagesLayout = new QVBoxLayout(messagesPage);
    messagesLayout->addWidget(messagesLabel);
    stackedWidget->addWidget(messagesPage);

    stackedWidget->setCurrentIndex(0);

    // Connect sidebar buttons to page switching
    QList<SidebarButton*> buttons;
    buttons.append(sidebar->findChild<SidebarButton*>("homeButton"));
    buttons.append(sidebar->findChild<SidebarButton*>("obfuscationButton"));
    buttons.append(sidebar->findChild<SidebarButton*>("encryptionButton"));
    buttons.append(sidebar->findChild<SidebarButton*>("settingsButton"));
    buttons.append(sidebar->findChild<SidebarButton*>("messagesButton"));
    
    for (int i = 0; i < buttons.size(); i++) {
        if (buttons[i]) {
            connect(buttons[i], &QPushButton::clicked, this, [this, i]() {
                switchPage(i);
            });
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

void MainWindow::switchPage(int index) {
    stackedWidget->setCurrentIndex(index);
}

MainWindow::~MainWindow() {}
