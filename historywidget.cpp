#include "historywidget.h"
#include <QLabel>
#include <QHeaderView>
#include <QPushButton>
#include <QHBoxLayout>
#include <QStyle>

HistoryWidget::HistoryWidget(QWidget *parent) : QWidget(parent) {
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

    // History table
    historyTable = new QTableWidget(this);
    historyTable->setObjectName("historyTable");
    historyTable->setRowCount(4);
    historyTable->setColumnCount(4);  // Added one more column for actions
    historyTable->setHorizontalHeaderLabels({"Title", "Level", "Date", ""});
    historyTable->verticalHeader()->setVisible(false);
    
    // Configure table properties
    historyTable->setShowGrid(false);
    historyTable->setAlternatingRowColors(true);
    historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Set column sizes
    historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);  // Title
    historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);    // Level
    historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);    // Date
    historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);    // Actions
    
    historyTable->setColumnWidth(1, 100);
    historyTable->setColumnWidth(2, 100);
    historyTable->setColumnWidth(3, 50);

    // Sample data
    QStringList titles = {"Program 1", "Some file", "Demo", "my project"};
    QStringList levels = {"High", "Low", "High", "Low"};
    QString date = "Dec 5";

    for(int row = 0; row < 4; ++row) {
        // Title
        historyTable->setItem(row, 0, new QTableWidgetItem(titles[row]));
        
        // Level
        QWidget* levelWidget = new QWidget();
        QHBoxLayout* levelLayout = new QHBoxLayout(levelWidget);
        QLabel* levelLabel = new QLabel(levels[row]);
        
        levelLabel->setProperty("class", QString("level-tag level-%1").arg(levels[row].toLower()));
        levelLayout->addWidget(levelLabel);
        levelLayout->addStretch();
        levelLayout->setContentsMargins(0, 2, 0, 2);
        
        historyTable->setCellWidget(row, 1, levelWidget);
        
        // Date
        historyTable->setItem(row, 2, new QTableWidgetItem(date));
        
        // Actions
        QWidget* actionsWidget = new QWidget();
        QHBoxLayout* actionsLayout = new QHBoxLayout(actionsWidget);
        QPushButton* moreButton = new QPushButton("...");
        moreButton->setFixedSize(24, 24);
        actionsLayout->addWidget(moreButton);
        actionsLayout->setAlignment(Qt::AlignCenter);
        actionsLayout->setContentsMargins(0, 0, 0, 0);
        
        historyTable->setCellWidget(row, 3, actionsWidget);
    }

    // Add widgets to main layout
    mainLayout->addWidget(titleLabel);
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(historyTable);

    setLayout(mainLayout);
}
