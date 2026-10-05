# KHCoM Android enhancement research notes

Research date: 2026-10-05  
Android development baseline inspected: `android-port` at `6521927fba98535feef3cfe8148a0725e5fc46f0`  
Target device: Retroid Pocket 5, Android 13, Snapdragon 865 / Adreno 650  
Native core remains ARMv7 / `armeabi-v7a`.

> This is a development note, not a release document. It is stored on a separate branch so ordinary Android builds are not triggered by research-note edits. The repository itself is public, so this file is **not secret** even though it is intentionally unlinked from the user-facing README.

## Executive findings

1. Replacing the current Java Canvas presenter with OpenGL ES is the correct foundation for live graphics options.
2. The current Android `port/android/host/ppu.c` and `port/android/include/ppu.h` are byte-for-byte identical to the current Vita widescreen-capable versions inspected from `ggdrn/KH_COM_VITA`. This is the most important finding: much of the difficult PPU-side widescreen rendering logic is already present in Android.
3. Android currently never activates that widescreen machinery because the host frame buffers, JNI copy path, Java view and game-side stream/camera hooks are still effectively fixed to 240x160.
4. Sharp Pixels and GBA-color correction can be ported directly from the Vita shader logic to GLSL ES.
5. Scale3x can be ported directly from the Vita MIT-licensed implementation or reimplemented as a GPU shader.
6. ScaleFX and classic xBR are attractive later filters because permissively licensed shader implementations exist. xBRZ needs more caution because common implementations incorporate GPLv3 code.
7. The Vita port already provides concrete, tested QoL designs for dedicated dodge, right-stick card control, one-button L+R, unstocking cards, field HP, Moogle-point notices, skip intro, wide menus and extra save banks.
8. The current Android native lifecycle has no real stop/restart function. A launcher can exist before game start immediately, and an in-game settings overlay is easy to add, but “return to launcher and start a different ROM without restarting the process” needs additional native lifecycle work.

---

# 1. Current Android rendering path

## Current frame path

The Android build currently does:

```
GBA game logic
  -> software PPU
  -> native 240x160 RGBA frame
  -> AndroidHostCopyFrame()
  -> JNI NativeBridge.copyFrame()
  -> Java direct ByteBuffer
  -> Bitmap.copyPixelsFromBuffer()
  -> Canvas.drawBitmap()
  -> RP5 screen
```

Relevant files:

- `port/android/host/android_host.c`
- `port/android/host/android_jni.c`
- `port/android/app/src/main/java/com/narmanb/khcomandroid/NativeBridge.java`
- `port/android/app/src/main/java/com/narmanb/khcomandroid/GameView.java`

`GameView` creates a 240x160 `Bitmap`, disables bitmap filtering with `paint.setFilterBitmap(false)`, and scales the bitmap to the largest 3:2 rectangle that fits the screen. On a 1920x1080 panel that becomes 1620x1080, a 6.75x non-integer enlargement.

That means the current display is crisp nearest-neighbor but not pixel-perfect: source-pixel boundaries must land unevenly on physical pixels.

## Current native frame limitations

`android_host.c` currently allocates:

- `sRenderRgba[240 * 160]`
- `sPublishedRgba[240 * 160]`

and calls:

```c
PpuSetOutput(..., GBA_SCREEN_WIDTH, GBA_SCREEN_WIDTH);
```

The JNI path also rejects buffers smaller than exactly `240 * 160 * 4`.

These hardcoded dimensions are the immediate blockers for activating 282-wide Vita-style rendering.

---

# 2. Recommended OpenGL ES migration

## Why GLSurfaceView

Android's own documentation describes `GLSurfaceView` + `GLSurfaceView.Renderer` as the straightforward full-screen OpenGL ES integration. The renderer runs on a dedicated thread, separate from the UI thread, and Android manages EGL/surface lifecycle.

Official references:

- https://developer.android.com/develop/ui/views/graphics/opengl/environment
- https://developer.android.com/reference/android/opengl/GLSurfaceView
- https://developer.android.com/reference/android/opengl/GLSurfaceView.Renderer

This is preferable to writing custom EGL/SurfaceView management first because the existing app is simple Java and controller input already lives in a View.

