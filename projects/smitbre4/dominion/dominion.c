#include "dominion.h"
#include "dominion_helpers.h"
#include "rngs.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


int compare(const void* a, const void* b) {
  if (*(int*)a > *(int*)b)
    return 1;
  if (*(int*)a < *(int*)b)
    return -1;
  return 0;
}

struct gameState* newGame() {
  struct gameState* g = malloc(sizeof(struct gameState));
  return g;
}

int* kingdomCards(int k1, int k2, int k3, int k4, int k5, int k6, int k7,
		  int k8, int k9, int k10) {
  int* k = malloc(10 * sizeof(int));
  k[0] = k1;
  k[1] = k2;
  k[2] = k3;
  k[3] = k4;
  k[4] = k5;
  k[5] = k6;
  k[6] = k7;
  k[7] = k8;
  k[8] = k9;
  k[9] = k10;
  return k;
}

int initializeGame(int numPlayers, int kingdomCards[10], int randomSeed, struct gameState *state) {

  int i;
  int j;
  int it;

  //set up random number generator
  SelectStream(1);
  PutSeed((long)randomSeed);

  //check number of players
  if (numPlayers > MAX_PLAYERS || numPlayers < 2) {
    return -1;
  }

  //set number of players
  state->numPlayers = numPlayers;

  //check selected kingdom cards are different
  for (i = 0; i < 10; i++) {
    for (j = 0; j < 10; j++) {
      if (j != i && kingdomCards[j] == kingdomCards[i]) {
        return -1;
      }
    }
  }

  //initialize supply
  ///////////////////////////////

  //set number of Curse cards
  if (numPlayers == 2) {
    state->supplyCount[curse] = 10;
  } else if (numPlayers == 3) {
    state->supplyCount[curse] = 20;
  } else {
    state->supplyCount[curse] = 30;
  }

  //set number of Victory cards
  if (numPlayers == 2) {
    state->supplyCount[estate] = 8;
    state->supplyCount[duchy] = 8;
    state->supplyCount[province] = 8;
  } else {
    state->supplyCount[estate] = 12;
    state->supplyCount[duchy] = 12;
    state->supplyCount[province] = 12;
  }

  //set number of Treasure cards
  state->supplyCount[copper] = 60 - (7 * numPlayers);
  state->supplyCount[silver] = 40;
  state->supplyCount[gold] = 30;

  //set number of Kingdom cards
  for (i = adventurer; i <= treasure_map; i++) {
    for (j = 0; j < 10; j++) {
      if (kingdomCards[j] == i) {
	     //check if card is a 'Victory' Kingdom card
	      if (kingdomCards[j] == great_hall || kingdomCards[j] == gardens) {
	        if (numPlayers == 2){
	          state->supplyCount[i] = 8;
	        } else {
            state->supplyCount[i] = 12;
          }
        } else {
          state->supplyCount[i] = 10;
        }
	      break;
	    } else {
        //card is not in the set choosen for the game
	      state->supplyCount[i] = -1;
	    }
	  }
  }

  ////////////////////////
  //supply intilization complete

  //set player decks
  for (i = 0; i < numPlayers; i++) {
    state->deckCount[i] = 0;
    for (j = 0; j < 3; j++)	{
      state->deck[i][j] = estate;
      state->deckCount[i]++;
    }
    for (j = 3; j < 10; j++) {
      state->deck[i][j] = copper;
      state->deckCount[i]++;
    }
  }

  //shuffle player decks
  for (i = 0; i < numPlayers; i++) {
    if ( shuffle(i, state) < 0 ) {
	     return -1;
	  }
  }

  //draw player hands
  for (i = 0; i < numPlayers; i++) {
      //initialize hand size to zero
      state->handCount[i] = 0;
      state->discardCount[i] = 0;
      //draw 5 cards
      // for (j = 0; j < 5; j++)
      //	{
      //	  drawCard(i, state);
      //	}
  }

  //set embargo tokens to 0 for all supply piles
  for (i = 0; i <= treasure_map; i++) {
    state->embargoTokens[i] = 0;
  }

  //initialize first player's turn
  state->outpostPlayed = 0;
  state->phase = 0;
  state->numActions = 1;
  state->numBuys = 1;
  state->playedCardCount = 0;
  state->whoseTurn = 0;
  state->handCount[state->whoseTurn] = 0;

  //Moved draw cards to here, only drawing at the start of a turn
  for (it = 0; it < 5; it++){
    drawCard(state->whoseTurn, state);
  }

  updateCoins(state->whoseTurn, state, 0);
  return 0;
}


