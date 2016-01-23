/*
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SPARKMAPPLUGIN_H
#define SPARKMAPPLUGIN_H

#include "sparkmap_global.h"

#include "gidmapper.h"
#include "map.h"
#include "mapformat.h"

#include <QDir>
#include <QObject>

namespace Tiled {
class MapObject;
class ObjectGroup;
class Properties;
class TileLayer;
class Tileset;
}

namespace SparkMap {

class SPARKMAPSHARED_EXPORT SparkMapPlugin : public Tiled::WritableMapFormat
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.mapeditor.MapFormat" FILE "plugin.json")

public:
    SparkMapPlugin();

    //Tiled::Map *read(const QString &fileName) override;
    bool write(const Tiled::Map *map, const QString &fileName) override;
    QString nameFilter() const override;
    QString errorString() const override;

private:
    QString mError;
};

} // namespace SparkMap

#endif // SPARKMAPPLUGIN_H
