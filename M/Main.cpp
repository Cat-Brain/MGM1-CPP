﻿// Go to https ://github.com/Matthew-12525/Create-Your-Own-Adventure for the unmodded experience. =]
/*
Hiya, this 'mod' was written by Jordan Baumann(me),
however this all wouldn't of existed without Matthew's original version of the game(link above^^^).
I hope that you enjoy this experience, and if you want, maybe you'll even mod this mod! =]
*/


#include "Combat.h"


#pragma region Variables

bool restart = true;
bool specialFightEnding = false;
vector<Entity*> specialFightEndingMonsters(0);
Settings currentSettings;
vector<char> currentStrings(0);
int divByFour[6]{ 8, 12, 16, 20, 24, 28 };
string location;
string emptyStr;

//Constant variables:
//Attacks
//The syntax for a status effect is :
//StatusEffect(InflictionType.YOURINFLICTION, how long you want it to last)
//The syntax for an attacks is :
//Attack([Status effects], [chance of each status effect happening], damage, damage randomness(how far from the original value the actual value can be), [self inflictions], [self infliction procs], self damage, self damage randomness, [summons], turns to do, name)
//First up is the attacks that are required to be early.
Attack fireBreath{ { {BURNING, 2} }, { 100 }, 0, 0, {}, {}, 0, 0, {}, 3, "fire breath" };
Attack heavyBite{ {}, {}, 50, 0, {}, {}, 0, 0, {}, 4, "heavy bite" };
//Enemies that must be declared early.
Entity joshroHead{ 25, 50, {fireBreath, heavyBite}, "Joshro Head", 0.5f };
//Normal attacks.
Attack clubBash{ { {STUN, 2} }, { 100 }, 25, 10, {}, {}, 0, 0, {}, 3, "club bash" };
Attack punch{ {}, {}, 15, 15, {}, {}, 0, 0, {}, 1, "punch" };
Attack heavyPunch{ { {STUN, 2} }, { 75 }, 25, 25, {}, {}, 0, 0, {}, 2, "heavy punch" };
Attack quickStab{ { {POISON, 3} }, { 50 }, 5, 5, {}, {}, 0, 0, {}, 1, "quick stab" };
Attack rockThrow{ { {STUN, 1} }, { 25 }, 5, 5, {}, {}, 0, 0, {}, 1, "rock throw" };
Attack slimeHug{ { {DEADLY_HUG, 3} }, { 100 }, 0, 0, {}, {}, 0, 0, {}, 1, "slime hug" };
Attack slimeSpike{ { {BLEED, 3} }, { 100 }, 5, 0, {}, {}, 0, 0, {}, 1, "slime spike" };
Attack arrowShoot{ { {BURNING, 3}, {BURNING, 3}, {POISON, 8} }, { 50, 50, 100 }, 35, 10, {}, {}, 0, 0, {}, 3, "shoot arrow" };
Attack chokeHold{ { {STUN, 1} }, { 95 }, 5, 5, {}, {}, 0, 0, {}, 1, "choke hold" };
Attack deepCut{ { {BLEED, 15}, {BLEED, 15}, {BLEED, 15} }, { 100, 50, 25 }, 0, 0, {}, {}, 0, 0, {}, 1, "deep cut" };
Attack finisher{ {}, {}, 20, 20, {}, {}, 0, 0, {}, 1, "finisher" };
Attack heavyBlow{ {}, {}, 100, 0, {}, {}, 0, 0, {}, 5, "heavy blow" };
Attack quickAttack{ {}, {}, 35, 0, {}, {}, 0, 0, {}, 2, "quick attack" };
Attack heaviestBlow{ {}, {}, 125, 0, {}, {}, 0, 0, {}, 6, "heaviest blow" };
Attack splash{ { {WET, 5} }, { 100 }, 3, 3, {}, {}, 0, 0, {}, 1, "splash" };
Attack quickClubBash{ { {STUN, 2} }, { 75 }, 10, 10, {}, {}, 0, 0, {}, 2, "quick club bash" };
Attack bite{ { {POISON, 4}, {BLEED, 4} }, { 5, 5 }, 5, 5, {}, {}, 0, 0, {}, 2, "bite" };
Attack scratch{ { {BLEED, 4} }, { 25 }, 15, 5, {}, {}, 0, 0, {}, 1, "scratch" };
Attack spare{ {}, {}, 0, 0, {}, {}, 0, 0, {}, 1, "spare" };
Attack growHead{ {}, {}, 0, 0, {}, {}, 0, 0, { &joshroHead }, 4, "grow head" };
Attack ultraFireBreath{ { {BURNING, 3} }, { 100 }, 0, 0, {}, {}, 0, 0, {}, 1, "ultra fire breath" };
Attack strengthen{ { {STRENGTHEN, 2} }, {100}, 0, 0, { {STRENGTHEN, 4} }, {100}, 0, 0, {}, 1, "strengthen" };
Attack healify{ {}, {}, -25, 25, {}, {}, -50, 50, {}, 1, "healify" };

//The syntax for enemies is :
//Enemy(start health, max health, [attack1, attack2, ...], "name", leech amount 0 to 1 work best
Entity joshrosBody{ 300, 300, { growHead }, "Joshro's Body", 0.0 };
Entity ogre{ 100, 100, { clubBash, punch }, "Ogre", 0.0 };
Entity goblin{ 100, 100, { quickStab, rockThrow }, "Goblin", 0.0 };
Entity slime{ 25, 50, { slimeHug }, "Pet Slime", 1.0 };
Entity troll{ 125, 125, { quickClubBash, splash }, "troll", 0.0 };
Entity mutant{ 200, 200, { punch, heavyPunch }, "mutant", 0.0 };
Entity rat{ 100, 200, { bite, scratch }, "Rat", 0.25 };
Entity babyRat{ 25, 50, { bite, scratch, splash }, "Baby Rat", 0.5 };
Entity guard{ 200, 200, { heavyBlow, quickAttack }, "Unloyal Guard", 0.0 };

Weapon bow{ { arrowShoot, chokeHold }, "Bow", 0.0 };
Weapon axe{ { deepCut, finisher }, "Axe", 0.0 };
Weapon sword{ { heavyBlow, quickAttack }, "Sword", 0.0 };
Weapon ogreInABottle{ { clubBash, punch }, "Ogre in a Bottle", 0.5 };
Weapon python{ { heaviestBlow, quickAttack }, "Python", 0.5 };


Player player;

#pragma endregion





#pragma region Functions
void Sleep()
{
    sleep_for(seconds(currentSettings.sleepTime));
}




void FindSettings()
{
    string prompt = Input("How many seconds do you want to wait after key events('default' = 1) ");
    int result = -1;
    if (IsNumber(prompt))
        result = stoi(prompt);
    else if (prompt == "default")
        result = 1;
    while (result == -1)
    {
        prompt = Input("It has to be a number or 'default'. How many seconds do you want to wait after key events('default' = 1) ");
        if (IsNumber(prompt))
            result = stoi(prompt);
        else if (prompt == "default")
            result = 1;
    }
    currentSettings = Settings(result);
}




void WeaponSelect()
{
    string weaponChoice = Input("Do you take a 'bow', 'axe', or 'sword'?: ");
    while (weaponChoice != "bow" && weaponChoice != "axe" && weaponChoice != "sword" && weaponChoice != "ogre in a bottle")
    {
        weaponChoice = Input("That weapon isn't here! Do you take a 'bow', 'axe', or 'sword'?: ");
    }

    if (weaponChoice == "bow")
    {
        player.weapon = bow;
        printf("A standard bow.\n\
It's a slow weapon that stays inside of enemies and damages them over time.\n\
Good if you want to forget about some foes, but bad at finishing things within a timely manor.");
    }
    else if (weaponChoice == "axe")
    {
        player.weapon = axe;
        printf("A pair of small battle axes.\n\
They're quick weapons that do damage over time, and accumulate their damage instead of giving it to you upfront\n\
good at single target and very small targets, but bad against leech and terrible at quick fights.");
    }
    else if (weaponChoice == "sword")
    {
        player.weapon = sword;
        printf("A steel longsword.\n\
It does high damage, but does all of it's damage upfront but does enough damage to one shot most foes, good at big foes, but bad at small ones and long fights.");
    }
    else if (weaponChoice == "ogre in a bottle")
    {
        player.weapon = ogreInABottle;
        printf("BONK");
    }
}




void End()
{
    restart = true;
    printf("You have been slain.\n%s\nPress 'enter' on your keyboard to start a new game. :)\n\n\n", player.currentDeathMessage.c_str());
    string ignore;
    std::getline(cin, ignore);
}




void CodeFind()
{
    vector<vector<string>> library{
        {"'WEB OF CHARLOTTE'S'", "INsIDe THE wATer, CHARLOTTE SEARCHED FOR WILBUR."},
        {"'THE GODDESS OF THE NECKLACES'", "FROODOO lookED At The EYE OF SOUrON INSANeLY."},
        {"'FURRY PLANTER'", "hARRY urGENTLY NEEDED TO GET TO THE BrEWERy BEFORE LORD VOLDEMORT."}
    };
    string finalCodes[3]{ "sewer", "lookthere", "hurry" };

    for (int i = 0; i < library.size(); i++)
    {
        bool correctlyEnteredCode = false;
        for (int j = 0; !correctlyEnteredCode; j = (j % library[i].size() + library[i].size()) % library[i].size())
        {
            printf("%s", library[i][j].c_str());
            string next = Input("Do you want to go to the 'next' verse or 'enter' the code or go 'back' to the original page: ");
            while (next != "next" && next != "enter" && next != "back")
                next = Input("That won't work this time! Do you want to go to the 'next' verse or 'enter' the code or go 'back' to the original page: ");
            if (next == "next")
            {
                j++;
            }
            else if (next == "back")
            {
                j--;
            }
            else while (!correctlyEnteredCode)
            {
                string code = Input("Enter the code (or type 'go back to page' if you need to check again): ");
                if (code == finalCodes[i])
                {
                    correctlyEnteredCode = true;
                    printf("You got it!");
                }
                else if (code == "go back to page")
                    break;
            }
        }
    }
    printf("You piece the code together, and find that the completed code is 'sewer look there hurry'. You exit the library, and hurry off\n\
to the local sewer entrance to search for the secret entrance to the fortress where Misty is held.");
}




