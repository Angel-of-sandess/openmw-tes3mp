#ifndef OPENMW_PROCESSORACTORANIMPLAY_HPP
#define OPENMW_PROCESSORACTORANIMPLAY_HPP

#include "apps/openmw-mp/ActorProcessor.hpp"

namespace mwmp
{
    class ProcessorActorAnimPlay : public ActorProcessor
    {
    public:
        ProcessorActorAnimPlay()
        {
            BPP_INIT(ID_ACTOR_ANIM_PLAY)
        }

        void Do(ActorPacket &packet, Player &player, BaseActorList &actorList) override
        {
            // Send only to players who have the cell loaded
            Cell *serverCell = CellController::get()->getCell(&actorList.cell);

            if (serverCell != nullptr)
                serverCell->sendToLoaded(&packet, &actorList);

            //Script::Call<Script::CallbackIdentity("OnActorAnimPlay")>(player.getId(), actorList.cell.getDescription().c_str());
        }
    };
}

#endif //OPENMW_PROCESSORACTORANIMPLAY_HPP
