#include "DirectoryWatcher.h"
#include <QtDebug>
#include <QMenu>
#include <QFileInfo>
#include <QSettings>
#include <QDir>

DirectoryWatcher::DirectoryWatcher() {
    settings = new Settings();
    systemTray = new QSystemTrayIcon(QIcon(":/img/img/eye.png"), this);
    fileSystemWatcher = new QFileSystemWatcher(this);

    createMenu();
    qApp->setQuitOnLastWindowClosed(false);
    systemTray->show();
    fillWatcher();

    connect(fileSystemWatcher, SIGNAL(directoryChanged(QString)), this, SLOT(directoryChanged(QString)));
    connect(fileSystemWatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));
    connect(settings, SIGNAL(settingsChanged()), this, SLOT(fillWatcher()));
}

DirectoryWatcher::~DirectoryWatcher() {
}

void DirectoryWatcher::createMenu() {
    QMenu* menu = new QMenu();
    QAction* action = menu->addAction(tr("Settings"));
    connect(action, SIGNAL(triggered()), settings, SLOT(show()));
    action = menu->addAction(tr("Close"));
    connect(action, SIGNAL(triggered()), qApp, SLOT(quit()));

    systemTray->setContextMenu(menu);
}

void DirectoryWatcher::fillWatcher() {
    fileSystemWatcher->removePaths(fileSystemWatcher->directories());
    fileSystemWatcher->removePaths(fileSystemWatcher->files());

    QSettings qsettings;
    QStringList directories = qsettings.value(Settings::SETTINGS_DIRECTORIES).toStringList();

    foreach(QString directory, directories) {

        foreach(QString file, QDir(directory).entryList(QDir::Files)) {
            fileSystemWatcher->addPath(directory + QDir::separator() + file);
        }
    }
    qDebug() << "watched files: " << fileSystemWatcher->files();
    fileSystemWatcher->addPaths(directories);
}

void DirectoryWatcher::fileChanged(const QString& path) {
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        fileSystemWatcher->removePath(path);
        return;
    }

    notifyChange(path);
}

void DirectoryWatcher::directoryChanged(const QString& path) {
    qDebug() << "change in directory";
    qDebug() << "path: " << path;

    QSet<QString> newDirContent;

    foreach(QString file, QDir(path).entryList(QDir::Files)) {
        newDirContent << path + QDir::separator() + file;
    }

    QSet<QString> newFiles = newDirContent.subtract(fileSystemWatcher->files().toSet());
    qDebug() << "newFiles: " << newFiles;

    foreach(QString file, newFiles) {
        notifyChange(file);
    }
}

void DirectoryWatcher::notifyChange(const QString & file) {
    QSettings qsettings;
    QStringList patterns = qsettings.value(Settings::SETTINGS_PATTERNS).toStringList();

    Qt::CaseSensitivity cs;
    if (qsettings.value(Settings::SETTINGS_MATCH_CASE).toBool()) {
        cs = Qt::CaseSensitive;
    } else {
        cs = Qt::CaseInsensitive;
    }

    foreach(QString pattern, patterns) {
        if (file.contains(pattern, cs)) {
            if (qsettings.value(Settings::SETTINGS_DATE_MATCH).toBool()) {
                QFileInfo fileInfo(file);
                switch (qsettings.value(Settings::SETTINGS_DATE_OPTION).toInt()) {
                    case 0:
                        if (fileInfo.created() > QDateTime(QDate::currentDate())) {
                            createNotification(pattern, file);
                        }
                        break;
                    case 1:
                        if (fileInfo.created() > qsettings.value(Settings::SETTINGS_DATE).toDateTime()) {
                            createNotification(pattern, file);
                        }
                        break;
                    default:
                        return;
                }
            } else {
                createNotification(pattern, file);
            }
        }
    }

    fillWatcher();
}

void DirectoryWatcher::createNotification(const QString & pattern, const QString& path) {
    qDebug() << "trying to create notification...";
    systemTray->showMessage(tr("Match found!"),
            tr("Pattern ") + pattern + tr(" found in path ") + path + ".");
}
