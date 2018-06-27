#pragma once

#include <tuple>
#include <components/openmw-mp/Base/BaseObject.hpp>
#include <memory>

class LuaState;
class Player;

class BaseObject
{
public:
    BaseObject();
    std::string getRefId() const;
    void setRefId(const std::string &refId);

    unsigned getRefNum() const;
    void setRefNum(unsigned refNum);

    unsigned getMpNum() const;
    void setMpNum(unsigned mpNum);

    RakNet::RakNetGUID getGuid() const;
    void setGuid(const RakNet::RakNetGUID &guid);

    //void setEventCell(const std::string &cellDescription);


    mwmp::BaseObject object;
    bool changedBase;
    bool copied;
};

class Object : public BaseObject
{
public:
    static void Init(LuaState &lua);
public:
    Object();
    ~Object();

    void update();

    /**
     *
     * @return  x, y, z
     */

    std::tuple<float, float, float> getPosition() const;
    void setPosition(float x, float y, float z);

    /**
     *
     * @return  x, y, z
     */
    std::tuple<float, float, float> getRotation() const;
    void setRotation(float x, float y, float z);

    int getCount() const;
    void setCount(int count);

    int getCharge() const;
    void setCharge(int charge);

    float getEnchantmentCharge() const;
    void setEnchantmentCharge(float enchantmentCharge);

    int getGoldValue() const;
    void setGoldValue(int gold);

    float getScale() const;
    void setScale(float scale);

    bool getState() const;
    void setState(bool state);

    int getDoorState() const;
    void setDoorState(int state);

    int getLockLevel() const;
    void setLockLevel(int locklevel);

    bool hasContainer() const;

    void setTeleportState(bool state);
    void setDoorDestination(const std::string &cellDescription, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);

    void setDisarmState(bool state);
    void setMasterState(bool state);

    bool changedDoorState, changedDoorDestination, changedObjectState, changedObjectScale, changedObjectTrap, changedObjectLock,
            changedObjectDelete, changedObjectSpawn, changedObjectPlace;
};

class Container : public BaseObject
{
public:
    static void Init(LuaState &lua);
public:
    Container();

    std::tuple<std::string, int, int, double> getItem(int i) const;
    void addItem(const std::string &refId, int count, int charge, float enchantmentCharge);

    void setItem(int i, const std::string &refId, int count, int charge, float enchantmentCharge);
    int getActionCount(int i) const;

    size_t size() const;
    bool changed;
};


class ObjectController
{
public:
    static void Init(LuaState &lua);
public:

    std::shared_ptr<std::vector<std::shared_ptr<Object>>> copyObjects(mwmp::BaseObjectList &objectList);
    std::shared_ptr<std::vector<std::shared_ptr<Container>>> copyContainers(mwmp::BaseObjectList &objectList);

    void sendObjects(std::shared_ptr<Player> player, std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects,
        const ESM::Cell &cell, bool broadcast = false);
    void sendConsoleCommand(std::shared_ptr<Player> player, std::shared_ptr<std::vector<std::shared_ptr<Object>>> objects,
        const ESM::Cell &cell, const std::string &consoleCommand, bool broadcast = false);
    void sendContainers(std::shared_ptr<Player> player, std::shared_ptr<std::vector<std::shared_ptr<Container>>> objects,
        const ESM::Cell &cell, bool broadcast = false);

    void requestContainers(std::shared_ptr<Player> player);
};
