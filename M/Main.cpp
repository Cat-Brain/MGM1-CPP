// Go to https ://github.com/Matthew-12525/Create-Your-Own-Adventure for the unmodded experience. =]
/*
Hiya, this 'mod' was written by Jordan Baumann(me),
however this all wouldn't of existed without Matthew's original version of the game(link above^^^).
I hope that you enjoy this experience, and if you want, maybe you'll even mod this mod! =]
*/

#include <chrono> // <= Nice time lib
#include <thread> // <= Helper library for chrono me thinks.
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
/*
sleep_for(nanoseconds(10));
sleep_until(system_clock::now() + seconds(1));
*/

#include <stdio.h>      // printf, scanf, puts, NULL
#include <stdlib.h>     // srand, rand
#include <time.h>       // time(NULL)
#include <string> // for string class
#include <vector> // For vector class.

using std::string;
using std::vector;

typedef unsigned int uint;


int RandRange(int min, int max)
{
    return (rand() % (max - min)) + min;
}















//Classes:

enum InflictionType
{
    NULL_INFLICTION_TYPE,
    POISON,
    BLEED,
    BURNING,
    DEADLY_HUG,
    STUN,
    WET,
    STRENGHTEN
    // Insert more status effects here.
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

    int FindDamage()
    {
        switch (effect)
        {
        case POISON:
            return 2;
        case BLEED:
            return 3;
        case BURNING:
            return 10;
        case DEADLY_HUG:
            return 1;
        default:
            return 0;
        }
    }

    int DeathDamage()
    {
        switch (effect)
        {
        case POISON:
            return 2;
        case BLEED:
            return 1;
        case BURNING:
            return 5;
        case DEADLY_HUG:
            return 10;
        default:
            return 0;
        }
    }

    int FindDamageReduction()
    {
        switch (effect)
        {
        case WET:
            return 15;
        case STRENGHTEN:
            return -10;
        default:
            return 0;
        }
    }

    string FindName()
    {
        switch (effect)
        {
        case POISON:
            return "poison";
        case BLEED:
            return "bleed";
        case BURNING:
            return "burning";
        case DEADLY_HUG:
            return "deadly hug";
        case STUN:
            return "stun";
        case WET:
            return "wet";
        case STRENGHTEN:
            return "strengthen";
        default:
            return "NULL INFLICTION TYPE";
        }
    }
};



class StatusEffect
{
public:
    Infliction effect;
    int durationLeft;
    bool shouldBeDestroyed;

    StatusEffect(InflictionType effect, int duration):
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



class Hit
{
public:
    int damage;
    vector<StatusEffect> inflictions;
    int attacker;

    Hit(int damage, vector<StatusEffect> inflictions, int attacker):
        damage(damage), inflictions(inflictions), attacker(attacker)
    {}

    Hit() = default;
};



class AttackHit
{
public:
    Hit hit, selfHit;
    int unmodifiedDamage, unmodifiedSelfDamage;
    
    AttackHit(Hit hit, int unmodifiedDamage, Hit selfHit, int unmodifiedSelfDamage):
        hit(hit), selfHit(selfHit), unmodifiedDamage(unmodifiedDamage), unmodifiedSelfDamage(unmodifiedSelfDamage)
    {}

    AttackHit() = default;
};



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
    vector<void*> summons;

    Attack(vector<StatusEffect> procs, vector<int> procChances, int damage, int damageRand,
        vector<StatusEffect> selfProcs, vector<int> selfProcChances, int selfDamage, int selfDamageRand,
        vector<void*> summons, int length, string name) :
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

        int unModifiedDamage = fmaxf(0, RandRange(damage - damageRand, damage + damageRand));
        int unModifiedSelfDamage = fmaxf(0, RandRange(selfDamage - selfDamageRand, selfDamage + selfDamageRand));
        return AttackHit(Hit(fmaxf(0, unModifiedDamage - damageReduction), inflictions, currentIndex), unModifiedDamage,
            Hit(fmaxf(0, unModifiedSelfDamage - damageReduction), selfInflictions, currentIndex), unModifiedSelfDamage);
    }
};



class TurnReturn
{
public:
    Hit hit;
    vector<void*> summons;

    TurnReturn(Hit hit, vector<void*> summons) :
        hit(hit), summons(summons)
    { }

    TurnReturn() = default;
};



class Enemy
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

    Enemy(int health, int maxHealth, vector<Attack> attacks, string name, float leech) :
        inflictions(vector<StatusEffect>()), inflictionAttackers(vector<int>()),
        health(health), maxHealth(maxHealth), activeAttack(0),
        attacks(attacks), name(name), leech(leech), summoned(false)
    { }

    Enemy() = default;

    Attack* CurrentAttack()
    {
        return &attacks[activeAttack];
    }

    void FindNewAttack()
    {

        CurrentAttack()->timeSinceStart = 0;
        activeAttack = RandRange(0, attacks.size() - 1);
        CurrentAttack()->timeSinceStart = 1;
        if (CurrentAttack()->length - CurrentAttack()->timeSinceStart > 0)
            printf("%s starts preparing %s It'll be done in %i turns.", name, CurrentAttack()->name, CurrentAttack()->length - CurrentAttack()->timeSinceStart + 1);
        else
            printf("%s starts preparing %s It'll be done next turn.", name, CurrentAttack()->name);
    }

    int FindDamageReduction()
    {
        int damageReduction = 0;
        for(int i = 0; i < inflictions.size(); i++)
            damageReduction += inflictions[i].Reduction();
        return damageReduction;
    }

    TurnReturn TakeTurn(int currentIndex)
    {
        //Hit hit; // REMOVE
        vector<void*> enemiesBorn = vector<void*>();

        Attack* currentAttack = CurrentAttack();

        if (currentAttack->length <= currentAttack->timeSinceStart)
        {
            AttackHit attackHit = currentAttack->RollDamage(currentIndex, FindDamageReduction());
            enemiesBorn = currentAttack->summons;
            if (attackHit.hit.damage != 0 || attackHit.hit.inflictions.size() != 0)
            {
                if (attackHit.unmodifiedDamage != attackHit.hit.damage)
                {
                    printf("%s does %s. This attack deals %i damage. It would've done %i if it weren't for inflictions.",
                        name, currentAttack->name, attackHit.hit.damage, attackHit.unmodifiedDamage);
                }
                else
                {
                    printf("%s does %s. This attack deals %i damage.",
                        name, currentAttack->name, attackHit.hit.damage);
                }
                for (StatusEffect statusEffect : attackHit.hit.inflictions)
                    printf("This attack inflicts %s for %i turns.", statusEffect.Name(), statusEffect.durationLeft);
            }
            else
            {
                if (attackHit.unmodifiedDamage > 0)
                    printf("%s does %s. %s misses, but it would've done %i if it weren't for inflictions.", name, currentAttack->name, name, attackHit.unmodifiedDamage);
                else if (currentAttack->damage != 0 || currentAttack->damageRand != 0)
                    printf("%s does %s. %s misses.", name, currentAttack->name, name);
            }

if selfHit.damage != 0 or selfHit.inflictions != [] :
    healOrDeal = "deals " + str(selfHit.damage) + " damage to"
    if selfHit.damage < 0 :
        healOrDeal = "heals for " + str(-selfHit.damage) + " health points on"

        if unmodifiedSelfDamage != selfHit.damage :
            print(self.name + " does " + self.CurrentAttack().name + ".\n\
This attack " + healOrDeal + " themselve. It would've done " + str(unmodifiedSelfDamage) + " if it weren't for inflictions.")
        else:
print(self.name + " does " + self.CurrentAttack().name + ".\n\
This attack " + healOrDeal + " themselve.")
for infliction in selfHit.inflictions :
    print("This attack inflicts " + infliction.Name() + " on themselve for " + str(infliction.durationLeft) + " turns.")
    self.ApplyHit(selfHit)

    self.FindNewAttack()
        }

    else:
hit = Hit(0, [], currentIndex)
if self.CurrentAttack().length - self.CurrentAttack().timeSinceStart > 1:
print(self.name + " continues to prepare their " + self.CurrentAttack().name + ". They have " + str(self.CurrentAttack().length - self.CurrentAttack().timeSinceStart) + " turns left.")
else :
    print(self.name + " continues to prepare their " + self.CurrentAttack().name + ". They will be done next turn.")
    self.attacks[self.activeAttack].timeSinceStart += 1

    return hit, enemiesBorn

    def ApplyHit(self, hit : Hit) :
    self.health -= hit.damage
    self.inflictions.extend(deepcopy(hit.inflictions))
    self.inflictionAttackers.extend([hit.attacker] * len(hit.inflictions))

    def UpdateInflictions(self) :
    destroyedThisFrame = 0

    damageFromSources = [0] * len(self.inflictionAttackers)

    respectiveNames = [""] * len(self.inflictionAttackers)
    for i in range(len(self.inflictions)) :
        respectiveNames[i] = self.inflictions[i].Name()

        orinalInflictionAttackers = deepcopy(self.inflictionAttackers)

        for i in range(len(self.inflictions)) :
            damage = self.inflictions[i - destroyedThisFrame].Update()
            self.health -= damage
            damageFromSources[i] += damage

            if self.inflictions[i - destroyedThisFrame].shouldBeDestroyed :
                if damage > 0:
print(self.name + "'s " + self.inflictions[i - destroyedThisFrame].Name() + " infliction has been destroyed but it did " + str(damage) + " damage this turn.")
                else :
                    print(self.name + "'s " + self.inflictions[i - destroyedThisFrame].Name() + " infliction has been destroyed and did no damage this turn.")
                    del self.inflictions[i - destroyedThisFrame]
                    del self.inflictionAttackers[i - destroyedThisFrame]
                    destroyedThisFrame += 1
            else:
damageThisTurn = ""
reductionThisTurn = ""
durationThisTurn = "It'll be gone next turn."
if damage > 0:
damageThisTurn = "It did " + str(damage) + " damage this turn.\n"
reduction = self.inflictions[i - destroyedThisFrame].Reduction()
if reduction > 0:
reductionThisTurn = "It will reduce physical attacks by " + str(reduction) + ".\n"
elif reduction < 0 :
    reductionThisTurn = "It will increase physical attacks by " + str(-reduction) + ".\n"
    duration = self.inflictions[i - destroyedThisFrame].durationLeft
    if duration > 1 :
        durationThisTurn = "It'll be gone in " + str(duration) + " turns."
        print(self.name + " is inflicted with " + self.inflictions[i - destroyedThisFrame].Name() + ".\n" + damageThisTurn + reductionThisTurn + durationThisTurn)

        return orinalInflictionAttackers, damageFromSources, respectiveNames

        def IsStunned(self) :
        for infliction in self.inflictions :
            if infliction.effect.effect == InflictionType.STUN :
                return True
                return False
    }
};



                class Weapon :
            activeAttack: int
                attacks : Attack
                name : str
                leech : float

                def __init__(self, attacks : Attack, name : str, leech : float) :
                self.activeAttack = 0
                self.attacks = deepcopy(attacks)
                self.name = name
                self.leech = leech

                def CurrentAttack(self) :
                return self.attacks[self.activeAttack]

                def ChangeAttackTo(self, newAttack : int) :
                self.attacks[self.activeAttack].timeSinceStart = 0
                self.activeAttack = newAttack
                self.attacks[self.activeAttack].timeSinceStart = 1
                if self.CurrentAttack().length - self.CurrentAttack().timeSinceStart > 0:
print("Max starts preparing " + self.CurrentAttack().name + " It'll be done in " + str(self.CurrentAttack().length - self.CurrentAttack().timeSinceStart + 1) + " turns.")
                else:
print("Max starts preparing " + self.CurrentAttack().name + " It'll be done next turn.")

