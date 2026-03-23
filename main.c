/* COP 3502C PA2
This program is written by: Richard Magiday */
/******************************************************************************
Richard Magiday
cop3502c_cmb_26
01/14/26
problem: CS1 PA2

cd PA2
gcc main.c
Get-Content in1.txt | .\a.exe > out_test1.txt
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SCORES 5 // size of a cat scores array representing 5 traits
#define MAX_STR 25   // maximum name length of a cat
const float POSITION_BONUS[10] = {3.0, 5.0, 4.0, 6.0, 7.0, 2.0, 8.0, 1.0, 9.0, 1.5};
// bonus percentages that apply to each positionCat *cats;

typedef struct Cat
{
    char *name;            // dynamaxally allocated cat name
    char *breed;           // dynamaclly allocated breed
    int score[MAX_SCORES]; // array of size 5 representing trait scores
    int baseScore;         // sum of scores array
} Cat;

typedef struct Rivals
{
    Cat *cat1; // pointer to cat1
    Cat *cat2; // pointer to cat2
} Rivals;

Cat *cats; // for array of cats
Rivals *rivals;
float bestPermScore;
int **tracker;

// function prototypes
void createCats(int numTeams, int numCatsPer, int *totalCatsOut);
void createRivals(int r, int totalCats);

// void printCats(Cat **cats, int totalCats);
// void printRivals(Rivals *rivals, int r);

void printFinal(int numTeams, int numCatsPer, int r);
void permuteUsed(int k, int totalCats, int *perm, int *used, int numTeams, int numCatsPer, int r);

float scoreTeam(Cat **team, int teamSize, int r);
float scorePermutation(int *perm, int numTeams, int numCatsPer, int r);

int countHighPerformersTraits(Cat **team, int teamSize, int catIdx, int traitIdx);
int synergyBonusApplies(Cat **team, int teamSize, int catIdx);
int rivalPenaltyApplies(Cat **team, int teamSize, int idx, int r);

int areRivals(Cat *a, Cat *b, int r)
{
    for (int i = 0; i < r; i++)
    {
        if ((rivals[i].cat1 == a && rivals[i].cat2 == b) || (rivals[i].cat1 == b && rivals[i].cat2 == a)) // check rivals
            return 1;
    }
    return 0;
}

int allBreedsDifferent(Cat **team, int teamSize)
{
    for (int i = 0; i < teamSize; i++)
    {
        for (int j = i + 1; j < teamSize; j++)
        {
            if (strcmp(team[i]->breed, team[j]->breed) == 0) // comp breeds
                return 0;
        }
    }
    return 1;
}

int countHighPerformersTraits(Cat **team, int teamSize, int catIdx, int traitIdx)
{
    if (catIdx == teamSize)
        return 0;

    if (traitIdx == MAX_SCORES)
        return countHighPerformersTraits(team, teamSize, catIdx + 1, 0);

    int add = (team[catIdx]->score[traitIdx] >= 90) ? 1 : 0;
    return add + countHighPerformersTraits(team, teamSize, catIdx, traitIdx + 1);
}

int synergyBonusApplies(Cat **team, int teamSize, int catIdx)
{
    if (catIdx == teamSize)
        return 1; // base case

    int ok = 0;
    for (int i = 0; i < MAX_SCORES; i++)
    {
        if (team[catIdx]->score[i] >= 85)
        {
            ok = 1;
            break;
        }
    }
    if (!ok)
        return 0;

    return synergyBonusApplies(team, teamSize, catIdx + 1);
}

int rivalPenaltyApplies(Cat **team, int teamSize, int idx, int r)
{
    if (idx >= teamSize - 1) // base case
        return 0;

    int add = areRivals(team[idx], team[idx + 1], r) ? 1 : 0;     // check if rivails
    return add + rivalPenaltyApplies(team, teamSize, idx + 1, r); // return rivails debuf
}

float scoreTeam(Cat **team, int teamSize, int r)
{
    float sumBase = 0.0;
    for (int i = 0; i < teamSize; i++) // add all base scores
    {
        sumBase += team[i]->baseScore;
    }

    float score = sumBase / (teamSize * 1.0); // make sure no int division

    int hiTraits = countHighPerformersTraits(team, teamSize, 0, 0); // get hitraits scores
    score += 5.0f * (hiTraits * 1.0);

    if (synergyBonusApplies(team, teamSize, 0)) // get synergy bonus
        score += 30.0;

    if (allBreedsDifferent(team, teamSize)) // get breads bonus
        score += 10.0;
    else
        score -= 15.0;

    for (int p = 0; p < teamSize; p++)
    {
        score += ((team[p]->baseScore) * 1.0) * (POSITION_BONUS[p] / 100.0); // add position bonuses
    }

    int adjRivals = rivalPenaltyApplies(team, teamSize, 0, r); // get rivals penilty
    score -= 25.0 * (adjRivals * 1.0);

    return score;
}

float scorePermutation(int *perm, int numTeams, int numCatsPer, int r)
{
    float total = 0.0;
    Cat *teamLocal[10];

    for (int i = 0; i < numTeams; i++)
    {
        int start = i * numCatsPer;

        for (int j = 0; j < numCatsPer; j++)
        {
            teamLocal[j] = &cats[perm[start + j]];
        }
        total += scoreTeam(teamLocal, numCatsPer, r);
    }
    return total;
}

void permuteUsed(int k, int totalCats, int *perm, int *used, int numTeams, int numCatsPer, int r)
{
    if (k == totalCats) // if all cats are used check team if score is better and store it
    {
        float s = scorePermutation(perm, numTeams, numCatsPer, r);

        if (s > bestPermScore)
        {
            bestPermScore = s;

            // store new best arragment into traker as teams
            for (int i = 0; i < numTeams; i++) // lopp through teams
            {
                for (int j = 0; j < numCatsPer; j++) // loop through each cat
                {
                    tracker[i][j] = perm[i * numCatsPer + j];
                }
            }
        }
        return;
    }

    for (int i = 0; i < totalCats; i++) // permtaion alg
    {
        if (!used[i])
        {
            used[i] = 1;
            perm[k] = i;
            permuteUsed(k + 1, totalCats, perm, used, numTeams, numCatsPer, r);
            used[i] = 0;
        }
    }
}
float scoreTeamFromTracker(int teamIndex, int numCatsPer, int r)
{
    Cat *teamLocal[10]; // max size of team <=10 from prompt

    for (int i = 0; i < numCatsPer; i++)
    {
        int catIndex = tracker[teamIndex][i]; // get catidx from tracker using teamidx and position idx
        teamLocal[i] = &cats[catIndex];
    }
    return scoreTeam(teamLocal, numCatsPer, r);
}

// final print function
void printFinal(int numTeams, int numCatsPer, int r)
{
    printf("Best Teams Grouping score: %.2f\n", bestPermScore);

    float bestTeamScore = 0.0;
    int bestTeamIdx = 0;

    for (int t = 0; t < numTeams; t++)
    {
        float teamScore = scoreTeamFromTracker(t, numCatsPer, r); // get current team score

        printf("Team %d:", t + 1);
        for (int i = 0; i < numCatsPer; i++)
        {
            printf(" %s", cats[tracker[t][i]].name); // print name of cats in team
        }
        printf(" %0.2f\n", teamScore); // preint team score

        if (teamScore > bestTeamScore) // if new team score bigger then best team score set ne team to best
        {
            bestTeamScore = teamScore;
            bestTeamIdx = t;
        }
    }

    printf("Best Candidate:");
    for (int i = 0; i < numCatsPer; i++)
    {
        printf(" %s", cats[tracker[bestTeamIdx][i]].name);
    }
    printf(" \n");
}
// create cats array
void createCats(int numTeams, int numCatsPer, int *totalCatsOut)
{
    int totalCats = numTeams * numCatsPer;
    if (totalCatsOut)
        *totalCatsOut = totalCats; // set totalptr

    cats = (Cat *)malloc(sizeof(Cat) * totalCats); // dynamically filling cat array

    for (int i = 0; i < totalCats; i++)
    {
        char tempName[MAX_STR + 1];  // max len+ 1
        char tempBreed[MAX_STR + 1]; // max len+ 1
        int t[MAX_SCORES];

        scanf("%s %s %d %d %d %d %d", tempName, tempBreed, &t[0], &t[1], &t[2], &t[3], &t[4]);
        cats[i].name = (char *)malloc(strlen(tempName) + 1); // malloc exact string length
        strcpy(cats[i].name, tempName);                      // copy string

        cats[i].breed = (char *)malloc(strlen(tempBreed) + 1); // malloc exact string length
        strcpy(cats[i].breed, tempBreed);                      // copy string

        cats[i].baseScore = 0;
        for (int s = 0; s < MAX_SCORES; s++)
        {
            cats[i].score[s] = t[s];   // filling cat score
            cats[i].baseScore += t[s]; // filling cat base score
        }
    }
}
// pcreate each paired rivails
void createRivals(int r, int totalCats)
{
    if (r <= 0)
        return;

    rivals = (Rivals *)malloc(sizeof(Rivals) * r);

    for (int i = 0; i < r; i++)
    {
        char n1[MAX_STR + 1];
        char n2[MAX_STR + 1];

        scanf("%s %s", n1, n2);

        Cat *c1 = NULL; // dummie cat
        Cat *c2 = NULL; // dummie cat

        for (int j = 0; j < totalCats; j++) // looping to find cat by name
        {
            if (strcmp(cats[j].name, n1) == 0)
                c1 = &cats[j]; // set dummie to rival cat

            if (strcmp(cats[j].name, n2) == 0)
                c2 = &cats[j]; // set dummie to rival cat 2
        }
        rivals[i].cat1 = c1; // allocate to rivals
        rivals[i].cat2 = c2; // allocate to rivals
    }
}

// for testing purposes
//  void printCats(int totalCats)
//  {
//      printf("\n=== DEBUG: CATS ===\n");

//     for (int i = 0; i < totalCats; i++)
//     {
//         printf("Cat %d\n", i);
//         printf("  Name : %s\n", cats[i].name);
//         printf("  Breed: %s\n", cats[i].breed);
//         printf("  Traits: ");

//         for (int j = 0; j < MAX_SCORES; j++) {
//             printf("%d ", cats[i].score[j]);
//         }

//         printf("\n  Base Score: %d\n\n", cats[i].baseScore);
//     }
// }

// void printRivals(int r)
// {
//     if (r == 0 || rivals == NULL)
//     {
//         printf("\n=== DEBUG: NO RIVALS ===\n");
//         return;
//     }

//     printf("\n=== DEBUG: RIVALS ===\n");

//     for (int i = 0; i < r; i++)
//     {
//         printf("Rival Pair %d: %s <-> %s\n",
//                i,
//                rivals[i].cat1->name,
//                rivals[i].cat2->name);
//     }
// }

int main(void)
{
    int numTeams, numCatsPer;
    scanf("%d %d", &numTeams, &numCatsPer);

    int totalCats = 0;
    createCats(numTeams, numCatsPer, &totalCats);

    int r;
    scanf("%d", &r);
    createRivals(r, totalCats);

    // printCats(totalCats);
    // printRivals(r);

    tracker = (int **)malloc(sizeof(int *) * numTeams); // makeing number of teams
    for (int t = 0; t < numTeams; t++)
    {
        tracker[t] = (int *)malloc(sizeof(int) * numCatsPer); // making each team hold num of cats
    }

    bestPermScore = 0.0;

    int *perm = (int *)malloc(sizeof(int) * totalCats); // perm array size of cats
    int *used = (int *)calloc(totalCats, sizeof(int));  // used array initalized to 0

    permuteUsed(0, totalCats, perm, used, numTeams, numCatsPer, r);

    printFinal(numTeams, numCatsPer, r);

    // freeing
    free(perm);
    free(used);

    for (int t = 0; t < numTeams; t++)
    {
        free(tracker[t]);
    }
    free(tracker);

    if (rivals)
        free(rivals);

    for (int i = 0; i < totalCats; i++)
    {
        free(cats[i].name);
        free(cats[i].breed);
    }
    free(cats);

    return 0;
}