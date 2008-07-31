#ifndef MOO_WS_COLOR_COMBO_BOX_H
#define MOO_WS_COLOR_COMBO_BOX_H

#include <QComboBox>
#include <QColor>

namespace moo {
namespace ws {

class ColorComboBox : public QComboBox {
    Q_OBJECT

public:
    ColorComboBox(QWidget *parent = 0);

    void insertColor(int index, const QColor &color, const QString &name);
    void addColor(const QColor &color, const QString &name) { insertColor(colorCount(), color, name); }

    int colorCount() const;
    QColor color(int index) const;

    void setCurrentColor(const QColor &color);
    QColor currentColor() const;

    void setColorDialogEnabled(bool enabled);
    bool colorDialogEnabled() const;

    void setColorNamesEnabled(bool show);
    bool colorNamesEnabled() const;

    void setStandardColors();

protected:
    virtual QSize sizeHint() const;

Q_SIGNALS:
    void activated(const QColor &color);

private Q_SLOTS:
    void activated(int index);

private:
    QColor last;
    int n_custom_colors;
    bool custom_enabled;
    bool show_names;
};

} // namespace ws
} // namespace moo

#endif // MOO_WS_COLOR_COMBO_BOX_H