void RandomNumtosolve()
{
    int number = divByFour[RandRange(0, 5)];
    int playerNum = 0;
    while (playerNum < number)
    {
        printf("%i\n", number);
        printf("^ NUMBER TO REACH ^\n");
        printf("%i\n", playerNum);
        printf("^ YOUR CURRENT TOTAL ^\n");
        string inputString = Input("Choose a number that is less than 1/4\n\
of what the number is, and try to add up to the sum of it (keep in mind you'll have to add more numbers to this one): ");
        int numberGiven = number;
        if (!IsNumber(inputString))
        {
            printf("That won't work this time! Must be a number!");
            continue;
        }

        numberGiven = stoi(inputString);
        if (numberGiven >= number / 4)
        {
            printf("That won't work this time! Must be under the number to reach divided by 4.");
            continue;
        }

        playerNum += numberGiven;
        if (playerNum > number)
        {
            printf("You went over the number, and have to start over!");
            number = divByFour[RandRange(0, 5)];
            playerNum = 0;
        }
            
        if(playerNum == number)
            printf("You did it! Good job!");
    }
}




void FightSequence(vector<Entity> enemyTypes, bool spareable, vector<vector<string>> specialEnding)
{
    specialFightEnding = false;
    bool fightOn = true;
    bool fightFrameOne = true;
    int target = 0;

    vector<Entity*> enemies = vector<Entity*>(enemyTypes.size());
    for (int i = 0; i < enemies.size(); i++)
        enemies[i] = new Entity(enemyTypes[i]);

    while (fightOn)
    {
        printf("");
        for (Entity* enemy : enemies)
            printf("%s's health: %i\n", enemy->name.c_str(), enemy->health);
        printf("Max's health: %i\n", player.health);

        if (fightFrameOne)
        {
            fightFrameOne = false;
            for (Entity* enemy : enemies)
                enemy->FindNewAttack();
            player.weapon.SwitchAttacks("Do you want to use ", false);
        }

        Sleep();
        printf("\n++++++++++++++++\n\n");

        bool spare = player.weapon.CurrentAttack()->name == "spare";
        string dialogue = "'dodge' or '" + string(spare? "spare" : "attack") + "' to continue your current attack or 'switch' your attack";
        if (enemies.size() >= 2)
            dialogue += " or 'change' targets from " + enemies[target]->name;
        dialogue += "? ";
        string prompt = Input(dialogue);
        while (prompt != "dodge" && ((prompt != "spare" && spare) || (prompt != "attack" && !spare)) && prompt != "switch" && (!(enemies.size() > 1 && prompt == "change")))
            prompt = Input("That won't work this time! " + dialogue);


        if (prompt == "dodge")
        {
            printf("\n");
            for (int i = 0; i < enemies.size(); i++)
            {
                if (!player.IsStunned())
                {
                    if (!enemies[i]->IsStunned())
                    {
                        int unblockedDamage = 0;
                        int blockedDamage = 0;
                        if (!enemies[i]->IsStunned())
                        {
                            TurnReturn tr = enemies[i]->TakeTurn(i);
                            for (int j = 0; j < tr.summons.size(); j++)
                            {
                                (tr.summons[j])->summoned = true;
                                printf("%s has birthed a new %s!\n", enemies[i]->name.c_str(), tr.summons[j]->name.c_str());
                                enemies.push_back(new Entity(*tr.summons[j]));
                            }

                            unblockedDamage += tr.hit.damage;
                            int damageDelt = tr.hit.damage / 2;
                            blockedDamage += damageDelt;
                            int heal = static_cast<int>(floorf(damageDelt * enemies[i]->leech));
                            if (heal != 0)
                            {
                                printf("%s heal's off of you for %i.\n", enemies[i]->name.c_str(), heal);
                                enemies[i]->health = std::min(enemies[i]->maxHealth, enemies[i]->health + heal);
                            }
                            player.ApplyHit(Hit(damageDelt, tr.hit.inflictions, i), true);
                            printf("You dodged the attack and took %i damage instead of taking %i damage!\n", blockedDamage, unblockedDamage);
                        }
                        else
                            printf("%s did not attack this round as they were stunned.\n", enemies[i]->name.c_str());
                    }
                    else
                    {
                        printf("%s did not attack this round as they were stunned.\n", enemies[i]->name.c_str());
                        printf("");
                    }
                }
                else
                {
                    if (!enemies[i]->IsStunned())
                    {
                        TurnReturn tr = enemies[i]->TakeTurn(i);
                        for (int j = 0; j < tr.summons.size(); j++)
                        {
                            (tr.summons[j])->summoned = true;
                            printf("%s has birthed a new %s!\n", enemies[i]->name.c_str(), tr.summons[j]->name.c_str());
                            enemies.push_back(new Entity(*tr.summons[j]));
                        }

                        player.ApplyHit(tr.hit, false);
                        int heal = static_cast<int>(floorf(tr.hit.damage * enemies[i]->leech));
                        if (heal != 0)
                        {
                            printf("%s heal's off of you for %i.\n", enemies[i]->name.c_str(), heal);
                            enemies[i]->health = std::min(enemies[i]->maxHealth, enemies[i]->health + heal);
                        }
                        printf("Becuase you were stunned you didn't block.\n");
                    }
                    else
                    {
                        printf("%s did not attack this round as they were stunned.\n", enemies[i]->name.c_str());
                        printf("");
                    }
                }
            }

            for (int i = 0; i < enemies.size(); i++)
            {
                InflictionResults ir = enemies[i]->UpdateInflictions();
                for (int j = 0; j < ir.damageFromSources.size(); j++)
                {
                    int heal = static_cast<int>(floorf(ir.damageFromSources[j] * player.weapon.leech));
                    if (heal != 0)
                    {
                        printf("You heal off of %s for %i because of %s.\n", enemies[i]->name.c_str(), heal, ir.names[j].c_str());
                        player.health = std::min(player.maxHealth, player.health + heal);
                    }
                }
            }

            InflictionResults ir = player.UpdateInflictions();
            for (int i = 0; i < ir.damageFromSources.size(); i++)
            {
                int heal = static_cast<int>(floorf(ir.damageFromSources[i] * enemies[ir.originalAttackers[i]]->leech));
                if (heal != 0)
                {
                    printf("%s heal's off of you for %i because of %s.\n", enemies[ir.originalAttackers[i]]->name.c_str(), heal, ir.names[i].c_str());
                    enemies[ir.originalAttackers[i]]->health = std::min(enemies[ir.originalAttackers[i]]->maxHealth, enemies[ir.originalAttackers[i]]->health + heal);
                }
            }
        }
        else if (prompt == "attack" || prompt == "spare")
        {
            printf("");
            for (int i = 0; i < enemies.size(); i++)
            {
                if (!enemies[i]->IsStunned())
                {
                    TurnReturn tr = enemies[i]->TakeTurn(i);
                    for (int j = 0; j < tr.summons.size(); j++)
                    {
                        (tr.summons[j])->summoned = true;
                        printf("%s has birthed a new %s!\n", enemies[i]->name.c_str(), (tr.summons[j])->name.c_str());
                        enemies.push_back(new Entity(*tr.summons[j]));
                    }

                    player.ApplyHit(tr.hit, false);
                    int heal = static_cast<int>(floorf(tr.hit.damage * enemies[i]->leech));
                    if (heal != 0)
                    {
                        printf("%s heal's off of you for %i.\n", enemies[i]->name.c_str(), heal);
                        enemies[i]->health = std::min(enemies[i]->maxHealth, enemies[i]->health + heal);
                    }
                }
                else
                {
                    printf("%s did not attack this round as they were stunned.\n", enemies[i]->name.c_str());
                }
            }

            for (int i = 0; i < enemies.size(); i++)
            {
                InflictionResults ir = enemies[i]->UpdateInflictions();
                for (int j = 0; j < ir.damageFromSources.size(); j++)
                {
                    int heal = static_cast<int>(floor(ir.damageFromSources[j] * player.weapon.leech));
                    if (heal != 0)
                    {
                        printf("You heal off of %s for %i because of %s.\n", enemies[i]->name.c_str(), heal, ir.names[j].c_str());
                        player.health = std::min(player.maxHealth, player.health + heal);
                    }
                }
            }

            if (!player.IsStunned())
            {
                if (spare)
                {
                    if (spareable)
                    {
                        bool spareSucceeds = RandRange(1, 3) == 3;
                        if (spareSucceeds)
                        {
                            printf("You attempt to spare and are successful!\n");
                            specialFightEnding = true;
                            specialFightEndingMonsters = enemies;
                            fightOn = false;
                            break;
                        }
                        printf("You attempt to spare and are unsuccessful.\n");
                    }
                    else
                        printf("It seems that this foe does not want to be spared.\n");
                }
                else
                {
                    TurnReturn tr = player.TakeTurn();
                    enemies[target]->ApplyHit(tr.hit);
                    player.health = std::min(player.maxHealth, player.health + static_cast<int>(floorf(tr.hit.damage * player.weapon.leech)));
                    printf("");
                }
            }
            else
                printf("Becuase you were stunned you didn't attack.\n");

            InflictionResults ir = player.UpdateInflictions();
            for (int i = 0; i < ir.damageFromSources.size(); i++)
            {
                int heal = static_cast<int>(floorf(ir.damageFromSources[i] * enemies[ir.originalAttackers[i]]->leech));
                if (heal != 0)
                {
                    printf("%s heal's off of you for %i because of %s.\n", enemies[ir.originalAttackers[i]]->name.c_str(), heal, ir.names[i].c_str());
                    enemies[ir.originalAttackers[i]]->health = std::min(enemies[ir.originalAttackers[i]]->maxHealth, enemies[ir.originalAttackers[i]]->health + heal);
                }
            }
        }
        else if(prompt == "switch")
            player.weapon.SwitchAttacks("", true);

        else
        {
            int answer = -1;
            while (answer < 0 || answer > enemies.size())
            {
                for (int i = 0; i < enemies.size(); i++)
                    printf("%s with %i health: '%i'\n", enemies[i]->name.c_str(), enemies[i]->health, i + 1);
                string prompt = Input("or 'nevermind'.\n");
                if (prompt == "nevermind")
                    answer = target;
                else if (IsNumber(prompt))
                    answer = stoi(prompt) - 1;
            }
            target = answer;
        }

        //                        \/ \/ \/ Just a failsafe, shouldn't normally come up. Will not say "You won!".
        if (player.health <= 0 || enemies.size() == 0)
        {
            fightOn = false;
            break;
        }

        int enemiesRemovedThisFrame = 0;
        for (int i = 0; i < enemies.size() + enemiesRemovedThisFrame; i++)
            if (enemies[i - enemiesRemovedThisFrame]->health <= 0)
            {
                printf("%s has been defeated!\n", enemies[i - enemiesRemovedThisFrame]->name.c_str());

                if (enemies.size() <= 1)
                {
                    printf("You won!\n");
                    fightOn = false;
                    break;
                }

                int inflictionsRemovedThisFrame = 0;
                for (int j = 0; j < player.inflictionAttackers.size() + inflictionsRemovedThisFrame; j++)
                {
                    if (player.inflictionAttackers[j - inflictionsRemovedThisFrame] == i - enemiesRemovedThisFrame)
                    {
                        printf("%s has worn off.\n", player.inflictions[j - inflictionsRemovedThisFrame].Name().c_str());
                        player.inflictionAttackers.erase(player.inflictionAttackers.begin() + j - inflictionsRemovedThisFrame);
                        player.inflictions.erase(player.inflictions.begin() + j - inflictionsRemovedThisFrame);
                        inflictionsRemovedThisFrame++;
                    }
                    else if (player.inflictionAttackers[j - inflictionsRemovedThisFrame] > i - enemiesRemovedThisFrame)
                        player.inflictionAttackers[j - inflictionsRemovedThisFrame]--;
                }
                enemies.erase(enemies.begin() + i - enemiesRemovedThisFrame);
                enemiesRemovedThisFrame++;
                if (target >= enemies.size())
                    target--;
            }

        vector<string> enemyNames = vector<string>(enemies.size());
        for (int i = 0; i < enemies.size(); i++)
            enemyNames[i] = enemies[i]->name;

        for (vector<string> option : specialEnding)
            if (option == enemyNames)
            {
                string printableCombinedEnemyNames = enemies[0]->name;
                if (enemies.size() == 2)
                {
                    if (enemies[0]->name == enemies[1]->name)
                        printableCombinedEnemyNames += "s";
                    else
                        printableCombinedEnemyNames += " and the " + enemies[1]->name;
                }
                else if (enemies.size() > 2)
                {
                    for (int i = 1; i < enemies.size() - 1; i++)
                        printableCombinedEnemyNames += ", the " + enemies[i]->name;
                    printableCombinedEnemyNames += ", and the " + enemies[enemies.size() - 1]->name;
                }

                bool hasHave = enemies.size() > 1; // false = has, true = have;

                printf("The %s %s chosen to stop fighting.\n", printableCombinedEnemyNames.c_str(), hasHave ? "have" : "has");

                fightOn = false;
                specialFightEnding = true;
                specialFightEndingMonsters = enemies;
                break;
            }

        bool allSummoned = true;
        for (Entity* enemy : enemies)
            allSummoned &= enemy->summoned;

        if (allSummoned)
        {
            string printableCombinedEnemyNames = enemies[0]->name;
            if (enemies.size() == 2)
            {
                if (enemies[0]->name == enemies[1]->name)
                    printableCombinedEnemyNames += "s";
                else
                    printableCombinedEnemyNames += " and the " + enemies[1]->name;
            }
            else if (enemies.size() > 2)
            {
                for (int i = 1; i < enemies.size() - 1; i++)
                    printableCombinedEnemyNames += ", " + enemies[i]->name;
                printableCombinedEnemyNames += ", and the" + enemies[enemies.size() - 1]->name;
            }


            bool hasHave = enemies.size() > 1; // false = has, true = have;

            printf("The %s %s chosen to stop fighting.\n", printableCombinedEnemyNames.c_str(), hasHave ? "have" : "has");
            fightOn = false;
            specialFightEnding = true;
            specialFightEndingMonsters = enemies;
            break;
        }
    }




    if (player.health <= 0)
    {
        End();
        return;
    }

    player.inflictions.clear();
    player.inflictionAttackers.clear();
    player.health = std::min(player.maxHealth, player.health + 50);
    printf("You end the fight with %i health.\n", player.health);
}




