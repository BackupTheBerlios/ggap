#ifndef GGAP_UTILS_H
#define GGAP_UTILS_H

#include <QString>
#include <QByteArray>
#include <QFile>

namespace ggap {
namespace util {

QByteArray getFileContent(const QString &path, QString *error = 0);
QString getFileText(const QString &path, QString *error = 0);
bool saveFile(const QString &path, const QString &content, QString *error = 0);
bool mkpath(const QString &path);
bool mkdir(const QString &path);

QString findProgramInPath(const QString &program);

template<typename T>
class DataOnDemand {
    mutable T* p;
    T*(*factory)();
    void check() { if (!p) p = factory ? factory() : new T; }
public:
    DataOnDemand(T*(*factory)() = 0) : p(0), factory(factory) {}
    ~DataOnDemand() { delete p; }
    operator T* () { check(); return p; }
    operator const T* () const { check(); return p; }
    operator T& () { check(); return *p; }
    operator const T& () const { check(); return *p; }
    T *operator -> () { check(); return p; }
    const T *operator -> () const { check(); return p; }
    operator bool() const { return p != 0; }
};

}
}

#endif // GGAP_UTILS_H
