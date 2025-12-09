# Architecture Finale : Contrôle de Framerate et Contextes pour Banjo
## Rapport Technique après Discussion et Review

**Date:** 2025-12-08
**Auteur:** Claude Code + Kevin Dorange
**Review:** Gemini (validé)
**Statut:** Décisions finales

---

## Résumé Exécutif

Après discussions approfondies et review par Gemini, voici l'architecture finale pour le contrôle de framerate et les contextes de rendu dans Banjo.

**Décisions clés:**
1. ✅ **Le contrôle de framerate EST essentiel pour le rendu logiciel**
2. ✅ **Swap interval (style GLFW) avec macros pour clarté**
3. ✅ **Pattern vtable pour les contextes (comme pour les backends)**
4. ✅ **Interval de présentation par fenêtre** (stocké dans context_data)

---

## 1. Architecture des Contextes

### Concept

Un **contexte de rendu** détermine comment on dessine dans une fenêtre :
- **SOFTWARE** : Rendu CPU vers bitmap
- **VULKAN** : Rendu GPU via Vulkan
- **OPENGL** : Rendu GPU via OpenGL (futur)
- **METAL** : Rendu GPU via Metal (futur)

### Pattern Vtable (comme les backends)

```c
// Interface de contexte (vtable)
typedef struct bj_context_vtable {
    void (*present)(bj_window* win);
    bj_bitmap* (*get_framebuffer)(bj_window* win);
    void (*set_swap_interval)(bj_window* win, int interval);
    void (*destroy)(bj_window* win);
} bj_context_vtable;

// Données spécifiques au contexte
typedef struct bj_software_context {
    bj_bitmap* framebuffer;
    int swap_interval;           // 0=immediate, 1=vsync, N=every N vsyncs
    uint64_t last_frame_time;
    // ... autres données
} bj_software_context;

// Structure fenêtre
struct bj_window {
    void* native_handle;                // HWND, Window, wl_surface*, etc.
    const bj_context_vtable* context;   // Pointeur vers vtable de contexte
    void* context_data;                 // Données d'instance du contexte
};
```

### Contextes Pré-Définis

```c
// Contextes fournis par Banjo (globaux/statiques)
extern const bj_context_vtable bj_context_software;
extern const bj_context_vtable bj_context_vulkan;
extern const bj_context_vtable bj_context_opengl;  // futur

// Utilisation
bj_window* win = bj_create_window("Game", 0, 0, 800, 600,
                                   &bj_context_software);
```

**Alternative avec enum (style GLFW):**

```c
typedef enum {
    BJ_CONTEXT_SOFTWARE,
    BJ_CONTEXT_VULKAN,
    BJ_CONTEXT_OPENGL
} bj_context_type;

bj_window* win = bj_create_window("Game", 0, 0, 800, 600,
                                   BJ_CONTEXT_SOFTWARE);
```

**Avantage enum:** Plus simple, impossible de passer un contexte invalide
**Avantage pointeur:** Permet des contextes custom définis par l'utilisateur

**DÉCISION FINALE À PRENDRE:** Enum ou pointeur ?

### Implémentation Interne

Le contexte software dépend du backend de fenêtre :

```c
// X11 backend
void sw_present_x11(bj_window* win) {
    bj_software_context* ctx = win->context_data;

    if (ctx->swap_interval == 0) {
        // IMMEDIATE: présenter directement
        XPutImage(display, win->window, gc, ctx->framebuffer, ...);
    } else if (ctx->swap_interval == 1) {
        // VSYNC: attendre via timer (approximation)
        wait_for_refresh_interval();
        XPutImage(display, win->window, gc, ctx->framebuffer, ...);
    } else {
        // VSYNC/N: attendre N fois l'intervalle
        wait_for_n_refresh_intervals(ctx->swap_interval);
        XPutImage(display, win->window, gc, ctx->framebuffer, ...);
    }
}

// Win32 backend
void sw_present_win32(bj_window* win) {
    bj_software_context* ctx = win->context_data;

    if (ctx->swap_interval >= 1) {
        // VSYNC: attendre DWM
        for (int i = 0; i < ctx->swap_interval; i++) {
            DwmFlush();
        }
    }

    BitBlt(hdc, 0, 0, w, h, fbdc, 0, 0, SRCCOPY);
}

// Emscripten backend
void sw_present_emscripten(bj_window* win) {
    bj_software_context* ctx = win->context_data;

    // swap_interval == 0 : "as soon as possible" (mais rAF quand même)
    // swap_interval >= 1 : simuler en skippant des frames

    static int frame_counter = 0;
    if (ctx->swap_interval == 0 ||
        frame_counter % ctx->swap_interval == 0) {
        EM_ASM({
            ctx.putImageData(imageData, 0, 0);
        });
    }
    frame_counter++;
}
```

