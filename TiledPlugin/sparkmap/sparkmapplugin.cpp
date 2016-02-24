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
    uint8_t output = *(static_cast<uint8_t *>(static_cast<void*>(tempchar)));
    return output;
}

uint16_t SparkMapFormat::read_uint16(istream & file)
{
    char tempchars[2];
    file.read(tempchars, 2);
    uint16_t output = *(static_cast<uint16_t *>(static_cast<void*>(tempchars)));;
    return output;
}

Map * SparkMapFormat::read(const QString &fileName)
{
    /*string rawFileName = fileName.toStdString().substr(0, fileName.toStdString().find_last_of("."));
    string tilesetFilename = rawFileName + ".tileset";
    fstream tilesetFile(tilesetFilename.c_str(), fstream::in);
    while (tilesetFile.good())
    {
        char buffer[256];
        tilesetFile.getline(buffer, 256);
        if (tilesetFile.good())
        {
            string bufstr = string(buffer);
            if (bufstr.compare(0,8, "TILESET ")==0)
            {
                SharedTileset sharedtileset = Tileset::create("New Tileset", 32, 32, 0);
                int first_quote = bufstr.find("\"");
                int last_quote = bufstr.find("\"", first_quote+1);
                string name
            }
        }
    }*/

    /*fstream file(fileName.toStdString().c_str(), fstream::in|fstream::binary);
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

        SharedTileset sharedtileset = Tileset::create("testing tileset", 32, 32, 0);
        sharedtileset->loadFromImage("C:/Users/Nick/Desktop/Apples/tileset.png");
        output->addTileset(sharedtileset);
        // start parsing layers
        while ( file.good() )
        {
            //search for a layer
            char layerHeader[4];
            file.read(layerHeader, 4);
            if (memcmp(layerHeader, "LYR", 4)==0)
            {
                TileLayer * layer = new TileLayer(QString("Loaded Layer"), 0, 0, w, h);
                bool keepreading=true;
                int position = file.tellg();
                unsigned int iw=0;
                unsigned int ih=0;
                while (keepreading==true && file.good())
                {
                    char tempchars[4];
                    file.read(tempchars, 4);
                    if (memcmp(tempchars, "LYR", 4)==0)
                    {
                        keepreading=false;
                        position=file.tellg();
                    }
                    else
                    {
                        file.seekg(position);
                    }
                    if (file.eof())
                    {
                        keepreading=false;
                    }
                    if (keepreading)
                    {
                        uint16_t tileId = read_uint16(file);
                        layer->setCell(iw, ih, Cell(sharedtileset->tileAt(tileId)));
                        position = file.tellg();
                        iw++;
                        if (iw >= w)
                        {
                            iw = 0;
                            ih++;
                        }
                        qDebug() << iw;
                        qDebug() << ih;
                        qDebug() << "Done";
                    }
                }
                output->addLayer(layer);
            }
        }
    }
    file.close();
    */
    Map * output = new Map(Map::Orthogonal, 1, 1, 32, 32);
    return output;
}

void SparkMapFormat::uint_to_char(char * buffer, uint8_t input)
{
    input = boost::endian::native_to_big(input);
    void * input_pointer = &input;
    char * output = static_cast<char *>(input_pointer);
    buffer[0] = output[0];
}

void SparkMapFormat::uint_to_char(char * buffer, uint16_t input)
{
    input = boost::endian::native_to_big(input);
    void * input_pointer = &input;
    char * output = static_cast<char *>(input_pointer);
    buffer[0] = output[0];
    buffer[1] = output[1];
}

bool SparkMapFormat::write(const Map *map, const QString &fileName)
{
    // write the header
    const char fileVersion_code = 0x00;
    const char mapWidth_code = 0x01;
    const char mapHeight_code = 0x02;
    const char beginMap_code = 0x03;
    const char beginLayer_code = 0x04;
    const char tile_code = 0x05;
    const char endLayer_code = 0x06;
    const char endMap_code = 0x07;

    const char fileVersion = 3;

    fstream mapfile(fileName.toStdString().c_str(), fstream::out|fstream::binary);
    // write the header and version number
    mapfile.write(&fileVersion_code, 1);
    mapfile.write(&fileVersion, 1);

    // get the map's dimensions
    uint8_t width = map->width();
    uint8_t height = map->height();
    width=boost::endian::native_to_big(width);
    height=boost::endian::native_to_big(height);

    // convert them to chars so we can write them
    char widthChar[1];
    char heightChar[1];
    uint_to_char(widthChar, width);
    uint_to_char(heightChar, height);

    // now write the dimensions to the file
    mapfile.write(&mapWidth_code, 1);
    mapfile.write(widthChar, 1);
    mapfile.write(&mapHeight_code, 1);
    mapfile.write(heightChar, 1);

    // indicate that the map is starting
    mapfile.write(&beginMap_code, 1);

    // Traverse all tile layers
    foreach (const Layer *layer, map->layers())
    {
        if (layer->layerType() != Layer::TileLayerType)
        {
            continue;
        }
        else
        {
            mapfile.write(&beginLayer_code, 1);
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
                    char id_char[2];
                    uint_to_char(id_char, id);
                    mapfile.write(&tile_code, 1);
                    mapfile.write(id_char, 2);
                }
            }
            mapfile.write(&endLayer_code, 1);
        }
    }

    // indicate that the map is finished
    mapfile.write(&endMap_code, 1);

    mapfile.close();
    return true;
}

QString SparkMapFormat::nameFilter() const
{
    return tr("Apples Map files (*.map)");
}

QString SparkMapFormat::errorString() const
{
    return mError;
}

bool SparkMapFormat::supportsFile(const QString &fileName) const
{
    return fileName.endsWith(QLatin1String(".map"), Qt::CaseInsensitive);
}
