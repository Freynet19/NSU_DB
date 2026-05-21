#pragma once

#include <QWidget>

class QueryRepository;

class HrWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HrWidget(QWidget *parent = nullptr);

private:
    QueryRepository *m_repo = nullptr;
};
