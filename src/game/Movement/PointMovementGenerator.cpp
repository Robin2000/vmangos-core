/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 *
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

#include "PointMovementGenerator.h"
#include "Errors.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "GameObjectAI.h"
#include "PlayerAI.h"
#include "TemporarySummon.h"
#include "World.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Anticheat.h"

//----- Point Movement Generator
template<class T>
void PointMovementGenerator<T>::Initialize(T& unit)
{
    if (!unit.IsStopped())
        unit.StopMoving();

    unit.AddUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    Movement::MoveSplineInit init(unit, "PointMovementGenerator<T>::Initialize");
    init.MoveTo(m_x, m_y, m_z, m_options);
    if (m_speed > 0.0f)
        init.SetVelocity(m_speed);
    if (m_options & MOVE_WALK_MODE)
        init.SetWalk(true);
    if (m_options & MOVE_RUN_MODE)
        init.SetWalk(false);
    if (m_options & MOVE_FLY_MODE)
        init.SetFly();
    if (m_options & MOVE_FALLING)
        init.SetFall();
    if (m_options & MOVE_CYCLIC)
        init.SetCyclic();
    if (m_o > -7.0f)
        init.SetFacing(m_o);
    init.Launch();
}

template<class T>
void PointMovementGenerator<T>::Finalize(T& unit)
{
    unit.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    MovementInform(unit);
}

template<class T>
void PointMovementGenerator<T>::Interrupt(T& unit)
{
    unit.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
}

template<class T>
void PointMovementGenerator<T>::Reset(T& unit)
{
    if (!unit.IsStopped())
        unit.StopMoving();

    unit.AddUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
}

template<class T>
bool PointMovementGenerator<T>::Update(T& unit, uint32 const& /*diff*/)
{
    if (!&unit)
        return false;

    if (unit.HasUnitState(UNIT_STAT_CAN_NOT_MOVE))
    {
        unit.ClearUnitState(UNIT_STAT_ROAMING_MOVE);
        return true;
    }

    unit.AddUnitState(UNIT_STAT_ROAMING_MOVE);

    if (!unit.movespline->Finalized() && m_recalculateSpeed)
    {
        m_recalculateSpeed = false;
        Initialize(unit);
    }
    return !unit.movespline->Finalized();
}

template<>
void PointMovementGenerator<Player>::MovementInform(Player& player)
{
    if (!player.IsAlive())
        return;

    if (player.AI())
        player.AI()->MovementInform(POINT_MOTION_TYPE, m_id);
}

template <>
void PointMovementGenerator<Creature>::MovementInform(Creature& unit)
{
    if (!unit.IsAlive())
        return;

    if (unit.AI())
        unit.AI()->MovementInform(POINT_MOTION_TYPE, m_id);

    if (unit.IsTemporarySummon())
    {
        TemporarySummon* pSummon = (TemporarySummon*)(&unit);
        if (pSummon->GetSummonerGuid().IsCreature())
        {
            if (Creature* pSummoner = unit.GetMap()->GetCreature(pSummon->GetSummonerGuid()))
                if (pSummoner->AI())
                    pSummoner->AI()->SummonedMovementInform(&unit, POINT_MOTION_TYPE, m_id);
        }
        else
        {
            if (GameObject* pSummoner = unit.GetMap()->GetGameObject(pSummon->GetSummonerGuid()))
                if (pSummoner->AI())
                    pSummoner->AI()->SummonedMovementInform(&unit, POINT_MOTION_TYPE, m_id);
        }
    }
}

template void PointMovementGenerator<Player>::Initialize(Player&);
template void PointMovementGenerator<Creature>::Initialize(Creature&);
template void PointMovementGenerator<Player>::Finalize(Player&);
template void PointMovementGenerator<Creature>::Finalize(Creature&);
template void PointMovementGenerator<Player>::Interrupt(Player&);
template void PointMovementGenerator<Creature>::Interrupt(Creature&);
template void PointMovementGenerator<Player>::Reset(Player&);
template void PointMovementGenerator<Creature>::Reset(Creature&);
template bool PointMovementGenerator<Player>::Update(Player&, uint32 const& diff);
template bool PointMovementGenerator<Creature>::Update(Creature&, uint32 const& diff);

template <class T>
bool DistancingMovementGenerator<T>::Update(T& unit, uint32 const& /*diff*/)
{
    if (!&unit)
        return false;
    
    if (unit.HasUnitState(UNIT_STAT_CAN_NOT_MOVE))
    {
        unit.ClearUnitState(UNIT_STAT_ROAMING_MOVE);
        return false;
    }
    
    unit.AddUnitState(UNIT_STAT_ROAMING_MOVE);

    if (!unit.movespline->Finalized() && m_recalculateSpeed)
    {
        m_recalculateSpeed = false;
        Initialize(unit);
    }
    return !unit.movespline->Finalized();
}

