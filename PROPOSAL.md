# Final Project Proposal

## Group Members: Yu Lu
       
# Intentions:

A simple recreation of word bomb through networking in C. Servers acting as gamemasters that track rounds, validate words, and points/health of each client as they take turns in guessing the right words. The last client alive wins.
    
# Intended usage:

User interactions will be facilitated through a console interface
  
# Technical Details:


* ASCII/retro style interface with user input on client end; updating console/summary screen tracking rounds and players for server end

* Client will handshake and connect with the server with sockets; servers will store and uniquely mark each client that connects to it

* Successful connections and `-start` command in server console will start a round, which splits into turns for each player to guess a word based on a rng generated first-letter. Turn order is determined by when a client had initally connected to the server

* Servers will then send information about the round to every client and 1 of the clients will be sent a signal to guess a word based on a first letter

* Client will send guess attempt to server, server validates and updates result of the turn to the round's information accordingly. Then it will move on to the next player's turn

* Clients have 3 health max, when clients do not provide a valid word during their turn they lose a life. This is tracked by the server, which will disconnect them after all 3 lives are lost

* A round will end when 1 client remains

**Technologies and Algos to be used**
* multi-socket connections
* file streaming
* sorting algorithm for quickly validating a word based on a db file with different words
* argv and argc to handle server commands (such as starting round, ending, reset, etc)
* Ways to make console look pretty? Real time updating?
* ncurses?

    
# Intended pacing:

**1/8/9** ==> 
Server is working, can send and recieve messages to/from MULTIPLE client. Client is working, can send and recieve messages from server

**1/12/9** ==> 
BASIC game state and game loop is finished: Round based game loop system with time-based turns for each client actively playing. Server keeps track of points and health and input from clients, validates all of this information and updates game state accoringly

**1/15/9** ==> 
Better UI, expanded console commands for the server owner. Optional features such as real-time counters and visually updating word input

**1/16/9** ==> 
Final bug testing, review and organization of codebase. Ensure comments are in the right places