int shuffle(int player, struct gameState *state) {
  int newDeck[MAX_DECK];
  int newDeckPos = 0;
  int card;
  int i;

  if (state->deckCount[player] < 1)
    return -1;

  qsort ((void*)(state->deck[player]), state->deckCount[player], sizeof(int), compare);
  /* SORT CARDS IN DECK TO ENSURE DETERMINISM! */

  while (state->deckCount[player] > 0) {
    card = floor(Random() * state->deckCount[player]);
    newDeck[newDeckPos] = state->deck[player][card];
    newDeckPos++;
    for (i = card; i < state->deckCount[player]-1; i++) {
      state->deck[player][i] = state->deck[player][i+1];
    }
    state->deckCount[player]--;
  }
  for (i = 0; i < newDeckPos; i++) {
    state->deck[player][i] = newDeck[i];
    state->deckCount[player]++;
  }

  return 0;
}

int playCard(int handPos, int choice1, int choice2, int choice3, struct gameState *state)
{
  int card;
  int coin_bonus = 0; 		//tracks coins gain from actions

  //check if it is the right phase
  if (state->phase != 0)
    {
      return -1;
    }

  //check if player has enough actions
  if ( state->numActions < 1 )
    {
      return -1;
    }

  //get card played
  card = handCard(handPos, state);

  //check if selected card is an action
  if ( card < adventurer || card > treasure_map )
    {
      return -1;
    }

  //play card
  if ( cardEffect(card, choice1, choice2, choice3, state, handPos, &coin_bonus) < 0 )
    {
      return -1;
    }

  //reduce number of actions
  state->numActions--;

  //Brenna added this: May need to move this elsewhere
  state->playedCards[state->playedCardCount] = card;
  state->playedCardCount++;

  //update coins (Treasure cards may be added with card draws)
  updateCoins(state->whoseTurn, state, coin_bonus);

  return 0;
}

int buyCard(int supplyPos, struct gameState *state) {
  int who;
  if (DEBUG){
    printf("Entering buyCard...\n");
  }

  // I don't know what to do about the phase thing.

  who = state->whoseTurn;

  if (state->numBuys < 1){
    if (DEBUG)
      printf("You do not have any buys left\n");
    return -1;
  } else if (supplyCount(supplyPos, state) <1){
    if (DEBUG)
      printf("There are not any of that type of card left\n");
    return -1;
  } else if (state->coins < getCost(supplyPos)){
    if (DEBUG)
      printf("You do not have enough money to buy that. You have %d coins.\n", state->coins);
    return -1;
  } else {
    state->phase=1;
    //state->supplyCount[supplyPos]--;
    gainCard(supplyPos, state, 0, who); //card goes in discard, this might be wrong.. (2 means goes into hand, 0 goes into discard)

    state->coins = (state->coins) - (getCost(supplyPos));
    state->numBuys--;
    if (DEBUG)
      printf("You bought card number %d for %d coins. You now have %d buys and %d coins.\n", supplyPos, getCost(supplyPos), state->numBuys, state->coins);
  }

  //state->discard[who][state->discardCount[who]] = supplyPos;
  //state->discardCount[who]++;

  return 0;
}

int cardsInHand(struct gameState *state) {
  return state->handCount[ state->whoseTurn ];
}

int handCard(int handPos, struct gameState *state) {
  return state->hand[state->whoseTurn][handPos];
}

int supplyCount(int card, struct gameState *state) {
  return state->supplyCount[card];
}

int fullDeckCount(int player, int card, struct gameState *state) {
  int i;
  int count = 0;

  for (i = 0; i < state->deckCount[player]; i++)
    {
      if (state->deck[player][i] == card) count++;
    }

  for (i = 0; i < state->handCount[player]; i++)
    {
      if (state->hand[player][i] == card) count++;
    }

  for (i = 0; i < state->discardCount[player]; i++)
    {
      if (state->discard[player][i] == card) count++;
    }

  return count;
}

