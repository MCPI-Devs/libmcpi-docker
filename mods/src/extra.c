#include <stdint.h>
#include <stdio.h>

#include <libcore/libcore.h>

static int32_t get_game_type(__attribute__((unused)) int32_t level_data) {
    return 0;
}

static uint32_t can_spawn_mobs(__attribute__((unused)) int32_t obj) {
    return 1;
}

#include <SDL/SDL_mouse.h>

typedef void (*releaseUsingItem_t)(unsigned char *t, unsigned char *player);
static releaseUsingItem_t survival_releaseUsingItem = (releaseUsingItem_t) 0x1a598;
static releaseUsingItem_t creative_releaseUsingItem = (releaseUsingItem_t) 0x1b1a0;

typedef void (*handle_input_t)(unsigned char *, unsigned char *, unsigned char *, unsigned char *);
static handle_input_t handle_input = (handle_input_t) 0x15ffc;
static void *handle_input_original = NULL;

static int is_survival = 0;

static void handle_input_injection(unsigned char *param_1, unsigned char *param_2, unsigned char *param_3, unsigned char *param_4) {
    // Call Original Method
    revert_overwrite((void *) handle_input, handle_input_original);
    (*handle_input)(param_1, param_2, param_3, param_4);
    revert_overwrite((void *) handle_input, handle_input_original);

    // GameMode Is Offset From param_1 By 0x160
    // Player Is Offset From param_1 By 0x18c
    int using_item = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT);
    if (!using_item) {
        unsigned char *game_mode = *(unsigned char **) (param_1 + 0x160);
        unsigned char *player = *(unsigned char **) (param_1 + 0x18c);
        if (player != NULL && game_mode != NULL) {
            (*(is_survival ? survival_releaseUsingItem : creative_releaseUsingItem))(game_mode, player);
        }
    }
}

static int has_feature(const char *name) {
    char *env = getenv("FEATURES");
    char *features = strdup(env != NULL ? env : "");
    char *tok = strtok(features, "|");
    int ret = 0;
    while (tok != NULL) {
        if (strcmp(tok, name) == 0) {
            ret = 1;
            break;
        }
        tok = strtok(NULL, "|");
    }
    free(features);
    fprintf(stderr, "Feature: %s: %s\n", name, ret ? "Enabled" : "Disabled");
    return ret;
}

__attribute__((constructor)) static void init() {
    if (has_feature("Touch GUI")) {
        // Use Touch UI
        unsigned char patch_data[4] = {0x01, 0x00, 0x50, 0xe3};
        patch((void *) 0x292fc, patch_data);
    }

    is_survival = has_feature("Survival Mode");
    if (is_survival) {
        // Survival Mode Inventpry UI
        unsigned char patch_data_2[4] = {0x00, 0x30, 0xa0, 0xe3};
        patch((void *) 0x16efc, patch_data_2);

        // Replace Creative Mode VTable With Survival Mode VTable
        patch((void *) 0x1a0d8, (unsigned char *) 0x1b804);

        // Use Correct Size For Survival Mode Object
        unsigned char patch_data_3[4] = {0x24, 0x00, 0xa0, 0xe3};
        patch((void *) 0x1a054, patch_data_3);

        // Force GameType To 0 (Required For Day-Night Cycle)
        overwrite((void *) 0xbabdc, get_game_type);
    }

    if (has_feature("Fix Bow & Arrow")) {
        // Fix Bow
        handle_input_original = overwrite((void *) handle_input, handle_input_injection);
    }

    if (has_feature("Fix Attacking")) {
        // Allow Attacking Mobs
        unsigned char patch_data_5[4] = {0x00, 0xf0, 0x20, 0xe3};
        patch((void *) 0x162d4, patch_data_5);
    }

    if (has_feature("Mob Spawning")) {
        // Enable Mob Spawning
        overwrite((void *) 0xbabec, can_spawn_mobs);

        // Replace CreatorLevel With Level
        unsigned char patch_data_4[4] = {0x96, 0x3d, 0x02, 0xeb};
        patch((void *) 0x16f84, patch_data_4);
    }
}