#include <iostream>
#include <string>// used for string user input
#include <array> //used for deck of cards
#include <ctime> //used to seed MT from <random>
#include <algorithm> //for shuffle
#include <random>//for MT

//ENUMS
enum class CardRanks{
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
    king,
    ace,
};

enum class CardSuits{
    spade,
    club,
    diamond,
    heart,
};

// STRUCTURES
struct Card{
    CardSuits suit;
    CardRanks face;
};

struct Hand{ //used for storing player data.
    int score{0}; //card score
    int aces{0}; //how many aces player has. Used to implement the 1 or 11 ace rule.
};

//NAMESPACES
namespace RandomSeed{
    std::mt19937 seed{ static_cast<std::mt19937::result_type>(std::time(nullptr)) };//only seed RNG once
}

namespace DealerConst{
    const int stand {17}; //edit to change what value the dealer must stand to.
}

namespace RateReturns{//edit to change multiplier on bets.
    double loss {-1}; //default -1: lose betted money
    double tie {0};//default 0: return betted money
    double blackjack {1.5}; //default 1.5: return money + half
    double win {2}; //default 2: return money + bet from dealer
}

//DECK / INDIVIDUAL CARD FUNCTIONS
void printCard(const Card& input){
    /*Uses 2 switches to print card face and suit.*/
    switch(input.face){
    case CardRanks::two:std::cout << "2"; break;
    case CardRanks::three: std::cout << "3"; break;
    case CardRanks::four: std::cout << "4"; break;
    case CardRanks::five: std::cout << "5"; break;
    case CardRanks::six: std::cout << "6"; break;
    case CardRanks::seven: std::cout << "7"; break;
    case CardRanks::eight: std::cout << "8"; break;
    case CardRanks::nine: std::cout << "9"; break;
    case CardRanks::ten: std::cout << "10"; break;
    case CardRanks::jack: std::cout << "J"; break;
    case CardRanks::queen: std::cout << "Q"; break;
    case CardRanks::king: std::cout << "K"; break;
    case CardRanks::ace: std::cout << "A"; break;
    default: std::cout << "X!"; break;
    }

    switch(input.suit){
    case CardSuits::club: std::cout << "C"; break;
    case CardSuits::diamond: std::cout << "D"; break;
    case CardSuits::heart: std::cout << "H"; break;
    case CardSuits::spade: std::cout << "S"; break;
    default: std::cout << "X!"; break;
    }
}

std::array<Card, 52> createDeck(){
    /*Creates deck in order of SPADE, CLUB, DIAMOND, HEART. Rising from 2 to ace.*/
    std::array<Card, 52> deck{};
    std::array<Card, 52>::size_type index{0};
    for(int suit=0; suit<=static_cast<int>(CardSuits::heart); ++suit){
        for(int crank=0; crank<=static_cast<int>(CardRanks::ace); ++crank){
            deck[index].suit = static_cast<CardSuits>(suit);
            deck[index].face = static_cast<CardRanks>(crank);
            ++index;
        }

    }
    return deck;
}

std::array<Card, 52> shuffleDeck(std::array<Card, 52>& deck){
    /*Shuffles deck. Uses RandomSeed to avoid seeding MT multiple times*/
    std::shuffle(deck.begin(), deck.end(), RandomSeed::seed);

    return deck;
}

void printDeck(std::array<Card, 52>& deck){
    /*prints entire deck by looping printCard()*/
    for(int i=0; i<52; ++i){
        printCard(deck[static_cast<std::array<Card, 52>::size_type>(i)]);
        std::cout << "\n";
    }
}

//Blackjack functions
void addScore(Hand& user, Card& drawnCard){
    /*Adds card scores to the Hand struct. Works for both player and dealer. Aces add 1 to ace Hand component*/
    switch(drawnCard.face){
        case CardRanks::two:
            user.score +=2;
            break;
        case CardRanks::three:
            user.score +=3;
            break;
        case CardRanks::four:
            user.score +=4;
            break;
        case CardRanks::five:
            user.score +=5;
            break;
        case CardRanks::six:
            user.score +=6;
            break;
        case CardRanks::seven:
            user.score +=7;
            break;
        case CardRanks::eight:
            user.score +=8;
            break;
        case CardRanks::nine:
            user.score +=9;
            break;
        case CardRanks::ten:
            user.score +=10;
            break;
        case CardRanks::jack:
            user.score +=10;
            break;
        case CardRanks::queen:
            user.score +=10;
            break;
        case CardRanks::king:
            user.score +=10;
            break;
        case CardRanks::ace:
            user.score +=11;
            user.aces +=1;
            break;
        default:
            user.score +=0;
            break;
    }
}

void aceHandler(Hand& user, bool isDealer){
    /*Handles aces and them having values of 1 or 11. Bool is for correct std::cout names*/
    if(user.aces > 0 && user.score > 21){
        --user.aces;
        user.score -=10;
        if(isDealer){
            std::cout << "Dealer is now using an ace value of 1.\n";
            std::cout << "Dealer has " << user.score << ".\n";
        }
        else{
            std::cout << "Player is now using an ace value of 1.\n";
            std::cout << "Player has " << user.score << ".\n";
        }
    }
}