int endTurn(struct gameState *state) {
  int k;
  int i;
  int currentPlayer = state->whoseTurn;

  //Discard hand
  for (i = 0; i < state->handCount[currentPlayer]; i++){
    state->discard[currentPlayer][state->discardCount[currentPlayer]++] = state->hand[currentPlayer][i];//Discard
    state->hand[currentPlayer][i] = -1;//Set card to -1
  }
  state->handCount[currentPlayer] = 0;//Reset hand count

  //Code for determining the player
  if (currentPlayer < (state->numPlayers - 1)){
    state->whoseTurn = currentPlayer + 1;//Still safe to increment
  } else {
    state->whoseTurn = 0;//Max player has been reached, loop back around to player 1
  }

  state->outpostPlayed = 0;
  state->phase = 0;
  state->numActions = 1;
  state->coins = 0;
  state->numBuys = 1;
  state->playedCardCount = 0;
  state->handCount[state->whoseTurn] = 0;

  //int k; move to top
  //Next player draws hand
  for (k = 0; k < 5; k++){
    drawCard(state->whoseTurn, state);//Draw a card
  }

  //Update money
  updateCoins(state->whoseTurn, state , 0);

  return 0;
}

int isGameOver(struct gameState *state) {
  int i;
  int j;

  //if stack of Province cards is empty, the game ends
  if (state->supplyCount[province] == 0){
    return 1;
  }

  //if three supply pile are at 0, the game ends
  j = 0;
  for (i = 0; i < CARD_COUNT; i++) {
    if (state->supplyCount[i] == 0)	{
	     j++;
       if (j >= 3){
        return 1;
      }
	  }
  }

  return 0;
}

int scoreFor (int player, struct gameState *state) {

  int i;
  int score = 0;
  //score from hand
  for (i = 0; i < state->handCount[player]; i++)
    {
      if (state->hand[player][i] == curse) { score = score - 1; };
      if (state->hand[player][i] == estate) { score = score + 1; };
      if (state->hand[player][i] == duchy) { score = score + 3; };
      if (state->hand[player][i] == province) { score = score + 6; };
      if (state->hand[player][i] == great_hall) { score = score + 1; };
      if (state->hand[player][i] == gardens) { score = score + ( fullDeckCount(player, 0, state) / 10 ); };
    }

  //score from discard
  for (i = 0; i < state->discardCount[player]; i++)
    {
      if (state->discard[player][i] == curse) { score = score - 1; };
      if (state->discard[player][i] == estate) { score = score + 1; };
      if (state->discard[player][i] == duchy) { score = score + 3; };
      if (state->discard[player][i] == province) { score = score + 6; };
      if (state->discard[player][i] == great_hall) { score = score + 1; };
      if (state->discard[player][i] == gardens) { score = score + ( fullDeckCount(player, 0, state) / 10 ); };
    }

  //score from deck
  for (i = 0; i < state->discardCount[player]; i++)
    {
      if (state->deck[player][i] == curse) { score = score - 1; };
      if (state->deck[player][i] == estate) { score = score + 1; };
      if (state->deck[player][i] == duchy) { score = score + 3; };
      if (state->deck[player][i] == province) { score = score + 6; };
      if (state->deck[player][i] == great_hall) { score = score + 1; };
      if (state->deck[player][i] == gardens) { score = score + ( fullDeckCount(player, 0, state) / 10 ); };
    }

  return score;
}