void DeadlyTreasure()
{
    printf("While walking through the forest, you come across a golden box \
that shimmers in the sunlight. A key lays inside the tree next to the box, \
but the tree hole is too dark for you to see what else is inside the tree.\n");
    string key = Input("Do you 'take the key' or 'let it be'? ");
    while (key != "take the key" && key != "let it be")
        key = Input("That won't work this time! Do you 'take the key' or 'let it be'? ");
    if (key == "take the key")
    {
        printf("You grab the key quickly, thinking that history favors the bold, and open the box to find a potion \
that increases your health by 50. You drink it, and continue along the path.\n");
        player.maxHealth += 50;
        player.health = player.health + 50;
    }
    else if (key == "let it be")
        printf("You decide it's not worth the risk, and continue walking on the road.\n");
}




void PigLanguage()
{
    string answer = Input("\nThe pig says, 'Atwhay isyay ouryay amenay?'\nWhat did the pig say in English? ");
    while (answer != "What is your name?")
        answer = Input("That won't work this time! What did the pig say in English? ");
    answer = Input("What is your character's name? ");
    while (answer != "Axmay")
        answer = Input("That won't work this time! What is your character's name? ");
    printf("The pig then says, 'Areyay ouyay ethay ersonpay at'sthay upposedsay otay ebay ivinggay Oshrojay ishay ailyday assagemay?' ");
    answer = Input("What did the pig say in English? ");
            while(answer != "Are you the person that's supposed to be giving Joshro his daily massage?")
            answer = Input("That won't work this time! What did the pig say in English? ");
                answer = Input("*Are* you the person that's supposed to be giving Joshro his daily massage(hint: you should *probably* say yes)? ");
                while(answer != "Esyay")
                answer = Input("That won't work this time!\n\
*Are* you the person that's supposed to be giving Joshro his daily massage(hint: you should *probably* say yes)? ");
printf("\nYou finish checking in with the pig, and enter the next chamber, eager to advance to the second to last stage of your journey.");
}




void RiverEscape() // What if you had defeat an encounter while under a time limit, or if you had to stall for a while.
{
    int stepsYes = RandRange(5, 15);
    printf("You reach a river that could separate you from the ferocious carnivores, if you can reach the other side \
of course. Fortunately, an old fisherman is seen walking towards his flimsy boat, and you take this opportunity to ask the man \
for safe passage across the swift rapids. The only issue is, the man is not picking up the social cues that indicate your panic, so \
you must repeatedly prod him into picking up the pace to save both of your souls!\n");
    printf("You need to continously say yes to the man %i times before the wolves advance the %i \
steps needed to close the distance and devour you.\n", stepsYes, stepsYes);
    int prod = 0;
    int wolfRun = 0;
    while (prod != stepsYes && wolfRun != stepsYes)
    {
        string prodInput = Input("Say 'yes': ");
        if (prodInput == "yes")
            prod = prod + 1;
        printf("You've prodded the fisherman %i total time(s)\n", prod);
        int wolfAdvance = RandRange(1, 4);
        if (wolfAdvance == 1)
            printf("The werewolves didn't advance at all and are still at %i total step(s)\n", wolfRun);
        else
        {
            wolfRun++;
            printf("The werewolves advanced %i total step(s)\n", wolfRun);
        }
    }
    if (prod == stepsYes)
        printf("The fisherman FINALLY finishes talking and gets the boat going, and you breathe a much-deserved \
sigh of relief as you sit back and watch the werewolves cower in fear from the rushing water.\n\
Once you get across the river, the fisherman bids you farewell, and you venture on towards the castle.\n");
    else
    {
        printf("The fisherman doesn't seem to get the hint, even after the %i times you told him to hurry up. \
The werewolves close in swiftly, and you and the fisherman become a hungry family's next meal.", prod);
        player.currentDeathMessage = "It looks like you may need to work on your talking skills.\n";
        End();
    }
}




void CastleEntrance()
{
    bool stick = false;
    bool placeholder = false;
    while (stick == false or placeholder == false)
    {
        string whereTolook = Input("There are several areas of interest nearby that may contain the items you need, which include: a 'shed', \
an old 'stable', or a strangely shaped 'tree'. Where do you look? ");
        if (whereTolook == "stable")
        {
            printf("You enter the dusty and decrepit stable, where you make a mental note of how rotten the wooden interior looks from the several \
years of neglect. Fortunately, a piece of wood that isn't entirely ruined juts out of the wall, and you decide that this will suffice as\n\
a lever.%s", stick ? "" : " You grab the STICK, and exit the stable.");
            stick = true;
            Sleep();
        }
        else if (whereTolook == "shed")
        {
            printf("You enter the small shed, and laugh to yourself as you observe just how little gardening tools and other trinkets typically \
found in a shed are actually present. One thing that isn't present, however, is the placeholder, or even something that could pass for one. \
Saddened by this, you leave the shed.");
            Sleep();
        }
        else if (whereTolook == "tree")
        {
            printf("After having a previous encounter when it comes to sticking your hand inside trees, you hesitate before blindly \
shoving your hand inside the dark crevice, and %s", placeholder ? "find nothing." : "your hand connects with something cold- the PLACEHOLDER! You don't test your luck any further\n\
and exit the hole, PLACEHOLDER in hand.");
            Sleep();
            placeholder = true;
        }
        else printf("That won't work this time! ");
    }
    printf("Using your knowledge as the village carpenter, you combine these two objects into a functional lever, and place it in the appropriate location\n\
next to the gate.");
    printf("Hearing the gears churn inside the gate's various mechanisms, you take this as a good sign as you see the gate slowly open.\n\
Huzzah! The gate has opened and you finally come to the last part of your journey... or so you think;)");
}




void StringWord(string& emptyStr)
{
    int letter = RandRange(0, static_cast<int>(currentStrings.size()) - 1);
    emptyStr += currentStrings[letter];
    currentStrings.erase(currentStrings.begin() + letter);
    printf("******\n%s\n******\n", emptyStr.c_str());
}




bool Cave()
{
    printf("You hesitantly enter the cave, anxious about who or what might await you inside the dark depths. Fortunately, luck\n\
seems to favor you in this instance, and no trace of another inhabitant is visible inside the damp enclosure. In fact, you find\n\
a perfectly preserved elixir that has the label 'Miriam's Medical Miracle' stamped in fine lettering across the brownish glass.\n\
Before you take a swig, you doubt how safe ingesting the bottle's contents will be, and pause.");
    string drinkDroptake = Input("Do you 'drink' or 'drop' or 'take' the bottle? ");
    while (drinkDroptake != "drink" && drinkDroptake != "drop" && drinkDroptake != "take")
    {
        drinkDroptake = Input("That won't work this time! Do you 'drink' or 'drop' or 'take' the bottle? ");
    }
    if (player.weapon.name == "Pet Slime")
    {
        printf("But before being able to do anything,\n\
your slime pet jumps from your bag and consumes the entire bottle, glass included. Slime Pet has learned 'slime spike'");
        player.weapon.LearnAttack(slimeSpike);
        return false;
    }
    if (drinkDroptake == "drink")
    {
        printf("You drink the potion, but after sitting down, you instantly pass out. You wake up in the morning feeling strangely\n\
healthier, and relish in the fact that you now have 50 more health!");
        player.maxHealth += 50;
        player.health = player.health + 50;
        return false;
    }
    if (drinkDroptake == "drop")
    {
        printf("You drop the potion carelessly on the ground, and am unsurprised as you witness the potion's contents dissolve the hard rock\n\
below. You go to sleep, and wake up, happy that you avoided a possibly deadly drink.");
        return false;
    }
    // Must be take
    printf("For some almost supernatural reason, you feel as though you can use the potion for something greater, and decide\n\
to store it in your pack for later use. You have a dream that a large green creature asks you for the potion in exchange for\n\
safe passage across a path, but brush it off as your mind playing tricks on you and have a pretty uneventful rest of the night.\n");
    return true;
}




