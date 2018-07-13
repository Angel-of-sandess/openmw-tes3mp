#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerAttack.hpp"

using namespace mwmp;

PacketPlayerAttack::PacketPlayerAttack(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_ATTACK;
}

void PacketPlayerAttack::Packet(RakNet::BitStream *bs, bool send)
{
    PlayerPacket::Packet(bs, send);

    RW(player->attack.target.isPlayer, send);

    if (player->attack.target.isPlayer)
    {
        RW(player->attack.target.guid, send);
    }
    else
    {
        RW(player->attack.target.refId, send, true);
        RW(player->attack.target.refNum, send);
        RW(player->attack.target.mpNum, send);
    }

    RW(player->attack.spellId, send, true);
    RW(player->attack.type, send);
    RW(player->attack.success, send);
    RW(player->attack.damage, send, false); // never compress damage

    RW(player->attack.pressed, send);
    RW(player->attack.knockdown, send);
    RW(player->attack.block, send);

    RW(player->attack.applyWeaponEnchantment, send);
    RW(player->attack.applyProjectileEnchantment, send);
}
