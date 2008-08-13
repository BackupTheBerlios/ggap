#ifndef GGAP_OPTS_H
#define GGAP_OPTS_H

#include <QString>
#include <QStringList>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

namespace ggap {

class Option {
public:
    enum Flag {
        None      = 0,

        IsQt      = 1 << 0,

        NeedValue = 1 << 1,
        HasValue  = 1 << 2,

        NoHelp    = 1 << 3,
        NoHelpAll = 1 << 4,

        Dash      = 1 << 5,
        NoEqual   = 1 << 6
    };

    Q_DECLARE_FLAGS(Flags, Flag)

private:
    QString name_;
    QString value_;
    QString description_;
    Flags flags_;

    QString *svalue;
    bool *bvalue;

    uint count;
    QStringList values;

public:
    Option(const QString &name, const QString &value, const QString &description,
           Flags flags, QString *sptr, bool *bptr) :
        name_(name), value_(value), description_(description), flags_(flags),
        svalue(sptr), bvalue(bptr), count(0)
    {
        if (name_.startsWith('='))
            name_ = name_.mid(1);
        else if (flags_.testFlag(IsQt))
            flags_ |= NoEqual;

        if (!value.isEmpty())
            flags_ |= HasValue;
    }

    QString name() const
    {
        return name_;
    }

    QString formatName(bool format_value = true) const
    {
        QString prefix = "--";
        if (flags_.testFlag(Dash))
            prefix = "-";

        QString n = prefix + name_;

        if (format_value)
        {
            QString val_format;

            if (flags_.testFlag(NeedValue))
            {
                if (flags_.testFlag(NoEqual))
                    val_format = " %1";
                else
                    val_format = "=%1";
            }
            else if (flags_.testFlag(HasValue))
            {
                if (flags_.testFlag(NoEqual))
                    val_format = " [%1]";
                else
                    val_format = "[=%1]";
            }

            if (!val_format.isEmpty())
                n += val_format.arg(value_);
        }

        return n;
    }

    QString formatDescription() const
    {
        return description_;
    }

    bool isQt() const
    {
        return flags_.testFlag(IsQt);
    }

    bool needValue() const
    {
        return flags_.testFlag(NeedValue);
    }

    bool showInHelp() const
    {
        return !flags_.testFlag(NoHelp) && !flags_.testFlag(NoHelpAll);
    }

    bool showInHelpAll() const
    {
        return !flags_.testFlag(NoHelpAll);
    }

    void use()
    {
        count++;
    }

    bool addValue(const QString &value, QString *errors)
    {
        if (!flags_.testFlag(HasValue))
        {
            QString n = formatName(false);
            *errors = QString("Option %1 does not take a value\n").arg(n);
            return false;
        }

        values << value;
        return true;
    }

    void writeValue() const
    {
        if (svalue && !values.isEmpty())
            *svalue = values.last();
        else if (bvalue && count > 0)
            *bvalue = true;
    }
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Option::Flags)

class OptParser {
    QList<Option> options;
    QStringList positional;
    QString appName;

    void addQt(const char *option, const char *value, const char *description,
               bool showInHelp = false)
    {
        Option::Flags flags = Option::IsQt;
        if (value)
            flags |= Option::HasValue | Option::NeedValue;
        if (!showInHelp)
            flags |= Option::NoHelp;
        options << Option(option, value ? value : QString(), description, flags, 0, 0);
    }

    void addQt(const char *option, const char *description)
    {
        addQt(option, NULL, description);
    }

    Option *findOption(const QString &name)
    {
        for (QList<Option>::iterator i = options.begin(); i != options.end(); ++i)
            if (i->name() == name)
                return &*i;
        return 0;
    }

