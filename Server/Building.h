#ifndef BUILDING_H
#define BUILDING_H

class Building{
    public:
        Building();

        void setBuildingPos(int posX, int posY);
        void setBuildingId(int buildingId);
        void setBuildingType(int buildingType);
        void setBuildingOwner(int ownerId);
        void setBuildingRange(int buildingRange);
        void setBuildingHealth(int buildingHealth);
        void setBuildingAttack(int buildingAttack);
        void takeDamage(int damage);
        int attackBuilding(int posX, int posY);

        int getBuildingPosX();
        int getBuildingPosY();
        int getBuildingId();
        int getBuildingType();
        int getBuildingOwner();
        int getBuildingRange();
        int getBuildingHealth();
        int getBuildingMaxHealth();
        int getBuildingAttack();

        void checkConnectedNeighbours();
    protected:
    private:
        int posX, posY;
        int buildingId, buildingType, ownerId;
        int buildingRange, buildingHealth, buildingMaxHealth, buildingAttack;
};

#endif // BUILDING_H
