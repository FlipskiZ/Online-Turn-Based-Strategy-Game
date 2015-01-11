#ifndef PLAYER_H
#define PLAYER_H

class Player{
    public:
        Player();
        void update();

        void setPlayerId(int id);
        void setPlayerGuid(RakNet::RakNetGUID playerGuid);
        void setPlayerName(const char* playerName);

        int getStats(int id);
        std::string getPlayerName();
        RakNet::RakNetGUID playerGuid;
    protected:
    private:
        int playerId;
        std::string playerName;
};

#endif // PLAYER_H
