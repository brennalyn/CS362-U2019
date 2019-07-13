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
  G->hand[p][0] = minion;
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
               , remodel, smithy, village, minion, great_hall};
    struct gameState* G = newGame();
    int maxHandCount = 5;


    printf ("TESTING minionEffect():\n");
    for (p = 0; p < numPlayer; p++)
    {
        for (handCount = 1; handCount <= maxHandCount; handCount++)
        {

          if (NOISY_TEST == 1) {
            printf("Test player %d with %d card(s) in hand.\n", p, handCount);
          }

          r = initializeGame(numPlayer, k, seed, G); // initialize a new game

          G->whoseTurn = p;
          G->handCount[p] = handCount;               // set the number of cards on hand
          G->handCount[p+1] = handCount + 3;         //Give another player more
          resetHand(G, p, handCount);

          //tests for choice to gain coins
          minionEffect(0, 1, G);
          asserttrue(G->coins == 2);
          #if (NOISY_TEST == 1)
            printf("coins = %d, expected = %d\n", G->coins, 2);
          #endif

          //clear data
          resetHand(G, p, handCount);

          //This test only works if there are 2 or more cards in hand
          for (int i = 0; i < numPlayer; i++) {
            if (G->handCount[i] > 4 || i == p) {
              int deckStart = G->discardCount[i] + G->handCount[i] + G->deckCount[i];
              minionEffect(0, 2, G);
              int deckAfter = G->discardCount[i] + G->handCount[i] + G->deckCount[i];
              asserttrue(G->handCount[i] == 4);
              #if (NOISY_TEST == 1)
                printf("cards in player %d hand = %d, expected = %d\n", i, G->handCount[i], 4);
              #endif
              asserttrue(G->handCount[i] == 4);
              #if (NOISY_TEST == 1)
                printf("cards in player %d hand = %d, expected = %d\n", i, G->handCount[i], 4);
              #endif
              //make sure no cards were gained or lost from total deck in the process
              asserttrue(deckAfter == deckStart);
              #if (NOISY_TEST == 1)
                printf("cards in player %d full deck = %d, expected = %d\n", i, deckAfter, deckStart);
              #endif
            } else {
              int tempCount = G->handCount[i];
              minionEffect(0, 2, G);
              asserttrue(G->handCount[i] == tempCount);
              #if (NOISY_TEST == 1)
                printf("cards in player %d hand = %d, expected = %d\n", i, G->handCount[i], tempCount);
              #endif
            }
          }

          //Make sure the coins didn't change
          asserttrue(G->coins == 0);
          #if (NOISY_TEST == 1)
            printf("coins = %d, expected = %d\n", G->coins, 4);
          #endif

          resetHand(G, p, handCount);

          printf("\n");
        }
    }

    return 0;
}
