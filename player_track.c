
#include "players.h"
#include "player_track.h"

#define MULTUPLY_FOR_WIN 2
#define MULTIPLY_FOR_TIE 1

#define LEVEL_WIN_MULTIPLY 6
#define LEVEL_LOSE_MULTIPLY 10
#define LEVEL_DRAW_MULTIPLY 2

/*struck in order to keep track about information relative to a palyer*/
struct track_player_t 
{
    int tournament_id;
    int wins;
    int losses;
    int draws;
    int amount_of_games_played;
    struct track_player_t* next;
};

void destroyPlayerTrack(TrackPlayer player_track)
{
    if(player_track==NULL)
    {
        return;
    }
    TrackPlayer current=player_track;
    while(current!=NULL)
    {
        TrackPlayer to_free=current;
        current=current->next;
        free(to_free);
    }
    return;
}

int findNumberOfGamesPlayedInTournament(TrackPlayer player_track,int tournament_id)
{
    assert(tournament_id>0);
    if(player_track==NULL) //the player didn't played yet or was deleted
    {
        return 0;
    }
    int num_of_games = 0;
    TrackPlayer current = player_track;
    while (current != NULL)
    {
        if(current->tournament_id==tournament_id)
        {
            num_of_games = current->amount_of_games_played;
            return num_of_games;
        }
        current=current->next;
    }
    return num_of_games; //equall to zero because player didn't practiced in the tournament  
}

void fillTrackPlayer(TrackPlayer track,int tournament_id,int wins,int loses, int draws,
                                                                   int games_played,TrackPlayer next)
{
    track->amount_of_games_played = games_played;
    track->tournament_id = tournament_id;
    track->next=next;
    track->wins = wins;
    track->losses = loses;
    track->draws = draws;
    return;
}

bool checkIfTournamentExistInTrack(TrackPlayer track,int tournament_id)
{
    if(track==NULL)
    {
        return false;
    }
    TrackPlayer current=track;
    while(current!=NULL)
    {
        if((current->tournament_id)==tournament_id)
        {
            return true;
        }
        current=current->next;
    }
    return false;
}

bool addTornamentToNotEmptyTrack(TrackPlayer track,int tournament_id,int win,int lose,int draw)
{
    assert(track!=NULL);
    while((track!=NULL) && (track->next!=NULL))
    {
        track=track->next;
    }
    track->next=malloc(sizeof(*(track->next)));
    if(track->next==NULL)
    {
        return false;
    }
    fillTrackPlayer(track->next,tournament_id,win,lose,draw,1,NULL);
    return true;
}

bool updateTournamentInTrack(TrackPlayer track,int tournament_id,int win,int lose,int draw)
{
    assert(track!=NULL);
    while((track!=NULL) && (track->tournament_id!=tournament_id))
    {
        track=track->next;  //the tournament exist so the while wiil end
    }
    track->amount_of_games_played=(track->amount_of_games_played)+1;
    track->draws=(track->draws)+draw;
    track->losses=(track->losses)+lose;
    track->wins=(track->wins)+win;
    return true;
}

TrackPlayer freePlayerTrackOfCertainTournament (TrackPlayer track,int tournament_id,bool* is_first_changed)
{
    assert(track!=NULL);
    TrackPlayer before_track=NULL;
    while((track!=NULL) && (track->tournament_id!=tournament_id))
    {
        before_track=track;
        track=track->next; //the tournament exist so the while will end
    }
    if(before_track==NULL) //we remove the first
    {
        TrackPlayer to_free=track;
        track=track->next;
        *is_first_changed=true;
        free(to_free);
        return track;
    }
    TrackPlayer to_free=track;
    before_track->next=track->next;
    free(to_free);
    *is_first_changed=false;
    return NULL; //there will be no use in the return value, so initialize to NULL
}

void updatePlayerTrackAfterAutoWin(int tournament_id,int win,int lose,int draw,TrackPlayer player_track)
{
    assert(player_track!=NULL);
    while((player_track->next!=NULL) && (player_track->tournament_id!=tournament_id)) 
    {
        player_track=player_track->next; //we will exit the while because the player played in the tournament
    }
    player_track->wins=(player_track->wins)+win;
    player_track->losses=(player_track->losses)+lose;
    player_track->draws=(player_track->draws)+draw;
    return;
}


int calculateScore(int tournament_id,TrackPlayer track)
{
    while(track!=NULL)
    {
        if(track->tournament_id==tournament_id)
        {
            int win=track->wins;
            int tie=track->draws;
            int score=(win*MULTUPLY_FOR_WIN)+(tie*MULTIPLY_FOR_TIE); //losing doesn't add points
            return score;
        }
        track=track->next;
    }
    return 0; //won't get here because the tournament exist in the track
}

void fillWinLoseForPlayer(TrackPlayer track,int* current_wins,int* current_loses,int tournament_id)
{
    assert(track!=NULL);
    while((track!=NULL) && (track->tournament_id!=tournament_id))
    {
        track=track->next;  //the tournament exist so the while will end before reaching NULL
    }
    *current_loses=track->losses;
    *current_wins=track->wins;
    return;
}


int getAmountOfGamesPlayed(TrackPlayer track)
{
    int games=0;
    while(track!=NULL)
    {
        games=games+track->amount_of_games_played;
        track=track->next;
    }
    return games; //we know the player is in at least one game
}

double calculatePlayerLevel(TrackPlayer track)
{
    if(track==NULL) //we use this function ourself when we are sure track isn't null, this is
    {                                                             //for forcaution from user
        return 0;
    }
    int win=0,lose=0,tie=0,games=0;
    while(track!=NULL)
    {
        win=win+(track->wins);
        lose=lose+(track->losses);
        tie=tie+(track->draws);
        games=games+(track->amount_of_games_played);
        track=track->next;
    }
    int score=(win*LEVEL_WIN_MULTIPLY)-(lose*LEVEL_LOSE_MULTIPLY)+(tie*LEVEL_DRAW_MULTIPLY);
    double level=score/(double)games;
    return level; 
}

TrackPlayer allocateMemoryForTrack()
{
    TrackPlayer track=malloc(sizeof(*track));
    return track; //may return NULL
}