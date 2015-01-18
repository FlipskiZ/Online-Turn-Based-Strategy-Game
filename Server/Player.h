#ifndef PLAYER_H
#define PLAYER_H

class Player{
    public:
        Player();
        void endPlayerTurn();

        void setPlayerId(int id);
        void setPlayerGuid(RakNet::RakNetGUID playerGuid);
        void setPlayerName(const char* playerName);
        void setPlayerTurn(bool endTurn);
        void setPlayerFirst(bool first);
        void setPlayerResources(int metalResource, int foodResource, int oilResource, int silverResource);
        void changePlayerResource(int resourceEnum, int amount);

        int getPlayerId();
        std::string getPlayerName();
        RakNet::RakNetGUID playerGuid;
        bool getPlayerTurn();
        bool getPlayerFirst();
        int getPlayerResource(int resourceEnum);
    protected:
    private:
        int playerId;
        std::string playerName;
        bool endTurn, firstBuilding;

        int metalResource, foodResource, oilResource, silverResource;
};

#endif // PLAYER_H
