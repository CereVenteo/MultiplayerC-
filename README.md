# Group members:
- Cere Venteo
- Guillem Sánchez

# Game Name:
Shoot the Loser

# Description:
Shoot the Loser is a mini multiplay shooter game created by Cere Venteo and Guillem Sánchez where the loser fight in an arena shooting each other to get the highest possible score before dying.


# Gameplay Tutorial:
Join the server and start shooting other loosers with your gun. 

Controls:
- Press A: Turn left
- Press D: Turn right
- Press Left: Shoot gun
- Press Down: Move forward

# List of implemented features by author:
 Cere
- World State Replication, almost achieved but we don't transfer animations.
- Delivery manager, completely achieved.
- Assets collection
- Bugfix

 Guillem:
- UDP Virtual Connection, completely achieved.
- World State Replication, almost achieved but we don't transfer animations.
- Improving latency handling, completely achieved client side prediction and entity interpolation.
- Bugfix

Disclaimer: we worked together so both of us work in all the ambits but the statment above is more like the guy that put more effort in this part.

Other known bugs: 
- The Time.deltatime isn't a deltatime, depends on the PC where is played, it will have different speeds.
- If spawn a lot the shoot, game crashes.
- We also don't destroy the player, we use the alpha parameter to make them invisible and we also give him inmortality so we don't bug the game.
- It's not a bug, but is different, we use a pong answer instead of sending pings constantly from the server.
