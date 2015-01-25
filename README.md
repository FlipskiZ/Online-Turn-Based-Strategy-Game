# Online Turn Based Strategy Game

You need at least 2 players on different computers to play. Or, alternatively play alone on 1 computer, but there is no AI in the game.

The client code+exe is located in the /Client directory and the server code+exe is located in the /Server directory.

This game is still in its early stages, so don't expect it to have too many features. Everything that is implemented should work properly though, unless stated otherwise.
This game doesn't represent the final game, and everything is subject to change.

How to play:

Camera movement: WASD
Select building type: 1-4
Place and select buildings: Left click
End turn: Enter
Send chat message: Enter when the chat box is selected

The end goal of the game currently is simply to destroy the enemies capital using weapon buildings.
The first building you place is your capital, you can place this building anywhere as long as it's not beside an enemy building. This building is also free.
You can only place buildings beside buildings you already own.
If you place a building beside an unowned building, it becomes your building.
You start with some amount of each base resource.
It costs resources to place buildings. How much each building costs to place is written on the screen.
When everyone ends their turn, the next turn begins. At the end of each turn every miner building gives you 10 of the resource it is placed on.

Connector buildings are used in order to take ownership of tiles to come close enough to resources in order to mine them or to come close enough to your enemy to destroy them.
Miner buildings are used to harvest resources.
Small weapon buildings are used to attack the enemy buildings.
Big weapon buildings are the same as small weapon buildings but they have a longer range while costing more resources.