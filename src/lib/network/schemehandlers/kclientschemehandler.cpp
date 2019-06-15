/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2010-2018 David Rosca <nowrep@gmail.com>
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
#include "kclientschemehandler.h"
#include "qztools.h"
#include "browserwindow.h"
#include "mainapplication.h"
#include "speeddial.h"
#include "pluginproxy.h"
#include "plugininterface.h"
#include "settings.h"
#include "datapaths.h"
#include "iconprovider.h"
#include "sessionmanager.h"
#include "restoremanager.h"
#include "../config.h"

#include <QTimer>
#include <QSettings>
#include <QUrlQuery>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestJob>

static QString authorString(const char* name, const QString &mail)
{
    return QSL("%1 &lt;<a href=\"mailto:%2\">%2</a>&gt;").arg(QString::fromUtf8(name), mail);
}

KClientSchemeHandler::KClientSchemeHandler(QObject *parent)
    : QWebEngineUrlSchemeHandler(parent)
{
}

void KClientSchemeHandler::requestStarted(QWebEngineUrlRequestJob *job)
{
    if (handleRequest(job)) {
        return;
    }

    QStringList knownPages;
    knownPages << "about" << "start" << "speeddial" << "config" << "restore";

    if (knownPages.contains(job->requestUrl().path()))
        job->reply(QByteArrayLiteral("text/html"), new KClientSchemeReply(job, job));
    else
        job->fail(QWebEngineUrlRequestJob::UrlInvalid);
}

bool KClientSchemeHandler::handleRequest(QWebEngineUrlRequestJob *job)
{
    QUrlQuery query(job->requestUrl());
    if (!query.isEmpty() && job->requestUrl().path() == QL1S("restore")) {
        if (mApp->restoreManager()) {
            if (query.hasQueryItem(QSL("new-session"))) {
                mApp->restoreManager()->clearRestoreData();
            } else if (query.hasQueryItem(QSL("restore-session"))) {
                mApp->restoreSession(nullptr, mApp->restoreManager()->restoreData());
            }
        }
        mApp->destroyRestoreManager();
        job->redirect(QUrl(QSL("kclient:start")));
        return true;
    } else if (job->requestUrl().path() == QL1S("reportbug")) {
        job->redirect(QUrl(Qz::BUGSADDRESS));
        return true;
    }

    return false;
}

KClientSchemeReply::KClientSchemeReply(QWebEngineUrlRequestJob *job, QObject *parent)
    : QIODevice(parent)
    , m_loaded(false)
    , m_job(job)
{
    m_pageName = m_job->requestUrl().path();
    loadPage();
}

void KClientSchemeReply::loadPage()
{
    if (m_loaded)
        return;

    QString contents;

    if (m_pageName == QLatin1String("about")) {
        contents = aboutPage();
    } else if (m_pageName == QLatin1String("start")) {
        contents = startPage();
    } else if (m_pageName == QLatin1String("speeddial")) {
        contents = speeddialPage();
    } else if (m_pageName == QLatin1String("config")) {
        contents = configPage();
    } else if (m_pageName == QLatin1String("restore")) {
        contents = restorePage();
    }

    QMutexLocker lock(&m_mutex);
    m_buffer.setData(contents.toUtf8());
    m_buffer.open(QIODevice::ReadOnly);
    lock.unlock();

    emit readyRead();

    m_loaded = true;
}

qint64 KClientSchemeReply::bytesAvailable() const
{
    QMutexLocker lock(&m_mutex);
    return m_buffer.bytesAvailable();
}

qint64 KClientSchemeReply::readData(char *data, qint64 maxSize)
{
    QMutexLocker lock(&m_mutex);
    return m_buffer.read(data, maxSize);
}

qint64 KClientSchemeReply::writeData(const char *data, qint64 len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    return 0;
}

QString KClientSchemeReply::startPage()
{
    static QString sPage;

    if (!sPage.isEmpty()) {
        return sPage;
    }

    sPage.append(QzTools::readAllFileContents(":html/start.html"));
    sPage.replace(QLatin1String("%ABOUT-IMG%"), QSL("qrc:icons/other/startpage.svg"));

    sPage.replace(QLatin1String("%TITLE%"), tr("Стартовая страница"));
    sPage.replace(QLatin1String("%BUTTON-LABEL%"), tr("Найти"));
    sPage.replace(QLatin1String("%SEARCH-BY%"), tr("Результаты поиска предоставлены DuckDuckGo"));
    sPage.replace(QLatin1String("%WWW%"), Qz::WIKIADDRESS);
    sPage.replace(QLatin1String("%ABOUT-KCLIENT%"), tr("О KClient"));
    sPage.replace(QLatin1String("%PRIVATE-BROWSING%"), mApp->isPrivate() ? tr("<h1>Режим приватного просмотра</h1>") : QString());
    sPage = QzTools::applyDirectionToPage(sPage);

    return sPage;
}

