/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* ScriptData
SDName: Orgrimmar
SD%Complete: 100
SDComment: Quest support: 2460, 6566
SDCategory: Orgrimmar
EndScriptData */

/* ContentData
npc_shenthul
npc_thrall_warchief
EndContentData */

#include "scriptPCH.h"

/*######
## npc_shenthul
######*/

enum
{
    QUEST_SHATTERED_SALUTE  = 2460
};

struct npc_shenthulAI : public ScriptedAI
{
    npc_shenthulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool CanTalk;
    bool CanEmote;
    uint32 Salute_Timer;
    uint32 Reset_Timer;
    uint64 playerGUID;

    void Reset()
    {
        CanTalk = false;
        CanEmote = false;
        Salute_Timer = 6000;
        Reset_Timer = 0;
        playerGUID = 0;
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_SALUTE && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
        {
            if (CanEmote)
            {
                pPlayer->AreaExploredOrEventHappens(QUEST_SHATTERED_SALUTE);
                Reset();
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanEmote)
        {
            if (Reset_Timer < diff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(playerGUID))
                {
                    if (pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->FailQuest(QUEST_SHATTERED_SALUTE);
                }
                Reset();
            }
            else Reset_Timer -= diff;
        }

        if (CanTalk && !CanEmote)
        {
            if (Salute_Timer < diff)
            {
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                CanEmote = true;
                Reset_Timer = 60000;
            }
            else Salute_Timer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_shenthul(Creature* pCreature)
{
    return new npc_shenthulAI(pCreature);
}

bool QuestAccept_npc_shenthul(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SHATTERED_SALUTE)
    {
        ((npc_shenthulAI*)pCreature->AI())->CanTalk = true;
        ((npc_shenthulAI*)pCreature->AI())->playerGUID = pPlayer->GetGUID();
    }
    return true;
}

struct SpawnLocation
{
    float m_fX, m_fY, m_fZ;
};

// NOTE: Valley of Honor is explicitly not supposed to receive the buff
static const SpawnLocation aRallyGeneratorLocs[6] =
{
    { 1678.6f, -4355.3f, 61.73f }, // Valley of Strength (North)
    { 1550.3f, -4186.2f, 40.95f }, // Valley of Spirits
    { 1931.2f, -4279.3f, 29.90f }, // Valley of Wisdom
    { 1919.0f, -4117.2f, 43.45f }, // Thrall's Chamber
    { 1804.3f, -4344.9f, -2.9f  }, // The Cleft of Shadow
    { 1881.0f, -4518.5f, 27.74f }  // The Drag
};

enum
{
    SPELL_RALLYING_CRY_DRAGONSLAYER = 22888,
    NPC_RALLY_CRY_GENERATOR_HORDE   = 21001,

    MAX_RALLY_GENERATORS            = 6
};

/*######
## npc_overlord_runthak
######*/

enum
{
    QUEST_FOR_ALL_TO_SEE                = 7491,

    YELL_ONY_REWARD_1_HORDE             = -1900113,
    YELL_ONY_REWARD_2_HORDE             = -1900112,

    GO_ONYXIAS_HEAD_HORDE               = 179556
};

struct npc_overlord_runthakAI : public ScriptedAI
{
    npc_overlord_runthakAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }
    uint32 m_uiTick;
    uint32 m_uiDialogueTimer;
    bool m_bRallyingCryEvent;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        m_uiTick = 0;
        m_uiDialogueTimer = 2000;
        m_bRallyingCryEvent = false;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void StartRallyEvent(ObjectGuid playerGuid)
    {
        m_playerGuid = playerGuid;
        m_bRallyingCryEvent = true;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case 0:
                m_creature->GetMotionMaster()->MovePoint(1, 1541.95f, -4405.1f, 10.244f);
                break;
            case 1:
                m_uiDialogueTimer = 1000; 
                m_uiTick++;
                break;
            case 2:
                m_creature->GetMotionMaster()->MovePoint(3, 1568.0f, -4405.87f, 8.13371f);
                break;
            case 3:
                Reset();
                break;
        }
    }

    void UpdateAI(uint32 const uiDiff)
    {
        if (m_bRallyingCryEvent)
        {
            if (m_uiDialogueTimer <= uiDiff)
            {
                switch (m_uiTick)
                {
                    case 0:
                        m_creature->GetMotionMaster()->MovePoint(0, 1540.2f, -4405.02f, 11.18f, MOVE_PATHFINDING);
                        m_uiDialogueTimer = 30000; //handled by MovementInform
                        break;
                    case 1:
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        {
                            m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                            m_creature->MonsterYellToZone(YELL_ONY_REWARD_1_HORDE, 0, pPlayer);
                        }
                        if (GameObject* pGo = m_creature->FindNearestGameObject(GO_ONYXIAS_HEAD_HORDE, 150.0f))
                        {
                            if (!pGo->isSpawned())
                            {
                                pGo->SetRespawnTime(7200);
                                pGo->Refresh();
                            }
                        }
                        m_uiDialogueTimer = 8000;
                        m_uiTick++;
                        break;
                    case 2:
                        m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                        m_creature->MonsterYellToZone(YELL_ONY_REWARD_2_HORDE);
                        if (GameObject* pGo = m_creature->FindNearestGameObject(GO_ONYXIAS_HEAD_HORDE, 150.0f))
                        {
                            pGo->SetGoState(GO_STATE_ACTIVE);
                        }
                        m_uiDialogueTimer = 7000;
                        m_uiTick++;
                        break;
                    case 3:
                        m_creature->CastSpell(m_creature, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        for (uint8 i = 0; i < MAX_RALLY_GENERATORS; ++i)
                        {
                            if (Creature* pGenerator = m_creature->SummonCreature(NPC_RALLY_CRY_GENERATOR_HORDE, aRallyGeneratorLocs[i].m_fX, aRallyGeneratorLocs[i].m_fY, aRallyGeneratorLocs[i].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                                pGenerator->CastSpell(pGenerator, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        }
                        m_uiDialogueTimer = 2000;
                        m_uiTick++;
                        break;
                    case 4:
                        m_creature->GetMotionMaster()->MovePoint(2, 1570.0f, -4405.89f, 7.63613f);
                        m_uiDialogueTimer = 30000; //handled by MovementInform
                        return;
                }
            }
            else m_uiDialogueTimer -= uiDiff;
        } 

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_overlord_runthak(Creature* pCreature)
{
    return new npc_overlord_runthakAI(pCreature);
}

bool QuestRewarded_npc_overlord_runthak(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_FOR_ALL_TO_SEE)
    {
        if (npc_overlord_runthakAI* pRunthak = dynamic_cast<npc_overlord_runthakAI*>(pCreature->AI()))
            pRunthak->StartRallyEvent(pPlayer->GetObjectGuid());
    }
    return true;
}

/*######
## npc_overlord_saurfang
######*/

enum
{
    SPELL_SF_EXECUTE      = 7160,  //OK
    SPELL_SF_CLEAVE       = 15284, //OK
    SPELL_SF_CHARGE       = 22886, //OK
    SPELL_SF_THUNDERCLAP  = 23931, //?
    SPELL_SF_MORTALSTRIKE = 12294, //?
    SPELL_SF_SAURFANGRAGE = 26339,

    QUEST_LORD_OF_BLACKROCK_HORDE       = 7784,

    YELL_NEF_REWARD_1_HORDE     = -1900106,
    YELL_NEF_REWARD_2_HORDE     = -1900105,

    GO_NEFARIANS_HEAD_HORDE            = 179881,
};

struct npc_overlord_saurfangAI : public ScriptedAI
{
    npc_overlord_saurfangAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiExecute_Timer;
    uint32 m_uiCleave_Timer;
    uint32 m_uiCharge_Timer;
    uint32 m_uiThunderClap_Timer;
    uint32 m_uiMortalStrike_Timer;
    uint32 m_uiSaurfangRage_Timer;
    bool isInCombat;

    uint32 m_uiTick;
    uint32 m_uiDialogueTimer;
    bool m_bRallyingCryEvent;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        isInCombat = false;
        m_uiExecute_Timer = 0;
        m_uiCleave_Timer = 7000;
        m_uiCharge_Timer = 0;
        m_uiThunderClap_Timer = 5000;
        m_uiMortalStrike_Timer = 13000;
        m_uiSaurfangRage_Timer = urand(3000, 8000);

        m_uiTick = 0;
        m_uiDialogueTimer = 2000;
        m_bRallyingCryEvent = false;
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* pWho)
    {
        if (isInCombat == false)
        {
			m_creature->MonsterTextEmote(GetMangosString(-2000319));//"Saurfang's eyes glow red for a brief moment"
            isInCombat = true;
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            m_creature->MonsterSay(GetMangosString(-2000320), 0, 0);//"Is that the best you can do?"
    }

    void StartRallyEvent(ObjectGuid playerGuid)
    {
        m_playerGuid = playerGuid;
        m_bRallyingCryEvent = true;
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP | UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId) override
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case 0:
                m_creature->GetMotionMaster()->MovePoint(1, 1542.73f, -4425.55f, 10.87f);
                break;
            case 1:
                m_uiDialogueTimer = 1000;
                m_uiTick++;
                break;
            case 2:
                m_creature->GetMotionMaster()->MovePoint(3, 1565.79f, -4395.27f, 6.9866f);
                break;
            case 3:
                Reset();
                break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
      if (m_bRallyingCryEvent)
        {
            if (m_uiDialogueTimer <= diff)
            {
                switch (m_uiTick)
                {
                    case 0:
                        m_creature->GetMotionMaster()->MovePoint(0, 1540.54f, -4427.2f, 11.29f, MOVE_PATHFINDING);
                        m_uiDialogueTimer = 30000; //handled by MovementInform
                        break;
                    case 1:
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        {
                            m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                            m_creature->MonsterYellToZone(YELL_NEF_REWARD_1_HORDE, 0, pPlayer);
                        }
                        m_uiDialogueTimer = 10000;
                        m_uiTick++;
                        break;
                    case 2:
                        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                        {
                            m_creature->HandleEmote(EMOTE_ONESHOT_SHOUT);
                            m_creature->MonsterYellToZone(YELL_NEF_REWARD_2_HORDE, 0, pPlayer);
                        }
                        if (GameObject* pGo = m_creature->FindNearestGameObject(GO_NEFARIANS_HEAD_HORDE, 150.0f))
                        {
                            if (!pGo->isSpawned())
                            {
                                pGo->SetRespawnTime(7200);
                                pGo->Refresh();
                            }
                        }
                        m_uiDialogueTimer = 8000;
                        m_uiTick++;
                        break;
                    case 3:
                        if (GameObject* pGo = m_creature->FindNearestGameObject(GO_NEFARIANS_HEAD_HORDE, 150.0f))
                        {
                            pGo->SetGoState(GO_STATE_ACTIVE);
                        }
                        m_uiDialogueTimer = 5000;
                        m_uiTick++;
                        break;
                    case 4:
                        m_creature->CastSpell(m_creature, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        for (uint8 i = 0; i < MAX_RALLY_GENERATORS; ++i)
                        {
                            if (Creature* pGenerator = m_creature->SummonCreature(NPC_RALLY_CRY_GENERATOR_HORDE, aRallyGeneratorLocs[i].m_fX, aRallyGeneratorLocs[i].m_fY, aRallyGeneratorLocs[i].m_fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1000))
                                pGenerator->CastSpell(pGenerator, SPELL_RALLYING_CRY_DRAGONSLAYER, true);
                        }
                        m_uiDialogueTimer = 10000;
                        m_uiTick++;
                        break;
                    case 5:
                        m_creature->GetMotionMaster()->MovePoint(2, 1567.39f, -4394.9f, 6.89f);
                        m_uiDialogueTimer = 30000; //handled by MovementInform
                        return;
                }
            }
            else
                m_uiDialogueTimer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->getVictim()->GetHealth() <= m_creature->getVictim()->GetMaxHealth() * 0.2f && m_uiExecute_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_EXECUTE);
            m_uiExecute_Timer = 2000;
        }
        else m_uiExecute_Timer -= diff;

        if (m_uiMortalStrike_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_MORTALSTRIKE);
            m_uiMortalStrike_Timer = 13000;
        }
        else m_uiMortalStrike_Timer -= diff;

        /*        if (m_uiCleave_Timer < diff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_CLEAVE);
                    m_uiCleave_Timer = 7000;
                }
                else m_uiCleave_Timer -= diff;
        */
        if (m_uiCharge_Timer < diff && m_creature->GetDistance(m_creature->getVictim()->GetPositionX(),
                m_creature->getVictim()->GetPositionY(),
                m_creature->getVictim()->GetPositionZ()) >= 8.0f)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_CHARGE);
            m_uiCharge_Timer = 9000;
        }
        else m_uiCharge_Timer -= diff;
        /*
                if (m_uiThunderClap_Timer < diff)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_THUNDERCLAP);
                    m_uiThunderClap_Timer = 9000;
                }
                else m_uiThunderClap_Timer -= diff;
        */
        if (m_uiSaurfangRage_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SF_SAURFANGRAGE) ;
            m_uiSaurfangRage_Timer = 8000;
        }
        else m_uiSaurfangRage_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_npc_overlord_saurfang(Creature* pCreature)
{
    return new npc_overlord_saurfangAI(pCreature);
}

bool QuestRewarded_npc_overlord_saurfang(Player* pPlayer, Creature* pCreature, Quest const* quest)
{
    if (quest->GetQuestId() == QUEST_LORD_OF_BLACKROCK_HORDE)
    {
        if (npc_overlord_saurfangAI* pSaurfang = dynamic_cast<npc_overlord_saurfangAI*>(pCreature->AI()))
            pSaurfang->StartRallyEvent(pPlayer->GetObjectGuid());
    }
    return true;
}

bool GossipHello_npc_eitrigg(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetGUID());

    if (pPlayer->GetQuestStatus(4941) == QUEST_STATUS_INCOMPLETE)
        pPlayer->AreaExploredOrEventHappens(4941);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());
    return true;
}

void AddSC_orgrimmar()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_shenthul";
    newscript->GetAI = &GetAI_npc_shenthul;
    newscript->pQuestAcceptNPC =  &QuestAccept_npc_shenthul;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_eitrigg";
    newscript->pGossipHello =  &GossipHello_npc_eitrigg;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_overlord_runthak";
    newscript->GetAI = &GetAI_npc_overlord_runthak;
    newscript->pQuestRewardedNPC =  &QuestRewarded_npc_overlord_runthak;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_overlord_saurfang";
    newscript->GetAI = &GetAI_npc_overlord_saurfang;
    newscript->pQuestRewardedNPC = &QuestRewarded_npc_overlord_saurfang;
    newscript->RegisterSelf();
}