**Le backend de fenêtre peut fournir:** `get_software_context_vtable()` qui retourne la vtable appropriée pour la plateforme.

---

## 2. API de Contrôle de Framerate

### Swap Interval (style GLFW)

```c
void bj_window_set_swap_interval(bj_window* win, int interval);
int bj_window_get_swap_interval(bj_window* win);
```

**Sémantique:**
- `0` : **IMMEDIATE** - Présenter aussi vite que possible (peut tear)
- `1` : **VSYNC** - Synchro au rafraîchissement de l'écran
- `N >= 2` : **VSYNC/N** - Présenter tous les N vblanks (60Hz → 30fps si N=2)

**Macros pour clarté:**

```c
#define BJ_PRESENT_IMMEDIATE  0
#define BJ_PRESENT_VSYNC      1
#define BJ_PRESENT_VSYNC2     2  // 30fps sur écran 60Hz
#define BJ_PRESENT_VSYNC3     3  // 20fps sur écran 60Hz
```

**Utilisation:**

```c
// Vsync (smooth, no tearing)
bj_window_set_swap_interval(win, BJ_PRESENT_VSYNC);

// Maximum FPS (competitive gaming)
bj_window_set_swap_interval(win, BJ_PRESENT_IMMEDIATE);

// 30fps sur écran 60Hz (économie batterie)
bj_window_set_swap_interval(win, BJ_PRESENT_VSYNC2);

// Ou directement avec la valeur
bj_window_set_swap_interval(win, 1);  // VSYNC
```

### Pourquoi Swap Interval au lieu de Present Modes ?

**Arguments contre les present modes:**
1. Swap interval est **plus simple** (une seule valeur)
2. **Plus familier** pour les devs OpenGL/GLFW
3. **Suffit pour les cas d'usage** principaux
4. Le cas "30fps sur n'importe quel écran" (indépendant du refresh) est **rare en pratique**

**Arguments pour swap interval:**
1. API minimale et claire
2. Le backend simule ce qu'il peut (ex: Emscripten simule interval=0)
3. Les macros rendent l'API auto-documentée

**Limitations acceptées:**
- ❌ Pas de framerate fixe indépendant de l'écran (30fps sur 144Hz → utilise swap_interval=4 ou 5)
- ❌ Pas de mode Mailbox explicite (peut être ajouté plus tard)
- ✅ Couvre 95% des besoins réels

### Stockage par Fenêtre

Le swap interval est stocké dans `context_data` (par fenêtre) :

```c
struct bj_software_context {
    bj_bitmap* framebuffer;
    int swap_interval;        // Unique pour cette fenêtre
    uint64_t last_frame_time;
};

// Deux fenêtres, deux modes différents
bj_window_set_swap_interval(game_window, BJ_PRESENT_VSYNC);      // Smooth
bj_window_set_swap_interval(debug_window, BJ_PRESENT_IMMEDIATE); // Max FPS
```

---

## 3. Implémentation par Plateforme

### Windows (Win32 + DWM)

