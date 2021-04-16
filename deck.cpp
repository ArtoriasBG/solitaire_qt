#include "deck.h"

Deck::Deck(DeckType type, int foundation_index) : _type(type), foundation_index(foundation_index) // set base position according to type
{
    switch (_type) {
    case STOCK:
        base_row = 0;
        base_col = 0;
        break;

    case WASTE:
        base_row = 4;
        base_col = 0;
        break;

    case FOUNDATION:
        base_row = foundation_index*4;
        base_col = 16;
        break;
    }
}

Deck::~Deck() { // delete all holding cards and clear the list
    for (auto it = cards.begin(); it != cards.end(); ++it) delete (*it);
    cards.clear();
}
bool Deck::isEmpty() const { return cards.empty(); }

Deck::DeckType Deck::deckType() const { return _type; }

void Deck::addCard(Card *new_card) {
    if (new_card == nullptr) return;

    // stock are adding card from front to back.
    // Flow of a card between stock and waste. stock top -> waste top -> stock bottom -> stock top
    if (_type == DeckType::STOCK)
    {
        cards.push_front(new_card);
        new_card->set_pos(0 ,0);
        new_card->set_zvalue(lowest_zvalue);
        new_card->cover_the_card(); // all card in stock is covered
        --lowest_zvalue; // for easy undo implementation
    }

    // waste are adding card from back to front
    else if (_type == DeckType::WASTE)
    {
        if (cards.size() == 0)
        {
            new_card->set_pos(base_row, base_col);
            new_card->set_zvalue(0);
        }
        else if (cards.size() == 1)
        {
            new_card->set_pos(base_row + 1, base_col);
            new_card->set_zvalue(1);
        }
        else if (cards.size() == 2)
        {
            new_card->set_pos(base_row + 2, base_col);
            new_card->set_zvalue(2);
        }
        cards.push_back(new_card);
        new_card->reveal_the_card();
    }

    // foundations are adding card from back to front
    else if (_type == DeckType::FOUNDATION)
    {
        cards.push_back(new_card);
        new_card->reveal_the_card(); // all cards in foundation is revealed
        new_card->set_pos(base_row, base_col);
        new_card->set_zvalue(new_card->get_rank());
    }
}

void Deck::removeCard() {
    if (isEmpty()) return;
    cards.pop_back();
}

void Deck::removeCard(Card* card) {
    cards.remove(card);
}

int Deck::get_foundation_index() const { return foundation_index; }

Card* Deck::top_card() const { return (cards.size() == 0 ? nullptr : cards.back()); }

bool Deck::contain(Card *card) const {
    for (auto it = cards.begin(); it != cards.end(); ++it)
        if ((*it) == card) return true;
    return false;
}

int Deck::get_base_row () const { return base_row; }
int Deck::get_base_col () const { return base_col; }

void Deck::clear_deck() { cards.clear(); }

void Deck::initialize_card(Card* new_card) {cards.push_back(new_card);}

const list<Card*>& Deck::get_cards() const { return cards; }
