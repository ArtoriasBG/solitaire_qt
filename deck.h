#ifndef DECK_H
#define DECK_H
/* Deck contains a series of card and provides different functions
 * The basic information about a deck is: deck type, base position, a list of cards,
 * Foundation index is to indentify which foundation this deck is.
 * Lowest_zvalue is set to implement undo easily.
 */
#include <card.h>
#include <list>
using std::list;

class Deck {
public:
    enum DeckType {
        STOCK, // the deck that contains all the cards to be drew
        WASTE, // the deck that contains all the cards drew from stock
        FOUNDATION // the rightmost decks that collect cards from A to K
    };

    Deck(DeckType type, int foundation_index = -1); // constructor
    ~Deck(); // destructor, will destruct all cards in this deck
    void addCard(Card* new_card); //add a card to the deck
    void removeCard(); // remove the top card
    void removeCard(Card* card); // remove the exact card
    DeckType deckType() const; // return the type of this deck
    bool isEmpty() const; // check if this deck is empty
    int get_foundation_index() const; // for moving a pile single card to foundation deck
    Card* top_card() const; // return the top card, nullptr if empty
    void initialize_card(Card* new_card); // add a card regardless of rule

    bool contain(Card* card) const; // check if this deck contain certain card
    int get_base_row () const; // get base position of this deck
    int get_base_col () const;

    const list<Card*>& get_cards() const; // get the card list of this deck

    void clear_deck(); // clear the deck
private:

    int base_row, base_col; // the position of this deck

    list<Card*> cards; // the cards in this deck
    DeckType _type; // the type of this deck

    int foundation_index; // for foundations 0 for spade 1 for heart, 2 for diamond, 3 for club

    long long int lowest_zvalue = 0; // for easy undo, -1 when each time trying to add a card
};

#endif // DECK_H
