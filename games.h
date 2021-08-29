
#ifndef GAME_H_
#define GAME_H_
#include "chessSystem.h"
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h> 


/*struct that include the information about the game and it's result*/
typedef struct game_t *Game;

/*make a copy of game
error type: NULL if NULL argument was sent or if allocation failed
otherwise return a copy
is_copied_game will tell us if we copied succesfully, if we have nothing to copy
it will count as success, it will only be false if allocation failed*/
Game copyAllGamesInData(Game game,bool* is_copied_game);

/*free the game*/
void freeGame(Game game);



/*fill the game*/
void fillGame(Game game,int first_id, int second_id, Winner result, int game_time,Game next_game);

/*at a game to an unempty list of games and put it last
error type: false in case of allocation fail
otherwise return true*/
bool addGameAtEnd(Game game,int tournament_id,int first_player, 
                                int second_player, Winner winner, int play_time);



/*before a player is removed from the tournament, if a game between the player and the other
player exist, update it that it will be known that player was removed and return the statictic
update for the second player via pointers*/
void checkIfGameBetweemTwoExistAndUpdate(Game game,int player_id,int other_player_id,int* win,
                                                            int* lose,int* draw,bool* is_player_in_tournament);


/*check if there is a game where player is registered but the other player was removed and update
the game so that player is also removed*/
void checkIfGameWithOnlyPlayerExistAndRemoveIt(Game game,int player_id,int removed_player_mark,
                                                                        bool* is_playing_solo);

/*calculate the whole time the player played in a certain (not empty) tournament (if he played)
and return the score*/
int calculatePlayerTimeInTournamentGames(Game first_game,int player_id);

/*return the anount of the games in a tournament*/
int getNumberOfGames(Game game);

/*returns the longest game time in a tournament*/
int getLongestGameTime(Game game);

/*calculate the average play time in the tournament (whole time/games amount) */
double calculateAverageGamesTime(Game game);

/*check if the player's id are of the players in the game*/
bool CheckIfPlayerIdMatch(Game game,int first_player, int second_player);

/*move to the next game and return it*/
Game advanceGame(Game game);

/*allocate a memory for a game and return it,if failed return NULL */
Game allocateMemoryForGame();

#endif /* GAME_H_ */
