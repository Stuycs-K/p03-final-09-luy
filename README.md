[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/QfAIDTGi)
# MEGASYMBOLDETHWORDS

Yu Lu

### Video Link:
https://www.youtube.com/watch?v=-VzpGXkBps4

### Project Description:

A simple recreation of word bomb through networking in C. Servers acting as gamemasters that track rounds, validate words, and points/health of each client as they take turns in guessing the right words. The last client alive wins.


### Instructions:

*To setup the game room and 1 potential player:* <br>
>> make setup <br>
>> ./server <br>
>> ./client <br>
<br>

*To join an active room (assuming server with room is present):* <br>
>> make setup <br>
>> ./client <br>
<br>

**INTERACTIONS:** <br>

*Use the follow commands to control the game from server:* <br>

__kill__ (kill all connections) <br>

*__Player clients__* <br>
__score__ (will print current scores of room, along with active clients that have connected successfully, and current player turn) <br>

### Resources/ References:

[Mr. K's Website - Systems Programming](https://konstantinnovation.github.io/systems.html) <br>
[Beej's guide to network programming](https://beej.us/guide/bgnet/html/)
