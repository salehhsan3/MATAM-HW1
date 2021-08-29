
#include "players.h"
#include <stdbool.h>
#include <stdlib.h>


struct player_t 
{
    int player_id;
    TrackPlayer player_track;
    struct player_t* next;
};

struct players_container_t{
    Player head;  //head will point to the first player in the system
};

/*print the next player in the list who have the same level as the printed one and bigger id than
his (but still smaller than the rest players'id with the same level)*/
static void printSameLevelBiggerId(Player first_player,double printed_level,int* printed_id,FILE* file)
{
    Player current=first_player;
    int to_print_id=0;
    bool found_another=false;
    while(current!=NULL)
    {
        if(current->player_id==0 || current->player_track==NULL) //its not a player with a level
        {
            current=current->next;
            continue; 
        }
        else if(current->player_track!=NULL)
        { 
            double current_level=calculatePlayerLevel(current->player_track);
            if((current_level==printed_level) && (current->player_id>*printed_id))
            {
                if(found_another==false) //found person with same level for the first time
                {
                    found_another=true;
                    to_print_id=current->player_id;
                }
                else
                {
                    if(current->player_id<to_print_id)
                    {
                        to_print_id=current->player_id;
                    }
                }
            }
        }
        current=current->next;
    }
    printPlayerIdAndLevel(to_print_id,printed_level,file);
    *printed_id=to_print_id;
    return;
}

/*find another player (we know he exist) with a smaller level than the former printed player and print him.
this function also update the printed level and id to be of the one she print*/
void findPlayerWithSmallerLevelAndPrint(Player first_player,double* printed_level,int* printed_id,
                                                                    int* left_to_print,FILE* file)
{
    Player current=first_player;
    int to_print=0;
    double next_bigger_level=0;
    bool found_another=false;
    while(current!=NULL)
    {
        if(current->player_id!=0 && current->player_track!=NULL)
        {
            double temp_calculate=calculatePlayerLevel(current->player_track);
            if(temp_calculate<*printed_level)
            {
                if(found_another==false)
                {
                    found_another=true;
                    to_print=current->player_id;
                    next_bigger_level=temp_calculate;
                }
                else
                {
                    if(temp_calculate>next_bigger_level && temp_calculate<*printed_level)
                    {
                        to_print=current->player_id;
                        next_bigger_level=temp_calculate;
                    }
                    else if(temp_calculate==next_bigger_level)
                    {
                        if(current->player_id<to_print)
                        {
                            to_print=current->player_id;
                        }
                    }
                }

            }
        }
        current=current->next;
    }
    printPlayerIdAndLevel(to_print,next_bigger_level,file);
    *left_to_print=*left_to_print-1;
    *printed_level=next_bigger_level;
    *printed_id=to_print;
    return;
}

/*find all the valid playes who played some games and therefore have a level and need to be printed
-we know there is at least one (if we call this function we allready printed one player) */
static int findAmountPlayersWithLevel(PlayersContainer players_list)
{
    int count_players=0;
    Player current=players_list->head->next;
    while(current!=NULL)
    {
        if(current->player_id!=0 && current->player_track!=NULL)
        {
            count_players++;
        }
        current=current->next;
    }
    assert(count_players>=1); 
    return count_players;
}


/*find the id and the level of the first player being printed- we know that a player that can be
printed exist*/
static void findFirstPlayerToPrintLevel(PlayersContainer players_list,int* id,double* level)
{
    Player player=players_list->head->next;
    assert(player!=NULL);
    int max_level_id=0;
    double temp_level=0,max_level=0;
    bool is_id_updated_for_the_first_time=true;
    while(player!=NULL) //here we will find the first player to print
    {
        if(player->player_id!=0 && player->player_track!=NULL)//if id is of a not removed player
                                                            // and it is a player that played
        {
            temp_level=calculatePlayerLevel(player->player_track);
            if(temp_level>=max_level)
            {
                if(is_id_updated_for_the_first_time==true) //max_level_id is invalid (=0)
                {                                          //and can't be used for comparing
                    max_level_id=player->player_id;
                    max_level=temp_level;  
                    is_id_updated_for_the_first_time=false;
                }
                else
                {
                    if(temp_level>max_level) 
                    {
                        max_level_id=player->player_id;
                        max_level=temp_level;
                    }
                    else //temp_level=max level -> no need to update the level 
                    {
                        if(max_level_id>player->player_id) //else-no change 
                        {
                            max_level_id=player->player_id;
                        }
                    }
                }   
            }
        }
        player=player->next;
    }
    *id=max_level_id;
    *level=max_level;
    return;
}


