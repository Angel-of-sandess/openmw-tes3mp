#ifndef OPENMW_PACKETACTORCELLCHANGE_HPP
#define OPENMW_PACKETACTORCELLCHANGE_HPP

#include <components/openmw-mp/Packets/Actor/ActorPacket.hpp>

namespace mwmp
{
    class PacketActorCellChange : public ActorPacket
    {
    public:
        PacketActorCellChange(RakNet::RakPeerInterface *peer);

        virtual void Packet(RakNet::BitStream *bs, bool send);
    };
}

#endif //OPENMW_PACKETACTORCELLCHANGE_HPP
