/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2017 David Rosca <nowrep@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
* ============================================================ */
#include "qztools.h"
#include "siteinfowidget.h"
#include "ui_siteinfowidget.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "webpage.h"
#include "tabbedwebview.h"
#include "sqldatabase.h"
#include "protocolhandlermanager.h"

#include <QToolTip>

SiteInfoWidget::SiteInfoWidget(BrowserWindow* window, QWidget* parent)
    : LocationBarPopup(parent)
    , ui(new Ui::SiteInfoWidget)
    , m_window(window)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

    setPopupAlignment(Qt::AlignLeft);

    WebView* view = m_window->weView();

    ui->titleLabel->setText(tr("<b>Сайт %1<b/>").arg(view->url().host()));

    if (view->url().scheme() == QL1S("https")) {
        ui->secureLabel->setText(tr("Выше подключение к этому сайту <b>безопасно</b>."));
        ui->secureIcon->setPixmap(QPixmap(":/icons/locationbar/safe.png"));
    }
    else {
        ui->secureLabel->setText(tr("Ваше подключение к этому сайту <b>не безопасно</b>."));
        ui->secureIcon->setPixmap(QPixmap(":/icons/locationbar/unsafe.png"));
    }

    QString scheme = view->url().scheme();
    QString host = view->url().host();

    QSqlQuery query(SqlDatabase::instance()->database());
    query.prepare("SELECT sum(count) FROM history WHERE url LIKE ?");
    query.addBindValue(QString("%1://%2%").arg(scheme, host));
    query.exec();

    if (query.next()) {
        int count = query.value(0).toInt();
        if (count > 3) {
            ui->historyLabel->setText(tr("Это ваше <b>%1</b> посещение этого сайта.").arg(QString::number(count) + "."));
            ui->historyIcon->setPixmap(QPixmap(":/icons/locationbar/visit3.png"));
        }
        else if (count == 0) {
            ui->historyLabel->setText(tr("Вы <b>никогда</b> не посещали это сайт ранее."));
            ui->historyIcon->setPixmap(QPixmap(":/icons/locationbar/visit1.png"));
        }
        else {
            ui->historyIcon->setPixmap(QPixmap(":/icons/locationbar/visit2.png"));
            QString text;
            if (count == 1) {
                text = tr("первый");
            }
            else if (count == 2) {
                text = tr("второй");
            }
            else if (count == 3) {
                text = tr("третий");
            }
            ui->historyLabel->setText(tr("Это ваше <b>%1</b> посещение этого сайта.").arg(text));
        }
    }

    updateProtocolHandler();

    connect(ui->pushButton, &QAbstractButton::clicked, m_window->action(QSL("Tools/SiteInfo")), &QAction::trigger);
    connect(ui->protocolHandlerButton, &QPushButton::clicked, this, &SiteInfoWidget::protocolHandlerButtonClicked);
}

SiteInfoWidget::~SiteInfoWidget()
{
    delete ui;
}

void SiteInfoWidget::updateProtocolHandler()
{
    WebPage *page = m_window->weView()->page();

    const QString scheme = page->registerProtocolHandlerRequestScheme();
    const QUrl registeredUrl = mApp->protocolHandlerManager()->protocolHandlers().value(scheme);

    if (!scheme.isEmpty() && registeredUrl != page->registerProtocolHandlerRequestUrl()) {
        ui->protocolHandlerLabel->setText(tr("Зарегистрировать как <b>%1</b> обработчик ссылок").arg(page->registerProtocolHandlerRequestScheme()));
        ui->protocolHandlerButton->setText(tr("Регистрация"));
    } else {
        ui->protocolHandlerLabel->hide();
        ui->protocolHandlerButton->hide();
        ui->protocolHandlerLine->hide();
    }
}

void SiteInfoWidget::protocolHandlerButtonClicked()
{
    WebPage *page = m_window->weView()->page();

    mApp->protocolHandlerManager()->addProtocolHandler(page->registerProtocolHandlerRequestScheme(), page->registerProtocolHandlerRequestUrl());
    close();
}
