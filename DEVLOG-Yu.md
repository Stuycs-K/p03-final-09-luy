# Dev Log:

## Yu Lu

### 2026-01-6 - Update README and PROPOSAL
Moved yesterday's PROPOSAL work into repo today, also make sure that everything in the readme is filled out

### 2026-01-7 - Multisocket (BROKEN)
attempting multisocket on new branch

### 2026-01-8 - Multisocket + game skeleton
Finished fixing the multisocket, now rewriting lab to fit game logic
Player structs into server, no change in displays yet

### 2026-01-9 - Working on reading linux word list
Read linux word list in another c prg
then working on constructing more game logic and revise client.c

### 2026-01-11 - Client, .gitattributes, Game
Implemented select() to client; it now will not be blocked waiting for a input from user and can recieve from server
gitattributes config to LF bc working w/ windows at home
Game is in its mvp stages, you have to manually set the starting letter and u can only guess 4 letter words from a preset dictionary


### 2026-01-12 - Welcome, Logic
Welcome msg for every client that connects
logic that checks validity of words based if in dictionary, in word list, current player turn

### 2026-01-13 - Research into raw mode
Researched into feasibility of raw mode for better client interactions
was not worth it

### 2026-01-14 - exclusive server messages
returns excluisive msgs to clients that are in the turn

### 2026-01-15 - random Prompt
Letter to start with now are random instead of just the same one as start with when initally running main
more cleanup including broadcast for only server and only client

### 2026-01-16 - stdin and video
standard in supported with 1 command for server and client
recorded video