## Proposed pipeline

```
native game / software PPU
  -> latest RGBA source frame (240x160 or 282x160)
  -> NativeBridge.copyFrame() into direct ByteBuffer
  -> GLES texture upload
  -> one or more shader/FBO passes
  -> final fullscreen quad
```

Recommended first implementation:

- Java class: replace `GameView extends View` with something like `GameGLView extends GLSurfaceView`.
- Keep all current controller event handling in that class.
- Use OpenGL ES 3.0. API level 23 is already well above the Android level that introduced ES 3.0.
- Allocate the source texture once.
- Update it each frame with `glTexSubImage2D`.
- Render one textured fullscreen quad.
- Start with `RENDERMODE_CONTINUOUSLY`; it is the lowest-risk 60 Hz integration.
- Keep the game thread independent from the GL thread exactly as it is now.
- Keep the native complete-frame mutex/handoff so GL never samples a half-written PPU frame.

A 282x160 RGBA source uploaded at 60 fps is only about 10.8 MB/s of source texture data. That is tiny for the RP5 hardware. The expensive work, if any, should remain on the GPU after this upload.

## Native changes needed for dynamic frame width

Use `PORT_MAX_SCREEN_WIDTH` (already 288 in the widescreen PPU headers) for native buffers:

```
PORT_MAX_SCREEN_WIDTH * GBA_SCREEN_HEIGHT
```

Add current-frame metadata, for example:

- width
- height (currently always 160)
- frame counter

Possible API:

```c
uint32_t AndroidHostCopyFrame(uint32_t* dst, uint32_t lastFrame, int* width);
```

or separate JNI accessors:

```
NativeBridge.getFrameWidth()
NativeBridge.copyFrame(...)
```

Better long-term design: a single compact frame-info JNI call or width returned through an integer pair.

## OpenGL texture format

Current native output is RGBA8888 in a Java-compatible direct buffer. Keep that first. Avoid changing PPU pixel format and renderer architecture simultaneously.

---

# 3. Display/scaling modes

These should be independent choices where possible.

## Aspect / scene mode

- Original 3:2, 240x160
- Vita-style widescreen, 282x160
- Optional stretch mode for users who explicitly want it

## Presentation scale mode

### Fit / Fill Height

Current behavior conceptually: fill available height while maintaining source aspect ratio.

Original mode on 1920x1080:
- 240x160 -> 1620x1080
- scale = 6.75x

Vita-style widescreen:
- 282x160 -> about 1903.5x1080 at 6.75x
- only about 8 pixels of unused border on each side of a 1920-wide display

### Pixel Perfect

Use the largest whole-number scale that fits both dimensions.

Original 240x160 on 1920x1080:
- 6x = 1440x960
- 240 px border each side
- 60 px border top and bottom

Widescreen 282x160:
- 6x = 1692x960
- 114 px border each side
- 60 px border top and bottom

This should be an option, not the default requirement.

### Sharp Pixels

This is not the same as current nearest-neighbor.

At a fractional scale like 6.75x, plain nearest-neighbor has to make some source pixels cover 6 physical pixels and others 7. The Vita shader instead keeps most of each source pixel solid and limits interpolation to a narrow subpixel boundary region, producing visually even spacing without the full blur of bilinear filtering.

The current Vita Cg logic is in:

- `ggdrn/KH_COM_VITA/port/vita/host/vita_video.c`

Porting to GLSL ES 3.0 is straightforward:

- `float2` -> `vec2`
- `tex2D` -> `texture`
- explicit fragment output
- add ES precision qualifiers

This should be one of the first shaders implemented.

### Nearest

Keep the current visual style as a selectable fallback/reference.

### Linear

Keep ordinary bilinear filtering as another baseline. It will look softer but is useful for comparison and for users who prefer it.

---

# 4. GBA color correction

The Vita shader also has optional GBA LCD color correction.

The implementation:

1. roughly linearizes by squaring RGB,
2. applies a color-mixing matrix,
3. reduces brightness slightly,
4. square-roots back,
5. mixes the corrected result with the original at 50%.

This is already implemented and MIT-licensed in the Vita port. It can be translated with Sharp Pixels into the same GLSL ES fragment shader.

