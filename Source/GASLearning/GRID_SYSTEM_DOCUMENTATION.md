# Grid System Documentation

## Overview
The Grid system (AGrid) is the core foundation for the turn-based tactical combat system. It handles tile positioning, world-to-grid conversion, ground detection, and cursor interaction.

## Core Properties

### Grid Configuration
- **`GridStartingPosition`** - World position where the grid begins (anchor point)
- **`FinalTileSize`** - Size of each tile in world units (used for positioning)
- **`TileSize`** - TileSize before adjustments(redundant with final tile size, should've' been one)
- **`GridOffsetFromGround`** - How high above ground the grid sits (default: 2.0) protection from Z-infighting

### Grid Data Storage
- **`GridTiles`** - `TMap<FIntPoint, FS_TileData>` - All tiles with their data
- **`TileStatesToIndexes`** - `TMap<ETileState, FS_IntPointArray>` - Quick lookup for tiles by state, and state holder
- **`bIsHeightFound`** - Internal flag for ground detection logic should have been local var, but isn't.

### Visual Component
- **`MeshHandle`** - Static mesh component to grab the grid by something during level creation

## Core Functions

### Grid Validation
```cpp
bool IsIndexValid(FIntPoint Index) const
```
- **Purpose:** Check if a tile exists at the given grid coordinates
- **Usage:** Validate movement targets, spell targeting, pathfinding
- **Returns:** True if tile exists in GridTiles map

### Coordinate Conversion

#### Grid Index → World Position
```cpp
FVector GetTileLocationFromIndex(FIntPoint GridIndex) const
```
- **Purpose:** Convert grid coordinates to world position
- **Formula:** `GridStartingPosition + (GridIndex * FinalTileSize)`
- **Usage:** Unit positioning, spell effects, visual placement

#### World Position → Grid Index  
```cpp
FIntPoint GetIndexFromWorldLocation(FVector InputLocation)
```
- **Purpose:** Convert world position to grid coordinates
- **Logic:** 
  1. Subtract GridStartingPosition to get local coordinates
  2. Divide by FinalTileSize and round to get grid index
- **Usage:** Pathfinding targets, spell targeting, unit placement
- **Logging:** Logs input and result for debugging

### Cursor/Mouse Integration

#### Get Mouse World Position
```cpp
FVector GetCursorLocationOnGrid()
```
- **Purpose:** Convert mouse cursor to world position on grid plane
- **Multi-tier approach:**
  1. **Primary:** Hit test against GroundAndGridMod channel (ECC_GameTraceChannel1)
  2. **Secondary:** Hit test against Grid channel (ECC_GameTraceChannel2)  
  3. **Fallback:** Mathematical plane intersection at Z=0
  4. **Final:** Return default position (7014, 0, 0)
- **Usage:** Mouse targeting, spell placement, unit selection
- **Logging:** Shows which method succeeded and result position

#### Get Grid Index Under Cursor
```cpp
FIntPoint GetTileIndexUnderCursor()
```
- **Purpose:** Direct conversion from mouse cursor to grid coordinates
- **Implementation:** Combines GetCursorLocationOnGrid() + GetIndexFromWorldLocation()
- **Usage:** One-step mouse-to-tile conversion

### Ground Detection & Height Calculation
```cpp
FVector TraceForGround(FVector PotentialLocation, ETileType& OutTileType)
```
- **Purpose:** Find ground height and tile type at a location
- **Process:**
  1. Line trace from +1000Z to -1000Z at location
  2. Loop through all hits to find GridModifiers and ground
  3. Calculate proper Z height based on grid snapping
  4. Determine tile type from GridModifiers
- **GridModifier Logic:**
  - If `bUseForTileHeight = true`: Use modifier for height calculation
  - Set `TileType` from modifier properties
  - Grid snap hit location Z + GridOffsetFromGround
- **Ground Logic:**
  - If no height modifier found, use ground hit location
  - Grid snap + GridOffsetFromGround for final height
