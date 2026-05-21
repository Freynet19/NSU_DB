#include "shell/MainWindow.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "roles/AdminWidget.h"
#include "roles/HrWidget.h"
#include "roles/ProductionReportWidget.h"

#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(UserRole role, QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("ИС автомобилестроительного предприятия — %1")
                       .arg(userRoleDisplayName(role)));
    resize(1200, 800);

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    auto *header = new QLabel(
        tr("Пользователь: %1").arg(DatabaseManager::instance().username()), central);
    layout->addWidget(header);
    layout->addWidget(createRoleWidget(role), 1);

    setCentralWidget(central);
    statusBar()->showMessage(userRoleDisplayName(role));

    auto *fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(tr("Выход"), this, &QWidget::close);
}

QWidget *MainWindow::createRoleWidget(UserRole role)
{
    switch (role) {
    case UserRole::Admin:
        return new AdminWidget(this);
    case UserRole::Hr:
        return new HrWidget(this);
    case UserRole::ProductionReport:
        return new ProductionReportWidget(this);
    default:
        auto *fallback = new QWidget(this);
        auto *layout = new QVBoxLayout(fallback);
        layout->addWidget(new QLabel(tr("Роль не поддерживается приложением."), fallback));
        return fallback;
    }
}
