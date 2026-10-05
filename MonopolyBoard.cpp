#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>

class MonopolyWindow : public QWidget {
public:

//The constructor creates the window along with all of the parts attached to the gui WRITTEN WITH GPT-6 Luna
    MonopolyWindow()
        : board(), turnsCompleted(0), currentPlayerIndex(0), pendingProperty(nullptr),
          pendingExtraRoll(false), gameOver(false) {
        players = {
            {"Player 1", 1500, board.getHead(), false},
            {"Player 2", 1500, board.getHead(), false}
        };

        setWindowTitle("Monopoly");
        setStyleSheet(
            "QWidget { background: #176b4d; color: #173329; font-family: Arial; }"
            "QLabel#title { color: #fff8e7; font-size: 28px; font-weight: 800; }"
            "QLabel#subtitle { color: #d8eee2; font-size: 13px; }"
            "QWidget#center { background: #e9f0d8; border: 3px solid #b28b42; border-radius: 12px; }"
            "QPushButton { background: #d84335; color: white; border: 0; border-radius: 8px;"
            " font-size: 15px; font-weight: bold; padding: 10px 16px; }"
            "QPushButton:hover { background: #ed5b49; }"
            "QPushButton:disabled { background: #888; }"
        );

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(22, 18, 22, 22);
        root->setSpacing(10);

        auto* title = new QLabel("MONOPOLY");
        title->setObjectName("title");
        title->setAlignment(Qt::AlignCenter);
        root->addWidget(title);

        auto* subtitle = new QLabel("Take turns, buy properties, collect rent, and avoid jail");
        subtitle->setObjectName("subtitle");
        subtitle->setAlignment(Qt::AlignCenter);
        root->addWidget(subtitle);

        auto* boardLayout = new QGridLayout();
        boardLayout->setSpacing(5);

        const int tilePositions[15][2] = {
            {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0},
            {3, 0}, {2, 0}, {1, 0}, {0, 0}, {0, 1},
            {0, 2}, {0, 3}, {0, 4}, {1, 4}, {2, 4}
        };

        const auto properties = board.getProperties();
        for (size_t i = 0; i < properties.size(); ++i) {
            auto* tile = new QLabel(this);
            tile->setMinimumSize(130, 105);
            tile->setAlignment(Qt::AlignCenter);
            tile->setWordWrap(true);
            tile->setMargin(7);
            propertyTiles.push_back(tile);
            boardLayout->addWidget(tile, tilePositions[i][0], tilePositions[i][1]);
        }

        auto* center = new QWidget(this);
        center->setObjectName("center");
        auto* centerLayout = new QVBoxLayout(center);
        centerLayout->setContentsMargins(18, 16, 18, 16);
        centerLayout->setSpacing(9);

        turnLabel = new QLabel(center);
        turnLabel->setAlignment(Qt::AlignCenter);
        turnLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
        centerLayout->addWidget(turnLabel);

        diceLabel = new QLabel("🎲");
        diceLabel->setAlignment(Qt::AlignCenter);
        diceLabel->setStyleSheet("font-size: 44px;");
        centerLayout->addWidget(diceLabel);

        messageLabel = new QLabel("Player 1 starts on Go. Roll to begin.");
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setWordWrap(true);
        messageLabel->setMinimumHeight(58);
        messageLabel->setStyleSheet("font-size: 13px;");
        centerLayout->addWidget(messageLabel);

        moneyLabel = new QLabel(center);
        moneyLabel->setAlignment(Qt::AlignCenter);
        moneyLabel->setStyleSheet("font-size: 13px; font-weight: bold;");
        centerLayout->addWidget(moneyLabel);

        rollButton = new QPushButton("Roll Dice", center);
        centerLayout->addWidget(rollButton);
        buyButton = new QPushButton("Buy Property", center);
        centerLayout->addWidget(buyButton);
        passButton = new QPushButton("Pass on Property", center);
        centerLayout->addWidget(passButton);
        bailButton = new QPushButton("Pay $50 Bail & Roll", center);
        centerLayout->addWidget(bailButton);
        serveButton = new QPushButton("Serve 1 Turn", center);
        centerLayout->addWidget(serveButton);

        connect(rollButton, &QPushButton::clicked, this, [this]() { rollTurn(); });
        connect(buyButton, &QPushButton::clicked, this, [this]() { buyPendingProperty(); });
        connect(passButton, &QPushButton::clicked, this, [this]() { passPendingProperty(); });
        connect(bailButton, &QPushButton::clicked, this, [this]() { payBailAndRoll(); });
        connect(serveButton, &QPushButton::clicked, this, [this]() { serveJailTurn(); });

        boardLayout->addWidget(center, 1, 1, 3, 3);
        root->addLayout(boardLayout);
        setMinimumSize(980, 790);

        hideDecisionButtons();
        refreshBoard();
    }

private:
    Player& currentPlayer() {
        return players[currentPlayerIndex];
    }
  //Implements the functionality of the game for example, the game logic, dice rolling, property management, and turn management WRITTEN WITH GPT-6 Luna
    void rollTurn() {
        if (gameOver || currentPlayer().inJail) {
            return;
        }
        rollAndResolve();
    }

