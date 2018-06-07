//
// Created by koncord on 05.01.16.
//

#include <components/openmw-mp/NetworkMessages.hpp>

#include "Networking.hpp"

#include "Player.hpp"
#include "Inventory.hpp"
#include "Settings.hpp"
#include "Players.hpp"
#include "Script/EventController.hpp"

using namespace std;

void Player::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Player>("Player",
                                         "getPosition", &NetActor::getPosition,
                                         "setPosition", &NetActor::setPosition,
                                         "getRotation", &NetActor::getRotation,
                                         "setRotation", &NetActor::setRotation,
                                         "setMomentum", &NetActor::setMomentum,

                                         "getHealth", &NetActor::getHealth,
                                         "setHealth", &NetActor::setHealth,
                                         "getMagicka", &NetActor::getMagicka,
                                         "setMagicka", &NetActor::setMagicka,
                                         "getFatigue", &NetActor::getFatigue,
                                         "setFatigue", &NetActor::setFatigue,

                                         "getCell", &NetActor::getCell,
                                         "getInventory", &NetActor::getInventory,

                                         "getPreviousCellPos", &Player::getPreviousCellPos,

                                         "kick", &Player::kick,
                                         "ban", &Player::ban,
                                         "address", sol::property(&Player::getIP),

                                         "getAvgPing", &Player::getAvgPing,

                                         "message", &Player::message,
                                         "joinChannel", &Player::joinChannel,
                                         "cleanChannel", &Player::cleanChannel,
                                         "renameChannel", &Player::renameChannel,
                                         "closeChannel", &Player::closeChannel,
                                         "leaveChannel", &Player::leaveChannel,
                                         "setChannel", &Player::setChannel,
                                         "isChannelOpen", &Player::isChannelOpen,

                                         "pid", sol::readonly_property(&Player::id),
                                         "guid", sol::readonly_property(&Player::getGUID),

                                         "name", sol::property(&Player::getName, &Player::setName),
                                         "setCharGenStages", &Player::setCharGenStages,
                                         "level", sol::property(&Player::getLevel, &Player::setLevel),
                                         "gender", sol::property(&Player::getGender, &Player::setGender),
                                         "race", sol::property(&Player::getRace, &Player::setRace),
                                         "head", sol::property(&Player::getHead, &Player::setHead),
                                         "hair", sol::property(&Player::getHair, &Player::setHair),
                                         "birthsign", sol::property(&Player::getBirthsign, &Player::setBirthsign),
                                         "setResetStats", &Player::setResetStats,

                                         "bounty", sol::property(&Player::getBounty, &Player::setBounty),
                                         "reputation", sol::property(&Player::getReputation, &Player::setReputation),
                                         "levelProgress", sol::property(&Player::getLevelProgress, &Player::setLevelProgress),
                                         "creatureRefId", sol::property(&Player::getCreatureRefId, &Player::setCreatureRefId),
                                         "displayCreatureName",  sol::property(&Player::getCreatureNameDisplayState, &Player::setCreatureNameDisplayState),

                                         "resurrect", &Player::resurrect,
                                         "jail", &Player::jail,
                                         "werewolf",  sol::property(&Player::getWerewolfState, &Player::setWerewolfState),
                                         "scale", sol::property(&Player::getScale, &Player::setScale),

                                         "getAttribute", &Player::getAttribute,
                                         "setAttribute", &Player::setAttribute,

                                         "getSkill", &Player::getSkill,
                                         "setSkill", &Player::setSkill,

                                         "getSkillIncrease", &Player::getSkillIncrease,
                                         "setSkillIncrease", &Player::setSkillIncrease,

                                         "getClass", &Player::getCharClass,
                                         "getSettings", &Player::getSettings,
                                         "getBooks", &Player::getBooks,
                                         "getGUI", &Player::getGUI,
                                         "getDialogue", &Player::getDialogue,
                                         "getFactions", &Player::getFactions,
                                         "getQuests", &Player::getQuests,
                                         "getSpells", &Player::getSpells,
                                         "getQuickKeys", &Player::getQuickKeys,
                                         "getWeatherMgr", &Player::getWeatherMgr,

                                         "getMark", &Player::getMark,
                                         "setMark", &Player::setMark,

                                         "getSelectedSpell", &Player::getSelectedSpell,
                                         "setSelectedSpell", &Player::setSelectedSpell,

                                         "cellStateSize", &Player::cellStateSize,
                                         "getCellState", &Player::getCellState,

                                         "setAuthority", &Player::setAuthority,

                                         "storedData", &Player::storedData,
                                         "customData", &Player::customData,
                                         "markedForDeletion", sol::property(&Player::isMarkedForDeletion)
    );

    lua.getState()->new_enum("ChannelAction",
                            "createChannel", 0,
                            "joinChannel", 1,
                            "closeChannel", 2,
                            "leaveChannel", 3);
}

