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

void resetHand(struct gameState* G, int p, int handCount) {
  G->hand[p][0] = baron;
  for (int i = 1; i < handCount; i++) {
    G->hand[p][i] = copper;
  }
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
               , remodel, smithy, village, baron, great_hall};
    struct gameState* G = newGame();
    int maxHandCount = 5;


    printf ("TESTING baronEffect():\n");
    for (p = 0; p < numPlayer; p++)
    {
        for (handCount = 1; handCount <= maxHandCount; handCount++)
        {

          if (NOISY_TEST == 1) {
            printf("Test player %d with %d card(s) in hand.\n", p, handCount);
          }

          //memset(G, 23, sizeof(struct gameState));   // clear the game state
          r = initializeGame(numPlayer, k, seed, G); // initialize a new game

          G->whoseTurn = p;
          G->handCount[p] = handCount;                 // set the number of cards on hand
          resetHand(G, p, handCount);

          //tests for discarding estate choice but no estate in hand
          int deckStart = fullDeckCount(p, estate, G);
          playBaron(1, 0, p, G);
          asserttrue(fullDeckCount(p, estate, G) == deckStart+1);
          #if (NOISY_TEST == 1)
            printf("estates in player %d deck = %d, expected = %d\n", p, fullDeckCount(p, estate, G), deckStart+1);
          #endif
          asserttrue(G->coins == 0);
          #if (NOISY_TEST == 1)
            printf("coins = %d, expected = %d\n", G->coins, 0);
          #endif

          //clear data
          resetHand(G, p, handCount);

          //This test only works if there are 2 or more cards in hand
          if (handCount > 1) {
            G->handCount[p] = handCount;
            G->hand[p][handCount-1] = estate;

            //tests for discarding estate w/ estate in hand
            deckStart = fullDeckCount(p, estate, G);
            playBaron(1, 0, p, G);
            asserttrue(fullDeckCount(p, estate, G) == deckStart);
            #if (NOISY_TEST == 1)
              printf("estates in player %d deck = %d, expected = %d\n", p, fullDeckCount(p, estate, G), deckStart);
            #endif
            asserttrue(G->coins == 4);
            #if (NOISY_TEST == 1)
              printf("coins = %d, expected = %d\n", G->coins, 4);
            #endif
          }

          resetHand(G, p, handCount);

          //tests for gaining estate and changing hand position
          deckStart = fullDeckCount(p, estate, G);
          G->hand[p][handCount-1] = baron;
          playBaron(0, handCount-1, p, G);
          asserttrue(fullDeckCount(p, estate, G) == deckStart+1);
          #if (NOISY_TEST == 1)
            printf("estates in player %d deck = %d, expected = %d\n", p, fullDeckCount(p, estate, G), deckStart+1);
          #endif
          asserttrue(G->coins == 0);
          #if (NOISY_TEST == 1)
            printf("coins = %d, expected = %d\n", G->coins, 0);
          #endif

          printf("\n");
        }
    }

    return 0;
}