void hitOrStick(Hand& player, std::array<Card, 52>& deck, std::array<Card, 52>::size_type& index){
    /*Player decision function. */
    std::string choice{"hit"};

    aceHandler(player, false);//isDealer=false
    //implement aceHandler before while loop to prevent case of Ace-ace hand (22) causing a bust even
    //though it should be 12. If not here, while loop will not run meaning that acehandler wont be run.
    //same idea for dealerHit() function

    while(player.score < 21 && choice == "hit"){
        std::cout << "Enter 'hit' to hit, or enter 'stick' to stick.\n";
        std::cin >> choice;
        if(choice=="hit"){
            addScore(player, deck[index++]);
            std::cout << "Player has been dealt a ";
            printCard(deck[index-1]);
            std::cout << ".\nPlayer has " << player.score << ".\n";
        }
        aceHandler(player, false);//isDealer=false
    }
}

void dealerHit(Hand& user, std::array<Card, 52>& deck, std::array<Card, 52>::size_type& index){
    /*Function for dealer decision. Separate function as dealer's decisions are algorithmic*/
    aceHandler(user, true);//isDealer=true

    while(user.score < DealerConst::stand){
        addScore(user, deck[index++]);
        std::cout << "Dealer has been dealt a ";
        printCard(deck[index-1]);
        std::cout << ".\nDealer has " << user.score << ".\n";
        aceHandler(user, true);//isDealer=true
    }
}

double showdown(Hand& player, Hand& dealer, bool blackjack){
    /*function for deciding winner and returning a rate to apply to bet.*/
    if(player.score < dealer.score){
        std::cout << "Player has " << player.score << " while Dealer has " << dealer.score << ".\n";
        std::cout << "Dealer wins!";
        return RateReturns::loss;
    }
    else if(player.score == dealer.score){
        std::cout << "Player has " << player.score << " while Dealer has " << dealer.score << ".\n";
        std::cout << "Nobody wins but nobody loses!\n";
        return RateReturns::tie;
    }
    else{
        std::cout << "Dealer has " << dealer.score << " while Player has " << player.score << ".\n";
        std::cout << "Player wins!";
        if(blackjack){
            return RateReturns::blackjack;
        }
        else{
            return RateReturns::win;
        }
    }
}

double playBlackjack(std::array<Card, 52>& deck){
    /*Function that handles game of blackjack. Returns rate of return on bet.*/
    std::array<Card, 52>::size_type deckIndex{0};
    Hand dealer{};
    Hand player{};
    bool blackjack{false};
    double rate{};

    //deal dealer's first card. Dealer's second card is face down in blackjack. So can be dealt later here.
    addScore(dealer, deck[deckIndex]);
    std::cout << "Dealer has been dealt ";
    printCard(deck[deckIndex++]);//++ here as avoids needing to -1 it if ++ was on addScore
    std::cout << ".\nDealer has " << dealer.score << ".\n";

    //deal player's hand
    addScore(player, deck[deckIndex]);
    std::cout << "Player has been dealt ";
    printCard(deck[deckIndex++]);

    addScore(player, deck[deckIndex]);
    std::cout << ".\nPlayer has been dealt ";
    printCard(deck[deckIndex++]);
    std::cout << ".\nPlayer has " << player.score << ".\n";

    if(player.score == 21){
        std::cout << "Player has hit a blackjack!\n";
        dealerHit(dealer, deck, deckIndex);
        blackjack = true;
        std::cout << "Player has " << player.score << " while Dealer has " << dealer.score << ".\n";
        if(player.score == dealer.score){//works as player.score is 21 in this case
            std::cout << "You hit a blackjack but the dealer hit 21. Unlucky!\n";
            return RateReturns::tie;
        }
        else{
            return RateReturns::blackjack;
        }
    }

    hitOrStick(player, deck, deckIndex); //start player decisions
    if(player.score > 21){
        std::cout << "Player has gone BUST!\n";
        std::cout << "Dealer wins!";
        return RateReturns::loss;
    }
    else{
        dealerHit(dealer, deck, deckIndex);
        if(dealer.score > 21){
            std::cout << "Dealer has gone BUST!\n";
            std::cout << "Player wins!";
            return RateReturns::win;
        }
        else{
            rate = showdown(player, dealer, blackjack);
            return rate;
        }
    }
}

//MAIN
int main()
{
    std::array<Card, 52> deck{};
    deck = createDeck();

    std::string choice{"play"};
    double playerMoney{0};
    double winningRate{};

    while(choice == "play"){
        std::cout << "BLACKJACK\nbut it's in C++\n";

        double bet{};
        std::cout << "Please enter how much money you would like to bet on this hand: ";
        std::cin >> bet;
        deck = shuffleDeck(deck); //shuffle every game of blackjack

        winningRate = playBlackjack(deck);
        playerMoney += bet*winningRate;
        std::cout << "\nYou now have: " << playerMoney << "\n";
        std::cout <<"\nTo play again, enter 'play', to quit, enter anything else: ";
        std::cin >> choice;
        std::cout << "\n";
    }

    std::cout << "Thanks for playing at the C++ casino!\n";
    if(playerMoney < 0){
        std::cout << "You lost " << -playerMoney << " in total.\n";
    }
    else if(playerMoney == 0){
        std::cout << "You broke even!\n";
    }
    else{
        std::cout << "You walked away with " << playerMoney << " in total.\nWell done!\n";
    }
    return 0;
}
