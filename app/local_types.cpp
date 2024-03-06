#include "local_types.h"

std::map<char, promnum> promo_by_char{
    {'N', to_knight},
    {'B', to_bishop},
    {'R', to_castle},
    {'Q', to_queen},
    {'e', no_promotion}
};

std::map<promnum, char> char_by_promo{
    {to_knight, 'N'},
    {to_bishop, 'B'},
    {to_castle, 'R'},
    {to_queen, 'Q'},
    {no_promotion, 'e'}
};

std::map<QString, QVariant> settings;