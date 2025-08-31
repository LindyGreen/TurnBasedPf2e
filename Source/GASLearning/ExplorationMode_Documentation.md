# Exploration Game Mode Documentation
Player experience goal - this is the non-combat section of the game. 
It has to break the gameplay and show the world operating.
Here we control the one protagonist - Chad.
Isometric top-down with point click
# TODOs
☐ Create BP_ExpGameMode class                    
☐ Create BP_ExpHUD class
☐ Create BP_ExpChad character class
☐ Create CameraBoundaries checkerBP
☐ Create Edge Scrolling
☐ Create BP_ExpPlayerController class
☐ Create BP_BaseInteractables base class
☐ Create BPI_Interactables interface class
☐ Create BP_InteractableItems class inheriting from Interactables
☐ Create BP_NPC class inheriting from Interactables
☐ Create APB_ExpMovement ability
☐ Integrate Async loading screen plugin
☐ Integrate the Save system






# Features

## Core Systems
- **Movement:** Point-and-click with pathfinding - using navmesh
- **Camera:** Lazy following with manual WASD/border override
- **Dialogue:** Dialogue Tree plugin integration
- **World State:** GameplayTags container in GI_Subsystem (PF2e save Sybsystem BP child)
- **Save System:** Manual saves + auto-save (TBD implementation)
- **Interactions:** Click-to-talk NPCs, hover+always-on interactable highlights

## Technical Architecture
- **Game Mode:** Separate from combat mode (clean separation)
- **Character:** Solo Chad exploration
- **UI:** Blueprint-based for rapid iteration
- **Persistence:** GameplayTags for story flags and world state

## Scope Decisions
- **No inventory system** - use flags for important items
- **No out-of-combat abilities** - MaxHP restoration on combat start
- **No party following** - Chad solo exploration
- **No consumables** - avoid micromanagement

# Design questions that lead to the design document.
## WASD or top down.
WASD pro - more action-like feel, faster to build.
Point and click - more relaxed feel, unified input.
 Decision - Considering input consistency, touchscreen friendliness and genre convention
## Plugin or develop myself
Pro plugin - quicker release. Included systems like UI, dialogue, Working with other's code.
Pro develop myself - increase in skill. Lack of extra features, works as designed.
### Decision
Develop myself. It will be nice to develop it all, and I am going to hate myself for this.
## Camera - simple follow or boundaries.
Simple follow - its simple, but clicking while moving is uncomfortable, and it doesn't follow the combat logic,
Boundaries - slower. but unified input with combat, that will use boundaries for camera movement.
### Decision - Boundaries + WSAD to move camera.
#### Full decision on Camera logic 
Lazy following - camera handles itself most of the time
Manual override - player can take control when needed.
i.e. Click to move -> Set bCameraFollowing=True. Any WASD or border input -> SetCameraFollowingFalse.
On tick lerp the camera if bCameraFollowing=True towards the Chad.
## Dialogue system options
1. Use a plugin like Dialogue tree - commonly used plugin, easy to use, almost an industry standard
2. build one on state trees - I will refresh my skills in state trees, hard to use, delays release
### Decision 
Use the dialogue tree plugin. Quicker release prioritized.

## Inventory
1. Screw inventory, I don't use it anyway. just use flags for important things.
2. Make inventory for Consumables and game items. So people could heal.
### Decision - Screw inventory
Consumables and inventory are overrated and will lead to a scope creep.
Consumables are rarely spent are hard to ballance and micromanagement, slowing the game.
Track the important things in flags. Unimportant things are unimportant. 
Consumables also will require equip functions to use for ones with 2 hands busy.

## World state and Save system
1. use gameplay tags container for flags
2. use an array of enums for flags
### Decision - Gameplay tags
as I am mainly using blueprints for the mode this negates potential issues with typos and tracking
will be a container in my GI subsystem.

## Ability use outside of combat
1. Pros - Ability to pre-buff, heal between fights and stuff.
2. Cons - scope-creep - make a whole ass UI with character switching and tracking it all.  
### Decision - no. MaxHP on combat start, keeps exploration streamlined

## Level transition
### Decision - use ASync Loading Screen Plugin

## NPC Interaction 
1. proximity - can make as barriers, more cinematic - more work.
2. click to talk - can lead to players skipping content - less work
3. hybrid - haha so much work.
### Decision - Click to talk

## Interactable highlights
1. Always on - easiest to make
2. hover - easy to make,
3. hold a button + hover. - industry standard
4. figure out an alternative
### Decision -  Hover+Always on
Always on in a more subtle way, hover in a more direct way. Keeps the controls scheme intact.
I.e. show that it's an interactable, and on hover make it glow and show interaction widget

## Save System
1. 1 Auto save only - easy to make, less ui, less work. Can't load earlier save, so can't avoid bugs this way.
2. Multiple auto saves on key points - easy to make, a bit more work, avoids issue with bugs, as you can load to an early stage.
3. 1 Autosave and manual saves - Longer to make, requires higher code discipline.
### Decision - TBD. Likely option 3. Longer to make, but it will save time during polish and testing phases.

## Party Members
1. Visible, Follow Chad. - Leads to a visual clutter, but opens opportunities, pathfinding issues etc.
2. Party Visible only in combat - Easier, less clutter, verisimilitude issues
### Decision - Solo Chad.
Dialogues happen with comics and UI anyway. Good things from them present:
Opportunity to use abilities outside combat, and quips and barks are not realistic for solo me.
