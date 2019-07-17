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
    int numPlayer = 3;
    int k[10] = {adventurer, council_room, feast, gardens, mine, remodel, smithy, village, tribute, great_hall};
    struct gameState* G = newGame();


    printf ("TESTING shuffle():\n");

    if (NOISY_TEST == 1) {
      printf("Test shuffle with no cards in deck.\n");
    }


    for (int i = 0; i < G->deckCount[0]; i++){
      G->deck[0][i] = -1;
    }
    G->deckCount[0] = 0;
    int result = shuffle(0, G);
    asserttrue(result == -1);
    #if (NOISY_TEST == 1)
      printf("function returned = %d, expected = %d\n", result, -1);
    #endif

    initializeGame(numPlayer, k, seed, G); //reset hands
    int changeFound = 0;
    int matchFound = 0;
    int testHand[5] = {feast, gardens, remodel, estate, copper};

    for (int i = 0; i < 5; i++){
      G->hand[0][i] = testHand[i];
    }
    G->deckCount[0] = 5;
    shuffle(0, G);
    for (int i = 0; i < 5; i++) {
      if (testHand[i] != G->hand[0][i]) {
        changeFound++;
      }
    }
    asserttrue(changeFound > 1);   //Make sure there are at least 2 differences in the array
    #if (NOISY_TEST == 1)
      printf("There were %d changes found, expected > 1\n", changeFound);
    #endif

    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        if (testHand[i] == G->hand[0][j]){
          matchFound++;
        }
      }
      if (matchFound != 1) {
        break;
      } else {
        matchFound = 0;
      }
    }
    asserttrue(matchFound == 1);   //Make sure there are the same set of cards we started with
    #if (NOISY_TEST == 1)
      printf("There were %d matches foundfor a card, expected exactly 1\n", matchFound);
    #endif

    printf("\n");

    return 0;
}