This should be independent from the upscale algorithm:

- Original colors
- GBA-corrected colors

---

# 5. Scale3x research

Vita source:

- `ggdrn/KH_COM_VITA/port/vita/host/scale3x.c`

The Vita implementation is a classic Scale3x/EPX-style edge-aware algorithm. For each source pixel E it inspects neighboring pixels A-I and constructs a 3x3 output block. When the surrounding pattern suggests a diagonal, matching neighbor colors are used in selected output positions, smoothing stair-step diagonals while preserving the source palette.

The Vita implementation is small and simple. Vita-specific parts are mainly its output-copy optimization (`sceClibMemcpy`). On Android that can be replaced with ordinary `memcpy` if using a CPU path.

## Two Android implementation options

### A. CPU Scale3x

Pros:
- easiest to validate against Vita
- known behavior
- can reuse algorithm nearly directly

Cons:
- creates a 3x source frame before upload
- 282x160 becomes 846x480
- still easy for RP5, but adds unnecessary memory traffic

### B. GPU Scale3x shader

Pros:
- source frame uploaded only once at low resolution
- no large CPU-scaled frame
- integrates cleanly with later post-processing

Cons:
- must carefully reproduce Scale3x neighborhood rules in GLSL

Recommendation: once OpenGL ES is in place, use a GPU Scale3x implementation unless exact parity testing strongly favors the CPU port.

---

# 6. Other enhancement filters researched

## ScaleFX

ScaleFX is specifically designed for pixel art and was originally conceived as an improvement on Scale3x. It reconstructs smoother slopes while keeping colors from the original source.

The commonly used reference implementation has an MIT-style permissive license.

Reference:
- https://github.com/libretro/common-shaders/blob/master/scalefx/shaders/scalefx-pass1.cg
- https://github.com/libretro/glsl-shaders/tree/master/scalefx

Important implementation note: ScaleFX is multi-pass. Therefore the Android GL renderer should be designed from the start to support ping-pong framebuffer objects rather than assuming every filter is a single fragment shader.

Priority: high after Sharp Pixels / Scale3x.

## xBR

Classic xBR shader variants by Hyllian use a permissive MIT-style license and are another good candidate.

Reference:
- https://github.com/libretro/glsl-shaders/tree/master/xbr

xBR tends to produce a smoother, more illustrated look than plain Scale3x. This may suit KHCoM's existing outlined/cartoon art.

Priority: medium/high experimental option.

## xBRZ

Be careful.

Common xBRZ implementations and shader ports incorporate GPLv3-licensed xBRZ code. A typical libretro 6xBRZ shader explicitly includes GPLv3-derived xBRZ concepts/code even though other parts of the same shader are permissively licensed.

Reference:
- https://github.com/libretro/glsl-shaders/blob/master/xbrz/shaders/6xbrz.glsl

Do not copy xBRZ into the Android port casually. Resolve licensing first. Prefer permissive ScaleFX or xBR unless there is a strong reason to take on GPL obligations.

## Anime4K

Anime4K is MIT-licensed and provides GLSL implementations.

Reference:
- https://github.com/bloc97/Anime4K

It is designed for anime/video rather than low-resolution GBA pixel art. It may be worth an experimental preset later, but it should not be an early dependency. It could over-process text or introduce a look that is less faithful than ScaleFX/xBR.

Priority: low/experimental.

---

# 7. Cartoon / toon-style post-processing

True 3D cel shading is not possible from the final KHCoM frame because there are no surface normals, depth buffers or 3D lighting information. The PPU hands us a finished 2D image.

A convincing **toon-style 2D post-process** is still practical.

Recommended pipeline:

```
source frame
 -> edge-aware upscale (Scale3x / ScaleFX / xBR)
 -> optional color quantization/posterization
 -> optional edge darkening
 -> final sharp presentation
```

Possible custom shader controls:

- cartoon strength
- color levels (for example original / 16 / 8)
- outline strength
- saturation
- contrast

A custom shader can be written in-house, avoiding outside licensing concerns.

## Edge detection

A simple luminance/color-gradient pass or Sobel-like 3x3 neighborhood can darken strong edges.