/*determines the winner of the tournament by the system's rules*/
static int detemineWinnerByRules(int winner_id,int winner_loses,int winner_wins,int winner_score,
            int current_wins,int current_player_score,int current_player_id,int current_loses)
{
    if(winner_id==0) //there was not (yet) a candidate for victory
    {
        return current_player_id;
    }
    if(winner_score<current_player_score)
    {
        return current_player_id;
    }
    if(winner_score==current_player_score)
    {
        if(winner_loses==current_loses)
        {
            if(winner_wins==current_wins)
            {
                return ((winner_id<current_player_id)?winner_id:current_player_id);
            }
            else
            {
                return ((winner_wins>current_wins)?winner_id:current_player_id);
            }
        }
        else
        {
            return ((winner_loses<current_loses)?winner_id:current_player_id);
        }
    }
    return winner_id;

}

/*add the first player to the list of the players in the system
error type:false in case of allocation fail
otherwise return true*/
static bool addPlayerInEmptyList(Player head,int player_id,int win,int lose,int draw,int tournament_id)
{
    assert((head->next)==NULL);
    head->next=malloc(sizeof(*(head->next)));
    if(!(head->next))
    {
        return false;
    }
    Player new_player=head->next;
    new_player->player_track=allocateMemoryForTrack();
    if(!new_player->player_track)
    {
        free(new_player);
        head->next=NULL;
        return false;
    }
    new_player->next=NULL;
    new_player->player_id=player_id;
    fillTrackPlayer(new_player->player_track,tournament_id,win,lose,draw,1,NULL);
    return true;
}

/*add a player that didn't existed before to an unempty list of players
error type: false if allocation failed
otherwise return true*/
static bool addPlayerInNotEmptyList(Player head,int player_id,int win,int lose,int draw,
                                                                                    int tournament_id)
{
    assert(head->next!=NULL); // a player exist
    Player current=head->next;
    while(current->next!=NULL)
    {
        current=current->next;
    }
    current->next=malloc(sizeof(*(current->next)));
    if(!(current->next))
    {
        current->next=NULL;
        return false;
    }
    current->next->player_id=player_id;
    current->next->next=NULL;
    current->next->player_track=allocateMemoryForTrack();
    if(!(current->next->player_track))
    {
        free(current->next);
        current->next=NULL;
        return false;
    }
    fillTrackPlayer(current->next->player_track,tournament_id,win,lose,draw,1,NULL);
    return true;
}

/*after conducting a game,if the player doesn't exits in the list of the players-add him
otherwise update his information
error type: false in case of allocation fail
otherwise return true*/
static bool updatePlayerOrAddPlayerToList(Player head,int player_id,int win,int lose,int draw,
                                                                             int tournament_id)
{
    if(head->next==NULL) //list empty 
    {
        return addPlayerInEmptyList(head,player_id,win,lose,draw,tournament_id);
    }
    //if we are here-the list is not empty
    Player current=head->next;
    while(current!=NULL)
    {
        if(current->player_id==player_id) //our player exist so we need to update or create his information
        {
            if(checkIfTournamentExistInTrack(current->player_track,tournament_id)==false) //we need to create it
            {                                                    
                if(current->player_track==NULL)  //it's his first game in the tournament and in the system
                                                  //-the track in empty
                {
                    current->player_track=allocateMemoryForTrack();
                    {
                        if(!current->player_track)
                        {
                            return false;
                        }
                    }
                    fillTrackPlayer(current->player_track,tournament_id,win,lose,draw,1,NULL);
                    return true;
                }                   //we will add the information at the end of the track (since not empty)
                return addTornamentToNotEmptyTrack(current->player_track,tournament_id,win,lose,draw);
            }
            //the tournament exist in the player_track and we need to update it's value
            return updateTournamentInTrack(current->player_track,tournament_id,win,lose,draw);
        }
        current=current->next;
    }
    //the player doesn't exist in the system, meaning it's his first game and first tournament
    return addPlayerInNotEmptyList(head,player_id,win,lose,draw,tournament_id);
}


/*save the result of a game from the player view*/
static void saveGameResultForPlayer(Winner player,Winner winner,int* win,int* lose,int* draw)
{
    if(winner==player)
    {
        *win=1,*lose=0,*draw=0;
        return;
    }
    else if(winner==DRAW)
    {
        *win=0, *lose=0, *draw=1;
        return;
    }
    //if he didn't win, and it wasn't a draw it means that he lost.
    *win=0,*lose=1,*draw=0;
    return;

}

/////////////implementaion of the functions from players.h/////////////////