def SwitchAttacks(self, startingText : str, nevermindable : bool) :
    turnDialogue = startingText + "'" + self.attacks[0].name + "' "
    for attack in range(len(self.attacks) - 1) :
        turnDialogue += "or '" + self.attacks[attack + 1].name + "' "

        if nevermindable :
            turnDialogue += " or 'nevermind'? "

            prompt = input(turnDialogue)

            inputedAttack = 0

            badInput = True

            if prompt == "nevermind" and nevermindable:
badInput = False
inputedAttack = self.activeAttack

for currentAttack in range(len(self.attacks)) :
    if prompt == self.attacks[currentAttack].name :
        badInput = False
        inputedAttack = currentAttack

        while badInput :
            prompt = input("That won't work this time! Do you want to " + turnDialogue)
            badInput = ((prompt != "nevermind") or (not nevermindable))
            for currentAttack in range(len(self.attacks)) :
                if prompt == self.attacks[currentAttack].name :
                    badInput = False
                    inputedAttack = currentAttack

                    self.ChangeAttackTo(inputedAttack)

                    def LearnAttack(self, attack : Attack) :
                    self.attacks.append(attack)

                    def KnowsAttack(self, name : str) :
                    for attack in self.attacks :
                        if attack.name == name :
                            return True
                            return False



                            class Player :
                        inflictions: StatusEffect
                            inflictionAttackers : int
                            maxHealth : int # More like Max'S health, am I right!...
                            currentHealth : int
                            weapon : Weapon
                            currentDeathMessage : str

                            def __init__(self, maxHealth : int) :
                            self.inflictions = []
                            self.inflictionAttackers = []
                            self.maxHealth = maxHealth
                            self.currentHealth = maxHealth
                            self.currentDeathMessage = "NULL DEATH MESSAGE"

                            def FindDamageReduction(self) :
                            damageReduction = 0
                            for infliction in self.inflictions :
                                damageReduction += infliction.Reduction()
                                return damageReduction

                                def TakeTurn(self) :
                                hit : Hit

                                if self.weapon.CurrentAttack().length <= self.weapon.CurrentAttack().timeSinceStart :
                                    hit, unmodifiedDamage, selfHit, unmodifiedSelfDamage = self.weapon.CurrentAttack().RollDamage(0, self.FindDamageReduction())
                                    if hit.damage != 0 or hit.inflictions != [] :
                                        if unmodifiedDamage != hit.damage :
                                            print("Max uses their " + self.weapon.name + " and does " + self.weapon.CurrentAttack().name + ".\n\
This attack deals " + str(hit.damage) + " damage. It would've done " + str(unmodifiedDamage) + " if it weren't for inflictions.")
                                        else :
                                            print("Max uses their " + self.weapon.name + " and does " + self.weapon.CurrentAttack().name + ".\n\
This attack deals " + str(hit.damage) + " damage.")
for infliction in hit.inflictions :
    print("This attack inflicts " + infliction.Name() + " for " + str(infliction.durationLeft) + " turns.")
                                    else:
if unmodifiedDamage > 0:
print("Max's " + self.weapon.name + " misses, but it would've done " + str(unmodifiedDamage) + " if it weren't for inflictions.")
else :
    print("Max's " + self.weapon.name + " misses.")

    self.weapon.attacks[self.weapon.activeAttack].timeSinceStart = 0

    if selfHit.damage != 0 or selfHit.inflictions != [] :
        healOrDeal = "deals " + str(selfHit.damage) + " damage to"
        if selfHit.damage < 0 :
            healOrDeal = "heals for " + str(-selfHit.damage) + " health points on"

            if unmodifiedSelfDamage != selfHit.damage :
                print("Max does " + self.CurrentAttack().name + ".\n\
This attack " + healOrDeal + " themselve. It would've done " + str(unmodifiedSelfDamage) + " if it weren't for inflictions.")
            else:
print("Max does " + self.CurrentAttack().name + ".\n\
This attack " + healOrDeal + " themselve.")
for infliction in selfHit.inflictions :
    print("This attack inflicts " + infliction.Name() + " on themselve for " + str(infliction.durationLeft) + " turns.")
    self.ApplyHit(selfHit)


        else:
hit = Hit(0, [], 0)
if self.weapon.CurrentAttack().length - self.weapon.CurrentAttack().timeSinceStart > 1:
print("Max continues to prepare their " + self.weapon.name + "'s " + self.weapon.CurrentAttack().name + ". They have " + str(self.weapon.CurrentAttack().length - self.weapon.CurrentAttack().timeSinceStart) + " turns left.")
else :
    print("Max continues to prepare their " + self.weapon.name + "'s " + self.weapon.CurrentAttack().name + ". They will be done next turn.")

    self.weapon.attacks[self.weapon.activeAttack].timeSinceStart += 1

    return hit

    def ApplyHit(self, hit : Hit, dodged : bool) :
    self.currentHealth -= hit.damage
    for infliction in deepcopy(hit.inflictions) :
        if infliction.effect.effect != InflictionType.STUN or not dodged :
            self.inflictions.append(infliction)
        else :
            halfTimeInfliction = infliction
            halfTimeInfliction.durationLeft = int(floor(float(halfTimeInfliction.durationLeft) / 2))
            self.inflictions.append(halfTimeInfliction)
            print("Because you blocked get stunned for half as long. Which is in this case " + str(halfTimeInfliction.durationLeft) + " turn.")
            self.inflictionAttackers.extend([hit.attacker] * len(hit.inflictions))

            def UpdateInflictions(self) :
            destroyedThisFrame = 0

            damageFromSources = [0] * len(self.inflictionAttackers)

            respectiveNames = [""] * len(self.inflictionAttackers)
            for i in range(len(self.inflictions)) :
                respectiveNames[i] = self.inflictions[i].Name()

                orinalInflictionAttackers = deepcopy(self.inflictionAttackers)

                for i in range(len(self.inflictions)) :
                    damage = self.inflictions[i - destroyedThisFrame].Update()
                    self.currentHealth -= damage
                    damageFromSources[i] += damage

                    if self.inflictions[i - destroyedThisFrame].shouldBeDestroyed :
                        if damage > 0:
print("Your " + self.inflictions[i - destroyedThisFrame].Name() + " infliction has been destroyed but it did " + str(damage) + " damage this turn.")
                        else :
                            print("Your " + self.inflictions[i - destroyedThisFrame].Name() + " infliction has been destroyed and did no damage this turn.")
                            del self.inflictions[i - destroyedThisFrame]
                            del self.inflictionAttackers[i - destroyedThisFrame]
                            destroyedThisFrame += 1

                    else:
damageThisTurn = ""
reductionThisTurn = ""
durationThisTurn = "It'll be gone next turn."
if damage > 0:
damageThisTurn = "It did " + str(damage) + " damage this turn.\n"
reduction = self.inflictions[i - destroyedThisFrame].Reduction()
if reduction > 0:
reductionThisTurn = "It will reduce physical attacks by " + str(reduction) + ".\n"
elif reduction < 0 :
    reductionThisTurn = "It will increase physical attacks by " + str(-reduction) + ".\n"
    duration = self.inflictions[i - destroyedThisFrame].durationLeft
    if duration > 1 :
        durationThisTurn = "It'll be gone in " + str(duration) + " turns."
        print("Max is inflicted with " + self.inflictions[i - destroyedThisFrame].Name() + ".\n" + damageThisTurn + reductionThisTurn + durationThisTurn)

        return orinalInflictionAttackers, damageFromSources, respectiveNames

        def IsStunned(self) :
        for infliction in self.inflictions :
            if infliction.effect.effect == InflictionType.STUN :
                return True
                return False



                class Settings :
            sleepTime: int

                def __init__(self, sleepTime : int) :
                self.sleepTime = sleepTime






















                def FindSettings() :
                global currentSettings
                prompt = input("How many seconds do you want to wait after key events('default' = 1) ")
                goodInput = prompt.isnumeric()
                if goodInput :
                    goodInput = int(prompt) >= 0
                    goodInput |= prompt == "default"
                    while not goodInput :
                        prompt = input("It has to be a number or 'default'. How many seconds do you want to wait after key events('default' = 1) ")
                        goodInput = prompt.isnumeric()
                        if goodInput :
                            goodInput = int(prompt) >= 0
                            goodInput |= prompt == "default"
                            if prompt == "default" :
                                currentSettings = Settings(1)
                            else :
                                currentSettings = Settings(int(prompt))




                                def weaponSelect() :
                                global player

                                weaponChoice = input("Do you take a 'bow', 'axe', or 'sword'?: ")
                                while weaponChoice != "bow" and weaponChoice != "axe" and weaponChoice != "sword" and weaponChoice != "ogre in a bottle" :
                                    weaponChoice = input("That weapon isn't here! Do you take a 'bow', 'axe', or 'sword'?: ")

                                    if weaponChoice == "bow" :
                                        player.weapon = Weapon([arrowShoot, chokeHold], "Bow", 0.0)
                                        print("A standard bow.\n\
It's a slow weapon that stays inside of enemies and damages them over time.\n\
Good if you want to forget about some foes, but bad at finishing things within a timely manor.")


elif weaponChoice == "axe":
player.weapon = Weapon([deepCut, finisher], "Axe", 0.0)
print("A pair of small battle axes.\n\
They're quick weapons that do damage over time, and accumulate their damage instead of giving it to you upfront\n\
good at single target and very small targets, but bad against leech and quick fights.")
elif weaponChoice == "sword" :
    player.weapon = Weapon([heavyBlow, quickAttack], "Sword", 0.0)
    print("A steel longsword.\n\
It does high damage, but does all of it's damage upfront but does enough damage to one shot most foes, good at big foes, but bad at small ones and long fights.")
elif weaponChoice == "ogre in a bottle" :
    player.weapon = Weapon([clubBash, punch], "Ogre in a Bottle", 0.5)
    print("BONK")




    def end() :
    global restart
    restart = True
    input("You have been slain.\n" + player.currentDeathMessage + "\nPress 'enter' on your keyboard to start a new game. :)")




    def codeFind() :
    print("After your fight with the mutant, you realize how dire the situation is, and sit on a bench to think about your next move \n\
very carefully. All this worry has got you very stressed out, so to alleviate that, you make a stop by the local library to pick up a good read. \n\
Inside the library, you find that only three books are available to be checked out. While grabbing the books, a letter falls out, which reads as follows: \n\
'In order to infiltrate Joshro's fortress, you must find the secret code inscribed within the following texts. Good luck, brave traveller!' \n\
Revived by this sudden revelation that could help you save Misty, you get comfortable and start sifting through the books to find what you need.")
print(" ")
library = []
library.append(["'WEB OF CHARLOTTE'S'", "INsIDe THE wATer, CHARLOTTE SEARCHED FOR WILBUR."])
library.append(["'THE GODDESS OF THE NECKLACES'", "FRODO lookED At The EYE OF SAUrON INSANeLY."])
library.append(["'FURRY PLANTER'", "hARRY urGENTLY NEEDED TO GET TO THE BrEWERy BEFORE LORD VOLDEMORT."])
for page in library :
finalCode = []
nextPage = False
for verse in page :
for letter in verse :
if letter.islower() :
    finalCode.append(letter)
    while not nextPage :
        enteredCode = True
        userEnterCode = False
        for verse in page :
