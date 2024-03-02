#include "local_types.h"

std::map<QString, QVariant> settings;

std::map<char, promo_ty> promo_by_char{
    {'N', to_knight},
    {'B', to_bishop},
    {'R', to_castle},
    {'Q', to_queen},
    {'e', no_promotion}
};

std::map<promo_ty, char> char_by_promo{
    {to_knight, 'N'},
    {to_bishop, 'B'},
    {to_castle, 'R'},
    {to_queen, 'Q'},
    {no_promotion, 'e'}
};