int getWinners(int players[MAX_PLAYERS], struct gameState *state) {
  int i;
  int j;
  int highScore;
  int currentPlayer;

  //get score for each player
  for (i = 0; i < MAX_PLAYERS; i++) {
    //set unused player scores to -9999
    if (i >= state->numPlayers) {
      players[i] = -9999;
    } else {
      players[i] = scoreFor(i, state);
    }
  }

  //find highest score
  j = 0;
  for (i = 0; i < MAX_PLAYERS; i++) {
    if (players[i] > players[j]) {
      j = i;
    }
  }
  highScore = players[j];

  //add 1 to players who had less turns
  currentPlayer = state->whoseTurn;
  for (i = 0; i < MAX_PLAYERS; i++){
    if (players[i] == highScore && i > currentPlayer) {
      players[i]++;
    }
  }

  //find new highest score
  j = 0;
  for (i = 0; i < MAX_PLAYERS; i++) {
    if ( players[i] > players[j] ){
      j = i;
	  }
  }
  highScore = players[j];

  //set winners in array to 1 and rest to 0
  for (i = 0; i < MAX_PLAYERS; i++) {
    if ( players[i] == highScore ){
      players[i] = 1;
    } else {
      players[i] = 0;
    }
  }

  return 0;
}

int drawCard(int player, struct gameState *state) {
  if (state->deckCount[player] <= 0){//Deck is empty
    //Move discard to deck
    for (int i = 0; i < state->discardCount[player]; i++){
      state->deck[player][i] = state->discard[player][i];
      state->discard[player][i] = -1;
    }

    state->deckCount[player] = state->discardCount[player];
    state->discardCount[player] = 0;//Reset discard

    //Shuffle the deck
    shuffle(player, state);

    if (DEBUG) {  //Debug statements
      printf("Deck count now: %d\n", state->deckCount[player]);
    }
  }

  int count = state->handCount[player];//Get current hand count for player
  int deckCounter;
  if (DEBUG){//Debug statements
    printf("Current hand count: %d\n", count);
  }

  deckCounter = state->deckCount[player];//Create holder for the deck count
  state->hand[player][count] = state->deck[player][deckCounter - 1];//Add card to the hand
  state->deckCount[player]--;
  state->handCount[player]++;//Increment hand count

  return 0;
}



int whoseTurn(struct gameState *state) {
	  return state->whoseTurn;
}


int getCost(int cardNumber)
{
  switch( cardNumber )
    {
    case curse:
      return 0;
    case estate:
      return 2;
    case duchy:
      return 5;
    case province:
      return 8;
    case copper:
      return 0;
    case silver:
      return 3;
    case gold:
      return 6;
    case adventurer:
      return 6;
    case council_room:
      return 5;
    case feast:
      return 4;
    case gardens:
      return 4;
    case mine:
      return 5;
    case remodel:
      return 4;
    case smithy:
      return 4;
    case village:
      return 3;
    case baron:
      return 4;
    case great_hall:
      return 3;
    case minion:
      return 5;
    case steward:
      return 3;
    case tribute:
      return 5;
    case ambassador:
      return 3;
    case cutpurse:
      return 4;
    case embargo:
      return 2;
    case outpost:
      return 5;
    case salvager:
      return 4;
    case sea_hag:
      return 4;
    case treasure_map:
      return 4;
    }

  return -1;
}

