#include <components/openmw-mp/NetworkMessages.hpp>
#include "PacketPlayerCellState.hpp"


mwmp::PacketPlayerCellState::PacketPlayerCellState(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_CELL_STATE;
    priority = IMMEDIATE_PRIORITY;
    reliability = RELIABLE_ORDERED;
}

void mwmp::PacketPlayerCellState::Packet(RakNet::BitStream *bs, bool send)
{
    PlayerPacket::Packet(bs, send);

    if (send)
        player->cellStateChanges.count = (unsigned int)(player->cellStateChanges.cellStates.size());
    else
        player->cellStateChanges.cellStates.clear();

    RW(player->cellStateChanges.count, send);

    for (unsigned int i = 0; i < player->cellStateChanges.count; i++)
    {
        CellState cellState;

        if (send)
            cellState = player->cellStateChanges.cellStates.at(i);
        
        RW(cellState.type, send);
        RW(cellState.cell.mData, send, true);
        RW(cellState.cell.mName, send, true);

        if (!send)
            player->cellStateChanges.cellStates.push_back(cellState);
    }
}
