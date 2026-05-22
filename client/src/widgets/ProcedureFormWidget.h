#pragma once

#include <QWidget>

class QFormLayout;

class ProcedureFormWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProcedureFormWidget(QWidget* parent = nullptr);

    QFormLayout* formLayout() const;
    void clearForm();

private:
    QFormLayout* m_form = nullptr;
};
