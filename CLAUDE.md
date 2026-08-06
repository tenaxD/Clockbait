# Clockbait – Game Project Brief for Claude Code

## What is this game?
Clockbait is an Unreal Engine 5 multiplayer horror/puzzle game built primarily in Blueprints with a C++ base.

## Core Gameplay Loop
- 2 players, 2 side-by-side rooms: **Safe Room** and **Unsafe Room**
- **Safe Room player** enters a dark room alone with only a flashlight and solves a puzzle (e.g. colored button sequence)
- **Unsafe Room player** has a top-down map/sketch of the Safe Room on their screen and navigates their partner verbally (voice/prox chat)
- The Unsafe Room player also sees the **entity as a red dot** on their own room's map
- Once the puzzle is completed → alarm activates → entity is distracted → doors open
- Both players advance forward → **roles swap** for the next segment
- Previous rooms are unloaded via **Level Streaming**

## Room Layout
```
[Safe Room A] | [Unsafe Room B]
      ↓               ↓
   forward         forward
      ↓               ↓
[Unsafe Room A] | [Safe Room B]   ← roles reversed
```

## What Already Exists (Blueprints)
- **First person movement** (BP_Character or similar)
- **Alarm system** (BP_Alarm):
  - Activated via RMB line trace from camera (Visibility channel)
  - Sphere collision set to block WorldDynamic and Visibility only
  - Stores a NavTarget Scene Component for AI pathfinding destination
  - Has Activate and Deactivate functions
- **AI / Entity**:
  - Chases player when spotted
  - Runs to alarm when activated (reads AlarmLocation from Blackboard as Vector)
  - Deactivates alarm using a custom BTTask that measures distance (BTTask_DeactivateAlarm)
  - Uses AIC_Enemy (AI Controller) + Behavior Tree + Blackboard
- **Behavior Tree keys**: AlarmLocation (Vector), alarm actor reference (Object)

## Multiplayer Plan (not yet implemented)
- Local multiplayer first (2 players in editor via Play > 2 players)
- Will use Unreal's built-in replication system
- Eventually: online via Steam or LAN
- Communication: proximity/radio voice chat (implementation TBD)

## Tech Stack
- Unreal Engine 5
- Blueprints primary, C++ base classes
- NavMesh for AI pathfinding
- Level Streaming for room management
- Target platform: PC

## Naming Conventions
- Blueprints: BP_ prefix
- AI Controllers: AIC_ prefix
- Behavior Trees: BT_ prefix
- BTTasks: BTTask_ prefix
- Blackboard: BB_ prefix

## Current Priority
Setting up basic multiplayer: 2 players in the same world locally in editor, replicated movement and possession.

## Known Issues / Notes
- Alarm NavTarget must be placed manually per instance as a Scene Component offset to the floor in front of the shelf
- Line trace for alarm activation uses Visibility channel; alarm sphere collision must NOT be set to BlockAll
- AI deactivates alarm by distance check (BTTask) not by overlap, because alarm sits off NavMesh on a shelf
