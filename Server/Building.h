#ifndef BUILDING_H
#define BUILDING_H

class Building{
    public:
        Building();

        void setBuildingPos(int posX, int posY);
        void setBuildingId(int buildingId);
        void setBuildingType(int buildingType);
        void setBuildingOwner(int ownerId);

        int getBuildingPosX();
        int getBuildingPosY();
        int getBuildingId();
        int getBuildingType();
        int getBuildingOwner();

        void checkConnectedNeighbours();
    protected:
    private:
        int posX, posY;
        int buildingId, buildingType, ownerId;
};

#endif // BUILDING_H