int cardEffect(int card, int choice1, int choice2, int choice3, struct gameState *state, int handPos, int *bonus)
{
  int i;
  int j;
  int k;
  int x;
  int index;
  int currentPlayer = state->whoseTurn;
  int nextPlayer = currentPlayer + 1;

  int temphand[MAX_HAND];// moved above the if statement
  int drawntreasure=0;
  int cardDrawn;
  int z = 0;// this is the counter for the temp hand
  if (nextPlayer > (state->numPlayers - 1)){
    nextPlayer = 0;
  }


  //uses switch to select card and perform actions
  switch( card )
    {
    case adventurer:
      while(drawntreasure<2){
	if (state->deckCount[currentPlayer] <1){//if the deck is empty we need to shuffle discard and add to deck
	  shuffle(currentPlayer, state);
	}
	drawCard(currentPlayer, state);
	cardDrawn = state->hand[currentPlayer][state->handCount[currentPlayer]-1];//top card of hand is most recently drawn card.
	if (cardDrawn == copper || cardDrawn == silver || cardDrawn == gold)
	  drawntreasure++;
	else{
	  temphand[z]=cardDrawn;
	  state->handCount[currentPlayer]--; //this should just remove the top card (the most recently drawn one).
	  z++;
	}
      }
      while(z-1>=0){
	state->discard[currentPlayer][state->discardCount[currentPlayer]++]=temphand[z-1]; // discard all cards in play that have been drawn
	z=z-1;
      }
      return 0;

    case council_room:
      //+4 Cards
      for (i = 0; i < 4; i++)
	{
	  drawCard(currentPlayer, state);
	}

      //+1 Buy
      state->numBuys++;

      //Each other player draws a card
      for (i = 0; i < state->numPlayers; i++)
	{
	  if ( i != currentPlayer )
	    {
	      drawCard(i, state);
	    }
	}

      //put played card in played card pile
      discardCard(handPos, currentPlayer, state, 0);

      return 0;

    case feast:
      //gain card with cost up to 5
      //Backup hand
      for (i = 0; i <= state->handCount[currentPlayer]; i++){
	temphand[i] = state->hand[currentPlayer][i];//Backup card
	state->hand[currentPlayer][i] = -1;//Set to nothing
      }
      //Backup hand

      //Update Coins for Buy
      updateCoins(currentPlayer, state, 5);
      x = 1;//Condition to loop on
      while( x == 1) {//Buy one card
	if (supplyCount(choice1, state) <= 0){
	  if (DEBUG)
	    printf("None of that card left, sorry!\n");

	  if (DEBUG){
	    printf("Cards Left: %d\n", supplyCount(choice1, state));
	  }
	}
	else if (state->coins < getCost(choice1)){
	  printf("That card is too expensive!\n");

	  if (DEBUG){
	    printf("Coins: %d < %d\n", state->coins, getCost(choice1));
	  }
	}
	else{

	  if (DEBUG){
	    printf("Deck Count: %d\n", state->handCount[currentPlayer] + state->deckCount[currentPlayer] + state->discardCount[currentPlayer]);
	  }

	  gainCard(choice1, state, 0, currentPlayer);//Gain the card
	  x = 0;//No more buying cards

	  if (DEBUG){
	    printf("Deck Count: %d\n", state->handCount[currentPlayer] + state->deckCount[currentPlayer] + state->discardCount[currentPlayer]);
	  }

	}
      }

      //Reset Hand
      for (i = 0; i <= state->handCount[currentPlayer]; i++){
	state->hand[currentPlayer][i] = temphand[i];
	temphand[i] = -1;
      }
      //Reset Hand

      return 0;

    case gardens:
      return -1;

    case mine:
      return mineEffect(handPos, choice1, choice2, state);

    case remodel:
      j = state->hand[currentPlayer][choice1];  //store card we will trash

      if ( (getCost(state->hand[currentPlayer][choice1]) + 2) > getCost(choice2) )
	{
	  return -1;
	}

      gainCard(choice2, state, 0, currentPlayer);

      //discard card from hand
      discardCard(handPos, currentPlayer, state, 0);

      //discard trashed card
      for (i = 0; i < state->handCount[currentPlayer]; i++)
	{
	  if (state->hand[currentPlayer][i] == j)
	    {
	      discardCard(i, currentPlayer, state, 0);
	      break;
	    }
	}


      return 0;

    case smithy:
      //+3 Cards
      for (i = 0; i < 3; i++)
	{
	  drawCard(currentPlayer, state);
	}

      //discard card from hand
      discardCard(handPos, currentPlayer, state, 0);
      return 0;

    case village:
      //+1 Card
      drawCard(currentPlayer, state);

      //+2 Actions
      state->numActions = state->numActions + 2;

      //discard played card from hand
      discardCard(handPos, currentPlayer, state, 0);
      return 0;

    case baron:
      return baronEffect(handPos, choice1, state);

    case great_hall:
      //+1 Card
      drawCard(currentPlayer, state);

      //+1 Actions
      state->numActions++;

      //discard card from hand
      discardCard(handPos, currentPlayer, state, 0);
      return 0;

    case minion:
      return minionEffect(handPos, choice1, state);

    case steward:
      if (choice1 == 1)
	{
	  //+2 cards
	  drawCard(currentPlayer, state);
	  drawCard(currentPlayer, state);
	}
      else if (choice1 == 2)
	{
	  //+2 coins
	  state->coins = state->coins + 2;
	}
      else
	{
	  //trash 2 cards in hand
	  discardCard(choice2, currentPlayer, state, 1);
	  discardCard(choice3, currentPlayer, state, 1);
	}

      //discard card from hand
      discardCard(handPos, currentPlayer, state, 0);
      return 0;

    case tribute:
      return tributeEffect(handPos, state);

    case ambassador:
      return ambassadorEffect(handPos, choice1, choice2, state);

    case cutpurse:
      updateCoins(currentPlayer, state, 2);
      for (i = 0; i < state->numPlayers; i++)	{
	       if (i != currentPlayer) {
	          for (j = 0; j < state->handCount[i]; j++)	{
		            if (state->hand[i][j] == copper) {
		                discardCard(j, i, state, 0);
		                break;
		            }
		            if (j == state->handCount[i]) {
		                for (k = 0; k < state->handCount[i]; k++)	{
			                   if (DEBUG)
			                      printf("Player %d reveals card number %d\n", i, state->hand[i][k]);
			              }
		                break;
		            }
		        }
	       }
	    }

      //discard played card from hand
      discardCard(handPos, currentPlayer, state, 0);

      return 0;


    case embargo:
      //+2 Coins
      state->coins = state->coins + 2;

      //see if selected pile is in play
      if ( state->supplyCount[choice1] == -1 )
	{
	  return -1;
	}

      //add embargo token to selected supply pile
      state->embargoTokens[choice1]++;

      //trash card
      discardCard(handPos, currentPlayer, state, 1);
      return 0;

    case outpost:
      //set outpost flag
      state->outpostPlayed++;

      //discard card
      discardCard(handPos, currentPlayer, state, 0);
      return 0;

    case salvager:
      //+1 buy
      state->numBuys++;

      if (choice1) {
    	  //gain coins equal to trashed card
    	  state->coins = state->coins + getCost( handCard(choice1, state) );
    	  //trash card
    	  discardCard(choice1, currentPlayer, state, 1);
	    }

      //discard card
      discardCard(handPos, currentPlayer, state, 0);
      return 0;

    case sea_hag:
      for (i = 0; i < state->numPlayers; i++){
	if (i != currentPlayer){
	  state->discard[i][state->discardCount[i]] = state->deck[i][state->deckCount[i]--];
    state->deckCount[i]--;
	  state->discardCount[i]++;
	  state->deck[i][state->deckCount[i]--] = curse;//Top card now a curse
	}
      }
      return 0;

    case treasure_map:
      //search hand for another treasure_map
      index = -1;
      for (i = 0; i < state->handCount[currentPlayer]; i++)
	{
	  if (state->hand[currentPlayer][i] == treasure_map && i != handPos)
	    {
	      index = i;
	      break;
	    }
	}
      if (index > -1)
	{
	  //trash both treasure cards
	  discardCard(handPos, currentPlayer, state, 1);
	  discardCard(index, currentPlayer, state, 1);

	  //gain 4 Gold cards
	  for (i = 0; i < 4; i++)
	    {
	      gainCard(gold, state, 1, currentPlayer);
	    }

	  //return success
	  return 1;
	}

      //no second treasure_map found in hand
      return -1;
    }

  return -1;
}


