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
    int k[10] = {adventurer, council_room, minion, gardens, mine
           , remodel, smithy, village, baron, great_hall};
    int handCounts[MAX_PLAYERS] = { 0 };

    srand(time(NULL));
    printf ("TESTING minionEffect():\n");

    for (int i = 0; i < 100; i++) {

      int numPlayers =  rand() % (MAX_PLAYERS + 1 - 2) + 2;
      r = initializeGame(numPlayers, k, seed, G); // initialize a new game
      G->whoseTurn = rand() % G->numPlayers;
      int current = G->whoseTurn;
      G->coins = 0;

      for (int j = 0; j < G->numPlayers; j++) {
        int temp = (rand() % 10) + 1;       // set the number of cards on hand
        G->handCount[j] = temp;
        resetHand(G, j, temp, k);
        handCounts[j] = temp;
      }

      int handPos = rand() % G->handCount[current];     //minion has to be included
      G->hand[current][handPos] = minion;               //randomize hand position

      int choice = rand() % 2 + 1;
      minionEffect(handPos, choice, G);

      if (choice == 1){
        for (int j = 0; j < G->numPlayers; j++) {
          if (j != current) {
            asserttrue(G->handCount[j] == handCounts[j]);
            #if (NOISY_TEST == 1)
              printf("cards in player %d hand = %d, expected = %d\n", j, G->handCount[j], handCounts[j]);
            #endif
          } else {
            asserttrue(G->handCount[j] == handCounts[j] - 1);  //have to account for current player discarding minion
            #if (NOISY_TEST == 1)
              printf("cards in player %d hand = %d, expected = %d\n", j, G->handCount[j], handCounts[j] - 1);
            #endif
          }
        }
        asserttrue(G->coins == 2);
        #if (NOISY_TEST == 1)
          printf("coins = %d, expected = %d\n", G->coins, 2);
        #endif
      } else {
        for (int j = 0; j < G->numPlayers; j++) {
          if (j == current || handCounts[j] >= 5) {
            asserttrue(G->handCount[j] == 4);
            #if (NOISY_TEST == 1)
              printf("cards in player %d hand = %d, expected = %d\n", j, G->handCount[j], 4);
            #endif
          } else {
            asserttrue(G->handCount[j] == handCounts[j]);  //have to account for any players with < 5 cards
            #if (NOISY_TEST == 1)
              printf("cards in player %d hand = %d, expected = %d\n", j, G->handCount[j], handCounts[j]);
            #endif
          }
        }
        asserttrue(G->coins == 0);
        #if (NOISY_TEST == 1)
          printf("coins = %d, expected = %d\n", G->coins, 0);
        #endif
      }

      asserttrue(G->numActions == 1);
      #if (NOISY_TEST == 1)
        printf("actionss = %d, expected = %d\n", G->numActions, 1);
      #endif
    }

    printf("\n");
    return 0;
}
