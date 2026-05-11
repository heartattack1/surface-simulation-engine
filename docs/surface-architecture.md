# Surface System Architecture (MVP)

## Core Rule: Profile vs State vs Response

The system is organized around a strict separation of concerns:

- **Profile** = what the surface **is** (static authored data).
- **State** = what is happening **now** (dynamic runtime context).
- **Response** = how systems **react** (derived outputs for gameplay/physics/audio/VFX).

This rule is mandatory for all integrations and future extensions.

## Domain Model

### Profile (authoring-time, mostly static)

A surface profile defines persistent material-like behavior, for example:

- friction baseline
- traction class
- impact response parameters
- audio/vfx descriptors

Profiles are reusable assets and should not encode per-frame world state.

### State (runtime, contextual)

State represents transient context at the moment of contact, for example:

- wetness or contamination level
- local temperature state
- temporary modifiers from gameplay events
- per-instance wear/damage context

State is world-owned and expected to change over time.

### Response (evaluation output)

Response is produced at query time and consumed by systems. Typical response fields:

- effective friction / traction
- control modifiers for character movement
- audio event parameters
- VFX intensity / decal hints

Response is never authored directly as a giant static table; it is computed.

## Anti-pattern to Avoid: `enum -> switch`

### Why this is an anti-pattern

A design based on broad surface enums and large `switch` statements causes:

- brittle coupling between all systems
- duplicated logic in CharacterController/Physics/Audio/VFX
- poor extensibility (new surface type requires touching many switches)
- hidden behavior regressions when enum branches diverge

Example smell:

- `SurfaceType::Mud => ...` in CharacterController
- `SurfaceType::Mud => ...` in Physics
- `SurfaceType::Mud => ...` in Audio

This multiplies maintenance cost and breaks single-source-of-truth.

## Target Path: `evaluate(profile, state, contact)`

All runtime decisions should converge to a single evaluation pattern:

`evaluate(profile, state, contact) -> response`

Where:

- `profile` is resolved via `SurfaceId`
- `state` is fetched from world/components at contact location
- `contact` carries query-specific context (normal, relative velocity, impulse, tags, etc.)
- `response` is consumed by each subsystem without re-implementing surface logic

This keeps behavior centralized and composable.

## Ownership Model

Ownership must remain explicit and stable:

- **Registry owns profiles** (authoring/static data store).
- **World components own state** (runtime mutable data).
- Runtime entities/components reference surfaces by **`SurfaceId`**.

Implications:

- Systems do not own profile definitions.
- Systems do not duplicate state snapshots as long-lived truth.
- `SurfaceId` is the stable join key between world data and profile registry.

## Integration Contract

All major runtime consumers integrate through a common evaluation API:

- `SurfaceSystem::evaluateContact(...)`

### Contract expectations

`SurfaceSystem::evaluateContact(...)` should:

1. Resolve `SurfaceId` -> Profile from Registry.
2. Resolve runtime State from world/component context.
3. Accept contact/query input (collision/manifold/controller hit context).
4. Produce a normalized Response payload.
5. Return deterministic results for equal inputs within the same frame state.

### Deterministic / normalized response safeguards

To preserve determinism and normalized output ranges, profile/state validation is split into two classes:

- **hard-error**: registration is rejected; profile cannot enter runtime registry.
- **warning**: registration is allowed, but the issue is logged for tuning follow-up.

Baseline normalization rules:

- Coefficients that feed direct response math (friction/grip/hardness/restitution and environmental slip/softening factors) must stay in documented ranges, typically `[0..1]` (movement/feedback multipliers may use broader documented ranges).
- Curve samples (`Curve.samples`) must stay within normalized `[0..1]` unless an explicitly documented alternative range is introduced.
- Runtime state layers/modulators (`water/snow/ice/dust/oil`, `deformation`, `damage`) are normalized to `[0..1]` and validated accordingly.

This keeps equal input tuples (`profile`, `state`, `contact`) producing stable, bounded outputs across systems.

### Consumer responsibilities

- **CharacterController**: consume movement/control response values only.
- **Physics**: consume physical coefficients/modifiers only.
- **Audio**: consume event/material cues from response, avoid reclassification logic.
- **VFX**: consume visual intensity/type hints from response.

Consumers must not branch on hardcoded surface enums when response is available.

## Future Extension Points (without breaking MVP)

The MVP architecture must remain open for additive enhancements:

### 1) Layers

Allow composition of multiple contributing surfaces/material layers (e.g., base asphalt + puddle overlay) while preserving the same evaluate flow.

### 2) Modifiers

Introduce scoped runtime modifiers (weather, abilities, zone effects, gameplay buffs/debuffs) as inputs to state/evaluation rather than enum proliferation.

### 3) Curves

Support non-linear response curves (e.g., slip vs speed, splash intensity vs impulse) in profile data so tuning can evolve without code rewrites.

## Compatibility Rule for Extensions

Any extension must preserve the MVP contract:

- keep `Profile / State / Response` separation intact
- continue to evaluate through `SurfaceSystem::evaluateContact(...)`
- avoid reintroducing cross-system `enum -> switch` branching

If a proposal violates these rules, it should be rejected or refactored before merge.
