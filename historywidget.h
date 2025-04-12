#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableWidget>
#include "filehistory.h"

class HistoryWidget : public QWidget {
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget *parent = nullptr);
    void refreshTable();

signals:
    void openFile(const QString &filePath, const QString &processType);

private slots:
    void onTableCellClicked(int row, int column);
    void onSearchTextChanged(const QString &text);
    void openActionMenu(QPoint pos);
    void clearHistory();

private:
    QLineEdit *searchBar;
    QTableWidget *historyTable;
    QList<HistoryEntry> currentEntries;
    
    void setupUI();
    void setupConnections();
    void loadHistory();
    void filterEntries(const QString &text);
};

#endif // HISTORYWIDGET_H
