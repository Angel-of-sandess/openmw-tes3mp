#ifndef OPENMW_PACKETACTORSPEECH_HPP
#define OPENMW_PACKETACTORSPEECH_HPP

#include <components/openmw-mp/Packets/Actor/ActorPacket.hpp>

namespace mwmp
{
    class PacketActorSpeech : public ActorPacket
    {
    public:
        PacketActorSpeech(RakNet::RakPeerInterface *peer);

        virtual void Packet(RakNet::BitStream *bs, bool send);
    };
}

#endif //OPENMW_PACKETACTORSPEECH_HPP
