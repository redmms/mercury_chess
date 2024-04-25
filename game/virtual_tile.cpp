#include "virtual_tile.h"
#include "virtual_board.h"

namespace mmd
{
    using namespace std;

    VirtualTile::VirtualTile(scoord tile_coord_, char piece_name_, bool piece_color_, VirtualBoard* mother_board_) :
        coord(tile_coord_),
        piece_name(piece_name_),
        piece_color(piece_color_),
        tile_color((coord.x + coord.y) % 2),
        board(mother_board_)
    {}

    void VirtualTile::setPiece(char name, bool color, bool virtually)
    {
        piece_name = name;
        piece_color = color;
        if (name == 'K' && board) {
            if (color) board->white_king = coord;
            else board->black_king = coord;
        }
    }
}