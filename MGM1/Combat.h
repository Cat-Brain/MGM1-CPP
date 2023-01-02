#include "Infliction.h"



class Hit
{
public:
    int damage;
    vector<StatusEffect> inflictions;
    int attacker;

    Hit(int damage, vector<StatusEffect> inflictions, int attacker) :
        damage(damage), inflictions(inflictions), attacker(attacker)
    {}

    Hit() = default;
};



class AttackHit
{
public:
    Hit hit, selfHit;
    int unmodifiedDamage, unmodifiedSelfDamage;

    AttackHit(Hit hit, int unmodifiedDamage, Hit selfHit, int unmodifiedSelfDamage) :
        hit(hit), selfHit(selfHit), unmodifiedDamage(unmodifiedDamage), unmodifiedSelfDamage(unmodifiedSelfDamage)
    {}

    AttackHit() = default;
};



class Entity;
class Attack
{
public:
    vector<StatusEffect> procs;
    vector<int> procChances; // Percent
    int damage;
    int damageRand;
    vector<StatusEffect> selfProcs;
    vector<int> selfProcChances; // Percent
    int selfDamage;
    int selfDamageRand;
    int length;
    int timeSinceStart;
    string name;
    vector<Entity*> summons;

    Attack(vector<StatusEffect> procs, vector<int> procChances, int damage, int damageRand,
        vector<StatusEffect> selfProcs, vector<int> selfProcChances, int selfDamage, int selfDamageRand,
        vector<Entity*> summons, int length, string name) :
        procs(procs), procChances(procChances), damage(damage), damageRand(damageRand), selfProcs(selfProcs),
        selfProcChances(selfProcChances), selfDamage(selfDamage), selfDamageRand(selfDamageRand), summons(summons),
        length(length), name(name), timeSinceStart(0)
    { }

    Attack() = default;

    AttackHit RollDamage(int currentIndex, int damageReduction)
    {
        vector<StatusEffect> inflictions = vector<StatusEffect>();
        for (int i = 0; i < procs.size(); i++)
        {
            if (rand() % 100 + 1 <= procChances[i])
            {
                inflictions.push_back(procs[i]);
            }
        }
        vector<StatusEffect> selfInflictions = vector<StatusEffect>();
        for (int i = 0; i < selfProcs.size(); i++)
        {
            if (rand() % 100 + 1 <= selfProcChances[i])
            {
                selfInflictions.push_back(selfProcs[i]);
            }
        }

        int unModifiedDamage = RandRange(damage - damageRand, damage + damageRand);
        int unModifiedSelfDamage = RandRange(selfDamage - selfDamageRand, selfDamage + selfDamageRand);
        return AttackHit(Hit(unModifiedDamage < 0 ? unModifiedDamage - damageReduction : std::max(0, unModifiedDamage - damageReduction),
            inflictions, currentIndex), unModifiedDamage,
            Hit(unModifiedSelfDamage < 0 ? unModifiedSelfDamage + damageReduction : std::max(0, unModifiedSelfDamage + damageReduction),
                selfInflictions, currentIndex), unModifiedSelfDamage);
    }
};



class TurnReturn
{
public:
    Hit hit;
    vector<Entity*> summons;

    TurnReturn(Hit hit, vector<Entity*> summons) :
        hit(hit), summons(summons)
    { }

    TurnReturn() = default;
};



class InflictionResults
{
public:
    vector<int> originalAttackers;
    vector<int> damageFromSources;
    vector<string> names;

    InflictionResults(vector<int> originalAttackers, vector<int> damageFromSources, vector<string> names) :
        originalAttackers(originalAttackers), damageFromSources(damageFromSources), names(names)
    { }

    InflictionResults() = default;
};



class Entity
{
public:
    vector<StatusEffect> inflictions;
    vector<int> inflictionAttackers;
    int health;
    int maxHealth;
    int activeAttack;
    vector<Attack> attacks;
    string name;
    float leech;
    bool summoned;
    bool ally;

    Entity(int health, int maxHealth, vector<Attack> attacks, string name, float leech) :
        inflictions(vector<StatusEffect>()), inflictionAttackers(vector<int>()),
        health(health), maxHealth(maxHealth), activeAttack(0),
        attacks(attacks), name(name), leech(leech), summoned(false), ally(false)
    { }

