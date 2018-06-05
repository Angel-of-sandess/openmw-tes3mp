//
// Created by koncord on 25.08.17.
//

#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/openmw-mp/Base/BaseNetCreature.hpp>

#include "Script/LuaState.hpp"
#include "Networking.hpp"

#include "NetActor.hpp"
#include "Player.hpp"

using namespace std;

NetActor::NetActor() : inventory(this), cellAPI(this), isActorPlayer(false)
{

}

void NetActor::resetUpdateFlags()
{
    baseInfoChanged = false;
    shapeshiftChanged = false;
    levelChanged = false;
    statsChanged = false;
    positionChanged = false;
    skillsChanged = false;
    attributesChanged = false;
}

std::tuple<float, float, float> NetActor::getPosition() const
{
    return make_tuple(netCreature->position.pos[0], netCreature->position.pos[1], netCreature->position.pos[2]);
}

void NetActor::setPosition(float x, float y, float z)
{
    netCreature->position.pos[0] = x;
    netCreature->position.pos[1] = y;
    netCreature->position.pos[2] = z;

    if (!positionChanged && isPlayer())
        toPlayer()->addToUpdateQueue();
    positionChanged = true;
}

std::tuple<float, float> NetActor::getRotation() const
{
    return make_tuple(netCreature->position.rot[0], netCreature->position.rot[2]);
}

void NetActor::setRotation(float x, float z)
{
    netCreature->position.rot[0] = x;
    netCreature->position.rot[2] = z;

    if (!positionChanged && isPlayer())
        toPlayer()->addToUpdateQueue();
    positionChanged = true;
}

void NetActor::setMomentum(float x, float y, float z)
{
    netCreature->momentum.pos[0] = x;
    netCreature->momentum.pos[1] = y;
    netCreature->momentum.pos[2] = z;

    if (!momentumChanged && isPlayer())
        toPlayer()->addToUpdateQueue();
    momentumChanged = true;
}

std::tuple<float, float> NetActor::getHealth() const
{
    return make_tuple(netCreature->creatureStats.mDynamic[0].mBase, netCreature->creatureStats.mDynamic[0].mCurrent);
}

void NetActor::setHealth(float base, float current)
{
    netCreature->creatureStats.mDynamic[0].mBase = base;
    netCreature->creatureStats.mDynamic[0].mCurrent = current;

    if (!Utils::vectorContains(&netCreature->statsDynamicIndexChanges, 0))
        netCreature->statsDynamicIndexChanges.push_back(0);

    if (!statsChanged && isPlayer())
        toPlayer()->addToUpdateQueue();
    statsChanged = true;
}

std::tuple<float, float> NetActor::getMagicka() const
{
    return make_tuple(netCreature->creatureStats.mDynamic[1].mBase, netCreature->creatureStats.mDynamic[1].mCurrent);
}

void NetActor::setMagicka(float base, float current)
{
    netCreature->creatureStats.mDynamic[1].mBase = base;
    netCreature->creatureStats.mDynamic[1].mCurrent = current;

    if (!Utils::vectorContains(&netCreature->statsDynamicIndexChanges, 1))
        netCreature->statsDynamicIndexChanges.push_back(1);

    if (!statsChanged && isPlayer())
        toPlayer()->addToUpdateQueue();
    statsChanged = true;
}

std::tuple<float, float> NetActor::getFatigue() const
{
    return make_tuple(netCreature->creatureStats.mDynamic[2].mBase, netCreature->creatureStats.mDynamic[2].mCurrent);
}

void NetActor::setFatigue(float base, float current)
{
    netCreature->creatureStats.mDynamic[2].mBase = base;
    netCreature->creatureStats.mDynamic[2].mCurrent = current;

    if (!Utils::vectorContains(&netCreature->statsDynamicIndexChanges, 2))
        netCreature->statsDynamicIndexChanges.push_back(2);

    if (!statsChanged && isPlayer())
        toPlayer()->addToUpdateQueue();
    statsChanged = true;
}

Inventory &NetActor::getInventory()
{
    return inventory;
}

Cells &NetActor::getCell()
{
    return cellAPI;
}

Player *NetActor::toPlayer()
{
    if (isPlayer())
        return dynamic_cast<Player*>(this);
    return nullptr;
}
