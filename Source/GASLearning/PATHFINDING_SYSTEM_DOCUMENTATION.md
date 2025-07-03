# Pathfinding System Documentation

## Overview
The pathfinding system is built around the **URangeFinder** component, which serves as the primary computational engine for both PF2e-compliant pathfinding (Hybrid a/Dijkstra) and spell pattern generation. The system has been partially converted from Blueprint to C++ for performance optimization while maintaining Blueprint accessibility.
In case of performance issues - there are a few functions which are in BP 
These functions will benefit from making them multicore and asynch,

## Core Architecture

### URangeFinder Component
- **Type:** `UActorComponent` (BlueprintSpawnableComponent)
- **Purpose:** Primary pathfinding and spell pattern calculation engine (should have been 2 actors, but if it works it works)
- **Integration:** Extensively exposed to Blueprint via `UFUNCTION(BlueprintCallable)` for BP communication and delegates.
- **Location:** `/Source/GASLearning/Public/RangeFinder.h`

### RangeFinderLibrary
- **Purpose:** Contains core data structures and enums
- **Location:** `/Source/GASLearning/Public/RangeFinderLibrary.h`

## Core Data Structures

### FS_PathfindingData
```cpp
struct FS_PathfindingData
{
    FIntPoint Index;                    // Grid coordinates (-999,-999 = invalid)
    int32 CostToEnterTile;             // Movement cost for this specific tile
    int32 CostFromStart;               // G cost in A* algorithm (cumulative from start)
    int32 MinimumCostToTarget;         // H cost in A* algorithm (heuristic to target)
    FIntPoint PreviousIndex;           // Parent node for path reconstruction (-999,-999 = no parent)
};
```

### EAE_SpellPattern Enum
```cpp
enum EAE_SpellPattern : uint8
{
    EAE_SpellPattern_Invalid = 0,      // Debug/default state
    EAE_SpellPattern_Burst = 1,        // 4-directional burst pattern
    EAE_SPellPattern_Line = 2,         // Line pattern (Bresenham's algorithm)
    EAE_SpellPattern_Cone = 3,         // Directional cone pattern
    EAE_SpellPattern_Emanation = 4     // Square area around point
};
```

## Pathfinding Algorithm (Disguisting Algo that probably had a name, but I never learned it)

### Core Variables
```cpp
// Input Parameters
FIntPoint Start;                       // Pathfinding start position
FIntPoint Target;                      // Pathfinding destination
int32 MaxPathLength;                   // Maximum allowed path length
bool Reachable;                        // Whether target is reachable (output)

// A* Algorithm State
TArray<FIntPoint> DiscoveredTileIndexes;      // Open list (tiles to process)
TArray<int32> DiscoveredTileSortedCost;       // F-costs for priority queue
TArray<FIntPoint> AnalizedTileIndexes;        // Closed list (processed tiles)
TMap<FIntPoint, FS_PathfindingData> PathfindingData;  // Complete tile data
```

### Functions

#### Core Algorithm Flow
```cpp
LoopThroughNeighbors()                 // Main A* loop
├── PullCheapestTileOutOfDiscoveredList()  // Get lowest F-cost tile
├── DiscoverNextNeighbor()             // Process each neighbor
│   ├── GetNeighborIndexes()           // Get adjacent tiles
│   ├── RemoveInvalidNeighbors()       // Filter valid moves
│   └── DiscoverTile()                 // Add to open list
└── GeneratePath()                     // Reconstruct final path
```

#### Distance Calculation (PF2e Compliant)
```cpp
int32 TotalCost(FIntPoint OriginPoint, FIntPoint CurrentTile)
```
- **Algorithm:** Chebyshev distance with diagonal penalty
- **Formula:** `Max(|dx|, |dy|) + Min(|dx|, |dy|) / 2`
- **Result:** Diagonal movement costs 1.5x normal movement (PF2e tactical combat rules)
- **Usage:** Heuristic calculation for the hybrid algorithm

#### Priority Queue Management
```cpp
void InsertTileInDiscoveredArray(FS_PathfindingData CurrentTileData)
```
- **Purpose:** Maintains sorted open list for efficient A* processing
- **Implementation:** Uses parallel arrays (DiscoveredTileIndexes + DiscoveredTileSortedCost)
- **Performance:** O(n) insertion, O(1) removal of cheapest tile

#### Neighbor Processing
```cpp
TArray<FIntPoint> GetNeighborIndexes(FIntPoint Index, bool bIncludeDiagonals, TArray<FIntPoint>& Diagonals)
void RemoveInvalidNeighbors(FIntPoint CurrentArrayElement, TArray<FIntPoint>& NeighborsToRemove, FIntPoint Index)
bool IsDiagonal(FIntPoint Index1, FIntPoint Index2)
```
- **GetNeighborIndexes:** Returns 4 orthogonal + 4 diagonal neighbors
- **RemoveInvalidNeighbors:** Prevents wall clipping and validates movement
- **IsDiagonal:** Determines if movement is diagonal for cost calculation

#### Path Reconstruction
```cpp
TArray<FIntPoint> GeneratePath()
```
- **Method:** Backtrack through PreviousIndex chain from target to start
- **Output:** Ordered array of grid coordinates forming the path
- **Usage:** Convert A* result into actionable movement sequence