Player::Player(RakNet::RakNetGUID guid) : BasePlayer(guid), NetActor(), cClass(this), settings(this), books(this), gui(this),
                                          dialogue(this), factions(this), quests(this), spells(this), quickKeys(this),
                                          weatherMgr(this)
{
    basePlayer = this;
    netCreature = this;
    printf("Player::Player()\n");
    handshakeCounter = 0;
    loadState = NOTLOADED;
    resetUpdateFlags();

    cell.blank();
    npc.blank();
    npcStats.blank();
    creatureStats.blank();
    charClass.blank();
    scale = 1;
    isWerewolf = false;

    markedForDeletion = false;
    storedData = mwmp::Networking::get().getState().getState()->create_table();
    customData = mwmp::Networking::get().getState().getState()->create_table();
    isActorPlayer = true;
    inUpdateQueue = false;
}

Player::~Player()
{
    printf("Player::~Player()\n");
    CellController::get().deletePlayer(this);
}

void Player::addToUpdateQueue()
{
    if (inUpdateQueue)
        return;
    inUpdateQueue = true;
    Players::addToQueue(this);
}

void Player::update()
{
    auto plPCtrl = mwmp::Networking::get().getPlayerPacketController();

    if (baseInfoChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_BASEINFO);
        packet->setPlayer(basePlayer);
        packet->Send(false);
        packet->Send(true);
    }

    // Make sure we send a cell change before we send the position so the position isn't overridden
    if (cellAPI.isChangedCell())
    {
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_CELL_CHANGE);
        packet->setPlayer(this);
        packet->Send(/*toOthers*/ false);
        cellAPI.resetChangedCell();
    }

    if (positionChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_POSITION);
        packet->setPlayer(basePlayer);
        packet->Send(false);
    }

    if (momentumChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_MOMENTUM);
        packet->setPlayer(basePlayer);
        packet->Send(false);
    }

    if (shapeshiftChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_SHAPESHIFT);
        packet->setPlayer(basePlayer);
        packet->Send(false);
        packet->Send(true);
    }

    // The character class can override values from below on the client, so send it first
    cClass.update();

    if (levelChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_LEVEL);
        packet->setPlayer(basePlayer);
        packet->Send(false);
        packet->Send(true);
    }

    if (statsChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_STATS_DYNAMIC);
        packet->setPlayer(basePlayer);
        packet->Send(false);
        packet->Send(true);

        statsDynamicIndexChanges.clear();
    }

    if (attributesChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_ATTRIBUTE);
        packet->setPlayer(basePlayer);
        packet->Send(false);
        packet->Send(true);

        attributeIndexChanges.clear();
    }

    if (skillsChanged)
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_SKILL);
        packet->setPlayer(basePlayer);
        packet->Send(false);
        packet->Send(true);

        skillIndexChanges.clear();
    }

    if (inventory.isEquipmentChanged())
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_EQUIPMENT);
        packet->setPlayer(this);
        packet->Send(false);
        packet->Send(true);
        inventory.resetEquipmentFlag();
    }

    if (inventory.isInventoryChanged())
    {
        auto packet = plPCtrl->GetPacket(ID_PLAYER_INVENTORY);
        packet->setPlayer(this);
        packet->Send(/*toOthers*/ false);
        inventory.resetInventoryFlag();
    }

    if (changedMarkLocation)
    {
        miscellaneousChangeType = mwmp::MiscellaneousChangeType::MarkLocation;
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_MISCELLANEOUS);
        packet->setPlayer(this);
        packet->Send(false);
        changedMarkLocation = false;
    }

    if (changedSelectedSpell)
    {
        miscellaneousChangeType = mwmp::MiscellaneousChangeType::SelectedSpell;
        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_MISCELLANEOUS);
        packet->setPlayer(this);
        packet->Send(false);
        changedSelectedSpell = false;
    }

    settings.update();
    books.update();
    gui.update();
    dialogue.update();
    factions.update();
    quests.update();
    spells.update();
    quickKeys.update();
    weatherMgr.update();

    resetUpdateFlags();
    inUpdateQueue = false;
}

