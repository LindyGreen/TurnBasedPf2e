# Turn Manager Documentation

## Action Tracking and Combat Flow Design

### Pre-Combat Setup:
- **Character reaches encounter area** → Visual setup (could be comics/cutscenes hiding the map)
- **Turn manager reads data table** to determine what combatants to spawn and their positions
- **Spawn all combatants at index 000** (hidden/off-screen location with no transform)
- **Move combatants to combat positions** based on data table entries
- **Keep map/UI hidden** during this setup phase

### Initiative Phase:
- **Pre-roll initiative** for all combatants (or roll when "Roll Initiative" button pressed)
- **Sort CombatantArray** by initiative (highest first) using your StartCombat() function
- **"Roll Initiative" button** just reveals the pre-calculated initiative order
- **Show combat UI** and reveal the map to player

### Action Economy Architecture:
- **TurnManager owns all action tracking** (CurrentTurnActions, SpendActions, EndTurn)
- **Combatant handles only its own effects** (conditions, EndTurnEffects, BeginTurn calculations)
- **BeginTurn()** calculates actions based on conditions, sends result to TurnManager
- **SpendActions()** in TurnManager decrements actions, auto-calls EndTurn() when actions ≤ 0
- **EndTurn()** in TurnManager handles turn progression and calls next combatant's BeginTurn()

### Turn Flow:
1. **Current combatant's BeginTurn()** → calculates actions → sends to TurnManager
2. **Player/AI performs actions** → calls TurnManager.SpendActions()
3. **When actions ≤ 0** → TurnManager.EndTurn() automatically triggered
4. **EndTurn()** calls current combatant's EndTurnEffects() → advances to next combatant
5. **Cycle repeats** with wraparound (back to index 0 after last combatant)

### Pre-battle positioning:
- **Menu for later** - no need to worry about it now

### Key Benefits:
- **Clean separation**: TurnManager = flow control, Combatant = self-effects
- **No circular dependencies**: No more combatants asking for their own positions
- **Centralized action tracking**: All economy logic in one place
- **Automatic turn progression**: No manual "end turn" needed when actions spent

## Implementation Status

### ✅ Completed:
- StartCombat() function with initiative sorting
- Action economy refactor (SpendActions moved to TurnManager)
- EndTurn() flow with proper turn progression
- EndTurnEffects() implementation for degrading conditions
- Clean architecture separation

### 🔄 Next Steps:
- Data table reading for spawn configuration
- Combatant spawning system
- Position assignment from data table
- UI integration for initiative reveal
- Combat state management