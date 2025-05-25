#pragma once

#include <QWidget>

class QVBoxLayout;
class QLabel;

class HomeWidget : public QWidget {
    Q_OBJECT

public:
    explicit HomeWidget(QWidget *parent = nullptr);

private:
    void setupUI();
    void createWelcomeSection();
    void createDescriptionSection();
    void createFeaturesSection();
    void createGettingStartedSection();

    QVBoxLayout *mainLayout;
}; 