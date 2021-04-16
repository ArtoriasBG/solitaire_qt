#include "pile.h"
#include <QDebug>

Pile::Pile(int index) {
        base_row = 0;
        base_col = 2*index + 2;
}

Pile::~Pile(){
    for (auto it = cards.begin(); it != cards.end(); ++it) delete (*it);
    cards.clear();
}

const Card* Pile::top_card() const { return (cards.empty() ? nullptr : cards.back()); } // when empty return nullptr

void Pile::add_card(Card* new_card) { cards.push_back(new_card); }

bool Pile::moveable(Pile *target, Card *selected_card) {
    bool empty_check = target->top_card() == nullptr && selected_card->get_rank() == 13;// target pile is empty
    bool unempty_check = target->top_card() != nullptr
            && target->top_card()->get_rank() == selected_card->get_rank() + 1 // target pile is not empty, check rank and color
            && target->top_card()->get_color() != selected_card->get_color();

    return empty_check || unempty_check;
}

bool Pile::moveable(Deck* target, Card* selected_card) {
    bool rank_match = (target->top_card() == nullptr) ? (selected_card->get_rank() == 1)
                                                      : (selected_card->get_rank() == target->top_card()->get_rank() + 1);
    bool is_single_card = top_card() == selected_card; // selected one is the top card in this pile
    bool suit_match = selected_card->get_suit() == target->get_foundation_index(); // suit match
    return (is_single_card && suit_match && rank_match);
}

void Pile::move_cards_to(Pile *target, Card* selected_card) {
    if (moveable(target, selected_card)) {
        // get the iterator of the selected card
        auto it = cards.begin();
        for (; it != cards.end(); ++it)
            if (*it == selected_card) break;

        // move the cards
        for (; it != cards.end(); ++it) {
            target->add_card(*it);
            cards.erase(it);
        }
    }
    else return;
    // if the new top card of current pile is not revealed, reveal it
    if (top_card() != nullptr && !top_card()->is_revealed()) cards.back()->reveal_the_card();

    target->refresh_cards_pos(); // reset moved cards position
}

void Pile::move_cards_to(Deck *target, Card *seleted_card) {
    if (moveable(target, seleted_card)){
        target->addCard(seleted_card);
        cards.pop_back();
        if (top_card() != nullptr && !top_card()->is_revealed()) cards.back()->reveal_the_card();
    }
}

void Pile::refresh_cards_pos() {
    for (unsigned i = 0; i != cards.size(); ++i) {
        auto it = cards.begin();
        std::advance(it, i);
        (*it)->set_pos(base_row + i, base_col);
        (*it)->set_zvalue(i);
    }
}

void Pile::add_single_card(Card *new_card) { // add a single card to this pile
    if ((top_card() == nullptr && new_card->get_rank() == 13)
    || (top_card()->get_color() != new_card->get_color() && top_card()->get_rank() == new_card->get_rank() + 1))
        add_card(new_card);
    refresh_cards_pos();
}

void Pile::initialize_card(Card* new_card) {
    cards.push_back(new_card);
}

bool Pile::contain(Card *card) const {
    for (auto it = cards.begin(); it != cards.end(); ++it)
        if ((*it) == card) return true;
    return false;
}

int Pile::num_of_selected_cards(Card *clicked_card) const {
    bool flag = false; // check if iterator reaches the clicked card
    int result = 0; // number of card behind the cilicked card (including)
    for (auto it = cards.begin(); it != cards.end(); ++it) {
        if ((*it) == clicked_card) flag = true;
        if (flag) ++result;
    }
    return result;
}

int Pile::get_base_row() const { return base_row; }
int Pile::get_base_col() const { return base_col; }

void Pile::clear_pile() { cards.clear(); }

const list<Card*>& Pile::get_cards() const { return cards; }
