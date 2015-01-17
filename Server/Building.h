#ifndef BUILDING_H
#define BUILDING_H

class Building{
    public:
        Building();

        void setBuildingPos(int posX, int posY);
        void setBuildingId(int buildingId);
        void setBuildingType(int buildingType);
        void setBuildingOwner(int ownerId);
        void setBuildingCapital(bool capital);

        int getBuildingPosX();
        int getBuildingPosY();
        int getBuildingId();
        int getBuildingType();
        int getBuildingOwner();
        bool getBuildingCapital();

        void checkConnectedNeighbours(int posX, int posY);
    protected:
    private:
        int posX, posY;
        int buildingId, buildingType, ownerId;
        bool capital;
};

#endif // BUILDING_H
