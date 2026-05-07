# Combat Mechanics Guide

## Overview

This is a **directional parry/block defense system** with **AI enemy stun mechanics**. The player can parry enemy attacks during a tight timing window, stunning the enemy for 5 seconds. Blocking outside the parry window reduces damage by 50%.

https://mmutube.mmu.ac.uk/media/t/1_s9latc2r

## Core Combat Loop

```
1. Enemy detects player → enters Chasing state
2. Enemy gets within attack range → enters AttackWindup state (0.35s)
3. "Parry now!" text appears above enemy
4. Player presses Block button within 0.18 seconds → PARRY SUCCEEDS
5. Enemy is stunned for 5 seconds, takes damage
6. Enemy recovers and resumes chasing
```

---

## Player Mechanics

### Attack
- **Input:** Left Click / Gamepad Attack Button (Make sure you have the sword equipped)

### Block
- **Input:** Right Click / Gamepad Block Button

### Parry
- **Window:** 0.18 seconds (tight timing = skill-based)
- **Timing Start:** When player presses Block
- **Success Condition:** Enemy attack connects while within parry window

### Damage States
| State | Damage Taken | Audio | Effect |
|-------|--------------|-------|--------|
| **Parry** | 0 | ParrySoundAsset | Enemy stunned 5s |
| **Block** | 50% reduction | BlockSoundAsset | Damage reduced |
| **Hit** | Full damage | HitSoundAsset | Take full damage |

---

## Enemy Mechanics

**Special: Stunned State** (when parried)
- Entered when: Player successfully parries enemy attack
- Duration: 5 seconds (configurable via `ParryStunDuration`)
- Effect: Actor tick disabled, all movement frozen, AI logic frozen
- Exit: Automatically returns to Chasing state after timer

### Attack Behavior
- **Activation:** When player enters AttackRange (140 units)
- **Windup Time:** 0.35 seconds (player can see "Parry now!" and react)
- **Recovery Time:** 0.5 seconds (enemy cannot attack again immediately)
- **Damage:** 14 damage per hit (enemy damage value)
- **Audio:** SlimeHitSoundAsset plays on attack connect (slime-specific)

### Status Text
- **Display:** "Parry now!" appears above enemy head during AttackWindup
- **Color:** Yellow
- **Purpose:** Visual cue to player that attack is incoming
- **Clear:** Automatically clears when state changes

---
