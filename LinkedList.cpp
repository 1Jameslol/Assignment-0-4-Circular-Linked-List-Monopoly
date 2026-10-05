#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

//The struct that acts as the functionality of the LinkedList along with the necessary variables to make monopoly functional WRITTEN WITH GPT-6 Luna
struct PropertyNode {
    std::string name;
    int cost;
    std::string owner;
    PropertyNode* next;
    PropertyNode* prev;
};

//The player struct functions as a player, including the necessary variables to identify the different characteristics of a player WRITTEN WITH GPT-6 Luna

struct Player {
    std::string name;
    int money;
    PropertyNode* position;
    bool inJail;
};

//Manages more board management as well as player management WRITTEN WITH GPT-6 Luna
class MonopolyBoard {
public:
    MonopolyBoard() : head(nullptr) {
        buildDefaultBoard();
    }

    ~MonopolyBoard() {
        clearBoard();
    }

    PropertyNode* getHead() const {
        return head;
    }

    std::vector<PropertyNode*> getProperties() const {
        std::vector<PropertyNode*> list;
        if (head == nullptr) {
            return list;
        }

        PropertyNode* current = head;
        do {
            list.push_back(current);
            current = current->next;
        } while (current != head);

        return list;
    }

    void movePlayer(Player& player, int spaces) const {
        if (player.position == nullptr) {
            player.position = head;
        }

        for (int i = 0; i < spaces; ++i) {
            player.position = player.position->next;
            if (player.position == head) {
                player.money += 200;
            }
        }
    }

    void buyProperty(Player& player) {
        if (player.position == nullptr) {
            return;
        }

        if (player.position->owner == "Bank" && player.position->cost > 0) {
            if (player.money >= player.position->cost) {
                player.position->owner = player.name;
                player.money -= player.position->cost;
            }
        }
    }

    //Creates all of the properties as well as tiles WRITTEN WITH GPT-6 Luna
private:
    PropertyNode* head;

    void buildDefaultBoard() {
        clearBoard();

        auto* go = new PropertyNode{"Go", 0, "Bank", nullptr, nullptr};
        auto* oldKent = new PropertyNode{"Old Kent Road", 60, "Bank", nullptr, nullptr};
        auto* whitechapel = new PropertyNode{"Whitechapel", 60, "Bank", nullptr, nullptr};
        auto* incomeTax = new PropertyNode{"Income Tax", 0, "Bank", nullptr, nullptr};
        auto* theAngel = new PropertyNode{"The Angel", 100, "Bank", nullptr, nullptr};
        auto* euston = new PropertyNode{"Euston Road", 100, "Bank", nullptr, nullptr};
        auto* pentonville = new PropertyNode{"Pentonville", 120, "Bank", nullptr, nullptr};
        auto* kingsCross = new PropertyNode{"King's Cross", 140, "Bank", nullptr, nullptr};
        auto* jail = new PropertyNode{"Jail", 0, "Bank", nullptr, nullptr};
        auto* marylebone = new PropertyNode{"Marylebone", 140, "Bank", nullptr, nullptr};
        auto* oxford = new PropertyNode{"Oxford Street", 180, "Bank", nullptr, nullptr};
        auto* freeParking = new PropertyNode{"Free Parking", 0, "Bank", nullptr, nullptr};
        auto* bond = new PropertyNode{"Bond Street", 200, "Bank", nullptr, nullptr};
        auto* goToJail = new PropertyNode{"Go To Jail", 0, "Bank", nullptr, nullptr};
        auto* regent = new PropertyNode{"Regent Street", 220, "Bank", nullptr, nullptr};

        const std::vector<PropertyNode*> tiles = {
            go, oldKent, whitechapel, incomeTax, theAngel, euston, pentonville,
            kingsCross, jail, marylebone, oxford, freeParking, bond, goToJail, regent
        };
        for (size_t i = 0; i < tiles.size(); ++i) {
            tiles[i]->next = tiles[(i + 1) % tiles.size()];
            tiles[i]->prev = tiles[(i + tiles.size() - 1) % tiles.size()];
        }

        head = tiles.front();
    }

    void clearBoard() {
        if (head == nullptr) {
            return;
        }

        PropertyNode* current = head;
        PropertyNode* start = head;

        do {
            PropertyNode* nextNode = current->next;
            delete current;
            current = nextNode;
        } while (current != start);

        head = nullptr;
    }
};

//Calls the gui to start the game and create everything WRITTEN WITH GPT-6 Luna
#include "MonopolyBoard.cpp"

int main(int argc, char* argv[]) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    return runMonopolyGui(argc, argv);
}