nextVerse = False
print(verse)
while not nextVerse :
    next = input("Do you want to go to the 'next' verse or 'enter' the code or go 'back' to the original page: ")
    while next != "next" and next != "enter" and next != "back" :
        next = input("That won't work this time! Do you want to go to the 'next' verse or 'enter' the code or go 'back' to the original page: ")
        if next == "next" :
            nextVerse = True
            elif next == "enter" :
            userEnterCode = True
            enteredCode = False
            break
            elif next == "back" :
            userEnterCode = True
            enteredCode = True
            break
            if userEnterCode == True :
                break
                while enteredCode == False :
                    code = input("Enter (or type 'go back to page' if you need to check again): ")
                    if vector(code) == finalCode :
                        enteredCode = True
                        print("You got it!")
                        nextPage = True
                        continue
                        elif code == "go back to page" :
                        enteredCode = True
                        continue
                        print("You piece the code together, and find that the completed code is 'sewer look there hurry'. You exit the library, and hurry off \n\
to the local sewer entrance to search for the secret entrance to the fortress where Misty is held.")




def randomNumtosolve(number) :
    playerNum = 0
                        while playerNum < number :
                            print(number)
                            print("^ NUMBER TO REACH ^")
                            print(playerNum)
                            print("^ YOUR CURRENT TOTAL ^")
                            inputString = input("Choose a number that is less than 1/4 \n\
of what the number is, and try to add up to the sum of it (keep in mind you'll have to add more numbers to this one): ")
if inputString.isnumeric() :
    numberGiven = int(inputString)
else:
continue
while numberGiven >= number / 4 :
    numberGiven = int(input("That won't work this time! Choose a number that is less than 1/4 \n\
of what the number is, and try to add up to the sum of it (keep in mind you'll have to add more numbers to this one): "))
print(number)
print("^ NUMBER TO REACH ^")
print(playerNum)
print("^ YOUR CURRENT TOTAL ^")
playerNum += numberGiven
if playerNum > number:
print("You went over the number, and have to start over!")
number = random.choice(divByfour)
print(number)
playerNum = 0
if playerNum == number :
    print("You did it! Good job!")




    def fightSequence(enemies : Enemy, spareable : bool, specialEnding : str) :
    global player, specialFightEnding, specialFightEndingMonsters

    specialFightEnding = False
    enemiesC = [] # The C in enemiesC stands for copy.
    for enemy in enemies :
enemiesC.append(deepcopy(enemy))
fightOn = True
fightFrameOne = True
target = 0

while fightOn:

print("")
for enemy in enemiesC :
print(enemy.name + "'s health: " + str(enemy.health))
print("Max's health: ", player.currentHealth)
print("")

if fightFrameOne :
    fightFrameOne = False
    for enemy in enemiesC :
enemy.FindNewAttack()
player.weapon.SwitchAttacks("Do you want to use ", False)


dialogue = "'dodge' or 'attack' to continue your current attack or 'switch' your attack"
if len(enemiesC) > 1:
dialogue += " or 'change' targets from " + enemiesC[target].name
dialogue += "? "
prompt = input(dialogue)

while prompt != "dodge" and prompt != "attack" and prompt != "switch" and (not (len(enemiesC) > 1 and prompt == "change")) :
    prompt = input("That won't work this time! " + dialogue)


    if prompt == "dodge" :
        print("")
        stunned = player.IsStunned()
        for i in range(len(enemiesC)) :
            if not stunned or not player.IsStunned() :
                unblockedDamage = 0
                blockedDamage = 0
                if not enemiesC[i].IsStunned() :
                    enemyHit, enemiesBorn = enemiesC[i].TakeTurn(i)
                    enemiesBornC = deepcopy(enemiesBorn)
                    for i in range(len(enemiesBornC)) :
                        enemiesBornC[i].summoned = True
                        print(enemiesC[i].name + " has birthed a new " + enemiesBornC[i].name + "!")
                        enemiesC.extend(enemiesBornC)
                        unblockedDamage += enemyHit.damage
                        damageDelt = floor(enemyHit.damage / 2)
                        blockedDamage += damageDelt
                        heal = int(floor(float(damageDelt) * enemiesC[i].leech))
                        if heal != 0:
print(enemiesC[i].name + " heal's off of you for " + str(heal) + ".")
enemiesC[i].health = min(enemiesC[i].maxHealth * 2, enemiesC[i].health + heal)
player.ApplyHit(Hit(damageDelt, enemyHit.inflictions, i), True)
print("You dodged the attack and took " + str(blockedDamage) + " damage instead of taking " + str(unblockedDamage) + " damage!")
                        else:
print(enemiesC[i].name + " did not attack this round as they were stunned.")
print("")

                else:
if not enemiesC[i].IsStunned() :
    enemyHit, enemiesBorn = enemiesC[i].TakeTurn(i)
    enemiesBornC = deepcopy(enemiesBorn)
    for i in range(len(enemiesBornC)) :
        enemiesBornC[i].summoned = True
        print(enemiesC[i].name + " has birthed a new " + enemiesBornC[i].name + "!")
        enemiesC.extend(enemiesBornC)
        player.ApplyHit(enemyHit, False)
        heal = int(floor(float(enemyHit.damage) * enemiesC[i].leech))
        if heal != 0:
print(enemiesC[i].name + " heal's off of you for " + str(heal) + ".")
enemiesC[damageDealer[i]].health = min(enemiesC[damageDealer[i]].maxHealth, enemiesC[damageDealer[i]].health + heal)
print("Becuase you were stunned you didn't block.\n")
        else:
print(enemiesC[i].name + " did not attack this round as they were stunned.")
print("")

for i in range(len(enemiesC)) :
    ignored, inflictionDamageDelt, respectiveNames = enemiesC[i].UpdateInflictions()
    for j in range(len(inflictionDamageDelt)) :
        heal = int(floor(float(inflictionDamageDelt[j]) * player.weapon.leech))
        if heal != 0 :
            print("You heal off of " + enemiesC[i].name + " for " + str(heal) + " because of " + respectiveNames[j] + ".")
            player.currentHealth = min(player.maxHealth, player.currentHealth + heal)
            print("")

            damageDealer, inflictionDamageDelt, respectiveNames = player.UpdateInflictions()
            for i in range(len(damageDealer)) :
                heal = int(floor(float(inflictionDamageDelt[i]) * enemiesC[damageDealer[i]].leech))
                if heal != 0 :
                    print(enemiesC[damageDealer[i]].name + " heal's off of you for " + str(heal) + " because of " + respectiveNames[i] + ".")
                    enemiesC[damageDealer[i]].health = min(enemiesC[damageDealer[i]].maxHealth, enemiesC[damageDealer[i]].health + heal)


                    elif prompt == "attack" :
                    print("")
                    for i in range(len(enemiesC)) :
                        if not enemiesC[i].IsStunned() :
                            enemyHit, enemiesBorn = enemiesC[i].TakeTurn(i)
                            enemiesBornC = deepcopy(enemiesBorn)
                            for i in range(len(enemiesBornC)) :
                                enemiesBornC[i].summoned = True
                                print(enemiesC[i].name + " has birthed a new " + enemiesBornC[i].name + "!")
                                enemiesC.extend(enemiesBornC)
                                player.ApplyHit(enemyHit, False)
                                heal = int(floor(float(enemyHit.damage) * enemiesC[i].leech))
                                if heal != 0:
print(enemiesC[i].name + " heal's off of you for " + str(heal) + ".")
enemiesC[i].health = min(enemiesC[i].maxHealth, enemiesC[i].health + heal)
                                else:
print(enemiesC[i].name + " did not attack this round as they were stunned.")
print("")

for i in range(len(enemiesC)) :
    ignored, inflictionDamageDelt, respectiveNames = enemiesC[i].UpdateInflictions()
    for j in range(len(inflictionDamageDelt)) :
        heal = int(floor(float(inflictionDamageDelt[j]) * player.weapon.leech))
        if heal != 0 :
            print("You heal off of " + enemiesC[i].name + " for " + str(heal) + " because of " + respectiveNames[j] + ".")
            player.currentHealth = min(player.maxHealth, player.currentHealth + heal)
            print("")

            if not player.IsStunned() :
                if spareableand player.weapon.CurrentAttack().name == "spare" :
                    spareSucceeds = random.randint(1, 3) == 3
                    if spareSucceeds :
                        print("You attempt to spare and are successful!")
                        specialFightEnding = True
                        specialFightEndingMonsters = enemiesC
                        fightOn = False
                        break
                        print("You attempt to spare and are unsuccessful.")
                        playerHit = player.TakeTurn()
                        enemiesC[target].ApplyHit(playerHit)
                        player.currentHealth = min(player.maxHealth, player.currentHealth + int(floor(float(playerHit.damage) * player.weapon.leech)))
                        print("")
                    else:
print("Becuase you were stunned you didn't attack.\n")

damageDealer, inflictionDamageDelt, respectiveNames = player.UpdateInflictions()
for i in range(len(damageDealer)) :
    heal = int(floor(float(inflictionDamageDelt[i]) * enemiesC[damageDealer[i]].leech))
    if heal != 0 :
        print(enemiesC[damageDealer[i]].name + " heal's off of you for " + str(heal) + " because of " + respectiveNames[i] + ".")
        enemiesC[damageDealer[i]].health = min(enemiesC[damageDealer[i]].maxHealth, enemiesC[damageDealer[i]].health + heal)


        elif prompt == "switch" :
        player.weapon.SwitchAttacks("", True)


    else:
answer = 0
while answer < 1 or answer > len(enemiesC) :
    for i in range(len(enemiesC)) :
        print(enemiesC[i].name + " with " + str(enemiesC[i].health) + " health: '" + str(i + 1) + "', or")
        tempAnswer = input("'nevermind'. ")
        if tempAnswer.isnumeric() :
            answer = int(tempAnswer)
            elif tempAnswer == "nevermind" :
            answer = target + 1
            target = answer - 1



            fightOn = player.currentHealth > 0 and len(enemiesC) > 0
            if not fightOn:
break

enemiesRemovedThisFrame = 0
for i in range(len(enemiesC)) :
    if enemiesC[i - enemiesRemovedThisFrame].health <= 0 :
        print(enemiesC[i - enemiesRemovedThisFrame].name + " has been defeated.")

        if len(enemiesC) <= 1 :
            print("You won!")
            fightOn = False
            break

            inflictionsRemovedThisFrame = 0
            for j in range(len(player.inflictionAttackers)) :
                if player.inflictionAttackers[j - inflictionsRemovedThisFrame] > i - enemiesRemovedThisFrame:
player.inflictionAttackers[j - inflictionsRemovedThisFrame] -= 1
elif player.inflictionAttackers[j - inflictionsRemovedThisFrame] == i - enemiesRemovedThisFrame :
    print(player.inflictions[j - inflictionsRemovedThisFrame].Name() + " has worn off.")
    del(player.inflictionAttackers[j - inflictionsRemovedThisFrame])
    del(player.inflictions[j - inflictionsRemovedThisFrame])
    inflictionsRemovedThisFrame += 1
    del(enemiesC[i - enemiesRemovedThisFrame])
    enemiesRemovedThisFrame += 1
    target = 0

    enemyNames = [enemiesC[0].name]
    combinedEnemyNames = enemiesC[0].name
    for i in range(len(enemiesC) - 1) :
        enemyNames.append(enemiesC[i + 1].name)
        combinedEnemyNames += ", " + enemiesC[i + 1].name
        for option in specialEnding :
if option == enemyNames :
    printableCombinedEnemyNames = enemiesC[0].name
    if len(enemiesC) == 2 :
        if enemiesC[0].name == enemiesC[1].name :
            printableCombinedEnemyNames += "s"
        else :
            printableCombinedEnemyNames += " and the " + enemiesC[1].name
    else:
for i in range(len(enemiesC) - 2) :
    printableCombinedEnemyNames += ", " + enemiesC[i + 1].name
    printableCombinedEnemyNames += ", and the" + enemiesC[len(enemiesC) - 1].name

    hasHave = " has"
    if len(enemiesC) > 1:
hasHave = " have"
print("The " + printableCombinedEnemyNames + hasHave + " chosen to stop fighting.")

