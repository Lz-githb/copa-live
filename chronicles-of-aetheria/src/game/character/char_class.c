#include "char_class.h"
#include "char_skill.h"
#include "gba_types.h"

/* ---- Skill unlock lists per class ----------------------- */

static const SkillUnlock s_guardian_skills[] = {
    { CSKILL_BASIC_ATTACK,    1 },
    { CSKILL_DEFEND,          1 },
    { CSKILL_SHIELD_BASH,     1 },
    { CSKILL_TAUNT,           5 },
    { CSKILL_GUARD,           8 },
    { CSKILL_IRON_WALL,      12 },
    { CSKILL_GUARDIAN_STRIKE,15 },
    { CSKILL_PROVOKE,        20 },
    { CSKILL_FORTRESS,       30 },
    { CSKILL_SACRED_GROUND,  40 },
    { CSKILL_LAST_STAND,     50 },
    { CSKILL_WARDEN_OATH,    70 },
};

static const SkillUnlock s_arcsword_skills[] = {
    { CSKILL_BASIC_ATTACK,    1 },
    { CSKILL_DEFEND,          1 },
    { CSKILL_RUNIC_SLASH,     1 },
    { CSKILL_INFUSE_FIRE,     5 },
    { CSKILL_ARCANE_STRIKE,   8 },
    { CSKILL_INFUSE_ICE,     10 },
    { CSKILL_MANA_BLADE,     14 },
    { CSKILL_INFUSE_STORM,   18 },
    { CSKILL_SPELL_PARRY,    22 },
    { CSKILL_RUNIC_BURST,    28 },
    { CSKILL_DUAL_RESONANCE, 40 },
    { CSKILL_ARCANE_UNLEASH, 60 },
};

static const SkillUnlock s_arcanist_skills[] = {
    { CSKILL_BASIC_ATTACK,    1 },
    { CSKILL_DEFEND,          1 },
    { CSKILL_ARC_FIRE,        1 },
    { CSKILL_ARC_BLIZZARD,    5 },
    { CSKILL_ARC_QUAKE,       8 },
    { CSKILL_SPELLCHARGE,    10 },
    { CSKILL_ARC_TEMPEST,    14 },
    { CSKILL_MANA_SHIELD,    18 },
    { CSKILL_ARC_NOVA,       22 },
    { CSKILL_CHAIN_BOLT,     30 },
    { CSKILL_ARC_VOID,       40 },
    { CSKILL_GRAND_ARCANA,   65 },
};

static const SkillUnlock s_hunter_skills[] = {
    { CSKILL_BASIC_ATTACK,    1 },
    { CSKILL_DEFEND,          1 },
    { CSKILL_QUICK_SHOT,      1 },
    { CSKILL_SHADOW_STEP,     5 },
    { CSKILL_DUAL_FANG,       8 },
    { CSKILL_POISON_ARROW,   10 },
    { CSKILL_SMOKE_BOMB,     14 },
    { CSKILL_TRAP_BIND,      18 },
    { CSKILL_SNIPE,          22 },
    { CSKILL_MARKED_PREY,    28 },
    { CSKILL_RAIN_OF_ARROWS, 40 },
    { CSKILL_DEATHMARK,      60 },
};

