#pragma once

#include "core/UserRole.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(UserRole role, QWidget *parent = nullptr);

private:
    QWidget *createRoleWidget(UserRole role);
};