```c
static void sw_present_win32(bj_window* win) {
    bj_software_context* ctx = win->context_data;
    HDC hdc = GetDC(win->hwnd);

    // Attendre N vblanks si swap_interval >= 1
    for (int i = 0; i < ctx->swap_interval; i++) {
        DwmFlush();  // Bloque jusqu'à la prochaine composition DWM
    }

    // Copier framebuffer
    BitBlt(hdc, 0, 0, win->width, win->height,
           ctx->fb_dc, 0, 0, SRCCOPY);

    ReleaseDC(win->hwnd, hdc);
}

static void sw_set_swap_interval_win32(bj_window* win, int interval) {
    bj_software_context* ctx = win->context_data;
    ctx->swap_interval = interval;
}

static const bj_context_vtable sw_vtable_win32 = {
    .present = sw_present_win32,
    .get_framebuffer = sw_get_framebuffer,
    .set_swap_interval = sw_set_swap_interval_win32,
    .destroy = sw_destroy
};
```

### X11

```c
static void sw_present_x11(bj_window* win) {
    bj_software_context* ctx = win->context_data;

    if (ctx->swap_interval == 0) {
        // IMMEDIATE: présenter directement
        XPutImage(display, win->window, gc, ctx->ximage,
                  0, 0, 0, 0, win->width, win->height);
        XSync(display, False);
    } else {
        // VSYNC: timer-based approximation
        static uint64_t last_frame = 0;
        static float refresh_interval = 16666; // Query via RANDR

        uint64_t now = get_time_us();
        uint64_t target = last_frame + (refresh_interval * ctx->swap_interval);

        if (now < target) {
            usleep(target - now);
        }

        XPutImage(display, win->window, gc, ctx->ximage,
                  0, 0, 0, 0, win->width, win->height);
        XSync(display, False);

        last_frame = get_time_us();
    }
}
```

**Query refresh rate via RANDR:**

```c
#include <X11/extensions/Xrandr.h>

float query_refresh_rate(Display* display, int screen) {
    XRRScreenConfiguration* conf =
        XRRGetScreenInfo(display, RootWindow(display, screen));
    short rate = XRRConfigCurrentRate(conf);
    XRRFreeScreenConfigInfo(conf);
    return (float)rate;  // ex: 60.0, 144.0
}
```

### Wayland

```c
// Le compositor contrôle le timing via frame callbacks
void sw_present_wayland(bj_window* win) {
    bj_software_context* ctx = win->context_data;

    // Demander frame callback au compositor
    struct wl_callback* cb = wl_surface_frame(win->surface);
    wl_callback_add_listener(cb, &frame_listener, win);
    wl_surface_commit(win->surface);
}

void frame_callback(void* data, struct wl_callback* cb, uint32_t time) {
    bj_window* win = data;
    bj_software_context* ctx = win->context_data;

    // Simuler swap_interval en skippant des frames
    static int counter = 0;

    if (ctx->swap_interval == 0 ||
        counter % ctx->swap_interval == 0) {

        // Attacher et présenter
        wl_surface_attach(win->surface, win->buffer, 0, 0);
        wl_surface_damage_buffer(win->surface, 0, 0,
                                  win->width, win->height);
        wl_surface_commit(win->surface);
    }

    counter++;
    wl_callback_destroy(cb);
}
```

### Emscripten

```c
void sw_present_emscripten(bj_window* win) {
    bj_software_context* ctx = win->context_data;

    emscripten_request_animation_frame(frame_callback, win);
}

void frame_callback(double time, void* userData) {
    bj_window* win = userData;
    bj_software_context* ctx = win->context_data;

    static int counter = 0;

    // swap_interval=0 : "asap" (mais rAF contrôle quand même)
    // swap_interval>=1 : simuler en skippant
    if (ctx->swap_interval == 0 ||
        counter % ctx->swap_interval == 0) {

        EM_ASM({
            var ctx = Module.canvas.getContext('2d');
            ctx.putImageData(Module.imageData, 0, 0);
        });
    }

    counter++;
}
```

---

## 4. API Complète

### Types

