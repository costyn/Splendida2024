#ifndef PALETTES_H
#define PALETTES_H

// Gradient Color Palette definitions for 33 different cpt-city color palettes.
//     956 bytes of PROGMEM for all of the palettes together,
//    +618 bytes of PROGMEM for gradient palette code (AVR).
//   1,494 bytes total for all 34 color palettes and associated code.

// Gradient palette "ib_jul01_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ing/xmas/tn/ib_jul01.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

#include <FastLED.h>
#include <wled_palettes.h>

extern const TProgmemRGBGradientPaletteRef gGradientPalettes[];

#define SECONDS_PER_PALETTE 10
// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const uint8_t gGradientPaletteCount;
// Current palette number from the 'playlist' of color palettes
uint8_t gCurrentPaletteNumber = 0;
CRGBPalette16 gCurrentPalette(CRGB::Black);
// CRGBPalette16 gTargetPalette ( gGradientPalettes[random8(gGradientPaletteCount)] );
CRGBPalette16 gTargetPalette; // move initialisation with random8 to setup()

// Gradient palette "es_pinksplash_07_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/pink_splash/tn/es_pinksplash_07.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_pinksplash_07_gp){
    0, 229, 1, 1,
    61, 242, 4, 63,
    101, 255, 12, 255,
    127, 249, 81, 252,
    153, 255, 11, 235,
    193, 244, 5, 68,
    255, 232, 1, 5};

// Gradient palette "Coral_reef_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/other/tn/Coral_reef.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE(Coral_reef_gp){
    0, 40, 199, 197,
    50, 10, 152, 155,
    96, 1, 111, 120,
    96, 43, 127, 162,
    139, 10, 73, 111,
    255, 1, 34, 71};

// Gradient palette "es_ocean_breeze_068_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/ocean_breeze/tn/es_ocean_breeze_068.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 24 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_ocean_breeze_068_gp){
    0, 100, 156, 153,
    51, 1, 99, 137,
    101, 1, 68, 84,
    104, 35, 142, 168,
    178, 0, 63, 117,
    255, 1, 10, 10};

// Gradient palette "Fuschia_7_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/ds/fuschia/tn/Fuschia-7.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.

DEFINE_GRADIENT_PALETTE(Fuschia_7_gp){
    0, 43, 3, 153,
    63, 100, 4, 103,
    127, 188, 5, 66,
    191, 161, 11, 115,
    255, 135, 20, 182};

// Gradient palette "es_emerald_dragon_08_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/es/emerald_dragon/tn/es_emerald_dragon_08.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE(es_emerald_dragon_08_gp){
    0, 97, 255, 1,
    101, 47, 133, 1,
    178, 13, 43, 1,
    255, 2, 10, 1};

// Gradient palette "fire_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/neota/elem/tn/fire.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(fire_gp){
    0, 1, 1, 0,
    76, 32, 5, 0,
    146, 192, 24, 0,
    197, 220, 105, 5,
    240, 252, 255, 31,
    250, 252, 255, 111,
    255, 255, 255, 255};

// Gradient palette "Magenta_Evening_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Magenta_Evening.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE(Magenta_Evening_gp){
    0, 71, 27, 39,
    31, 130, 11, 51,
    63, 213, 2, 64,
    70, 232, 1, 66,
    76, 252, 1, 69,
    108, 123, 2, 51,
    255, 46, 9, 35};