Do not make outlines too strong. KHCoM sprites already contain deliberate dark outlines; aggressive edge detection would double-outline text and UI.

## Important limitation: UI is already composited

The current software PPU produces one finished image containing:

- game world
- characters
- cards
- menus
- dialogue
- HUD
- text

A post-process sees all of it. A strong cartoon filter may improve characters while hurting card numbers or text.

Short-term solution:
- conservative presets
- allow instant live switching
- perhaps disable aggressive toon processing automatically during selected UI-heavy modes later

Long-term solution if needed:
- provide masks/layer information from the software PPU so post-processing can distinguish world graphics from UI
- this is significantly more work and should not block the first shader system

---

# 8. True widescreen research

## Major finding: Android already contains the widescreen-capable PPU

The following Android files were compared with the current Vita equivalents and were byte-for-byte identical at research time:

- Android: `port/android/host/ppu.c`
- Vita: `port/vita/host/ppu.c`

and:

- Android: `port/android/include/ppu.h`
- Vita: `port/vita/include/ppu.h`

Those files already contain:

- variable output width
- `xoff`
- `PpuBgStream`
- streamed-map margin rendering
- sprite rendering in extra columns
- panel/message-window margin handling
- UI clipping flags
- scene-margin handling
- wide-menu metadata
- max width 288

Therefore widescreen is **not** a ground-up renderer rewrite on Android.

## What Android is missing

### 1. The game-side widescreen stream capture

Vita file:

- `port/vita/game/widescreen.c`

This captures field/map background streams and exposes extra map columns to the PPU.

It supports:
- normal streamed maps
- card-built field rooms that do not have a single full map
- sliding message panels
- camera-relative margin tiles

Android does not currently have this file.

### 2. Port interface declarations

Android `port/android/include/port.h` is much smaller than Vita's and currently lacks:

- `PortSetBgStream`
- `PortCaptureBgStreams`
- `PortSetBgPanel`
- `PortBgPanelMap`
- `PortUiOverlay`
- `PortWideMenu`
- `PortModeStart`
- `PortWideMargin`
- related QoL hooks

These need Android implementations.

### 3. VBlank stream-capture call

Vita `VBlankIntrWait()` calls `PortCaptureBgStreams()` immediately after the VBlank handler has flushed scroll registers/VRAM, so the captured camera corresponds to the next rendered frame.

Android `port/android/game/gba_system.c` currently does not make this call.

### 4. Host frame-state integration

Vita host/render code tracks:
- current output width
- background streams
- whether sprites should be clipped
- whether scene margins should be filled
- whether a menu should be treated as wide

Android `android_host.c` currently captures only IO/palette/OAM/VRAM and renders at 240.

The Android host needs equivalents of the relevant small state functions from Vita's `vita_render.c`.

### 5. Game-source hooks

The Vita patch contains the source hooks that make widescreen robust. Important examples:

- `src/engine.c`
  - keep sprites alive in the extra horizontal margins
  - widen screen-outside tests
- `src/btl/btl_map.c`
  - change battle camera edge clamping based on `PortWideMargin()`
- `src/evt/event_message.c`
  - mark message window maps as sliding panels
- `src/card/card_msgwin.c`
  - same for card-message windows
- `src/map/map.c`
  - wide field menus/save menus
- `src/card/card_worldselect.c`
  - mark full-screen UI overlays so world margins/sprites do not leak behind them
- mode-start hooks
  - distinguish title/logo screens and menu screens
- map/background hooks used by `widescreen.c`

These should be ported under a shared non-GBA platform macro where practical instead of duplicating `PLATFORM_VITA` and `PLATFORM_ANDROID` versions forever.

## Initial Android widescreen target

Use the Vita-tested width first:

```
282 x 160
```

That is 21 extra source pixels on each side.

On a 1920x1080 display at 6.75x height scaling:
- width is ~1903.5 pixels
- visually almost full-screen
- no horizontal stretching required

Do **not** immediately change the tested source width to 284/285 simply to chase exact mathematical 16:9. First establish parity with the known-good Vita behavior.

## Runtime toggle

Presentation filters can change instantly.

