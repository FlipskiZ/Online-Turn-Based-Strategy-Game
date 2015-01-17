#ifndef PLAYER_H
#define PLAYER_H

class Player{
    public:
        Player();
        void update();
        void draw();

        void setPlayerId(int id);
        void setPlayerName(const char* playerName);
        void setPlayerTurn(bool endTurn);
        void setPlayerResources(int metalResource, int foodResource, int oilResource, int silverResource);
        void setPlayerResource(int resourceEnum, int amount);

        int getPlayerId();
        std::string getPlayerName();
        bool getPlayerTurn();
        int getPlayerResource(int resourceEnum);

    protected:
    private:
        int playerId;
        std::string playerName;
        bool endTurn;

        int metalResource, foodResource, oilResource, silverResource;
};

#endif // PLAYER_H
