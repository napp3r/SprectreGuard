#ifndef OBFUSCATIONWIDGET_H
#define OBFUSCATIONWIDGET_H

#include <QWidget>
#include <QLabel>

class ObfuscationWidget : public QWidget {
    Q_OBJECT

public:
    explicit ObfuscationWidget(QWidget *parent = nullptr);

private slots:
    void browseFile();

private:
    QLabel *selectLabel; // Для отображения имени выбранного файла
};

#endif // OBFUSCATIONWIDGET_H
