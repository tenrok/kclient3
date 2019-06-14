/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2017 Razi Alavizadeh <s.r.alavizadeh@gmail.com>
* Copyright (C) 2018 David Rosca <nowrep@gmail.com>
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
#include "browserwindow.h"
#include "datapaths.h"
#include "mainapplication.h"
#include "restoremanager.h"
#include "sessionmanager.h"
#include "sessionmanagerdialog.h"
#include "settings.h"

#include <QAction>
#include <QComboBox>
#include <QDateTime>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileSystemWatcher>
#include <QInputDialog>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QSaveFile>

SessionManager::SessionManager(QObject* parent)
    : QObject(parent)
    , m_firstBackupSession(DataPaths::currentProfilePath() + QL1S("/session.dat.old"))
    , m_secondBackupSession(DataPaths::currentProfilePath() + QL1S("/session.dat.old1"))
{
    QFileSystemWatcher* sessionFilesWatcher = new QFileSystemWatcher({DataPaths::path(DataPaths::Sessions)}, this);
    connect(sessionFilesWatcher, &QFileSystemWatcher::directoryChanged, this, &SessionManager::sessionsDirectoryChanged);
    connect(sessionFilesWatcher, &QFileSystemWatcher::directoryChanged, this, &SessionManager::sessionsMetaDataChanged);

    loadSettings();
}

void SessionManager::aboutToShowSessionsMenu()
{
    QMenu* menu = qobject_cast<QMenu*>(sender());
    menu->clear();

    QActionGroup *group = new QActionGroup(menu);

    const auto sessions = sessionMetaData(/*withBackups*/ false);
    for (const SessionManager::SessionMetaData &metaData : sessions) {
        QAction* action = menu->addAction(metaData.name);
        action->setCheckable(true);
        action->setChecked(metaData.isActive);
        group->addAction(action);
        connect(action, &QAction::triggered, this, [=]() {
            switchToSession(metaData.filePath);
        });
    }
}

void SessionManager::sessionsDirectoryChanged()
{
    m_sessionsMetaDataList.clear();
}

void SessionManager::openSession(QString sessionFilePath, SessionFlags flags)
{
    if (sessionFilePath.isEmpty()) {
        QAction* action = qobject_cast<QAction*>(sender());
        if (!action)
            return;

        sessionFilePath = action->data().toString();
    }

    if (isActive(sessionFilePath)) {
        return;
    }

    RestoreData sessionData;
    RestoreManager::createFromFile(sessionFilePath, sessionData);

    if (!sessionData.isValid())
        return;

    BrowserWindow* window = mApp->getWindow();
    if (flags.testFlag(SwitchSession)) {
        writeCurrentSession(m_lastActiveSessionPath);

        window = mApp->createWindow(Qz::BW_OtherRestoredWindow);
        for (BrowserWindow* win : mApp->windows()) {
            if (win != window)
                win->close();
        }

        if (!flags.testFlag(ReplaceSession)) {
            m_lastActiveSessionPath = QFileInfo(sessionFilePath).canonicalFilePath();
            m_sessionsMetaDataList.clear();
        }
    }

    mApp->openSession(window, sessionData);
}

void SessionManager::renameSession(QString sessionFilePath, SessionFlags flags)
{
    if (sessionFilePath.isEmpty()) {
        QAction* action = qobject_cast<QAction*>(sender());
        if (!action)
            return;

        sessionFilePath = action->data().toString();
    }

    bool ok;
    const QString suggestedName = QFileInfo(sessionFilePath).completeBaseName() + (flags.testFlag(CloneSession) ? tr("_клонированная") : tr("_переименованная"));
    QString newName = QInputDialog::getText(mApp->activeWindow(), (flags.testFlag(CloneSession) ? tr("Клонировать сессию") : tr("Переименовать сессию")),
                                            tr("Пожалуйста, укажите новое имя:"), QLineEdit::Normal,
                                            suggestedName, &ok);

    if (!ok)
        return;

    const QString newSessionPath = QString("%1/%2.dat").arg(DataPaths::path(DataPaths::Sessions)).arg(newName);
    if (QFile::exists(newSessionPath)) {
        QMessageBox::information(mApp->activeWindow(), tr("Ошибка!"), tr("Файл сессии \"%1\" уже существует. Пожалуйста, укажите другое имя.").arg(newName));
        renameSession(sessionFilePath, flags);
        return;
    }

    if (flags.testFlag(CloneSession)) {
        if (!QFile::copy(sessionFilePath, newSessionPath)) {
            QMessageBox::information(mApp->activeWindow(), tr("Ошибка!"), tr("An error occurred when cloning session file."));
            return;
        }
    } else {
        if (!QFile::rename(sessionFilePath, newSessionPath)) {
            QMessageBox::information(mApp->activeWindow(), tr("Ошибка!"), tr("An error occurred when renaming session file."));
            return;
        }
        if (isActive(sessionFilePath)) {
            m_lastActiveSessionPath = newSessionPath;
            m_sessionsMetaDataList.clear();
        }
    }
}