int discardCard(int handPos, int currentPlayer, struct gameState *state, int trashFlag)
{
  //if card is not trashed, added to Played pile
  if (!trashFlag) {
    //add card to discard pile
    state->discard[currentPlayer][state->discardCount[currentPlayer]] = state->hand[currentPlayer][handPos];
    state->discardCount[currentPlayer]++;
  }

  if (handPos != (state->handCount[currentPlayer] - 1)) { //if there are cards later in the hand
    //replace discarded card with last card in hand
    state->hand[currentPlayer][handPos] = state->hand[currentPlayer][state->handCount[currentPlayer] - 1];
    //set last card to -1
    state->hand[currentPlayer][state->handCount[currentPlayer] - 1] = -1;
  } else {
    //set played card to -1
    state->hand[currentPlayer][handPos] = -1;
  }
  state->handCount[currentPlayer]--;

  return 0;
}


int gainCard(int supplyPos, struct gameState *state, int toFlag, int player)
{
  //Note: supplyPos is enum of choosen card

  //check if supply pile is empty (0) or card is not used in game (-1)
  if (supplyCount(supplyPos, state) < 1)
    {
      return -1;
    }

  //added card for [whoseTurn] current player:
  // toFlag = 0 : add to discard
  // toFlag = 1 : add to deck
  // toFlag = 2 : add to hand

  if (toFlag == 1)
    {
      state->deck[player][state->deckCount[player]] = supplyPos;
      state->deckCount[player]++;
    }
  else if (toFlag == 2)
    {
      state->hand[player][state->handCount[player]] = supplyPos;
      state->handCount[player]++;
    }
  else
    {
      state->discard[player][state->discardCount[player]] = supplyPos;
      state->discardCount[player]++;
    }

  //decrease number in supply pile
  state->supplyCount[supplyPos]--;

  return 0;
}