QString KClientSchemeReply::aboutPage()
{
    static QString aPage;

    if (aPage.isEmpty()) {
        aPage.append(QzTools::readAllFileContents(":html/about.html"));
        aPage.replace(QLatin1String("%ABOUT-IMG%"), QSL("qrc:icons/other/about.svg"));
        aPage.replace(QLatin1String("%COPYRIGHT-INCLUDE%"), QzTools::readAllFileContents(":html/copyright").toHtmlEscaped());

        aPage.replace(QLatin1String("%TITLE%"), tr("О KClient"));
        aPage.replace(QLatin1String("%ABOUT-KCLIENT%"), tr("О KClient"));
        aPage.replace(QLatin1String("%INFORMATIONS-ABOUT-VERSION%"), tr("Информация о версии"));
        aPage.replace(QLatin1String("%COPYRIGHT%"), tr("Авторские права"));

        aPage.replace(QLatin1String("%VERSION-INFO%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Версия"),
#ifdef KCLIENT_GIT_REVISION
                              QString("%1 (%2)").arg(Qz::VERSION, KCLIENT_GIT_REVISION)));
#else
                              Qz::VERSION));
#endif

        aPage.replace(QLatin1String("%MAIN-DEVELOPER%"), tr("Основной разработчик"));
        aPage.replace(QLatin1String("%MAIN-DEVELOPER-TEXT%"), authorString(Qz::AUTHOR, "nowrep@gmail.com"));
        aPage = QzTools::applyDirectionToPage(aPage);
    }

    return aPage;
}

QString KClientSchemeReply::speeddialPage()
{
    static QString dPage;

    if (dPage.isEmpty()) {
        dPage.append(QzTools::readAllFileContents(":html/speeddial.html"));
        dPage.replace(QLatin1String("%IMG_PLUS%"), QLatin1String("qrc:html/plus.svg"));
        dPage.replace(QLatin1String("%IMG_CLOSE%"), QLatin1String("qrc:html/close.svg"));
        dPage.replace(QLatin1String("%IMG_EDIT%"), QLatin1String("qrc:html/edit.svg"));
        dPage.replace(QLatin1String("%IMG_RELOAD%"), QLatin1String("qrc:html/reload.svg"));
        dPage.replace(QLatin1String("%LOADING-IMG%"), QLatin1String("qrc:html/loading.gif"));
        dPage.replace(QLatin1String("%IMG_SETTINGS%"), QLatin1String("qrc:html/configure.svg"));

        dPage.replace(QLatin1String("%SITE-TITLE%"), tr("Страница быстрого доступа"));
        dPage.replace(QLatin1String("%ADD-TITLE%"), tr("Добавить новую страницу"));
        dPage.replace(QLatin1String("%TITLE-EDIT%"), tr("Править"));
        dPage.replace(QLatin1String("%TITLE-REMOVE%"), tr("Удалить"));
        dPage.replace(QLatin1String("%TITLE-RELOAD%"), tr("Обновить"));
        dPage.replace(QLatin1String("%TITLE-WARN%"), tr("Вы уверены, что хотите удалить этот быстрый набор?"));
        dPage.replace(QLatin1String("%TITLE-WARN-REL%"), tr("Вы уверены, что хотите обновить все быстрые наборы?"));
        dPage.replace(QLatin1String("%TITLE-FETCHTITLE%"), tr("Загрузить заголовок со страницы"));
        dPage.replace(QLatin1String("%JAVASCRIPT-DISABLED%"), tr("Быстрый набор требует JavaScript."));
        dPage.replace(QLatin1String("%URL%"), tr("Url"));
        dPage.replace(QLatin1String("%TITLE%"), tr("Заголовок"));
        dPage.replace(QLatin1String("%APPLY%"), tr("Применить"));
        dPage.replace(QLatin1String("%CANCEL%"), tr("Отмена"));
        dPage.replace(QLatin1String("%NEW-PAGE%"), tr("Новая страница"));
        dPage.replace(QLatin1String("%SETTINGS-TITLE%"), tr("Настройки быстрого доступа"));
        dPage.replace(QLatin1String("%TXT_PLACEMENT%"), tr("Размещение: "));
        dPage.replace(QLatin1String("%TXT_AUTO%"), tr("Автоматически"));
        dPage.replace(QLatin1String("%TXT_COVER%"), tr("Заполнение"));
        dPage.replace(QLatin1String("%TXT_FIT%"), tr("Подогнать"));
        dPage.replace(QLatin1String("%TXT_FWIDTH%"), tr("Подогнать по ширине"));
        dPage.replace(QLatin1String("%TXT_FHEIGHT%"), tr("Подогнать по высоте"));
        dPage.replace(QLatin1String("%TXT_NOTE%"), tr("Использовать фоновое изображение"));
        dPage.replace(QLatin1String("%TXT_SELECTIMAGE%"), tr("Щёлкните для выбора изображения"));
        dPage.replace(QLatin1String("%TXT_NRROWS%"), tr("Максимум страниц в строке:"));
        dPage.replace(QLatin1String("%TXT_SDSIZE%"), tr("Изменить размер страниц:"));
        dPage.replace(QLatin1String("%TXT_CNTRDLS%"), tr("Center speed dials"));
        dPage = QzTools::applyDirectionToPage(dPage);
    }

    QString page = dPage;
    SpeedDial* dial = mApp->plugins()->speedDial();

    page.replace(QLatin1String("%INITIAL-SCRIPT%"), dial->initialScript().toUtf8().toBase64());
    page.replace(QLatin1String("%IMG_BACKGROUND%"), dial->backgroundImage());
    page.replace(QLatin1String("%URL_BACKGROUND%"), dial->backgroundImageUrl());
    page.replace(QLatin1String("%B_SIZE%"), dial->backgroundImageSize());
    page.replace(QLatin1String("%ROW-PAGES%"), QString::number(dial->pagesInRow()));
    page.replace(QLatin1String("%SD-SIZE%"), QString::number(dial->sdSize()));
    page.replace(QLatin1String("%SD-CENTER%"), dial->sdCenter() ? QSL("true") : QSL("false"));

    return page;
}

