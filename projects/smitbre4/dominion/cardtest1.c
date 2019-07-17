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
    int k[10] = {adventurer, council_room, feast, gardens, mine, remodel, smithy, village, tribute, great_hall};
    struct gameState* G = newGame();

    printf ("TESTING initializeGame():\n");

    if (NOISY_TEST == 1) {
      printf("Test initializing with 1 and Max + 1 players, as well as invalid kingdom set.\n");
    }

    int result = initializeGame(1, k, seed, G); // initialize a new game with 1 player
    asserttrue(result == -1);
    #if (NOISY_TEST == 1)
      printf("function returned = %d, expected = %d\n", result, -1);
    #endif

    result = initializeGame(MAX_PLAYERS+1, k, seed, G); // initialize a new game with max +1 players
    asserttrue(result == -1);
    #if (NOISY_TEST == 1)
      printf("function returned = %d, expected = %d\n", result, -1);
    #endif

    k[1] = k[0];
    result = initializeGame(2, k, seed, G); // initialize a new game with 2 of the same kingdom card
    asserttrue(result == -1);
    #if (NOISY_TEST == 1)
      printf("function returned = %d, expected = %d\n", result, -1);
    #endif

    if (NOISY_TEST == 1) {
      printf("Test initializing with 2 players.\n");
    }

    k[1] = council_room;
    result = initializeGame(2, k, seed, G); // initialize a new game w/ 2 players and test all 2 player unique cases
    asserttrue(supplyCount(curse, G) == 10);
    #if (NOISY_TEST == 1)
      printf("curse cards = %d, expected = %d\n", supplyCount(curse, G), 10);
    #endif
    asserttrue(supplyCount(estate, G) == 8);
    #if (NOISY_TEST == 1)
      printf("estate cards = %d, expected = %d\n", supplyCount(estate, G), 8);
    #endif
    asserttrue(supplyCount(duchy, G) == 8);
    #if (NOISY_TEST == 1)
      printf("duchy cards = %d, expected = %d\n", supplyCount(duchy, G), 8);
    #endif
    asserttrue(supplyCount(province, G) == 8);
    #if (NOISY_TEST == 1)
      printf("province cards = %d, expected = %d\n", supplyCount(province, G), 8);
    #endif
    asserttrue(supplyCount(copper, G) == (60 - 14));
    #if (NOISY_TEST == 1)
      printf("copper cards = %d, expected = %d\n", supplyCount(copper, G), 60-14);
    #endif
    asserttrue(supplyCount(gardens, G) == 8);
    #if (NOISY_TEST == 1)
      printf("gardens cards = %d, expected = %d\n", supplyCount(gardens, G), 8);
    #endif
    asserttrue(supplyCount(great_hall, G) == 8);
    #if (NOISY_TEST == 1)
      printf("great_hall cards = %d, expected = %d\n", supplyCount(great_hall, G), 8);
    #endif


    if (NOISY_TEST == 1) {
      printf("Test initializing with 3 players.\n");
    }
    result = initializeGame(3, k, seed, G); //initialize a new game w/ 3 players and test all functionality
    asserttrue(supplyCount(curse, G) == 20);
    #if (NOISY_TEST == 1)
      printf("curse cards = %d, expected = %d\n", supplyCount(curse, G), 20);
    #endif
    asserttrue(supplyCount(estate, G) == 12);
    #if (NOISY_TEST == 1)
      printf("estate cards = %d, expected = %d\n", supplyCount(estate, G), 12);
    #endif
    asserttrue(supplyCount(duchy, G) == 12);
    #if (NOISY_TEST == 1)
      printf("duchy cards = %d, expected = %d\n", supplyCount(duchy, G), 12);
    #endif
    asserttrue(supplyCount(province, G) == 12);
    #if (NOISY_TEST == 1)
      printf("province cards = %d, expected = %d\n", supplyCount(province, G), 12);
    #endif
    asserttrue(supplyCount(gardens, G) == 12);
    #if (NOISY_TEST == 1)
      printf("gardens cards = %d, expected = %d\n", supplyCount(gardens, G), 12);
    #endif
    asserttrue(supplyCount(great_hall, G) == 12);
    #if (NOISY_TEST == 1)
      printf("great_hall cards = %d, expected = %d\n", supplyCount(great_hall, G), 12);
    #endif
    asserttrue(supplyCount(copper, G) == (60 - 21));
    #if (NOISY_TEST == 1)
      printf("copper cards = %d, expected = %d\n", supplyCount(copper, G), 60-21);
    #endif
    asserttrue(supplyCount(silver, G) == 40);
    #if (NOISY_TEST == 1)
      printf("silver cards = %d, expected = %d\n", supplyCount(silver, G), 40);
    #endif
    asserttrue(supplyCount(gold, G) == 30);
    #if (NOISY_TEST == 1)
      printf("gold cards = %d, expected = %d\n", supplyCount(gold, G), 30);
    #endif

    int inKingdomSet = 0;
    for (int i = adventurer; i <= treasure_map; i++) {
      if (i != great_hall && i != gardens) {
        for (int j = 0; j < 10; j++) {
          if (k[j] == i) {
            inKingdomSet = 1;
            break;
          }
        }
        if (inKingdomSet) {
          asserttrue(supplyCount(i, G) == 10);
          #if (NOISY_TEST == 1)
            printf("supplyPos %d cards = %d, expected = %d\n", i, supplyCount(i, G), 10);
          #endif
        } else {
          asserttrue(supplyCount(i, G) == -1);
          #if (NOISY_TEST == 1)
            printf("supplyPos %d cards = %d, expected = %d\n", i, supplyCount(i, G), -1);
          #endif
        }
        inKingdomSet = 0;
      }
    }

    asserttrue(G->whoseTurn == 0);
    #if (NOISY_TEST == 1)
      printf("the turn is set to %d, expected 0\n", G->whoseTurn);
    #endif

    for (int i = 0; i < 3; i++) { //test all players starting hands
      asserttrue(fullDeckCount(i, estate, G) == 3);
      #if (NOISY_TEST == 1)
        printf("estate cards in player %d deck = %d, expected = %d\n", i, fullDeckCount(i, estate, G), 3);
      #endif
      asserttrue(fullDeckCount(i, copper, G) == 7);
      #if (NOISY_TEST == 1)
        printf("copper cards in player %d deck = %d, expected = %d\n", i, fullDeckCount(i, copper, G), 7);
      #endif
      if (i == 0) {
        asserttrue(G->handCount[i] == 5);
        #if (NOISY_TEST == 1)
          printf("cards in player %d hand = %d, expected = %d\n", i, G->handCount[i], 5);
        #endif
        asserttrue(G->coins > 0);
        #if (NOISY_TEST == 1)
          printf("coins for player %d = %d, expected > 0\n", i, G->coins);
        #endif
      } else {
        asserttrue(G->handCount[i] == 0);
        #if (NOISY_TEST == 1)
          printf("cards in player %d hand = %d, expected = %d\n", i, G->handCount[i], 0);
        #endif
      }
    }

    asserttrue(G->phase == 0);
    #if (NOISY_TEST == 1)
      printf("the phase is set to %d, expected 0\n", G->phase);
    #endif

    asserttrue(G->outpostPlayed == 0);
    #if (NOISY_TEST == 1)
      printf("outpostPlayed is set to %d, expected 0\n", G->outpostPlayed);
    #endif

    asserttrue(G->numActions == 1 && G->numBuys == 1);
    #if (NOISY_TEST == 1)
      printf("numActions is %d and numBuys is %d, expected both to be 1\n", G->numActions, G->numBuys);
    #endif

    if (NOISY_TEST == 1) {
      printf("Test initializing with 4 players.\n");
    }
    result = initializeGame(4, k, seed, G); // initialize a new game w/ 4 players to check curse increased
    asserttrue(supplyCount(curse, G) == 30);
    #if (NOISY_TEST == 1)
      printf("curse cards = %d, expected = %d\n", supplyCount(curse, G), 30);
    #endif

    printf("\n");


    return 0;
}
