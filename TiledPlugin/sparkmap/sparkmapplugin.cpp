/*
 * Sparkmap Tiled Plugin
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

#include "sparkmapplugin.h"

#include "imagelayer.h"
#include "mapobject.h"
#include "objectgroup.h"
#include "properties.h"
#include "terrain.h"
#include "tile.h"
#include "tilelayer.h"
#include "tileset.h"

#include <QFile>
#include <QCoreApplication>
#include <QSaveFile>
#include <iostream>
#include <cstdint>
#include <deque>
#include <fstream>
#include <boost\endian\buffers.hpp>

using namespace SparkMap;
using namespace Tiled;
using namespace std;

SparkMapPlugin::SparkMapPlugin()
{
}

bool SparkMapPlugin::write(const Map *map, const QString &fileName)
{
    fstream file(fileName.toStdString().c_str(), fstream::out|fstream::binary);

    // write the header
    file.write("MAP_V01", 8);
    // Traverse all tile layers
    foreach (const Layer *layer, map->layers())
    {
        if (layer->layerType() != Layer::TileLayerType)
            continue;

        file.write("LYR", 4);
        const TileLayer *tileLayer = static_cast<const TileLayer*>(layer);

        // Write out tiles either by ID or their name, if given. -1 is "empty"
        for (int y = 0; y < tileLayer->height(); ++y)
        {
            for (int x = 0; x < tileLayer->width(); ++x)
            {
                const Cell &cell = tileLayer->cellAt(x, y);
                uint16_t id=0;
                if (cell.isEmpty()==false)
                {
                    const Tile *tile = cell.tile;
                    id = (tile->id())+1;
                }
                id = boost::endian::native_to_big(id);
                file.write(reinterpret_cast<const char *>(&id), sizeof(uint16_t));
            }
        }
    }
    return true;
}

QString SparkMapPlugin::nameFilter() const
{
    return tr("SparkMap files (*.map)");
}

QString SparkMapPlugin::errorString() const
{
    return mError;
}

