#pragma once

#include "core/UserRole.h"

#include <QMainWindow>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(UserRole role, QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    QWidget* createRoleWidget(UserRole role);

    Ui::MainWindow* ui = nullptr;
};