void SwordPull()
{
    string grunt[5]{ "gurr", "rawr", "guh", "rawr", "gurr" };
    bool exercise = true;
    bool failureFromExhaustion = false;
    while (exercise)
    {
        int distance = 0;
        for (int i = 0; i < 5; i++)
        {
            string muscle = Input("Exert your strength: ");
            
            distance++;
            for (int j = 0; j < distance; j++)
                printf("%s ", grunt[j].c_str());

            if (muscle != grunt[i])
            {
                distance = 0;
                if (player.health < 50)
                {
                    exercise = false;
                    failureFromExhaustion = true;
                    break;
                }
                player.health -= 25;
                printf("\nYou strain your ack leaving you with only %i health, and have to start over again.\n", player.health);
                break;
            }
            if (i == 4)
            {
                exercise = false;
                printf("Huzzah! The sword finally nudges out of its nested position, and you bask in its strangely magical glory. 'The Python' is\n\
engraved on the side of the blade, and you deduce that this is the name of the sword.\n\
Attached to the handle of the blade is a leaflet that reveals why 'The Python' feels so powerful: 'The blade decides \
the enemy's fate, not you'. You take this as a good sign, and place the sword at your side as you get ready for bed.\n");
            }
        }
    }
    if (failureFromExhaustion)
        printf("You succumbed to the sleepiness and have failed the sword.");
    else if (player.weapon.name == "Pet Slime")
    {
        printf("Your Pet Slime then jumps out and eats the sword! Your Pet Slime has learned heaviest blow, it also would like to know be known as Pet Slime, Decider of Fates.");
        player.weapon.LearnAttack(heaviestBlow);
        player.weapon.name = "Pet Slime Decider of Fates";
    }
    else
    {
        player.weapon = python;
        printf("The night is otherwise\n\
uneventful, and you wake up feeling strangely refreshed after having that small victory the previous night!");
    }
}




void WatchTower()
{
    printf("You enter the grey space, and find the area surprisingly empty, save for a single longsword of strikingly amazing quality.\n\
The only issue is the fact that the sword is coincidentally stuck in a slab of stone. You've seen this somewhere before, and know that\n\
swords stuck in stone are probably very good, but it's getting late and all that walking has you in an increasingly apparent state of exhaustion.");
    string tryAvoid = Input("Do you want to 'try' to take the sword out of the stone or 'avoid' the strenuous act altogether? ");
    while (tryAvoid != "try" && tryAvoid != "avoid")
        tryAvoid = Input("That won't work this time!\n\
Do you want to 'try' to take the sword out of the stone or 'avoid' the strenuous act altogether? ");
    if (tryAvoid == "try")
    {
        printf("As everyone knows, making grunting sounds while exerting energy always helps you accomplish the desired task.\n\
This goes the same when trying to pull metal out of stone, and in order to successfully dislodge the blade from its spot,\n\
you must alternate between 'guh', 'gurr', and 'rawr'.");
        SwordPull();
    }
    else
        printf("You decide that your rest is more important than something that almost certainly seems as if it could be useful to you,\n\
and go to sleep restlessly on the cold hard floor.");
}




string Home()
{
    string tavernInncottage = Input("Do you make the 'tavern', 'cottage', or 'inn' your new home? ");
    while (tavernInncottage != "tavern" && tavernInncottage != "cottage" && tavernInncottage != "inn")
        tavernInncottage = Input("That won't work this time! Do you make the 'tavern', 'cottage', or 'inn' your new home? ");
    return tavernInncottage;
}
#pragma endregion




const char* introMessages[]{ "As you walk past the village tavern, a drunken ogre unfortunately \
mistakes you for the same villager that stole his favorite gold coin, \
and pulls out his rusty dagger to take it back.\n",
"After walking through the forest some more, you come across a \
clearing that gives you a clear view of the castle where Misty is being held. \
As you stop to drink from your canteen, a goblin jumps out of the bushes and \
steals your pack! Goblins were hit especially hard by the last war, but you \
also know that that doesn't excuse it from stealing your stuff.\n",
"The castle doesn't seem so far now, which gives you a glimpse of hope \
that Misty can be saved. That hope quickly comes crashing down when you see a horde of werewolves descend from the dark depths of the \
the tree line, and you sprint towards the castle in hopes of finding safety from the hungry monsters.\n",
"The sun starts to set, and you decide that you must find a nice place to hit the hay for the night. There are only two places available: \
a small and unassuming cave that offers shelter at the expense of not knowing whether or not an animal ALSO considers the cave its home, \
and an abandoned watchtower that accentuates the damaging effects of the past war.\n",
"Being alive for this long is very commendable, \
so you pat yourself on the back as you cross a boring old bridge. You actually notice how distinctly boring this bridge is, \
and voice your distaste of the structure. As you do this, a rumbling can be heard from under you, and as you recover from the friction, \
a gross troll appears in front of you, and you're able to notice that she did NOT approve of your comment, and lets you know just as much.\n",
"After going through a whopping SEVEN possibly deadly levels, you've finally reached the castle, well, correction- castle entrance. The gate is \
still intact, although the lever used to open the hefty doors is of course missing, so you begin to look around for a solution to this most \
coincidental of situations.\n",
"After your fight with the mutant, you realize how dire the situation is, and sit on a bench to think about your next move \
very carefully. However, you get distracted by a bussiness card that lands on your leg.\n",
"All this worry has got you very stressed out, so to alleviate that, you make a stop by the local library to pick up a good read.\n\
Inside the library, you find that only three books are available to be checked out. While grabbing the books, a letter falls out, which reads as follows:\n\
'In order to infiltrate Joshro's fortress, you must find the secret code inscribed within the following texts. Good luck, brave traveller!'\n\
Revived by this sudden revelation that could help you save Misty, you get comfortable and start sifting through the books to find what you need.\n",
"Finding the sewers was easy, but actually navigating through the dense masses of questionable matter proves difficult. \
As you see a shimmer of light at the end of the tunnel, something large and hairy brushes past your leg, and as you question what\
exactly that could've been, a freakishly large mutated rat as well as its two offsprings jump out of the water and try to lunge at you,\
but thankfully miss, and you retrieve your weapon from its holster and get ready to battle these rage-filled rodents." };
const char* outroMessages[]{ 
// 0-Ogre
"The ogre stumbles to the ground, but before you can search him \
for anything valuable, a guard approaches from the distance, and \
you pick up the pace to the village exit where you can continue your quest. ",
// 1-Goblin1
"The goblin collapses, and you take pity on the creature \
as you collect your items and continue towards the castle.",
// 2-Goblin2
"Both goblins collapse, and after questioning your own ethics you continue on your journey; new pet in hand.",
// 3-Troll
"The troll becomes motionless, and after you poke it with your weapon to see if she's still alive, \
gravity finally takes effect, and the troll falls over the side of the bridge into the murky depths below, creating such a splash that the water soaks your hair. \
You regain your composure, and proceed to start walking along the path again.",
// 4-Rats
"You slay the horrid creature, and continue walking towards the light." };


#pragma region Endings
const char* endingOneHappens = "After slaying the dragon, you return Misty to her respective kingdom, and she thanks you profusely. Before you fully become content with your life, you remember\n\
the promise you made to the goblin that you would come work for him to repay your debt (although you still scratch your head as to what exactly the *debt* is). Nevertheless, you are a man of your word,\n\
and make the long trek to the goblin's farm, and find that the farm life isn't so bad when the person you're doing the farming for is so small in stature that a farm to him is like a small garden to you.\n\
In addition to this, you actually find that the goblin, whose name is soon discovered to be Bilbo, is surprisingly very funny and witty, and you two get along splendidly. The farm life treats you very well,\n\
and you decide to take up farming even after your servitude to Bilbo was fulfilled, selling various vegetables to travellers going on journeys of their own...\n\
YOU GOT THE 'Man of your word' ENDING! (1 out of 4)";
const char* endingTwoHappens = "After defeating Joshro, you bring Misty back to her palace, where a celebration is held in your name for bringing back the kingdom's beloved princess. After recuperating yourself from the\n\
many nights of royal partying, you leave the kingdom in a great mood. While walking across an outrageously decrepit bridge, you remember Samantha and the offer she made to you. Seeing how disgusting some bridges can be\n\
without proper maintenance, you decide that bridge cleaning is the next endeavor you wish to undertake. Returning to Samantha's bridge, you see that she's already made several noticeable steps to\n\
make her bridge look more approachable, which gives you the final push you need to really take this bridge thing seriously. For several years, you and Samantha work tirelessly to keep the bridge spotless,\n\
and become great friends in the process. After Samantha leaves the bridge business to pursue a career in dramatic acting, you transform the bridge into a resting place for weary travellers to\n\
to safely take shelter at while they go fight dragons and save princesses of their own...\n\
YOU GOT THE 'Human troll' ENDING! (2 out of 4)";
const char* endingThreeHappens = "After putting an end to Joshro's reign of terror, you and Misty travel back to her kingdom, and you get there just in time to see her get married, and start to feel a little bit empty.\n\
This emptiness consumes you for the next couple of days while you head back to the cottage, and you struggle to put your finger on what exactly is bothering you so much. That is, until you and Olivia meet again.\n\
The reunion fills you with an overwhelming sense of bliss, and you feel that emptiness become replaced with a new type of emotion- love. In the space of 8 years, you and Olivia open up a tavern, and your relationship with her\n\
eventually shifts from a platonic one to one filled with golden memories and endless laughter, a sign of the increasingly apparent chemistry between you two. Then, on the tenth anniversary since you two met during your quest to\n\
defeat Joshro, you propose to Olivia, and she says yes with tears in her eyes as you both hug. Many years go by, and a family of three is formed, with the new addition to it being your son Matthew.\n\
One day, Matthew tells you privately that he wishes to go out and stop the tyrant serpent Tylo from taking over a kingdom several oceans away, but informs you that Olivia is aggressively against this.\n\
Remembering how you met her in the first place, you smile and tell him to leave in the middle of the night and 'follow your dreams', just as you did so long ago...\n\
YOU GOT THE 'Unwidowing the widow' ENDING! (3 out of 4)";
const char* endingFourHappens = "After stopping Joshro dead in his tracks (hah, get it?), you and Misty begin the long trek back to her kingdom. While on the trip, you discover that there's more to Misty than her intense beauty, and\n\
find that she's actually a really smart girl that aims to study international affairs at the college level. Her studious nature, among other cherished traits, makes you start to fall for her.\n\
The night before you're expected to reach her kingdom, you profess your love to her, and she confesses her love for you as well. The arranged marriage she was supposed to end the next day was changed to one that she had\n\
complete control over, and, as expected, she chooses to marry you. The wedding is magical for both of you, and, because of how marriage directly influences who becomes king and queen, you become the next king of her kingdom!\n\
The next decades become one of the most prosperous times in the kingdom, and the public regard you as one of the best kings they've ever had.\n\
Sitting one day in the royal court, a young but hardy man asks you for a small amount of money, and states that his intentions with the money will be to discover new lands and put an end to injustice all over the world.\n\
Your advisors laugh him off, but you retain your steady gaze and approve the man's request, basking in the nostalgia of a time when you knew a man that also wanted to do good...\n\
YOU GOT THE 'From rags to royalty' ENDING (4 out of 4)";
#pragma endregion