PlayersContainer createPlayerList()
{
    PlayersContainer players_list=malloc(sizeof(*players_list));
    if(!players_list)
    {
        return NULL;
    }
    players_list->head=malloc(sizeof(*(players_list->head)));
    if(!players_list->head)
    {
        free(players_list);
        players_list=NULL;
        return NULL;
    }
    players_list->head->next=NULL; //there are no players yet in the system
    players_list->head->player_id=0;
    players_list->head->player_track=NULL;
    return players_list;
}

void destroyPlayerList(PlayersContainer players_list)
{
    if(players_list==NULL)
    {
        return;
    }
    if(players_list->head->next==NULL) //there are no players in the system
    {
        free(players_list->head);
        players_list->head=NULL;
        free(players_list);
        players_list=NULL;
        return;
    }
    //there is at least one player in the system
    Player current=players_list->head->next;
    while(current!=NULL)
    {
      destroyPlayerTrack(current->player_track);
      current->player_track=NULL;
      Player to_free=current;
      current=current->next;
      free(to_free);
      to_free=NULL;
    }
    players_list->head->next=NULL;
    free(players_list->head);
    players_list->head=NULL;
    free(players_list);
    players_list=NULL;
    return;
}

bool checkIfPlayerHasExcedeedMaxNumberOfGamesAllowedInTournament(PlayersContainer players_list,
                                                        int num_game_limit,int tournament_id,int player_id)
{
    assert((num_game_limit>0) && (tournament_id>0) && (player_id>0));
    Player current=players_list->head->next;
    while(current!=NULL)
   {
       if(current->player_id==player_id)
       {
           if(findNumberOfGamesPlayedInTournament(current->player_track,tournament_id) < num_game_limit )
           {
               return false;
           }
           else
           {
               return true;
           }
       }
       current = current->next;
   }
   return false;//he didn't play any games.
}


bool updatePlayersList(PlayersContainer players_list,int first_player,int second_player,Winner winner,
                                                                                            int tournament_id)
{
    int first_player_win=0, first_player_lose=0, first_player_draw=0;
    saveGameResultForPlayer(FIRST_PLAYER,winner,&first_player_win,&first_player_lose,&first_player_draw);
    bool result = updatePlayerOrAddPlayerToList(players_list->head,first_player,first_player_win,
                                                        first_player_lose,first_player_draw,tournament_id);
    if(result!=true)
    {
        return false;
    }
    int second_player_win=0, second_player_lose=0, second_player_draw=0;
    saveGameResultForPlayer(SECOND_PLAYER,winner,&second_player_win,&second_player_lose,&second_player_draw);
    return updatePlayerOrAddPlayerToList(players_list->head,second_player,second_player_win,
                                                        second_player_lose,second_player_draw,tournament_id);

}

void removePlayersTournamentTrack(PlayersContainer players_list,int tournament_id)
{
    assert( (players_list!=NULL) && (tournament_id>0) );
    Player current_player=players_list->head->next;
    if(current_player==NULL) //there are no players 
    {
        return;
    }
    while(current_player!=NULL)
    {
        if(checkIfTournamentExistInTrack(current_player->player_track,tournament_id)) 
        {
            bool first_changed=false;
            TrackPlayer track=freePlayerTrackOfCertainTournament(current_player->player_track,
            tournament_id,&first_changed);
            if(first_changed==true)
            {
                updateFirstInPlayerTrack(current_player,track);
            }
        }
        current_player=current_player->next;
    }
    return;
}

void updatePlayerTrackAfterGameChanged(PlayersContainer players_list,int tournament_id,int win,
                                                                  int lose,int draw,int player_id)
{
    Player current_player=players_list->head->next;
    assert(current_player!=NULL);//if we update information for a player, the list include him
                                                                             // and isn't empty
    while(current_player!=NULL)
    {
        if(current_player->player_id==player_id)
        {
            TrackPlayer current_track=current_player->player_track;
            assert(current_player!=NULL); //the player practic in a tournament, so the list isn't empty
            updatePlayerTrackAfterAutoWin(tournament_id,win,lose,draw,current_track);
            return;

        }
        current_player=current_player->next;
    }
    assert(1);    //suppose to exit at the former return
    return; 
}

Player getFirstPlayer(PlayersContainer players_list)
{
    return (players_list->head->next);
}

Player getNextPlayer( Player current_player)
{
    if (current_player == NULL)
    {
        return NULL;
    }
    return (current_player->next);
}

int getPlayerId(Player player)
{
    assert(player != NULL);
    return (player->player_id);
}

void removePlayerFromChessSystem(PlayersContainer players_list,int player_id)
{
    Player current_player=players_list->head->next;
    while(current_player!=NULL)
    {
        if(current_player->player_id == player_id)
        {
            current_player->player_id=0; //mark the player as deleted
            destroyPlayerTrack(current_player->player_track);
            current_player->player_track=NULL;
            return;
        }
        current_player=current_player->next;
    }
    return;
}