fightOn = False
specialFightEnding = True
specialFightEndingMonsters = enemiesC
break

allSummoned = True
for enemy in enemiesC :
allSummoned = allSummoned and enemy.summoned

if allSummoned :
    printableCombinedEnemyNames = enemiesC[0].name
    if len(enemiesC) == 2 :
        if enemiesC[0].name == enemiesC[1].name :
            printableCombinedEnemyNames += "s"
        else :
            printableCombinedEnemyNames += " and the " + enemiesC[1].name
            elif len(enemiesC) > 2:
for i in range(len(enemiesC) - 2) :
    printableCombinedEnemyNames += ", " + enemiesC[i + 1].name
    printableCombinedEnemyNames += ", and the" + enemiesC[len(enemiesC) - 1].name

    hasHave = " has"
    if len(enemiesC) > 1:
hasHave = " have"
print("The " + printableCombinedEnemyNames + hasHave + " chosen to stop fighting.")
fightOn = False
specialFightEnding = True
specialFightEndingMonsters = enemiesC
break




if player.currentHealth <= 0:
end()
return

player.inflictions.clear()
player.inflictionAttackers.clear()
player.currentHealth = min(player.maxHealth, player.currentHealth + 50)
print("You end the fight with " + str(player.currentHealth) + " health.")




def deadlyTreasure() :
    global player, player
    print("While walking through the forest, you come across a golden box \n\
that shimmers in the sunlight. A key lays inside the tree next to the box, \n\
but the tree hole is too dark for you to see what else is inside the tree.")
key = input("Do you 'take the key' or 'let it be'? ")
while key != "take the key" and key != "let it be":
key = input("That won't work this time! Do you 'take the key' or 'let it be'? ")
if key == "take the key" :
    print("You grab the key quickly, thinking that history favors the bold, and open the box to find a potion \n\
that increases your health by 50. You drink it, and continue along the path.")
player.maxHealth += 50
player.currentHealth = min(player.maxHealth, player.currentHealth + 50)
elif key == "let it be" :
    print("You decide it's not worth the risk, and continue walking on the \n\
road.")
return




def pigLanguage() :
    print(" ")
    print("The pig says, 'Atwhay isyay ouryay amenay?' ")
    pigName = input("What did the pig say in English? ")
    while pigName != "What is your name?" :
        pigName = input("That won't work this time! What did the pig say in English? ")
        pigNameask = input("What is your character's name? ")
        while pigNameask != "Axmay" :
            pigNameask = input("That won't work this time! What is your character's name? ")
            print(" ")
            print("The pig then says, 'Areyay ouyay ethay ersonpay at'sthay upposedsay otay ebay ivinggay Oshrojay ishay ailyday assagemay?' ")
            pigLie = input("What did the pig say in English? ")
            while pigLie != "Are you the person that's supposed to be giving Joshro his daily massage?" :
                pigLie = input("That won't work this time! What did the pig say in English? ")
                pigLieask = input("*Are* you the person that's supposed to be giving Joshro his daily massage(hint: you should *probably* say yes)? ")
                while pigLieask != "Esyay" :
                    pigLieask = input("That won't work this time! \n\
*Are* you the person that's supposed to be giving Joshro his daily massage(hint: you should *probably* say yes)? ")
print(" ")
print("You finish checking in with the pig, and enter the next chamber, eager to advance to the second to last stage of your journey.")




def riverEscape() :
    stepsYes = random.randint(10, 15)
                    print("You reach a river that could separate you from the ferocious carnivores, if you can reach the other side \n\
of course. Fortunately, an old fisherman is seen walking towards his flimsy boat, and you take this opportunity to ask the man \n\
for safe passage across the swift rapids. The only issue is, the man is not picking up the social cues that indicate your panic, so \n\
you must repeatedly prod him into picking up the pace to save both of your souls!")
print("You need to continously say yes to the man " + str(stepsYes) + " times before the wolves advance the " + str(stepsYes) + " steps needed to close the distance \
and devour you.")
prod = 0
wolfRun = 0
while prod != stepsYes and wolfRun != stepsYes:
prodInput = input("Say 'yes': ")
if prodInput == "yes" :
    prod = prod + 1
    print("You've prodded the fisherman " + str(prod) + " total time(s)")
    wolfAdvance = random.randint(1, 4)
    if wolfAdvance == 1 or wolfAdvance == 2 or wolfAdvance == 3 :
        wolfRun = wolfRun + 1
        print("The werewolves advanced " + str(wolfRun) + " total step(s)")
        elif wolfAdvance == 4 :
        print("The werewolves didn't advance at all and are still at " + str(wolfRun) + " total step(s)")
        if prod == stepsYes :
            print("The fisherman FINALLY finishes talking and gets the boat going, and you breathe a much-deserved \
sigh of relief as you sit back and watch the werewolves cower in fear from the rushing water.")
print("Once you get across the river, the fisherman bids you farewell, and you venture on towards the castle.")
elif wolfRun == stepsYes :
    print("The fisherman doesn't seem to get the hint, even after the " + str(prod) + " times you told him to hurry up. \
The werewolves close in swiftly, and you and the fisherman become a hungry family's next meal.")
player.currentDeathMessage = "It looks like you may need to work on your talking skills."
end()
return




def castleEntrance() :
    stick = False
            placeholder = False
            stable = True
            tree = True
            shed = True
            while stick == False or placeholder == False:
whereTolook = input("There are several areas of interest nearby that may contain the items you need, which include: a 'shed', \n\
an old 'stable', or a strangely shaped 'tree'. Where do you look? ")
if whereTolook == "stable" :
    print("You enter the dusty and decrepit stable, where you make a mental note of how rotten the wooden interior looks from the several \n\
years of neglect. Fortunately, a piece of wood that isn't entirely ruined juts out of the wall, and you decide that this will suffice as \n\
a lever. You grab the STICK, and exit the stable.")
time.sleep(currentSettings.sleepTime)
stick = True
stable = True
elif whereTolook == "shed":
print("You enter the small shed, and laugh to yourself as you observe just how little gardening tools and other trinkets typically \n\
found in a shed are actually present. One thing that isn't present, however, is the placeholder, or even something that could pass for one. \n\
Saddened by this, you leave the shed.")
time.sleep(currentSettings.sleepTime)
shed = True
elif whereTolook == "tree":
print("After having a previous encounter when it comes to sticking your hand inside trees, you hesitate before blindly \n\
shoving your hand inside the dark crevice, and your hand connects with something cold- the PLACEHOLDER! You don't test your luck any further \n\
and exit the hole, PLACEHOLDER in hand.")
time.sleep(currentSettings.sleepTime)
placeholder = True
tree = True
print("Using your knowledge as the village carpenter, you combine these two objects into a functional lever, and place it in the appropriate location \n\
next to the gate.")
print("Hearing the gears churn inside the gate's various mechanisms, you take this as a good sign as you see the gate slowly open. \n\
Huzzah! The gate has opened and you finally come to the last part of your journey... or so you think;)")
return placeholder, stick, tree, shed, stable




def stringWord(emptyStr) :
    letter = random.choice(strings)
    strings.remove(letter)
    emptyStr += letter
    print("******")
    print(emptyStr)
    print("******")
    return emptyStr, strings




    def cave() :
    global player
    print("You hesitantly enter the cave, anxious about who or what might await you inside the dark depths. Fortunately, luck \n\
seems to favor you in this instance, and no trace of another inhabitant is visible inside the damp enclosure. In fact, you find \n\
a perfectly preserved elixir that has the label 'Miriam's Medical Miracle' stamped in fine lettering across the brownish glass. \n\
Before you take a swig, you doubt how safe ingesting the bottle's contents will be, and pause.")
drinkDroptake = input("Do you 'drink' or 'drop' or 'take' the bottle? ")
while drinkDroptake != "drink" and drinkDroptake != "drop" and drinkDroptake != "take":
drinkDroptake = input("That won't work this time! Do you 'drink' or 'drop' or 'take' the bottle? ")
if player.weapon.name == "Pet Slime" :
    print("But before being able to do anything,\n\
your slime pet jumps from your bag and consumes the entire bottle, glass included. Slime Pet has learned 'slime spike'")
player.weapon.LearnAttack(slimeSpike)
return False
elif drinkDroptake == "drink" :
    print("You drink the potion, but after sitting down, you instantly pass out. You wake up in the morning feeling strangely \n\
healthier, and relish in the fact that you now have 50 more health!")
player.maxHealth += 50
player.currentHealth = min(player.maxHealth, player.currentHealth + 50)
return False
elif drinkDroptake == "drop":
print("You drop the potion carelessly on the ground, and am unsurprised as you witness the potion's contents dissolve the hard rock \n\
below. You go to sleep, and wake up, happy that you avoided a possibly deadly drink.")
return False
elif drinkDroptake == "take" :
    print("For some almost supernatural reason, you feel as though you can use the potion for something greater, and decide \n\
to store it in your pack for later use. You have a dream that a large green creature asks you for the potion in exchange for \n\
safe passage across a path, but brush it off as your mind playing tricks on you and have a pretty uneventful rest of the night.")
return True




def swordPull() :
    global player
    grunt = ["gurr", "rawr", "guh", "rawr", "gurr"]
    exercise = True
    while exercise :
        userGrunt = []
        for i in range(5) :
            muscle = input("Exert your strength: ")
            userGrunt.append(muscle)
            print(*userGrunt, sep = ", ")
            if userGrunt[i] != grunt[i] :
                userGrunt = []
                print("You strain your back and have to start over!")
                player.currentHealth = max(0, player.currentHealth - 25)
                break
                elif i == 4 :
                exercise = False
                print("Huzzah! The sword finally nudges out of its nested position, and you bask in its strangely magical glory. 'The Python' is \n\
engraved on the side of the blade, and you deduce that this is the name of the sword.")
print("Attached to the handle of the blade is a leaflet that reveals why 'The Python' feels so powerful: 'The blade decides \n\
the enemy's fate, not you'. You take this as a good sign, and place the sword at your side as you get ready for bed.")
if player.weapon.name == "Pet Slime":
print("Your Pet Slime then jumps out and eats the sword! Your Pet Slime has learned heaviest blow, it also would like to know be known as Pet Slime, Decider of Fates.")
player.weapon.LearnAttack(heaviestBlow)
player.weapon.name = "Pet Slime Decider of Fates"
else:
player.weapon = Weapon([heaviestBlow, quickAttack], "Python", 0.5)
print("The night is otherwise \n\
uneventful, and you wake up feeling strangely refreshed after having that small victory the previous night!")




def watchTower() :
    print("You enter the grey space, and find the area surprisingly empty, save for a single longsword of strikingly amazing quality. \n\
The only issue is the fact that the sword is coincidentally stuck in a slab of stone. You've seen this somewhere before, and know that \n\
swords stuck in stone are probably very good, but it's getting late and all that walking has you in an increasingly apparent state of exhaustion.")
tryAvoid = input("Do you want to 'try' to take the sword out of the stone or 'avoid' the strenuous act altogether? ")
while tryAvoid != "try" and tryAvoid != "avoid" :
    tryAvoid = input("That won't work this time! \n\
Do you want to 'try' to take the sword out of the stone or 'avoid' the strenuous act altogether? ")
if tryAvoid == "try" :
    print("As everyone knows, making grunting sounds while exerting energy always helps you accomplish the desired task. \n\
This goes the same when trying to pull metal out of stone, and in order to successfully dislodge the blade from its spot, \n\
you must alternate between 'guh', 'gurr', and 'rawr'.")
swordPull()
if tryAvoid == "avoid" :
    print("You decide that your rest is more important than something that almost certainly seems as if it could be useful to you, \n\
and go to sleep restlessly on the cold hard floor.")




