
#include "chessSystem.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "map.h" 
#include "players.h"
#include "tournament_data.h"

#define BIG_LETTER_MIN 'A'
#define BIG_LETTER_MAX 'Z'
#define SMALL_LETTER_MAX 'z'
#define SMALL_LETTER_MIN 'a'
#define VALID_CHAR ' '
#define ERROR -1


/** Type for defining the tournaments */
typedef Map Tournaments;

/*a struct that will be used as a shell to the chess system and will include the tournaments and 
a list of the players in the system*/
struct chess_system_t{
    Tournaments tournaments_list;
    PlayersContainer players_list;
};


/*copy function for tournament id in order to sent to mapCreate to create a chess system*/
static MapKeyElement copyTournamentId (MapKeyElement id)
{
    if(!id)
    {
        return NULL;
    }
    int* copy=malloc(sizeof(*(copy)));
    if(!copy)
    {
        return NULL;
    }
    *copy=*((int*)id);
    return (MapKeyElement)copy;
}


/*compare function to compare between two tournaments id
return value:
positive value if the first_id is bigger
0 if equall
negative value if the second_id is bigger*/
static int compareTournamentsId (MapKeyElement first_id,MapKeyElement second_id) {
    return (*(int *) first_id - *(int *) second_id);
}

/*free function for tournament id in order to sent to mapCreate to create a chess system*/
static void freeTournamentId (MapKeyElement id)
{
    if(id==NULL)
    {
        return;
    }
    free(id);
}
/*check if the location is valid
return value: CHESS_SUCCESS  if valid
              CHESS_INVALID_LOCATION if invalid
we know the location isn't empty*/
static ChessResult checklocationValidity (const char* tournament_location)
{
    if ( *tournament_location < BIG_LETTER_MIN || *tournament_location > BIG_LETTER_MAX ) 
    {
        return CHESS_INVALID_LOCATION;
    }
    tournament_location++;

    while (*tournament_location)
    {
        if ( (*tournament_location < SMALL_LETTER_MIN || *tournament_location > SMALL_LETTER_MAX) &&
                                                            (*tournament_location != VALID_CHAR) )
        {
            return CHESS_INVALID_LOCATION;
        }
        tournament_location++;
    }
    return CHESS_SUCCESS;
}

