#include "mainwindow.h"
#include "sidebar.h"
#include "pages/homewidget.h"
#include "pages/sourceprotectionwidget.h"
#include "pages/exeprotectionwidget.h"
#include "pages/settingswidget.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    setupConnections();
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    sidebar = new Sidebar(this);
    stackedWidget = new QStackedWidget(this);

    homePage = new HomeWidget(this);
    sourceProtectionPage = new SourceProtectionWidget(this);
    exeProtectionPage = new ExeProtectionWidget(this);
    settingsPage = new SettingsWidget(this);

    stackedWidget->addWidget(homePage);
    stackedWidget->addWidget(sourceProtectionPage);
    stackedWidget->addWidget(exeProtectionPage);
    stackedWidget->addWidget(settingsPage);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stackedWidget);

    setCentralWidget(centralWidget);
    setWindowTitle("SpectreGuard");
    resize(1200, 800);
}

void MainWindow::setupConnections() {
    connect(sidebar, &Sidebar::pageSelected, this, &MainWindow::switchPage);
}

void MainWindow::switchPage(int index) {
    stackedWidget->setCurrentIndex(index);
}

MainWindow::~MainWindow() {} 