Widescreen can probably also become live-switchable because the PPU supports variable width, but first implementation should switch width only at a frame/VBlank boundary. If any game-side camera/HUD state proves sensitive, mark the widescreen toggle as “applies on next scene” or “restart game” until proven safe.

---

# 9. Vita QoL features worth porting

Vita source/research base:
- `ggdrn/KH_COM_VITA/README.md`
- `port/vita/host/vita_input.c`
- `port/vita/host/vita_notice.c`
- `port/vita/host/vita_menu.c`
- `patches/khcom-vita.patch`

## A. Dedicated dodge-roll button

Vita behavior:
- Square press sets a short-lived `gPortDodgePressed`.
- In `src/btl/btl.c`, Sora/Riku battle movement consumes it.
- It acts like the second tap of the game's normal left/right double-tap dodge logic.
- If a horizontal direction is held, roll that direction.
- Otherwise roll the direction the character faces.
- The original game's state machine still decides whether rolling is currently allowed.

Android plan:
- map a configurable face button to a special dodge request
- do not fake arbitrary repeated D-pad key events in Java
- keep the game-side state-machine hook, as Vita does
- add Sora and Riku support together

Difficulty: low/moderate because the working game-source patch exists.

## B. Right-stick card control

Android official controller mapping uses:
- right stick horizontal: `AXIS_Z`
- right stick vertical: `AXIS_RZ`

Official reference:
- https://developer.android.com/games/sdk/game-controller/controller-input

Vita mapping:
- right stick left -> GBA L
- right stick right -> GBA R
- right stick up -> L+R
- right stick down held for 1 second -> unstock

Vita also uses:
- engage threshold
- release threshold
- short settle period before locking a direction

This avoids diagonal/noisy stick motion causing accidental card changes.

Android should adopt the same stateful interpretation rather than treating every raw axis event as a button.

Difficulty: low for left/right/up, moderate with unstock integration.

## C. One-button L+R / stock-sleight

Vita Triangle synthesizes L+R.

Android can expose this as a remappable special action and default it to an unused RP5 face button.

Difficulty: very low.

## D. Return stocked cards to hand

Vita behavior:
- hold right stick down or rear touch for ~1 second
- progress bar shown under stocked cards
- validates that cards are in a safe stock state
- clears stock state and rebuilds the hand at the same cursor position

Relevant patches:
- `src/btl/battle_runtime.c`
- `src/card/card_battle.c`
- `include/card/card_battle.h`
- host notice/progress logic

This is a real gameplay QoL change, not merely input remapping.

Difficulty: moderate; working Vita implementation exists but should be audited against the current Android reload/null fixes before copying.

## E. Field HP display

Vita reuses the game's own battle HP art while exploring.

Relevant:
- `src/btl/btl2.c`
- `src/map/map.c`

It allocates the same portrait/gauge assets and updates from `gGameState.hp`.

Difficulty: moderate because it interacts with sprite allocation and menu layering, but reference implementation is complete.

## F. Moogle points pickup notice

Vita hooks point collection in:
- `src/map/map_tasks.c`
- `src/poo/poo.c`

and displays gained points + total through the port overlay.

Difficulty: low once an Android overlay/text system exists.

## G. Skip intro

Vita can skip copyright/logo/title intro flow and enter the title menu.

Relevant title-mode patch:
- `src/title/mode_copyright1.c`
- title auto-start support

Difficulty: low.

## H. Extra save banks

Vita exposes 5 save banks, each containing the game's normal 2 save slots, for 10 logical save slots.

Android already stores SRAM separately from the APK, so multiple bank files are conceptually easy. Safe live switching requires:
- flush current SRAM
- load selected bank
- run game-side save verification/repair hook
- prevent switching during unsafe save activity

Difficulty: moderate. Do later than visual/input QoL because save corruption risk is more serious than a graphics bug.

## I. Wide menus

Vita optionally stretches specific menu screens across the widescreen output while leaving gameplay world rendering genuinely wide.

This is separate from “stretch the whole game.”

Difficulty: moderate but largely contained in existing PPU + mode/menu hooks.

---

# 10. Android launcher and in-game settings architecture

## Current behavior

`MainActivity.onCreate()` eventually calls `launchGame()`.

