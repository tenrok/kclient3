/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2015 David Rosca <nowrep@gmail.com>
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

#ifndef AUTOFILLJSOBJECT_H
#define AUTOFILLJSOBJECT_H

#include <QObject>

class ExternalJsObject;

class AutoFillJsObject : public QObject
{
    Q_OBJECT
public:
    explicit AutoFillJsObject(ExternalJsObject *parent);

public Q_SLOTS:
    void formSubmitted(const QString &frameUrl, const QString &username, const QString &password, const QByteArray &data);

private:
    ExternalJsObject *m_jsObject;
};

#endif // AUTOFILLJSOBJECT_H