## Spell Pattern Generation

### Master Function
```cpp
TArray<FIntPoint> GeneratePossibleArray(FIntPoint OriginPoint, FIntPoint CasterLocation, uint8 Area, EAE_SpellPattern Enum, bool bIgnoreOrigin)
```
- **Purpose:** Unified interface for all spell pattern generation
- **Parameters:**
  - `OriginPoint`: Target location for spell effect
  - `CasterLocation`: Caster's position (for directional spells)
  - `Area`: Size/range of the spell effect
  - `Enum`: Which pattern to generate
  - `bIgnoreOrigin`: Whether to exclude the origin point from the pattern

### Pattern-Specific Functions

#### Emanation Pattern
```cpp
TArray<FIntPoint> GenerateEmanation(FIntPoint OriginPoint, uint8 Area, bool bIgnoreOrigin)
```
- **Pattern:** Square area around origin point
- **Size:** (Area * 2 + 1)² tiles
- **Usage:** Healing spells, area buffs, centered effects
- **PF2e Compliance:** Standard emanation pattern

#### Cone Pattern
```cpp
TArray<FIntPoint> GenerateCone(FIntPoint OriginPoint, FIntPoint CasterLocation, uint8 Area)
```
- **Pattern:** Directional cone from caster toward origin
- **Direction:** Calculated from CasterLocation to OriginPoint
- **Support:** Both orthogonal and diagonal casting directions
- **Usage:** Breath weapons, directional spells

#### Line Pattern
```cpp
TArray<FIntPoint> GenerateLine(FIntPoint CasterLocation, FIntPoint OriginPoint, uint8 Area)
```
- **Algorithm:** Bresenham's line algorithm for pixel-perfect lines
- **Length:** Extends from caster to OriginPoint + Area tiles beyond
- **Usage:** Lightning bolts, ray spells, line effects
- **Accuracy:** Mathematically precise line drawing

#### Burst Pattern
```cpp
TArray<FIntPoint> GenerateBurst(FIntPoint OriginPoint, uint8 Area)
```
- **Implementation:** Combines 4 cone patterns from adjacent points
- **Pattern:** Creates cross-shaped burst effect
- **Usage:** Explosion spells, radial effects
- **PF2e Compliance:** Standard burst pattern implementation

## Integration Points

### Grid System Integration
- **Coordinate System:** Uses FIntPoint for grid-based positioning
- **Validation:** Works with Grid.IsIndexValid() for tile validation
- **Movement:** Integrates with Grid coordinate conversion functions

### Blueprint Integration
- **Component-Based:** URangeFinder can be added to any actor
- **Function Exposure:** All major functions available in Blueprint
- **Data Access:** All arrays and variables accessible from Blueprint

### Action System Integration
- **Movement Actions:** Pathfinding provides movement paths for units
- **Spell Targeting:** Pattern generation provides valid spell targets
- **Range Calculation:** Distance functions validate action ranges

## Performance Characteristics
~~~~
### Pathfinding Performance
- **Algorithm:** Hybrid Algo with optimized priority queue
- **Complexity:** O(n log n) where n = number of tiles explored
- **Early Termination:** Stops immediately when target is reached
- **Memory:** O(n) where n = grid size

### Pattern Generation Performance
- **Emanation:** O(area²) - simple nested loops
- **Line:** O(distance) - Bresenham's algorithm
- **Cone:** O(area²) - directional filtering
- **Burst:** O(area²) - combination of cone patterns

### Optimization Features
- **Sorted Arrays:** Parallel arrays for efficient priority queue
- **Map Storage:** O(1) pathfinding data lookup
- **Neighbor Validation:** Prevents unnecessary calculations
- **Blueprint Caching:** Results can be cached in Blueprint variables

## Usage Patterns

### Common Workflows

#### Unit Movement
1. **Setup:** Set Start, Target, MaxPathLength
2. **Execute:** Call LoopThroughNeighbors()
3. **Result:** Check Reachable, call GeneratePath() if true
4. **Apply:** Use path for unit movement animation

#### Spell Targeting
1. **Input:** Caster position, target position, spell data
2. **Generate:** Call GeneratePossibleArray() with appropriate pattern
3. **Validate:** Filter results against valid targets
4. **Highlight:** Use results for visual targeting display

#### Range Calculation
1. **Origin:** Unit's current position
2. **Calculate:** Use GenerateEmanation() for movement/ability range
3. **Filter:** Remove invalid tiles (walls, occupied, etc.)
4. **Display:** Highlight valid tiles for player selection

## PF2e Compliance Features

### Movement Rules
- **Diagonal Cost:** 1.5x movement (handled by TotalCost function)
- **Grid-Based:** Integer coordinates for precise positioning
- **Range Limits:** Configurable maximum movement distances

### Spell Patterns
- **Official Patterns:** Burst, Line, Cone, Emanation match PF2e rules
- **Area Calculation:** Proper tile counting for spell effects
- **Directional Support:** Cone spells work in all 8 directions

### Tactical Combat
- **Turn-Based:** Pathfinding supports turn-based movement
- **Line of Sight:** Foundation for LOS calculations
- **Opportunity Attacks:** Path generation supports movement validation