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
#define NUM_PLAYERS 3

void resetHand(struct gameState* G, int p, int handCount) {
  G->hand[p][0] = tribute;
    for (int i = 1; i < G->handCount[p]; i++) {
      G->hand[p][i] = copper;
    }
  G->handCount[p] = handCount;
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
               , remodel, smithy, village, tribute, great_hall};
    struct gameState* G = newGame();
    int maxHandCount = 5;

    printf ("TESTING tributeEffect():\n");
    for (p = 0; p < numPlayer; p++)
    {
        for (handCount = 1; handCount <= maxHandCount; handCount++)
        {

          if (NOISY_TEST == 1) {
            printf("Test player %d with %d card(s) in hand.\n", p, handCount);
          }

          r = initializeGame(numPlayer, k, seed, G); // initialize a new game

          G->whoseTurn = p;
          int nextPlayer = p+1;
          if (nextPlayer >= numPlayer)
            nextPlayer = 0;
          G->handCount[p] = handCount;               // set the number of cards on hand
          resetHand(G, p, handCount);

          //Test to make sure nothing was gained when the next player has empty discard and deck
          G->deckCount[nextPlayer] = G->discardCount[nextPlayer] = 0;   //set nextplayer to have 0 cards outside hand
          int coinStart = G->coins;
          playTribute(p, G);
          asserttrue(G->coins == coinStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart);
          #endif
          resetHand(G, p, handCount);

          int actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->numActions == actionStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart);
          #endif
          resetHand(G, p, handCount);

          playTribute(p, G);
          asserttrue(G->handCount[p] == handCount-1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount-1);
          #endif
          resetHand(G, p, handCount);

          //test duplicate treasure cards
          int deckCounter = G->deckCount[nextPlayer] = 10;   //set nextplayer to have cards in deck
          G->deck[nextPlayer][deckCounter - 1] = G->deck[nextPlayer][deckCounter - 2] = gold;   //set top 2 of next player deck to a treasure card
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart + 2);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart+2);
          #endif
          asserttrue(G->numActions == actionStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart);
          #endif
          asserttrue(G->handCount[p] == handCount-1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount-1);
          #endif
          asserttrue(G->discardCount[nextPlayer] == 2);
          #if (NOISY_TEST == 1)
            printf("next player %d has %d cards in discard, expected = %d\n", p, G->discardCount[nextPlayer], 2);
          #endif
          resetHand(G, p, handCount);

          //test different treasure cards
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = gold;
          G->deck[nextPlayer][deckCounter - 2] = copper;      //set nextplayer deck to have 2 diff treasures
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart + 4);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart+4);
          #endif
          asserttrue(G->numActions == actionStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart);
          #endif
          asserttrue(G->handCount[p] == handCount-1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount-1);
          #endif
          resetHand(G, p, handCount);

          //test different treasure and action combo
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = silver;
          G->deck[nextPlayer][deckCounter - 2] = baron;      //set nextplayer deck to have 2 diff types
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart+2);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart+2);
          #endif
          asserttrue(G->numActions == actionStart+2);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart+2);
          #endif
          asserttrue(G->handCount[p] == handCount-1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount-1);
          #endif
          resetHand(G, p, handCount);

          //test duplicate action cards
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = G->deck[nextPlayer][deckCounter - 2] = mine;   //set top 2 of next player deck to a action card
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart);
          #endif
          asserttrue(G->numActions == actionStart+2);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart+2);
          #endif
          asserttrue(G->handCount[p] == handCount-1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount-1);
          #endif
          resetHand(G, p, handCount);

          //test different action cards
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = minion;
          G->deck[nextPlayer][deckCounter - 2] = village;   //set top 2 of next player deck to a action card
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart);
          #endif
          asserttrue(G->numActions == actionStart+4);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart+4);
          #endif
          asserttrue(G->handCount[p] == handCount-1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount-1);
          #endif
          resetHand(G, p, handCount);

          //test action and victory combo
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = smithy;
          G->deck[nextPlayer][deckCounter - 2] = duchy;      //set nextplayer deck to have 2 diff types
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart);
          #endif
          asserttrue(G->numActions == actionStart+2);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart+2);
          #endif
          asserttrue(G->handCount[p] == handCount+1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount+1);
          #endif
          resetHand(G, p, handCount);

          //test duplicate victory cards
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = G->deck[nextPlayer][deckCounter - 2] = province;   //set top 2 of next player deck to a treasure card
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart);
          #endif
          asserttrue(G->numActions == actionStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart);
          #endif
          asserttrue(G->handCount[p] == handCount+1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount+1);
          #endif
          resetHand(G, p, handCount);

          //test different victory cards
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = estate;
          G->deck[nextPlayer][deckCounter - 2] = gardens;   //set top 2 of next player deck to a treasure card
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart);
          #endif
          asserttrue(G->numActions == actionStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart);
          #endif
          asserttrue(G->handCount[p] == handCount+3);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount+3);
          #endif
          resetHand(G, p, handCount);

          //test treasure and victory combo
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = silver;
          G->deck[nextPlayer][deckCounter - 2] = great_hall;      //set nextplayer deck to have 2 diff types
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart+2);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart+2);
          #endif
          asserttrue(G->numActions == actionStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart);
          #endif
          asserttrue(G->handCount[p] == handCount+1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount+1);
          #endif
          resetHand(G, p, handCount);


          //Test double curse to make sure it does nothing
          G->deckCount[nextPlayer] = 10;
          G->deck[nextPlayer][deckCounter - 1] = G->deck[nextPlayer][deckCounter - 2] = curse;      //set nextplayer deck to have 2 diff types
          coinStart = G->coins;
          actionStart = G->numActions;
          playTribute(p, G);
          asserttrue(G->coins == coinStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d coins, expected = %d\n", p, G->coins, coinStart);
          #endif
          asserttrue(G->numActions == actionStart);
          #if (NOISY_TEST == 1)
            printf("player %d has %d actions, expected = %d\n", p, G->numActions, actionStart);
          #endif
          asserttrue(G->handCount[p] == handCount-1);
          #if (NOISY_TEST == 1)
            printf("player %d has %d cards in hand, expected = %d\n", p, G->handCount[p], handCount-1);
          #endif
          resetHand(G, p, handCount);

          printf("\n");
        }
    }

    return 0;
}