void SessionManager::saveSession()
{
    bool ok;
    QString sessionName = QInputDialog::getText(mApp->activeWindow(), tr("Сохранение сессии"),
                                         tr("Пожалуйста, укажите имя для сохранения сессии:"), QLineEdit::Normal,
                                         tr("Сохранённая сессия (%1)").arg(QDateTime::currentDateTime().toString("dd MMM yyyy HH-mm-ss")), &ok);

    if (!ok)
        return;

    const QString filePath = QString("%1/%2.dat").arg(DataPaths::path(DataPaths::Sessions)).arg(sessionName);
    if (QFile::exists(filePath)) {
        QMessageBox::information(mApp->activeWindow(), tr("Ошибка!"), tr("Файл сессии \"%1\" уже существует. Пожалуйста, укажите другое имя.").arg(sessionName));
        saveSession();
        return;
    }

    writeCurrentSession(filePath);
}

void SessionManager::replaceSession(const QString &filePath)
{
    QMessageBox::StandardButton result = QMessageBox::information(mApp->activeWindow(), tr("Восстановление резервной копии"),
                                                                  tr("Вы уверены, что хотите заменить текущую сессию?"),
                                                                  QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        openSession(filePath, ReplaceSession);
    }
}

void SessionManager::switchToSession(const QString &filePath)
{
    openSession(filePath, SwitchSession);
}

void SessionManager::cloneSession(const QString &filePath)
{
    renameSession(filePath, CloneSession);
}

void SessionManager::deleteSession(const QString &filePath)
{
    QMessageBox::StandardButton result = QMessageBox::information(mApp->activeWindow(), tr("Удаление сессии"),
                                                                  tr("Вы уверены, что хотите удалить сессию '%1'?")
                                                                  .arg(QFileInfo(filePath).completeBaseName()), QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::Yes) {
        QFile::remove(filePath);
    }
}

void SessionManager::newSession()
{
    bool ok;
    QString sessionName = QInputDialog::getText(mApp->activeWindow(), tr("Новая сессия"),
                                         tr("Пожалуйста, укажите имя для создания новой сессии:"), QLineEdit::Normal,
                                         tr("Новая сессия (%1)").arg(QDateTime::currentDateTime().toString("dd MMM yyyy HH-mm-ss")), &ok);

    if (!ok)
        return;

    const QString filePath = QString("%1/%2.dat").arg(DataPaths::path(DataPaths::Sessions)).arg(sessionName);
    if (QFile::exists(filePath)) {
        QMessageBox::information(mApp->activeWindow(), tr("Ошибка!"), tr("Файл сессии \"%1\" уже существует. Пожалуйста, укажите другое имя.").arg(sessionName));
        newSession();
        return;
    }

    writeCurrentSession(m_lastActiveSessionPath);

    BrowserWindow* window = mApp->createWindow(Qz::BW_NewWindow);
    for (BrowserWindow* win : mApp->windows()) {
        if (win != window)
            win->close();
    }

    m_lastActiveSessionPath = filePath;
    autoSaveLastSession();
}

QList<SessionManager::SessionMetaData> SessionManager::sessionMetaData(bool withBackups)
{
    fillSessionsMetaDataListIfNeeded();

    auto out = m_sessionsMetaDataList;

    if (withBackups && QFile::exists(m_firstBackupSession)) {
        SessionMetaData data;
        data.name = tr("Резервная копия 1");
        data.filePath = m_firstBackupSession;
        data.isBackup = true;
        out.append(data);
    }
    if (withBackups && QFile::exists(m_secondBackupSession)) {
        SessionMetaData data;
        data.name = tr("Резервная копия 2");
        data.filePath = m_secondBackupSession;
        data.isBackup = true;
        out.append(data);
    }

    return out;
}

bool SessionManager::isActive(const QString &filePath) const
{
    return QFileInfo(filePath) == QFileInfo(m_lastActiveSessionPath);
}

bool SessionManager::isActive(const QFileInfo &fileInfo) const
{
    return fileInfo == QFileInfo(m_lastActiveSessionPath);
}

