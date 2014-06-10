#ifndef _SETTINGS_H
#define	_SETTINGS_H

#include "ui_Settings.h"
#include <QString>

class Settings : public QWidget {
    Q_OBJECT
public:
    static const QString SETTINGS_PATTERNS;
    static const QString SETTINGS_DIRECTORIES;
    static const QString SETTINGS_MATCH_CASE;
    static const QString SETTINGS_DATE_MATCH;
    static const QString SETTINGS_DATE_OPTION;
    static const QString SETTINGS_DATE;

    Settings();
    virtual ~Settings();
    void loadSettings();
signals:
    void settingsChanged();
private:
    Ui::Settings ui;
    QString lastOpenedDirectory;
    void checkFirstRun();
    void setupGui();
    bool patternAlreadyExists(const QString & text);
private slots:
    void saveSettings();
    void changeAddPatternButtonState(const QString & text);
    void changeDeletePatternButtonState();
    void changeDeleteDirectoryButtonState();
    void addPattern();
    void deletePattern();
    void addDirectory();
    void deleteDirectory();
public slots:
    void show();
};

#endif	/* _SETTINGS_H */
