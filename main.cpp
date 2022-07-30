#include <iostream>
#include <string>// used for string user input
#include <array> //used for deck of cards
#include <ctime> //used to seed MT from <random>
#include <algorithm> //for shuffle
#include <random>//for MT

namespace Random{
    std::mt19937 seed{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };
    //only seed RNG once, keep accessing Random::seed
}
namespace Constants{
    namespace Dealer{
        const int stand {17}; //dealer must hit 17 or higher
    }
    namespace BetReturns{
        double loss{-1};//lose bet
        double tie{0}; //get bet back
        double blackjack {1.5};
        double win{2};//get bet back, then winnings of bet
    }
}

class Card
{
private:
    int m_value {};
    int m_suit {};
public:
enum Face{
    ace,
    two,
    three,
    four,
    five,
    six,
    seven,
    eight,
    nine,
    ten,
    jack,
    queen,
    king
};

enum Suit{
    spades,
    clubs,
    hearts,
    diamonds
};
    Card(Face value=ace, Suit suit=spades)//constructor, by default ace of spades is created
    : m_value {value} , m_suit {suit}
    {};

    char readSuit(){
        switch(m_suit){
        case spades: return 'S'; break;
        case clubs: return 'C'; break;
        case hearts: return 'H'; break;
        case diamonds: return 'D'; break;
        default: return 'X'; break;
        }
        return 'X';
    }
    char readValue(){
        switch(m_value){
        case ace: return 'A'; break;
        case two: return '2'; break;
        case three: return '3'; break;
        case four: return '4'; break;
        case five: return '5'; break;
        case six: return '6'; break;
        case seven: return '7'; break;
        case eight: return '8'; break;
        case nine: return '9'; break;
        case ten: return 'T'; break;
        case jack: return 'J'; break;
        case queen: return 'Q'; break;
        case king: return 'K'; break;
        default: return 'X'; break;
        }
        return 'X';
    }

    int returnValue(){
        switch(m_value){
        case ace: return 11; break;//1 case is handled by Hand class
        case two: return 2; break;
        case three: return 3; break;
        case four: return 4; break;
        case five: return 5; break;
        case six: return 6; break;
        case seven: return 7; break;
        case eight: return 8; break;
        case nine: return 9; break;
        case ten: return 10; break;
        case jack: return 10; break;
        case queen: return 10; break;
        case king: return 10; break;
        default: return 0; break;
        }
        return 0;
    }
    void setCard(int face, int suit){
        m_value = face;
        m_suit = suit;
    }

    friend std::ostream& operator<< (std::ostream& out, Card& card);

};


// overload << operator to make outputting a card natural and trivial
std::ostream& operator<< (std::ostream& out, Card& card){
    out << card.readValue() << card.readSuit();
    return out;
}

class Hand{
private:
    int m_value{};
    int m_aces{};
public:
    Hand(int value=0, int aces=0): m_value {value}, m_aces {aces}{};//constructor, default init 0 on both

    int getValue(){
        return m_value;
    }
    int getAces(){
        return m_aces;
    }

    void addCard(Card& card){
        m_value += card.returnValue();

        if(card.readValue() == 'A'){
        m_aces += 1;//add 1 if card is an ace
        }
    }

    void aceHandler(){/// needs to run iff m_value > 21 && m_aces > 0
        //no if statement here. Game handler will check for >21 (has to always be done)
        //then if >21, can then do the ace check, which then calls this.
        //this minimises the number of if statement checks that need to be made
        //if this had the checks its redundant and slow or it needs to be run every
        //time a card is handled - slow!
        m_aces -= 1;
        m_value -= 10;
    }

};

using Deck = std::array<Card, 52>;
using DeckIndex = Deck::size_type;

Deck createDeck(){
    Deck deck{};
    DeckIndex  index{0};
    //could use enum types and overload ++ to avoid static_casts but this is less code
    for(int suit=0; suit<=static_cast<int>(Card::diamonds); ++suit){
        for(int face=0; face<=static_cast<int>(Card::king); ++face){
            deck[index++].setCard(face, suit);
        }//end face loop
    }//end suit loop
    return deck;
}

Deck shuffleDeck(Deck& deck){
    /*Shuffles deck. Uses RandomSeed to avoid seeding MT multiple times*/
    std::shuffle(deck.begin(), deck.end(), Random::seed);

    return deck;
}
void printDeal(std::string person, Card& card){
    std::cout << person << " has been dealt a " << card << "\n";
}

void printScore(std::string person, Hand& hand){
    std::cout << person <<" has a value of " << hand.getValue() << ".\n";
}