def home() :
    tavernInncottage = input("Do you make the 'tavern', 'cottage', or 'inn' your new home? ")
    while tavernInncottage != "tavern" and tavernInncottage != "cottage" and tavernInncottage != "inn" :
        tavernInncottage = input("That won't work this time! Do you make the 'tavern', 'cottage', or 'inn' your new home? ")
        return tavernInncottage




        def printOutro() :
        global location
        if location == "village" :
            print(outroMessages[0])
            elif location == "forest" :
            print(outroMessages[1])
            elif location == "old bridge" :
            print(outroMessages[2])
            elif location == "sewers" :
            print(outroMessages[3])
            elif location == "forest2" :
            print(outroMessages[4])




            introMessages = ["As you walk past the village tavern, a drunken ogre unfortunately \
mistakes you for the same villager that stole his favorite gold coin, \
and pulls out his rusty dagger to take it back.", "After walking through the forest some more, you come across a \
clearing that gives you a clear view of the castle where Misty is being held. \
As you stop to drink from your canteen, a goblin jumps out of the bushes and \
steals your pack! Goblins were hit especially hard by the last war, but you \
also know that that doesn't excuse it from stealing your stuff.", "The castle doesn't seem so far now, which gives you a glimpse of hope \
that Misty can be saved. That hope quickly comes crashing down when you see a horde of werewolves descend from the dark depths of the \
the tree line, and you sprint towards the castle in hopes of finding safety from the hungry monsters.", "The sun starts to set, and you decide that you must find a nice place to hit the hay for the night. There are only two places available: \
a small and unassuming cave that offers shelter at the expense of not knowing whether or not an animal ALSO considers the cave its home, \
and an abandoned watchtower that accentuates the damaging effects of the past war.", "Being alive for this long is very commendable, \
so you pat yourself on the back as you cross a boring old bridge. You actually notice how distinctly boring this bridge is, \
and voice your distaste of the structure. As you do this, a rumbling can be heard from under you, and as you recover from the friction, \
a gross troll appears in front of you, and you're able to notice that she did NOT approve of your comment, and lets you know just as much.", \
"After going through a whopping SEVEN possibly deadly levels, you've finally reached the castle, well, correction- castle entrance. The gate is \
still intact, although the lever used to open the hefty doors is of course missing, so you begin to look around for a solution to this most \
coincidental of situations.", "Finding the sewers was easy, but actually navigating through the dense masses of questionable matter proves difficult. As you see a shimmer of light at the end of the tunnel, something large and hairy \n\
brushes past your leg, and as you question what exactly that could've been, a freakishly large mutated rat jumps out of the water and tries to lunge at you, \n\
but thankfully misses, and you retrieve your weapon from its holster and get ready to battle this rage-filled rodent."]
outroMessages = ["The ogre stumbles to the ground, but before you can search him \
for anything valuable, a guard approaches from the distance, and \
you pick up the pace to the village exit where you can continue your quest. ", "The goblin collapses, and you take pity on the creature \
as you collect your items and continue towards the castle.", "The troll becomes motionless, and after you poke it with your weapon to see if she's still alive, \
gravity finally takes effect, and the troll falls over the side of the bridge into the murky depths below, creating such a splash that the water soaks your hair. \
You regain your composure, and proceed to start walking along the path again.", "You slay the horrid creature, and continue walking towards the light.", \
"Both goblins collapse, and after questioning your own ethics you continue on your journey; new pet in hand."]





endingOneHappens = "After slaying the dragon, you return Misty to her respective kingdom, and she thanks you profusely. Before you fully become content with your life, you remember \n\
the promise you made to the goblin that you would come work for him to repay your debt (although you still scratch your head as to what exactly the *debt* is). Nevertheless, you are a man of your word, \n\
and make the long trek to the goblin's farm, and find that the farm life isn't so bad when the person you're doing the farming for is so small in stature that a farm to him is like a small garden to you. \n\
In addition to this, you actually find that the goblin, whose name is soon discovered to be Bilbo, is surprisingly very funny and witty, and you two get along splendidly. The farm life treats you very well, \n\
and you decide to take up farming even after your servitude to Bilbo was fulfilled, selling various vegetables to travellers going on journeys of their own... \n\
YOU GOT THE 'Man of your word' ENDING! (1 out of 4)"
endingTwoHappens = "After defeating Joshro, you bring Misty back to her palace, where a celebration is held in your name for bringing back the kingdom's beloved princess. After recuperating yourself from the \n\
many nights of royal partying, you leave the kingdom in a great mood. While walking across an outrageously decrepit bridge, you remember Samantha and the offer she made to you. Seeing how disgusting some bridges can be \n\
without proper maintenance, you decide that bridge cleaning is the next endeavor you wish to undertake. Returning to Samantha's bridge, you see that she's already made several noticeable steps to \n\
make her bridge look more approachable, which gives you the final push you need to really take this bridge thing seriously. For several years, you and Samantha work tirelessly to keep the bridge spotless, \n\
and become great friends in the process. After Samantha leaves the bridge business to pursue a career in dramatic acting, you transform the bridge into a resting place for weary travellers to \n\
to safely take shelter at while they go fight dragons and save princesses of their own... \n\
YOU GOT THE 'Human troll' ENDING! (2 out of 4)"
endingThreeHappens = "After putting an end to Joshro's reign of terror, you and Misty travel back to her kingdom, and you get there just in time to see her get married, and start to feel a little bit empty. \n\
This emptiness consumes you for the next couple of days while you head back to the cottage, and you struggle to put your finger on what exactly is bothering you so much. That is, until you and Olivia meet again. \n\
The reunion fills you with an overwhelming sense of bliss, and you feel that emptiness become replaced with a new type of emotion- love. In the space of 8 years, you and Olivia open up a tavern, and your relationship with her \n\
eventually shifts from a platonic one to one filled with golden memories and endless laughter, a sign of the increasingly apparent chemistry between you two. Then, on the tenth anniversary since you two met during your quest to \n\
defeat Joshro, you propose to Olivia, and she says yes with tears in her eyes as you both hug. Many years go by, and a family of three is formed, with the new addition to it being your son Matthew. \n\
One day, Matthew tells you privately that he wishes to go out and stop the tyrant serpent Tylo from taking over a kingdom several oceans away, but informs you that Olivia is aggressively against this. \n\
Remembering how you met her in the first place, you smile and tell him to leave in the middle of the night and 'follow your dreams', just as you did so long ago... \n\
YOU GOT THE 'Unwidowing the widow' ENDING! (3 out of 4)"
endingFourHappens = "After stopping Joshro dead in his tracks (hah, get it?), you and Misty begin the long trek back to her kingdom. While on the trip, you discover that there's more to Misty than her intense beauty, and \n\
find that she's actually a really smart girl that aims to study international affairs at the college level. Her studious nature, among other cherished traits, makes you start to fall for her. \n\
The night before you're expected to reach her kingdom, you profess your love to her, and she confesses her love for you as well. The arranged marriage she was supposed to end the next day was changed to one that she had \n\
complete control over, and, as expected, she chooses to marry you. The wedding is magical for both of you, and, because of how marriage directly influences who becomes king and queen, you become the next king of her kingdom! \n\
The next decades become one of the most prosperous times in the kingdom, and the public regard you as one of the best kings they've ever had. \n\
Sitting one day in the royal court, a young but hardy man asks you for a small amount of money, and states that his intentions with the money will be to discover new lands and put an end to injustice all over the world. \n\
Your advisors laugh him off, but you retain your steady gaze and approve the man's request, basking in the nostalgia of a time when you knew a man that also wanted to do good... \n\
YOU GOT THE 'From rags to royalty' ENDING (4 out of 4)"


























#Variables and game:

#Globalizing variables

restart = True
specialFightEnding = False
specialFightEndingMonsters = []
global location, player, \
potionTroll, divByfour, morality, trackEndings, strings, emptyStr, currentSettings
currentSettings : Settings

#Constant variables :
#Attacks
#The syntax for a status effect is :
#StatusEffect(InflictionType.YOURINFLICTION, how long you want it to last)
#The syntax for an attacks is :
#Attack([Status effects], [chance of each status effect happening], damage, damage randomness(how far from the original value the actual value can be), [self inflictions], [self infliction procs], self damage, self damage randomness, [summons], turns to do, name)
#First up is the attacks that are required to be early.
fireBreath = Attack([StatusEffect(InflictionType.BURNING, 2)], [100], 0, 0, [], [], 0, 0, [], 3, "fire breath")
heavyBite = Attack([], [], 50, 0, [], [], 0, 0, [], 4, "heavy bite")
#Enemies that must be declared early.
joshroHead = Enemy(25, 50, [fireBreath, heavyBite], "Joshro Head", 0.5)
#Normal attacks.
clubBash = Attack([StatusEffect(InflictionType.STUN, 2)], [100], 25, 10, [], [], 0, 0, [], 3, "club bash")
punch = Attack([], [], 15, 15, [], [], 0, 0, [], 1, "punch")
heavyPunch = Attack([StatusEffect(InflictionType.STUN, 2)], [75], 25, 25, [], [], 0, 0, [], 2, "heavy punch")
quickStab = Attack([StatusEffect(InflictionType.POISON, 3)], [50], 5, 5, [], [], 0, 0, [], 1, "quick stab")
rockThrow = Attack([StatusEffect(InflictionType.STUN, 1)], [25], 5, 5, [], [], 0, 0, [], 1, "rock throw")
slimeHug = Attack([StatusEffect(InflictionType.DEADLY_HUG, 3)], [100], 0, 0, [], [], 0, 0, [], 1, "slime hug")
slimeSpike = Attack([StatusEffect(InflictionType.BLEED, 3)], [100], 5, 0, [], [], 0, 0, [], 1, "slime spike")
arrowShoot = Attack([StatusEffect(InflictionType.BURNING, 3), StatusEffect(InflictionType.POISON, 8)], [100, 100], 35, 10, [], [], 0, 0, [], 3, "shoot arrow")
chokeHold = Attack([StatusEffect(InflictionType.STUN, 1)], [75], 5, 5, [], [], 0, 0, [], 1, "choke hold")
deepCut = Attack([StatusEffect(InflictionType.BLEED, 15), StatusEffect(InflictionType.BLEED, 15), StatusEffect(InflictionType.BLEED, 15)], [100, 50, 25], 0, 0, [], [], 0, 0, [], 1, "deep cut")
finisher = Attack([], [], 20, 0, [], [], 0, 0, [], 1, "finisher")
heavyBlow = Attack([], [], 100, 0, [], [], 0, 0, [], 5, "heavy blow")
quickAttack = Attack([], [], 35, 0, [], [], 0, 0, [], 2, "quick attack")
heaviestBlow = Attack([], [], 125, 0, [], [], 0, 0, [], 6, "heaviest blow")
splash = Attack([StatusEffect(InflictionType.WET, 5)], [100], 3, 3, [], [], 0, 0, [], 1, "splash")
quickClubBash = Attack([StatusEffect(InflictionType.STUN, 2)], [75], 10, 10, [], [], 0, 0, [], 2, "quick club bash")
bite = Attack([StatusEffect(InflictionType.POISON, 4), StatusEffect(InflictionType.BLEED, 4)], [5, 5], 5, 5, [], [], 0, 0, [], 2, "bite")
scratch = Attack([StatusEffect(InflictionType.BLEED, 4)], [25], 15, 5, [], [], 0, 0, [], 1, "scratch")
spare = Attack([], [], 0, 0, [], [], 0, 0, [], 1, "spare")
growHead = Attack([], [], 0, 0, [], [], 0, 0, [joshroHead], 2, "grow head")
ultraFireBreath = Attack([StatusEffect(InflictionType.BURNING, 3)], [100], 0, 0, [], [], 0, 0, [], 1, "ultra fire breath")

