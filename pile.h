#ifndef TABLEAU_H
#define TABLEAU_H
/* Pile is the 7 tableau pile in the center of the window
 * Different from a normal deck, it supports different operations
 */
#include "card.h"
#include "deck.h"
#include <list>

using std::list;

class Pile {
public:
    Pile(int index);
    ~Pile();
    bool moveable(Pile* target, Card* selected_card); // moveable to another pile
    bool moveable(Deck* target, Card* selected_card); // moveable to a foundatiion
    void move_cards_to(Pile* target, Card* seleted_card); //move cards from one tableau to another
    void move_cards_to(Deck* target, Card* seleted_card); // move top card to a foundation
    void add_single_card(Card* new_card); // add a single card from waste or foundation
    void refresh_cards_pos(); // after each move, refresh the positions of all cards in this pile
    void initialize_card(Card* new_card); // when initializing, add card regardless of rule

    bool contain(Card* card) const; // check whether this pile contain certain card
    int num_of_selected_cards(Card* clicked_card) const; // check the number of cards from the selected card to the bottom of this pile

    int get_base_row() const; // get base position of this pile
    int get_base_col() const;

    const Card* top_card() const; // get the top card on this pile

    const list<Card*>& get_cards() const; // get the card list of this pile

    void clear_pile(); // for easier implementation of undo, clear the pile and then reload it
private:
    void add_card(Card* new_card); // helper, put card into the list, similar to initialize
    int base_row, base_col; // base position
    list<Card*> cards; // the card that this pile hold
};

#endif // TABLEAU_H