- **Returns:** Proper world position with correct Z height
- **Usage:** Tile spawning, unit placement, height-accurate positioning

### Grid State Management

#### Get Tiles by State
```cpp
TArray<FIntPoint> GetAllTilesWithState(ETileState State)
```
- **Purpose:** Find all tiles with a specific state (Selected, Highlighted, etc.)
- **Usage:** Movement range display, spell targeting, pathfinding results
- **Performance:** Uses indexed lookup via TileStatesToIndexes for efficiency

#### Add State to Tile
```cpp
void AddStateToTile(FIntPoint TileIndex, ETileState State)
```
- **Purpose:** Add a state to a specific tile and update visual representation
- **Process:**
  1. Validate tile exists in GridTiles
  2. Get local copy of tile data
  3. Add state to tile's TileStates array (unique only)
  4. Update GridTiles with modified data
  5. Update TileStatesToIndexes for efficient lookups
  6. Call GridMeshInstance->UpdateTileVisual() for visual updates
- **Usage:** Highlighting tiles, marking movement range, spell targeting
- **Visual Integration:** Automatically triggers visual updates through GridMeshInstance
- **Note:** Requires GridMeshInstance to be set for visual updates to work

#### Calculate Spawn Positions
```cpp
FVector CalculateSquaresSpawnLocation(FIntPoint GridIndex) const
```
- **Purpose:** Calculate where to spawn tile visuals/objects
- **Formula:** `GridStartingPosition + (GridIndex * FinalTileSize) + 0.01Z`
- **Usage:** Tile visual placement, slightly above grid plane

### Grid Initialization
```cpp
void CalculateStartingPosition()
```
- **Purpose:** Snap grid starting position to proper grid alignment
- **Process:**
  1. Grid snap current GridStartingPosition to FinalTileSize boundaries
  2. Update MeshHandle position to snapped location
  3. Store snapped position as new GridStartingPosition
- **Usage:** Grid setup, ensuring alignment with visual materials

## Integration Points

### GridModifier System
- Grid works with AGridModifier actors for height and tile type modification
- GridModifiers can override ground height calculation
- Tile types (Normal, Difficult, Impassable) determined by modifiers

### GridMeshInstance Integration
- **`GridMeshInstance`** - Reference to UGridMeshInstance component for visual updates
- Grid class calls GridMeshInstance->UpdateTileVisual() when tile states change
- AddStateToTile() automatically triggers visual updates through this integration
- GridMeshInstance handles instanced mesh rendering and tile coloring

### Collision Channels
- **ECC_GameTraceChannel1:** GroundAndGridMod (primary mouse detection, as you hit mod first)
- **ECC_GameTraceChannel2:** Grid (secondary mouse detection)

### Data Structures
- **`FS_TileData`:** Individual tile information
- **`ETileType`:** Normal, Difficult, Impassable, None
- **`ETileState`:** Selected, Highlighted, Reachable, etc.
- **`FS_IntPointArray`:** Array wrapper for efficient state indexing

## Usage Patterns

### Common Workflows
1. **Mouse to Tile:** GetTileIndexUnderCursor() → Validate with IsIndexValid()
2. **Unit Movement:** GetIndexFromWorldLocation(unit pos) → pathfinding → GetTileLocationFromIndex(target)
3. **Spell Targeting:** GetCursorLocationOnGrid() → range calculation → highlight tiles
4. **Tile Creation:** TraceForGround() for height → spawn at CalculateSquaresSpawnLocation()

### Performance Notes
- Grid coordinate conversion is optimized (direct math, no unnecessary operations)
- State lookups use indexed maps for O(1) access
- Ground tracing may be expensive with many GridModifiers, A bunch of traces after all.
This operation is done during grid initialization only once, so might require hiding

## Dependencies
- **GridModifier.h:** For height and tile type modification
- **KismetMathLibrary:** For grid snapping and plane intersection
- **GameplayStatics:** For player controller access
- Various enum/struct headers for data types