    void showHelp(bool all)
    {
        QStringList output;

        for (QList<Option>::const_iterator i = options.constBegin(); i != options.constEnd(); ++i)
        {
            if ((!all && !i->showInHelp()) || !i->showInHelpAll())
                continue;
            output << i->formatName() << i->formatDescription();
        }

        if (all)
            output << "-h, -help, --help" << "show common options"
                   << "--help-all" << "show this help";
        else
            output << "-h, -help, --help" << "show this help"
                   << "--help-all" << "show all options";

        int max_len = 0;
        for (int i = 0; i < output.size(); i += 2)
            max_len = qMax(max_len, output.at(i).length());

        std::cout << "Usage: " << qPrintable(appName) << " [OPTIONS] [FILES]\n";
        std::cout << "Available options:\n";

        for (int i = 0; i < output.size(); i += 2)
        {
            std::cout << "  "
                      << qPrintable(output.at(i))
                      << std::setw(max_len + 2 - output.at(i).length())
                      << " "
                      << qPrintable(output.at(i + 1))
                      << "\n";
        }

        ::exit(EXIT_SUCCESS);
    }

public:
    OptParser(const QString &appName) :
        appName(appName)
    {
        // -style= style, sets the application GUI style. Possible values are motif, windows, and platinum.
        // If you compiled Qt with additional styles or have additional styles as plugins these will be
        // available to the -style command line option.
        // -style style, is the same as listed above.
        addQt("=style", "style", "set the application GUI style");
        // -stylesheet= stylesheet, sets the application styleSheet. The value must be a path to a file
        // that contains the Style Sheet. Note that relative URLs in the Style Sheet file are relative
        // to the Style Sheet file's path.
        // -stylesheet stylesheet, is the same as listed above.
        addQt("=stylesheet", "stylesheet", "set the application style sheet");
        // -session= session, restores the application from an earlier session.
        // -session session, is the same as listed above.
        addQt("=session", "session", "restore the application from an earlier session");
        // -widgetcount, prints debug message at the end about number of widgets left undestroyed and
        // maximum number of widgets existed at the same time
        addQt("widgetcount", "prints debug message at the end about number of widgets left undestroyed "
                                "and maximum number of widgets existed at the same time");
        // -reverse, sets the application's layout direction to Qt::RightToLeft
        addQt("reverse", "set the application's layout direction to Right To Left");

#ifdef Q_WS_X11
        // -display display, sets the X display (default is $DISPLAY).
        addQt("display", "display", "set the X display (default is $DISPLAY)", true);
        // -geometry geometry, sets the client geometry of the first window that is shown.
        addQt("geometry", "geometry", "set the client geometry of the first window that is shown");
        // -fn or -font font, defines the application font. The font should be specified using an X logical font description.
        addQt("font", "font", "define the application font");
        addQt("fn", "font", "same as -font");
        // -bg or -background color, sets the default background color and an application palette (light and dark shades are calculated).
        addQt("background", "color", "set the default background color and an application palette");
        addQt("bg", "color", "same as -background");
        // -fg or -foreground color, sets the default foreground color.
        addQt("foreground", "color", "set the default foreground color");
        addQt("fg", "color", "same as -foreground");
        // -btn or -button color, sets the default button color.
        addQt("button", "color", "set the default button color");
        addQt("btn", "color", "same as -button");
        // -name name, sets the application name.
        addQt("name", "name", "set the application name");
        // -title title, sets the application title.
        addQt("title", "title", "set the application title");
        // -visual TrueColor, forces the application to use a TrueColor visual on an 8-bit display.
        addQt("visual", "TrueColor", "force the application to use a TrueColor visual");
        // -ncols count, limits the number of colors allocated in the color cube on an 8-bit display, if the application is using the QApplication::ManyColor color specification. If count is 216 then a 6x6x6 color cube is used (i.e. 6 levels of red, 6 of green, and 6 of blue); for other values, a cube approximately proportional to a 2x3x1 cube is used.
        addQt("ncols", "count", "limits the number of colors allocated in the color cube on an 8-bit display");
        // -cmap, causes the application to install a private color map on an 8-bit display.
        addQt("cmap", "install a private color map on an 8-bit display");
        // -im, sets the input method server (equivalent to setting the XMODIFIERS environment variable)
        addQt("im", "set the input method server");
        // -noxim, disables the input method framework ("no X input method").
        addQt("noxim", "disable the input method framework (\"no X input method\")");
        // -inputstyle, defines how the input is inserted into the given widget. E.g., onTheSpot makes the input appear directly in the widget, while overTheSpot makes the input appear in a box floating over the widget and is not inserted until the editing is done.
        addQt("inputstyle", "inputstyle", "define how the input is inserted into the given widget");
#endif
    }

