/* -----------------------------------------------------------------------
 * Brenna Smith
 * CS362 2019
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

void asserttrue(int result) {
  if (result) {
    printf("SUCCESS ");
  } else {
    printf("FAIL ");
  }
}

int main() {
    int seed = 1000;
    int nextPlayer, currPlayer = 0;
    int numPlayer = 3;
    int k[10] = {adventurer, council_room, feast, gardens, mine, remodel, smithy, village, tribute, great_hall};
    struct gameState* G = newGame();


    printf ("TESTING endTurn():\n");

    if (NOISY_TEST == 1) {
      printf("Test that endTurn does not advance when game is over\n");
    }

    //Test endturn call when game is over (should not advance if game is over)
    initializeGame(numPlayer, k, seed, G);
    G->supplyCount[province] = 0;
    G->whoseTurn = currPlayer;
    endTurn(G);
    asserttrue(G->whoseTurn == currPlayer);
    #if (NOISY_TEST == 1)
      printf("It's player %d turn, expected to be player %d\n", G->whoseTurn, currPlayer);
    #endif

    for (int i = 2; i <= numPlayer; i++) {
      initializeGame(i, k, seed, G);
      currPlayer = G->whoseTurn;
      nextPlayer = currPlayer + 1;
      if (currPlayer >= numPlayer) nextPlayer = 0;

      //Test that the turn is changed to p + 1
      G->phase = 1;
      endTurn(G);
      asserttrue(G->whoseTurn == nextPlayer);
      #if (NOISY_TEST == 1)
        printf("It's player %d turn, expected to be player %d\n", G->whoseTurn, nextPlayer);
      #endif

      //Test that phase == 0
      asserttrue(G->phase == 0);
      #if (NOISY_TEST == 1)
        printf("phase %d, expected to be start phase of 0\n", G->phase);
      #endif

      //test that all initial values are correct (set hand and check coins + others from initialize)
      asserttrue(G->numActions == 1 && G->numBuys == 1);
      #if (NOISY_TEST == 1)
        printf("numActions is %d and numBuys is %d, expected both to be 1\n", G->numActions, G->numBuys);
      #endif

      asserttrue(G->coins > 0);
      #if (NOISY_TEST == 1)
        printf("coins for player %d = %d, expected > 0\n", nextPlayer, G->coins);
      #endif

      asserttrue(G->outpostPlayed == 0);
      #if (NOISY_TEST == 1)
        printf("outpostPlayed is set to %d, expected 0\n", G->outpostPlayed);
      #endif

      asserttrue(G->playedCardCount == 0);
      #if (NOISY_TEST == 1)
        printf("playedCards is set to %d, expected 0\n", G->playedCardCount);
      #endif

      //make sure new player hand == 5
      asserttrue(G->handCount[nextPlayer] == 5);
      #if (NOISY_TEST == 1)
        printf("player %d hand count = %d, expected = 5\n", nextPlayer, G->handCount[nextPlayer]);
      #endif

      //make sure last player discarded hand
      asserttrue(G->handCount[currPlayer] == 0);
      #if (NOISY_TEST == 1)
        printf("player %d hand count = %d, expected = 0\n", currPlayer, G->handCount[currPlayer]);
      #endif
    }


    printf("\n");

    return 0;
}
