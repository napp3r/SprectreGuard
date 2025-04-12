#include "historywidget.h"
#include <QLabel>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>
#include <QFileInfo>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QDir>
#include <QProcess>

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
    setupConnections();
    loadHistory();
}

void HistoryWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(0);

    // Title
    QLabel *titleLabel = new QLabel("History");
    titleLabel->setObjectName("titleLabel");

    // Search bar with icon
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->setContentsMargins(0, 20, 0, 20);
    
    searchBar = new QLineEdit();
    searchBar->setObjectName("searchBar");
    searchBar->setPlaceholderText("Search");
    searchBar->setFixedWidth(300);
    
    // Use Qt's standard search icon
    searchBar->addAction(style()->standardIcon(QStyle::SP_FileDialogContentsView), QLineEdit::LeadingPosition);
    
    searchLayout->addWidget(searchBar);
    searchLayout->addStretch();
    
    // Clear history button
    QPushButton *clearButton = new QPushButton("Clear History");
    clearButton->setObjectName("clearButton");
    connect(clearButton, &QPushButton::clicked, this, &HistoryWidget::clearHistory);
    searchLayout->addWidget(clearButton);

    // History table
    historyTable = new QTableWidget(this);
    historyTable->setObjectName("historyTable");
    historyTable->setColumnCount(4);
    historyTable->setHorizontalHeaderLabels({"Title", "Type", "Date", ""});
    historyTable->verticalHeader()->setVisible(false);
    
    // Configure table properties
    historyTable->setShowGrid(false);
    historyTable->setAlternatingRowColors(true);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    historyTable->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // Set column sizes
    historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);  // Title
    historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);    // Type
    historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);    // Date
    historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);    // Actions
    
    historyTable->setColumnWidth(1, 120);
    historyTable->setColumnWidth(2, 120);
    historyTable->setColumnWidth(3, 50);

    // Add widgets to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(historyTable);

    setLayout(mainLayout);
}

void HistoryWidget::setupConnections() {
    connect(searchBar, &QLineEdit::textChanged, this, &HistoryWidget::onSearchTextChanged);
    connect(historyTable, &QTableWidget::cellClicked, this, &HistoryWidget::onTableCellClicked);
    connect(historyTable, &QTableWidget::customContextMenuRequested, this, &HistoryWidget::openActionMenu);
}

void HistoryWidget::loadHistory() {
    currentEntries = FileHistory::getEntries();
    refreshTable();
}

void HistoryWidget::refreshTable() {
    historyTable->setRowCount(0);
    
    for (const HistoryEntry &entry : currentEntries) {
        int row = historyTable->rowCount();
        historyTable->insertRow(row);
        
        // Title/Filename
        historyTable->setItem(row, 0, new QTableWidgetItem(entry.fileName));
        
        // Process Type + Level
        QWidget* typeWidget = new QWidget();
        QHBoxLayout* typeLayout = new QHBoxLayout(typeWidget);
        QString typeText = QString("%1 (%2)").arg(entry.processType).arg(entry.level);
        QLabel* typeLabel = new QLabel(typeText);
        
        typeLabel->setProperty("class", QString("level-tag level-%1").arg(entry.level.toLower()));
        typeLayout->addWidget(typeLabel);
        typeLayout->addStretch();
        typeLayout->setContentsMargins(0, 2, 0, 2);
        
        historyTable->setCellWidget(row, 1, typeWidget);
        
        // Date
        QString dateStr = entry.timestamp.date() == QDate::currentDate() 
            ? "Today " + entry.timestamp.toString("hh:mm")
            : entry.timestamp.toString("MMM d");
        historyTable->setItem(row, 2, new QTableWidgetItem(dateStr));
        
        // Actions
        QWidget* actionsWidget = new QWidget();
        QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
        QPushButton* moreButton = new QPushButton("...");
        moreButton->setFixedSize(24, 24);
        moreButton->setProperty("row", row);
        connect(moreButton, &QPushButton::clicked, this, [this, row]() {
            QPoint pos = historyTable->cellWidget(row, 3)->mapToGlobal(QPoint(0, 24));
            openActionMenu(pos);
        });
        
        actionsLayout->addWidget(moreButton);
        actionsLayout->setAlignment(Qt::AlignCenter);
        actionsLayout->setContentsMargins(0, 0, 0, 0);
        
        historyTable->setCellWidget(row, 3, actionsWidget);
    }
}

void HistoryWidget::onTableCellClicked(int row, int column) {
    if (column != 3 && row < currentEntries.size()) { // Skip action column
        emit openFile(currentEntries[row].filePath, currentEntries[row].processType);
    }
}

void HistoryWidget::onSearchTextChanged(const QString &text) {
    filterEntries(text);
}

void HistoryWidget::filterEntries(const QString &text) {
    if (text.isEmpty()) {
        // Show all entries
        currentEntries = FileHistory::getEntries();
    } else {
        // Filter entries
        QList<HistoryEntry> allEntries = FileHistory::getEntries();
        currentEntries.clear();
        
        for (const HistoryEntry &entry : allEntries) {
            if (entry.fileName.contains(text, Qt::CaseInsensitive) ||
                entry.processType.contains(text, Qt::CaseInsensitive)) {
                currentEntries.append(entry);
            }
        }
    }
    
    refreshTable();
}

void HistoryWidget::openActionMenu(QPoint pos) {
    QModelIndex index = historyTable->indexAt(historyTable->viewport()->mapFromGlobal(pos));
    if (index.isValid() && index.row() < currentEntries.size()) {
        QMenu menu(this);
        HistoryEntry entry = currentEntries[index.row()];
        
        menu.addAction("Open File", [this, entry]() {
            emit openFile(entry.filePath, entry.processType);
        });
        
        menu.addAction("Copy Path", [entry]() {
            QApplication::clipboard()->setText(entry.filePath);
        });
        
        menu.addAction("Show in Folder", [entry]() {
            QFileInfo fileInfo(entry.filePath);
            QString command = QString("explorer.exe /select,\"%1\"").arg(
                QDir::toNativeSeparators(entry.filePath));
            QProcess::startDetached(command);
        });
        
        menu.exec(pos);
    }
}

void HistoryWidget::clearHistory() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, 
        "Clear History", "Are you sure you want to clear all history?",
        QMessageBox::Yes | QMessageBox::No);
        
    if (reply == QMessageBox::Yes) {
        FileHistory::clear();
        loadHistory();
    }
}
