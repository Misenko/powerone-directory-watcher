#ifndef _DIRECTORYWATCHER_H
#define	_DIRECTORYWATCHER_H

#include "Settings.h"
#include <QObject>
#include <QSystemTrayIcon>
#include <QFileSystemWatcher>

class DirectoryWatcher : public QObject {
    Q_OBJECT
public:
    DirectoryWatcher();
    virtual ~DirectoryWatcher();
private:
    Settings* settings;
    QSystemTrayIcon* systemTray;
    QFileSystemWatcher* fileSystemWatcher;
    void createMenu();
    void createNotification(const QString & pattern, const QString& path);
    void notifyChange(const QString & path);
private slots:
    void fileChanged(const QString & path);
    void directoryChanged(const QString & path);
    void fillWatcher();
};

#endif	/* _DIRECTORYWATCHER_H */
