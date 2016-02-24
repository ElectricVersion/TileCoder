/*
 * Lua Tiled Plugin
 * Copyright 2011, Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 *
 * This file is part of Tiled.
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
#include <fstream>
#include "plugin.h"

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

class SPARKMAPSHARED_EXPORT SparkMapPlugin :  public Tiled::Plugin
{
    Q_OBJECT
    Q_INTERFACES(Tiled::Plugin)
    Q_PLUGIN_METADATA(IID "org.mapeditor.MapFormat" FILE "plugin.json")

public:
    void initialize() override;
};

class SPARKMAPSHARED_EXPORT SparkMapFormat : public Tiled::MapFormat
{
    Q_OBJECT
    Q_INTERFACES(Tiled::MapFormat)
public:
    SparkMapFormat(QObject *parent);
    Tiled::Map *read(const QString &fileName) override;
    bool write(const Tiled::Map *map, const QString &fileName) override;
    QString nameFilter() const override;
    QString errorString() const override;

    bool supportsFile(const QString &fileName) const override;
private:
    QString mError;
    uint8_t read_uint8(std::istream & file);
    uint16_t read_uint16(std::istream & file);
    void uint_to_char(char * buffer, uint8_t input);
    void uint_to_char(char * buffer, uint16_t input);
};

} // namespace SparkMap

struct SparkTile
{
    uint id;
    uint idProperty;
};

#endif // SPARKMAPPLUGIN_H
