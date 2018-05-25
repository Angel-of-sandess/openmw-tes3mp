#include "WorldstateProcessor.hpp"
#include "Networking.hpp"

using namespace mwmp;

template<class T>
typename BasePacketProcessor<T>::processors_t BasePacketProcessor<T>::processors;

void WorldstateProcessor::Do(WorldstatePacket &packet, const std::shared_ptr<Player> &player, BaseWorldstate &worldstate)
{
    packet.Send(true);
}

bool WorldstateProcessor::Process(RakNet::Packet &packet, BaseWorldstate &worldstate) noexcept
{
    worldstate.guid = packet.guid;

    for (auto &processor : processors)
    {
        if (processor.first == packet.data[0])
        {
            auto player = Players::getPlayerByGUID(packet.guid);
            WorldstatePacket *myPacket = Networking::get().getWorldstatePacketController()->GetPacket(packet.data[0]);

            myPacket->setWorldstate(&worldstate);
            worldstate.isValid = true;

            if (!processor.second->avoidReading)
                myPacket->Read();

            if (worldstate.isValid)
                processor.second->Do(*myPacket, player, worldstate);
            else
                LOG_MESSAGE_SIMPLE(Log::LOG_ERROR, "Received %s that failed integrity check and was ignored!", processor.second->strPacketID.c_str());
            
            return true;
        }
    }
    return false;
}
