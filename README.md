##	MercuryChess: Toxic Edition.
Online chess with lots of stylesheets spiced by toxic humor, using an innovative data format in it's built-in archiver.
# Pet project or not?
This is a ready for production app. 
Actually, it consists of several different projects: server, gui chess, which uses archiver, which uses finestream.
Archiver will be a closed project, I hope to make it commercial.  
Finestream is available here:   
https://github.com/redmms/finestream.git  
# Unique  
So, what is special about these chess? You've probably seen hundreds of different chess applications.
This app is not only written fully in C++, not only cross-platform due to Qt framework, not only uses various stylesheets and sound effects, 
it is also connected to a server and you can play online with a friend (or an enemy, if you will make him play with you), you may chat, set 
a profile picture, play offline, AND, most 
importantly, watch the history of your games, using not an ordinary .pgn format, but my own .mmd18 data format which compresses
every halfmove to less then 1 byte. PGN halmove takes about 5 bytes, other formats may use 12-16 bits per halfmove, halfmoves in my format take only 5 bits in average, so in the end it is more than 10x more compact compared to PGN.
# Friend offline mode
If you have a chess lover nearby or you just like to spend time in a smart people company (yourself) you can play in friend offline mode (Play->Offline->With friend). Use mouse wheel to scroll history area.
![friend_offline_mode](/decription_media/friend_offline_mode.gif)  
# Training mode
Here (Play->Offline->Training) you can set any start position you want from a FEN string (most popular format for describing board position) to train your skills or test my chess engine, by default it opens you this position https://www.chessprogramming.org/Perft_Results#Position_5
![training_mode](/decription_media/training_mode.gif)  
# Friend online mode 
Set your best picture in profile (Settings->Profile and network) take some flowers and call your beloved to play such an intellectual and developing game (Play->Online->With friend), use chat area on the right to say how much he/she matters to you.
![friend_online_mode](/decription_media/friend_online_mode.gif)  
# History mode
At the moment archiver consists of 2 buttons: "Save game" and "Load game". To save a game you need at first to open the board and play some moves (in friend offline or friend online modes). To load a game you need to choose previously saved game in .mmd18 data format.
![history_mode](/decription_media/history_mode.gif)  
# Constraints
The server is not supposed to be used for playing more than 2 players at the same time, it is a pet project, don't forget it.
I don't get any money for this, and spend my own money on running the server, if you like the project, consider buying me a coffee:  
https://www.buymeacoffee.com/mmd18  
It will inspire me to develop the project.
# Build
If you want to build this project by your own with the archiver, you may probably want to use MS Visual Studio (VS) with Qt VS tools addon, because
VS has the best support of C++20 modules at the moment. You will need to enable Qt 5.15 if you want to hear sounds in the game (there's a bug in Qt 6). Then you will need to use this compiler command /dxifcInlineFunctions- 
because of the MSVC compiler bug, or you can just dispose of "archiver" folder.
