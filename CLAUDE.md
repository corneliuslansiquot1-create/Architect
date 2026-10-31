# ARCHITECT: Claude Code brief (prototype)

Hand this whole file to a new Claude Code session, as CLAUDE.md in the prototype project folder or pasted as the first message. The prompt to start with is at the bottom.

---

## 1. Summary of the idea

**ARCHITECT** (working title) is a PC (Steam) game by Cornelius, built in Unreal Engine 5.

- **Style:** 2.5D side-on puzzle-adventure, like Inside and Limbo. A small dark character moves along a 2D path through a fully 3D world. The camera is continuous and never cuts.
- **Premise:** you're a dream architect who can't wake up. Every room is secretly built from hidden cube pieces.
- **Core loop, per act:**
  1. **Explore and solve (Inside-style):** walk, climb, push and pull, with a few small puzzles.
  2. **Big puzzle:** one larger puzzle ends the act (the last lever, button or weight).
  3. **The rebuild (the signature feature):** solving it makes the room crack along hidden seams. Floor, walls, windows, ceiling and debris come apart into their pieces and rearrange like a Rubik's cube into the next level:
     - **ROTATE:** a section of pieces turns 90° or 180° about one axis, like a cube face. A wall section turns, and the door on it ends up sideways.
     - **FOLD:** a strip of pieces swings 90° on an edge hinge, like a book page. The ceiling folds up and away.
     - **SLIDE:** pieces or rows move along the grid. Columns rise out of the floor as stairs; the exit block slides in from the dark.
     - **SPIN:** a single piece flips 360° in place (spectacle only; it lands where it started).
     - **POP:** pieces jump out of the surface and settle back, in a wave spreading from the lever (spectacle only).

     Many pieces move in staggered waves. Then the seams close and the new space looks seamless again.
  4. **Walk or climb on** into the next level. No loading screen; the room is the transition.
- **The grid is never visible in normal play.** Rooms look like ordinary seamless Inside-style rooms. The grid only shows itself during a rebuild: seams crack open, and the inner faces of the pieces are a dark construction material.
- **Camera:** close on the character during play. During a big rebuild it pulls back moderately (enough to see the room coming apart, never so far the character is lost), then pushes back in. No cuts.
- **Player during a rebuild (prototype scope):** stands on safe pieces that never move. Riding moving pieces comes later.

**Reference video:** `ARCHITECT_concept_v5.mp4` (34 s), showing exactly this sequence with stand-in art.
**Reference choreography:** `index5.html` (a three.js mock-up), which contains a working move list with timings, easing and the pivot maths. Port the logic, not the code.

## 2. What to build: a one-week grey-box prototype

**Goal:** answer one question. *Does the room-rebuild feel like a reward, read clearly, and keep the player in control without feeling like a cutscene?*

Scope, nothing more:
1. **One room** about 18 m wide × 5 m tall × 5 m deep, open towards the camera. It's built from 1 m cube pieces: floor, back wall, ceiling and two side walls.
   - Seamless grey-box materials (one material per surface type).
   - A dark material on the pieces' hidden faces.
2. **A player character** that is side-on and constrained to a plane, can walk, push a crate and pull a lever, and can climb 1 m ledges.
3. **One small puzzle:** push a crate onto a pressure plate, which drops a lever from the ceiling; pull the lever.
4. **One rebuild,** triggered by the lever, using all five move types:
   - at least 3 rotates (wall face turns);
   - 1 fold (ceiling);
   - 1 slide group (stairs rising out of the floor plus an exit block sliding in);
   - pop and spin waves on 30% or more of the pieces;
   - the seam-crack effect at the start and seam-close at the end.
5. **The camera** follows close, eases back during the rebuild, and returns to follow, all without a cut.
6. **After the rebuild,** the player climbs the new stairs and walks into the exit block's lit doorway.

Out of scope: art, audio beyond placeholder, menus, saving, multiple levels, enemies or suspicion, controller polish.

## 3. Recommended technical approach (check each against the installed engine version)