QString KClientSchemeReply::restorePage()
{
    static QString rPage;

    if (rPage.isEmpty()) {
        rPage.append(QzTools::readAllFileContents(":html/restore.html"));
        rPage.replace(QLatin1String("%IMAGE%"), QzTools::pixmapToDataUrl(IconProvider::standardIcon(QStyle::SP_MessageBoxWarning).pixmap(45)).toString());
        rPage.replace(QLatin1String("%TITLE%"), tr("Восстановление сессии"));
        rPage.replace(QLatin1String("%OOPS%"), tr("Oops, KClient crashed."));
        rPage.replace(QLatin1String("%APOLOGIZE%"), tr("Мы приносим извинения за это. Вы хотите восстановить последнее сохраненное состояние?"));
        rPage.replace(QLatin1String("%TRY-REMOVING%"), tr("Попробуйте удалить одну или несколько вкладок, которые, по вашему мнению, вызывают проблемы"));
        rPage.replace(QLatin1String("%START-NEW%"), tr("Или вы можете начать совершенно новую сессию"));
        rPage.replace(QLatin1String("%WINDOW%"), tr("Окно"));
        rPage.replace(QLatin1String("%WINDOWS-AND-TABS%"), tr("Окна и вкладки"));
        rPage.replace(QLatin1String("%BUTTON-START-NEW%"), tr("Запустить новую сессию"));
        rPage.replace(QLatin1String("%BUTTON-RESTORE%"), tr("Восстановить"));
        rPage.replace(QLatin1String("%JAVASCRIPT-DISABLED%"), tr("Requires enabled JavaScript."));
        rPage = QzTools::applyDirectionToPage(rPage);
    }

    return rPage;
}

