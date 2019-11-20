/*
Pandaria
World boss
*/
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"

enum eBosses
{
    BOSS_GALLEON,
};

enum eSpells
{
    SPELL_STOMP                 = 121787,
    SPELL_CANNON_BARRAGE        = 121600,
    SPELL_FIRE_SHOT             = 121673,
    SPELL_EMPALLING_PULL        = 121747,
    SPELL_BERSERK               = 47008,
};

enum eEvents
{
    EVENT_STOMP                 = 1,
    EVENT_CANNON                = 2,
    EVENT_FIRE_SHOT             = 3,
    EVENT_EMPALLING             = 4,
    EVENT_SPAWN                 = 6,
    EVENT_BERSERK               = 7,
};

enum eCreatures
{
    CREATURE_GALLEON           = 62351,
};


class boss_galleon : public CreatureScript
{
public:
    boss_galleon() : CreatureScript("boss_galleon") { }

    CreatureAI* GetAI(Creature* creature) const
    {
        return new boss_galleon_AI(creature);
    }

    struct boss_galleon_AI : public BossAI
    {
        boss_galleon_AI(Creature* creature) : BossAI(creature, BOSS_GALLEON)
        {}
        void Reset()
        {
            events.Reset();
            _Reset();
        }

        void KilledUnit(Unit* u)
        {
        }

        void EnterCombat(Unit* unit)
        {
            events.ScheduleEvent(EVENT_STOMP, 50000);
            events.ScheduleEvent(EVENT_CANNON,25000);
            events.ScheduleEvent(EVENT_SPAWN,60000);
            events.ScheduleEvent(EVENT_FIRE_SHOT,10000);
            events.ScheduleEvent(EVENT_BERSERK,900000);
        }

        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_STOMP:
                    {
                        me->CastSpell(me,SPELL_STOMP,true);
                        events.ScheduleEvent(EVENT_STOMP, 60000);
                        break;
                    }
                    case EVENT_CANNON:
                    {
                        me->CastSpell(me,SPELL_CANNON_BARRAGE,true);
                        events.ScheduleEvent(EVENT_CANNON, 60000);
                        break;
                    }
                    case EVENT_SPAWN:
                    {
                        for (uint8 i=0; i<6;++i)
                             me->SummonCreature(CREATURE_GALLEON, me->GetPositionX()+rand()%5, me->GetPositionY()+3+rand()%5, me->GetPositionZ()+2, 10.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 10000);

                        events.ScheduleEvent(EVENT_SPAWN, 60000);
                        break;
                    }
                    case EVENT_BERSERK:
                    {
                        me->CastSpell(me,SPELL_BERSERK,false);
                        break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };
};

class npc_galleon : public CreatureScript
{
    public:
        npc_galleon() : CreatureScript("npc_galleon") { }

        struct npc_galleonAI : public ScriptedAI
        {
            npc_galleonAI(Creature* creature) : ScriptedAI(creature)
            {
            }
        EventMap events;
        void Reset()
        {
            events.Reset();
        }
        void EnterCombat(Unit* unit)
        {
            events.ScheduleEvent(EVENT_EMPALLING, 50000);
        }
        void UpdateAI(const uint32 diff)
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_EMPALLING:
                    {
                        if(Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                            me->CastSpell(target,SPELL_EMPALLING_PULL,true);
                        events.ScheduleEvent(EVENT_EMPALLING, 60000);
                        break;
                    }
                }
            }
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_galleonAI (creature);
    }
};

void AddSC_boss_galleon()
{
    new boss_galleon();
    new npc_galleon();
}
