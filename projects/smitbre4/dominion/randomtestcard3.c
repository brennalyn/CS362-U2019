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
#define NOISY_TEST 1

void resetHand(struct gameState* G, int p, int handCount, int* k) {
  for (int i = 0; i < handCount; i++) {
    G->hand[p][i] = k[rand() % 10];  //pick random cards from k for the hand (added estate for reasons)
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
    int r;
    struct gameState* G = newGame();
    //int maxHandCount = 5;
    int k[10] = {estate, province, tribute, mine, gardens
           , remodel, smithy, copper, silver, gold};   //edited to have cards of all 3 types
    //int handCounts[MAX_PLAYERS] = { 0 };

    srand(time(NULL));
    printf ("TESTING tributeEffect():\n");

    for (int i = 0; i < 100; i++) {
      int numPlayers =  rand() % MAX_PLAYERS + 1;
      if (numPlayers == 1) numPlayers++;
      r = initializeGame(numPlayers, k, seed, G); // initialize a new game
      G->whoseTurn = rand() % numPlayers;

      int current = G->whoseTurn;
      int next = current + 1;
      if (next >= G->numPlayers) next = 0;
      G->coins = 0;

      int temp = rand() % 10 + 1;       // set the number of cards on hand
      G->handCount[current] = temp;
      resetHand(G, current, temp, k);
      int handPos = rand() % G->handCount[current];     //minion has to be included
      G->hand[current][handPos] = tribute;               //randomize hand position

      temp = rand() % 10;       // set the number of cards on hand
      G->handCount[next] = temp;
      resetHand(G, next, temp, k);

      int revealedCards[2] = {-1, -1};
      for (int j = 0; j < 2; j++){
        if (j < G->handCount[next] - 1) {
          revealedCards[j] = G->hand[next][G->handCount[next] - j - 1];
        }
      }

      if (revealedCards[0] == revealedCards[1]) {
        revealedCards[1] = -1;
      }

      int coinTracker = G->coins;
      int actionTracker = G->numActions;
      int handTracker = G->handCount[current] - 1;
      tributeEffect(handPos, G);

      if (G->handCount[next] != 0) {
        int x = 0;
        while (revealedCards[x] != -1 && x < 2) {
          if (revealedCards[x] == copper || revealedCards[x] == silver || revealedCards[x] == gold) {
            coinTracker++;
          } else if (revealedCards[x] == estate || revealedCards[x] == province || revealedCards[x] == gardens) {
            handTracker++;
          } else if (revealedCards[x] != curse) {
            actionTracker++;
          }
          x++;
        }
      }

      asserttrue(G->coins == coinTracker);
      #if (NOISY_TEST == 1)
        printf("coins for current player = %d, expected = %d\n", G->coins, coinTracker);
      #endif
      asserttrue(G->numActions == actionTracker);
      #if (NOISY_TEST == 1)
        printf("actions for current player = %d, expected = %d\n", G->numActions, actionTracker);
      #endif
      asserttrue(G->handCount[current] == handTracker);
      #if (NOISY_TEST == 1)
        printf("hand count for current player = %d, expected = %d\n", G->handCount[current], handTracker);
      #endif
    }

    printf("\n");
    return 0;
}
