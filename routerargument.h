#ifndef ROUTERARGUMENT_H
#define ROUTERARGUMENT_H

#include <QString>


class RouterArgument
{
public:
    RouterArgument(const QString argName, const QString argValue);
    QString getArgName() const;
    QString getArgValue() const;

private:
    const QString argName;
    const QString argValue;
};

#endif // ROUTERARGUMENT_H