```c
// Type de contexte (si on utilise enum)
typedef enum {
    BJ_CONTEXT_SOFTWARE,
    BJ_CONTEXT_VULKAN,
    BJ_CONTEXT_OPENGL
} bj_context_type;

// Macros pour swap interval
#define BJ_PRESENT_IMMEDIATE  0
#define BJ_PRESENT_VSYNC      1
#define BJ_PRESENT_VSYNC2     2
#define BJ_PRESENT_VSYNC3     3
```

### Fenêtre et Contexte

```c
// Création de fenêtre avec contexte
bj_window* bj_create_window(const char* title, int x, int y,
                             int width, int height,
                             bj_context_type context);

// Opérations de contexte (délèguent à la vtable)
void bj_window_present(bj_window* win);
bj_bitmap* bj_window_get_framebuffer(bj_window* win);

// Contrôle de framerate (par fenêtre)
void bj_window_set_swap_interval(bj_window* win, int interval);
int bj_window_get_swap_interval(bj_window* win);

// Opérations de fenêtre (indépendantes du contexte)
void bj_window_set_title(bj_window* win, const char* title);
void bj_window_set_size(bj_window* win, int width, int height);
bool bj_should_close_window(bj_window* win);
void bj_destroy_window(bj_window* win);
```

### Événements

```c
void bj_dispatch_events(void);  // Traiter les événements plateforme
```

### Exemple d'Utilisation

```c
#include <banjo/banjo.h>

int main(void) {
    bj_initialize(NULL);

    // Créer fenêtre avec contexte software
    bj_window* win = bj_create_window("My Game", 0, 0, 800, 600,
                                       BJ_CONTEXT_SOFTWARE);

    // Activer VSYNC (smooth, no tearing)
    bj_window_set_swap_interval(win, BJ_PRESENT_VSYNC);

    bool running = true;

    // Boucle principale classique
    while (running) {
        // Traiter événements
        bj_dispatch_events();

        if (bj_should_close_window(win)) {
            running = false;
            continue;
        }

        // Dessiner
        bj_bitmap* fb = bj_window_get_framebuffer(win);
        bj_clear_bitmap(fb, 0x202020);
        draw_game(fb);

        // Présenter (applique swap_interval)
        bj_window_present(win);
    }

    bj_destroy_window(win);
    bj_shutdown(NULL);
    return 0;
}
```

**Autres modes:**

```c
// Maximum FPS (gaming compétitif)
bj_window_set_swap_interval(win, BJ_PRESENT_IMMEDIATE);

// 30fps sur écran 60Hz (économie batterie)
bj_window_set_swap_interval(win, BJ_PRESENT_VSYNC2);

// Deux fenêtres, deux modes
bj_window* game_win = bj_create_window(...);
bj_window_set_swap_interval(game_win, BJ_PRESENT_VSYNC);

bj_window* debug_win = bj_create_window(...);
bj_window_set_swap_interval(debug_win, BJ_PRESENT_IMMEDIATE);
```

---

## 5. Résumé des Décisions Finales

### ✅ Validé

1. **Le contrôle de framerate est essentiel pour le rendu logiciel**
   - VSYNC fonctionne via coordination avec le compositeur
   - DwmFlush, wl_surface_frame, requestAnimationFrame sont des mécanismes valides
   - Fournit une vraie valeur : efficacité, smoothness, économie batterie

2. **Swap interval au lieu de present modes**
   - Plus simple et familier (style GLFW)
   - Macros pour clarté (`BJ_PRESENT_VSYNC`)
   - Suffit pour 95% des cas d'usage

3. **Swap interval par fenêtre**
   - Stocké dans `context_data`
   - Chaque fenêtre peut avoir son propre mode
   - Use case : fenêtre jeu (VSYNC) + fenêtre debug (IMMEDIATE)