int updateCoins(int player, struct gameState *state, int bonus)
{
  int i;

  //reset coin count
  state->coins = 0;

  //add coins for each Treasure card in player's hand
  for (i = 0; i < state->handCount[player]; i++) {
        if (state->hand[player][i] == copper)
  	{
  	  state->coins += 1;
  	}
        else if (state->hand[player][i] == silver)
  	{
  	  state->coins += 2;
  	}
        else if (state->hand[player][i] == gold)
  	{
  	  state->coins += 3;
  	}
  }

  //add bonus
  state->coins += bonus;

  return 0;
}




int ambassadorEffect(int handPos, int choicePos, int numCopies, struct gameState* state) {
  int currentPlayer = state->whoseTurn;
  int revealedCard = state->hand[currentPlayer][choicePos];
  //if the number of cards they try to discard is not 0, 1, or 2
  if (numCopies > 2 || numCopies < 0) {
    return -1;
  }

  //count the # of occurences of the chosen card
  int j = 0;
  for (int i = 0; i < state->handCount[currentPlayer]; i++) {
    if (i != handPos && state->hand[currentPlayer][i] == revealedCard) {   //removed && i != choice1
      j++;
    }
  }

  //used to check if player has enough cards to discard
  if (j < numCopies) {
    return -2;
  }

  if (DEBUG)
    printf("Player %d reveals card number: %d\n", currentPlayer, revealedCard);

  //increase supply count for chosen card by amount being discarded
  state->supplyCount[revealedCard] += numCopies;

  //each other player gains a copy of revealed card starting left and going around
  int nextPlayer;
  if (currentPlayer == (state->numPlayers - 1)){
    nextPlayer = 0;
  } else {
    nextPlayer = currentPlayer + 1;
  }
  while (nextPlayer != currentPlayer) {
    gainCard(revealedCard, state, 0, nextPlayer);
    nextPlayer++;
    if (nextPlayer > (state->numPlayers - 1)){
      nextPlayer = 0;
    }
  }

  //discard played card from hand
  discardCard(handPos, currentPlayer, state, 0);

  //remove copies of cards returned to supply, but do not put in discard
  j = 0;
  for (int i = 0; i < state->handCount[currentPlayer] && j < numCopies; i++) {
    if (state->hand[currentPlayer][i] == revealedCard) {
      discardCard(i, currentPlayer, state, 1);
      j++;
    }
  }

  return 0;
}


int baronEffect(int handPos, int discardEstate, struct gameState* state) {
  //discard card from hand
  int currentPlayer = state->whoseTurn;
  state->numBuys++;//Increase buys by 1!
  discardCard(handPos, currentPlayer, state, 0);
  if (discardEstate) {   //Boolean for going to discard an estate (choice1 should be > 0)
    int p = 0;//Iterator for hand!
    int card_not_discarded = 1;//Flag for discard set!
    while(card_not_discarded) {
      //if we've searched the whole hand
      if (p >= state->handCount[currentPlayer]){
        if(DEBUG) {
          printf("No estate cards in your hand, invalid choice\n");
          printf("Must gain an estate if there are any\n");
        }
        gainCard(estate, state, 0, currentPlayer);
        card_not_discarded = 0;
      } else if (state->hand[currentPlayer][p] == estate){//Found an estate card!
        state->coins += 4;//Add 4 coins to the amount of coins
        //Discard the estate card
        discardCard(p, currentPlayer, state, 0);
        card_not_discarded = 0;//Exit the loop

      } else {
        p++;//Next card
      }
    }
  } else {
    gainCard(estate, state, 0, currentPlayer);//Gain an estate
  }

  return 0;
}