    void rollAndResolve() {
        const int dieOne = std::rand() % 6 + 1;
        const int dieTwo = std::rand() % 6 + 1;
        const int roll = dieOne + dieTwo;
        Player& player = currentPlayer();
        pendingExtraRoll = dieOne == dieTwo;
        board.movePlayer(player, roll);
        diceLabel->setText(QString("🎲  %1 + %2 = %3").arg(dieOne).arg(dieTwo).arg(roll));

        PropertyNode* landed = player.position;
        const QString name = QString::fromStdString(landed->name);

        if (landed->name == "Go To Jail") {
            for (PropertyNode* tile : board.getProperties()) {
                if (tile->name == "Jail") {
                    player.position = tile;
                    break;
                }
            }
            player.inJail = true;
            messageLabel->setText(QString("%1 rolled %2 and was sent to Jail. On their next turn, pay $50 bail or serve one turn.")
                .arg(QString::fromStdString(player.name)).arg(roll));
            finishTurn(false);
            return;
        }

        if (landed->name == "Income Tax") {
            if (player.money < 100) {
                endGame(player, QString("%1 cannot pay $100 Income Tax.")
                    .arg(QString::fromStdString(player.name)));
                return;
            }
            const int tax = 100;
            player.money -= tax;
            messageLabel->setText(QString("%1 rolled %2 and paid $%3 Income Tax.")
                .arg(QString::fromStdString(player.name)).arg(roll).arg(tax));
            finishTurn(pendingExtraRoll);
            return;
        }

        if (landed->name == "Jail") {
            messageLabel->setText(QString("%1 rolled %2 and is just visiting Jail.")
                .arg(QString::fromStdString(player.name)).arg(roll));
            finishTurn(pendingExtraRoll);
            return;
        }

        if (landed->name == "Free Parking") {
            messageLabel->setText(QString("%1 rolled %2 and landed on Free Parking.")
                .arg(QString::fromStdString(player.name)).arg(roll));
            finishTurn(pendingExtraRoll);
            return;
        }

        if (landed->name == "Go") {
            messageLabel->setText(QString("%1 rolled %2 and collected $200 at Go.")
                .arg(QString::fromStdString(player.name)).arg(roll));
            finishTurn(pendingExtraRoll);
            return;
        }

        if (landed->owner == "Bank") {
            if (player.money < landed->cost) {
                messageLabel->setText(QString("%1 landed on %2 but cannot afford its $%3 price.")
                    .arg(QString::fromStdString(player.name)).arg(name).arg(landed->cost));
                finishTurn(pendingExtraRoll);
                return;
            }

            pendingProperty = landed;
            messageLabel->setText(QString("%1 landed on %2 ($%3). Buy it or pass?")
                .arg(QString::fromStdString(player.name)).arg(name).arg(landed->cost));
            rollButton->setEnabled(false);
            buyButton->show();
            passButton->show();
            refreshBoard();
            return;
        }

        if (landed->owner == player.name) {
            messageLabel->setText(QString("%1 landed on their own property, %2.")
                .arg(QString::fromStdString(player.name)).arg(name));
            finishTurn(pendingExtraRoll);
            return;
        }

        const int rent = std::max(10, landed->cost / 5);
        if (player.money < rent) {
            endGame(player, QString("%1 cannot pay $%2 rent to %3.")
                .arg(QString::fromStdString(player.name)).arg(rent)
                .arg(QString::fromStdString(landed->owner)));
            return;
        }
        player.money -= rent;
        for (Player& owner : players) {
            if (owner.name == landed->owner) {
                owner.money += rent;
                break;
            }
        }
        messageLabel->setText(QString("%1 landed on %2 and paid $%3 rent to %4.")
            .arg(QString::fromStdString(player.name)).arg(name).arg(rent)
            .arg(QString::fromStdString(landed->owner)));
        finishTurn(pendingExtraRoll);
    }