unsigned short Player::getId()
{
    return id;
}

void Player::setId(unsigned short newId)
{
    this->id = newId;
}

bool Player::isHandshaked()
{
    return handshakeCounter == numeric_limits<int>::max();
}

void Player::setHandshake()
{
    handshakeCounter = numeric_limits<int>::max();
}

void Player::incrementHandshakeAttempts()
{
    handshakeCounter++;
}

int Player::getHandshakeAttempts()
{
    return handshakeCounter;
}

int Player::getLoadState()
{
    return loadState;
}

void Player::setLoadState(int state)
{
    loadState = state;
}

CellController::TContainer *Player::getCells()
{
    return &cells;
}

void Player::forEachLoaded(std::function<void(Player *pl, Player *other)> func)
{
    std::list <Player*> plList;

    for (auto &&cell : cells)
    {
        for (auto &&pl : *cell)
        {
            if (pl != nullptr && !pl->npc.mName.empty())
                plList.push_back(pl);
        }
    }

    plList.sort();
    plList.unique();

    for (auto &&pl : plList)
    {
        if (pl == this) continue;
        func(this, pl);
    }
}

void Player::sendToLoaded(mwmp::PlayerPacket &myPacket)
{
    static std::set<Player*> plList;
    static CellController::TContainer *addrCells = nullptr;

    if (addrCells != &cells)
    {
        addrCells = &cells;
        plList.clear();
        for (auto &cell : cells)
            for (auto &pl : *cell)
                plList.insert(pl);
    }

    for (auto &pl : plList)
    {
        if (pl == this) continue;
        myPacket.setPlayer(this);
        myPacket.Send(pl->guid);
    }
}

void Player::kick() const
{
    mwmp::Networking::getPtr()->kickPlayer(guid);
}

void Player::ban() const
{
    auto netCtrl = mwmp::Networking::getPtr();
    RakNet::SystemAddress addr = netCtrl->getSystemAddress(guid);
    netCtrl->banAddress(addr.ToString(false));
}

void Player::cleanChannel(unsigned channelId)
{
    chat.action = mwmp::Chat::Action::clear;
    chat.channel = channelId;
    chat.message.clear();

    auto packet = mwmp::Networking::get().getPlayerPacketController();
    packet->GetPacket(ID_CHAT_MESSAGE)->setPlayer(this);
    packet->GetPacket(ID_CHAT_MESSAGE)->Send(false);
}

std::string Player::getIP() const
{
    RakNet::SystemAddress addr = mwmp::Networking::getPtr()->getSystemAddress(guid);
    return addr.ToString(false);
}

int Player::getAvgPing()
{
    return mwmp::Networking::get().getAvgPing(guid);
}

std::string Player::getName()
{
    return npc.mName;
}

void Player::setName(const std::string &name)
{
    npc.mName = name;
    baseInfoChanged = true;
}

void Player::setCharGenStages(int currentStage, int endStage)
{
    charGenState.currentStage = currentStage;
    charGenState.endStage = endStage;
    charGenState.isFinished = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_CHARGEN);
    packet->setPlayer(this);
    packet->Send(false);
}

