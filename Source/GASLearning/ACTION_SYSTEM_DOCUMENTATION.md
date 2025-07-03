# Action System Documentation

## Current State: OVERCOMPLICATED MESS
**Status:** Needs complete overhaul/simplification

## What We Currently Have (TO BE DELETED)

### Grid Operation Actions (REDUNDANT - Use Obstacles/GridModifiers instead)
- `Action_AddTile` - Add tiles to grid → **REPLACE:** Add rows functionality
- `Action_RemoveTile` - Remove tiles from grid → **REPLACE:** Place obstacles  
- `Action_IncreaseDecreaseTileHeight` - Modify tile elevation → **REPLACE:** Obstacles with `bUseForTileHeight = true`
- `Action_SetTileType` - Set difficult terrain, etc. → **REPLACE:** Obstacles with appropriate `TileType`

### Unit Management Actions (REDUNDANT - Use TurnManager + Database instead)
- `Action_AddRemoveUnitFromCombat` - Add/remove combat units → **REPLACE:** TurnManager spawning from database
- `Action_SelectTileAndUnit` - Unit selection → **REPLACE:** Initiative system in TurnManager

### Pathfinding Actions (REDUNDANT - Core systems handle this)
- `Action_MoveUnitOnGrid` - Unit movement → **REPLACE:** Simple movement through grid system
- `Action_FindPathToTarget` - Pathfinding → **REPLACE:** Direct pathfinding calls
- `Action_SelectAndReachable` - Show movement range → **REPLACE:** Grid highlighting
- `Action_SelectTile_CalculateMinCost` - Movement cost calculation → **REPLACE:** Pathfinding system
- `Action_ShowTileNeighbor` - Debug tile neighbors → **DELETE:** Debug only

### Combat Actions (POTENTIALLY USEFUL)
- `Action_CastASpell` - Spell casting → **SIMPLIFY:** Direct spell system integration
- `Action_SelectTileAndUnitSpellRange` - Spell targeting → **SIMPLIFY:** Integrated with spell casting

### Debug Actions (DELETE ALL)
- Multiple `Button_Action_*` classes for debug UI → **DELETE:** Not needed for production

## What We ACTUALLY Need (Simple TTRPG Actions)

### Core Action States
```
None → ActionSelected → Execute/Cancel
```

### Essential Actions (3-4 total)
1. **Cast Spell**
   - Input: Click spell button/hotkey
   - State: Show spell targets/range
   - Execute: Left click target → Cast spell
   - Cancel: Right click → Clear selection

2. **Move Unit** 
   - Input: Select unit (through initiative)
   - State: Show movement range
   - Execute: Left click destination → Move unit
   - Cancel: Right click → Clear selection

3. **End Turn**
   - Input: End turn button/hotkey
   - Execute: Pass turn to next unit in initiative

4. **Special Abilities** (maybe)
   - Similar to spell casting but for class abilities

### Implementation Strategy
- **Remove:** All 20+ existing action classes
- **Replace with:** Simple state machine in PlayerController
- **Use:** Direct function calls to existing systems (Grid, TurnManager, Spells)
- **Keep simple:** 3 states, minimal complexity

## Integration Points

### TurnManager Integration
- Handle unit spawning/despawning from database
- Manage initiative order
- Control whose turn it is
- **Expand to include:** Enemy spawning logic

### Grid System Integration  
- Use existing C++ pathfinding for movement
- Use existing highlighting for ranges/targets
- Use obstacles for all terrain modification

### Spell System Integration
- Direct calls to spell casting logic
- Use existing targeting patterns (Burst, Line, Cone, etc.)
- Integrate with action economy

## Migration Plan
1. **Document current mess** ✓
2. **Delete redundant actions** (90% of them)
3. **Build simple 3-state system** 
4. **Test with basic spell casting**
5. **Add movement if needed**
6. **Clean up any remaining complexity**

## Notes
- This system got out of hand because each simple operation became its own class
- TTRPG needs simple click-to-execute, not enterprise action patterns
- Most functionality already exists in core systems (Grid, TurnManager, Spells)
- Keep it stupid simple: Click → Select → Execute → Done