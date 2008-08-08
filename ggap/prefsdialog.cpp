#include "ggap/prefsdialog.h"
#include "ggap/prefsdialog-p.h"
#include "ggap/prefs.h"
#include "ggap/gap.h"
#include "ggap/wswindow-p.h"
#include "ggap/help.h"
#include "ggap/ui_prefs.h"
#include <QtGui>
#include <QDir>

using namespace ggap;

///////////////////////////////////////////////////////////////////////////
// PrefsDialogBase
//

PrefsDialogBase::PrefsDialogBase(QWidget *parent) :
    NiceDialog(parent)
{
}

void PrefsDialogBase::accept()
{
    NiceDialog::accept();
    apply();
}


///////////////////////////////////////////////////////////////////////////
// PrefsDialog
//

struct PrefsDialog::Private {
    Ui::PrefsDialog ui;
    QFont worksheetFont;
    QFont helpFont;

    QString gapDir;
    QString gapExe;
    QString gapArgs;
    QStringList gapRoots;

    static QPointer<PrefsDialog> instance;
};

QPointer<PrefsDialog> PrefsDialog::Private::instance;

PrefsDialog::PrefsDialog() :
    PrefsDialogBase(0),
    priv(new Private)
{
    priv->ui.setupUi(this);
    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(priv->ui.radioWorkspace);
    group->addButton(priv->ui.radioWorksheet);
    init();
}

PrefsDialog::~PrefsDialog()
{
    delete priv;
}

void PrefsDialog::showDialog()
{
    NiceDialog::showDialog(Private::instance);
}

void PrefsDialog::execDialog()
{
    showDialog();
    Private::instance->exec();
}


static void setFontEntry(QLineEdit *entry, const QFont &font)
{
    entry->setText(QString("%1, %2").arg(font.family()).arg(font.pointSize()));
    entry->setFont(font);
}

static QString norm_file_path(const QString &p)
{
    return QDir::fromNativeSeparators(p);
}

static QString disp_file_path(const QString &p)
{
    return QDir::toNativeSeparators(p);
}

static QString norm_dir_path(const QString &p)
{
    QString np = QDir::fromNativeSeparators(p);
    if (np.endsWith('/') && np != "/")
        np.chop(1);
    return np;
}

static QString disp_dir_path(const QString &p)
{
    QString np = p;
    if (!np.isEmpty() && !np.endsWith('/'))
        np.append('/');
    return QDir::toNativeSeparators(np);
}

void PrefsDialog::on_buttonUseDefaultGap_stateChanged(int state)
{
    bool enabled = !priv->ui.buttonUseDefaultGap->isEnabled() || state == Qt::Unchecked;
    priv->ui.labelGapDir->setEnabled(enabled);
    priv->ui.labelGapExe->setEnabled(enabled);
    priv->ui.entryGapDir->setEnabled(enabled);
    priv->ui.entryGapExe->setEnabled(enabled);
    priv->ui.buttonGapDir->setEnabled(enabled);
    priv->ui.buttonGapExe->setEnabled(enabled);
}

void PrefsDialog::init()
{
    GapOptions go;

    QString dfltRootDir = go.defaultRootDir();
    QString dfltExe = go.defaultExe();

    priv->ui.buttonUseDefaultGap->setEnabled(!dfltExe.isEmpty());
    priv->ui.buttonUseDefaultGap->setChecked(prefsValue(Prefs::UseDefaultGap));

    priv->gapDir = norm_dir_path(GapOptions().rootDir());
    priv->gapExe = GapOptions().exe();
    priv->gapRoots = prefsValue(Prefs::ExtraGapRoots);
    priv->gapArgs = GapOptions().args();
    if (priv->gapArgs.isEmpty())
        priv->gapArgs = "";

    priv->ui.entryGapDir->setText(disp_dir_path(priv->gapDir));
    priv->ui.entryGapExe->setText(disp_file_path(priv->gapExe));
    priv->ui.entryGapArgs->setText(priv->gapArgs);
    priv->ui.entryGapRoots->setText(priv->gapRoots.join("; "));

    priv->ui.buttonSaveWorkspace->setChecked(prefsValue(Prefs::SaveWorkspace));

    if (prefsValue(Prefs::DefaultFileFormat) == "workspace")
        priv->ui.radioWorkspace->setChecked(true);
    else
        priv->ui.radioWorksheet->setChecked(true);

    priv->worksheetFont = prefsValue(Prefs::WorksheetFont);
    priv->helpFont = prefsValue(Prefs::HelpFont);
    setFontEntry(priv->ui.entryWorksheetFont, priv->worksheetFont);
    setFontEntry(priv->ui.entryHelpFont, priv->helpFont);
}

