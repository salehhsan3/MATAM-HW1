CC = gcc
OBJS = chessSystem.c chessSystem.h map.h players.h player_track.h games.h tournament_data.h libmap.a 
EXEC = chess
DEBUG_FLAG = #empty, assign -g for debug
COMP_FLAG = -std=c99 -Wall -pedantic-errors -Werror -DNDEBUG

$(EXEC)	: $(OBJS)
	$(CC) $(DEBUG_FLAG) $(OBJS) -o $@
chessSystemTestsExample.o	:chessSystemTestsExample.c chessSystem.h test_utilities.h
	$(cc) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
games.o	:games.c games.h chessSystem.h                                    
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
players.o	:players.c players.h chessSystem.h player_track.h games.h
	$(CC) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
player_track.o	:player_track.c player_track.h players.h chessSystem.h games.h
	$(cc) -c $(DEBUG_FLAG) $(COMP_FLAG) $*.c
tournament_data.o	:tournament_data.c tournament_data.h games.h chessSystem.h map.h 
	$(cc) -c $(DNDEBUG) $(COMP_FLAG) $*.c 
clean:
	rm -f $(OBJS) $(EXEC)