// Single array of defined cpt-city color palettes.
// This will let us programmatically choose one based on
// a number, rather than having to activate each explicitly
// by name every time.
// Since it is const, this array could also be moved
// into PROGMEM to save SRAM, but for simplicity of illustration
// we'll keep it in a regular SRAM array.
//
// This list of color palettes acts as a "playlist"; you can
// add or delete, or re-arrange as you wish.
const TProgmemRGBGradientPaletteRef gGradientPalettes[] = {
    Coral_reef_gp,
    es_ocean_breeze_068_gp,
    es_pinksplash_07_gp,
    Fuschia_7_gp,
    es_emerald_dragon_08_gp,
    fire_gp,
    Magenta_Evening_gp,
    Sunset_Real_gp,              // 13-00 Sunset
    es_rivendell_15_gp,          // 14-01 Rivendell
    es_ocean_breeze_036_gp,      // 15-02 Breeze
    rgi_15_gp,                   // 16-03 Red & Blue
    retro2_16_gp,                // 17-04 Yellowout
    Analogous_1_gp,              // 18-05 Analogous
    es_pinksplash_08_gp,         // 19-06 Splash
    Sunset_Yellow_gp,            // 20-07 Pastel
    Another_Sunset_gp,           // 21-08 Sunset2
    Beech_gp,                    // 22-09 Beech
    es_vintage_01_gp,            // 23-10 Vintage
    departure_gp,                // 24-11 Departure
    es_landscape_64_gp,          // 25-12 Landscape
    es_landscape_33_gp,          // 26-13 Beach
    rainbowsherbet_gp,           // 27-14 Sherbet
    gr65_hult_gp,                // 28-15 Hult
    gr64_hult_gp,                // 29-16 Hult64
    GMT_drywet_gp,               // 30-17 Drywet
    ib_jul01_gp,                 // 31-18 Jul
    es_vintage_57_gp,            // 32-19 Grintage
    ib15_gp,                     // 33-20 Rewhi
    Tertiary_01_gp,              // 34-21 Tertiary
    lava_gp,                     // 35-22 Fire
    fierce_ice_gp,               // 36-23 Icefire
    Colorfull_gp,                // 37-24 Cyane
    Pink_Purple_gp,              // 38-25 Light Pink
    es_autumn_19_gp,             // 39-26 Autumn
    BlacK_Blue_Magenta_White_gp, // 40-27 Magenta
    BlacK_Magenta_Red_gp,        // 41-28 Magred
    BlacK_Red_Magenta_Yellow_gp, // 42-29 Yelmag
    Blue_Cyan_Yellow_gp,         // 43-30 Yelblu
    Orange_Teal_gp,              // 44-31 Orange & Teal
    Tiamat_gp,                   // 45-32 Tiamat
    April_Night_gp,              // 46-33 April Night
    Orangery_gp,                 // 47-34 Orangery
    C9_gp,                       // 48-35 C9
    Sakura_gp,                   // 49-36 Sakura
    Aurora_gp,                   // 50-37 Aurora
    Atlantica_gp,                // 51-38 Atlantica
    C9_2_gp,                     // 52-39 C9 2
    C9_new_gp,                   // 53-40 C9 New
    temperature_gp,              // 54-41 Temperature
    Aurora2_gp,                  // 55-42 Aurora 2
    retro_clown_gp,              // 56-43 Retro Clown
    candy_gp,                    // 57-44 Candy
    toxy_reaf_gp,                // 58-45 Toxy Reaf
    fairy_reaf_gp,               // 59-46 Fairy Reaf
    semi_blue_gp,                // 60-47 Semi Blue
    pink_candy_gp,               // 61-48 Pink Candy
    red_reaf_gp,                 // 62-49 Red Reaf
    aqua_flash_gp,               // 63-50 Aqua Flash
    yelblu_hot_gp,               // 64-51 Yelblu Hot
    lite_light_gp,               // 65-52 Lite Light
    red_flash_gp,                // 66-53 Red Flash
    blink_red_gp,                // 67-54 Blink Red
    red_shift_gp,                // 68-55 Red Shift
    red_tide_gp,                 // 69-56 Red Tide
    candy2_gp                    // 70-57 Candy2
};

// In palettes.h - Add WLED palette names
const char *paletteNames[] = {
    "Coral_reef_gp",
    "es_ocean_breeze_068_gp",
    "es_pinksplash_07_gp",
    "Fuschia_7_gp",
    "es_emerald_dragon_08_gp",
    "fire_gp",
    "Magenta_Evening_gp",
    "Sunset",
    "Rivendell",
    "Breeze",
    "Red & Blue",
    "Yellowout",
    "Analogous",
    "Splash",
    "Pastel",
    "Sunset2",
    "Beech",
    "Vintage",
    "Departure",
    "Landscape",
    "Beach",
    "Sherbet",
    "Hult",
    "Hult64",
    "Drywet",
    "Jul",
    "Grintage",
    "Rewhi",
    "Tertiary",
    "Fire",
    "Icefire",
    "Cyane",
    "Light Pink",
    "Autumn",
    "Magenta",
    "Magred",
    "Yelmag",
    "Yelblu",
    "Orange & Teal",
    "Tiamat",
    "April Night",
    "Orangery",
    "C9",
    "Sakura",
    "Aurora",
    "Atlantica",
    "C9 2",
    "C9 New",
    "Temperature",
    "Aurora 2",
    "Retro Clown",
    "Candy",
    "Toxy Reaf",
    "Fairy Reaf",
    "Semi Blue",
    "Pink Candy",
    "Red Reaf",
    "Aqua Flash",
    "Yelblu Hot",
    "Lite Light",
    "Red Flash",
    "Blink Red",
    "Red Shift",
    "Red Tide",
    "Candy2"};

const uint8_t gGradientPaletteCount =
    sizeof(gGradientPalettes) / sizeof(TProgmemRGBGradientPaletteRef);

#endif