/*check the arguments and return error if any invalid , otherwise success
error types:CHESS_NULL_ARGUMENT if any of the arguments is NULL 
            CHESS_INVALID_ID if the tournament_is is invalid
            CHESS_TOURNAMENT_ALREADY_EXISTS if there is a tournament in the system with the same id
            CHESS_INVALID_LOCATION if the name of the location is invalid
            CHESS_INVALID_MAX_GAMES if the value of max_games_per_player is invalid
used in the function chessAddTournament*/
static ChessResult checkArguments (ChessSystem chess, int tournament_id,
                                        int max_games_per_player, const char* tournament_location)
{
    if( (!chess) || (!tournament_location) )
    {
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if(mapContains(chess->tournaments_list,(MapKeyElement)(&tournament_id))==true)
    {
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    if (checklocationValidity(tournament_location) != CHESS_SUCCESS)
    {
        return CHESS_INVALID_LOCATION;
    }
    if (max_games_per_player <= 0)
    {
        return CHESS_INVALID_MAX_GAMES;
    }
    return CHESS_SUCCESS;   
}

/*check if the arguments of "chessAddGame" are invalid and return error values as written in 
the "chessAddGame" function's description*/
static ChessResult checkArgumentsOfAddGame (ChessSystem chess, int tournament_id, int first_player, 
                                int second_player, Winner winner, int play_time)
{
    if(!chess)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if( (tournament_id <= 0) || (first_player <= 0) || 
        (second_player <= 0) || (first_player == second_player) )
    {
        return CHESS_INVALID_ID;
    }
    if(winner != FIRST_PLAYER && winner != SECOND_PLAYER && winner != DRAW) 
    {
        return CHESS_INVALID_ID;
    }
    if(mapContains((chess->tournaments_list),(MapKeyElement)(&tournament_id))==false)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    TournamentData tournament_data=mapGet(chess->tournaments_list,(MapKeyElement)(&tournament_id));
    assert(tournament_data!=NULL);
    TournamentStatus tournament_status = checkTournamentStatus(tournament_data);
    if(tournament_status == ENDED)
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    if(checkIfGameExistInTournament(tournament_data,first_player,second_player))
    {
       return CHESS_GAME_ALREADY_EXISTS;
    }   
    if(play_time < 0)
    {
        return CHESS_INVALID_PLAY_TIME;
    }  
    int limit_of_games= findGameLimitForPlayerInTournament(tournament_data);
    if (((checkIfPlayerHasExcedeedMaxNumberOfGamesAllowedInTournament(chess->players_list,limit_of_games,
         tournament_id,first_player) )||
        ( checkIfPlayerHasExcedeedMaxNumberOfGamesAllowedInTournament(chess->players_list,limit_of_games,
                                                                              tournament_id,second_player)))) 
    {
        return CHESS_EXCEEDED_GAMES;
    }
    return CHESS_SUCCESS;  
}   

/*check the arguments for the "chessSaveTournamentStatistics" function 
error type: CHESS_NULL_ARGUMENT if chess or path_file are NULL
            CHESS_NO_TOURMENTS_ENDED if there are no tournaments that has ended
otherwise return CHESS_SUCCESS*/
static ChessResult checkArgumentsForSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
    if( (!chess) || (!path_file) )
    {
        return CHESS_NULL_ARGUMENT;
    }
    //int* tournamnt_id=NULL;
    bool is_tournament_ended = false;
    for(int* id_tournament=(int*)mapGetFirst(chess->tournaments_list);
    id_tournament!=NULL; id_tournament=(int*)(mapGetNext(chess->tournaments_list)))
    {
        TournamentData data=mapGet(chess->tournaments_list,(MapKeyElement)(id_tournament));
        if(checkTournamentStatus(data) == ENDED)
        {
            is_tournament_ended = true;
        }
        freeTournamentId(id_tournament);
    }
    if(is_tournament_ended == false)
    {
        return CHESS_NO_TOURNAMENTS_ENDED;  
    }
    return CHESS_SUCCESS;
}



/////////////////////////implementaion of the functions from chessSystem.h/////////////////

ChessSystem chessCreate()
{
    ChessSystem chess_system=malloc(sizeof(*chess_system));
    if(!chess_system)
    {
        return NULL;
    }
    Tournaments tournaments=mapCreate(copyTournamentData,copyTournamentId,freeTournamentData,
    freeTournamentId, compareTournamentsId);
    if(tournaments==NULL)
    {
        free(chess_system);
        return NULL;
    }
    chess_system->tournaments_list=tournaments;
    PlayersContainer players_list=createPlayerList(); 
    if(!players_list)
    {
        mapDestroy(tournaments);
        free(chess_system);
        return NULL;
    }
    chess_system->players_list = players_list;
    return chess_system;                                         
}

void chessDestroy(ChessSystem chess)
{
    if(chess==NULL)
    {
        return;
    }
    destroyPlayerList(chess->players_list);
    chess->players_list=NULL;
    mapDestroy(chess->tournaments_list);
    chess->tournaments_list=NULL;
    free(chess);
    chess=NULL;
    return;
}

ChessResult chessAddTournament(ChessSystem chess, int tournament_id,
                                        int max_games_per_player, const char* tournament_location)
{
    ChessResult result_arguments = checkArguments(chess,tournament_id,max_games_per_player,tournament_location);
    if(result_arguments != CHESS_SUCCESS)
    {
        return result_arguments;
    }
    TournamentData data=createTournamentData((char*)tournament_location,max_games_per_player);
    if(data == NULL)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    MapResult result=mapPut(chess->tournaments_list,(MapKeyElement)(&tournament_id), (MapDataElement)data);
    if(result==MAP_OUT_OF_MEMORY)
    {
        freeTournamentData(data); 
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    freeTournamentData(data); //the data was added successfully via mapPut, need to be freed. 
    assert(result == MAP_SUCCESS); 
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess,int tournament_id,int first_player, 
                                int second_player,Winner winner, int play_time)
{
    ChessResult result =  checkArgumentsOfAddGame(chess,tournament_id,first_player,
                                                     second_player,winner,play_time);
    if ( result != CHESS_SUCCESS )
    {
        return result;
    }
    TournamentData data=mapGet(chess->tournaments_list,( MapKeyElement)(&tournament_id)); //we know
                                                                                   //the tournament exist 
    bool add_game_result=addGameToTournament(data,tournament_id,first_player,second_player,winner,play_time);
    if(!add_game_result)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    bool update_result = updatePlayersList(chess->players_list,first_player,second_player,
                                                                                    winner,tournament_id);
    if(!update_result)
    {
        chessDestroy(chess);
        return CHESS_OUT_OF_MEMORY;
    }
    return CHESS_SUCCESS;
   
}

ChessResult chessRemoveTournament(ChessSystem chess, int tournament_id)
{
    if(!chess)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id<=0)
    {
        return CHESS_INVALID_ID;
    }
    if((mapContains(chess->tournaments_list,(MapKeyElement)(&tournament_id)))==false)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    mapRemove(chess->tournaments_list,(MapKeyElement)(&tournament_id)); //can't fail because of previous checks
    removePlayersTournamentTrack(chess->players_list,tournament_id); 
    return CHESS_SUCCESS;

}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    if(!chess)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if(player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    if(!(checkIfPlayerExistsInSystem(chess->players_list,player_id)))
    {
        return  CHESS_PLAYER_NOT_EXIST;
    }
    for(int* id_tournament=(int*)mapGetFirst(chess->tournaments_list);
    id_tournament!=NULL; id_tournament=(int*)(mapGetNext(chess->tournaments_list)))
    {
       TournamentData data=mapGet(chess->tournaments_list,( MapKeyElement)(id_tournament)); 
       if(checkTournamentStatus(data) != ENDED) //the tournament is still going-may be updates
        {
           bool is_player_in_tournament=false; //lets us know if the player is in a two people game
           for(Player player=getFirstPlayer(chess->players_list); player != NULL;player=getNextPlayer(player))
            {
               int other_player_id=getPlayerId(player);
               int win=0,lose=0,draw=0;
               updateGameBetweenPlayersIfExist(data,player_id,other_player_id,&win,&lose,&draw,
                                                                    &is_player_in_tournament);
                updatePlayerTrackAfterGameChanged(chess->players_list,*(id_tournament),win,lose,draw,
                                                                                     other_player_id);
                if(is_player_in_tournament==true)
                {
                    updateRemovedTournamentData(data);//the player will be removed so we need 
                }                                     //to update the amount of the deleted players 
            }
            int removed_player_mark=0;
            bool is_playing_solo=false; //let us know if the player is in a game without another player
            updateGameWhereOtherPlayerWasRemoved(data,player_id,removed_player_mark,&is_playing_solo);
            if((is_playing_solo==true) && (is_player_in_tournament==false) )
            {
                updateRemovedTournamentData(data); //we only need to update it 
                                //if one of the cases in the if happened to avoid double counting
            }
        }
        freeTournamentId(id_tournament);
    }
    removePlayerFromChessSystem(chess->players_list,player_id);
    return CHESS_SUCCESS;   
}

ChessResult chessEndTournament(ChessSystem chess, int tournament_id)
{
    if(!chess)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id<=0)
    {
        return CHESS_INVALID_ID;
    }
    if((mapContains(chess->tournaments_list,( MapKeyElement)(&tournament_id)))==false)
    {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    TournamentData data=mapGet(chess->tournaments_list,(MapKeyElement)(&tournament_id));
    if(checkTournamentStatus(data)==ENDED)
    {
        return CHESS_TOURNAMENT_ENDED;
    }
    if(checkIfTournamentIsEmpty(data))
    {
        return CHESS_NO_GAMES;
    }
    int winner_id = determineTheTournamentWinner(tournament_id,chess->players_list);
    updateTournametStatusToEndedAndUpdateWinner(winner_id,data);
    return CHESS_SUCCESS;
}


double chessCalculateAveragePlayTime(ChessSystem chess, int player_id, ChessResult* chess_result)
{
    if(!chess_result)
    {
        return ERROR;
    }
    if(!chess)
    {
        *chess_result = CHESS_NULL_ARGUMENT;
        return ERROR;
    }
    if(player_id <= 0)
    {
        *chess_result=CHESS_INVALID_ID;
        return ERROR;
    }
    if(checkIfPlayerExistsInSystem(chess->players_list,player_id)==false)
    {
        *chess_result=CHESS_PLAYER_NOT_EXIST;
        return ERROR;
    }
    //we know the player exist so there is at least one tournament and one game
    int game_time =0;
    for(int* id_tournament=(int*)mapGetFirst(chess->tournaments_list);
    id_tournament!=NULL; id_tournament=(int*)(mapGetNext(chess->tournaments_list)))
    {
        TournamentData data=mapGet(chess->tournaments_list,( MapKeyElement)(id_tournament));
        game_time+=calculatePlayerTimeInTournament(data,player_id);
        freeTournamentId(id_tournament);
    }
    int num_of_games = getNumberOfGamesPlayed(chess->players_list,player_id);
    *chess_result=CHESS_SUCCESS;
    if(game_time==0) 
    {
        return 0;  
    }                                     //if game time isn't zero than num_of_games also not zero
    double average_game_time=((double)(game_time) / (double)(num_of_games));
    return average_game_time;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE* file)
{
    if(!chess)
    {
        return CHESS_NULL_ARGUMENT;
    }
    if(!file) //the file is not open
    {
        return CHESS_SAVE_FAILURE;
    }
    bool condition = checkIfThereArePlayersToSave(chess->players_list);
    if (condition==false) //there are no people to print- success  
    {
        return CHESS_SUCCESS;  
    }
    printPlayersLevels(chess->players_list,file);
    return CHESS_SUCCESS;
}

ChessResult chessSaveTournamentStatistics(ChessSystem chess, char* path_file)
{
    ChessResult error_value = checkArgumentsForSaveTournamentStatistics(chess, path_file);
    if(error_value != CHESS_SUCCESS)
    {
        return error_value;
    }
    FILE* stream = fopen(path_file,"w");
    if (stream == NULL)
    {
        return CHESS_SAVE_FAILURE; 
    }
    for(int* id_tournament=(int*)mapGetFirst(chess->tournaments_list);
    id_tournament!=NULL; id_tournament=(int*)(mapGetNext(chess->tournaments_list)))
    {
        TournamentData data=mapGet(chess->tournaments_list,(MapKeyElement)(id_tournament));
        if(checkTournamentStatus(data)==ENDED)
        {
            int winner = determineTheTournamentWinner(*id_tournament,chess->players_list);
            int longest_game_time =getLongestGame(data);
            int num_of_games = getNumberOfGamesInTournament(data);
            double average_game_time=calculateAverageGameTimeInTournament(data);
            const char* location = getTournamentLocation(data);
            int num_of_players = getNumberOfPlayersInTournament(chess->players_list,(*id_tournament));
            //num of players doesn't take into account the player who were removed from the tournament
            num_of_players += getRemovedPlayersAmountFromTournament(data); //now it include them

            fprintf(stream, "%d\n",winner);
            fprintf(stream, "%d\n",longest_game_time);
            fprintf(stream, "%.2f\n",average_game_time);
            fprintf(stream, "%s\n",location);
            fprintf(stream, "%d\n",num_of_games);
            fprintf(stream, "%d\n",num_of_players);
        }
        freeTournamentId(id_tournament);
    }
    fclose(stream);
    return CHESS_SUCCESS;
}