    Entity() : Entity(0, 0, {}, "", 0.0f) { }

    Attack* CurrentAttack()
    {
        return &attacks[activeAttack];
    }

    void FindNewAttack()
    {

        CurrentAttack()->timeSinceStart = 0;
        activeAttack = RandRange(0, static_cast<int>(attacks.size()) - 1);
        CurrentAttack()->timeSinceStart = 1;
        if (CurrentAttack()->length - CurrentAttack()->timeSinceStart > 0)
            printf("%s starts preparing %s It'll be done in %i turns.\n", name.c_str(), CurrentAttack()->name.c_str(), CurrentAttack()->length - CurrentAttack()->timeSinceStart + 1);
        else
            printf("%s starts preparing %s It'll be done next turn.\n", name.c_str(), CurrentAttack()->name.c_str());
    }

    int FindDamageReduction()
    {
        int damageReduction = 0;
        for (int i = 0; i < inflictions.size(); i++)
            damageReduction += inflictions[i].Reduction();
        return damageReduction;
    }

    TurnReturn TakeTurn(int currentIndex)
    {
        //Hit hit; // REMOVE
        vector<Entity*> newSummons = vector<Entity*>();

        Attack* currentAttack = CurrentAttack();

        if (currentAttack->length <= currentAttack->timeSinceStart)
        {
            AttackHit attackHit = currentAttack->RollDamage(currentIndex, FindDamageReduction());
            newSummons = currentAttack->summons;

            // Enemy hit.
            if (attackHit.hit.damage != 0 || attackHit.hit.inflictions.size() != 0)
            {
                if (attackHit.unmodifiedDamage != attackHit.hit.damage)
                {
                    printf("%s does %s. This attack deals %i damage. It would've done %i if it weren't for inflictions.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.hit.damage, attackHit.unmodifiedDamage);
                }
                else
                {
                    printf("%s does %s. This attack deals %i damage.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.hit.damage);
                }
                for (StatusEffect statusEffect : attackHit.hit.inflictions)
                    printf("This attack inflicts %s for %i turns.\n", statusEffect.Name().c_str(), statusEffect.durationLeft);
            }
            else
            {
                if (attackHit.unmodifiedDamage > 0)
                    printf("%s does %s. %s misses, but it would've done %i if it weren't for inflictions.\n", name.c_str(), currentAttack->name.c_str(), name.c_str(), attackHit.unmodifiedDamage);
                else if (currentAttack->damage != 0 || currentAttack->damageRand != 0)
                    printf("%s does %s. %s misses.\n", name.c_str(), currentAttack->name.c_str(), name.c_str());
            }

            // Self hit:
            if (attackHit.selfHit.damage != 0 || attackHit.selfHit.inflictions.size() != 0)
            {
                if (attackHit.unmodifiedSelfDamage != attackHit.selfHit.damage)
                {
                    printf("%s does %s. This attack deals %i self damage. It would've done %i if it weren't for inflictions.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.selfHit.damage, attackHit.unmodifiedSelfDamage);
                }
                else
                {
                    printf("%s does %s. This attack deals %i damage.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.selfHit.damage);
                }
                for (StatusEffect statusEffect : attackHit.selfHit.inflictions)
                    printf("This attack inflicts %s for %i turns on %s.\n", statusEffect.Name().c_str(), statusEffect.durationLeft, name.c_str());
            }
            else
            {
                if (attackHit.unmodifiedSelfDamage > 0)
                    printf("%s does %s. %s misses its self-damage, but it would've done %i if it weren't for inflictions.\n", name.c_str(), currentAttack->name.c_str(), name.c_str(), attackHit.unmodifiedSelfDamage);
                else if (currentAttack->selfDamage != 0 || currentAttack->selfDamageRand != 0)
                    printf("%s does %s. %s misses its self-damage.\n", name.c_str(), currentAttack->name.c_str(), name.c_str());
            }
            ApplyHit(attackHit.selfHit);

            FindNewAttack();
            return TurnReturn(attackHit.hit, newSummons);
        }
        else
        {
            if (currentAttack->length - currentAttack->timeSinceStart > 1)
                printf("%s continues to prepare their %s. They have %i turns left.\n", name.c_str(), currentAttack->name.c_str(), currentAttack->length - currentAttack->timeSinceStart);
            else
                printf("%s continues to prepare their %s. They will be done next turn.\n", name.c_str(), currentAttack->name.c_str());
            currentAttack->timeSinceStart += 1;
        }
        return TurnReturn(Hit(0, {}, 0), {});
    }

    void ApplyHit(Hit hit)
    {
        health -= hit.damage;

        inflictions.reserve(inflictions.size() + hit.inflictions.size());
        inflictions.insert(inflictions.end(), hit.inflictions.begin(), hit.inflictions.end());

        inflictionAttackers.reserve(inflictionAttackers.size() + hit.inflictions.size());
        inflictionAttackers.insert(inflictionAttackers.end(), hit.inflictions.size(), hit.attacker);
    }

    InflictionResults UpdateInflictions()
    {
        int destroyedThisFrame = 0;

        vector<int> orinalInflictionAttackers(inflictionAttackers);

        vector<int> damageFromSources(inflictionAttackers.size());

        vector<string> respectiveNames(inflictionAttackers.size());

        for (int i = 0; i < inflictions.size(); i++)
        {
            respectiveNames[i] = inflictions[i].Name();
        }

        for (int i = 0; i < orinalInflictionAttackers.size(); i++)
        {
            int damage = inflictions[i - destroyedThisFrame].Update();
            health -= damage;
            damageFromSources[i] += damage;

            if (inflictions[i - destroyedThisFrame].shouldBeDestroyed)
            {
                if (damage > 0)
                    printf("%s's %s infliction has been destroyed but it did %i damage this turn.\n", name.c_str(), inflictions[i - destroyedThisFrame].Name().c_str(), damage);
                else
                    printf("%s's %s infliction has been destroyed and it did no damage this turn.\n", name.c_str(), inflictions[i - destroyedThisFrame].Name().c_str());
                inflictions.erase(i - destroyedThisFrame + inflictions.begin());
                inflictionAttackers.erase(i - destroyedThisFrame + inflictionAttackers.begin());
                destroyedThisFrame += 1;
            }
            else
            {
                int reduction = inflictions[i - destroyedThisFrame].Reduction();
                int duration = inflictions[i - destroyedThisFrame].durationLeft;
                printf("%s is inflicted with %s. It did %s damage this turn. %s%s\n",
                    name.c_str(), inflictions[i - destroyedThisFrame].Name().c_str(), damage > 0 ? to_string(damage).c_str() : "no",
                    reduction != 0 ? (reduction > 0 ? "It will reduce physical attacks by " + to_string(reduction) + ". " : "It will increase physical attacks by " + to_string(-reduction) + ". ").c_str() : "",
                    duration > 1 ? ("It'll be done in " + to_string(duration) + " turns").c_str() : "It'll be done next turn");
            }
        }
        return InflictionResults(orinalInflictionAttackers, damageFromSources, respectiveNames);
    }

    bool IsStunned()
    {
        for (int i = 0; i < inflictions.size(); i++)
            if (inflictions[i].effect.effect == STUN)
                return true;
        return false;
    }
};



class Weapon
{
public:
    int activeAttack;
    vector<Attack> attacks;
    string name;
    float leech;

    Weapon(vector<Attack> attacks, string name, float leech) :
        activeAttack(0), attacks(attacks), name(name), leech(leech)
    { }

    Weapon() = default;

    Attack* CurrentAttack()
    {
        return &attacks[activeAttack];
    }

    void ChangeAttackTo(int newAttack)
    {
        attacks[activeAttack].timeSinceStart = 0;
        activeAttack = newAttack;
        attacks[activeAttack].timeSinceStart = 1;
        if (CurrentAttack()->length - CurrentAttack()->timeSinceStart > 0)
            printf("Max starts preparing %s It'll be done in %i turns.\n", CurrentAttack()->name.c_str(), CurrentAttack()->length - CurrentAttack()->timeSinceStart + 1);
        else
            printf("Max starts preparing %s It'll be done next turn.\n", CurrentAttack()->name.c_str());
    }

    void SwitchAttacks(string startingText, bool nevermindable)
    {
        string turnDialogue = startingText + "'" + attacks[0].name + "' ";
        for (int i = 0; i < attacks.size() - 1; i++)
            turnDialogue += "or '" + attacks[i + 1].name + "' ";

        if (nevermindable)
            turnDialogue += " or 'nevermind'? ";

        string prompt = Input(turnDialogue);

        int inputedAttack = activeAttack;

        bool badInput = true;

        badInput = !(prompt == "nevermind" && nevermindable);

        for (int i = 0; i < attacks.size(); i++)
            if (prompt == attacks[i].name)
            {
                badInput = false;
                inputedAttack = i;
            }

        while (badInput)
        {
            prompt = Input("That won't work this time! Do you want to " + turnDialogue);
            badInput = !(prompt == "nevermind" && nevermindable);

            for (int i = 0; i < attacks.size(); i++)
                if (prompt == attacks[i].name)
                {
                    badInput = false;
                    inputedAttack = i;
                }
        }

        ChangeAttackTo(inputedAttack);
    }

    void LearnAttack(Attack attack)
    {
        attacks.push_back(attack);
    }

    bool KnowsAttack(string name)
    {
        for (Attack attack : attacks)
            if (attack.name == name)
                return true;
        return false;
    }
};



class Player
{
public:
    vector<StatusEffect> inflictions;
    vector<int> inflictionAttackers;
    int maxHealth;
    int health;
    Weapon weapon;
    string currentDeathMessage;
    string name = "Max";

    Player(int maxHealth) :
        inflictions(0), inflictionAttackers(0), maxHealth(maxHealth), health(maxHealth), currentDeathMessage("NULL DEATH MESSAGE")
    { }

    Player() = default;

    int FindDamageReduction()
    {
        int damageReduction = 0;
        for (StatusEffect infliction : inflictions)
            damageReduction += infliction.Reduction();
        return damageReduction;
    }

    Attack* CurrentAttack()
    {
        return weapon.CurrentAttack();
    }

    TurnReturn TakeTurn()
    {
        vector<Entity*> newSummons = vector<Entity*>();

        Attack* currentAttack = CurrentAttack();

        if (currentAttack->length <= currentAttack->timeSinceStart)
        {
            currentAttack->timeSinceStart = 1;
            AttackHit attackHit = currentAttack->RollDamage(0, FindDamageReduction());
            newSummons = currentAttack->summons;

            // Enemy hit.
            if (attackHit.hit.damage != 0 || attackHit.hit.inflictions.size() != 0)
            {
                if (attackHit.unmodifiedDamage != attackHit.hit.damage)
                {
                    printf("%s does %s. This attack deals %i damage. It would've done %i if it weren't for inflictions.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.hit.damage, attackHit.unmodifiedDamage);
                }
                else
                {
                    printf("%s does %s. This attack deals %i damage.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.hit.damage);
                }
                for (StatusEffect statusEffect : attackHit.hit.inflictions)
                    printf("This attack inflicts %s for %i turns.\n", statusEffect.Name().c_str(), statusEffect.durationLeft);
            }
            else
            {
                if (attackHit.unmodifiedDamage > 0)
                    printf("%s does %s. %s misses, but it would've done %i if it weren't for inflictions.\n", name.c_str(), currentAttack->name.c_str(), name.c_str(), attackHit.unmodifiedDamage);
                else if (currentAttack->damage != 0 || currentAttack->damageRand != 0)
                    printf("%s does %s. %s misses.\n", name.c_str(), currentAttack->name.c_str(), name.c_str());
            }

            // Self hit:
            if (attackHit.selfHit.damage != 0 || attackHit.selfHit.inflictions.size() != 0)
            {
                if (attackHit.unmodifiedSelfDamage != attackHit.selfHit.damage)
                {
                    printf("%s does %s. This attack deals %i self damage. It would've done %i if it weren't for inflictions.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.selfHit.damage, attackHit.unmodifiedSelfDamage);
                }
                else
                {
                    printf("%s does %s. This attack deals %i damage.\n",
                        name.c_str(), currentAttack->name.c_str(), attackHit.selfHit.damage);
                }
                for (StatusEffect statusEffect : attackHit.selfHit.inflictions)
                    printf("This attack inflicts %s for %i turns on %s.\n", statusEffect.Name().c_str(), statusEffect.durationLeft, name.c_str());
            }
            else
            {
                if (attackHit.unmodifiedSelfDamage > 0)
                    printf("%s does %s. %s misses its self-damage, but it would've done %i if it weren't for inflictions.\n", name.c_str(), currentAttack->name.c_str(), name.c_str(), attackHit.unmodifiedSelfDamage);
                else if (currentAttack->selfDamage != 0 || currentAttack->selfDamageRand != 0)
                    printf("%s does %s. %s misses its self-damage.\n", name.c_str(), currentAttack->name.c_str(), name.c_str());
            }
            ApplyHit(attackHit.selfHit, false);

            //weapon.FindNewAttack();
            return TurnReturn(attackHit.hit, newSummons);
        }
        else
        {
            if (currentAttack->length - currentAttack->timeSinceStart > 1)
                printf("%s continues to prepare their %s. They have %i turns left.\n", name.c_str(), currentAttack->name.c_str(), currentAttack->length - currentAttack->timeSinceStart);
            else
                printf("%s continues to prepare their %s. They will be done next turn.\n", name.c_str(), currentAttack->name.c_str());
            currentAttack->timeSinceStart += 1;
        }
        return TurnReturn(Hit(0, {}, 0), {});
    }

    void ApplyHit(Hit hit, bool dodged)
    {
        health -= hit.damage;
        for (StatusEffect infliction : hit.inflictions)
        {
            if (infliction.effect.effect != STUN || !dodged)
                inflictions.push_back(infliction);
            else
            {
                StatusEffect halfTimeInfliction = infliction;
                halfTimeInfliction.durationLeft = halfTimeInfliction.durationLeft / 2;
                inflictions.push_back(halfTimeInfliction);
                printf("Because you blocked get stunned for half as long. Which is in this case %i turn.\n", halfTimeInfliction.durationLeft);
            }
        }
        inflictionAttackers.insert(inflictionAttackers.end(), hit.inflictions.size(), hit.attacker);
    }

    InflictionResults UpdateInflictions()
    {
        int destroyedThisFrame = 0;

        vector<int> orinalInflictionAttackers(inflictionAttackers);

        vector<int> damageFromSources(inflictionAttackers.size());

        vector<string> respectiveNames(inflictionAttackers.size());
        for (int i = 0; i < inflictions.size(); i++)
            respectiveNames[i] = inflictions[i].Name();

        for (int i = 0; i < inflictions.size() + destroyedThisFrame; i++)
        {
            int damage = inflictions[i - destroyedThisFrame].Update();
            health -= damage;
            damageFromSources[i] += damage;

            if (inflictions[i - destroyedThisFrame].shouldBeDestroyed)
            {
                if (damage > 0)
                    printf("%s's %s infliction has been destroyed but it did %i damage this turn.\n", name.c_str(), inflictions[i - destroyedThisFrame].Name().c_str(), damage);
                else
                    printf("%s's %s infliction has been destroyed and it did no damage this turn.\n", name.c_str(), inflictions[i - destroyedThisFrame].Name().c_str());
                inflictions.erase(i - destroyedThisFrame + inflictions.begin());
                inflictionAttackers.erase(i - destroyedThisFrame + inflictionAttackers.begin());
                destroyedThisFrame += 1;
            }
            else
            {
                int reduction = inflictions[i - destroyedThisFrame].Reduction();
                int duration = inflictions[i - destroyedThisFrame].durationLeft;
                printf("%s is inflicted with %s. It did %s damage this turn. %s%s\n",
                    name.c_str(), inflictions[i - destroyedThisFrame].Name().c_str(), damage > 0 ? to_string(damage).c_str() : "no",
                    reduction != 0 ? (reduction > 0 ? "It will reduce physical attacks by " + to_string(reduction) + ". " : "It will increase physical attacks by " + to_string(-reduction) + ". ").c_str() : "",
                    duration > 1 ? ("It'll be done in " + to_string(duration) + " turns").c_str() : "It'll be done next turn");
            }
        }
        return InflictionResults(orinalInflictionAttackers, damageFromSources, respectiveNames);
    }

    bool IsStunned()
    {
        for (int i = 0; i < inflictions.size(); i++)
            if (inflictions[i].effect.effect == STUN)
                return true;
        return false;
    }
};