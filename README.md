# ArchitectProto

Grey-box prototype of ARCHITECT's room rebuild. The brief is in `CLAUDE.md`.

## First-time setup (Windows, Unreal Engine 5.8)

You need **Visual Studio 2022** with the **"Game development with C++"** workload, because this is a C++ project.

1. Put this folder somewhere of its own, for example `Documents\Unreal Projects\ArchitectProto`, away from STATIC and ECHO.
2. Right-click `ArchitectProto.uproject` and choose **Generate Visual Studio project files**.
   (If it asks which engine to use, pick 5.8.)
3. Double-click `ArchitectProto.uproject`. When it says modules are missing or out of date, click **Yes** to build them.
   This takes a few minutes the first time. If it fails, send me the error text.
4. In the editor: **File > New Level > Basic**. In the Outliner, delete the actor called **Floor**.
   Then **File > Save Current Level As...**, make a folder `Maps` in Content and save as **L_Proto**.

## Step 1 test: the room

Press **Play** (the green triangle). You fly around (WASD, mouse, E/Q up and down).

What you should see:
- An open-fronted grey room, 18 m wide, made of 1 m cubes, though you shouldn't be able to *see* the cubes:
  the floor, back wall, ceiling and side walls should look like smooth, continuous surfaces.
- A warm lamp glow under the middle of the ceiling.
- In the **Output Log** (Window > Output Log): `ArchitectRoom: built 258 pieces (242 visible face panels)`.
- If you fly behind the room or into a wall, the cubes' hidden sides are almost black. That's on purpose.

Tweak it: while playing, press **F8** to detach, click any `Piece_...` or the `ArchitectRoom` in the Outliner,
and look under **Architect** in the Details panel.
