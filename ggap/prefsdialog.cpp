#include "ggap/prefsdialog.h"
#include "ggap/prefs.h"
#include "ggap/gap.h"
#include "ggap/dialogs.h"
#include "ggap/wswindow-p.h"
#include "ggap/help.h"
#include "ggap/ui_prefs.h"
#include <QFontDialog>
#include <QDir>

using namespace ggap;

struct PrefsDialog::Private {
    Ui::PrefsDialog ui;
    QFont worksheetFont;
    QFont helpFont;

    QString gapDir;
    QString gapExe;
    QString gapArgs;

    static QPointer<PrefsDialog> instance;
};

QPointer<PrefsDialog> PrefsDialog::Private::instance;

PrefsDialog::PrefsDialog() :
    NiceDialog(0),
    priv(new Private)
{
    priv->ui.setupUi(this);
    init();
}

PrefsDialog::~PrefsDialog()
{
    delete priv;
}

void PrefsDialog::showDialog()
{
    if (!Private::instance)
    {
        Private::instance = new PrefsDialog;
        Private::instance->show();
    }

    Private::instance->raise();
    Private::instance->activateWindow();
}

void PrefsDialog::execDialog()
{
    showDialog();
    Private::instance->exec();
}

void PrefsDialog::accept()
{
    QDialog::accept();
    apply();
}


static void setFontLabel(QLabel *label, const QFont &font)
{
    label->setText(QString("%1, %2").arg(font.family()).arg(font.pointSize()));
    label->setFont(font);
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

void PrefsDialog::init()
{
    priv->gapDir = norm_dir_path(GapOptions().rootDir());
    priv->gapExe = GapOptions().exe();
    priv->gapArgs = GapOptions().args();
    if (priv->gapArgs.isEmpty())
        priv->gapArgs = "";

    priv->ui.entryGapDir->setText(disp_dir_path(priv->gapDir));
    priv->ui.entryGapExe->setText(disp_file_path(priv->gapExe));
    priv->ui.entryGapArgs->setText(priv->gapArgs);

    priv->worksheetFont = prefsValue(Prefs::WorksheetFont);
    priv->helpFont = prefsValue(Prefs::HelpFont);
    setFontLabel(priv->ui.labelWorksheetFont, priv->worksheetFont);
    setFontLabel(priv->ui.labelHelpFont, priv->helpFont);
}

void PrefsDialog::apply()
{
    QString gapDir = norm_dir_path(priv->ui.entryGapDir->text());
    QString gapExe = norm_file_path(priv->ui.entryGapExe->text());
    QString gapArgs = priv->ui.entryGapArgs->text();

    if (gapDir != priv->gapDir)
        setPrefsValue(Prefs::GapRootDir, gapDir);
    if (gapExe != priv->gapExe)
        setPrefsValue(Prefs::GapExe, gapExe);
    if (gapArgs != priv->gapArgs)
        setPrefsValue(Prefs::GapArgs, gapArgs);

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

static void font_dialog(PrefsDialog *dlg, QFont *font, QLabel *label)
{
    bool ok;
    QFont newFont = QFontDialog::getFont(&ok, *font, dlg, "Choose font");
    if (!ok)
        return;
    *font = newFont;
    setFontLabel(label, newFont);
}

void PrefsDialog::on_buttonWorksheetFont_clicked()
{
    font_dialog(this, &priv->worksheetFont, priv->ui.labelWorksheetFont);
}

void PrefsDialog::on_buttonHelpFont_clicked()
{
    font_dialog(this, &priv->helpFont, priv->ui.labelHelpFont);
}
