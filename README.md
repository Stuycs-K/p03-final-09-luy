[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/QfAIDTGi)
# MEGASYMBOLDETHWORDS
 
### wordd???

Yu Lu
       
### Project Description:

A simple recreation of word bomb through networking in C. Servers acting as gamemasters that track rounds, validate words, and points/health of each client as they take turns in guessing the right words. The last client alive wins.

  
### Instructions:

*To setup the game room and 1 potential player:* <br>
make setup <br>
make server <br>
make client <br>
<br>
*To join an active room (assuming server with room is present):* <br>
make setup <br>
make client <br>

**INTERACTIONS:** <br>

*Use the follow commands to control the game from server:* <br>
__-status__ (will print current status of room, along with active clients that have connected successfully) <br>
__-start__ (attempt to start one round of word bomb) <br>
__-end__ (force ends current round, disconnecting all clients and resets game state) <br>

*__Player clients should wait for their turn and play accordingly!__* <br>

### Resources/ References:

[Mr. K's Website - Systems Programming](https://konstantinnovation.github.io/systems.html) <br>
[Beej's guide to network programming](https://beej.us/guide/bgnet/html/)
