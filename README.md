# Tomb Raider II Multiplayer Mod
 
This is a modification that implements multiplayer functionalities to the game Tomb Raider II using C++17.

This project has two dependencies, [SlikeNet](https://www.slikesoft.com/) and [MinHook](https://github.com/TsudaKageyu/minhook). Compiled library binaries must be placed in the following directory: **"SolutionDir\deps\libs\"** .

Must be compiled in 32 (x86) bits because the modifications are carried out using the client as a DLL.

Credits to SlikeNet, MinHook and [Arsunt](https://github.com/Arsunt/TR2Main) whose project helped me reversing some game structures and functions.

**NOTE: This project is just an early testing version of the mod, there are a few bugs when synchronizing entities such as vehicles and doors. Besides those bugs, the mod can be completely used to play and complete the game with friends.**

**Features included:**

- Full synchronization of players except for "rifle" weapons animations (shotgun, m16, harpoon and grenader launcher).
- Synchronization for PVP (Player vs Player).
- Synchronization of flipped rooms to ensure proper player sync.
· Synchronization of enemies and other entities like boats, skidoos, doors etc.
· Full support for all levels. There is a bug in "Home sweet home" that displays the wrong waist of Lara, it will be fixed in future updates.
- Complete chat to talk (type) with players and commands.
- Many commands for utilities in game, the following ones are a few of them:
    - "boom": makes your lara explode.
    - "name <new_name>": change your name in the server.
    - "tp <player_name>": teleport to other player, you can also type the player's partial name and it will automatically find it.
    - "level <level_name>": teleport to the beginning of the specified level.
    - "ff <state>": toggle friendly fire, an example would be "ff true" to enable it or "ff 0" to disable it.
    - "give_item <item_name>": receive an item.
    - "kill": kill yourself with this command.

**Binaries usage:**

-1. Download the latest version of the mod and extract it anywhere.
-2. If you own a hosted virtual server/dedicated server you should place **_TRIIO_SV.exe_** and **_settings.ini_** together in your server. If you don't own any of this, you could use Hamachi to open your server to other people, as long as both files are together you can place them wherever you please.
-3. Open **_TRIIO_SV.exe_** to initialize the server, once it's initialized you can go ahead and open **_TRIIO.exe_** which is the client that will launch the game and connect to the server. You need to specify the location of the game when you first open the client.
-4. To connect to a server you have to use the command "connect" which can take none or one parameter, in the case you don't specify any parameter it will connect automatically to the last joined server, otherwise you have to specify the ip of the server, an example is **"connect 127.0.0.1"**.

Brief showcase of the mod: https://www.youtube.com/watch?v=Uq37HU1494s
