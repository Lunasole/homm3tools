// Created by John Åkerblom 2014-11-22

#include "../h3mlib.h"
#include "parse_oa.h"
#include "../safe_read.h"

#include "../h3m_object.h"

#include "../gen/def_bodies_array.h"

#include "../vcmi_oa_constants.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)   // M$ standard lib unsafe
#endif

static int _oa_type_from_constants(uint32_t object_class,
    uint32_t object_number, uint8_t object_group)
{
    // defines missing in VCMI :-)
    // TODO merge these and vcmi_oa_constants.h
#define OA_CLASS_DECORATIVE_TOWN 1      // Observed "decorative" Fortress on map "West Orient & Underground Cave" use this
#define OA_CLASS_MARKET_OF_TIME 50

#define OA_CLASS_IMPASSABLE_TERRAIN1 116
#define OA_CLASS_IMPASSABLE_TERRAIN2 118
#define OA_CLASS_IMPASSABLE_TERRAIN3 119
#define OA_CLASS_IMPASSABLE_TERRAIN4 120
#define OA_CLASS_IMPASSABLE_TERRAIN5 121
#define OA_CLASS_IMPASSABLE_TERRAIN6 126
#define OA_CLASS_IMPASSABLE_TERRAIN7 127
#define OA_CLASS_IMPASSABLE_TERRAIN8 130
#define OA_CLASS_IMPASSABLE_TERRAIN9 131
#define OA_CLASS_IMPASSABLE_TERRAIN10 133
#define OA_CLASS_IMPASSABLE_TERRAIN11 134
#define OA_CLASS_IMPASSABLE_TERRAIN12 135
#define OA_CLASS_IMPASSABLE_TERRAIN13 136
#define OA_CLASS_IMPASSABLE_TERRAIN14 137
#define OA_CLASS_IMPASSABLE_TERRAIN15 143
#define OA_CLASS_IMPASSABLE_TERRAIN16 147
#define OA_CLASS_IMPASSABLE_TERRAIN17 148
#define OA_CLASS_IMPASSABLE_TERRAIN18 149
#define OA_CLASS_IMPASSABLE_TERRAIN19 150
#define OA_CLASS_IMPASSABLE_TERRAIN20 151
#define OA_CLASS_IMPASSABLE_TERRAIN21 153
#define OA_CLASS_IMPASSABLE_TERRAIN22 155
#define OA_CLASS_IMPASSABLE_TERRAIN23 158
#define OA_CLASS_IMPASSABLE_TERRAIN24 161
#define OA_CLASS_IMPASSABLE_TERRAIN25 128
#define OA_CLASS_IMPASSABLE_TERRAIN26 129
#define OA_CLASS_IMPASSABLE_TERRAIN27 117
#define OA_CLASS_IMPASSABLE_TERRAIN28 132

#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD1 199
#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD2 206
#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD3 207
#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD4 208
#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD5 209
#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD6 210
#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD7 211
#define OA_CLASS_IMPASSABLE_TERRAIN_ABSOD8 177

#define OA_CLASS_KELP 125
#define OA_CLASS_WOG1 40        // ZRain00

    switch (object_class) {
    case OA_CLASS_IMPASSABLE_TERRAIN1:
    case OA_CLASS_IMPASSABLE_TERRAIN2:
    case OA_CLASS_IMPASSABLE_TERRAIN3:
    case OA_CLASS_IMPASSABLE_TERRAIN4:
    case OA_CLASS_IMPASSABLE_TERRAIN5:
    case OA_CLASS_IMPASSABLE_TERRAIN6:
    case OA_CLASS_IMPASSABLE_TERRAIN7:
    case OA_CLASS_IMPASSABLE_TERRAIN8:
    case OA_CLASS_IMPASSABLE_TERRAIN9:
    case OA_CLASS_IMPASSABLE_TERRAIN10:
    case OA_CLASS_IMPASSABLE_TERRAIN11:
    case OA_CLASS_IMPASSABLE_TERRAIN12:
    case OA_CLASS_IMPASSABLE_TERRAIN13:
    case OA_CLASS_IMPASSABLE_TERRAIN14:
    case OA_CLASS_IMPASSABLE_TERRAIN15:
    case OA_CLASS_IMPASSABLE_TERRAIN16:
    case OA_CLASS_IMPASSABLE_TERRAIN17:
    case OA_CLASS_IMPASSABLE_TERRAIN18:
    case OA_CLASS_IMPASSABLE_TERRAIN19:
    case OA_CLASS_IMPASSABLE_TERRAIN20:
    case OA_CLASS_IMPASSABLE_TERRAIN21:
    case OA_CLASS_IMPASSABLE_TERRAIN22:
    case OA_CLASS_IMPASSABLE_TERRAIN23:
    case OA_CLASS_IMPASSABLE_TERRAIN24:
    case OA_CLASS_IMPASSABLE_TERRAIN25:
    case OA_CLASS_IMPASSABLE_TERRAIN26:
    case OA_CLASS_IMPASSABLE_TERRAIN27:
    case OA_CLASS_IMPASSABLE_TERRAIN28:
        return H3M_OBJECT_GENERIC_IMPASSABLE_TERRAIN;
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD1:
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD2:
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD3:
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD4:
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD5:
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD6:
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD7:
    case OA_CLASS_IMPASSABLE_TERRAIN_ABSOD8:
        return H3M_OBJECT_GENERIC_IMPASSABLE_TERRAIN_ABSOD;
    case OA_CLASS_WOG1:        // TODO when better WoG support is had adjust this
        return H3M_OBJECT_GENERIC_VISITABLE_ABSOD;
    case OA_CLASS_ALTAR_OF_SACRIFICE:
    case OA_CLASS_ANCHOR_POINT:
    case OA_CLASS_ARENA:
    case OA_CLASS_BLACK_MARKET:
    case OA_CLASS_CARTOGRAPHER:
    case OA_CLASS_BUOY:
    case OA_CLASS_SWAN_POND:
    case OA_CLASS_COVER_OF_DARKNESS:
    case OA_CLASS_CREATURE_BANK:
    case OA_CLASS_CORPSE:
    case OA_CLASS_MARLETTO_TOWER:
    case OA_CLASS_DERELICT_SHIP:
    case OA_CLASS_DRAGON_UTOPIA:
    case OA_CLASS_EYE_OF_MAGI:
    case OA_CLASS_FAERIE_RING:
    case OA_CLASS_FOUNTAIN_OF_FORTUNE:
    case OA_CLASS_FOUNTAIN_OF_YOUTH:
    case OA_CLASS_GARDEN_OF_REVELATION:
    case OA_CLASS_HILL_FORT:
    case OA_CLASS_HUT_OF_MAGI:
    case OA_CLASS_IDOL_OF_FORTUNE:
    case OA_CLASS_LEAN_TO:
    case OA_CLASS_LIBRARY_OF_ENLIGHTENMENT:

    case OA_CLASS_SCHOOL_OF_MAGIC:
    case OA_CLASS_MAGIC_SPRING:
    case OA_CLASS_MAGIC_WELL:
    case OA_CLASS_MERCENARY_CAMP:
    case OA_CLASS_MERMAID:
    case OA_CLASS_MYSTICAL_GARDEN:
    case OA_CLASS_OASIS:
    case OA_CLASS_OBELISK:
    case OA_CLASS_REDWOOD_OBSERVATORY:
    case OA_CLASS_PILLAR_OF_FIRE:
    case OA_CLASS_STAR_AXIS:
    case OA_CLASS_RALLY_FLAG:
    case OA_CLASS_BORDERGUARD:
    case OA_CLASS_KEYMASTER:
    case OA_CLASS_REFUGEE_CAMP:
    case OA_CLASS_SANCTUARY:
    case OA_CLASS_CRYPT:
    case OA_CLASS_SHIPWRECK:
    case OA_CLASS_SIRENS:
    case OA_CLASS_STABLES:
    case OA_CLASS_TAVERN:
    case OA_CLASS_TEMPLE:
    case OA_CLASS_DEN_OF_THIEVES:
    case OA_CLASS_TRADING_POST:
    case OA_CLASS_LEARNING_STONE:
    case OA_CLASS_TREE_OF_KNOWLEDGE:
    case OA_CLASS_UNIVERSITY:
    case OA_CLASS_WAGON:
    case OA_CLASS_WAR_MACHINE_FACTORY:
    case OA_CLASS_SCHOOL_OF_WAR:
    case OA_CLASS_WARRIORS_TOMB:
    case OA_CLASS_WATER_WHEEL:
    case OA_CLASS_WATERING_HOLE:
    case OA_CLASS_WHIRLPOOL:
    case OA_CLASS_WINDMILL:
    case OA_CLASS_MARKET_OF_TIME:
    case OA_CLASS_DECORATIVE_TOWN:
        return H3M_OBJECT_GENERIC_VISITABLE;
    case OA_CLASS_TRADING_POST_SNOW:
    case OA_CLASS_PYRAMID:     // TODO actually this is a WoG object, so move/refactor 
    case OA_CLASS_BORDER_GATE:
    case OA_CLASS_FREELANCERS_GUILD:
        return H3M_OBJECT_GENERIC_VISITABLE_ABSOD;
    case OA_CLASS_ARTIFACT:
    case OA_CLASS_RANDOM_ART:
    case OA_CLASS_RANDOM_TREASURE_ART:
    case OA_CLASS_RANDOM_MINOR_ART:
    case OA_CLASS_RANDOM_MAJOR_ART:
    case OA_CLASS_RANDOM_RELIC_ART:
        return H3M_OBJECT_ARTIFACT;
    case OA_CLASS_ABANDONED_MINE:      // TODO check object_number
        return H3M_OBJECT_ABANDONED_MINE_ABSOD;
    case OA_CLASS_CREATURE_GENERATOR1:
    case OA_CLASS_CREATURE_GENERATOR2:
    case OA_CLASS_CREATURE_GENERATOR3:
    case OA_CLASS_CREATURE_GENERATOR4:
        return H3M_OBJECT_DWELLING;
    case OA_CLASS_EVENT:
        return H3M_OBJECT_EVENT;
    case OA_CLASS_GARRISON:
        return H3M_OBJECT_GARRISON;
    case OA_CLASS_GARRISON2:
        return H3M_OBJECT_GARRISON_ABSOD;
    case OA_CLASS_BOAT:
        return H3M_OBJECT_GENERIC_BOAT;
    case OA_CLASS_CLOVER_FIELD:
    case OA_CLASS_EVIL_FOG:
    case OA_CLASS_FAVORABLE_WINDS:
    case OA_CLASS_FIERY_FIELDS:
    case OA_CLASS_HOLY_GROUNDS:
    case OA_CLASS_LUCID_POOLS:
    case OA_CLASS_MAGIC_CLOUDS:
    case OA_CLASS_ROCKLANDS:
    case OA_CLASS_CURSED_GROUND2:
    case OA_CLASS_MAGIC_PLAINS2:
        return H3M_OBJECT_GENERIC_PASSABLE_TERRAIN_SOD;
    case OA_CLASS_HOLE:
    case OA_CLASS_CURSED_GROUND1:
    case OA_CLASS_MAGIC_PLAINS1:
    case OA_CLASS_KELP:
        return H3M_OBJECT_GENERIC_PASSABLE_TERRAIN;
    case OA_CLASS_PANDORAS_BOX:
        return H3M_OBJECT_PANDORAS_BOX;
    case OA_CLASS_GRAIL:
        return H3M_OBJECT_GRAIL;
    case OA_CLASS_HERO:
        return H3M_OBJECT_HERO;
    case OA_CLASS_LIGHTHOUSE:
        return H3M_OBJECT_LIGHTHOUSE;
    case OA_CLASS_MONOLITH_TWO_WAY:    // TODO once monoliths have been divided fix this
        //return H3M_OBJECT_MONOLITH_TWO_WAY;
    case OA_CLASS_MONOLITH_ONE_WAY_ENTRANCE:
    case OA_CLASS_MONOLITH_ONE_WAY_EXIT:
        return H3M_OBJECT_GENERIC_VISITABLE_ABSOD;
    case OA_CLASS_MONSTER:
    case OA_CLASS_RANDOM_MONSTER:
    case OA_CLASS_RANDOM_MONSTER_L1:
    case OA_CLASS_RANDOM_MONSTER_L2:
    case OA_CLASS_RANDOM_MONSTER_L3:
    case OA_CLASS_RANDOM_MONSTER_L4:
    case OA_CLASS_RANDOM_MONSTER_L5:
    case OA_CLASS_RANDOM_MONSTER_L6:
    case OA_CLASS_RANDOM_MONSTER_L7:
        return H3M_OBJECT_MONSTER;
    case OA_CLASS_OCEAN_BOTTLE:
        return H3M_OBJECT_OCEAN_BOTTLE;
    case OA_CLASS_PRISON:
        return H3M_OBJECT_PRISON;
    case OA_CLASS_QUEST_GUARD:
        return H3M_OBJECT_QUEST_GUARD;
    case OA_CLASS_RANDOM_DWELLING:
        return H3M_OBJECT_RANDOM_DWELLING_ABSOD;
    case OA_CLASS_RANDOM_DWELLING_LVL:
        return H3M_OBJECT_RANDOM_DWELLING_PRESET_LEVEL_ABSOD;
    case OA_CLASS_RANDOM_DWELLING_FACTION:
        return H3M_OBJECT_RANDOM_DWELLING_PRESET_ALIGNMENT_ABSOD;
    case OA_CLASS_RANDOM_HERO:
        return H3M_OBJECT_RANDOM_HERO;
    case OA_CLASS_HERO_PLACEHOLDER:
        return H3M_OBJECT_PLACEHOLDER_HERO;
    case OA_CLASS_RESOURCE:
    case OA_CLASS_RANDOM_RESOURCE:
        return H3M_OBJECT_RESOURCE;
    case OA_CLASS_MINE:
        return H3M_OBJECT_RESOURCE_GENERATOR;
    case OA_CLASS_SCHOLAR:
        return H3M_OBJECT_SCHOLAR;
    case OA_CLASS_SEER_HUT:
        return H3M_OBJECT_SEERS_HUT;
    case OA_CLASS_SHIPYARD:
        return H3M_OBJECT_SHIPYARD;
    case OA_CLASS_SHRINE_OF_MAGIC_INCANTATION:
    case OA_CLASS_SHRINE_OF_MAGIC_GESTURE:
    case OA_CLASS_SHRINE_OF_MAGIC_THOUGHT:
        return H3M_OBJECT_SHRINE;
    case OA_CLASS_SIGN:
        return H3M_OBJECT_SIGN;
    case OA_CLASS_SPELL_SCROLL:
        return H3M_OBJECT_SPELL_SCROLL;
    case OA_CLASS_SUBTERRANEAN_GATE:
        return H3M_OBJECT_SUBTERRANEAN_GATE;
    case OA_CLASS_TOWN:
    case OA_CLASS_RANDOM_TOWN:
        return H3M_OBJECT_TOWN;
    case OA_CLASS_WITCH_HUT:
        return H3M_OBJECT_WITCH_HUT;
    case OA_CLASS_CAMPFIRE:
    case OA_CLASS_FLOTSAM:
    case OA_CLASS_SEA_CHEST:
    case OA_CLASS_SHIPWRECK_SURVIVOR:
    case OA_CLASS_TREASURE_CHEST:
        return H3M_OBJECT_GENERIC_TREASURE;
    default:
        break;
    }

    return -1;
}

int parse_oa_meta_type(struct H3M_OA_ENTRY *oa_entry, int *meta_type,
    int *is_custom, h3m_custom_def_cb_t cb, void *cb_data)
{
    int val = 0;

    *is_custom = 0;
#if 0
    // Custom check
    if (NULL != cb && -1 != (val = cb(oa_entry->header.def, cb_data))) {
        *meta_type = val;
        *is_custom = 1;
        return 0;
    }
#endif

    // OA's constants check
    if (-1 != (val =
            _oa_type_from_constants(oa_entry->body.object_class,
                oa_entry->body.object_number, oa_entry->body.object_group))) {
        //val = _oa_type_version_adjust(val, oa_entry->header.def, oa_entry->body.object_number);

        *meta_type = val;
        return 0;
    }

    *meta_type = -1;
    return -1;
}
