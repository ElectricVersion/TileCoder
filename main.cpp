#include <iostream>
#include <cstdint>
#include <deque>
#include <fstream>
#include <boost/endian/buffers.hpp>

using namespace std;

struct Tile
{
    uint16_t type;

    Tile(uint16_t new_type) : type(new_type) { };
};

struct Map;

struct Layer
{
    static char header[4];
    deque<Tile> tiles;
    Layer(Map & parentmap, int index, uint16_t filler = 0);
};

char Layer::header[4]="LYR";

struct Map
{
    static char header[8];
    uint8_t width;
    uint8_t height;
    Layer * layers[4];

    Map(uint8_t w, uint8_t h) : width(w), height(h) { };
};

char Map::header[8]="MAP_V01";

Layer::Layer(Map & parentmap, int index, uint16_t filler)
{
    int area = parentmap.width * parentmap.height;
    tiles.assign(area, Tile(filler));
    parentmap.layers[index] = this;
}

int main()
{

    Map examplemap(7, 9);
    Layer layer0(examplemap, 0, 1);
    Layer layer1(examplemap, 1, 2);
    Layer layer2(examplemap, 2);
    Layer layer3(examplemap, 3);
    fstream file("example.map", fstream::out|fstream::binary);
    if (file.is_open())
    {
        file.write(examplemap.header, 8);
        for (int l=0; l<4; l++)
        {
            Layer * the_layer = examplemap.layers[l];
            file.write(the_layer->header, 4);
            for (unsigned int t=0; t<the_layer->tiles.size(); t++)
            {
                uint16_t the_int = boost::endian::native_to_big(the_layer->tiles[t].type);
                file.write(reinterpret_cast<const char *>(&the_int), sizeof(uint16_t));
            }
        }

    }
    return 0;
}