template <>
void DistancingMovementGenerator<Creature>::MovementInform(Creature& unit)
{
    if (unit.AI())
        unit.AI()->DoSpellsListCasts(1);
}

template <>
void DistancingMovementGenerator<Player>::MovementInform(Player& /*unit*/)
{

}

template bool DistancingMovementGenerator<Player>::Update(Player&, uint32 const& diff);
template bool DistancingMovementGenerator<Creature>::Update(Creature&, uint32 const& diff);

void AssistanceMovementGenerator::Initialize(Creature& unit)
{
    if (unit.HasUnitState(UNIT_STAT_CAN_NOT_REACT | UNIT_STAT_CAN_NOT_MOVE))
        return;

    if (!unit.IsStopped())
        unit.StopMoving();

    unit.AddUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    Movement::MoveSplineInit init(unit, "AssistanceMovementGenerator::Initialize");
    init.MoveTo(m_x, m_y, m_z, (m_options & (MOVE_PATHFINDING | MOVE_FORCE_DESTINATION)));
    init.SetWalk(true);
    init.SetVelocity(unit.GetFleeingSpeed());
    init.Launch();
}

void AssistanceMovementGenerator::Finalize(Creature& unit)
{
    unit.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);

    unit.SetNoCallAssistance(false);
    unit.CallAssistance();
    if (unit.IsAlive())
        unit.GetMotionMaster()->MoveSeekAssistanceDistract(sWorld.getConfig(CONFIG_UINT32_CREATURE_FAMILY_ASSISTANCE_DELAY));
}

bool EffectMovementGenerator::Update(Unit& unit, uint32 const&)
{
    return !unit.movespline->Finalized();
}

void EffectMovementGenerator::Finalize(Unit& unit)
{
    if (unit.GetTypeId() != TYPEID_UNIT)
        return;

    if (((Creature&)unit).AI() && unit.movespline->Finalized())
        ((Creature&)unit).AI()->MovementInform(EFFECT_MOTION_TYPE, m_id);
    unit.RestoreMovement();
}

//----- Charge Movement Generator
template<class T>
void ChargeMovementGenerator<T>::Initialize(T& unit)
{
    if (!unit.IsStopped())
        unit.StopMoving();
    if (path.getPathType() & PATHFIND_NOPATH)
        return;
    unit.AddUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    unit.m_movementInfo.moveFlags = unit.m_movementInfo.moveFlags & ~MOVEFLAG_MASK_MOVING_OR_TURN;
    Movement::MoveSplineInit init(unit, "ChargeMovementGenerator<T>::Initialize");
    init.Move(&path);
    init.SetVelocity(m_speed);
    init.SetWalk(false);
    init.SetFacingGUID(unit.GetTargetGuid());
    init.Launch();
}

template<class T>
void ChargeMovementGenerator<T>::Finalize(T& unit)
{
    unit.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    if (unit.HasUnitState(UNIT_STAT_PENDING_ROOT))
        unit.AddUnitState(UNIT_STAT_ROOT);
    if (unit.HasUnitState(UNIT_STAT_PENDING_STUNNED))
        unit.AddUnitState(UNIT_STAT_STUNNED);
    if (unit.HasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT))
        unit.SetRooted(true);
    if (m_triggerAttack)
        if (!unit.IsPlayer() || m_victimGuid == unit.ToPlayer()->GetSelectionGuid())
            if (Unit* victim = unit.GetMap()->GetUnit(m_victimGuid))
                unit.Attack(victim, true);
    unit.RestoreMovement();
}

