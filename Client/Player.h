#ifndef PLAYER_H
#define PLAYER_H

class Player{
    public:
        Player();
        void update();
        void draw();

        void setPlayerId(int id);
        void setPlayerTile(int playerTile);

        int getStats(int id);

    protected:
    private:
        int playerId;
        int playerTile;
};

#endif // PLAYER_H
