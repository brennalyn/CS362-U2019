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

void resetHand(struct gameState* G, int p) {
  for (int i = 0; i < NUM_PLAYERS; i++) {
    G->hand[i][0] = ambassador;
    for (int j = 1; j < G->handCount[p]; j++) {
      G->hand[i][j] = copper;
    }
    G->handCount[i] = G->handCount[p];
  }
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
               , remodel, smithy, village, ambassador, great_hall};
    struct gameState* G = newGame();
    int maxHandCount = 5;

    printf ("TESTING ambassadorEffect():\n");
    for (p = 0; p < numPlayer; p++)
    {
        for (handCount = 2; handCount <= maxHandCount; handCount++)
        {

          if (NOISY_TEST == 1) {
            printf("Test player %d with %d card(s) in hand.\n", p, handCount);
          }

          r = initializeGame(numPlayer, k, seed, G); // initialize a new game

          G->whoseTurn = p;
          G->handCount[p] = handCount;               // set the number of cards on hand
          resetHand(G, p);

          //tests for bad parameters
          int result = playAmbassador(1, 4, 0, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif
          result = playAmbassador(1, -1, 0, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif
          result = playAmbassador(0, 1, 0, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif
          G->hand[p][1] = curse;
          result = playAmbassador(1, 2, 0, p, G);
          asserttrue(result == -1);
          #if (NOISY_TEST == 1)
            printf("function returned = %d, expected = %d\n", result, -1);
          #endif

          //clear data
          resetHand(G, p);

          //Now test calls for 0, 1, 2 discards of revealed card 'curse'

          for (int i = 0; i < 3; i++) {
            if (handCount > 2 || i < 2) {   //make sure there are enough cards in hand for discarding 2
              for (int j = 0; j < numPlayer; j++){
                G->hand[p][1] = curse;
                G->hand[p][2] = curse;
                G->handCount[p] = handCount;
                int curseCount = fullDeckCount(j, curse, G);
                result = playAmbassador(1, i, 0, p, G);
                if (j == p) {
                  asserttrue(fullDeckCount(j, curse, G) == curseCount-i);
                  #if (NOISY_TEST == 1)
                    printf("player %d has %d curse cards, expected = %d\n", j, fullDeckCount(j, curse, G), curseCount-i);
                  #endif
                } else {
                  asserttrue(fullDeckCount(j, curse, G) == curseCount+1);
                  #if (NOISY_TEST == 1)
                    printf("player %d has %d curse cards, expected = %d\n", j, fullDeckCount(j, curse, G), curseCount+1);
                  #endif
                }
                resetHand(G, p);
              }
            }
          }

          resetHand(G, p);

          printf("\n");
        }
    }

    return 0;
}