void Player::message(unsigned channelId, const std::string &message, bool toAll)
{
    if (isChannelOpen(channelId))
    {
        mwmp::Chat tmp;
        tmp.action = mwmp::Chat::Action::print;
        tmp.channel = channelId;
        tmp.message = message;
        chat = tmp;

        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
        packet->setPlayer(this);

        packet->Send(false);
        if (toAll)
        {
            auto channel = mwmp::Networking::get().getChannel(channelId);

            for (auto it = channel->members.begin(); it != channel->members.end();)
            {
                if (auto member = it->lock())
                {
                    ++it;
                    if (member->guid == this->guid)
                        continue;
                    member->chat = tmp;
                    packet->setPlayer(member.get());
                    packet->Send(false);

                }
                else
                    it = channel->members.erase(it);
            }
        }
    }
}

bool Player::joinChannel(unsigned channelId, const std::string &name)
{
    auto channel = mwmp::Networking::get().getChannel(channelId);

    if (channel == nullptr) // channel not found
        return false;

    for (const auto &weakMember : channel->members)
    {
        if (auto member = weakMember.lock())
        {
            if (member->guid == guid)
                return false;  // the player is already a member of the channel
        }
    }
    auto thisPl = Players::getPlayerByGUID(guid);
    channel->members.push_back(thisPl);

    chat.action = mwmp::Chat::Action::addchannel;
    chat.channel = channelId;
    chat.message = name;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
    packet->setPlayer(this);
    packet->Send(false);

    return true;
}

void Player::renameChannel(unsigned channelId, const std::string &name)
{
    if (isChannelOpen(channelId))
    {
        chat.action = mwmp::Chat::Action::renamechannel;
        chat.channel = channelId;
        chat.message = name;

        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
        packet->setPlayer(this);

        packet->Send(false);
    }
}

void Player::leaveChannel(unsigned channelId)
{
    chat.action = mwmp::Chat::Action::closechannel;
    chat.channel = channelId;
    chat.message.clear();

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
    packet->setPlayer(this);

    packet->Send(false);

    mwmp::Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_CHANNEL_ACTION>(this, channelId, 2); // left the channel
}

void Player::closeChannel(unsigned channelId)
{
    auto channel = mwmp::Networking::get().getChannel(channelId);

    for (auto &weakMember : channel->members) // kick the channel's members before deleting it
    {
        if (auto member = weakMember.lock())
            member->leaveChannel(channelId);
    }

    if (!mwmp::Networking::get().closeChannel(channelId)) // cannot close channel
        return;

    mwmp::Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_CHANNEL_ACTION>(this, channelId, 2); // channel closed
}

void Player::setChannel(unsigned channelId)
{
    if (isChannelOpen(channelId))
    {
        chat.action = mwmp::Chat::Action::setchannel;
        chat.channel = channelId;
        chat.message.clear();

        auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_CHAT_MESSAGE);
        packet->setPlayer(this);

        packet->Send(false);
    }
}

bool Player::isChannelOpen(unsigned channelId)
{
    auto channel = mwmp::Networking::get().getChannel(channelId);

    auto it = std::find_if(channel->members.begin(), channel->members.end(), [this](const auto &weakMember){
        if (auto member = weakMember.lock())
            return member->guid == guid;
        return false;
    });
    return it != channel->members.end();
}

int Player::getLevel() const
{
    return creatureStats.mLevel;
}

void Player::setLevel(int level)
{
    creatureStats.mLevel = level;
    levelChanged = true;
}

int Player::getGender() const
{
    return npc.isMale();
}

void Player::setGender(int gender)
{
    npc.setIsMale(gender);
    baseInfoChanged = true;
}

std::string Player::getRace() const
{
    return npc.mRace;
}

