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
#include <qdebug.h>
#include <string.h>
#include <boost\endian\buffers.hpp>

using namespace SparkMap;
using namespace Tiled;
using namespace std;

void SparkMapPlugin::initialize()
{
    addObject(new SparkMapFormat(this));
}

SparkMapFormat::SparkMapFormat(QObject *parent)
    : Tiled::MapFormat(parent)
{}


uint8_t SparkMapFormat::read_uint8(istream & file)
{
    char tempchar[1];
    file.read(tempchar, 1);
    uint8_t output = *(reinterpret_cast<uint8_t *>(tempchar));
    return output;
}

uint16_t SparkMapFormat::read_uint16(istream & file)
{
    char tempchars[2];
    file.read(tempchars, 2);
    void * outputPointer = tempchars;
    uint16_t output = *(reinterpret_cast<uint16_t *>(outputPointer));
    return output;
}

Map * SparkMapFormat::read(const QString &fileName)
{
    fstream file(fileName.toStdString().c_str(), fstream::in|fstream::binary);
    file.seekg(0);
    char fileHeader[8];
    file.read(fileHeader, 8);
    uint8_t w = 1;
    uint8_t h = 1;
    Map * output = nullptr;
    if (memcmp(fileHeader, "MAP_V02", 8)==0)
    {
        char wHeader[2];
        file.read(wHeader, 2);
        w = read_uint8(file);
        char hHeader[2];
        file.read(hHeader, 2);
        h = read_uint8(file);
        output = new Map(Map::Orthogonal, w, h, 32, 32);
        // start parsing layers
        while ( (file.rdstate() & std::ifstream::eofbit ) != 0 )
        {
            //search for a layer
            char layerHeader[4];
            file.read(layerHeader, 4);
            if (memcmp(layerHeader, "LYR", 4))
            {
                TileLayer * layer = new TileLayer(QString("Loaded Layer"), 0, 0, w, h);
                for (unsigned int l=0; ((l<(w*h)) && (file.rdstate() & std::ifstream::eofbit )) != 0; l++)
                {
                    read_uint16(file);
                }
                output->addLayer(layer);
            }
        }
    }
    file.close();
    return output;
}

bool SparkMapFormat::write(const Map *map, const QString &fileName)
{
    fstream file(fileName.toStdString().c_str(), fstream::out|fstream::binary);

    // write the header
    file.write("MAP_V02", 8);
    uint8_t width = map->width();
    uint8_t height = map->height();
    width=boost::endian::native_to_big(width);
    height=boost::endian::native_to_big(height);

    file.write("W", 2);
    file.write(reinterpret_cast<const char *>(&width), sizeof(uint8_t));
    file.write("H", 2);
    file.write(reinterpret_cast<const char *>(&height), sizeof(uint8_t));
    // Traverse all tile layers
    foreach (const Layer *layer, map->layers())
    {
        if (layer->layerType() != Layer::TileLayerType)
            continue;

        file.write("LYR", 4);
        const TileLayer *tileLayer = static_cast<const TileLayer*>(layer);

        bool usingCustomIds=true;
        const QString idproperty = "tileID";


        // make sure everything has the custom id property
        for (int y = 0; y < tileLayer->height(); ++y)
        {
            for (int x = 0; x < tileLayer->width(); ++x)
            {
                const Cell &cell = tileLayer->cellAt(x, y);
                if (cell.isEmpty()==false)
                {
                    const Tile *tile = cell.tile;
                    if (tile->hasProperty(idproperty))
                    {
                        if (!((tile->property(idproperty).toUInt())>0)) // if the id property cant be converted to an uint or eq zero
                        {
                            usingCustomIds=false;
                        }
                    }
                    else
                    {
                        usingCustomIds=false;
                    }
                }
            }
        }

        // now write it
        for (int y = 0; y < tileLayer->height(); ++y)
        {
            for (int x = 0; x < tileLayer->width(); ++x)
            {
                const Cell &cell = tileLayer->cellAt(x, y);
                uint16_t id=0;
                if (cell.isEmpty()==false)
                {
                    const Tile *tile = cell.tile;
                    if (usingCustomIds)
                    {
                        id = (tile->property(idproperty).toUInt());
                    }
                    else
                    {
                        id = (tile->id())+1;
                    }
                }
                id = boost::endian::native_to_big(id);
                file.write(reinterpret_cast<const char *>(&id), sizeof(uint16_t));
            }
        }
    }
    file.close();
    return true;
}

QString SparkMapFormat::nameFilter() const
{
    return tr("SparkMap files (*.map)");
}

QString SparkMapFormat::errorString() const
{
    return mError;
}

bool SparkMapFormat::supportsFile(const QString &fileName) const
{
    return fileName.endsWith(QLatin1String(".map"), Qt::CaseInsensitive);
}
