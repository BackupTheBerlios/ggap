#include "moows/colorcombobox.h"
#include <QColorDialog>
#include <QPainter>
#include <QFontMetrics>

using namespace moo::ws;

ColorComboBox::ColorComboBox(QWidget *parent) :
    QComboBox(parent),
    n_custom_colors(0),
    custom_enabled(false),
    show_names(true)
{
    connect(this, SIGNAL(activated(int)), SLOT(activated(int)));
}

void ColorComboBox::setColorDialogEnabled(bool enabled)
{
    if (custom_enabled == enabled)
	return;

    custom_enabled = enabled;

    if (enabled)
	addItem("Other...");
    else
        removeItem(colorCount());
}

bool ColorComboBox::colorDialogEnabled() const
{
    return custom_enabled;
}


const int pix_size = 12;

void ColorComboBox::insertColor(int index, const QColor &color, const QString &name)
{
    QPixmap pix(pix_size, pix_size);
    QPainter painter(&pix);

    if (color.isValid())
    {
	painter.setPen(Qt::gray);
	painter.setBrush(QBrush(color));
	painter.drawRect(0, 0, pix_size, pix_size);
    }

    QIcon icon;
    icon.addPixmap(pix);

    if (custom_enabled && index > colorCount())
        index = colorCount() - 1;

    insertItem(index, icon, name, color);
}

int ColorComboBox::colorCount() const
{
    return QComboBox::count() - (custom_enabled ? 1 : 0);
}

QColor ColorComboBox::currentColor() const
{
    return color(currentIndex());
}

void ColorComboBox::setCurrentColor(const QColor &newColor)
{
    for (int i = 0; i < colorCount(); i++)
    {
	if (color(i) == newColor)
        {
	    setCurrentIndex(i);
	    break;
	}
    }
}

QColor ColorComboBox::color(int index) const
{
    if (index == colorCount())
	return QColor();
    else
        return qVariantValue<QColor>(itemData(index));
}

void ColorComboBox::activated(int index)
{
    QColor newColor;

    if (custom_enabled && index == colorCount())
    {
	newColor = QColorDialog::getColor();

        if (newColor.isValid())
        {
            n_custom_colors += 1;
	    addColor(newColor, QString("Custom %1").arg(n_custom_colors));
	    setCurrentIndex(index);
	}
        else
        {
	    setCurrentColor(last);
	    newColor = last;
	}
    }
    else
    {
        newColor = color(index);
    }

    if (newColor != last)
    {
        last = newColor;
        emit activated(newColor);
    }
}

QSize ColorComboBox::sizeHint() const
{
    QFontMetrics metrics = fontMetrics();
    QStyleOptionComboBox option;
    option.init(this);
    return style()->sizeFromContents(QStyle::CT_ComboBox, &option,
                                     QSize(metrics.width(tr("Custom 99")) + pix_size + 8, metrics.height() + 4),
                                     this);
}

void ColorComboBox::setStandardColors()
{
    addColor(Qt::black, "Black");
    addColor(Qt::white, "White");
    addColor(Qt::red, "Red");
    addColor(Qt::darkRed, "Dark red");
    addColor(Qt::green, "Green");
    addColor(Qt::darkGreen, "Dark green");
    addColor(Qt::blue, "Blue");
    addColor(Qt::darkBlue, "Dark blue");
    addColor(Qt::cyan, "Cyan");
    addColor(Qt::darkCyan, "Dark cyan");
    addColor(Qt::magenta, "Magenta");
    addColor(Qt::darkMagenta, "Dark magenta");
    addColor(Qt::yellow, "Yellow");
    addColor(Qt::darkYellow, "Dark yellow");
    addColor(Qt::gray, "Gray");
    addColor(Qt::darkGray, "Dark gray");
    addColor(Qt::lightGray, "Light gray");
}
