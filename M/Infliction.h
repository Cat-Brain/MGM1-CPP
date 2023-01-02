#include "Includes.h"



enum InflictionType
{
    NULL_INFLICTION_TYPE,
    POISON,
    BLEED,
    BURNING,
    DEADLY_HUG,
    STUN,
    WET,
    STRENGTHEN
    // Insert more status effects here.
};


struct InflictionSpecs
{
    int damage;
    int deathDamage;
    int damageReduction;
    string name;
    Term::RGB color;
};
InflictionSpecs inflictionSpecs[] = {
    { 0, 0, 0, "NULL INFLICTION TYPE", Term::RGB(255, 25, 2) },
    { 2, 2, 0, "poison", Term::RGB(111, 158, 0)},
    { 3, 1, 0, "bleed", Term::RGB(158, 0, 55)},
    { 10, 5, 0, "burning", Term::RGB(255, 167, 66)},
    { 1, 10, 0, "deadly hug", Term::RGB(41, 255, 198)},
    { 0, 0, 0, "stun", Term::RGB(200, 200, 0)},
    { 0, 0, 15, "wet", Term::RGB(0, 0, 200)},
    { 0, 0, -10, "strengthen", Term::RGB(150, 0, 200)}
};


class Infliction
{
public:
    InflictionType effect;

    Infliction(InflictionType effect) :
        effect(effect)
    {
    }

    Infliction() = default;

    InflictionSpecs GetSpecs()
    {
        return inflictionSpecs[effect];
    }

    int FindDamage()
    {
        return GetSpecs().damage;
    }

    int DeathDamage()
    {
        return GetSpecs().deathDamage;
    }

    int FindDamageReduction()
    {
        return GetSpecs().damageReduction;
    }

    string FindRawName()
    {
        return GetSpecs().name;
    }

    string FindName()
    {
        InflictionSpecs specs = GetSpecs();
        return Term::color_fg(specs.color) + specs.name + T_RESET;
    }

    Term::RGB FindRawColor()
    {
        return GetSpecs().color;
    }

    string FindColor()
    {
        return Term::color_fg(FindRawColor());
    }
};



class StatusEffect
{
public:
    Infliction effect;
    int durationLeft;
    bool shouldBeDestroyed;

    StatusEffect(InflictionType effect, int duration) :
        effect(Infliction(effect)), durationLeft(duration), shouldBeDestroyed(false)
    {}

    StatusEffect() = default;

    int Update()
    {
        durationLeft -= 1;
        if (durationLeft <= 0)
        {
            shouldBeDestroyed = true;
            return effect.DeathDamage();
        }
        return effect.FindDamage();
    }

    int Reduction()
    {
        return effect.FindDamageReduction();
    }

    string Name()
    {
        return effect.FindName();
    }
};