int mineEffect(int handPos, int trashPos, int tradeCard, struct gameState* state) {
  int currentPlayer = state->whoseTurn;
  int trashCard = state->hand[currentPlayer][trashPos];  //store card we will trash
  //discard mine card from hand
  discardCard(handPos, currentPlayer, state, 0);

  //make sure both cards are treasure cards
  if (trashCard < copper || trashCard > gold || tradeCard < copper || tradeCard > gold) {
    return -1;
  }

  //Make sure the trade card does not cost more than 3 more than trashcard
  if (tradeCard - trashCard > 3) {
    return -1;
  }

  //gain treasure card to hand
  gainCard(tradeCard, state, 2, currentPlayer);

  //find and trash original choice card
  discardCard(trashPos, currentPlayer, state, 1);

  return 0;
}


int minionEffect(int handPos, int cardOption, struct gameState* state) {
  int currentPlayer = state->whoseTurn;
  //+1 action
  state->numActions++;
  if (cardOption > 1) {   //boolean for choice of card options
    //+2 coins
    state->coins += 2;
  } else {
    //players discard hand and redraw if hand size > 4
    for (int i = 0; i < state->numPlayers; i++) {
      if (i == currentPlayer || state->handCount[i] >= 5) {
        //if it is the currentPlayer or anyone else with 5+ cards
        //discard first card in hand until empty
        while(state->handCount[i] > 0) {
          discardCard(0, i, state, 0);
        }
        //draw 4
        for (int j = 0; j < 4; j++) {
          drawCard(i, state);
        }
      }
    }
  }
  //discard card from hand
  discardCard(handPos, currentPlayer, state, 0);
  return 0;
}


int tributeEffect(int handPos, struct gameState* state) {
  int currentPlayer = state->whoseTurn;
  int nextPlayer = currentPlayer + 1;
  if (nextPlayer >= state->numPlayers){
    nextPlayer = 0;
  }
  int tributeRevealedCards[2] = {-1, -1};
  //discard the tribute card
  discardCard(handPos, currentPlayer, state, 0);

  //if the next player has any cards
  if (state->deckCount[nextPlayer] > 0 || state->discardCount[nextPlayer] > 0){
    int newCardPos;
    for (int i = 0; i < 2; i++) {
      drawCard(nextPlayer, state);
      newCardPos = state->handCount[nextPlayer] - 1;
      tributeRevealedCards[i] = state->hand[nextPlayer][newCardPos];
      if (DEBUG) {
        printf("Player %d revealed a %d\n", nextPlayer, tributeRevealedCards[i]);
      }
      discardCard(newCardPos, nextPlayer, state, 0);
    }
  } else {
    //No Card to Reveal
    if (DEBUG) {
      printf("No cards to reveal\n");
    }
  }

  if (tributeRevealedCards[0] == tributeRevealedCards[1]){//If we have a duplicate card, just drop one
    tributeRevealedCards[1] = -1;
  }

  for (int i = 0; i < 2; i++){
    if (tributeRevealedCards[i] == -1){
      break;
    }
    if (tributeRevealedCards[i] == copper || tributeRevealedCards[i] == silver || tributeRevealedCards[i] == gold){//Treasure cards
      state->coins += 2;
    } else if ((tributeRevealedCards[i] >= estate && tributeRevealedCards[i] <= province) || tributeRevealedCards[i] == gardens || tributeRevealedCards[i] == great_hall){//Victory Card Found
      drawCard(currentPlayer, state);
      drawCard(currentPlayer, state);
      //state->handCount[currentPlayer] += 2;
    } else if (tributeRevealedCards[i] == curse) {
      continue;
    } else {//Action Card
      state->numActions += 2;
    }
  }

  return 0;
}

//end of dominion.c