template<class T>
void ChargeMovementGenerator<T>::ComputePath(T& attacker, Unit& victim)
{
    Vector3 attackPos; // attacker position
    Vector3 victimPos; // victim position
    Vector3 chargeVect; // vector of the charge
    Vector3 victimSpd; // speed vector of victim
    float o;

    if (GenericTransport* t = attacker.GetTransport())
        path.SetTransport(t);

    if (m_speed == 0)
        m_speed = attacker.GetSpeed(MOVE_RUN) * 4;
    if (m_speed > 24.0f)
        m_speed = 24.0f;

    attacker.GetPosition(attackPos.x, attackPos.y, attackPos.z);
    victim.GetPosition(victimPos.x, victimPos.y, victimPos.z);
    chargeVect = victimPos - attackPos;
    chargeVect = chargeVect.direction(); // unit vector

    // Base path is to current victim position
    path.calculate(victimPos.x, victimPos.y, victimPos.z, false);

    // Improved path to victim future estimated position
    if (Player* victimPlayer = victim.ToPlayer())
    {
        if (sWorld.getConfig(CONFIG_BOOL_ENABLE_MOVEMENT_EXTRAPOLATION_CHARGE) && 
            victimPlayer->ExtrapolateMovement(victimPlayer->m_movementInfo, 1000, victimSpd.x, victimSpd.y, victimSpd.z, o) &&
            victimPlayer->ExtrapolateMovement(victimPlayer->m_movementInfo, 0, victimPos.x, victimPos.y, victimPos.z, o))
        {
            // Victim speed per sec.
            victimSpd -= victimPos;
            // We get only the component of the speed in the direction of charge vector
            float victimSpeed = victimSpd.dot(chargeVect); // dot product
            if (abs(victimSpeed) > 0.1f)
            {
                float currDistance = path.Length();
                uint32 pathTravelTime;
                // Equation when matching collision distance, to get collision time:
                // m_speed * t = currDistance + victimSpeed * t
                // t = currDistance / (m_speed - victimSpeed)
                if (m_speed > 2 * victimSpeed) // we don't want to reach target if target speed is more than half charge speed
                    pathTravelTime = (uint32)(1000 * currDistance / (m_speed - victimSpeed));
                else
                    pathTravelTime = (uint32)(1000 * 2 * currDistance / m_speed);

                pathTravelTime *= 0.45f; // Attenuation factor (empirical)
                m_extrapolateDelay = (WorldTimer::getMSTime() - victimPlayer->m_movementInfo.stime) + pathTravelTime;
                if (m_extrapolateDelay > 1500) m_extrapolateDelay = 1500;
                if (victimPlayer->ExtrapolateMovement(victimPlayer->m_movementInfo, m_extrapolateDelay, victimPos.x, victimPos.y, victimPos.z, o))
                {
                    victim.UpdateAllowedPositionZ(victimPos.x, victimPos.y, victimPos.z);
                    path.calculate(victimPos.x, victimPos.y, victimPos.z, false);
                    path.UpdateForMelee(&victim, attacker.GetMeleeReach());
                }
            }
            else
                path.UpdateForMelee(&victim, attacker.GetMeleeReach());
        }
    }
    else
    {
        // TODO: PvE victim position prediction?
        // Relocate last path point to hitbox rather than exact position of victim
        path.UpdateForMelee(&victim, attacker.GetMeleeReach());
    }
}

template<class T>
void ChargeMovementGenerator<T>::Interrupt(T& unit)
{
    unit.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    if (unit.HasUnitState(UNIT_STAT_PENDING_ROOT))
        unit.AddUnitState(UNIT_STAT_ROOT);
    if (unit.HasUnitState(UNIT_STAT_PENDING_STUNNED))
        unit.AddUnitState(UNIT_STAT_STUNNED);
    if (unit.HasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT))
        unit.SetRooted(true);
}

template<class T>
void ChargeMovementGenerator<T>::Reset(T& unit)
{
    unit.ClearUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE);
    if (unit.HasUnitState(UNIT_STAT_PENDING_ROOT))
        unit.AddUnitState(UNIT_STAT_ROOT);
    if (unit.HasUnitState(UNIT_STAT_PENDING_STUNNED))
        unit.AddUnitState(UNIT_STAT_STUNNED);
    if (unit.HasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ROOT))
        unit.SetRooted(true);
}

template<class T>
bool ChargeMovementGenerator<T>::Update(T& unit, uint32 const& diff)
{
    if (!&unit)
        return false;

    if (m_scheduleStopMoving)
    {
        unit.StopMoving();
        return false;
    }

    unit.AddUnitState(UNIT_STAT_ROAMING_MOVE);

    if (!unit.movespline->Finalized() && m_recalculateSpeed)
    {
        m_recalculateSpeed = false;
        path.calculate(path.getEndPosition().x, path.getEndPosition().y, path.getEndPosition().z, false);
        Initialize(unit);
    }
    return !unit.movespline->Finalized();
}

template void ChargeMovementGenerator<Player>::ComputePath(Player& attacker, Unit& victim);
template void ChargeMovementGenerator<Creature>::ComputePath(Creature& attacker, Unit& victim);
template void ChargeMovementGenerator<Player>::Initialize(Player&);
template void ChargeMovementGenerator<Creature>::Initialize(Creature&);
template void ChargeMovementGenerator<Player>::Finalize(Player&);
template void ChargeMovementGenerator<Creature>::Finalize(Creature&);
template void ChargeMovementGenerator<Player>::Interrupt(Player&);
template void ChargeMovementGenerator<Creature>::Interrupt(Creature&);
template void ChargeMovementGenerator<Player>::Reset(Player&);
template void ChargeMovementGenerator<Creature>::Reset(Creature&);
template bool ChargeMovementGenerator<Player>::Update(Player&, uint32 const& diff);
template bool ChargeMovementGenerator<Creature>::Update(Creature&, uint32 const& diff);