# The syntax for enemies is :
# Enemy(start health, max health, [attack1, attack2, ...], "name", leech amount 0 to 1 work best
    joshrosBody = Enemy(300, 300, [growHead], "Joshro's Body", 0.0)
    ogre = Enemy(100, 100, [clubBash, punch], "Ogre", 0.0)
    goblin = Enemy(100, 100, [quickStab, rockThrow], "Goblin", 0.0)
    slime = Enemy(25, 50, [slimeHug], "Pet Slime", 1.0)
    troll = Enemy(125, 125, [quickClubBash, splash], "troll", 0.0)
    mutant = Enemy(200, 200, [punch, heavyPunch], "mutant", 0.0)
    rat = Enemy(100, 200, [bite, scratch], "Rat", 0.25)
    babyRat = Enemy(25, 50, [bite, scratch, splash], "Baby Rat", 0.5)
    guard = Enemy(200, 200, [heavyBlow, quickAttack], "Unloyal Guard", 0.0)

def main():
    /* initialize random seed: */
    srand(time(NULL));
    global location, player, joshroHead, specialFightEnding, restart, \
    potionTroll, divByfour, morality, trackEndings, strings, emptyStr, currentSettings
    restart = False
    player = Player(150)
    potionTroll = False
    divByfour = [12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64]
    morality = 0
    trackEndings = []
    strings = ["s", "t", "r", "i", "n", "g"]
    emptyStr = ""
    endingOne = False
    endingTwo = False
    endingThree = False
    endingFour = False
    brutalEnding = True
    endingChosen = False
    print("---------------------------------------------------------------------------------------------------------------------------")
    print("You are Max, a young man who takes on the brave quest of saving the \n\
beautiful princess Misty from the evil dragon Joshro. Your story begins at the village \n\
blacksmith, where you must decide what kind of weapon you will bring with you \n\
on your journey.")
time.sleep(currentSettings.sleepTime)
weaponSelect()
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
location = "village"
player.currentDeathMessage = "Don't forget that if you dodge an attack that would stun you, you won't get stunned."
print(introMessages[0])
fightRun = input("Do you want to 'fight' or 'run' away from the ogre? ")
while fightRun != "fight" and fightRun != "run":
fightRun = input("That won't work this time! Do you want to 'fight' or 'run' away from the ogre? ")
if fightRun == "fight" :
    fightSequence([ogre], False, [[]] )
    if restart :
        return
        elif fightRun == "run" :
        print("Before you can run, the ogre grabs your shirt and pulls you back, \
and asks in a booming but obviously slurred voice:'Bretton, do you have ma \
gold coin yet or not?'")
fightAvoid = input("'1'. Lie and say its back at the inn and you'll get it as soon as he lets you go.\n\
'2'. Tell him to let you go or else he'll have to worry about more than just a stupid gold coin. \n\
(pick a number): ")
while fightAvoid != "1" and fightAvoid != "2":
fightAvoid = input("That won't work this time! PICK A NUMBER: ")
if player.weapon.name == "Ogre in a Bottle" :
    print("The ogre then ignores what you say and calls you back saying incoherently that you smell suspicious, and you're forced to fight them.")
    fightSequence([ogre], False, [[]] )
    elif fightAvoid == "1" :
    print("The ogre seems to like that option, and lets you go as he \n\
meanders back to the tavern. You regain your composure and continue walking \n\
to the village exit.")
emptyStr, strings = stringWord(emptyStr)
brutalEnding = False
elif fightAvoid == "2":
print("The ogre becomes enraged and slams you on the ground, howling \n\
like a dog as he searches himself for the dagger he carries. You get back \n\
on your feet and pull out your weapon.")
fightSequence([ogre], False, [[]] )
if restart :
    return
    printOutro()
    time.sleep(currentSettings.sleepTime)
    print(" ")
    print("++++++++++++++++")
    print(" ")
    deadlyTreasure()
    if restart:
return
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
location = "forest"
player.currentDeathMessage = "Don't forget that you can 'change' targets when there's more than 1 foe."
print(introMessages[1])
fightPersuade = input("Do you want to 'fight' or 'persuade' the goblin? ")
while fightPersuade != "fight" and fightPersuade != "persuade":
fightPersuade = input("That won't work this time! Do you want to 'fight' or 'persuade' the goblin? ")
if fightPersuade == "fight" :
    print("A Pet Slime also jumps out of the bushes to protect their owner!")
    fightSequence([goblin, slime], False, [[]] )
    if restart :
        return
        printOutro()
        elif fightPersuade == "persuade" :
        parkour = input("The goblin runs away, and you scramble after it. To catch up to the goblin, you can either 'slide' under a fallen log, or \n\
'vault' over a thorny bush. What do you do? ")
while parkour != "slide" and parkour != "vault" :
    parkour = input("That won't work this time! What do you do? 'slide' or 'vault'? ")
    if parkour == "slide" :
        persuade = input("You successfully slide under the log, and find the goblin scurried up on a tree branch, \n\
just out of reach. What do you say to the scared creature? \n\
Oh yeah, and there's some weird slime looking at him.\n\
'1'. \"I won't hurt you I promise, just please give me back my stuff and I promise I'll make it up to you.\" \n\
'2'. Pet that slime that you hope might be his and say \"You know, I like slimes too.\" \n\
(pick a number): ")
while persuade != "1" and persuade != "2":
persuade = input("That won't work this time! PICK A NUMBER: ")
if persuade == "1" :
    brutalEnding = False
    print("The goblin begrudgingly drops your items, but makes you promise that, under oath, you will come \n\
back after you finish your quest to come work for him to pay off your debt. You gather your pack and continue towards the castle.")
emptyStr, strings = stringWord(emptyStr)
trackEndings.append("Do you want to repay your debt to the 'goblin'?")
endingOne = True
#!!!ENDING ONE!!!
elif persuade == "2":
print("The goblin becomes furious after you pet HIS pet slime, and swiftly slashes you with his claws. \n\
You're able to get up, but because of the surprise attack, you've lost valuable health.")
player.maxHealth = int(player.maxHealth / 2)
player.currentHealth = int(player.currentHealth / 2 + 0.5)
fightSequence([goblin, slime], False, [["Pet Slime"]] )
if restart:
return
printOutro()
player.maxHealth = player.maxHealth * 2
player.currentHealth = player.currentHealth * 2
if (specialFightEnding) :
    specialFightEnding = False
    print("Oddly enough, you have now seemingly befriended the pet slime. The Pet Slime has replaced your weapon.")
    oldWeapon = player.weapon
    oldAttack = oldWeapon.attacks[0]
    player.weapon = Weapon([slimeHug, oldAttack], "Pet Slime", 1.0)
    print("After taking the place as your weapon, the Slime Pet decides that it should eat your old " + oldWeapon.name + ".\n\
After doing this, your Slime Pet learns a new skill, '" + oldAttack.name + "'.")
print("And after seeing this beautiful sight 2 disgusted goblins jump out of the trees to take you on.")
location = "forest2"
player.currentDeathMessage = "Your Pet Slime eats you soon after."
fightSequence([goblin, goblin], False, [[]] )
if restart:
return
printOutro()
else:
print("You try to vault over the bush, but because you skipped leg day \n\
at the medieval gym, you fail and fall face first into some *very* sharp thorns")
player.currentDeathMessage = "Maybe try making some different choices next time."
end()
return
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
location = "river"
print(introMessages[2])
riverEscape()
if restart:
return
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
location = "cave/watchtower"
print(introMessages[3])
caveWatchtower = input("Do you take shelter in the 'cave' or the 'watchtower'? ")
while caveWatchtower != "cave" and caveWatchtower != "watchtower":
caveWatchtower = input("That won't work this time! Do you take shelter in the 'cave' or the 'watchtower'? ")
if caveWatchtower == "cave" :
    potionTroll = cave()
    print("You leave the cave, and trek on towards the increasingly visible castle.")
    if caveWatchtower == "watchtower" :
        watchTower()
        print("You exit the watchtower, and trek on towards the increasingly visible castle.")
        time.sleep(currentSettings.sleepTime)
        print(" ")
        print("++++++++++++++++")
        print(" ")
        #Troll encounter / fight!!
        location = "old bridge"
        player.currentDeathMessage = "Don't forget that splash will weaken your attacks by a fixed amount, and it doesn't even effect status effects."
        print(introMessages[4])
        print("'I'm tired of you rude humans criticizing my bridge upkeep skills!:( Gah! All this yelling has got me thirsty, \n\
would you happen to have anything that could satiate my thirst?")
if potionTroll == True:
print("You suddenly remember that you have that potion from the cave, as well as the oddly specific dream that turned out \n\
to manifest itself into reality... Anyway, you think about if you should give the troll the potion or keep it for yourself.")
bargainFight = input("Do you 'give' the troll the potion or 'keep' it for yourself? ")
while bargainFight != "give" and bargainFight != "keep" :
    bargainFight = input("That won't work this time! Do you 'give' the troll the potion or 'keep' it for yourself? ")
    if bargainFight == "give" :
        brutalEnding = False
        print("You quickly give the troll the potion, and she monstrously consumes its contents before proceeding to burp in your \n\
face with a satisfied grin on her face. 'WOW! I don't know what kind of stuff was in that, but I think I'm gonna take a nap for a little bit. \n\
DON'T GO ANYWHERE HUMAN, OR ELS-', is all the troll can say before the potion's effects fully take place, and she falls onto the bridge and starts snoring loudly.")
emptyStr, strings = stringWord(emptyStr)
stayGo = input("Maybe it's because you felt bad for disrespecting the troll's bridge, or more likely because you're crazy, but you \n\
you hesitate before going and wonder whether or not you should 'stay' and see what the troll wants from you, or 'go' because the princess stuck in an \
evil dragon's castle is still there and, well, that was the whole reason you're out here in the first place... What do you do? ")
while stayGo != "stay" and stayGo != "go":
stayGo = input("That won't work this time! Do you 'stay' or 'go'? ")
if stayGo == "stay" :
    print("You wait several hours (trolls are notorious for sleeping for long periods of time), and just as you're about to forget it, \n\
the troll wakes up and looks considerably more happy. 'Heya pal! Look I'm sorry 'bout all that yelling I did earlier, I just can't stand inconsiderate individuals, \n\
human, werewolf, goblin, what have you. So, as a token of my apology, I'll offer you a job that'll consist of helping me keep this place spick-and-span... \n\
not at all because I get sorta lonely out here and could use another person to talk to... So whaddaya say?' Well, not one to turn down an offer as rare as this, you \n\
accept the troll's offer, but remind her that you still have a girl to save, and she nods her head understandably. \n\
You find out the troll's name is Samantha, and wish her well as you finally cross the bridge to continue your quest.")
trackEndings.append("Do you want to help keep the bridge clean with 'Samantha' the troll?")
endingTwo = True
#ENDING TWO
if stayGo == "go":
print("You decide that the troll was probably just saying all of that due to her inebriated state, and cross the bridge \n\
quietly to continue your journey.")
elif bargainFight == "keep" :
    print("You decide to be greedy and admittedly sort of dumb so as not to rid yourself of extra weight, and it comes back to bite you. 'WOW! So first \n\
you disrespect my bridge, and then you don't even give me something for my dehydration! This won't do! I'm going to have to teach you a lesson in manners!")
print("Quickly, you ask if there's a riddle you can try to solve in order to avoid a fight, but the troll's mind is already made up, and in fact this seems to \n\
make her even more angry, which doesn't help things in the slightest. You ready your weapon and prepare to fight the burly creature.")
fightSequence([troll], False, [[]] )
if restart:
return
printOutro()
print("P.S., while fighting the troll, the potion broke in your bag, so it's of no use to you now, and you think for a second how much \n\
better that situation could've turned out if you would've given the troll the potion in the first place, but ah whatever it's just a game, morals don't matter.")
elif potionTroll == False :
    print("Because you have nothing to give the troll, like a potion most likely found in a cave that could easily keep the upcoming situation peaceful, \n\
you blank out, and the troll notices this. 'WOW! So first you disrespect my bridge, and then you don't even give me something for my dehydration! \n\
This won't do! I'm going to have to teach you a lesson in manners!")
print("Quickly, you ask if there's a riddle you can try to solve in order to avoid a fight, but the troll's mind is already made up, and in fact this seems to \n\
make her even more angry, which doesn't help things in the slightest. You ready your weapon and prepare to fight the burly creature.")
fightSequence([troll], False, [[]] )
if restart:
return
printOutro()
player.weapon.LearnAttack(splash)
print("Before leaving, you let your " + player.weapon.name + " soak in the water, and it seems to absorb it.\n\
Your " + player.weapon.name + " has learned 'splash'")
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
location = "castle entrance"
print(introMessages[5])
castleEntrance()
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("Inside the castle walls, you become flabbergasted as you realize that the castle was not what you thought it was, and, \n\
instead of being one small keep, the castle actually houses a considerably sized village and population of people.")
print("Overwhelmed by this new revelation, you make your way through the solemn streets, and note how subdued the locals seem. \n\
Luckily, you find a map taped to a community board that gives the location of 3 places you deem could function as temporary homes: \n\
an inn, a two-person cottage in need of one more roommate, and a tavern with rooms to rent. Fortunately, your pickpocketing skills have empowered \n\
you with the ability to easily pay for all three.")
x = home()
print("You choose to make the", x, "your new home.")
if x == "cottage":
print("Knocking on the door of the cottage, you take a minute to observe that everyone has been avoiding you with intense dedication, \n\
which deeply concerns you. The door then opens, and for the first time since you step foot in this strange place, a friendly face seems eager to see you. \n\
'Hello there, sir', a woman with a strikingly beautiful appearance says earnestly. You both exchange pleasantries, and the lady, whose name is discovered to be \n\
Olivia, shows you to your room, and you get settled into your new living situation. While having dinner, you find out that Olivia is a widow, whose husband was killed \n\
in the past war. You give her your condolences, and the rest of the dinner is spent in silence. But, while getting ready for bed, you find \n\
a letter slipped under the carpet, which reveals that Olivia also aims to slay the dragon. Hearing some noise behind you, you see Olivia standing in the \n\
doorway, dagger in hand, and asks in a serious voice:'Well, now that you know, are you with me, or against me?'")
print("Of course, you say you're with her, and then proceed to tell her all about why you've come to the castle anyways. \n\
With Olivia vectorening intently, you curiously ask her why everyone acts so strange around town to newcomers, and then Olivia responds eagerly:'Joshro's \n\
goons have threatened the villagers with execution so much that no one dares to step out of line, even to help poor but handsome souls like yourself.' \n\
Taking in this information, as well as Olivia's flirting, you realize that you're going to have to be much more careful so as not to attract any \n\
unwanted attention. The conversation ends, and you go to sleep swiftly.")
print("Before you leave the house, Olivia rushes to you and gives you a hug, and through stifled sniffles, tells you to stop by her house again after you complete your mission, \n\
promising you a place to lay low after the dragon is slayed. You accept the offer, and Olivia nods before letting you know that she'll be out of town for a few days to visit her husband's funeral.")
trackEndings.append("Do you want to go back to the cottage and live with 'Olivia'?")
endingThree = True
#ENDING THREE
elif x == "tavern":
print("Entering the tavern, you search out the bartender and negotiate the cost it'll take to rent out a spare room. A deal is settled, and the bartender shows you around the place before directing you to your room. \n\
You get settled in for the night, eating some food that you bought earlier from the bartender and just thinking about everything that's happened recently. \n\
In one of the drawers, you find a damaged note that warns its readers to be weary of Joshro's henchmen. You dismiss this, and get in bed before falling asleep easily.")
elif x == "inn" :
    print("Once inside the inn, you eat a hearty meal in the lobby before purchasing a room. As you walk past the many rooms, an old and disheveled man grabs you and warns you to stay weary of the \
'Dragon's Devils'. You push the man off you and hurry to your room. The man's words still linger in your mind, but you shut them off and go to sleep begrudgingly.")

