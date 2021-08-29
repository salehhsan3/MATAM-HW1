
#include "games.h"
struct game_t
{
    int first_player_id;
    int second_player_id;
    Winner game_result;
    int play_time;
    struct game_t *next;
};

/*update the statictics (of the other player) according to the result in the game*/
static void updateStatistic(Winner game_result,Winner to_be_removed_player,int* win,int* lose,int* draw)
{
    if(game_result==to_be_removed_player) //the other will become the winner
    {
        *win=1;
        *lose=-1;
        *draw=0;
    } 
    else if(game_result==DRAW)  //the other will become the winner
    {
        *win=1;
        *lose=0;
        *draw=-1;
    }
    else //the other player won anyway and there is nothing to update
    {
        *win=0;;
        *lose=0;
        *draw=0;
    }
    return;
}

/*update the parameters in the game to describe a game in which a certain player was removed*/
static void updateIdOfPlayerToBeRemovedAndWinner(Game game,Winner to_be_removed)
{
    if(to_be_removed == FIRST_PLAYER)
    {
        game->first_player_id=0;
        game->game_result=SECOND_PLAYER;
    }
    else // to_be_removed == SECOND
    {
        game->second_player_id=0;
        game->game_result=FIRST_PLAYER;
    }
    return;
}

/////////////////////////implementaion of the functions from games.h///////////////////

void freeGame(Game game)
{
    if(game==NULL)
    {
        return;
    }
    while(game!=NULL)
    {
        Game to_free=game;
        game=game->next;
        free(to_free);
    }
    return;
}


Game copyAllGamesInData(Game game,bool* is_copied_game)
{
    if(!game)
    {
        *is_copied_game=true; //nothing to copy- success
        return NULL;
    }                         //there is at least one game
    Game copy_game=malloc(sizeof(*copy_game));
    if(!copy_game)
    {
        *is_copied_game=false;
        return NULL;
    }
    fillGame(copy_game,game->first_player_id,game->second_player_id,game->game_result,game->play_time,NULL);
    Game first_game=copy_game; //pointer to the first added game 
    Game next_to_copy=game->next;
    while(next_to_copy!=NULL)
    {
        copy_game->next=malloc(sizeof(*copy_game));   
        if(!copy_game->next)
        {
            freeGame(first_game); 
            *is_copied_game=false;
            return NULL;
        }
        fillGame(copy_game->next,next_to_copy->first_player_id,next_to_copy->second_player_id,
            next_to_copy->game_result,next_to_copy->play_time,NULL);
        copy_game=copy_game->next;
        next_to_copy=next_to_copy->next;  
    }
    *is_copied_game=true;
    return first_game;
}



void fillGame(Game game,int first_id, int second_id, Winner result, int game_time,Game next_game)
{
    game->first_player_id=first_id;
    game->second_player_id=second_id;
    game->play_time=game_time;
    game->game_result=result;
    game->next=next_game;
    return;
}


bool addGameAtEnd(Game game,int tournament_id, int first_player, 
                                int second_player, Winner winner, int play_time)
{
    assert(game!=NULL);
    Game current_game=game;
    while(current_game->next!=NULL)
    {
        current_game=current_game->next;
    }
    current_game->next=malloc(sizeof(*(current_game->next)));
    if(!(current_game->next))
    {
        return false;// a memory error happened
    }
    fillGame(current_game->next,first_player,second_player,winner,play_time,NULL);
    return true;//the function was successful   
}   


void checkIfGameBetweemTwoExistAndUpdate(Game game,int player_id,int other_player_id,int* win,
                                                    int* lose,int* draw,bool* is_player_in_tournament)
{
    Game current = game;
    while ( current != NULL )
    {
        if( (current->first_player_id == player_id && current->second_player_id == other_player_id) )
        {
            updateStatistic(current->game_result,FIRST_PLAYER,win,lose,draw);
            updateIdOfPlayerToBeRemovedAndWinner(current,FIRST_PLAYER);
            *is_player_in_tournament=true;
            return;

        }
        else if( (current->second_player_id == player_id && current->first_player_id == other_player_id) )
        {
            updateStatistic(current->game_result,SECOND_PLAYER,win,lose,draw);
            updateIdOfPlayerToBeRemovedAndWinner(current,SECOND_PLAYER);
            *is_player_in_tournament=true;
            return;
        }
        current = current->next;
    }
    //there is no game as such
    *is_player_in_tournament=false;
    return; 
}

void checkIfGameWithOnlyPlayerExistAndRemoveIt(Game game,int player_id,int removed_player_mark,
                                                bool* is_playing_solo)
{
    Game current_game=game;
    while(current_game!=NULL)
    {
        if((current_game->first_player_id==player_id && current_game->second_player_id==removed_player_mark)
        ||(current_game->second_player_id==player_id && current_game->first_player_id==removed_player_mark))
        {
            *is_playing_solo=true;
            //either way, both the players will be removed and marked as deletead
            current_game->first_player_id=removed_player_mark;
            current_game->second_player_id=removed_player_mark;
            current_game->game_result=DRAW;
        }
        current_game=current_game->next;

    }
    *is_playing_solo=false;
    return; //there are no games as such

}

int calculatePlayerTimeInTournamentGames(Game first_game,int player_id)
{
    int total_time=0;
    Game current_game=first_game;
    while(current_game != NULL)
    {
        if((current_game->first_player_id == player_id) || (current_game->second_player_id==player_id))
        {
            total_time=total_time+(current_game->play_time);
        }
        current_game=current_game->next;
    }
    return total_time;
}

int getNumberOfGames(Game game)
{
    int num_of_games=0;
    Game current = game;
    while (current!=NULL)
    {
        num_of_games++;
        current=current->next;
    }
    return num_of_games;   
}

int getLongestGameTime(Game game)
{
    int longest_game_time=0;
    Game current = game;
    while (current!=NULL)
    {
        if (current->play_time > longest_game_time)
        {
            longest_game_time = current->play_time;
        }
        current=current->next;
    }
    return longest_game_time;   
}

double calculateAverageGamesTime(Game game)
{
    int games_time = 0, number_of_games = 0;
    Game current=game;
    while(current!=NULL)
    {
        games_time=games_time+(current->play_time);
        number_of_games++;
        current=current->next;
    }
    return (games_time/(double)(number_of_games)); //number_of_games will not be zero becasue an 
                                                // ended tournament has at least one game
}

bool CheckIfPlayerIdMatch(Game game,int first_player, int second_player)
{
    return (((game->first_player_id == first_player) && (game->second_player_id == second_player))
           ||((game->first_player_id==second_player) && (game->second_player_id==first_player)) );
}

Game advanceGame(Game game)
{
    if (game == NULL)
    {
        return NULL;
    }
    return (game->next);
}

Game allocateMemoryForGame()
{
    Game game=malloc(sizeof(*game));
    return game; //return NULL if failed
}