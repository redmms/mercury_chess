This is a ready to be released in production app. 
Actually, it consists of 3 different projects: gui chess, which uses archiver, which uses finestream.
Archiver will be a closed project, I hope to make it commercial.
Finestream is available here: https://github.com/redmms/finestream.git
So, what is special about these chess? You've probably seen hundreds of different chess applications.
This app is not only written fully in C++, not only cross-platform due to Qt framework, not only uses various stylesheets and sound effects, 
it is also connected to a server and you can play online with a friend (or enemy, if you will make him play with you), you may chat, set 
a profile picture, play offline, AND, most 
importantly, watch the history of your games, not using an ordinary .pgn data format, but using my own .mmd18 data format which compresses
every halfmove to less then 1 byte. Imagine it: 1 coordinate is 6 bits at least (64 = 2^6), so 1 halfmove should be 12 bits MINIMUM, 
halfmoves in my format take only 5 bits in average, so in the end it is more than 10x more compact than .pgn.
NOTE:
The server is not supposed to be used for playing more than 2 players at the same time, it is a pet project, don't forget.
I don't get any money for this, and spend own money for the server, if you like the project, consider buying me a coffee:
https://www.buymeacoffee.com/mmd18 
It will inspire me to develop the project.
If you want to build the project by your own with the archiver, you may probably want to use MS Visual Studio, because
it has the best support of C++ modules at the moment. Then you will need to use this compiler command /dxifcInlineFunctions- 
because of the MSVC compiler bug, and you will also need to choose this flag as !!!!, because of Qt6 bug, which doesn't allow you use sounds.
Or you can just dispose of "archiver" folder.