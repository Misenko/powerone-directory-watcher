#include "Settings.h"
#include <QSettings>
#include <QStringList>
#include <QDateTime>
#include <QStringListModel>
#include <QModelIndexList>
#include <QFileDialog>

const QString Settings::SETTINGS_PATTERNS = "patterns";
const QString Settings::SETTINGS_DIRECTORIES = "directories";
const QString Settings::SETTINGS_MATCH_CASE = "match_case";
const QString Settings::SETTINGS_DATE_MATCH = "date_match";
const QString Settings::SETTINGS_DATE_OPTION = "date_option";
const QString Settings::SETTINGS_DATE = "date";

Settings::Settings() {
    setupGui();

    checkFirstRun();
    lastOpenedDirectory = QDir::homePath();
}

Settings::~Settings() {
}

void Settings::checkFirstRun() {
    QSettings settings;
    if (!settings.contains(SETTINGS_PATTERNS)) {
        settings.setValue(SETTINGS_PATTERNS, QStringList());
        settings.setValue(SETTINGS_DIRECTORIES, QStringList());
        settings.setValue(SETTINGS_MATCH_CASE, true);
        settings.setValue(SETTINGS_DATE_MATCH, false);
        settings.setValue(SETTINGS_DATE_OPTION, 0);
        settings.setValue(SETTINGS_DATE, QDateTime::currentDateTime());

        show();
    }
}

void Settings::setupGui() {
    ui.setupUi(this);

    connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(ui.patternEdit, SIGNAL(textChanged(QString)), this, SLOT(changeAddPatternButtonState(QString)));

    connect(ui.patternAddButton, SIGNAL(clicked()), this, SLOT(addPattern()));
    connect(ui.patternDeleteButton, SIGNAL(clicked()), this, SLOT(deletePattern()));

    connect(ui.directoryOpenButton, SIGNAL(clicked()), this, SLOT(addDirectory()));
    connect(ui.directoryDeleteButton, SIGNAL(clicked()), this, SLOT(deleteDirectory()));
}

void Settings::saveSettings() {
    QSettings settings;

    QStringListModel* model = (QStringListModel*) ui.patternListView->model();
    settings.setValue(SETTINGS_PATTERNS, model->stringList());
    model = (QStringListModel*) ui.directoryListView->model();
    settings.setValue(SETTINGS_DIRECTORIES, model->stringList());
    settings.setValue(SETTINGS_MATCH_CASE, ui.matchCaseCheckBox->isChecked());
    settings.setValue(SETTINGS_DATE_MATCH, ui.dateMatchGroupBox->isChecked());
    if (ui.dateOption0RadioButton->isChecked()) {
        settings.setValue(SETTINGS_DATE_OPTION, 0);
    } else {
        settings.setValue(SETTINGS_DATE_OPTION, 1);
    }
    settings.setValue(SETTINGS_DATE, ui.dateOption1dateTimeEdit->dateTime());

    disconnect(ui.patternListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(changeDeletePatternButtonState()));
    disconnect(ui.directoryListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(changeDeleteDirectoryButtonState()));

    hide();

    emit settingsChanged();
}

void Settings::loadSettings() {
    QSettings settings;

    if (ui.patternListView->model() == NULL) {
        QStringListModel* model = new QStringListModel();
        model->setStringList(settings.value(SETTINGS_PATTERNS).toStringList());
        ui.patternListView->setModel(model);
    } else {
        ((QStringListModel*) ui.patternListView->model())->setStringList(settings.value(SETTINGS_PATTERNS).toStringList());
    }

    if (ui.directoryListView->model() == NULL) {
        QStringListModel* model = new QStringListModel();
        model->setStringList(settings.value(SETTINGS_DIRECTORIES).toStringList());
        ui.directoryListView->setModel(model);
    } else {
        ((QStringListModel*) ui.directoryListView->model())->setStringList(settings.value(SETTINGS_DIRECTORIES).toStringList());
    }

    ui.matchCaseCheckBox->setChecked(settings.value(SETTINGS_MATCH_CASE).toBool());
    ui.dateMatchGroupBox->setChecked(settings.value(SETTINGS_DATE_MATCH).toBool());
    if (settings.value(SETTINGS_DATE_OPTION).toInt() == 0) {
        ui.dateOption0RadioButton->setChecked(true);
        ui.dateOption1RadioButton->setChecked(false);
    } else {
        ui.dateOption0RadioButton->setChecked(false);
        ui.dateOption1RadioButton->setChecked(true);
    }

    ui.dateOption1dateTimeEdit->setDateTime(settings.value(SETTINGS_DATE).toDateTime());

    ui.patternEdit->clear();

    connect(ui.patternListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(changeDeletePatternButtonState()));
    connect(ui.directoryListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(changeDeleteDirectoryButtonState()));
}

void Settings::changeAddPatternButtonState(const QString & text) {
    ui.patternAddButton->setEnabled(!text.isEmpty() && !patternAlreadyExists(text));
}

void Settings::changeDeletePatternButtonState() {
    ui.patternDeleteButton->setEnabled(ui.patternListView->selectionModel()->hasSelection());
}

bool Settings::patternAlreadyExists(const QString& text) {
    return ((QStringListModel*) ui.patternListView->model())->stringList().contains(text);
}

void Settings::addPattern() {
    QStringList list = ((QStringListModel*) ui.patternListView->model())->stringList();
    list << ui.patternEdit->text();
    ((QStringListModel*) ui.patternListView->model())->setStringList(list);
    ui.patternEdit->clear();
    ui.patternDeleteButton->setEnabled(false);
}

void Settings::deletePattern() {
    ui.patternListView->setUpdatesEnabled(false);

    QModelIndexList indexes = ui.patternListView->selectionModel()->selectedIndexes();
    qSort(indexes.begin(), indexes.end());

    for (int i = indexes.count() - 1; i > -1; --i) {
        ui.patternListView->model()->removeRow(indexes.at(i).row());
    }

    ui.patternListView->setUpdatesEnabled(true);
}

void Settings::changeDeleteDirectoryButtonState() {
    ui.directoryDeleteButton->setEnabled(ui.directoryListView->selectionModel()->hasSelection());
}

void Settings::addDirectory() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
            lastOpenedDirectory,
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    QStringList list = ((QStringListModel*) ui.directoryListView->model())->stringList();
    list << dir;
    ((QStringListModel*) ui.directoryListView->model())->setStringList(list);
}

void Settings::deleteDirectory() {
    ui.directoryListView->setUpdatesEnabled(false);

    QModelIndexList indexes = ui.directoryListView->selectionModel()->selectedIndexes();
    qSort(indexes.begin(), indexes.end());

    for (int i = indexes.count() - 1; i > -1; --i) {
        ui.directoryListView->model()->removeRow(indexes.at(i).row());
    }

    ui.directoryListView->setUpdatesEnabled(true);
}

void Settings::show() {
    loadSettings();
    QWidget::show();
}