bool bustChecker(std::string userName, Hand& user){
    if(user.getValue() > 21){
        if(user.getAces() > 0){//has to be nested. >21 && aces=0 wouldn't trigger bust if ace=0
                user.aceHandler();
                std::cout << userName << " has used an ace!\n";
                printScore(userName, user);
        }
        else{
                std::cout << userName << " has gone bust!\n";//if no aces, then dealer must be bust
                return true;
        }
    }
    return false;
}

bool dealerDecision(Deck& deck, Hand& dealer, DeckIndex& index){
    dealer.addCard(deck[index]);//dealer must always be dealt another card
    printDeal("Dealer", deck[index++]);

    bool bust{false};

    while(dealer.getValue() < Constants::Dealer::stand){//keep dealing cards if < stand const
        dealer.addCard(deck[index]);
        printDeal("Dealer", deck[index++]);
        bust = bustChecker("Dealer", dealer);
    }
    return bust;
}

bool playerDecision(Deck& deck, Hand& player, DeckIndex& index){
    bool bust{bustChecker("Player", player)};//run bust check in case of rare AA hand

    std::string choice{"hit"};
    while((!bust) && (choice == "hit")){
        std::cout << "Enter 'hit' or 'stick' to decide what to do.\n";
        std::cin >> choice;

        if(choice=="hit"){
            player.addCard(deck[index]);
            printDeal("Player", deck[index++]);
            printScore("Player", player);
            bust = bustChecker("Player", player);
        }
    }
    return bust;
}

double playBlackjack(){
    Deck deck{createDeck()};
    deck = shuffleDeck(deck);
    DeckIndex index{0};
    //deck is now ready for play

    //uncomment this block and adjust deck for testing purposes
    //deck[0].setCard(Card::queen, Card::diamonds);
    //deck[1].setCard(Card::ace, Card::spades);
    //deck[2].setCard(Card::ace, Card::diamonds);
    //deck[3].setCard(Card::ace, Card::hearts);

    //create dealer and player hands
    Hand dealer{};
    Hand player{};


    dealer.addCard(deck[index]);//deal dealer's first card.
    printDeal("Dealer", deck[index++]);

    player.addCard(deck[index]);//deal both player's cards
    printDeal("Player", deck[index++]);
    player.addCard(deck[index]);
    printDeal("Player", deck[index++]);

    printScore("Dealer", dealer);
    printScore("Player", player);

    //blackjack path
    if(player.getValue() == 21){
        std::cout << "Player has hit a blackjack!\n";
        dealerDecision(deck, dealer, index);
        if(dealer.getValue() == 21){
            std::cout << "Unlucky! Dealer also has 21!\n" << "Nobody wins!\n";
            return Constants::BetReturns::tie;
        }
        else{
            std::cout << "Player wins!\n";
            return Constants::BetReturns::blackjack;
        }

    }

    //player choice path
    bool playerBust{false};
    bool dealerBust{false};
    playerBust = playerDecision(deck, player, index);
    if(playerBust){
        std::cout << "Player loses!\n";
        return Constants::BetReturns::loss;
    }
    else{
        dealerBust = dealerDecision(deck, dealer, index);
        if(dealerBust){
            std::cout << "Player wins!\n";
            return Constants::BetReturns::win;
        }
        if(player.getValue() < dealer.getValue()){
            std::cout << "Player had " << player.getValue() << " while Dealer had " << dealer.getValue() << "\n";
            std::cout << "Player loses!\n";
            return Constants::BetReturns::loss;
        }
        else if(player.getValue() == dealer.getValue()){
            std::cout << "It's a draw!\n";
            return Constants::BetReturns::tie;
        }
        else{//if here, player must have won
            std::cout << "Player had " << player.getValue() << " while Dealer had " << dealer.getValue() << "\n";
            std::cout << "Player wins!\n";
        return Constants::BetReturns::win;
        }
    }
    return 0;
}



int main()
{
    std::cout << "Welcome to the C++ Casino\n";
    std::string enter{"play"};
    double playerMoney{0};


    while(enter=="play"){
        double bet{0};
        std::cout << "Enter how much you want to bet on this hand: ";
        std::cin >> bet;
        double returnMultiplier{playBlackjack()};
        playerMoney += returnMultiplier*bet;
        std::cout << "You currently have " << playerMoney << "\n";
        std::cout << "Enter 'play' to play again, enter anything else to quit: ";
        std::cin >> enter;
        std::cout << "\n";
    }
    std::cout << "Thanks for playing.\n";
    return 0;
}