void Player::setRace(const std::string &race)
{
    LOG_MESSAGE_SIMPLE(Log::LOG_VERBOSE, "Setting race for %s: %s -> %s", npc.mName.c_str(), npc.mRace.c_str(), race.c_str());

    npc.mRace = race;
    baseInfoChanged = true;
}

std::string Player::getHead() const
{
    return npc.mHead;
}

void Player::setHead(const std::string &head)
{
    npc.mHead = head;
    baseInfoChanged = true;
}

std::string Player::getHair() const
{
    return npc.mHair;
}

void Player::setHair(const std::string &hair)
{
    npc.mHair = hair;
    baseInfoChanged = true;
}

std::string Player::getBirthsign() const
{
    return birthsign;
}

void Player::setBirthsign(const std::string &sign)
{
    birthsign = sign;
    baseInfoChanged = true;
}

void Player::setResetStats(bool state)
{
    resetStats = state;
    baseInfoChanged = true;
}

int Player::getBounty() const
{
    return npcStats.mBounty;
}

void Player::setBounty(int bounty)
{
    npcStats.mBounty = bounty;
    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_BOUNTY);
    packet->setPlayer(this);
    packet->Send(false);
    packet->Send(true);
}

int Player::getReputation() const
{
    return npcStats.mReputation;
}

void Player::setReputation(int reputation, bool toOthers)
{
    npcStats.mReputation = reputation;
    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_REPUTATION);
    packet->setPlayer(this);
    packet->Send(false);

    if (toOthers)
      packet->Send(true);
}

int Player::getLevelProgress() const
{
    return npcStats.mLevelProgress;
}

void Player::setLevelProgress(int progress)
{
    npcStats.mLevelProgress = progress;
    levelChanged = true;
}

std::string Player::getCreatureRefId() const
{
    return creatureRefId;
}

void Player::setCreatureRefId(const std::string &refId)
{
    creatureRefId = refId;
    shapeshiftChanged = true;
}

bool Player::getCreatureNameDisplayState() const
{
    return displayCreatureName;
}

void Player::setCreatureNameDisplayState(bool useName)
{
    displayCreatureName = useName;
    shapeshiftChanged = true;
}

std::tuple<int, int> Player::getAttribute(unsigned short attributeId) const
{
    if (attributeId >= ESM::Attribute::Length)
        return make_tuple(0, 0);

    return make_tuple(creatureStats.mAttributes[attributeId].mBase, creatureStats.mAttributes[attributeId].mMod);
}

void Player::setAttribute(unsigned short attributeId, int base, bool clearModifier)
{
    if (attributeId >= ESM::Attribute::Length)
        return;

    creatureStats.mAttributes[attributeId].mBase = base;

    if (clearModifier)
        creatureStats.mAttributes[attributeId].mMod = 0;

    if (!Utils::vectorContains(&attributeIndexChanges, attributeId))
        attributeIndexChanges.push_back(attributeId);

    attributesChanged = true;
}

std::tuple<int, int, float> Player::getSkill(unsigned short skillId) const
{
    if (skillId >= ESM::Skill::Length)
        return make_tuple(0, 0, 0.0f);

    const auto &skill = npcStats.mSkills[skillId];

    return make_tuple(skill.mBase, skill.mMod, skill.mProgress);
}

void Player::setSkill(unsigned short skillId, int base, bool clearModifier, float progress)
{
    if (skillId >= ESM::Skill::Length)
        return;

    auto &skill = npcStats.mSkills[skillId];
    skill.mBase = base;
    if (clearModifier)
        skill.mMod = 0;
    skill.mProgress = progress;

    if (!Utils::vectorContains(&skillIndexChanges, skillId))
        skillIndexChanges.push_back(skillId);

    skillsChanged = true;
}

int Player::getSkillIncrease(unsigned short attributeId) const
{
    return npcStats.mSkillIncrease[attributeId];
}

void Player::setSkillIncrease(unsigned short attributeId, int increase)
{
    if (attributeId >= ESM::Attribute::Length)
        return;

    npcStats.mSkillIncrease[attributeId] = increase;

    if (!Utils::vectorContains(&attributeIndexChanges, attributeId))
        attributeIndexChanges.push_back(attributeId);

    attributesChanged = true;
}