player.currentHealth = player.maxHealth

time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("After leaving the", x, "in the morning, you turn the corner, but come face to face with a hideously disfigured mutant that shoves you to the ground before \n\
exclaiming:'LeAvE, RiGhT nOw.' Obviously, you don't move, and the mutant spits on the ground before getting into a battle stance, and as such you do the same.")
location = "random street"
player.currentDeathMessage = "The mutants heavy punch can stun you if you're not careful, it can be best to dodge it."
fightSequence([mutant], False, [[]] )
if restart:
return
printOutro()
spareKill = input("The mutant crashes to the ground, pleading with you to spare it. Do you 'spare' or 'kill' the mutant? ")
while spareKill != "spare" and spareKill != "kill" :
    spareKill = input("That won't work this time! Do you 'spare' or 'kill' the mutant? ")
    if spareKill == "spare" :
        brutalEnding = False
        print("You let the mutant go, and it shambles off into the early morning darkness.")
        print("Then your " + player.weapon.name + " learns 'spare'.")
        player.weapon.LearnAttack(spare)
        emptyStr, strings = stringWord(emptyStr)
        elif spareKill == "kill":
print("You end the mutant.")
print("The mutant shrivels up, and you jump back in horror as the supernatural occurrence unfolds in front of your eyes. \n\
Death is something you've sort of gotten used to after all of the previous fights you've been in, \n\
but this definitely takes the cake. You dust yourself off, and resume walking around the town in search of answers.")
# ADD A NEW ATTACK HERE OR SOMETHING.
if restart:
return
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
codeFind()
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
location = "sewers"
print(introMessages[6])
player.currentDeathMessage = "Maybe you should try to come here with more health, or try to outrun them."
print("Before beginning your attack on the rat, you remember that you used to take \n\
medieval track and field at your former academy, and ponder over whether or not you should see if your skills are still in tip-top shape. \n\
The little voice in your head warns you, though, that if your health has not been increased \n\
past the default 100 value, you might not be able to outrun the rambunctious rodent after all.")
outrunFight = input("Do you 'fight' or 'outrun' the rat? ")
while outrunFight != "fight" and outrunFight != "outrun":
outrunFight = input("That won't work this time! Do you 'fight' or 'outrun' the rat? ")
if outrunFight == "fight" :
    fightSequence([rat, babyRat, babyRat], False, [["Baby Rat"], ["Baby Rat", "Baby Rat"]] )
    if restart :
        return
        if specialFightEnding :
            ratOrRats = "rat"
            if len(specialFightEndingMonsters) > 1:
ratOrRats = "rats"
prompt = input("After you defeat the parent rat, the " + ratOrRats + " do you want to chase after them? ('yes' or 'no') ")
while prompt != "yes" and prompt != "no" :
    prompt = input("That won't work this time. Do you want to chase after them? ('yes' or 'no') ")
    if prompt == "yes" :
        fightSequence(specialFightEndingMonsters, False, [[]] )
    else :
        brutalEnding = False
        print("You decide to spare the " + ratOrRats + ".")
        if not player.weapon.KnowsAttack("spare") :
            print("Then your " + player.weapon.name + " learns 'spare'.")
            player.weapon.LearnAttack(spare)
            printOutro()
            elif outrunFight == "outrun" :
            print("You decide to try your luck and athletic skills by ceasing to fight the rat and instead violently thrash through the water \n\
to attempt to reach a safe distance.")
if player.currentHealth > 100:
print("You end up creating such a large distance between you and the rodent that it eventually just gives up and waddles away in the other direction.")
emptyStr, strings = stringWord(emptyStr)
brutalEnding = False
elif player.currentHealth <= 50 :
    fastNot = random.randint(1, 2)
    if fastNot == 1 :
        print("Despite your determination in trying to outrun the creature, it catches up to you and drags you down to the murky depths... \n\
and you never resurface.")
player.currentDeathMessage = "It feels like you could have made that if only you were luckier or had more health."
end()
return
    else:
print("In a surprising turn of events, you manage to wade quickly enough away from the rat that it decides that you're not worth the trouble, \n\
and waddles away in the other direction. In the process though you did cut your knee on a rock.")
emptyStr, strings = stringWord(emptyStr)
brutalEnding = False
if player.currentHealth > 25:
player.currentHealth -= 10
else:
player.currentHealth = max(1, min(10, player.currentHealth - 10))
else:
fastNot = random.randint(1, 2)
if fastNot == 1 :
    print("Despite your determination in trying to outrun the creature.\n\
However, it doesn't fully defeat you when it catches up with you, and as such you are forced to fight it.")
player.currentHealth -= 50
fightSequence([deepcopy(rat), deepcopy(babyRat), deepcopy(babyRat)], False, [["Baby Rat"], ["Baby Rat", "Baby Rat"]] )
if restart :
    return
    if specialFightEnding :
        ratOrRats = "rat"
        if len(specialFightEndingMonsters) > 1:
ratOrRats = "rats"
prompt = input("After you defeat the parent rat, the " + ratOrRats + " do you want to chase after them? ('yes' or 'no') ")
while prompt != "yes" and prompt != "no" :
    prompt = input("That won't work this time. Do you want to chase after them? ('yes' or 'no') ")
    if prompt == "yes" :
        fightSequence(deepcopy(specialFightEndingMonsters), False, [[]] )
    else :
        brutalEnding = False
        print("You decide to spare the " + ratOrRats + ".")
        if not player.weapon.KnowsAttack("spare") :
            print("Then your " + player.weapon.name + " learns 'spare'.")
            player.weapon.LearnAttack(spare)
            printOutro()
        else:
