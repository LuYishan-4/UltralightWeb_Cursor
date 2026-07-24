#include "PluginPath.hpp"

#include <QStandardPaths>

namespace KWin
{

std::filesystem::path PluginPath::dataDir()
{
    QString path =
        QStandardPaths::writableLocation(
            QStandardPaths::GenericDataLocation
        );

    path += QStringLiteral(
        "/kwin/effects/ultralightwebcursor"
        );

    return path.toStdString();
}

}