void PrefsDialog::apply()
{
    setPrefsValue(Prefs::UseDefaultGap, priv->ui.buttonUseDefaultGap->isChecked());

    if (priv->ui.buttonUseDefaultGap->isChecked())
    {
        QString gapDir = norm_dir_path(priv->ui.entryGapDir->text());
        QString gapExe = norm_file_path(priv->ui.entryGapExe->text());

        if (gapDir != priv->gapDir)
            setPrefsValue(Prefs::GapRootDir, gapDir);
        if (gapExe != priv->gapExe)
            setPrefsValue(Prefs::GapExe, gapExe);
    }

    QString gapArgs = priv->ui.entryGapArgs->text();
    if (gapArgs != priv->gapArgs)
        setPrefsValue(Prefs::GapArgs, gapArgs);

    setPrefsValue(Prefs::ExtraGapRoots, priv->gapRoots);
    setPrefsValue(Prefs::SaveWorkspace, priv->ui.buttonSaveWorkspace->isChecked());

    if (priv->ui.radioWorkspace->isChecked())
        setPrefsValue(Prefs::DefaultFileFormat, "workspace");
    else
        setPrefsValue(Prefs::DefaultFileFormat, "worksheet");

    setPrefsValue(Prefs::WorksheetFont, priv->worksheetFont);
    setPrefsValue(Prefs::HelpFont, priv->helpFont);

    WsWindowPrivate::applyPrefs();
    HelpBrowser::applyPrefs();
}

void PrefsDialog::on_buttonGapDir_clicked()
{
    QString oldDir = norm_dir_path(priv->ui.entryGapDir->text());
    QString newDir = getDirName(this, "Choose GAP Directory", oldDir);

    if (newDir.isEmpty())
        return;

    newDir = norm_dir_path(newDir);
    if (newDir == oldDir)
        return;

    priv->ui.entryGapDir->setText(disp_dir_path(newDir));

    QString newExe;
    QDir d(newDir);

#ifdef Q_OS_WIN32
    if (d.exists("bin/gapw95.exe"))
        newExe = d.filePath("bin/gapw95.exe");
#else
    if (d.exists("bin/gap"))
        newExe = d.filePath("bin/gap");
    else if (d.exists("bin/gap.sh"))
        newExe = d.filePath("bin/gap.sh");
#endif

    if (!newExe.isEmpty())
        priv->ui.entryGapExe->setText(disp_file_path(newExe));
}

void PrefsDialog::on_buttonGapExe_clicked()
{
    QString oldExe = norm_file_path(priv->ui.entryGapExe->text());
    QString newExe = getExeFileName(this, "Choose GAP Executable", oldExe);

    if (newExe.isEmpty())
        return;

    newExe = norm_file_path(newExe);
    if (newExe == oldExe)
        return;

    priv->ui.entryGapExe->setText(disp_file_path(newExe));

    QFileInfo fi(newExe);
    if (!fi.exists())
        return;

    {
        QString tmp = fi.symLinkTarget();
        if (!tmp.isEmpty())
            fi = tmp;
    }

    QDir d = fi.dir();
    if (d.exists() && d.dirName().toLower() == "bin")
    {
        d.cdUp();
        priv->ui.entryGapDir->setText(disp_dir_path(d.path()));
    }
}

void PrefsDialog::on_buttonGapRoots_clicked()
{
    EditDirListDialog dlg(priv->gapRoots, this);

    if (dlg.exec())
    {
        QStringList dirs = dlg.dirs();
        qDebug() << dirs;
        priv->gapRoots = dirs;
        priv->ui.entryGapRoots->setText(priv->gapRoots.join(";"));
    }
}

static void font_dialog(PrefsDialog *dlg, QFont *font, QLineEdit *entry)
{
    bool ok;
    QFont newFont = QFontDialog::getFont(&ok, *font, dlg, "Choose Font");
    if (!ok)
        return;
    *font = newFont;
    setFontEntry(entry, newFont);
}

void PrefsDialog::on_buttonWorksheetFont_clicked()
{
    font_dialog(this, &priv->worksheetFont, priv->ui.entryWorksheetFont);
}

void PrefsDialog::on_buttonHelpFont_clicked()
{
    font_dialog(this, &priv->helpFont, priv->ui.entryHelpFont);
}