print("In a surprising turn of events, you manage to wade quickly enough away from the rat that it decides that you're not worth the trouble, \n\
and waddles away in the other direction. In the process though your knee got cut by a rock.")
player.currentHealth -= 25
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("You eventually reach the end of the tunnel and climb up the ominous ladder to find a compound full of mutants, specifically ones that look \n\
much more deadly and aggressive than the one you encountered earlier. You'll need to get a disguise, and in order to do that, you must gain access to the \n\
armory. The only issue with that is that a key is required to enter the building, and you don't have one. \n\
You faintly remember that many keys are commonly kept in the main watchtower, so you sneak over there and find that the key you need is locked inside an \n\
intricately designed mathematical padlock that protects a metal box. You read a note that gives the instructions needed to bypass this, and begin working.")
randomNumtosolve(random.choice(divByfour))
print("You complete the puzzle, and unlock the box to find the key you need. You exit the main watchtower without arousing suspicion from the nearby mutants, \n\
and continue on your way to the armory.")
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("Once reaching the armory, you insert the key and quietly let yourself inside the armory. The armor present will do more than enough to hide your non-mutant \n\
appearance, and, in addition to that, offers more protection.")
player.maxHealth += 50
print("Approaching the keep, you see a human guard ahead that won't move no matter what distractions you use to guide him away. After some impatient waiting, you conclude \n\
that the only way to enter the keep is through a direct encounter with the guard. A fight is inevitable, but the only question now is *how* the interaction with the guard will end...")
player.currentDeathMessage = "It seems like he doesn't want to fight... maybe there's a way to convince him to stop."
fightSequence([guard], True, [[]] )
if restart:
return
if specialFightEnding :
    brutalEnding = False
    print("The guard, whose name you soon find out is Bruce, explains to you that he doesn't even really like Joshro or his ideals, \n\
but only works for him because the pay is good and he has mouths to feed. This seems like a fair reason to work for a ruthless overlord, so you empathize with him \n\
and offer to give him the riches found inside Joshro's treasury in exchange for him helping you take on Joshro.\n\n\
After some thinking, Bruce declines your offer, and says that he is not the fighter he once was, but because he likes you, he will give you an ancient rejuvination potion. \n\
You nod in agreement, drink the potion, and enter the keep, where the evil Joshro and precious Misty are located... ")
player.maxHealth += 50
player.currentHealth = min(player.maxHealth, player.currentHealth + 50)
emptyStr, strings = stringWord(emptyStr)
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("Once inside the keep, you see that the building is separated into three chambers, with you currently being in the first. \n\
To enter the next room you first need to speak to the receptionist, who happens to be a pig. He appropriately talks to you in pig latin, \n\
but because pig latin is not really in fashion anymore, you struggle to decipher what he's saying, and must employ the use of writing to help \n\
you conversate better.")
pigLanguage()
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("Entering the second to last chamber, you find a sorcerer that offers you a boggling 100 value health potion! The only catch, he tells you, is that you must have no blood on \n\
your hands from the time you picked your weapon to the present moment. Depending on how you dealt with the various adversaries throughout the course of the journey, this will either be \n\
really helpful or a waste of potential...")
print("The sorcerer rubs his crystal ball, and reveals that......")
print(" ")
if len(emptyStr) == 6:
print("You were a pacifist! The sorcerer smiles and delights in the fact that you appreciate life, no matter if it's a troll, a human, or even a measly rat.")
print("The sorcerer points out that throughout your successful attempts at being peaceful, a letter was added to a growing vector of seemingly random letters, but in actuality, \n\
they add up to be the scrambled version of a six letter word. The only hint the sorcerer gives you is that the word is a part of what makes up cloth shirts, and also warns you that you \n\
only have three chances to guess the word... \n\
you crack your knuckles and massage your forehead in preparation for the mind-numbing task ahead...")
print("******")
print(emptyStr)
print("******")
stringCorrect = False
askString = input("What is the word unscrambled? ")
if askString == "string":
print("You got it! The sorcerer smiles with admiration as he hands you the obviously enlarged and luxurious potion full of fizzling bubbles. \n\
You drink it, and bask in the glory that is being a nonviolent person before heading to the door separating you from the girl you came to save and the unjust creature you must stop...")
player.maxHealth += 100
player.currentHealth = player.maxHealth
stringCorrect = True
else:
print("******")
print(emptyStr)
print("******")
if stringCorrect != True :
    askString = input("That won't work this time! What is the word unscrambled? ")
    if askString == "string" :
        print("You got it! The sorcerer smiles with admiration as he hands you the obviously enlarged and luxurious potion full of fizzling bubbles. \n\
    You drink it, and bask in the glory that is being a nonviolent person before heading to the door separating you from the girl you came to save and the unjust creature you must stop...")
        player.maxHealth += 100
        player.currentHealth = player.maxHealth
        stringCorrect = True
    else:
print("******")
print(emptyStr)
print("******")
if stringCorrect != True :
    askString = input("Last chance! What is the word unscrambled? ")
    if askString == "string" :
        print("You got it! The sorcerer smiles with admiration as he hands you the obviously enlarged and luxurious potion full of fizzling bubbles. \n\
You drink it, and bask in the glory that is being a nonviolent person before heading to the door separating you from the girl you came to save and the unjust creature you must stop...")
player.maxHealth += 100
player.currentHealth = player.maxHealth
stringCorrect = True
elif askString != "string":
print("You didn't guess the word, and sulk about it before heading to the door separating you from the girl you came to save and the tyrannical creature you must defeat...")
elif not brutalEnding :
    print("You weren't a pacifist! The sorcerer sighs, and because you're mad at the sorcerer for not giving you a chance, you try to attack him. The sorcerer teleports away just in time, and you hit the wall \n\
with your fist in anger. But before leaving the sorcerer comes back and says that he dislikes you far less than Joshro, and that he will give you a fair shot against him. You are healed back to full;\n\
you then head to the door separating you from Misty and Joshro...")
    else:
print("The sorcerer spits at your feet and teleports away, and you confusedly walk onwards to the door containing Joshro.")
player.currentHealth = player.maxHealth
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
if (currentSettings.sleepTime != 0) :
    time.sleep(currentSettings.sleepTime + 2)
    print("You take one last deep breath before preparing for one last fight.")
    print("")
    if (currentSettings.sleepTime != 0) :
        time.sleep(currentSettings.sleepTime + 2)

        player.currentDeathMessage = "You were so close, just keep trying!"
        fightSequence([joshrosBody], False, [[]] )
        if restart :
            return

            if specialFightEnding :
                print("And all of Joshro's heads fall to the floor.")

                if brutalEnding :
                    print("After defeating the dragon, you get ready to walk away from it's body,\n\
but unexpectadly one of the heads bites at you, and even weirder the head then seems combine with your " + player.weapon.name + ".\n\
Your " + player.weapon.name + " has learned 'ultra fire breath'.")
player.weapon.LearnAttack(ultraFireBreath)

trackEndings.append("Do you want to stick with 'Misty'?")
endingFour = True
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
for ending in trackEndings :
print(ending)
endingChosen = False
while not endingChosen :
    chooseEnding = input("What do you do (Type the name of the person)? ").lower()
    if brutalEnding :
        print("=]")
        endingChosen
        elif chooseEnding == "goblin" and endingOne == True :
        print(" ")
        print(endingOneHappens)
        endingChosen = True
        elif chooseEnding == "samantha" and endingTwo == True :
        print(" ")
        print(endingTwoHappens)
        endingChosen = True
        elif chooseEnding == "olivia" and endingThree == True :
        print(" ")
        print(endingThreeHappens)
        endingChosen = True
        elif chooseEnding == "misty" and endingFour == True :
        print(" ")
        print(endingFourHappens)
        endingChosen = True
        time.sleep(currentSettings.sleepTime)
        print(" ")
        print("++++++++++++++++")
        print(" ")
        specialThanks = input("Would you like to read the special thanks note (it's totally optional :) ) \n\
('yes' or 'no')? ")
while specialThanks != "yes" and specialThanks != "no":
specialThanks = input("That won't work this time! Do you want to read the note ('yes' or 'no')? ")
if specialThanks == "yes" :
    print("This game took me(Matthew) a little bit under 2 months to make, and it was definitely a rollercoaster \n\
of hating programming and loving it, but I'd just like to say a couple quick things about some of the people who helped/supported me through \n\
the development process... ")
time.sleep(currentSettings.sleepTime)
print(" ")
print("The first person I'd like to thank is God for allowing me to be here and have the experience to be able to make such a cool game, as well as allowing me to meet wonderful people \n\
like Jasun, Jordan(That's me, the other dev!), Corbin, and other people I'd consider friends:)")
time.sleep(currentSettings.sleepTime)
print(" ")
print("The next person is, of course, Jasun, for two very good reasons. First and foremost, without Jasun pushing me to constantly upgrade and continue learning Python as well as \n\
making the game itself, this game would've never been completed. Him helping me get through tricky lines of code is the second reason, as many of the things you experience throughout the game, \n\
such as the pig latin challenge and the normal fight scenes, wouldn't have functioned properly or at all without his advice. All in all, I'd like to thank Jasun for allowing me to do a project that I \n\
genuinely enjoyed working on (for most of the time at least), as well as helping me explore programming in general, as it is something I definitely want to pursue as a future career.")
time.sleep(currentSettings.sleepTime)
print(" ")
print("For the next person, I'm actually going to thank two people- those two people being Jordan and Corbin. When Jasun was busy helping other people out or was stumped about something \n\
concerning my code, Jordan and Corbin, without hesitation, always came to my aid to help me solve whatever issue was plagueing my code. Without them and their \n\
Albert Einstein-like brains, a substantial portion of this game would not be possible:)")
time.sleep(currentSettings.sleepTime)
print(" ")
print("Ok because there are a bunch of other people on my vector that deserve paragraphs of their own, but also because I don't want this game to be like a million lines of code, I'm going to sum up the other people very quickly:")
time.sleep(currentSettings.sleepTime)
print("MICHELLE, for being supportive of my interest in coding even through tough times:)")
time.sleep(currentSettings.sleepTime)
print("~~~~~~~~~~~~~~~~")
print("LUKE, for showing a genuine interest in seeing my game be completed and even asking questions regarding the nerdy mechanics of what certain stuff did inside the code:)")
time.sleep(currentSettings.sleepTime)
print("~~~~~~~~~~~~~~~~")
print("VICTORY, for being the person I could have a good laugh with when programming annoyed me and I needed a break:)")
time.sleep(currentSettings.sleepTime)
print("~~~~~~~~~~~~~~~~")
print("DANNY, for being interested in playing my game once it was finished, as well as making ultimate frisbee be super fun and rewarding after doing a bunch of typing the Wednesday before in STEAM Club:)")
time.sleep(currentSettings.sleepTime)
print("~~~~~~~~~~~~~~~~")
print("DOUG, CAITLIN, BENNY, and JEFF, for letting me leave their classes early so I could work on my game:)")
time.sleep(currentSettings.sleepTime)
print("~~~~~~~~~~~~~~~~")
print("MORGAN, ELIZABETH, LUKAS, and GENEVIEVE, for being pretty cool people and keeping my mental state at a good place while \n\
personal issues were taking place that could've definitely sunk my drive to finish making the game, \n\
as well as showing interest in wanting to play my game once it was finished:)")
time.sleep(currentSettings.sleepTime)
print("~~~~~~~~~~~~~~~~")
print("JASUN, LUKE, CHARLOTTE, NOX, JORDAN(I'm back again =]), CORBIN, MORGAN, SHEPARD, BENNY, LUKAS, and many others for playtesting the game!:)")
time.sleep(currentSettings.sleepTime)
print("~~~~~~~~~~~~~~~~")
print("And lastly, YOU, the player, whoever you are, for beating my broken but meaningful mess of a game!:)")
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("If you're curious about what the '(NUMBER out of 4)' means next to the name of the ending you got, try playing the game again and find out what would happen if you did things differently!:)")
else:
time.sleep(currentSettings.sleepTime)
print(" ")
print("++++++++++++++++")
print(" ")
print("Thanks for playing our game! If you're curious about what the '(NUMBER out of 4)' means next to the name of the ending you got, try playing the game again and find out what would happen if you did things differently!:)")

print("\n")
if not brutalEnding:
oneOrTwo = random.randint(1, 2)
if oneOrTwo == 1 and not player.weapon.KnowsAttack("club bash") :
    print("Hiya! It's Jordan! I'm the dev behind this 'mod',\n\
I just came by to tell you that I heard a rummor that the blacksmith had an 'ogre in a bottle' for sale, hmm, I wonder what that meant...")
elif not player.weapon.KnowsAttack("slime hug") :
    print("Hiya! It's Jordan! I'm the dev behind this 'mod',\n\
I just came by to tell you that I heard a rummor that the Pet Slime may not be as loyal as you'd thing, hmm, I wonder what that meant...")
else:
print("Hiya! It's Jordan! I'm the dev behind this 'mod',\n\
I just came by to tell you that I heard a rumor that some sorcerer can get far more mad then he normally does... hmm... I wonder what that meant...")
time.sleep(5)
else:
print("The fun's not quite over yet friends, just wait... =] =] =] =] =]\n\
    - sincerely, Jordan")

    prompt = input("Do you want to play again? ('yes' or 'no') ")
    while prompt != "yes" and prompt != "no" :
        prompt = input("'yes' or 'no'")
        restart = prompt == "yes"




















        FindSettings()
        while restart:
main()