void SessionManager::fillSessionsMetaDataListIfNeeded()
{
    if (!m_sessionsMetaDataList.isEmpty())
        return;

    QDir dir(DataPaths::path(DataPaths::Sessions));

    const QFileInfoList sessionFiles = QFileInfoList() << QFileInfo(defaultSessionPath()) << dir.entryInfoList({QSL("*.*")}, QDir::Files, QDir::Time);

    QStringList fileNames;

    for (int i = 0; i < sessionFiles.size(); ++i) {
        const QFileInfo &fileInfo = sessionFiles.at(i);

        if (!RestoreManager::validateFile(fileInfo.absoluteFilePath()))
            continue;

        SessionMetaData metaData;
        metaData.name = fileInfo.completeBaseName();

        if (fileInfo == QFileInfo(defaultSessionPath())) {
            metaData.name = tr("Сессия по-умолчанию");
            metaData.isDefault = true;
        } else if (fileNames.contains(fileInfo.completeBaseName())) {
            metaData.name = fileInfo.fileName();
        } else {
            metaData.name = fileInfo.completeBaseName();
        }

        if (isActive(fileInfo)) {
            metaData.isActive = true;
        }

        fileNames << metaData.name;
        metaData.filePath = fileInfo.canonicalFilePath();

        m_sessionsMetaDataList << metaData;
    }
}

void SessionManager::loadSettings()
{
    QDir sessionsDir(DataPaths::path(DataPaths::Sessions));

    Settings settings;
    settings.beginGroup("Web-Browser-Settings");
    m_lastActiveSessionPath = settings.value("lastActiveSessionPath", defaultSessionPath()).toString();
    settings.endGroup();

    if (QDir::isRelativePath(m_lastActiveSessionPath)) {
        m_lastActiveSessionPath = sessionsDir.absoluteFilePath(m_lastActiveSessionPath);
    }

    // Fallback to default session
    if (!RestoreManager::validateFile(m_lastActiveSessionPath))
        m_lastActiveSessionPath = defaultSessionPath();
}

void SessionManager::saveSettings()
{
    QDir sessionsDir(DataPaths::path(DataPaths::Sessions));

    Settings settings;
    settings.beginGroup("Web-Browser-Settings");
    settings.setValue("lastActiveSessionPath", sessionsDir.relativeFilePath(m_lastActiveSessionPath));
    settings.endGroup();
}

QString SessionManager::defaultSessionPath()
{
    return DataPaths::currentProfilePath() + QL1S("/session.dat");
}

QString SessionManager::lastActiveSessionPath() const
{
    return m_lastActiveSessionPath;
}

void SessionManager::backupSavedSessions()
{
    if (!QFile::exists(m_lastActiveSessionPath)) {
        return;
    }

    if (QFile::exists(m_firstBackupSession)) {
        QFile::remove(m_secondBackupSession);
        QFile::copy(m_firstBackupSession, m_secondBackupSession);
    }

    QFile::remove(m_firstBackupSession);
    QFile::copy(m_lastActiveSessionPath, m_firstBackupSession);
}

void SessionManager::writeCurrentSession(const QString &filePath)
{
    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly) || file.write(mApp->saveState()) == -1) {
        qWarning() << "Error! can not write the current session file: " << filePath << file.errorString();
        return;
    }
    file.commit();
}

void SessionManager::openSessionManagerDialog()
{
    SessionManagerDialog *dialog = new SessionManagerDialog(mApp->getWindow());
    dialog->open();
}

void SessionManager::autoSaveLastSession()
{
    if (mApp->isPrivate() || mApp->windowCount() == 0) {
        return;
    }

    saveSettings();
    writeCurrentSession(m_lastActiveSessionPath);
}

QString SessionManager::askSessionFromUser()
{
    fillSessionsMetaDataListIfNeeded();

    QDialog dialog(mApp->getWindow(), Qt::WindowStaysOnTopHint);
    QLabel label(tr("Пожалуйста, выберите сессию, используемую при запуске:"), &dialog);
    QComboBox comboBox(&dialog);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout layout;
    layout.addWidget(&label);
    layout.addWidget(&comboBox);
    layout.addWidget(&buttonBox);
    dialog.setLayout(&layout);

    const QFileInfo lastActiveSessionFileInfo(m_lastActiveSessionPath);

    for (const SessionMetaData &metaData : m_sessionsMetaDataList) {
        if (QFileInfo(metaData.filePath) != lastActiveSessionFileInfo) {
            comboBox.addItem(metaData.name, metaData.filePath);
        }
        else {
            comboBox.insertItem(0, tr("%1 (последняя сессия)").arg(metaData.name), metaData.filePath);
        }
    }

    comboBox.setCurrentIndex(0);

    if (dialog.exec() == QDialog::Accepted) {
        m_lastActiveSessionPath = comboBox.currentData().toString();
    }

    return m_lastActiveSessionPath;
}
