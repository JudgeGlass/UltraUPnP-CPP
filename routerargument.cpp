#include "routerargument.h"

RouterArgument::RouterArgument(const QString argName, const QString argValue)
    : argName(argName), argValue(argValue)
{}

QString RouterArgument::getArgName() const{
    return argName;
}

QString RouterArgument::getArgValue() const{
    return argValue;
}