`launchGame()`:
- if internal `rom.gba` exists -> immediately starts native game
- otherwise -> immediately opens file picker

This can be replaced cleanly with a launcher before touching native game logic.

## Launcher proposal

Initial screen:
- Play / Resume
- ROM status + Change ROM
- Settings
- Diagnostics / About

Keep selected ROM in internal app storage exactly as today.

## Shared settings storage

Use Android `SharedPreferences` initially.

Reasons:
- Java app is small
- no need for a new AndroidX/DataStore dependency just to store simple toggles/integers
- easy to read both in launcher and in-game overlay

Categories:

### Video
- aspect: original / widescreen / stretch
- scaling: fit / pixel perfect
- filter: nearest / linear / sharp pixels / Scale3x / later ScaleFX/xBR
- GBA colors
- toon preset/strength later

### Controls
- dedicated dodge
- one-button L+R
- right-stick card control
- unstock hold
- deadzone / sensitivity
- button remapping later

### Game/QoL
- field HP
- Moogle notice
- skip intro
- save bank
- wide menus

## In-game settings

Best first approach:
- Android overlay/menu, not a replacement for KHCoM's Start menu
- pause native game with existing `NativeBridge.setPaused(true)`
- show settings over the GL surface
- visual settings update immediately where safe
- resume with `setPaused(false)`

This avoids consuming a GBA button chord and avoids modifying the game's own menu.

## Native lifecycle caveat

Current JNI/native state has:
- global `sStarted`
- detached native game thread
- no clean stop function
- `AgbMain()` is assumed never to return

Therefore:
- pre-game launcher is easy
- overlay settings while running are easy
- “Return to launcher” can initially mean pause/hide the running game and resume it later
- changing ROM and creating a truly fresh native session without process restart requires a dedicated shutdown/restart design

Do not promise seamless ROM switching after a native session until this is implemented.

---

# 11. Recommended graphics architecture for future filters

Design the GL renderer for multi-pass use from the beginning.

Suggested structure:

```
source texture (240/282 x 160)
    |
    +-- nearest/linear/sharp -> screen
    |
    +-- Scale3x/ScaleFX/xBR -> FBO A
                               |
                               +-- optional toon/color pass -> FBO B
                                                              |
                                                              +-- final presentation -> screen
```

Use two reusable ping-pong FBOs sized for the currently selected intermediate scale.

This allows:
- one-pass sharp-bilinear
- multi-pass ScaleFX
- custom posterize/outline pass
- later CRT/LCD effects
without rewriting the renderer each time.

---

# 12. Recommended implementation order

Do not implement all of this in one APK.

## Phase 0 - current stability
Confirm the current card reload/empty-card crash fix works in normal gameplay.

## Phase 1 - settings + OpenGL baseline
- add launcher
- add shared settings storage
- replace Canvas presenter with GLES
- reproduce current nearest-neighbor output exactly
- add linear filter
- add pixel-perfect viewport
- verify input/audio/pause/save behavior unchanged

Acceptance requirement: “Nearest / Fit / Original 3:2” should look and behave like the current game before adding enhancements.

## Phase 2 - first visual enhancements
- Sharp Pixels
- GBA color correction
- live in-game video menu
- performance/frame-time diagnostics

## Phase 3 - edge-aware upscale
- Scale3x
- then ScaleFX
- optional xBR test
- compare text/cards/battle effects carefully

## Phase 4 - Vita widescreen parity
- widen native buffers
- port `widescreen.c`
- host stream-state functions
- VBlank stream capture
- game-source camera/sprite/panel/UI hooks
- 282x160
- wide dialogue/panels
- wide menu handling
- extensive field + battle + event regression testing

## Phase 5 - control QoL
- right-stick cards
- dedicated dodge
- one-button L+R
- unstock + progress bar
- remapping/deadzones

These could move before Phase 4 if desired; they are mostly independent.

## Phase 6 - gameplay overlays/QoL
- field HP
- Moogle notice
- skip intro
- save banks after careful save testing

## Phase 7 - experimental style shaders
- toon/cartoon preset
- ScaleFX/xBR tuning
- optional Anime4K experiment
- only after text/UI readability can be compared quickly in-game

