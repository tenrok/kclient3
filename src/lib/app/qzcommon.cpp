/* ============================================================
* Falkon - Qt web browser
* Copyright (C) 2014-2018 David Rosca <nowrep@gmail.com>
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
#include "qzcommon.h"
#include "../config.h"

namespace Qz
{
const int sessionVersion = 0x0004;

KCLIENT_EXPORT const char *APPNAME = "KClient";
KCLIENT_EXPORT const char *VERSION = KCLIENT_VERSION;
KCLIENT_EXPORT const char *AUTHOR = "David Rosca";
KCLIENT_EXPORT const char *COPYRIGHT = "2010-2018";
KCLIENT_EXPORT const char *WWWADDRESS = "https://falkon.org";
KCLIENT_EXPORT const char *BUGSADDRESS = "https://bugs.kde.org/describecomponents.cgi?product=Falkon";
KCLIENT_EXPORT const char *WIKIADDRESS = "https://userbase.kde.org/Falkon";
}
