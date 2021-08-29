
#ifndef PLAYER_TRACK_H_
#define PLAYER_TRACK_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>


/*struck in order to keep track about information relative to a palyer
(wins,loses,ties,in which tournaments he played)*/
typedef struct track_player_t* TrackPlayer;

/*destroy the information about the accomplishments of the player*/
void destroyPlayerTrack(TrackPlayer player_track);

/*return the number of games played in a specipic tournament by a specific player*/
int findNumberOfGamesPlayedInTournament( TrackPlayer player_track, int tournament_id);

/*fill the TrackPlayer of a player that is inserted in the system *for the first time* */
void fillTrackPlayer(TrackPlayer track,int tournament_id,int wins,int loses, int draws,
                                                                   int games_played,TrackPlayer next);

/*check if a player allready participated in the tournament and return true or false accordingly*/
bool checkIfTournamentExistInTrack(TrackPlayer track,int tournament_id);

/*add a tournament to an unempty player_track, which didn't existed in it before, at the end
of the list.
error type:false if allocation failed
otherwise return true*/
bool addTornamentToNotEmptyTrack(TrackPlayer track,int tournament_id,int win,int lose,int draw);

/*update the information of the player in a certain tournament*/
bool updateTournamentInTrack(TrackPlayer track,int tournament_id,int win,int lose,int draw);

/*remove the information about a player related to a certain tournament
-if we call this function we know there is information to remove*/
TrackPlayer freePlayerTrackOfCertainTournament(TrackPlayer track,int tournament_id,bool* is_first_changed);


/*update the statictics after a auto win in a certain tournament*/
void updatePlayerTrackAfterAutoWin(int tournament_id,int win,int lose,int draw,TrackPlayer player_track);

/*calculate the score of a certain player in a tournament*/
int calculateScore(int tournament_id,TrackPlayer track);

/*fill the amount of wins and loses a player have in a certain PlayerTrack*/
void fillWinLoseForPlayer(TrackPlayer track,int* current_wins,int* current_loses,int tournament_id);

/* a function to get the number of games in order to get the number of games played by a player
in a chess system */
int getAmountOfGamesPlayed(TrackPlayer track);

/*calculate the player level and return it
use this function only if you know that a player played at least one game*/
double calculatePlayerLevel(TrackPlayer track);

/*allocate a memory for a new track, may return NULL if failed*/
TrackPlayer allocateMemoryForTrack();


#endif /* PLAYER_TRACK_H */