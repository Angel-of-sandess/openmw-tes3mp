#include "actionteleport.hpp"

/*
    Start of tes3mp addition

    Include additional headers for multiplayer purposes
*/
#include <components/openmw-mp/Log.hpp>
#include "../mwbase/windowmanager.hpp"
#include "../mwmp/Main.hpp"
#include "../mwmp/CellController.hpp"
/*
    End of tes3mp addition
*/

#include "../mwbase/environment.hpp"
#include "../mwbase/world.hpp"
#include "../mwbase/mechanicsmanager.hpp"

#include "../mwmechanics/creaturestats.hpp"

#include "../mwworld/class.hpp"

#include "player.hpp"

namespace MWWorld
{
    ActionTeleport::ActionTeleport (const std::string& cellName,
        const ESM::Position& position, bool teleportFollowers)
    : Action (true), mCellName (cellName), mPosition (position), mTeleportFollowers(teleportFollowers)
    {
    }

    void ActionTeleport::executeImp (const Ptr& actor)
    {
        if (mTeleportFollowers)
        {
            // Find any NPCs that are following the actor and teleport them with him
            std::set<MWWorld::Ptr> followers;
            getFollowersToTeleport(actor, followers);

            for (std::set<MWWorld::Ptr>::iterator it = followers.begin(); it != followers.end(); ++it)
                teleport(*it);

            /*
                Start of tes3mp addition

                Update LocalActors before we unload their cells, so packets with their cell changes
                can be sent
            */
            mwmp::Main::get().getCellController()->updateLocal(true);
            /*
                End of tes3mp addition
            */
        }

        teleport(actor);
    }

    void ActionTeleport::teleport(const Ptr &actor)
    {
        MWBase::World* world = MWBase::Environment::get().getWorld();
        actor.getClass().getCreatureStats(actor).land();
        if(actor == world->getPlayerPtr())
        {
            world->getPlayer().setTeleported(true);
            if (mCellName.empty())
                world->changeToExteriorCell (mPosition, true);
            else
                world->changeToInteriorCell (mCellName, mPosition, true);
        }
        else
        {
            /*
                Start of tes3mp change (major)

                Only allow LocalActors to teleport across cells
            */
            if (!mwmp::Main::get().getCellController()->isLocalActor(actor))
            {
                MWBase::Environment::get().getWindowManager()->messageBox("That NPC can't follow you because their AI is running on another player's client.");
                return;
            }
            else
            {
                LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Teleporting actor %s-%i-%i to new cell", actor.getCellRef().getRefId().c_str(),
                    actor.getCellRef().getRefNum().mIndex, actor.getCellRef().getMpNum());
            }
            /*
                End of tes3mp change (major)
            */

            if (mCellName.empty())
            {
                int cellX;
                int cellY;
                world->positionToIndex(mPosition.pos[0],mPosition.pos[1],cellX,cellY);
                world->moveObject(actor,world->getExterior(cellX,cellY),
                    mPosition.pos[0],mPosition.pos[1],mPosition.pos[2]);
            }
            else
                world->moveObject(actor,world->getInterior(mCellName),mPosition.pos[0],mPosition.pos[1],mPosition.pos[2]);
        }
    }

    void ActionTeleport::getFollowersToTeleport(const MWWorld::Ptr& actor, std::set<MWWorld::Ptr>& out) {
        std::set<MWWorld::Ptr> followers;
        MWBase::Environment::get().getMechanicsManager()->getActorsFollowing(actor, followers);

        for(std::set<MWWorld::Ptr>::iterator it = followers.begin();it != followers.end();++it)
        {
            MWWorld::Ptr follower = *it;

            std::string script = follower.getClass().getScript(follower);
            if (!script.empty() && follower.getRefData().getLocals().getIntVar(script, "stayoutside") == 1)
                continue;

            if ((follower.getRefData().getPosition().asVec3() - actor.getRefData().getPosition().asVec3()).length2() <= 800*800)
                out.insert(follower);
        }
    }
}