void Run() // Runs the game, this function is called once per playthrough.
{
#pragma region Setup
    string prompt;
    srand(static_cast<uint>(time(NULL))); // Initialize random seed.
    restart = false;
    player = Player(150);
    bool potionTroll = false;
    int morality = 0;
    vector<string> trackEndings{};
    currentStrings = vector<char>({ 's', 't', 'r', 'i', 'n', 'g' });
    emptyStr = "";
    bool endingOne = false;
    bool endingTwo = false;
    bool endingThree = false;
    bool endingFour = false;
    bool brutalEnding = true;
    bool endingChosen = false;
#pragma endregion
#pragma region Armory
    printf("---------------------------------------------------------------------------------------------------------------------------\n\
You are Max, a young man who takes on the brave quest of saving the\n\
beautiful princess Misty from the evil dragon Joshro. Your story begins at the village\n\
blacksmith, where you must decide what kind of weapon you will bring with you\n\
on your journey.\n");
    Sleep();
    WeaponSelect();
#pragma endregion
#pragma region Ogre
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    location = "village";
    player.currentDeathMessage = "Don't forget that if you dodge an attack that would stun you, you won't get stunned.\n";
    printf(introMessages[0]);
    string fightRun = Input("Do you want to 'fight' or 'run' away from the ogre? ");
    while (fightRun != "fight" && fightRun != "run")
        fightRun = Input("That won't work this time! Do you want to 'fight' or 'run' away from the ogre? ");
    if (fightRun == "fight")
    {
        FightSequence({ ogre }, false, { {} });
        if (restart)
            return;
    }
    else
    {
        printf("Before you can run, the ogre grabs your shirt and pulls you back, \
and asks in a booming but obviously slurred voice:'Bretton, do you have ma \
gold coin yet or not?'");
        string fightAvoid = Input("'1'. Lie and say its back at the inn and you'll get it as soon as he lets you go.\n\
'2'. Tell him to let you go or else he'll have to worry about more than just a stupid gold coin.\n\
(pick a number): ");

        while (fightAvoid != "1" && fightAvoid != "2")
            fightAvoid = Input("That won't work this time! PICK A NUMBER: ");
        if (player.weapon.name == "Ogre in a Bottle")
        {
            printf("The ogre then ignores what you say and calls you back saying incoherently that you smell suspicious, and you're forced to fight them.");
            FightSequence({ ogre }, false, { {} });
        }
        else if (fightAvoid == "1")
        {
            printf("The ogre seems to like that option, and lets you go as he\n\
meanders back to the tavern. You regain your composure and continue walking\n\
to the village exit.");
            StringWord(emptyStr);
            brutalEnding = false;
        }
        else
        {
            printf("The ogre becomes enraged and slams you on the ground, howling\n\
like a dog as he searches himself for the dagger he carries. You get back\n\
on your feet and pull out your weapon.");
            FightSequence({ ogre }, false, { {} });
            if (restart)
                return;
        }
    }
    printf(outroMessages[0]);
#pragma endregion
#pragma region Treasure
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    DeadlyTreasure();
    if (restart)
        return;
#pragma endregion
#pragma region Goblins
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    location = "forest";
    player.currentDeathMessage = "Don't forget that you can 'change' targets when there's more than 1 foe.";
    printf(introMessages[1]);
    string fightPersuade = Input("Do you want to 'fight' or 'persuade' the goblin? ");
    while (fightPersuade != "fight" && fightPersuade != "persuade")
        fightPersuade = Input("That won't work this time! Do you want to 'fight' or 'persuade' the goblin? ");
    if (fightPersuade == "fight")
    {
        printf("A Pet Slime also jumps out of the bushes to protect their owner!");
        FightSequence({ goblin, slime }, false, { {} });
        if (restart) return;
        printf(outroMessages[1]);
    }
    else if (fightPersuade == "persuade")
    {
        string parkour = Input("The goblin runs away, and you scramble after it. To catch up to the goblin, you can either 'slide' under a fallen log, or\n\
'vault' over a thorny bush. What do you do? ");
        while (parkour != "slide" && parkour != "vault")
            parkour = Input("That won't work this time! What do you do? 'slide' or 'vault'? ");
        if (parkour == "slide")
        {
            string persuade = Input("You successfully slide under the log, and find the goblin scurried up on a tree branch,\n\
just out of reach. What do you say to the scared creature?\n\
Oh yeah, and there's some weird slime looking at him.\n\
'1'. \"I won't hurt you I promise, just please give me back my stuff and I promise I'll make it up to you.\"\n\
'2'. Pet that slime that you hope might be his and say \"You know, I like slimes too.\"\n\
(pick a number): ");
            while (persuade != "1" && persuade != "2")
                persuade = Input("That won't work this time! '1' or '2' ");
            if (persuade == "1")
            {
                brutalEnding = false;
                printf("The goblin begrudgingly drops your items, but makes you promise that, under oath, you will come\n\
back after you finish your quest to come work for him to pay off your debt. You gather your pack and continue towards the castle.\n");
                StringWord(emptyStr);
                trackEndings.push_back("Do you want to repay your debt to the 'goblin'?");
                endingOne = true;
            }
            else
            {
                printf("The goblin becomes furious after you pet HIS pet slime, and swiftly slashes you with his claws.\n\
You're able to get up, but because of the surprise attack, you've lost valuable health.\n");
                player.maxHealth = player.maxHealth / 2;
                player.health = (int)ceilf(player.health / 2.0f);
                FightSequence({ goblin, slime }, false, { {"Pet Slime"} });
                if (restart) return;
                printf(outroMessages[1]);
                player.maxHealth = player.maxHealth * 2;
                player.health = player.health * 2;
                if (specialFightEnding)
                {
                    specialFightEnding = false;
                    printf("Oddly enough, you have now seemingly befriended the pet slime. The Pet Slime has replaced your weapon.\n");
                    Weapon oldWeapon = player.weapon;
                    Attack oldAttack = oldWeapon.attacks[0];
                    player.weapon = Weapon({ slimeHug, oldAttack }, "Pet Slime", 1.0);
                    printf("After taking the place as your weapon, the Slime Pet decides that it should eat your old %s.\n\
After doing this, your Slime Pet learns a new skill, '%s'.", oldWeapon.name.c_str(), oldAttack.name.c_str());
                    printf("And after seeing this beautiful sight 2 disgusted goblins jump out of the trees to take you on.\n");
                    location = "forest2";
                    player.currentDeathMessage = "Your Pet Slime eats you soon after.";
                    FightSequence({ goblin, goblin }, false, { {} });
                    if (restart) return;
                    printf(outroMessages[2]);
                }
            }
        }
        else
        {
            printf("You try to vault over the bush, but because you skipped leg day\n\
at the medieval gym, you fail and fall face first into some *very* sharp thorns");
            player.currentDeathMessage = "Maybe try making some different choices next time.";
            End();
            return;
        }
    }
#pragma endregion
#pragma region River
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    location = "river";
    printf(introMessages[2]);
    RiverEscape();
    if (restart) return;
#pragma endregion
#pragma region Cave and Watchtower
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    location = "cave/watchtower";
    printf(introMessages[3]);
    string caveWatchtower = Input("Do you take shelter in the 'cave' or the 'watchtower'? ");
    while (caveWatchtower != "cave" && caveWatchtower != "watchtower")
        caveWatchtower = Input("That won't work this time! Do you take shelter in the 'cave' or the 'watchtower'? ");
    if (caveWatchtower == "cave")
    {
        potionTroll = Cave();
        printf("You leave the cave, and trek on towards the increasingly visible castle.");
    }
    else
    {
        WatchTower();
        printf("You exit the watchtower, and trek on towards the increasingly visible castle.");
    }
#pragma endregion
#pragma region Troll
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    location = "old bridge";
    player.currentDeathMessage = "Don't forget that splash will weaken your attacks by a fixed amount, and that it doesn't effect status effects.";
    printf(introMessages[4]);
    printf("'I'm tired of you rude humans criticizing my bridge upkeep skills!:( Gah! All this yelling has got me thirsty,\n\
would you happen to have anything that could satiate my thirst?\n");
    if (potionTroll == true)
    {
        printf("You suddenly remember that you have that potion from the cave, as well as the oddly specific dream that turned out\n\
to manifest itself into reality... Anyway, you think about if you should give the troll the potion or keep it for yourself.\n");
        string bargainFight = Input("Do you 'give' the troll the potion or 'keep' it for yourself? ");
        while (bargainFight != "give" && bargainFight != "keep")
            bargainFight = Input("That won't work this time! Do you 'give' the troll the potion or 'keep' it for yourself? ");
        if (bargainFight == "give")
        {
            brutalEnding = false;
            printf("You quickly give the troll the potion, and she monstrously consumes its contents before proceeding to burp in your\n\
face with a satisfied grin on her face. 'WOW! I don't know what kind of stuff was in that, but I think I'm gonna take a nap for a little bit.\n\
DON'T GO ANYWHERE HUMAN, OR ELS-', is all the troll can say before the potion's effects fully take place, and she falls onto the bridge and starts snoring loudly.\n");
            StringWord(emptyStr);
            string stayGo = Input("Maybe it's because you felt bad for disrespecting the troll's bridge, or more likely because you're crazy, but you\n\
you hesitate before going and wonder whether or not you should 'stay' and see what the troll wants from you, or 'go' because the princess stuck in an \
evil dragon's castle is still there and, well, that was the whole reason you're out here in the first place... What do you do? ");
            while (stayGo != "stay" && stayGo != "go")
                stayGo = Input("That won't work this time! Do you 'stay' or 'go'? ");
            if (stayGo == "stay")
            {
                printf("You wait several hours (trolls are notorious for sleeping for long periods of time), and just as you're about to forget it,\n\
the troll wakes up and looks considerably more happy. 'Heya pal! Look I'm sorry 'bout all that yelling I did earlier, I just can't stand inconsiderate individuals,\n\
human, werewolf, goblin, what have you. So, as a token of my apology, I'll offer you a job that'll consist of helping me keep this place spick-and-span...\n\
not at all because I get sorta lonely out here and could use another person to talk to... So whaddaya say?' Well, not one to turn down an offer as rare as this, you\n\
accept the troll's offer, but remind her that you still have a girl to save, and she nods her head understandably.\n\
You find out the troll's name is Samantha, and wish her well as you finally cross the bridge to continue your quest.\n");
                trackEndings.push_back("Do you want to help keep the bridge clean with 'Samantha' the troll?");
                endingTwo = true;
            }
            else
                printf("You decide that the troll was probably just saying all of that due to her inebriated state, and cross the bridge\n\
quietly to continue your journey.\n");
        }
        else
        {
            printf("You decide to be greedy and admittedly sort of dumb so as not to rid yourself of extra weight, and it comes back to bite you. 'WOW! So first\n\
you disrespect my bridge, and then you don't even give me something for my dehydration! This won't do! I'm going to have to teach you a lesson in manners!\n\
Quickly, you ask if there's a riddle you can try to solve in order to avoid a fight, but the troll's mind is already made up, and in fact this seems to\n\
make her even more angry, which doesn't help things in the slightest. You ready your weapon and prepare to fight the burly creature.\n");
            FightSequence({ troll }, false, { {} });
            if (restart) return;
            printf(outroMessages[3]);
            printf("P.S., while fighting the troll, the potion broke in your bag, so it's of no use to you now, and you think for a second how much \
better that situation could've turned out if you would've given the troll the potion in the first place, but ah whatever it's just a game, morals don't matter.\n");
        }
    }
    else
    {
        printf("Because you have nothing to give the troll, like a potion most likely found in a cave that could easily keep the upcoming situation peaceful,\n\
you blank out, and the troll notices this. 'WOW! So first you disrespect my bridge, and then you don't even give me something for my dehydration!\n\
This won't do! I'm going to have to teach you a lesson in manners!\
Quickly, you ask if there's a riddle you can try to solve in order to avoid a fight, but the troll's mind is already made up, and in fact this seems to\n\
make her even more angry, which doesn't help things in the slightest. You ready your weapon and prepare to fight the burly creature.\n");
        FightSequence({ troll }, false, { {} });
        if (restart) return;
        printf(outroMessages[3]);
    }
    player.weapon.LearnAttack(splash);
    printf("Before leaving, you let your %s soak in the water, and it seems to absorb it.\n\
Your %s has learned 'splash'", player.weapon.name.c_str(), player.weapon.name.c_str());
#pragma endregion
#pragma region Castle entrance
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    location = "castle entrance";
    printf(introMessages[5]);
    CastleEntrance();
#pragma endregion
#pragma region Village
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    printf("Inside the castle walls, you become flabbergasted as you realize that the castle was not what you thought it was, and,\n\
instead of being one small keep, the castle actually houses a considerably sized village and population of people.\n\
\n\
Overwhelmed by this new revelation, you make your way through the solemn streets, and note how subdued the locals seem.\n\
Luckily, you find a map taped to a community board that gives the location of 3 places you deem could function as temporary homes:\n\
an inn, a two-person cottage in need of one more roommate, and a tavern with rooms to rent. Fortunately, your pickpocketing skills have empowered\n\
you with the ability to easily pay for all three.");
    string home = Home();
    printf("You choose to make the %s your new home.", home.c_str());
    if (home == "cottage")
    {
        printf("Knocking on the door of the cottage, you take a minute to observe that everyone has been avoiding you with intense dedication,\n\
which deeply concerns you. The door then opens, and for the first time since you step foot in this strange place, a friendly face seems eager to see you.\n\
'Hello there, sir', a woman with a strikingly beautiful appearance says earnestly. You both exchange pleasantries, and the lady, whose name is discovered to be\n\
Olivia, shows you to your room, and you get settled into your new living situation. While having dinner, you find out that Olivia is a widow, whose husband was killed\n\
in the past war. You give her your condolences, and the rest of the dinner is spent in silence. But, while getting ready for bed, you find\n\
a letter slipped under the carpet, which reveals that Olivia also aims to slay the dragon. Hearing some noise behind you, you see Olivia standing in the\n\
doorway, dagger in hand, and asks in a serious voice:'Well, now that you know, are you with me, or against me?'\n\
Of course, you say you're with her, and then proceed to tell her all about why you've come to the castle anyways.\n\
With Olivia vectorening intently, you curiously ask her why everyone acts so strange around town to newcomers, and then Olivia responds eagerly:'Joshro's\n\
goons have threatened the villagers with execution so much that no one dares to step out of line, even to help poor but handsome souls like yourself.'\n\
Taking in this information, as well as Olivia's flirting, you realize that you're going to have to be much more careful so as not to attract any\n\
unwanted attention. The conversation ends, and you go to sleep swiftly.\n\
Before you leave the house, Olivia rushes to you and gives you a hug, and through stifled sniffles, tells you to stop by her house again after you complete your mission,\n\
promising you a place to lay low after the dragon is slayed. You accept the offer, and Olivia nods before letting you know that she'll be out of town for a few days to visit her husband's funeral.");
        trackEndings.push_back("Do you want to go back to the cottage and live with 'Olivia'?");
        endingThree = true;
    }
    else if (home == "tavern")
        printf("Entering the tavern, you search out the bartender and negotiate the cost it'll take to rent out a spare room. A deal is settled, and the bartender shows you around the place before directing you to your room.\n\
You get settled in for the night, eating some food that you bought earlier from the bartender and just thinking about everything that's happened recently.\n\
In one of the drawers, you find a damaged note that warns its readers to be weary of Joshro's henchmen. You dismiss this, and get in bed before falling asleep easily.");
    else // Inn
        printf("Once inside the inn, you eat a hearty meal in the lobby before purchasing a room. As you walk past the many rooms, an old and disheveled man grabs you and warns you to stay weary of the \
'Dragon's Devils'. You push the man off you and hurry to your room. The man's words still linger in your mind, but you shut them off and go to sleep begrudgingly.");

    player.health = player.maxHealth;
#pragma endregion
#pragma region Mutant
    Sleep();
    printf("\n\n++++++++++++++++\n\n");

    printf("After leaving the %s in the morning, you turn the corner, but come face to face with a hideously disfigured mutant that shoves you to the ground before\n\
exclaiming:'LeAvE, RiGhT nOw.' Obviously, you don't move, and the mutant spits on the ground before getting into a battle stance, and as such you do the same.\n", home.c_str());
    location = "random street";
    player.currentDeathMessage = "The mutants heavy punch can stun you if you're not careful, it may be best to dodge it.";
    FightSequence({ mutant }, false, { {} });
    if (restart) return;
    string spareKill = Input("The mutant crashes to the ground, pleading with you to spare it. Do you 'spare' or 'kill' the mutant? ");
    while (spareKill != "spare" && spareKill != "kill")
        spareKill = Input("That won't work this time! Do you 'spare' or 'kill' the mutant? ");
    if (spareKill == "spare")
    {
        brutalEnding = false;
        printf("You let the mutant go, and it shambles off into the early morning darkness.\n\
Then your %s learns 'spare'.\n", player.weapon.name.c_str());
        player.weapon.LearnAttack(spare);
        StringWord(emptyStr);
    }
    else // Kill
        printf("You end the mutant.\n\
The mutant shrivels up, and you jump back in horror as the supernatural occurrence unfolds in front of your eyes.\n\
Death is something you've sort of gotten used to after all of the previous fights you've been in,\n\
but this definitely takes the cake. You dust yourself off, and resume walking around the town in search of answers.\n");
#pragma endregion
#pragma region Magic Lesson
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    printf(introMessages[6]);
    printf("Rely cul magec lesuhns oll dey\nyul no wer to find\n\
");
    string yesMagic = Input("Do you want to join the 'magec lesuhns'? 'yes' or 'no' ");
    while (yesMagic != "yes" && yesMagic != "no")
        yesMagic = Input("That won't work this time Do you want to join the 'magic lesuhns'? 'yes' or 'no' ");

    if (yesMagic == "no")
        printf("You decide against wasting your time with probably scam magic, and return to your other worries.\n");
    else
    {
        printf("Upon deciding that you will go to the lesson you stand up from the bench and see a line of frogs going through an alleyway.\n\
The frogs continue leading you until they stop in front of a small oak door\n\
You continue through the door and are suprised to see that the room is small and nearly vacant, outside of more frogs and a message.\n\
'Wee froogs cen tach yu 1 of 2 difrent spols, 'stengthen' end 'hoalify',\n\
howevr bot o tem no only affact u, but also yur foes.\n\
What is your choice traveller?\n");
        string magicChoice = Input("You assume that they meant 'strengthen' or 'healify', but which one do you want? ");
        while (magicChoice != "strengthen" && magicChoice != "healify")
            magicChoice = Input("They only allow 'strengthen' and 'healify', which one? ");

        if (magicChoice == "strengthen")
        {
            printf("A red frog walks through an opening in the wall and jumps on your %s.\n\
The frog proceeds to dissolve into your %s.\n\
Your %s learns 'strengthen'.\n", player.weapon.name.c_str(), player.weapon.name.c_str(), player.weapon.name.c_str());
            player.weapon.LearnAttack(strengthen);
        }
        else
        {
            printf("A green frog walks through an opening in the wall and jumps on your %s.\n\
The frog proceeds to dissolve into your %s.\n\
Your %s learns 'healify'.\n", player.weapon.name.c_str(), player.weapon.name.c_str(), player.weapon.name.c_str());
            player.weapon.LearnAttack(healify);
        }

        printf("Upon looking back at the room, you realize that it is now completely barren.\n\
Bewildered, you leave and continue about your day.\n");
    }
#pragma endregion
#pragma region Code Find
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    printf(introMessages[7]);
    CodeFind(); // Lol, such a tiny region.
#pragma endregion
#pragma region Rats
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    location = "sewers";
    printf(introMessages[8]);
    player.currentDeathMessage = "Maybe you should try to come here with more health, or try to outrun them.";
    printf("Before beginning your attack on the rat, you remember that you used to take \
medieval track and field at your former academy, and ponder over whether or not you should see if your skills are still in tip-top shape.\n\
The little voice in your head warns you, though, that if your health has not been increased\
past the default 100 value, you might not be able to outrun the rambunctious rodent after all.\n");
    string outrunFight = Input("Do you 'fight' or 'outrun' the rat? ");
    while (outrunFight != "fight" && outrunFight != "outrun")
        outrunFight = Input("That won't work this time! Do you 'fight' or 'outrun' the rat? ");
    if (outrunFight == "fight")
    {
        FightSequence({ rat, babyRat, babyRat }, false, { { "Baby Rat" }, { "Baby Rat", "Baby Rat" } });
        if (restart) return;
        if (specialFightEnding)
        {
            prompt = Input("After you defeat the parent rat, the " + string(specialFightEndingMonsters.size() > 1 ? "rats" : "rat") + " do you want to chase after them ? ('yes' or 'no') ");
            while (prompt != "yes" && prompt != "no")
                prompt = Input("That won't work this time. Do you want to chase? ('yes' or 'no') ");
            if (prompt == "yes")
            {
                vector<Entity> survivorRats = vector<Entity>(specialFightEndingMonsters.size());
                for (int i = 0; i < survivorRats.size(); i++)
                    survivorRats[i] = *specialFightEndingMonsters[i];
                FightSequence(survivorRats, false, { {} });
                if (restart) return;
            }
            else
            {
                brutalEnding = false;
                printf("You decide to spare the %s.", specialFightEndingMonsters.size() > 1 ? "rats" : "rat");
                if (!player.weapon.KnowsAttack("spare"))
                {
                    printf("Then your %s learns 'spare'.", player.weapon.name.c_str());
                    player.weapon.LearnAttack(spare);
                }
            }
        }
        printf(outroMessages[4]);
    }
    else
    {
        printf("You decide to try your luck and athletic skills by ceasing to fight the rat and instead violently thrash through the water\n\
to attempt to reach a safe distance.");
        if (player.health > 100)
        {
            printf("You end up creating such a large distance between you and the rodent that it eventually just gives up and waddles away in the other direction.\n");
            StringWord(emptyStr);
            brutalEnding = false;
        }
        else if (player.health <= 50)
        {
            if (RandRange(0, 1) == 0)
            {
                printf("Despite your determination in trying to outrun the creature, it catches up to you and drags you down to the murky depths...\n\
and you never resurface.");
                player.currentDeathMessage = "It feels like you could have made that if only you were luckier or had more health.";
                End();
                return;
            }
            else
            {
                printf("In a surprising turn of events, you manage to wade quickly enough away from the rat that it decides that you're not worth the trouble,\n\
and waddles away in the other direction. In the process though you did cut your knee on a rock.\n");
                StringWord(emptyStr);
                brutalEnding = false;
                if (player.health > 25)
                    player.health -= 10;
                else
                    player.health = std::max(1, std::min(10, player.health - 10));
            }
        }
        else // 50 < health <= 100
        {
            if (RandRange(0, 1) == 0)
            {
                printf("Despite your determination in trying to outrun the creature.\n\
However, it doesn't fully defeat you when it catches up with you, and as such you are forced to fight it.");
                player.health -= 50;
                FightSequence({ rat, babyRat, babyRat }, false, { {"Baby Rat"}, {"Baby Rat", "Baby Rat"} });
                if (restart) return;
                if (specialFightEnding)
                {
                    prompt = Input("After you defeat the parent rat, the " + string(specialFightEndingMonsters.size() > 1 ? "rats" : "rat") + " do you want to chase after them? ('yes' or 'no') ");
                    while (prompt != "yes" && prompt != "no")
                        prompt = Input("That won't work this time. Do you want to chase after them? ('yes' or 'no') ");
                    if (prompt == "yes")
                    {
                        vector<Entity> survivorRats = vector<Entity>(specialFightEndingMonsters.size());
                        for (int i = 0; i < survivorRats.size(); i++)
                            survivorRats[i] = *specialFightEndingMonsters[i];
                        FightSequence(survivorRats, false, { {} });
                    }
                    else
                    {
                        brutalEnding = false;
                        printf("You decide to spare the %s.\n", specialFightEndingMonsters.size() > 1 ? "rats" : "rat");
                        if (!player.weapon.KnowsAttack("spare"))
                        {
                            printf("Then your %s learns 'spare'.\n", player.weapon.name.c_str());
                            player.weapon.LearnAttack(spare);
                        }
                        printf(outroMessages[4]);
                    }
                }
            }
            else
            {
                printf("In a surprising turn of events, you manage to wade quickly enough away from the rat that it decides that you're not worth the trouble,\n\
and waddles away in the other direction. In the process though your knee got cut by a rock.\n");
                player.health -= 25;
            }
        }
    }
#pragma endregion
#pragma region Number puzzle
    Sleep();
    printf("\n\n++++++++++++++++\n\n\
You eventually reach the end of the tunnel and climb up the ominous ladder to find a compound full of mutants, specifically ones that look\n\
much more deadly and aggressive than the one you encountered earlier. You'll need to get a disguise, and in order to do that, you must gain access to the\n\
armory. The only issue with that is that a key is required to enter the building, and you don't have one.\n\
You faintly remember that many keys are commonly kept in the main watchtower, so you sneak over there and find that the key you need is locked inside an\n\
intricately designed mathematical padlock that protects a metal box. You read a note that gives the instructions needed to bypass this, and begin working.\n");
    RandomNumtosolve();
    printf("You complete the puzzle, and unlock the box to find the key you need. You exit the main watchtower without arousing suspicion from the nearby mutants,\n\
and continue on your way to the armory.\n");
#pragma endregion
#pragma region Unloyal guard
    Sleep();
    printf("\n\n++++++++++++++++\n\n\
Once reaching the armory, you insert the key and quietly let yourself inside the armory. The armor present will do more than enough to hide your non-mutant\n\
appearance, and, in addition to that, offers more protection.");
    player.maxHealth += 50;
    printf("Approaching the keep, you see a human guard ahead that won't move no matter what distractions you use to guide him away. After some impatient waiting, you conclude\n\
that the only way to enter the keep is through a direct encounter with the guard. A fight is inevitable, but the only question now is *how* the interaction with the guard will end...");
    player.currentDeathMessage = "It seems like he doesn't want to fight... maybe there's a way to convince him to stop.";
    FightSequence({ guard }, true, { {} });
    if (restart) return;
    if (specialFightEnding)
    {
        brutalEnding = false;
        printf("The guard, whose name you soon find out is Bruce, explains to you that he doesn't even really like Joshro or his ideals,\n\
but only works for him because the pay is good and he has mouths to feed. This seems like a fair reason to work for a ruthless overlord, so you empathize with him\n\
and offer to give him the riches found inside Joshro's treasury in exchange for him helping you take on Joshro.\n\n\
After some thinking, Bruce declines your offer, and says that he is not the fighter he once was, but because he likes you, he will give you an ancient rejuvination potion.\n\
You nod in agreement, drink the potion, and enter the keep, where the evil Joshro and precious Misty are located...\n");
        player.maxHealth += 50;
        player.health += 50;
        StringWord(emptyStr);
    }
#pragma endregion
#pragma region Pig Latin puzzle
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    printf("Once inside the keep, you see that the building is separated into three chambers, with you currently being in the first.\n\
To enter the next room you first need to speak to the receptionist, who happens to be a pig. He appropriately talks to you in pig latin,\n\
but because pig latin is not really in fashion anymore, you struggle to decipher what he's saying, and must employ the use of writing to help\n\
you conversate better.");
    PigLanguage();
#pragma endregion
#pragma region Wizard
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    printf("Entering the second to last chamber, you find a sorcerer that offers you a boggling 100 value health potion! The only catch, he tells you, is that you must have no blood on\n\
your hands from the time you picked your weapon to the present moment. Depending on how you dealt with the various adversaries throughout the course of the journey, this will either be\n\
really helpful or a waste of potential...");
    printf("The sorcerer rubs his crystal ball, and reveals that......\n\n");
    if (emptyStr.size() == 6)
    {
        printf("You were a pacifist! The sorcerer smiles and delights in the fact that you appreciate life, no matter if it's a troll, a human, or even a measly rat.\n\
The sorcerer points out that throughout your successful attempts at being peaceful, a letter was added to a growing vector of seemingly random letters, but in actuality,\n\
they add up to be the scrambled version of a six letter word. The only hint the sorcerer gives you is that the word is a part of what makes up cloth shirts, and also warns you that you\n\
only have three chances to guess the word...\n\
you crack your knuckles and massage your forehead in preparation for the mind-numbing task ahead...\n\n");
        for (int i = 0;; i++)
        {
            if (i < 4)
            {
                printf("******\n%s\n******\n", emptyStr.c_str());
                string askString = Input(i < 3 ? "What is the word unscrambled? " : "Last chance! What is the word unscrambled? ");
                if (askString == "string")
                {
                    printf("You got it! The sorcerer smiles with admiration as he hands you the obviously enlarged and luxurious potion full of fizzling bubbles.\n\
You drink it, and bask in the glory that is being a nonviolent person before heading to the door separating you from the girl you came to save and the unjust creature you must stop...\n");
                    player.maxHealth += 100;
                    player.health = player.maxHealth;
                    break;
                }
            }
            else
            {
                printf("You didn't guess the word, and sulk about it before heading to the door separating you from the girl you came to save and the tyrannical creature you must defeat...\n");
                break;
            }
        }

    }
    else if (!brutalEnding)
    {
        printf("You weren't a pacifist! The sorcerer sighs, and because you're mad at the sorcerer for not giving you a chance, you try to attack him. The sorcerer teleports away just in time, and you hit the wall\n\
with your fist in anger. But before leaving the sorcerer comes back and says that he dislikes you far less than Joshro, and that he will give you a fair shot against him. You are healed back to full;\n\
you then head to the door separating you from Misty and Joshro...");
        player.health = player.maxHealth;
    }
    else
    {
        printf("The sorcerer spits at your feet and teleports away, and you confusedly walk onwards to the door containing Joshro.");
    }
#pragma endregion
#pragma region Joshros Throne
    Sleep();
    printf("\n\n++++++++++++++++\n\n\n\n\n");
    printf("You take one last deep breath before preparing for one last fight.\n\n");
    Sleep();
    Sleep();
    player.currentDeathMessage = "You were so close, just keep trying!";
    Entity joshroHeadSummoned = joshroHead;
    joshroHeadSummoned.summoned = true;
    FightSequence({ joshrosBody, joshroHeadSummoned, joshroHeadSummoned }, false, { {} });
    if (restart) return;

    if (specialFightEnding)
        printf("And all of Joshro's heads fall to the floor.");

    if (brutalEnding)
    {
        printf("After defeating the dragon, you get ready to walk away from it's body,\n\
but unexpectadly one of the heads bites at you, and even weirder the head then seems combine with your %s.\n\
Your %s has learned 'ultra fire breath'.\n", player.weapon.name.c_str(), player.weapon.name.c_str());
        player.weapon.LearnAttack(ultraFireBreath);
    }

    trackEndings.push_back("Do you want to stick with 'Misty'?");
    endingFour = true;
#pragma endregion
#pragma region Endings
    Sleep();
    printf("\n\n++++++++++++++++\n\n");
    for (string ending : trackEndings)
    {
        printf((ending + "\n").c_str());
    }
    endingChosen = false;
    int confusionCount = 0;
    while (!endingChosen && confusionCount < 10)
    {
        string chooseEnding = Input("What do you do (Type the name of the person)? ");
        std::transform(chooseEnding.begin(), chooseEnding.end(), chooseEnding.begin(),
            [](unsigned char c) { return std::tolower(c); });
        if (brutalEnding)
        {
            printf("=]");
            confusionCount++;
            Sleep();
        }
        else if (chooseEnding == "goblin" && endingOne)
        {
            printf("\n");
            printf(endingOneHappens);
            printf("\n");
            endingChosen = true;
        }
        else if (chooseEnding == "samantha" && endingTwo)
        {
            printf("\n");
            printf(endingTwoHappens);
            printf("\n");
            endingChosen = true;
        }
        else if (chooseEnding == "olivia" && endingThree)
        {
            printf("\n");
            printf(endingThreeHappens);
            printf("\n");
            endingChosen = true;
        }
        else if (chooseEnding == "misty" && endingFour)
        {
            printf("\n");
            printf(endingFourHappens);
            printf("\n");
            endingChosen = true;
        }
        Sleep();
        printf("\n\n++++++++++++++++\n\n");
    }
#pragma endregion
#pragma region Credits++
    string specialThanksText = "Would you like to read the special thanks note? it's totally optional :)  'yes' or 'no' ";
    if (brutalEnding)
        specialThanksText = "Would you like to read the special special thanks note =] 'yes or 'no' ";
    string specialThanks = Input(specialThanksText);
    while (specialThanks != "yes" && specialThanks != "no")
        specialThanks = Input("That won't work this time! Do you want to read the note ('yes' or 'no')? ");
    if (!brutalEnding)
    {
        if (specialThanks == "yes")
        {
            printf("This game took me(Matthew) a little bit under 2 months to make, and it was definitely a rollercoaster\n\
of hating programming and loving it, but I'd just like to say a couple quick things about some of the people who helped/supported me through\n\
the development process...\n");
            Sleep();
            printf("The first person I'd like to thank is God for allowing me to be here and have the experience to be able to make such a cool game, as well as allowing me to meet wonderful people\n\
like Jasun, Jordan(That's me, the other dev!), Corbin, and other people I'd consider friends:)\n");
            Sleep();
            printf("The next person is, of course, Jasun, for two very good reasons. First and foremost, without Jasun pushing me to constantly upgrade and continue learning Python as well as\n\
making the game itself, this game would've never been completed. Him helping me get through tricky lines of code is the second reason, as many of the things you experience throughout the game,\n\
such as the pig latin challenge and the normal fight scenes, wouldn't have functioned properly or at all without his advice. All in all, I'd like to thank Jasun for allowing me to do a project that I\n\
genuinely enjoyed working on (for most of the time at least), as well as helping me explore programming in general, as it is something I definitely want to pursue as a future career.\n");
            Sleep();
            printf("\n");
            printf("For the next person, I'm actually going to thank two people- those two people being Jordan and Corbin. When Jasun was busy helping other people out or was stumped about something\n\
concerning my code, Jordan and Corbin, without hesitation, always came to my aid to help me solve whatever issue was plagueing my code. Without them and their\n\
Albert Einstein-like brains, a substantial portion of this game would not be possible:)\n");
            Sleep();
            printf("\n");
            printf("Ok because there are a bunch of other people on my vector that deserve paragraphs of their own, but also because I don't want this game to be like a million lines of code, I'm going to sum up the other people very quickly:\n");
            Sleep();
            printf("MICHELLE, for being supportive of my interest in coding even through tough times:)\n");
            Sleep();
            printf("~~~~~~~~~~~~~~~~\n");
            printf("LUKE, for showing a genuine interest in seeing my game be completed and even asking questions regarding the nerdy mechanics of what certain stuff did inside the code:)\n");
            Sleep();
            printf("~~~~~~~~~~~~~~~~\n");
            printf("VICTORY, for being the person I could have a good laugh with when programming annoyed me and I needed a break:)\n");
            Sleep();
            printf("~~~~~~~~~~~~~~~~\n");
            printf("DANNY, for being interested in playing my game once it was finished, as well as making ultimate frisbee be super fun and rewarding after doing a bunch of typing the Wednesday before in STEAM Club:)\n");
            Sleep();
            printf("~~~~~~~~~~~~~~~~\n");
            printf("DOUG, CAITLIN, BENNY, and JEFF, for letting me leave their classes early so I could work on my game:)\n");
            Sleep();
            printf("~~~~~~~~~~~~~~~~\n");
            printf("MORGAN, ELIZABETH, LUKAS, and GENEVIEVE, for being pretty cool people and keeping my mental state at a good place while\n\
personal issues were taking place that could've definitely sunk my drive to finish making the game,\n\
as well as showing interest in wanting to play my game once it was finished:)\n");
            Sleep();
            printf("~~~~~~~~~~~~~~~~\n");
            printf("JASUN, LUKE, CHARLOTTE, NOX, JORDAN(I'm back again =]), CORBIN, MORGAN, SHEPARD, BENNY, LUKAS, and many others for playtesting the game!:)\n");
            Sleep();
            printf("~~~~~~~~~~~~~~~~\n");
            printf("And lastly, YOU, the player, whoever you are, for beating my broken but meaningful mess of a game!:)  <- Don't worry Matthew, it's only gotten messier as I've picked it up. =]\n");
            Sleep();
            printf("\n\n++++++++++++++++\n\n");
            printf("If you're curious about what the '(NUMBER out of 4)' means next to the name of the ending you got, try playing the game again and find out what would happen if you did things differently!:)\n");
        }
        else
        {
            Sleep();
            printf("++++++++++++++++");
            printf("Thanks for playing our game! If you're curious about what the '(NUMBER out of 4)' means next to the name of the ending you got, try playing the game again and find out what would happen if you did things differently!:)\n");
        }
    }
    else if (specialThanks == "yes")
        printf("Thanks so so much for playing, this mod was of course made by me, Jordan Baumann in the year 2022.\n\
The story behind why this was made is sort of odd, I was thinking about Matthew's game, the unmodded version, and I thought that it'd be nice if there was more in-depth battle mechanics.\n\
I of course didn't convince Matthew to do so, but I did come up with the idea of making a mod for that, so I did, and here we are now.\n\
This mod took a decent while to make, and on many occassions I found myself facing the fact that writing is not my strongsuit.\n\
I worked past this though, and ended up with a few extra jokes a things here and there to spice up the decently mundaine experiece of the game.\n\
If you're wanting to know a bit about me and Matthew nowadays, we're actually working together in addition to Jasun Burdick and Corbin Roberts on a new game.\n\
Isle of Hamsters is a exploration tower defense RTS with factory building mechanics set on an alien planet where you command giant hamsters.\n\
So yeah, I'm glad that you read this, but if you're sitting there thinking to yourself... hmmm... I wonder if the 'sequel' to this game will ever come, \
worry not, for it will... just give me a bit, game dev's hard enough with 1 game, even more so 2, but I've already started to think of new features. \
You can expect to see many new features in the sequel including but not limited to:\n\
    More complicated status effects\n\
    Multiple units working for the player\n\
    Bigger fights\n\
    More emphasis on reproducing enemies(not just the final boss LOL)\n\
    Better balance on weapons\n\
    New and interesting genres\n\
    Temporary summons that exist for single battles that aren't as janky as the heads LOL\n\
    More complicated movement, so not just the lever 'puzzle'\n\
    And much much more!!!\n\
    Oh yeah, and real time gameplay =] It's still a text adventure though, don't worry.\n\
... Anyways, I can't wait to see what you all think of this stuff! And if you're lucky, maybe I'll even put these games on Steam!!!\n\
So... uh... Ciao!");
    else
        printf("The fun's not quite over yet friends, just wait... =] =] =] =] =]\n\tSincerely, Jordan");


    if (!brutalEnding)
    {
        printf("\n\n");
        bool randomBool = bool(RandRange(0, 1));
        if (randomBool && !player.weapon.KnowsAttack("club bash"))
            printf("Hiya! It's Jordan! I'm the dev behind this 'mod',\n\
I just came by to tell you that I heard a rummor that the blacksmith had an 'ogre in a bottle' for sale, hmm, I wonder what that meant...\n");
        else if (!player.weapon.KnowsAttack("slime hug"))
            printf("Hiya! It's Jordan! I'm the dev behind this 'mod',\n\
I just came by to tell you that I heard a rummor that the Pet Slime may not be as loyal as you'd thing, hmm, I wonder what that meant...\n");
        else
            printf("Hiya! It's Jordan! I'm the dev behind this 'mod',\n\
I just came by to tell you that I heard a rumor that some sorcerer can get far more mad then he normally does... hmm... I wonder what that meant...\n");
        Sleep();
    }
#pragma endregion
#pragma region Wizard fight
    if (brutalEnding)
    {
        printf("\n\n\
You upon defeating the dragon and harnessing his powers for yourself, you get ubrubtly teleported to a mysterious and colorful room.\n\
'You think that I'll let my kingdom decend into tyranny oncemore?' The wizard says from afar, I'll be back\n\n\
And just like that you were back in the castle, as if nothing had happened.\n");
    }
#pragma endregion
#pragma region New run
    printf("\n\n");
    prompt = Input("Do you want to play again? ('yes' or 'no') ");
    while (prompt != "yes" && prompt != "no")
        prompt = Input("'yes' or 'no'");
    restart = prompt == "yes";
#pragma endregion
}





int main()
{
    Term::Terminal term(false);
    Term::terminal_title("M");
    FindSettings();
    while (restart)
        Run();
}