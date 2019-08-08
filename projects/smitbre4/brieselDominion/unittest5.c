/* -----------------------------------------------------------------------
 * Demonstration of how to write unit tests for dominion-base
 * Include the following lines in your makefile:
 *
 * testUpdateCoins: testUpdateCoins.c dominion.o rngs.o
 *      gcc -o testUpdateCoins -g  testUpdateCoins.c dominion.o rngs.o $(CFLAGS)
 * -----------------------------------------------------------------------
 */

#include "dominion.h"
#include "dominion_helpers.h"
#include <string.h>
#include <stdio.h>
#include "rngs.h"

// set NOISY_TEST to 0 to remove printfs from output
#define NOISY_TEST 1
#define NUM_PLAYERS 3

void resetHand(struct gameState* G, int p, int handCount) {
  G->hand[p][0] = mine;
    for (int i = 1; i < G->handCount[p]; i++) {
      G->hand[p][i] = copper;
    }
  G->handCount[p] = handCount;
  G->coins = 0;
}

void asserttrue(int result) {
  if (result) {
    printf("SUCCESS ");
  } else {
    printf("FAIL ");
  }
}

int main() {
    int seed = 1000;
    int numPlayer = 3;
    int p, r, handCount;
    int k[10] = {adventurer, council_room, feast, gardens, mine
               , remodel, smithy, village, tribute, great_hall};
    struct gameState* G = newGame();
    int maxHandCount = 5;

    printf ("TESTING mineEffect():\n");
    for (p = 0; p < numPlayer; p++)
    {
        for (handCount = 2; handCount <= maxHandCount; handCount++)
        {

          if (NOISY_TEST == 1) {
            printf("Test player %d with %d card(s) in hand.\n", p, handCount);
          }

          r = initializeGame(numPlayer, k, seed, G); // initialize a new game

          G->whoseTurn = p;
          resetHand(G, p, handCount);

          G->hand[p][1] = council_room;
          int result = playMine(1, 0, 4, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif

          resetHand(G, p, handCount);
          result = playMine(1, 0, 7, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif

          resetHand(G, p, handCount);
          G->hand[p][1] = province;
          result = playMine(1, 0, 4, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif

          resetHand(G, p, handCount);
          result = playMine(1, 0, 2, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif

          //test buying a treasure card that is > +3 cost
          resetHand(G, p, handCount);
          result = playMine(1, 0, gold, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif



          //Test to make sure desired card was drawn from deck and chosen card was trashed
          //copper >> silver
          resetHand(G, p, handCount);
          int silverStart = fullDeckCount(p, silver, G);
          int copperStart = fullDeckCount(p, copper, G);
          result = playMine(1, 0, silver, p, G);
          asserttrue(fullDeckCount(p, silver, G) == silverStart+1);
          #if (NOISY_TEST == 1)
            printf("player has %d silver cards, expected = %d\n", fullDeckCount(p, silver, G), silverStart+1);
          #endif
          asserttrue(fullDeckCount(p, copper, G) == copperStart-1);
          #if (NOISY_TEST == 1)
            printf("player has %d copper cards, expected = %d\n", fullDeckCount(p, copper, G), copperStart-1);
          #endif

          //Test to make sure desired card was drawn from deck and chosen card was trashed
          //silver >> gold
          resetHand(G, p, handCount);
          G->hand[p][1] = silver;
          silverStart = fullDeckCount(p, silver, G);
          int goldStart = fullDeckCount(p, gold, G);
          result = playMine(1, 0, gold, p, G);
          asserttrue(fullDeckCount(p, gold, G) == goldStart+1);
          #if (NOISY_TEST == 1)
            printf("player has %d gold cards, expected = %d\n", fullDeckCount(p, gold, G), goldStart+1);
          #endif
          asserttrue(fullDeckCount(p, silver, G) == silverStart-1);
          #if (NOISY_TEST == 1)
            printf("player has %d silver cards, expected = %d\n", fullDeckCount(p, silver, G), silverStart-1);
          #endif

          //Test to make sure desired card was drawn from deck and chosen card was trashed
          //copper >> copper
          resetHand(G, p, handCount);
          copperStart = fullDeckCount(p, copper, G);
          int copperSupply = G->supplyCount[copper];
          result = playMine(1, 0, copper, p, G);
          asserttrue(fullDeckCount(p, copper, G) == copperStart);
          #if (NOISY_TEST == 1)
            printf("player has %d copper cards, expected = %d\n", fullDeckCount(p, copper, G), copperStart);
          #endif
          asserttrue(G->supplyCount[copper] == copperSupply-1);
          #if (NOISY_TEST == 1)
            printf("supply has %d copper cards, expected = %d\n", G->supplyCount[copper], copperSupply-1);
          #endif

          printf("\n");
        }
    }

    return 0;
}
