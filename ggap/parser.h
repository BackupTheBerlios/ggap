#ifndef GAP_PARSER_H
#define GAP_PARSER_H

#include <QStringList>

namespace ggap {
namespace parser {

struct ErrorInfo {
    int line;
    int firstColumn;
    int lastColumn;
    QString message;
};

enum Result {
    Success,
    Incomplete,
    Error
};

struct Text {
    QStringList original;
    QStringList lines;
    QString mangled;
};

Result parse(const QStringList &input, Text *output, QList<ErrorInfo> *errors);

} // namespace parser
} // namespace ggap

#endif // GAP_PARSER_H
