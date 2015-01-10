#ifndef PLAYER_H
#define PLAYER_H

class Player{
    public:
        Player();
        void update();

        void setPlayerId(int id);
        void setPlayerAddress(RakNet::SystemAddress playerAddress);
        void setPlayerName(const char* playerName);

        int getStats(int id);
        std::string getPlayerName();
        RakNet::SystemAddress getPlayerAddress();
    protected:
    private:
        int playerId;
        std::string playerName;

        RakNet::SystemAddress playerAddress;
};

#endif // PLAYER_H
