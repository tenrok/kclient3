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
#ifndef KCLIENTSCHEMEHANDLER_H
#define KCLIENTSCHEMEHANDLER_H

#include <QMutex>
#include <QBuffer>
#include <QIODevice>
#include <QWebEngineUrlSchemeHandler>

#include "qzcommon.h"

class KCLIENT_EXPORT KClientSchemeHandler : public QWebEngineUrlSchemeHandler
{
public:
    explicit KClientSchemeHandler(QObject *parent = Q_NULLPTR);

    void requestStarted(QWebEngineUrlRequestJob *job) Q_DECL_OVERRIDE;

private:
    bool handleRequest(QWebEngineUrlRequestJob *job);
};

class KCLIENT_EXPORT KClientSchemeReply : public QIODevice
{
    Q_OBJECT

public:
    explicit KClientSchemeReply(QWebEngineUrlRequestJob *job, QObject *parent = Q_NULLPTR);

    qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    qint64 readData(char *data, qint64 maxSize) Q_DECL_OVERRIDE;
    qint64 writeData(const char *data, qint64 len) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void loadPage();

private:
    QString aboutPage();
    QString startPage();
    QString speeddialPage();
    QString restorePage();
    QString configPage();

    bool m_loaded;
    QBuffer m_buffer;
    QString m_pageName;
    QWebEngineUrlRequestJob *m_job;
    mutable QMutex m_mutex;
};

#endif // KCLIENTSCHEMEHANDLER_H