- **Engine:** Unreal Engine **5.8.4** (Cornelius's other projects use it). Confirm the installed version before writing anything version-specific. Start from the Blank (or Third Person) template in a **new, separate project** called `ArchitectProto`. Don't touch STATIC or ECHO.
- **Language:** Blueprints where Cornelius will tweak things (timings, the move list). C++ only if Blueprint performance or complexity demands it. Explain every step for someone learning.
- **Pieces:**
  - an `ArchitectPiece` actor (or component) holding one static mesh cube with grid coordinates (i, j, k), a surface type, and optional attached dressing (door, lamp, radiator, pipe) that moves with it;
  - an `ArchitectRoom` actor that spawns the pieces from a simple layout definition.
  - Start with separate static mesh components. Only move to instancing if profiling shows a problem; it's around 300–500 pieces.
- **The rebuild = a data-driven move list, played at runtime** (preferred over hand-keying hundreds of pieces in Sequencer). Each move is a struct:
  - type: Rotate / Fold / Slide / Spin / Pop;
  - a selector for which pieces it affects (grid ranges);
  - pivot point and axis, or offset;
  - angle, start time, duration, easing.

  The player evaluates all moves each tick. The pose maths (from `index5.html`) is:
  - **Rotate/Fold:** `pos = pivot + R(pos − pivot)`, `rot = R * rot`, with R built from axis and angle × eased progress.
  - **Slide:** `pos += offset × eased progress`.
  - **Spin:** `rot = R * rot` about the piece's own centre.
  - **Pop:** `pos += offset × sin(π × progress)`.
  - Moves on one piece apply in list order. When the rebuild ends, **bake the final transforms** so pieces stay put. Snap them to the grid to kill float drift.
  - The alternative (attach the layer to a temporary pivot actor, rotate it, detach) is also standard for Rubik's-style rotation and fine for structural moves.
- **Seams:** scale every piece from 1.0 to about 0.95 at the start of the rebuild (cracks open) and back to 1.0 at the end.
- **Player:**
  - Character Movement set to constrain to a plane (the planar-movement / "Constrain to Plane" settings), so it stays on the 2D line.
  - Climbing: a simple ledge-detect trace plus a short root-motion or timeline climb is enough for a grey-box.
  - The player's floor pieces are excluded from all moves.
- **Collision:** pieces block the player. Moving pieces must never sweep through the player's capsule; the move list design guarantees that. Check it.
- **Lighting:** fully dynamic (Lumen); no baked lighting. Lights such as the window light are attached to the pieces they belong to.
- **Camera:**
  - **gameplay:** a side-view camera following the player;
  - **rebuild:** blend to a rebuild framing and back (e.g. view-target blending, or an interpolated spring arm length and offset);
  - **no hard cuts.**
- **Trigger:** the lever's interaction fires the room's `PlayRebuild()`. When it finishes, enable the exit door's light and the exit's collision.

## 4. Working rules (from Cornelius; follow them)

- Don't guess or invent engine APIs, settings or menu names. If unsure, say so and check the engine source, docs, or by testing in the editor.
- Never claim something works without running it. Say what was tested and what wasn't.
- Ask before any download, install, marketplace or Fab purchase, plugin install, or change to system or editor-wide settings.
- Save regularly. Use Git if available (ask before creating a remote).
- Keep scope tight: this is a grey-box prototype, not the game.
- Close crash reporters with "Close Without Sending".
- Write clear, commented, beginner-friendly Blueprints or code, and explain where each thing goes and how to hook it up in the editor.

## 5. Acceptance checklist

- [ ] Room spawns from the layout and looks seamless before the rebuild.
- [ ] Crate, plate and lever puzzle works.
- [ ] Rebuild plays: 3+ rotates, 1 fold, slides (stairs and exit), pop and spin waves, seams crack and close.
- [ ] No piece passes through the player; the player keeps control (they can move during the rebuild within the safe area).
- [ ] Camera eases back and returns with no cut.
- [ ] Player climbs the new stairs and enters the exit.
- [ ] Final pieces snap to the grid, with no drift and no gaps.
- [ ] Runs at 60 fps or better in-editor on Cornelius's PC (RTX 4070 Ti SUPER, 32 GB).
- [ ] Timings and moves are editable in one place (data table or array) without touching code.
- [ ] Short screen recording of the sequence for review.

---

## 6. Prompt to paste into Claude Code

> I'm Cornelius. Read ARCHITECT_claude_code_brief.md (this file) and the reference mock-up index5.html, and watch or skim ARCHITECT_concept_v5.mp4 if you can. We're building a one-week grey-box prototype in Unreal Engine of ARCHITECT's core feature: an ordinary 2.5D Inside-style room that, when the player solves a small puzzle, cracks along hidden seams and rebuilds itself like a Rubik's cube (rotate, fold, slide, spin, pop) into the next space, which the player then climbs into without any camera cut.
>
> First:
> 1. confirm the installed Unreal version and template, and propose the project setup (a new project called ArchitectProto, separate from my other projects);
> 2. propose the class and Blueprint structure (ArchitectPiece, ArchitectRoom, the move-list data, the player, the lever, the camera);
> 3. list anything in the brief you're unsure of or can't verify.
>
> Wait for my OK, then build it in small steps, testing each step in the editor and telling me honestly what works and what doesn't:
> 1. the room layout;
> 2. the player;
> 3. the puzzle;
> 4. the move-list player with one rotate;
> 5. all move types;
> 6. the seams;
> 7. the camera;
> 8. stairs and exit;
> 9. polish timing.
>
> Ask before any download, install or plugin. Keep everything editable from one move-list table so I can tune timings myself.