4. **Pattern vtable pour contextes**
   - Même approche que les backends
   - Contextes pré-définis (`bj_context_software`, etc.)
   - Possibilité contextes custom (si pointeur au lieu d'enum)

5. **Boucle classique explicite**
   - Pas de self-continuation (trop alien en C)
   - `while (running) { events(); draw(); present(); }`
   - Naturel pour les devs C/C++

### ❌ Rejeté

1. **Present modes (IMMEDIATE/VSYNC/FIXED)**
   - Trop complexe pour les besoins réels
   - Le cas "30fps fixe sur n'importe quel écran" est rare
   - Swap interval + macros suffit

2. **Self-continuation model**
   - Trop alien pour le C
   - Naturel en JavaScript mais pas en C desktop
   - Boucle classique plus intuitive

3. **Global present mode**
   - Doit être par fenêtre
   - Permet différents modes pour différentes fenêtres

### ⚠️ Limitations Acceptées

1. **X11 VSYNC = approximation par timer**
   - Pas de vrai vsync hardware sans dépendances
   - Acceptable pour une lib zero-dependency

2. **Framerate fixe indépendant de l'écran**
   - Pas directement supporté
   - Peut utiliser swap_interval calculé (ex: 144Hz ÷ 30fps = interval 5)
   - Cas d'usage rare en pratique

3. **Emscripten IMMEDIATE est simulé**
   - requestAnimationFrame contrôle le timing
   - On peut seulement "dessiner à chaque rAF" (best effort)
   - C'est OK, "immediate" = "aussi vite que possible"

---

## 6. Questions Restantes

### Q1: Enum ou Pointeur pour les Contextes ?

**Option A: Enum (style GLFW)**
```c
bj_window* win = bj_create_window(..., BJ_CONTEXT_SOFTWARE);
```
- ✅ Plus simple
- ✅ Type-safe
- ❌ Pas de contextes custom

**Option B: Pointeur vers vtable**
```c
bj_window* win = bj_create_window(..., &bj_context_software);
```
- ✅ Permet contextes custom
- ❌ Possibilité de pointeur invalide
- ⚠️ Cas d'usage pour custom context ?

**RECOMMANDATION:** Commencer avec enum (plus simple), ajouter API pour custom contexts si besoin plus tard.

### Q2: Backend Software Spécifique ?

Le contexte software dépend du backend de fenêtre (X11 → XPutImage, Win32 → BitBlt).

**Option A:** Le backend fournit `get_software_context()`
```c
const bj_context_vtable* x11_backend_get_software_context(void);
```

**Option B:** Contexte software unique, branche en interne
```c
void sw_present(bj_window* win) {
    #ifdef BJ_OS_WINDOWS
        sw_present_win32(win);
    #elif defined(BJ_OS_LINUX)
        if (is_x11) sw_present_x11(win);
        else sw_present_wayland(win);
    #endif
}
```

**RECOMMANDATION:** Option A (backend fournit la vtable) - plus propre, séparation claire.

---

## 7. Prochaines Étapes d'Implémentation

### Phase 1: API de Base

1. Définir `bj_context_vtable`
2. Implémenter contexte software pour Win32
3. Implémenter contexte software pour X11
4. API `bj_window_set_swap_interval()` / `bj_window_present()`
5. Tests avec IMMEDIATE et VSYNC

### Phase 2: Autres Plateformes

6. Contexte software pour Wayland
7. Contexte software pour Emscripten
8. Contexte software pour Cocoa
9. Tests multi-fenêtres

### Phase 3: GPU (Futur)

10. Définir contexte Vulkan
11. Mapper swap_interval → VkPresentModeKHR
12. Tests switching entre software et Vulkan

---

## Conclusion

Architecture finale validée par Gemini, simplifiée et pragmatique :

- ✅ **Swap interval** (GLFW-style) avec macros
- ✅ **Vtable pour contextes** (comme backends)
- ✅ **Par fenêtre** (stocké dans context_data)
- ✅ **Boucle classique** (pas de callbacks)
- ✅ **Zero dependencies** (timer approximation sur X11)

**C'est parti pour l'implémentation !**

---

**Document rédigé par:** Claude Code + Kevin Dorange
**Validation:** Gemini (Google)
**Projet:** Banjo - Framework C99 pour jeux
**Date:** 2025-12-08 - 01:15 AM