CharClass &Player::getCharClass(sol::this_state thisState)
{
    return cClass;
}

GameSettings &Player::getSettings()
{
    return settings;
}

Books &Player::getBooks()
{
    return books;
}

GUI &Player::getGUI()
{
    return gui;
}

Dialogue &Player::getDialogue()
{
    return dialogue;
}

Factions &Player::getFactions()
{
    return factions;
}

Quests &Player::getQuests()
{
    return quests;
}

Spells &Player::getSpells()
{
    return spells;
}

QuickKeys &Player::getQuickKeys()
{
    return quickKeys;
}

WeatherMgr &Player::getWeatherMgr()
{
    return weatherMgr;
}

std::tuple<float, float, float> Player::getPreviousCellPos() const
{
    return make_tuple(previousCellPosition.pos[0], previousCellPosition.pos[1], previousCellPosition.pos[2]);
}

void Player::resurrect(unsigned int type)
{
    resurrectType = static_cast<mwmp::ResurrectType>(type);

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_RESURRECT);
    packet->setPlayer(this);
    packet->Send(false);
    packet->Send(true);
}

void Player::jail(int jailDays, bool ignoreJailTeleportation, bool ignoreJailSkillIncreases,
                  const std::string &jailProgressText, const std::string &jailEndText)
{
    this->jailDays = jailDays;
    this->ignoreJailTeleportation = ignoreJailTeleportation;
    this->ignoreJailSkillIncreases = ignoreJailSkillIncreases;
    this->jailProgressText = jailProgressText;
    this->jailEndText = jailEndText;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_JAIL);
    packet->setPlayer(this);
    packet->Send(false);
}

bool Player::getWerewolfState() const
{
    return isWerewolf;
}

void Player::setWerewolfState(bool state)
{
    isWerewolf = state;
    shapeshiftChanged = true;
}

float Player::getScale() const
{
    return scale;
}

void Player::setScale(float newScale)
{
    scale = newScale;
    shapeshiftChanged = true;
}

void Player::setMark(float x, float y, float z, float xRot, float zRot, const std::string &cellDescription)
{
    markPosition.pos[0] = x;
    markPosition.pos[1] = y;
    markPosition.pos[2] = z;

    markPosition.rot[0] = xRot;
    markPosition.rot[2] = zRot;
    markCell = Utils::getCellFromDescription(cellDescription);

    changedMarkLocation = true;
}

std::tuple<float, float, float, float, float, std::string> Player::getMark()
{
    return make_tuple(markPosition.pos[0], markPosition.pos[1], markPosition.pos[2],
                      markPosition.rot[0], markPosition.rot[2], markCell.getDescription());
}

std::string Player::getSelectedSpell()
{
    return selectedSpellId;
}

void Player::setSelectedSpell(const std::string &newSelectedSpell)
{
    selectedSpellId = newSelectedSpell;
    changedSelectedSpell = true;
}

size_t Player::cellStateSize() const
{
    return cellStateChanges.cellStates.size();
}

CellState Player::getCellState(int i)
{
    return CellState(cellStateChanges.cellStates.at(i));
}

void Player::setAuthority()
{
    mwmp::BaseActorList writeActorList;
    writeActorList.cell = cell;
    writeActorList.guid = guid;

    Cell *serverCell = CellController::get().getCell(cell);
    if (serverCell != nullptr)
    {
        serverCell->setAuthority(guid);

        mwmp::ActorPacket *authorityPacket = mwmp::Networking::get().getActorPacketController()->GetPacket(ID_ACTOR_AUTHORITY);
        authorityPacket->setActorList(&writeActorList);
        authorityPacket->Send(writeActorList.guid);

        // Also send this to everyone else who has the cell loaded
        serverCell->sendToLoaded(authorityPacket, &writeActorList);
    }
}

bool Player::isMarkedForDeletion() const
{
    return markedForDeletion;
}
