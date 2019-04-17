/* ============================================================
* Falkon - Qt web browser
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
#include "pythonplugin.h"

#include "plugins.h"
#include "datapaths.h"
#include "desktopfile.h"

#include <QDir>
#include <QCoreApplication>

#include <PyKClient/pykclient_python.h>

extern "C" PyObject *PyInit_PyKClient();

enum State
{
    PythonUninitialized,
    PythonInitialized,
    PythonError
};

State state = PythonUninitialized;

PluginInterface *pluginInterface = nullptr;
QHash<PyObject*, PluginInterface*> pluginInstances;

static QStringList script_paths()
{
    QStringList dirs = DataPaths::allPaths(DataPaths::Plugins);
    for (int i = 0; i < dirs.count(); ++i) {
        dirs[i].append(QSL("/python"));
    }
    return dirs;
}

static void cleanup()
{
    if (state > PythonUninitialized) {
        Py_Finalize();
        state = PythonUninitialized;
    }
}

static void set_path(const QStringList &scriptPaths)
{
    const QString originalPath = QString::fromLocal8Bit(qgetenv("PYTHONPATH"));

    QStringList paths = scriptPaths;
    paths.append(originalPath);

    qputenv("PYTHONPATH", paths.join(QL1C(':')).toLocal8Bit());
}

static State init()
{
    if (state > PythonUninitialized) {
        return state;
    }

    set_path(script_paths());

    if (PyImport_AppendInittab("KClient", PyInit_PyKClient) != 0) {
        PyErr_Print();
        qWarning() << "Failed to initialize KClient module!";
        return state = PythonError;
    }

    Py_Initialize();
    qAddPostRoutine(cleanup);
    return state = PythonInitialized;
}

void pykclient_register_plugin(PluginInterface *plugin)
{
    pluginInterface = plugin;
}

Plugins::Plugin pykclient_load_plugin(const QString &name)
{
    QString fullPath;
    if (QFileInfo(name).isAbsolute()) {
        fullPath = name;
    } else {
        fullPath = DataPaths::locate(DataPaths::Plugins, QSL("python/") + name);
        if (fullPath.isEmpty()) {
            qWarning() << "Plugin" << name << "not found";
            return Plugins::Plugin();
        }
    }

    Plugins::Plugin plugin;
    plugin.type = Plugins::Plugin::PythonPlugin;
    plugin.pluginId = QSL("python:%1").arg(QFileInfo(name).fileName());
    plugin.pluginSpec = Plugins::createSpec(DesktopFile(fullPath + QSL("/metadata.desktop")));
    return plugin;
}

void pykclient_init_plugin(Plugins::Plugin *plugin)
{
    if (init() != PythonInitialized) {
        return;
    }

    PyObject *module = static_cast<PyObject*>(plugin->data.value<void*>());
    if (module) {
        plugin->instance = pluginInstances.value(module);
        return;
    }

    const QString moduleName = plugin->pluginId.mid(7);

    pluginInterface = nullptr;

    module = PyImport_ImportModule(qPrintable(moduleName));
    if (!module) {
        PyErr_Print();
        qWarning() << "Failed to import module" << moduleName;
        return;
    }
    if (!pluginInterface) {
        qWarning() << "No plugin registered! KClient.registerPlugin() must be called from script.";
        return;
    }

    pluginInstances[module] = pluginInterface;
    plugin->instance = pluginInterface;
    plugin->data = QVariant::fromValue(static_cast<void*>(module));
}

QVector<Plugins::Plugin> pykclient_load_available_plugins()
{
    QVector<Plugins::Plugin> plugins;

    const QStringList dirs = script_paths();
    for (const QString &dir : dirs) {
        const auto modules = QDir(dir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &info : modules) {
            Plugins::Plugin plugin = pykclient_load_plugin(info.absoluteFilePath());
            if (plugin.pluginSpec.name.isEmpty()) {
                qWarning() << "Invalid plugin spec of" << info.absoluteFilePath() << "plugin";
                continue;
            }
            plugins.append(plugin);
        }
    }

    return plugins;
}

bool pykclient_run_script(const QByteArray &script)
{
    if (init() != PythonInitialized) {
        return false;
    }

    if (PyRun_SimpleString(script.constData()) != 0) {
        PyErr_Print();
        return false;
    }

    return true;
}