    void add(const char *option, const char *description)
    {
        options << Option(option, QString(), description, 0, 0, 0);
    }

    void add(const char *option, const char *description, bool *value)
    {
        options << Option(option, QString(), description, 0, 0, value);
    }

    void add(const char *option, const char *value, const char *description,
             QString *value_p, bool value_required = true)
    {
        options << Option(option, value, description,
                          value_required ? Option::NeedValue : Option::None,
                          value_p, 0);
    }

    QStringList files() const
    {
        return positional;
    }

    void parse(int &argc, char *argv[])
    {
        QStringList args;
        QList<int> ret_args;
        QString errors;

        for (int i = 0; i < argc; ++i)
            args << QString::fromLocal8Bit(argv[i]);

        if (!parse(args, &ret_args, &errors))
        {
            std::cerr << qPrintable(errors);
            ::exit(EXIT_FAILURE);
        }

        argc = ret_args.size();
        QList<char*> ret_argv;
        for (int i = 0; i < argc; ++i)
            ret_argv << argv[ret_args.at(i)];
        for (int i = 0; i < argc; ++i)
            argv[i] = ret_argv[i];
        argv[argc] = 0;
    }

    bool parse(const QStringList &args, QList<int> *ret_args_p, QString *errors)
    {
        bool dash_dash = false;
        Option *need_arg = 0;
        QString need_arg_string;
        QList<int> ret_args;

        ret_args << 0;

        for (int i = 1; i < args.size(); ++i)
        {
            QString arg = args.at(i);

            if (dash_dash)
            {
                positional << arg;
                continue;
            }

            if (!arg.startsWith('-'))
            {
                if (need_arg)
                {
                    if (!need_arg->addValue(arg, errors))
                        return false;
                    if (need_arg->isQt())
                        ret_args << i;
                    need_arg = 0;
                    need_arg_string = QString();
                }
                else
                {
                    positional << arg;
                }

                continue;
            }

            if (arg == "--")
            {
                dash_dash = true;
                continue;
            }

            if (arg == "-h" || arg == "-help" || arg == "--help" || arg == "-?")
                showHelp(false);
            if (arg == "--help-all")
                showHelp(true);

            int optstart = arg.startsWith("--") ? 2 : 1;
            int eq = arg.indexOf('=', 1);
            QString opt = arg.mid(optstart, eq - optstart);
            QString val = eq >= 0 ? arg.mid(eq + 1) : QString();

            Option *option = findOption(opt);
            if (!option)
            {
                *errors = QString("Invalid option '%1'\n").arg(opt);
                return false;
            }

            option->use();

            if (!val.isNull())
            {
                if (!option->addValue(val, errors))
                    return false;
            }
            else if (option->needValue())
            {
                need_arg = option;
                need_arg_string = arg;
            }

            if (option->isQt())
                ret_args << i;
        }

        if (need_arg)
        {
            *errors = QString("Missing value for option '%1'\n").arg(need_arg_string);
            return false;
        }

        for (QList<Option>::iterator i = options.begin(); i != options.end(); ++i)
            i->writeValue();

        *ret_args_p = ret_args;
        return true;
    }
};

}

#endif // GGAP_OPTS_H
