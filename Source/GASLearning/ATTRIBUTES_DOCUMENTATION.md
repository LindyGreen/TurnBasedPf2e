# GAS Attributes Documentation

## Current Implementation Status

### Completed AttributeSets

#### UCombatAttributeSet
**Location**: `Source/GASLearning/Private/GAS/CombatAttributeSet.h/cpp`
**Purpose**: Core combat statistics for all combatants

**Implemented Attributes**:
- `Health` - Current hit points (clamped 0-MaxHealth, broadcasts changes)
- `MaxHealth` - Maximum hit points 
- `AC` - Armor Class (broadcasts changes only)
- `Fortitude` - Fortitude save (broadcasts changes only)
- `Reflex` - Reflex save (broadcasts changes only)
- `Will` - Will save (broadcasts changes only)
- `Perception` - Perception modifier (no broadcast needed)
  //TODO add character speed here as well. Maybe to-hit and damage as well. 
- //TODO add initiative 
- //Note - damaging abilities don't care much about effects. 
- //Note: I don't have to care about debuffs on damage if no one can produce debuffs on hit and damage.
  **Handler Functions**: All attribute changes are handled in `ACombatant` class with proper logging via `LogGAS` category.

### Planned AttributeSets
#### UActionsAttributeSet
**Purpose**: Number of actions and reactions 
**Status**: Not implemented yet
//TODO move these from Combatant 
//TODO repurpose the listeners to change the initiative like one curently does 

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


### AttributeSet Distribution (reiteration)
- **UCombatAttributeSet**: Assigned to ALL combatants (PCs and NPCs)
- **USkillAttributeSet**: Only assigned to player characters
- **USpellResourceAttributeSet**: Only assigned to spellcasting characters

## Architecture Notes and Issues To Address

### Current Integration
- All AttributeSets are created as components in `ACombatant` constructor
- Event delegates are bound in `BeginPlay()` to handle attribute changes
- `PostGameplayEffectExecute()` handles clamping and broadcasting for each AttributeSet

### Prepared Spells
For prepared casters (like Clerics and Wizards), spell tracking is complex: 
- Each spell can be prepared multiple times
- Need to track individual uses per spell
- May require custom data structures beyond simple attributes
- The Bonded item is its own beast, so but party is Cleric, Rogue, Sorcerer, Fighter (archer).

### Conditions Integration
Current gameplay tags can be integrated with GAS but it's low priority. 
The game can ship without that.

## Testing Status
- ✅ Damage abilities successfully modify Health attribute
- ✅ Attribute change handlers properly log via LogGAS
- ✅ Health clamping works correctly - death is called on HP==0.
- ⏳ Need to implement initial attribute value setting 
- ⏳ Need to test other attribute modifications in bp all at the same time
- ⏳ Need to connect it to my Initiative Tracker 

- 
- Note: No health bar testing. Health bar for player combatants won't be on the character, only npcs
- So -- Logs are enough 
- 