    void buyPendingProperty() {
        if (pendingProperty == nullptr) {
            return;
        }

        Player& player = currentPlayer();
        const QString name = QString::fromStdString(pendingProperty->name);
        const int cost = pendingProperty->cost;
        board.buyProperty(player);
        pendingProperty = nullptr;
        messageLabel->setText(QString("%1 bought %2 for $%3.")
            .arg(QString::fromStdString(player.name)).arg(name).arg(cost));
        finishTurn(pendingExtraRoll);
    }

    void passPendingProperty() {
        if (pendingProperty == nullptr) {
            return;
        }

        const QString name = QString::fromStdString(pendingProperty->name);
        messageLabel->setText(QString("%1 passed on %2.")
            .arg(QString::fromStdString(currentPlayer().name)).arg(name));
        pendingProperty = nullptr;
        finishTurn(pendingExtraRoll);
    }

    void payBailAndRoll() {
        Player& player = currentPlayer();
        if (!player.inJail || player.money < 50) {
            messageLabel->setText("You need $50 to pay bail. Choose Serve 1 Turn instead.");
            return;
        }

        player.money -= 50;
        player.inJail = false;
        hideDecisionButtons();
        messageLabel->setText(QString("%1 paid $50 bail and rolled.")
            .arg(QString::fromStdString(player.name)));
        rollAndResolve();
    }

    void serveJailTurn() {
        if (!currentPlayer().inJail) {
            return;
        }

        currentPlayer().inJail = false;
        messageLabel->setText(QString("%1 served one turn in Jail and is released.")
            .arg(QString::fromStdString(currentPlayer().name)));
        finishTurn();
    }

    void finishTurn(bool extraRoll = false) {
        pendingProperty = nullptr;
        hideDecisionButtons();
        ++turnsCompleted;
        if (!extraRoll) {
            currentPlayerIndex = (currentPlayerIndex + 1) % players.size();
        }
        refreshBoard();

        if (gameOver) {
            return;
        }

        if (currentPlayer().inJail) {
            messageLabel->setText(messageLabel->text() +
                QString("\n%1 is in Jail. Pay $50 bail or serve one turn.")
                    .arg(QString::fromStdString(currentPlayer().name)));
            rollButton->setEnabled(false);
            bailButton->show();
            serveButton->show();
        } else {
            rollButton->setEnabled(true);
            if (extraRoll) {
                messageLabel->setText(messageLabel->text() + "\nDoubles! Roll again.");
            }
        }
    }

    void endGame(const Player& loser, const QString& reason) {
        gameOver = true;
        hideDecisionButtons();
        rollButton->setEnabled(false);
        rollButton->setText("Game Over");
        const QString winner = loser.name == players[0].name
            ? QString::fromStdString(players[1].name)
            : QString::fromStdString(players[0].name);
        messageLabel->setText(reason + "\n" + winner + " wins by bankruptcy.");
        refreshBoard();
    }

