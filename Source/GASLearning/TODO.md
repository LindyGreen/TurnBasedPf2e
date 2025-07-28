# TurnBasedPf2e TODO List (AI made, may be outdated on time of printing)

## High Priority (Weekend Sprint - Monday Video)

- [x] **Add GAS blocking tags** to prevent ability overlap 
- [x] Add right mouse button cancel ability for movement
### Animation System
- [x] **Create 1D Blendspace** (Idle → Walk → Run)
- [x] **Implement IsMoving bool** in Animation Blueprint
- [ ] **Set up montage system** for abilities
- [x] **Test movement speed consistency** with new animation system

### Visual Polish for Demo
- [ ] **Basic attack animations** - find montages
- [ ] **Rotate towards Target function** - 
- [ ] **Simple damage numbers** - floating text on hits
- [ ] **Death animations** - PC death montage, AI ragdoll, stand up if not dead
- [ ] **Basic spell VFX** - colored particles/spheres

### SaveSystem and Spawn system
- [ ] Create Blueprint SaveGame class with 4 individual FS_CompleteCharacterData slots (Character1, Character2, Character3, Character4)
- [ ]Create Blueprint functions to save/load  character data to/from save slots
- [ ] Design pre-combat initialization system that loads characters from save file
- [ ] Create character placement logic on grid based on loaded data
- [ ] Initialize ACombatant actors with loaded FS_CompleteCharacterData
- [ ] Add validation for loaded character data (ensure all required fields exist)
- [ ] Create fallback/default characters if save file doesn't exist or is corrupted
- [ ] Add quest progress structure to SaveGame for future use

### UI/UX Polish
- [ ] **Better ability icons** - replace placeholder art
- [ ] **Clean up development buttons** in initiative tracker
- [ ] **Sound effects** - footsteps, hits, spell casting

## Medium Priority (Next Week)

### Ability Implementation

- [ ] **CreateUniversalTemplate** - In progress
- [ ] **Implement basic melee attack** ability  - in progress
- [ ] **Implement basic spell** ability      -    in progress
- [ ] **Add ability cost validation** improvements

### Animation Improvements
- [ ] **Death system refinement** - PC vs AI differentiation
- [ ] **Ability montage integration** with GAS abilities
- [ ] **Shield raising VFX** attachment system

## Technical Debt (Future)

### Code Quality
- [ ] **Fix "overuse of casts"** mentioned in documentation (movement GE)
- [ ] **Improve error handling** in grid generation 
- [ ] **Add more validation** in ability system
- [ ] **Consistent naming conventions** cleanup

### Architecture Improvements
- [ ] **Data table integration** for character abilities (AC_unitData)
- [ ] **Optimize ability button refresh** system (current broadcast approach)
- [ ] **Better separation** between AI and player turns

### Grid System
- [ ] **Fix collision channel setup** for new environments
- [ ] **Improve TraceForGround** reliability(fix block all preset)
- [ ] **Add height validation** for grid placement(more granular height)

## Low Priority (Polish/Nice-to-Have)

### Visual Enhancements
- [ ] **Grid visual improvements** - better materials/shaders
- [ ] **Grid visual improvements** - make slopes
- [ ] **Character portraits** in initiative tracker
- [ ] **Ability tooltips** with detailed descriptions
- [ ] **Hit/miss feedback** visual improvements

### Audio
- [ ] **Background music** for combat
- [ ] **UI sound effects** (button clicks, etc.)
- [ ] **Environmental audio** (footsteps on different surfaces)

### Level Design
- [ ] **Create additional test maps** with varied terrain
- [ ] **Environmental obstacles** and interactive elements
- [ ] **Lighting improvements** for atmosphere

## Documentation Updates
- [ ] **Update CLAUDE.local.md** with animation system decisions
- [ ] **Document ability template architecture**
- [ ] **Create ability creation guide** for future reference

---

## Notes
- **Focus on shipping** - remember this is a $5 indie game
- **Avoid AA-level complexity** - simple solutions that work
- [ ] **Monday video deadline** - prioritize visual polish items
- **Core systems are solid** - 85% custom, GAS working well

**Current Status:** Core turn-based combat system functional, movement working, action economy integrated with GAS, UI updating in real-time. Ready for polish phase! 🎮

## Quick Wins for Demo Video
1. **Movement animations** - 1D blendspace with IsMoving bool ++
2. **Death animations** - montage for PC, ragdoll for AI  
4. **Damage numbers** - floating text on hits
5. **Clean UI** - hide dev buttons, better icons

**Video Script Ready:** Teacher advice → Simple game → CRPG scope → Technical demo with callouts

---
*Last Updated: July 24, 2025*