int determineTheTournamentWinner(int tournament_id,PlayersContainer players_list)
{
    assert(players_list->head->next!=NULL);
    Player current_player=players_list->head->next;
    int winner_id=0, winner_score=0,winner_wins=0,winner_loses=0;
    while(current_player!=NULL)
    {
        TrackPlayer track=current_player->player_track;
        if(track==NULL || !(checkIfTournamentExistInTrack(track,tournament_id)))
        {
            current_player=current_player->next;
            continue; //the player didn't played in the tournament
        }
            //the player played in the tournament
        int current_player_id=current_player->player_id;
        int current_player_score=calculateScore(tournament_id,track);
        int current_wins=0, current_loses=0;
        fillWinLoseForPlayer(track,&current_wins,&current_loses,tournament_id);
        winner_id=detemineWinnerByRules(winner_id,winner_loses,winner_wins,winner_score,current_wins,
                                            current_player_score,current_player_id,current_loses);
        if(winner_id==current_player->player_id)
        {
            winner_score=current_player_score;
            winner_wins=current_wins;
            winner_loses=current_loses;
        }
        current_player=current_player->next;
    }
    return winner_id;
}

bool checkIfPlayerExistsInSystem(PlayersContainer players_list,int player_id)
{
    Player current_player=players_list->head->next;
    if(!current_player) //therer are no players at all in the system
    {
     return false;   
    }
    while(current_player!=NULL)
    {
        if(current_player->player_id==player_id)
        {
            return true;
        }
        current_player=current_player->next;
    }
    return false; 
}

int getNumberOfGamesPlayed(PlayersContainer players_list,int player_id)
{
    Player current = players_list->head->next; //there is at least one player in the list (this player)
    
    while ( current != NULL && current->player_id != player_id)
    {
        current = current->next;
    }
    return (getAmountOfGamesPlayed(current->player_track));
}

int getNumberOfPlayersInTournament(PlayersContainer players_list, int id_tournament)
{
    Player current_player=players_list->head->next;
    int how_many_players=0;
    while(current_player!=NULL)
    {
        
        TrackPlayer current_track=current_player->player_track;
        if(checkIfTournamentExistInTrack(current_track,id_tournament))
        {
            how_many_players++;
        }
        current_player=current_player->next;
    }
    return how_many_players;
}


void printPlayerIdAndLevel(int id,double level,FILE* file)
{
    if(id==0)
    {
        return; 
    }
    fprintf(file,"%d %.2f\n",id,level);
    return;  
}

bool checkIfThereArePlayersToSave(PlayersContainer players_list)
{
    if(players_list==NULL || players_list->head==NULL) //there are no players in the system yet
    {
        return false;
    }
    Player current = players_list->head->next;
    while (current != NULL)
    {
        if ((current->player_id != 0) && (current->player_track!=NULL))
        {
            return true;
        }
        current=current->next;
    }
    return false;
}

int calculateSameLevelAmount(PlayersContainer players_list,double printed_level)
{
    Player current=players_list->head->next;
    int same_level=0;
    while(current!=NULL)
    {
        if(calculatePlayerLevel(current->player_track)==printed_level)
        {
            same_level++;
        }
        current=current->next;
    }
    return same_level;
}

void printPlayersLevels(PlayersContainer players_list,FILE* file)
{
    int first_id=0;
    double first_level=0;
    findFirstPlayerToPrintLevel(players_list,&first_id,&first_level);
    printPlayerIdAndLevel(first_id,first_level,file);
    double printed_level=first_level;
    int printed_id=first_id;
    int how_many_players_to_print=findAmountPlayersWithLevel(players_list); //inculding the first
    int left_to_print=how_many_players_to_print-1; //without the first

    while(left_to_print>0)
    {
        int players_with_same_level=calculateSameLevelAmount(players_list,printed_level);
        int remain_with_same_level=players_with_same_level-1;
        while(remain_with_same_level!=0)
        {
            printSameLevelBiggerId(players_list->head->next,printed_level,&printed_id,file);
            remain_with_same_level--;
        }
        left_to_print=left_to_print-remain_with_same_level;
        if(left_to_print>0)
        {
            findPlayerWithSmallerLevelAndPrint(players_list->head->next,&printed_level,&printed_id,
                                                                    &left_to_print,file);                    
        }
        //the function avobe updated the current printed_id and printed_level
        //for the next iteration
    }
    return;
 
}

void updateFirstInPlayerTrack(Player player,TrackPlayer new_first)
{
    player->player_track=new_first;
    return;
}




