
#ifndef PLAYERS_H_
#define PLAYERS_H_

#include "chessSystem.h"
#include "player_track.h"
#include "games.h"
#include <stdbool.h>
#include <stdio.h>

/*struct that will contain all the players in the system including removed players*/
typedef struct players_container_t *PlayersContainer;

/*struck of a player in the system in order to save his id and the information regarding the tournament
he played in (tournament id, wins, lose and ties in the tournament for the player and how many games
he played in the tournament)*/
typedef struct player_t *Player;

/*create an empty players list
error type: NULL in case of allocation fail
otherwise return the list*/
PlayersContainer createPlayerList();

/*destroy the list and erase all the players from the system*/
void destroyPlayerList(PlayersContainer players_list);

/*check if the player has reached the limit of the amount of games he can play in the tournament
and return TRUE or FALSE accordingly*/
bool checkIfPlayerHasExcedeedMaxNumberOfGamesAllowedInTournament(PlayersContainer player_list,
                                                        int num_game_limit,int tournament_id,int player_id);


/*after a game is added, this function is used to update the information about the players in the system
return true is success and false otherwise*/
bool updatePlayersList(PlayersContainer players_list,int first_player,int second_player,Winner winner,
                                                                                            int tournament_id);


/*after a tournament is removed, this function remove all the information the system had about the players 
regarding the tournament*/
void removePlayersTournamentTrack(PlayersContainer players_list,int tournament_id); 

/*after a player was removed and a game was changed, this function update the statictics of the
player that wasn't removed*/
void updatePlayerTrackAfterGameChanged(PlayersContainer players_list,int tournament_id,int win,
                                                                  int lose,int draw,int player_id);

/*return the first player in the player list
if the list is empty-return NULL*/
Player getFirstPlayer(PlayersContainer players_list);

/*return the player next to current_player
if the list is empty-return NULL*/
Player getNextPlayer( Player current_player);


/*return the id of the player (we know he exist)*/
int getPlayerId(Player player);

/*remove all the information about the player after he was removed from all the games in the system*/
void removePlayerFromChessSystem(PlayersContainer players_list,int player_id);

/*determine the winner in the tournament and return his id
-we are guaranteed to have at least one player in an ended tournament*/
int determineTheTournamentWinner(int tournament_id,PlayersContainer players_list);

/*check is a player exist in the chess system and return truth value:
true if exist and false otherwise*/
bool checkIfPlayerExistsInSystem(PlayersContainer players_list,int player_id);

/*a function to get the number of games played a certain player in a chess system*/
int getNumberOfGamesPlayed(PlayersContainer players_list,int player_id);

/*return the amount of the players in the system which played in a certain tournament*/
int getNumberOfPlayersInTournament(PlayersContainer players_list, int id_tournament);


/*print the level for each player in chess who played games (there is at least one according 
to checks before calling this function)*/
void printPlayersLevels(PlayersContainer players_list,FILE* file);

/*a function that returns true if there are players with a valid id and false in 
case there are no players with a valid id (players with a valid id play in at least one game)*/
bool checkIfThereArePlayersToSave(PlayersContainer players_list);


/*print the level and the id of a player*/
void printPlayerIdAndLevel(int id,double level,FILE* file);


int calculateSameLevelAmount(PlayersContainer players_list,double printed_level);

/*after removing the first in the track this upadate the new first*/
void updateFirstInPlayerTrack(Player player,TrackPlayer new_first);

#endif /* PLAYERS_H_ */