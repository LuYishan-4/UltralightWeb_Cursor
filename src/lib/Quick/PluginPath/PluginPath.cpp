#include "PluginPath.hpp"

#include <QStandardPaths>

namespace KWin
{

std::filesystem::path PluginPath::dataDir()
{
    QString path =
        QStandardPaths::locate(
            QStandardPaths::GenericDataLocation,
            QStringLiteral("kwin/effects/ultralightcursor"),
            QStandardPaths::LocateDirectory
        );

    return path.toStdString();
}

}