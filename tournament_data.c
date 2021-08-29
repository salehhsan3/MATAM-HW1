
#include "tournament_data.h"
#include "games.h"

struct tournament_data_t
{
    char* location;
    int max_games_per_player;
    Game game;
    TournamentStatus tournament_status;
    int winner_id;
    int amount_removed_players;
};


MapDataElement copyTournamentData(MapDataElement tournament_data)
{
    if(tournament_data==NULL)
    {
        return NULL;
    }
    TournamentData data=(TournamentData)tournament_data;
    TournamentData copy_data=malloc(sizeof(*copy_data));
    if(!copy_data)
    {
        return NULL;
    }
    copy_data->location=malloc(strlen((char*)data->location)+1);
    if(copy_data->location==NULL)
    {
        free(copy_data);
        copy_data=NULL;
        return NULL;
    }
    strcpy((char*)copy_data->location,data->location);  
    copy_data->max_games_per_player=data->max_games_per_player;
    copy_data->winner_id=data->winner_id;
    copy_data->amount_removed_players=data->amount_removed_players;
    copy_data->tournament_status=data->tournament_status;
    bool is_copied_game=false;
    Game first_game=copyAllGamesInData(data->game,&is_copied_game);
    copy_data->game=first_game;
    if(copy_data->game==NULL) //could be from two reason- nothing to copy, or allocation failed
    {
        if(is_copied_game==false) //the case is allocation failed
        {
            free((char*)copy_data->location);
            copy_data->location=NULL;
            free(copy_data);
            copy_data=NULL;
            return NULL;
        }
    }
    return (MapDataElement)(copy_data);  
}

void freeTournamentData(MapDataElement data)
{
    if(data==NULL)
    {
        return;
    }
    TournamentData tournament_data=(TournamentData)data;
    free((char*)tournament_data->location);   
    tournament_data->location=NULL;
    freeGame(tournament_data->game);
    tournament_data->game=NULL;
    free(data);
    data=NULL;
    return;
}

TournamentData createTournamentData(char* tournament_location,int max_games_per_player)
{
    assert((tournament_location!=NULL) && (max_games_per_player>0) );
    TournamentData data=malloc(sizeof(*data));
    if(!data)
    {
        return NULL;
    }
    data->location=malloc(strlen(tournament_location)+1);
    if(!(data->location))
    {
        free(data);
        data=NULL;
        return NULL;
    }
    strcpy((char*)data->location,tournament_location); 
    data->max_games_per_player=max_games_per_player;
    data->tournament_status=GOING;
    data->amount_removed_players=0;
    data->winner_id=0; //there is no winner, 0 is invalid id
    data->game=NULL; //there are no games yet
    return data;
}

TournamentStatus checkTournamentStatus(TournamentData data)
{
    return data->tournament_status;
}

int findGameLimitForPlayerInTournament(TournamentData data)
{
    return data->max_games_per_player;
}

Game getGame(TournamentData tournament_data)
{
    return (tournament_data->game);
}

bool addGameToTournament(TournamentData tournament, int tournament_id, int first_player, 
                                int second_player, Winner winner, int play_time)
{
    if(tournament->game == NULL)// there are no games yet in the tournament
    {
        Game new_game=allocateMemoryForGame();
        tournament->game = new_game;       
        if(!(tournament->game))
        {
            return false;
        }
        fillGame(tournament->game,first_player,second_player,winner,play_time,NULL);
        return true;
    }
    return addGameAtEnd(tournament->game,tournament_id,first_player, second_player,winner,play_time);
}




bool checkIfGameExistInTournament(TournamentData tournament_data,int first_player,int second_player)
{
    Game first_game = getGame(tournament_data);
    if(first_game == NULL) //there are no games at all in the tournament
    {
        return false;
    }
    Game current = first_game;
    while(current != NULL)
    {
        if(CheckIfPlayerIdMatch(current,first_player,second_player) )
        {
            return true;
        }
        current = advanceGame(current);
    }
    return false; 
}


void updateGameBetweenPlayersIfExist(TournamentData data,int player_id,int other_player_id,int *win,
                                                        int *lose,int *draw,bool* is_player_in_tournament)
{
    checkIfGameBetweemTwoExistAndUpdate(data->game,player_id,other_player_id,win,lose,draw,
                                                                    is_player_in_tournament);
    return;
}

void updateGameWhereOtherPlayerWasRemoved(TournamentData data,int player_id,int removed_player_mark,
                                                                                bool* is_playing_solo)
{
   checkIfGameWithOnlyPlayerExistAndRemoveIt(data->game,player_id,removed_player_mark,is_playing_solo);
   return;
}

bool checkIfTournamentIsEmpty(TournamentData tournaments_list)
{
    if(tournaments_list->game == NULL)
    {
        return true;
    }
    return false;
}

int calculatePlayerTimeInTournament(TournamentData data,int player_id)
{
    if(data->game == NULL) //there are no games so the player didn't play
    {
        return 0;
    }
    return calculatePlayerTimeInTournamentGames(data->game,player_id);
}

char* getTournamentLocation(TournamentData data)
{
    return data->location;
}

int getNumberOfGamesInTournament(TournamentData data)
{
    return getNumberOfGames(data->game);
}

int getLongestGame(TournamentData data)
{
    return getLongestGameTime(data->game);
}

double calculateAverageGameTimeInTournament(TournamentData data)
{
    return calculateAverageGamesTime(data->game);
}

void updateRemovedTournamentData(TournamentData data)
{
    data->amount_removed_players=(data->amount_removed_players)+1;
    return;
}

int getRemovedPlayersAmountFromTournament(TournamentData data)
{
    return (data->amount_removed_players);
}

void updateTournametStatusToEndedAndUpdateWinner(int winner_id,TournamentData data)
{
    data->tournament_status=ENDED;
    data->winner_id=winner_id;
    return;
}