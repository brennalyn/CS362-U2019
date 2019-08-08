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
#include "rngs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// set NOISY_TEST to 0 to remove printfs from output
#define NOISY_TEST 0

int resetHand(struct gameState* G, int p, int handCount, int* k) {
  for (int i = 0; i < handCount; i++) {
    G->hand[p][i] = k[rand() % 4];  //pick random cards from k for the hand (added estate for reasons)
  }
  int handPos = rand() % handCount;     //baron has to be included
  G->hand[p][handPos] = baron;
  G->coins = 0;
  return handPos;
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
    int r;
    struct gameState* G = newGame();
    //int maxHandCount = 5;
    int k[10] = {adventurer, council_room, estate, gardens, mine
           , remodel, smithy, village, baron, great_hall};

    srand(time(NULL));
    printf ("TESTING baronEffect():\n");

    for (int i = 0; i < 100; i++) {
      int numPlayers =  rand() % (MAX_PLAYERS + 1 - 2) + 2;
      r = initializeGame(numPlayers, k, seed, G); // initialize a new game
      G->whoseTurn = rand() % G->numPlayers;
      int handCount = rand() % MAX_HAND + 1;    // set the number of cards on hand
      G->handCount[G->whoseTurn] = handCount;
      int handPos = resetHand(G, G->whoseTurn, handCount, k);

      //tests for discarding estate choice but no estate in hand
      int estates = 0;
      for (int j = 0; j < handCount; j++) {
        if (G->hand[G->whoseTurn][j] == estate) estates = 1;
      }
      int estateStart = fullDeckCount(G->whoseTurn, estate, G);
      int choice = rand() % 2;
      playBaron(choice, handPos, G->whoseTurn, G);

      if (!estates || !choice){
        asserttrue(fullDeckCount(G->whoseTurn, estate, G) == estateStart+1);
        #if (NOISY_TEST == 1)
          printf("estates in player %d deck = %d, expected = %d\n", G->whoseTurn, fullDeckCount(G->whoseTurn, estate, G), estateStart+1);
        #endif
        asserttrue(G->coins == 0);
        #if (NOISY_TEST == 1)
          printf("coins = %d, expected = %d\n", G->coins, 0);
        #endif
      } else {
        asserttrue(fullDeckCount(G->whoseTurn, estate, G) == estateStart);
        #if (NOISY_TEST == 1)
          printf("estates in player %d deck = %d, expected = %d\n", G->whoseTurn, fullDeckCount(G->whoseTurn, estate, G), estateStart);
        #endif
        asserttrue(G->coins == 4);
        #if (NOISY_TEST == 1)
          printf("coins = %d, expected = %d\n", G->coins, 4);
        #endif
      }
      asserttrue(G->numBuys == 2);
      #if (NOISY_TEST == 1)
        printf("buys = %d, expected = %d\n", G->numBuys, 2);
      #endif
    }

    printf("\n");
    return 0;
}