/* ---- Class definitions (ROM) ---------------------------- */
const ClassDef g_class_table[CLASS_COUNT] = {

[CLASS_GUARDIAN] = {
    .name        = "Guardião",
    .description = "Guerreiro impenetrável. Protege aliados com escudo e provocação.",
    .sprite_base = 0,
    .start_attrs = { 18, 16, 14, 8, 4, 6, 8, 10 }, /* VIT END STR DEX INT WIS LCK CHA */
    .weapon_mask  = CLS_GUARDIAN_WEAPONS,
    .offhand_mask = CLS_GUARDIAN_OFFHAND,
    .armor_mask   = CLS_GUARDIAN_ARMOR,
    .helm_mask    = CLS_GUARDIAN_HELM,
    .stat_bonus   = { 80, 20, 10, 20, 0, 10, -5, 0 },
    .skill_unlocks = {
        { CSKILL_BASIC_ATTACK,1 },{ CSKILL_DEFEND,1 },
        { CSKILL_SHIELD_BASH,1 },{ CSKILL_TAUNT,5 },
        { CSKILL_GUARD,8 },{ CSKILL_IRON_WALL,12 },
        { CSKILL_GUARDIAN_STRIKE,15 },{ CSKILL_PROVOKE,20 },
        { CSKILL_FORTRESS,30 },{ CSKILL_SACRED_GROUND,40 },
        { CSKILL_LAST_STAND,50 },{ CSKILL_WARDEN_OATH,70 },
    },
    .skill_unlock_count = 12,
    .passive_name = "Mural de Ferro",
    .passive_desc = "Dano físico recebido -10% quando HP > 50%",
    .mechanic_id  = MECH_GUARDIAN_TAUNT,
},

[CLASS_ARCSWORD] = {
    .name        = "Espadachim Arcano",
    .description = "Une força física e magia elemental numa única lâmina.",
    .sprite_base = 4,
    .start_attrs = { 14, 12, 14, 12, 12, 10, 10, 10 },
    .weapon_mask  = CLS_ARCSWORD_WEAPONS,
    .offhand_mask = CLS_ARCSWORD_OFFHAND,
    .armor_mask   = CLS_ARCSWORD_ARMOR,
    .helm_mask    = CLS_ARCSWORD_HELM,
    .stat_bonus   = { 40, 40, 5, 5, 10, 5, 5, 5 },
    .skill_unlocks = {
        { CSKILL_BASIC_ATTACK,1 },{ CSKILL_DEFEND,1 },
        { CSKILL_RUNIC_SLASH,1 },{ CSKILL_INFUSE_FIRE,5 },
        { CSKILL_ARCANE_STRIKE,8 },{ CSKILL_INFUSE_ICE,10 },
        { CSKILL_MANA_BLADE,14 },{ CSKILL_INFUSE_STORM,18 },
        { CSKILL_SPELL_PARRY,22 },{ CSKILL_RUNIC_BURST,28 },
        { CSKILL_DUAL_RESONANCE,40 },{ CSKILL_ARCANE_UNLEASH,60 },
    },
    .skill_unlock_count = 12,
    .passive_name = "Ressonância Arcana",
    .passive_desc = "Infusão ativa aumenta dano mágico em 15%",
    .mechanic_id  = MECH_ARCSWORD_INFUSE,
},

[CLASS_ARCANIST] = {
    .name        = "Arcanista",
    .description = "Mestre das artes mágicas. Fragilidade compensada por poder devastador.",
    .sprite_base = 8,
    .start_attrs = { 8, 8, 4, 10, 20, 16, 12, 12 },
    .weapon_mask  = CLS_ARCANIST_WEAPONS,
    .offhand_mask = CLS_ARCANIST_OFFHAND,
    .armor_mask   = CLS_ARCANIST_ARMOR,
    .helm_mask    = CLS_ARCANIST_HELM,
    .stat_bonus   = { 10, 80, 0, 0, 20, 15, 0, 0 },
    .skill_unlocks = {
        { CSKILL_BASIC_ATTACK,1 },{ CSKILL_DEFEND,1 },
        { CSKILL_ARC_FIRE,1 },{ CSKILL_ARC_BLIZZARD,5 },
        { CSKILL_ARC_QUAKE,8 },{ CSKILL_SPELLCHARGE,10 },
        { CSKILL_ARC_TEMPEST,14 },{ CSKILL_MANA_SHIELD,18 },
        { CSKILL_ARC_NOVA,22 },{ CSKILL_CHAIN_BOLT,30 },
        { CSKILL_ARC_VOID,40 },{ CSKILL_GRAND_ARCANA,65 },
    },
    .skill_unlock_count = 12,
    .passive_name = "Sobrecarga de Mana",
    .passive_desc = "Cada feitiço consecutivo aumenta dano em 5% (máx 25%)",
    .mechanic_id  = MECH_ARCANIST_CHARGE,
},

[CLASS_HUNTER] = {
    .name        = "Caçador",
    .description = "Predador veloz. Supera inimigos com velocidade, veneno e armadilhas.",
    .sprite_base = 12,
    .start_attrs = { 12, 10, 12, 18, 6, 8, 14, 12 },
    .weapon_mask  = CLS_HUNTER_WEAPONS,
    .offhand_mask = CLS_HUNTER_OFFHAND,
    .armor_mask   = CLS_HUNTER_ARMOR,
    .helm_mask    = CLS_HUNTER_HELM,
    .stat_bonus   = { 30, 20, 8, 0, 0, 0, 15, 10 },
    .skill_unlocks = {
        { CSKILL_BASIC_ATTACK,1 },{ CSKILL_DEFEND,1 },
        { CSKILL_QUICK_SHOT,1 },{ CSKILL_SHADOW_STEP,5 },
        { CSKILL_DUAL_FANG,8 },{ CSKILL_POISON_ARROW,10 },
        { CSKILL_SMOKE_BOMB,14 },{ CSKILL_TRAP_BIND,18 },
        { CSKILL_SNIPE,22 },{ CSKILL_MARKED_PREY,28 },
        { CSKILL_RAIN_OF_ARROWS,40 },{ CSKILL_DEATHMARK,60 },
    },
    .skill_unlock_count = 12,
    .passive_name = "Instinto Predatório",
    .passive_desc = "Dano crítico +50%. Velocidade+5 ao atacar alvo envenenado",
    .mechanic_id  = MECH_HUNTER_DUALSTRIKE,
},

};

/* ---- API ------------------------------------------------ */

const ClassDef* class_get(CharClass cls)
{
    if (cls >= CLASS_COUNT) return &g_class_table[0];
    return &g_class_table[cls];
}

BOOL class_can_equip(CharClass cls, u8 slot, ItemType item_type)
{
    const ClassDef* def = class_get(cls);
    u32 mask = 0;
    u32 bit  = (u32)(1u << item_type);

    switch (slot) {
    case EQUIP_SLOT_WEAPON:  mask = def->weapon_mask;  break;
    case EQUIP_SLOT_OFFHAND: mask = def->offhand_mask; break;
    case EQUIP_SLOT_HELM:    mask = def->helm_mask;    break;
    case EQUIP_SLOT_ARMOR:   mask = def->armor_mask;   break;
    case EQUIP_SLOT_ACC1:
    case EQUIP_SLOT_ACC2:    mask = (1u << ITEM_ACCESSORY); break;
    default: return FALSE;
    }

    return (BOOL)((mask & bit) != 0);
}

u8 class_get_sprite(CharClass cls)
{
    return g_class_table[cls].sprite_base;
}

const char* class_name(CharClass cls)
{
    if (cls >= CLASS_COUNT) return "???";
    return g_class_table[cls].name;
}
