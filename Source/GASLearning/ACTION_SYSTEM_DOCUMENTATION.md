# Action System Documentation

## Current State: GAS-INTEGRATED SYSTEM ✅
**Status:** Successfully migrated to GAS with clean architecture



## NEW Action Economy System (✅ IMPLEMENTED)

### GAS-Integrated Action Management
**Location**: `UCombatAttributeSet` + `ACombatant` + `UTurnManagerComponent`

#### Action Attributes (in GAS)
- `ActionsRemaining` - Current actions left this turn (0-4, broadcasts changes)
- `MaxActions` - Maximum actions per turn (usually 3, modified by conditions)
- `ReactionAvailable` - Whether reaction is available (0 or 1, broadcasts changes)
- `Initiative` - Initiative value for turn order

#### Action Economy Flow
1. **Turn Start**: TurnManager calls `BeginTurn()` on current combatant
2. **Action Calculation**: Combatant calculates actions based on conditions
3. **Action Spending**: Abilities call `SpendAction(ActionCost)` 
4. **Auto Turn End**: When ActionsRemaining <= 0, TurnManager advances turn
5. **UI Updates**: All action buttons update via delegate system

### Ability System Integration (✅ IMPLEMENTED)

#### Base Ability Class: UMyBaseGameplayAbility
**Location**: `Source/GASLearning/Public/GAS/MyBaseGameplayAbility.h`

**UI Data Properties**:
- `DisplayName` - Ability name for buttons
- `Description` - Tooltip text
- `Icon` - Button icon (UTexture2D)
- `ActionCost` - How many actions this ability costs (1-3)
- `Range` - Ability range in grid squares
- `Category` - Attack/Spell/Item/Free for button grouping

#### Implemented Abilities - to modify and build others upon
1. **BasicMeleeAttackAbility** - 1 action, adjacent range/+1 if reach, uses weapon stats
2. **BasicRangedAttackAbility** - 1 action, 30 square range, range penalties
3. **BasicDamageSpellAbility** - 2 actions, 24 square range, save for half

#### PF2e Combat Integration
All abilities use **UPF2eCombatLibrary** for:
- Degree of success calculation (Critical Success/Success/Failure/Critical Failure)
- Natural 20/1 rule handling
- Centralized damage rolling with weapon/spell dice
- Attack roll and save mechanics

### Movement System Design (TBD)
**Decision**: Movement is NOT a GameplayAbility - uses simple function calls
- `BeginMovement()` → Show movement range
- `ExecuteMovement(Destination)` → Move unit + spend 1 action
- Integrates with existing BP pathfinding and animation systems

## Integration Points

### NEW: Combatant/TurnManager Action Architecture ✓
**Implemented:** Clean separation of concerns for action management

#### Combatant Responsibilities:
- Track individual unit conditions (`FGameplayTagContainer Conditions`)
- Calculate available actions based on conditions (`BeginTurn()`)
- Manage personal action state (`bDoesHaveReaction`)
- Handle condition-based action modifications:
  - **Stunned.1/2/3:** Reduced actions (2/1/0) + auto-remove stunned
  - **Slowed.1/2:** Reduced actions (2/1)
  - **Quickened:** Extra actions (4)
  - **Incapacitated:** Skip turn entirely
  - **Dying:** No actions (0)

#### TurnManager Responsibilities:
- Receive calculated actions from Combatant (`CurrentTurnActions()`)
- Track global turn state and initiative order
- Manage turn flow and transitions
- Control action economy validation

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

## UI Integration Plan (NEXT PHASE)

### Widget Architecture Design
- **Initiative Tracker Widget** = master widget with action count display - Requires testing
- **Dynamic Ability Widgets** = spawn/despawn based on current combatant's abilities
- **Single Action Source** = GAS delegates update all buttons when actions change
- **Turn Start Trigger** = get combatant abilities → categorize → spawn buttons

### Button Categories
1. **Movement** - Simple BeginMovement/ExecuteMovement functions
2. **Attacks** - BasicMeleeAttackAbility, BasicRangedAttackAbility (GAS)
3. **Spells** - BasicDamageSpellAbility and future spell abilities (GAS)
4. **Items** - Item usage abilities (GAS)
5. **Free Actions** - Quick actions that don't cost action points

### Data-Driven Approach
- **No switch statements** - abilities automatically expose UI data
- **Ability inheritance** - all abilities inherit from UMyBaseGameplayAbility
- **DataTable integration** - abilities granted based on character data
- **Widget loop**: Get ability specs → Cast to base class → Extract UI data

## Implementation Status
- ✅ **Phase 1**: GAS integration and ability system complete
- ✅ **Phase 2**: PF2e combat mechanics and centralized damage system complete
- 🚧 **Phase 3**: Data table creation and attribute population (CURRENT)
- ⏳ **Phase 4**: UI integration and widget generation
- ⏳ **Phase 5**: Movement system implementation
- ⏳ **Phase 5**: Targeting system implementation

## Notes
- Successfully simplified from 20+ blueprint action classes to 3 core ability types
- GAS provides clean action economy with automatic UI updates
- PF2e mechanics centralized in combat library for consistency
- Data-driven design eliminates hardcoded ability lists