---

# 13. Testing matrix

Every major rendering change should be checked in:

- title screen
- FMV
- field exploration
- scrolling rooms
- rooms near map edges
- dialogue opening/closing animation
- world/card-selection overlays
- normal battle
- battle arena edges
- card reload
- card stacking/sleights
- pause/start menu
- save point UI
- transitions/fades
- boss/effect-heavy scenes

For filters specifically:
- small card numbers
- menu fonts
- face portraits
- diagonal sprite outlines
- semitransparent effects
- fades
- motion stability while scrolling

For widescreen:
- sprite culling at both new edges
- battle camera clamping
- map wrapping/repeating
- message-panel margins
- hidden/offscreen UI leaking into margins

---

# 14. Risk notes

## Highest-risk areas
- widescreen game-side hooks across many modes
- save-bank switching
- native start/stop lifecycle
- aggressive post-processing that damages UI readability

## Lower-risk/high-value areas
- pixel-perfect scaling
- nearest/linear options
- Sharp Pixels
- GBA color correction
- one-button L+R
- right-stick left/right card cycling
- dedicated dodge

## Important rule
Port Vita features selectively. Do not blindly apply the whole Vita patch because:
- Android already has its own native fault handling and Android-specific null fixes
- current Android source has new reload-card/empty-card safety work
- some Vita fixes may already exist differently on Android
- platform macros should be generalized where the behavior is actually shared

---

# 15. Source inventory

## Current Android files
- https://github.com/narmanb/khcom-android/blob/android-port/port/android/app/src/main/java/com/narmanb/khcomandroid/GameView.java
- https://github.com/narmanb/khcom-android/blob/android-port/port/android/app/src/main/java/com/narmanb/khcomandroid/MainActivity.java
- https://github.com/narmanb/khcom-android/blob/android-port/port/android/app/src/main/java/com/narmanb/khcomandroid/NativeBridge.java
- https://github.com/narmanb/khcom-android/blob/android-port/port/android/host/android_host.c
- https://github.com/narmanb/khcom-android/blob/android-port/port/android/host/android_jni.c
- https://github.com/narmanb/khcom-android/blob/android-port/port/android/host/ppu.c
- https://github.com/narmanb/khcom-android/blob/android-port/port/android/include/ppu.h

## Vita reference
- https://github.com/ggdrn/KH_COM_VITA
- https://github.com/ggdrn/KH_COM_VITA/blob/main/README.md
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/game/widescreen.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/host/ppu.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/host/vita_render.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/host/vita_video.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/host/scale3x.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/host/vita_input.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/host/vita_menu.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/port/vita/host/vita_notice.c
- https://github.com/ggdrn/KH_COM_VITA/blob/main/patches/khcom-vita.patch

## Android documentation
- https://developer.android.com/develop/ui/views/graphics/opengl/environment
- https://developer.android.com/reference/android/opengl/GLSurfaceView
- https://developer.android.com/reference/android/opengl/GLSurfaceView.Renderer
- https://developer.android.com/games/sdk/game-controller/controller-input

## Filter references / licensing
- ScaleFX: https://github.com/libretro/common-shaders/tree/master/scalefx
- GLSL ScaleFX: https://github.com/libretro/glsl-shaders/tree/master/scalefx
- xBR: https://github.com/libretro/glsl-shaders/tree/master/xbr
- xBRZ licensing caution: https://github.com/libretro/glsl-shaders/blob/master/xbrz/shaders/6xbrz.glsl
- Anime4K (MIT): https://github.com/bloc97/Anime4K

---

# Bottom-line recommendation

The best near-term technical move after gameplay stability is confirmed is:

1. OpenGL ES presenter with exact nearest-neighbor parity.
2. Pixel-perfect + linear + Sharp Pixels + GBA colors.
3. Multi-pass filter framework.
4. Scale3x / ScaleFX.
5. Activate the already-present widescreen PPU using Vita's missing game/host integration.
6. Port controller QoL and overlays after the graphics foundation is stable.

The current Android PPU already containing the Vita widescreen renderer substantially lowers the expected difficulty of true widescreen compared with starting from the current Java Canvas behavior alone.