    void hideDecisionButtons() {
        buyButton->hide();
        passButton->hide();
        bailButton->hide();
        serveButton->hide();
    }

    //Creates the properties of the gui when it comes to coloring, more board management, and the instantiation of the variables WRITTEN WITH GPT-6 Luna

    QString tileColor(size_t index, const PropertyNode* property) const {
        if (property->name == "Go") return "#e7bd45";
        if (property->name == "Jail" || property->name == "Go To Jail") return "#e7a06a";
        if (property->name == "Free Parking") return "#76b9a3";
        if (property->name == "Income Tax") return "#d8d8d8";

        static const char* colors[] = {
            "#e7bd45", "#e7bd45", "#e887ae", "#e887ae", "#ee7b42",
            "#ee7b42", "#df4640", "#df4640", "#aa72b4", "#aa72b4"
        };
        return colors[(index - 1) % 10];
    }

    void refreshBoard() {
        const auto properties = board.getProperties();
        turnLabel->setText(QString("TURN %1  •  %2")
            .arg(turnsCompleted + 1)
            .arg(QString::fromStdString(currentPlayer().name)));

        for (size_t i = 0; i < properties.size(); ++i) {
            const PropertyNode* property = properties[i];
            QString playerMarkers;
            for (const Player& player : players) {
                if (player.position == property) {
                    const QString markerColor = player.name == "Player 1" ? "#1876d2" : "#d84335";
                    playerMarkers += QString("<span style='color:%1;font-weight:bold'>● %2</span><br>")
                        .arg(markerColor).arg(QString::fromStdString(player.name));
                }
            }

            QString price;
            if (property->cost > 0) {
                price = "$" + QString::number(property->cost);
            } else if (property->name == "Income Tax") {
                price = "PAY $100";
            } else {
                price = "SPECIAL";
            }

            QString owner = property->cost > 0
                ? (property->owner == "Bank" ? "For sale" : "Owner: " + QString::fromStdString(property->owner))
                : QString();
            propertyTiles[static_cast<int>(i)]->setText(
                QString("<div style='background:%1;height:8px;margin:-7px -7px 6px -7px'></div>"
                        "<b>%2</b><br><span style='font-size:11px'>%3</span><br>"
                        "<span style='font-size:10px'>%4</span><br>%5")
                    .arg(tileColor(i, property))
                    .arg(QString::fromStdString(property->name))
                    .arg(price).arg(owner).arg(playerMarkers)
            );

            const QString borderColor = property->owner == "Bank" ? "#b7a779" : "#d84335";
            propertyTiles[static_cast<int>(i)]->setStyleSheet(
                QString("QLabel { background:#fffdf5; border:2px solid %1; border-radius:7px;"
                        " color:#26352c; font-size:12px; }").arg(borderColor)
            );
        }

        moneyLabel->setText(
            QString("<span style='color:#1876d2'>● Player 1: $%1%2</span><br>"
                    "<span style='color:#d84335'>● Player 2: $%3%4</span>")
                .arg(players[0].money)
                .arg(players[0].inJail ? " (JAIL)" : "")
                .arg(players[1].money)
                .arg(players[1].inJail ? " (JAIL)" : "")
        );
    }

    MonopolyBoard board;
    std::vector<Player> players;
    int turnsCompleted;
    int currentPlayerIndex;
    PropertyNode* pendingProperty;
    bool pendingExtraRoll;
    bool gameOver;
    std::vector<QLabel*> propertyTiles;
    QLabel* turnLabel;
    QLabel* diceLabel;
    QLabel* messageLabel;
    QLabel* moneyLabel;
    QPushButton* rollButton;
    QPushButton* buyButton;
    QPushButton* passButton;
    QPushButton* bailButton;
    QPushButton* serveButton;
};

//Creates and instantiates the window WRITTEN WITH GPT-6 Luna

int runMonopolyGui(int argc, char* argv[]) {
    QApplication app(argc, argv);
    MonopolyWindow window;
    window.show();
    return app.exec();
}
