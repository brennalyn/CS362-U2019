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

void setDeck (int numPlayers, struct gameState* G) {
  for (int i = 0; i < numPlayers; i++) {
    for (int j = 5; j < MAX_DECK; j++) {
      G->deck[i][j] = -1;
    }
    G->deck[i][0] = copper;
    G->deck[i][1] = estate;
    G->deck[i][2] = duchy;
    G->deck[i][3] = province;
    G->deck[i][4] = estate;
    G->deckCount[i] = 5;
  }
  for (int i = 0; i < 5; i++) {
    G->hand[0][i] = copper;
  }
  G->handCount[0] = 5;
}

int main() {
    int seed = 1000;
    int k[10] = {adventurer, council_room, feast, gardens, mine, remodel, smithy, village, tribute, great_hall};
    int scoreCard[MAX_PLAYERS];
    struct gameState* G = newGame();


    printf ("TESTING getWinners():\n");

    if (NOISY_TEST == 1) {
      printf("Test getWinners with 1 winner\n");
    }

    //Test 1 winner
    initializeGame(2, k, seed, G);
    setDeck(2, G);
    G->hand[0][0] = estate;
    getWinners(scoreCard, G);
    asserttrue(scoreCard[0] == 1);
    #if (NOISY_TEST == 1)
      printf("Player 0 score card value = %d, expected = 1\n", scoreCard[0]);
    #endif
    asserttrue(scoreCard[1] == 0);
    #if (NOISY_TEST == 1)
      printf("Player 1 score card value = %d, expected = 0\n", scoreCard[1]);
    #endif
    asserttrue(scoreCard[2] == 0);
    #if (NOISY_TEST == 1)
      printf("Player 2 score card value = %d, expected = 0\n", scoreCard[2]);
    #endif

    //Test 2 winners w/ bonus to next player
    initializeGame(2, k, seed, G);
    setDeck(2, G);
    getWinners(scoreCard, G);
    asserttrue(scoreCard[0] == 0);
    #if (NOISY_TEST == 1)
      printf("Player 0 score card value = %d, expected = 0\n", scoreCard[0]);
    #endif
    asserttrue(scoreCard[1] == 1);
    #if (NOISY_TEST == 1)
      printf("Player 1 score card value = %d, expected = 1\n", scoreCard[1]);
    #endif
    asserttrue(scoreCard[2] == 0);
    #if (NOISY_TEST == 1)
      printf("Player 2 score card value = %d, expected = 0\n", scoreCard[2]);
    #endif

    //test 3 winners (2 way tie after adding bonus)
    initializeGame(3, k, seed, G);
    setDeck(3, G);
    getWinners(scoreCard, G);
    asserttrue(scoreCard[0] == 0);
    #if (NOISY_TEST == 1)
      printf("Player 0 score card value = %d, expected = 0\n", scoreCard[0]);
    #endif
    asserttrue(scoreCard[1] == 1);
    #if (NOISY_TEST == 1)
      printf("Player 1 score card value = %d, expected = 1\n", scoreCard[1]);
    #endif
    asserttrue(scoreCard[2] == 1);
    #if (NOISY_TEST == 1)
      printf("Player 2 score card value = %d, expected = 1\n", scoreCard[2]);
    #endif
    asserttrue(scoreCard[3] == 0);
    #if (NOISY_TEST == 1)
      printf("Player 3 score card value = %d, expected = 0\n", scoreCard[3]);
    #endif

    //test winners with garden and great_hall cards

    printf("\n");

    return 0;
}
