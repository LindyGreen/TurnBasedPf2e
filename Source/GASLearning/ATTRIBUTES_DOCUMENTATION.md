# GAS Attributes Documentation

## Current Implementation Status

### Completed AttributeSets

#### UCombatAttributeSet
**Location**: `Source/GASLearning/Private/GAS/CombatAttributeSet.h/cpp`
**Purpose**: Core combat statistics for all combatants

**Implemented Attributes**:

##### Health System
- `Health` - Current hit points (clamped 0-MaxHealth, broadcasts changes)
- `MaxHealth` - Maximum hit points 

##### Defenses & Saves (all broadcast changes)
- `AC` - Armor Class
- `Fortitude` - Fortitude save 
- `Reflex` - Reflex save
- `Will` - Will save
- `Perception` - Perception modifier

##### Movement
- `MovementSpeed` - Character movement speed (no broadcast needed)

##### Action Economy (✅ MIGRATED TO GAS)
- `Initiative` - Initiative modifier/roll result
- `ActionsRemaining` - Current actions left this turn (broadcasts changes)
- `MaxActions` - Maximum actions per turn (usually 3, modified by conditions)
- `ReactionAvailable` - Whether reaction is available (0 or 1, broadcasts changes)

##### Attack & Damage System (✅ NEW)
- `AttackBonus` - To-hit modifier for abilities
- `DamageBonus` - Flat damage bonus
- `DamageDie` - Weapon damage die size (d6, d8, etc.)
- `DamageDieCount` - Number of damage dice (1 normally, 2+ with striking runes)

**Handler Functions**: All attribute changes are handled in `ACombatant` class with proper logging via `LogGAS` category.

### Planned AttributeSets
#### USkillAttributeSet
**Purpose**: Skill modifiers for player characters, enemies don't need them
**Status**: Not implemented yet

**Planned Attributes**:
- Acrobatics, Athletics, Arcana, Crafting, Deception, Diplomacy, Intimidation, 
- Lore variants, Medicine, Nature, Occultism, Performance, Religion, 
- Society, Stealth, Survival, Thievery

#### USpellResourceAttributeSet  
**Purpose**: Spell slots and casting resources for spellcasters. 
**Status**: Not implemented yet
**Note**: Spontaneous casters as Slot1/lvl1Slot and prepared as Lvl1Slot1/2/3 etc. **TBD**
**Planned Attributes**:
- Spell slots by level (1st through 3rd level, BBEG might get third level slots) 
- Focus Points (TBD)
- Divine Font (Because I hate myself, why would I make a cleric protagonists?)

### AttributeSet Distribution
- **UCombatAttributeSet**: Assigned to ALL combatants (PCs and NPCs) ✅
- **USkillAttributeSet**: Only assigned to player characters (TBD)
- **USpellResourceAttributeSet**: Only assigned to spellcasting characters (TBD)

## Data Integration Architecture

### Character Data Structure (✅ IMPLEMENTED)
**Location**: `Source/GASLearning/Public/StructsAndEnums/`
- **FS_CompleteCharacterData** - Master data table struct combining all character data
- **FS_CombatAttributes** - Combat stats with weapon attributes
- **FS_CharacterInfo** - Basic character info (name, level, type)
- **FS_Skills** - All PF2e skills
- **FS_SpellResourcesAndAttributes** - Spell slots and casting mechanics
- **FS_CombatantDataAssets** - Visual assets (mesh, animations, portrait)

### GAS Integration Process
1. **Data Table Creation** - Create data table from FS_CompleteCharacterData
2. **Attribute Population** - Initialize GAS attributes from data table values
3. **Ability Granting** - Grant starting abilities based on character data

### Current Integration
- All AttributeSets are created as components in `ACombatant` constructor
- Event delegates are bound in `BeginPlay()` to handle attribute changes
- `PostGameplayEffectExecute()` handles clamping and broadcasting for each AttributeSet
- **NEW**: TurnManager listens to action changes via OnActionSpentInCombatant delegate

### Action Economy Integration (✅ COMPLETE)
- **Initiative**: Migrated from individual Combatant variables to GAS AttributeSet
- **Action Management**: ActionsRemaining, MaxActions, ReactionAvailable all in GAS
- **Delegate System**: OnActionsRemainingChanged, OnReactionAvailableChanged for UI updates
- **Auto Turn End**: TurnManager auto-ends turn when actions <= 0
- **Blueprint Access**: GetActionsRemaining(), GetMaxActions(), SpendAction() functions

## PF2e Combat Library Integration (✅ IMPLEMENTED)
**Location**: `Source/GASLearning/Public/GAS/PF2eCombatLibrary.h`

### Centralized Combat Mechanics
- **EDegreeOfSuccess** enum with PF2e's 4-tier system
- **Natural 20/1 Rules** - Auto-shift degree of success by one step
- **Damage Rolling** - Unified RollDamage(DamageDie, DamageDieCount, DamageBonus)
- **Attack/Save Rolling** - Handles all d20 rolls with proper PF2e mechanics

### Ability Integration
All abilities (BasicMeleeAttackAbility, BasicRangedAttackAbility, BasicDamageSpellAbility) use:
- Centralized damage calculation
- GAS attribute accessors for weapon stats
- PF2e degree of success system

## Testing Status
- ✅ Damage abilities successfully modify Health attribute
- ✅ Attribute change handlers properly log via LogGAS
- ✅ Health clamping works correctly - death is called on HP==0
- ✅ Health works correctly - widget displays it at least 
- ✅ Action economy migrated to GAS with delegate system
- ✅ Centralized combat mechanics in PF2eCombatLibrary
- ✅ All abilities use unified damage rolling system
- 🚧 **CURRENT**: Data table creation and attribute population
- ⏳ Need to test data table → GAS attribute initialization
- ⏳ Need to test ability granting from character data
- ⏳ Need to connect to Initiative Tracker UI