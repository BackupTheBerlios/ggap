#ifndef GGAP_PREFS_DIALOG_P_H
#define GGAP_PREFS_DIALOG_P_H

#include <ggap/ui_editdirlist.h>
#include <moo-macros.h>
#include <QDialog>
#include <QStringList>
#include <QStringListModel>

namespace ggap {

class EditDirListDialog : public QDialog, private Ui::EditDirListDialog {
    Q_OBJECT

    QStringListModel model;

public:
    EditDirListDialog(const QStringList &dirs, QWidget *parent) :
        QDialog(parent),
        model(dirs)
    {
        setupUi(this);
        dirList->setModel(&model);
        if (!dirs.isEmpty())
            selectRow(dirs.size() - 1);
        updateSensitivity();
        connect(dirList->selectionModel(),
                SIGNAL(currentRowChanged(QModelIndex, QModelIndex)),
                SLOT(updateSensitivity()));
    }

    QStringList dirs() const
    {
        return model.stringList();
    }

private:
    void selectRow(const QModelIndex &idx)
    {
        dirList->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::ClearAndSelect);
    }

    void selectRow(int row)
    {
        dirList->selectionModel()->setCurrentIndex(model.index(row), QItemSelectionModel::ClearAndSelect);
    }

    QModelIndex selectedRow() const
    {
        return dirList->selectionModel()->currentIndex();
    }

    QString getDir(const QString &start = QString())
    {
        return QFileDialog::getExistingDirectory(this, "Choose Directory", start);
    }

    void editIndex(const QModelIndex &idx)
    {
        m_return_if_fail(idx.isValid());
        QString path = model.data(idx, Qt::DisplayRole).toString();
        path = getDir(path);
        if (!path.isEmpty())
            model.setData(idx, path);
    }

private Q_SLOTS:
    void updateSensitivity()
    {
        QModelIndex idx = selectedRow();
        buttonRemove->setEnabled(idx.isValid());
        buttonOpen->setEnabled(idx.isValid());
        buttonUp->setEnabled(idx.isValid() && idx.row() > 0);
        buttonDown->setEnabled(idx.isValid() && idx.row() + 1 < model.rowCount());
    }

    void on_dirList_activated(const QModelIndex &idx)
    {
        editIndex(idx);
    }

    void on_buttonOpen_clicked()
    {
        QModelIndex idx = selectedRow();
        m_return_if_fail(idx.isValid());
        editIndex(idx);
    }

    void on_buttonAdd_clicked()
    {
        QString path = getDir();
        if (path.isEmpty())
            return;

        int row = model.rowCount();
        QModelIndex idx = selectedRow();
        if (idx.isValid())
            row = idx.row() + 1;
        model.insertRows(row, 1);
        idx = model.index(row);
        model.setData(idx, path);
        selectRow(idx);

        updateSensitivity();
    }

    void on_buttonRemove_clicked()
    {
        QModelIndex idx = selectedRow();
        m_return_if_fail(idx.isValid());
        model.removeRows(idx.row(), 1);
        updateSensitivity();
    }

    void on_buttonUp_clicked()
    {
        QModelIndex idx = selectedRow();
        if (idx.isValid() && idx.row() != 0)
        {
            int row = idx.row();
            QString path = model.data(idx, Qt::DisplayRole).toString();
            model.removeRows(row, 1);
            model.insertRows(row - 1, 1);
            idx = model.index(row - 1);
            model.setData(idx, path);
            selectRow(idx);
        }
    }

    void on_buttonDown_clicked()
    {
        QModelIndex idx = selectedRow();
        if (idx.isValid() && idx.row() != model.rowCount() - 1)
        {
            int row = idx.row();
            QString path = model.data(idx, Qt::DisplayRole).toString();
            model.removeRows(row, 1);
            model.insertRows(row + 1, 1);
            idx = model.index(row + 1);
            model.setData(idx, path);
            selectRow(idx);
        }
    }
};

}

#endif // GGAP_PREFS_DIALOG_P_H
