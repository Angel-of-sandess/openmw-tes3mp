//
// Created by koncord on 02.01.18.
//

#include "BaseMgr.hpp"
#include "Player.hpp"
#include "Worldstate.hpp"

BaseMgr::BaseMgr(Player *player) : player(player), changed(false)
{

}

BaseMgr::BaseMgr(Worldstate *worldstate) : worldstate(worldstate), changed(false)
{

}

void BaseMgr::update()
{
    if (!changed)
        return;
    changed = false;

    processUpdate();
}

void BaseMgr::setChanged()
{
    changed = true;

    if (player != nullptr)
        player->addToUpdateQueue();
}
