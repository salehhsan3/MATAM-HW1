
#ifndef TOURNAMENT_DATA_H_
#define TOURNAMENT_DATA_H_
#include <string.h>
#include "games.h"
#include "chessSystem.h"
#include "map.h"



/*enum for the status of a tournament*/
typedef enum tournament_status_t{
    ENDED,
    GOING
}TournamentStatus;


/*a struct to keep the data about a tournament*/
typedef struct tournament_data_t* TournamentData;


/*copy tha data of a tournament and return it
error type: NULL in case of allocation fail*/
MapDataElement copyTournamentData(MapDataElement tournament_data);

/*free the data of the tournament*/
void freeTournamentData(MapDataElement data);

/*create a new data for a new tournaments and fill it
error type: NULL in case of allocation fail
otherwise return data*/
TournamentData createTournamentData(char* tournament_location,int max_games_per_player);

/*check the status of a tournament and return it
return value: ENDED if finished
              GOING if not finished*/
TournamentStatus checkTournamentStatus(TournamentData data);

/*return the value of the max_games_per_player*/
int findGameLimitForPlayerInTournament(TournamentData data);

/*return the first game in the tournament
error type: return NULL if there are no games yet in the tournament*/
Game getGame(TournamentData tournament_data);


/*add a game betweem two players to a certain tournament
error type: false if allocation failed
otherwise return true*/
bool addGameToTournament(TournamentData tournament, int tournament_id, int first_player, 
                                int second_player, Winner winner, int play_time);


/*check if a game allready exist in the tournament and return truth value accordingly*/
bool checkIfGameExistInTournament(TournamentData tournament_data,int first_player,int second_player);


/*used in chessRemove: if a game of the players exist in a specific ongoing tournament,it will update the 
game after the player with the player_id is removed*/
void updateGameBetweenPlayersIfExist(TournamentData data,int player_id,int other_player_id,int *win,
                                                int *lose,int *draw,bool* is_player_in_tournament);


/*when we remove a player from the system he might participate in a game in which the other
player was removed, this function will remove him from games as such (if they exist)*/
void updateGameWhereOtherPlayerWasRemoved(TournamentData data,int player_id,int removed_player_mark,
                                                                            bool* is_playing_solo);

/*check if there are no games in the tournament
return value: true if there are no games
false otherwise*/
bool checkIfTournamentIsEmpty(TournamentData tournaments_list);

/*calculate the total time the player played in a tournament (if he played)*/
int calculatePlayerTimeInTournament(TournamentData data,int player_id);

/*give the location of the tournament*/
char* getTournamentLocation(TournamentData data);

/*return the amount of the games in the tournament*/
int getNumberOfGamesInTournament(TournamentData data);

/*get the longest game in a certain tournament*/
int getLongestGame(TournamentData data);

/*return the average play time in a tournament (amount of games/amount of time) */
double calculateAverageGameTimeInTournament(TournamentData data);

/*update the amount of players that got removed from the tournament*/
void updateRemovedTournamentData(TournamentData data);

/*return the amount of the player that got removed from the tournament*/
int getRemovedPlayersAmountFromTournament(TournamentData data);

/*update the data about the tournament regarding the winner and the status*/
void updateTournametStatusToEndedAndUpdateWinner(int winner_id,TournamentData data);

#endif /* TOURNAMENT_DATA_H_ */