QString KClientSchemeReply::configPage()
{
    static QString cPage;

    if (cPage.isEmpty()) {
        cPage.append(QzTools::readAllFileContents(":html/config.html"));
        cPage.replace(QLatin1String("%ABOUT-IMG%"), QSL("qrc:icons/other/about.svg"));

        cPage.replace(QLatin1String("%TITLE%"), tr("Информация о конфигурации"));
        cPage.replace(QLatin1String("%CONFIG%"), tr("Информация о конфигурации"));
        cPage.replace(QLatin1String("%INFORMATIONS-ABOUT-VERSION%"), tr("Информация о версии"));
        cPage.replace(QLatin1String("%CONFIG-ABOUT%"), tr("Эта страница содержит информацию о текущей конфигурации KClient - имеет отношение к устранению неполадок. Пожалуйста, включите эту информацию при отправке отчетов об ошибках."));
        cPage.replace(QLatin1String("%BROWSER-IDENTIFICATION%"), tr("Идентификация браузера"));
        cPage.replace(QLatin1String("%PATHS%"), tr("Пути"));
        cPage.replace(QLatin1String("%BUILD-CONFIG%"), tr("Конфигурация сборки"));
        cPage.replace(QLatin1String("%PREFS%"), tr("Настройки"));
        cPage.replace(QLatin1String("%OPTION%"), tr("Опция"));
        cPage.replace(QLatin1String("%VALUE%"), tr("Значение"));
        cPage.replace(QLatin1String("%PLUGINS%"), tr("Расширения"));
        cPage.replace(QLatin1String("%PL-NAME%"), tr("Наименование"));
        cPage.replace(QLatin1String("%PL-VER%"), tr("Версия"));
        cPage.replace(QLatin1String("%PL-AUTH%"), tr("Автор"));
        cPage.replace(QLatin1String("%PL-DESC%"), tr("Описание"));

        auto allPaths = [](DataPaths::Path type) {
            QString out;
            const auto paths = DataPaths::allPaths(type);
            for (const QString &path : paths) {
                if (!out.isEmpty()) {
                    out.append(QSL("<br>"));
                }
                out.append(path);
            }
            return out;
        };

        cPage.replace(QLatin1String("%VERSION-INFO%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Версия приложения"),
#ifdef KCLIENT_GIT_REVISION
                              QString("%1 (%2)").arg(Qz::VERSION, KCLIENT_GIT_REVISION)
#else
                              Qz::VERSION
#endif
                                                          ) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Версия Qt"), qVersion()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Платформа"), QzTools::operatingSystemLong()));

        cPage.replace(QLatin1String("%PATHS-TEXT%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Профиль"), DataPaths::currentProfilePath()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Настройки"), DataPaths::currentProfilePath() + "/settings.ini") +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Сохранённая сессия"), SessionManager::defaultSessionPath()) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Данные"), allPaths(DataPaths::AppData)) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Темы"), allPaths(DataPaths::Themes)) +
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Расширения"), allPaths(DataPaths::Plugins)));

#ifdef QT_DEBUG
        QString debugBuild = tr("<b>Enabled</b>");
#else
        QString debugBuild = tr("Отключен");
#endif

#ifdef Q_OS_WIN
#if defined(Q_OS_WIN) && defined(W7API)
        QString w7APIEnabled = tr("<b>Включен</b>");
#else
        QString w7APIEnabled = tr("Отключен");
#endif
#endif

        QString portableBuild = mApp->isPortable() ? tr("<b>Включен</b>") : tr("Отключен");

        cPage.replace(QLatin1String("%BUILD-CONFIG-TEXT%"),
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Отладочная сборка"), debugBuild) +
#ifdef Q_OS_WIN
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Windows 7 API"), w7APIEnabled) +
#endif
                      QString("<dt>%1</dt><dd>%2<dd>").arg(tr("Портативная сборка"), portableBuild));

        cPage = QzTools::applyDirectionToPage(cPage);
    }

    QString page = cPage;
    page.replace(QLatin1String("%USER-AGENT%"), mApp->webProfile()->httpUserAgent());

    QString pluginsString;
    const QList<Plugins::Plugin> &availablePlugins = mApp->plugins()->getAvailablePlugins();

    foreach (const Plugins::Plugin &plugin, availablePlugins) {
        PluginSpec spec = plugin.pluginSpec;
        pluginsString.append(QString("<tr><td>%1</td><td>%2</td><td>%3</td><td>%4</td></tr>").arg(
                                 spec.name, spec.version, spec.author.toHtmlEscaped(), spec.description));
    }

    if (pluginsString.isEmpty()) {
        pluginsString = QString("<tr><td colspan=4 class=\"no-available-plugins\">%1</td></tr>").arg(tr("Нет доступных расширений."));
    }

    page.replace(QLatin1String("%PLUGINS-INFO%"), pluginsString);

    QString allGroupsString;
    QSettings* settings = Settings::globalSettings();
    foreach (const QString &group, settings->childGroups()) {
        QString groupString = QString("<tr><th colspan=\"2\">[%1]</th></tr>").arg(group);
        settings->beginGroup(group);

        foreach (const QString &key, settings->childKeys()) {
            const QVariant keyValue = settings->value(key);
            QString keyString;

            switch (keyValue.type()) {
            case QVariant::ByteArray:
                keyString = QLatin1String("QByteArray");
                break;

            case QVariant::Point: {
                const QPoint point = keyValue.toPoint();
                keyString = QString("QPoint(%1, %2)").arg(point.x()).arg(point.y());
                break;
            }

            case QVariant::StringList:
                keyString = keyValue.toStringList().join(",");
                break;

            default:
                keyString = keyValue.toString();
            }

            if (keyString.isEmpty()) {
                keyString = QLatin1String("\"empty\"");
            }

            groupString.append(QString("<tr><td>%1</td><td>%2</td></tr>").arg(key, keyString.toHtmlEscaped()));
        }

        settings->endGroup();
        allGroupsString.append(groupString);
    }

    page.replace(QLatin1String("%PREFS-INFO%"), allGroupsString);

    return page;
}
