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

void asserttrue(int result) {
  if (result) {
    printf("SUCCESS ");
  } else {
    printf("FAIL ");
  }
}

int main() {
    int seed = 1000;
    int k[10] = {adventurer, council_room, feast, gardens, mine, remodel, smithy, village, tribute, great_hall};
    struct gameState* G = newGame();


    printf ("TESTING drawCard():\n");

    //Test draw card with empty deck
    initializeGame(2, k, seed, G);
    G->discard[0][0] = estate;
    G->discard[0][1] = estate;
    G->discard[0][2] = estate;
    G->discardCount[0] = 3;
    G->deckCount[0] = 0;
    for (int i = 0; i < 5; i++) {
      G->hand[0][i] = copper;
    }
    int startHand = G->handCount[0];
    drawCard(0, G);
    asserttrue(G->discardCount[0] == 0);
    #if (NOISY_TEST == 1)
      printf("Player 0 discard count = %d, expected = 0\n", G->discardCount[0]);
    #endif
    asserttrue(G->deckCount[0] == 2);
    #if (NOISY_TEST == 1)
      printf("Player 0 deck count = %d, expected = 2\n", G->deckCount[0]);
    #endif
    asserttrue(G->handCount[0] == startHand + 1);
    #if (NOISY_TEST == 1)
      printf("Player 0 deck count = %d, expected = %d\n", G->handCount[0], startHand+1);
    #endif

    //Test that right card was drawn and it was only one of them
    int foundCard = 0;
    for (int i = 0; i < G->handCount[0]; i++) {
      if (G->hand[0][i] == estate) foundCard++;
    }
    asserttrue(foundCard == 1);
    #if (NOISY_TEST == 1)
      printf("Player 0 estate count = %d, expected = 1\n", foundCard);
    #endif

    //test winners with garden and great_hall cards

    printf("\n");

    return 0;
}
