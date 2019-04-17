/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2013-2014  David Rosca <nowrep@gmail.com>
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
#ifndef DATABASEPASSWORDBACKEND_H
#define DATABASEPASSWORDBACKEND_H

#include "passwordbackend.h"
#include "qzcommon.h"

class KCLIENT_EXPORT DatabasePasswordBackend : public PasswordBackend
{
public:
    explicit DatabasePasswordBackend();

    QString name() const override;

    QVector<PasswordEntry> getEntries(const QUrl &url) override;
    QVector<PasswordEntry> getAllEntries() override;

    void addEntry(const PasswordEntry &entry) override;
    bool updateEntry(const PasswordEntry &entry) override;
    void updateLastUsed(PasswordEntry &entry) override;

    void removeEntry(const PasswordEntry &entry) override;
    void removeAll() override;
};

#endif // DATABASEPASSWORDBACKEND_H
