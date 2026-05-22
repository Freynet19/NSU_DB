#include "shell/MainWindow.h"

#include "core/UserRole.h"
#include "db/DatabaseManager.h"
#include "roles/AdminWidget.h"
#include "roles/HrWidget.h"
#include "roles/ProductionReportWidget.h"
#include "ui_MainWindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(UserRole role, QWidget* parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("ИС автомобилестроительного предприятия — %1")
        .arg(userRoleDisplayName(role)));
    resize(1200, 800);

    ui->userHeader->setText(tr("Пользователь: %1").arg(DatabaseManager::instance().username()));

    auto* roleLayout = new QVBoxLayout(ui->roleHost);
    roleLayout->setContentsMargins(0, 0, 0, 0);
    roleLayout->addWidget(createRoleWidget(role), 1);

    statusBar()->showMessage(userRoleDisplayName(role));
}

MainWindow::~MainWindow()
{
    delete ui;
}

QWidget* MainWindow::createRoleWidget(UserRole role)
{
    switch (role)
    {
    case UserRole::Admin:
        return new AdminWidget(this);
    case UserRole::Hr:
        return new HrWidget(this);
    case UserRole::ProductionReport:
        return new ProductionReportWidget(this);
    default:
        auto* fallback = new QWidget(this);
        auto* layout = new QVBoxLayout(fallback);
        layout->addWidget(new QLabel(tr("Роль не поддерживается приложением."), fallback));
        return fallback;
    }
}
