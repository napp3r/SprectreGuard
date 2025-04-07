#ifndef HISTORYWIDGET_H
#define HISTORYWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableWidget>

class HistoryWidget : public QWidget {
    Q_OBJECT

public:
    explicit HistoryWidget(QWidget *parent = nullptr);

private:
    QLineEdit *searchBar;
    QTableWidget *historyTable;
};

#endif // HISTORYWIDGET_H
