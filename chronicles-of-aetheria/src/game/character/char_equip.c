#include "char_equip.h"
#include "char_class.h"
#include "character.h"
#include "gba_types.h"

/* ---- Equipment table (ROM) ------------------------------ */
/* bonus[] order: HP_MAX, MP_MAX, ATK, DEF, MAG, MDEF, SPD, CRIT, EVA */
const EquipDef g_equip_table[EQUIP_COUNT] = {

[EQUIP_NONE] = { "None", "", ITEM_NONE, 0, 0, 0, RARITY_COMMON,
                 {0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, 0,
                 EPASSIVE_NONE, 0, 0, 0 },

/* --- Swords (Guardian / Arcsword) --- */
[EQUIP_IRON_SWORD]   = { "Espada de Ferro",  "Espada básica confiável",
    ITEM_SWORD, EQUIP_SLOT_WEAPON, 1,
    (1<<CLASS_GUARDIAN)|(1<<CLASS_ARCSWORD), RARITY_COMMON,
    {0,0,12,0,0,0,0,0,0},{0,0,0,0,0,0,0,0},0, EPASSIVE_NONE,0, 80,40 },

[EQUIP_STEEL_SWORD]  = { "Espada de Aço", "Forjada em aço temperado",
    ITEM_SWORD, EQUIP_SLOT_WEAPON, 15,
    (1<<CLASS_GUARDIAN)|(1<<CLASS_ARCSWORD), RARITY_UNCOMMON,
    {0,0,24,0,0,0,2,0,0},{0,0,1,0,0,0,0,0},0, EPASSIVE_NONE,0, 300,150 },

[EQUIP_GUARDIAN_BLADE] = { "Lâmina do Guardião","Espada sagrada do proteger",
    ITEM_SWORD, EQUIP_SLOT_WEAPON, 30,
    (1<<CLASS_GUARDIAN), RARITY_RARE,
    {20,0,38,5,0,5,0,0,0},{0,1,1,0,0,0,0,0},0, EPASSIVE_DEF_AURA,5, 800,400 },

[EQUIP_IRON_GS]      = { "Montante de Ferro","Espada enorme e devastadora",
    ITEM_GREATSWORD, EQUIP_SLOT_WEAPON, 1,
    (1<<CLASS_GUARDIAN), RARITY_COMMON,
    {0,0,18,0,0,0,-3,0,0},{0,0,2,-1,0,0,0,0},0, EPASSIVE_NONE,0, 120,60 },

[EQUIP_TITAN_GS]     = { "Montante Titã","Força bruta encarnada",
    ITEM_GREATSWORD, EQUIP_SLOT_WEAPON, 40,
    (1<<CLASS_GUARDIAN), RARITY_EPIC,
    {0,0,60,0,0,0,-5,5,0},{0,0,3,-1,0,0,0,0},0, EPASSIVE_LIFESTEAL_10,10, 2000,1000 },

/* --- Runic swords (Arcsword) --- */
[EQUIP_RUNIC_SWORD]  = { "Espada Rúnica","Absorve magia para o fio",
    ITEM_SWORD, EQUIP_SLOT_WEAPON, 10,
    (1<<CLASS_ARCSWORD), RARITY_UNCOMMON,
    {0,10,18,0,8,0,0,0,0},{0,0,0,0,1,0,0,0},0, EPASSIVE_SPELL_AMP,15, 500,250 },

[EQUIP_ARCSWORD_BLADE]={ "Lâmina Arcana Suprema","Equilíbrio perfeito entre fio e magia",
    ITEM_SWORD, EQUIP_SLOT_WEAPON, 50,
    (1<<CLASS_ARCSWORD), RARITY_LEGENDARY,
    {0,20,40,0,30,0,5,5,0},{0,1,2,0,2,0,0,1},0, EPASSIVE_SPELL_AMP,25, 5000,2500 },

/* --- Staves (Arcanist) --- */
[EQUIP_WOODEN_STAFF] = { "Cajado de Madeira","Primeiro cajado de um aprendiz",
    ITEM_STAFF, EQUIP_SLOT_WEAPON, 1,
    (1<<CLASS_ARCANIST), RARITY_COMMON,
    {0,15,0,0,14,5,0,0,0},{0,0,0,0,1,0,0,0},0, EPASSIVE_NONE,0, 60,30 },

[EQUIP_FIRE_STAFF]   = { "Cajado de Fogo","Canaliza chamas elementais",
    ITEM_STAFF, EQUIP_SLOT_WEAPON, 15,
    (1<<CLASS_ARCANIST), RARITY_UNCOMMON,
    {0,20,0,0,28,8,0,0,0},{0,0,0,0,2,0,0,0},ELEM_FIRE, EPASSIVE_ELEM_BOOST,30, 600,300 },

[EQUIP_ICE_STAFF]    = { "Cajado de Gelo","Congela a mana do inimigo",
    ITEM_STAFF, EQUIP_SLOT_WEAPON, 15,
    (1<<CLASS_ARCANIST), RARITY_UNCOMMON,
    {0,25,0,0,26,10,0,0,0},{0,0,0,0,1,1,0,0},ELEM_WATER, EPASSIVE_ELEM_BOOST,30, 600,300 },

[EQUIP_GRAND_ORB]    = { "Orbe Supremo","Poder mágico incomensurável",
    ITEM_ORB, EQUIP_SLOT_WEAPON, 55,
    (1<<CLASS_ARCANIST), RARITY_LEGENDARY,
    {0,60,0,0,55,20,0,0,0},{0,2,0,0,4,2,0,0},0, EPASSIVE_SPELL_AMP,30, 6000,3000 },

/* --- Daggers / Bow (Hunter) --- */
[EQUIP_BONE_DAGGER]  = { "Adaga de Osso","Leve e afiada",
    ITEM_DAGGER, EQUIP_SLOT_WEAPON, 1,
    (1<<CLASS_HUNTER)|(1<<CLASS_ARCSWORD), RARITY_COMMON,
    {0,0,10,0,0,0,4,2,0},{0,0,0,0,0,0,1,0},0, EPASSIVE_NONE,0, 70,35 },

[EQUIP_SHADOW_DAGGER] = { "Adaga das Sombras","Envenena ao perfurar",
    ITEM_DAGGER, EQUIP_SLOT_WEAPON, 20,
    (1<<CLASS_HUNTER), RARITY_RARE,
    {0,0,22,0,0,0,6,4,0},{0,0,0,0,0,0,1,1},0, EPASSIVE_CRIT_UP_10,10, 900,450 },

[EQUIP_SHORT_BOW]    = { "Arco Curto","Ataques rápidos à distância",
    ITEM_BOW, EQUIP_SLOT_WEAPON, 1,
    (1<<CLASS_HUNTER), RARITY_COMMON,
    {0,0,8,0,0,0,6,0,0},{0,0,0,0,0,0,1,0},0, EPASSIVE_NONE,0, 80,40 },

[EQUIP_HUNTER_BOW]   = { "Arco do Caçador","Velocidade letal",
    ITEM_BOW, EQUIP_SLOT_WEAPON, 30,
    (1<<CLASS_HUNTER), RARITY_EPIC,
    {0,0,30,0,0,0,10,6,0},{0,0,1,0,0,0,2,1},0, EPASSIVE_DOUBLESHOT,0, 2500,1250 },

/* --- Offhands --- */
[EQUIP_IRON_SHIELD]  = { "Escudo de Ferro","Bloqueio confiável",
    ITEM_SHIELD, EQUIP_SLOT_OFFHAND, 1,
    (1<<CLASS_GUARDIAN), RARITY_COMMON,
    {0,0,0,14,0,4,0,0,5},{0,1,0,0,0,0,0,0},0, EPASSIVE_AUTO_GUARD,10, 100,50 },

[EQUIP_TOWER_SHIELD] = { "Escudo Torre","Fortaleza ambulante",
    ITEM_SHIELD, EQUIP_SLOT_OFFHAND, 25,
    (1<<CLASS_GUARDIAN), RARITY_RARE,
    {30,0,0,25,0,8,0,0,8},{2,2,0,0,0,0,0,0},0, EPASSIVE_AUTO_GUARD,20, 1000,500 },

[EQUIP_FOCUS_CRYSTAL] = { "Cristal de Foco","Amplifica a infusão arcana",
    ITEM_FOCUS, EQUIP_SLOT_OFFHAND, 10,
    (1<<CLASS_ARCSWORD), RARITY_UNCOMMON,
    {0,15,0,0,10,5,0,0,0},{0,0,0,0,1,0,0,0},0, EPASSIVE_SPELL_AMP,10, 400,200 },

[EQUIP_QUIVER]       = { "Aljava Rápida","Disparo mais rápido",
    ITEM_QUIVER, EQUIP_SLOT_OFFHAND, 1,
    (1<<CLASS_HUNTER), RARITY_COMMON,
    {0,0,0,0,0,0,8,0,4},{0,0,0,0,0,0,1,0},0, EPASSIVE_QUICK_DRAW,10, 60,30 },

/* --- Helms --- */
[EQUIP_IRON_HELM]    = { "Elmo de Ferro","Protege a cabeça",
    ITEM_HELM_HEAVY, EQUIP_SLOT_HELM, 1,
    (1<<CLASS_GUARDIAN)|(1<<CLASS_ARCSWORD), RARITY_COMMON,
    {15,0,0,8,0,2,0,0,0},{1,0,0,1,0,0,0,0},0, EPASSIVE_NONE,0, 90,45 },

[EQUIP_LEATHER_HOOD] = { "Capuz de Couro","Leve e discreto",
    ITEM_HELM_LIGHT, EQUIP_SLOT_HELM, 1,
    (1<<CLASS_HUNTER)|(1<<CLASS_ARCSWORD)|(1<<CLASS_ARCANIST), RARITY_COMMON,
    {8,0,0,4,0,2,2,0,2},{0,0,0,0,0,0,0,1},0, EPASSIVE_NONE,0, 70,35 },

[EQUIP_MAGE_HAT]     = { "Chapéu Arcano","Amplifica ondas de mana",
    ITEM_HELM_LIGHT, EQUIP_SLOT_HELM, 10,
    (1<<CLASS_ARCANIST), RARITY_UNCOMMON,
    {0,20,0,0,8,6,0,0,0},{0,1,0,0,1,0,0,0},0, EPASSIVE_MP_REGEN,2, 350,175 },

/* --- Armors --- */
[EQUIP_CHAIN_ARMOR]  = { "Cota de Malha","Proteção média versátil",
    ITEM_ARMOR_MEDIUM, EQUIP_SLOT_ARMOR, 10,
    (1<<CLASS_GUARDIAN)|(1<<CLASS_ARCSWORD)|(1<<CLASS_HUNTER), RARITY_COMMON,
    {20,0,0,12,0,6,0,0,0},{1,1,0,0,0,0,0,0},0, EPASSIVE_NONE,0, 200,100 },

[EQUIP_PLATE_ARMOR]  = { "Armadura de Placas","Máxima proteção física",
    ITEM_ARMOR_HEAVY, EQUIP_SLOT_ARMOR, 20,
    (1<<CLASS_GUARDIAN), RARITY_RARE,
    {40,0,0,24,0,8,-3,0,0},{2,2,0,1,0,0,0,0},0, EPASSIVE_NONE,0, 700,350 },

[EQUIP_LEATHER_VEST] = { "Colete de Couro","Equilíbrio entre proteção e mobilidade",
    ITEM_ARMOR_LIGHT, EQUIP_SLOT_ARMOR, 1,
    (1<<CLASS_HUNTER)|(1<<CLASS_ARCSWORD), RARITY_COMMON,
    {10,0,0,6,0,4,2,0,2},{0,0,0,0,0,0,1,0},0, EPASSIVE_NONE,0, 80,40 },

[EQUIP_ROBE_ARCANE]  = { "Robe Arcano","Tecido imbuído de runa de proteção",
    ITEM_ROBE, EQUIP_SLOT_ARMOR, 5,
    (1<<CLASS_ARCANIST), RARITY_UNCOMMON,
    {0,30,0,4,6,14,0,0,0},{0,2,0,0,0,1,0,0},0, EPASSIVE_REFLECT_5,5, 300,150 },

/* --- Accessories --- */
[EQUIP_RING_POWER]   = { "Anel de Força","Bônus de ataque simples",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC1, 1, 0xFF, RARITY_COMMON,
    {0,0,8,0,4,0,0,0,0},{0,0,1,0,0,0,0,0},0, EPASSIVE_NONE,0, 150,75 },

[EQUIP_RING_SPEED]   = { "Anel de Velocidade","Quem golpeia primeiro vence",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC1, 1, 0xFF, RARITY_COMMON,
    {0,0,0,0,0,0,10,3,3},{0,0,0,0,0,0,1,0},0, EPASSIVE_NONE,0, 150,75 },

[EQUIP_AMULET_MANA]  = { "Amuleto de Mana","Reserva de mana expandida",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC1, 1, 0xFF, RARITY_UNCOMMON,
    {0,40,0,0,5,5,0,0,0},{0,2,0,0,0,0,0,0},0, EPASSIVE_MP_REGEN,3, 300,150 },

[EQUIP_PENDANT_CRIT] = { "Pingente Crítico","A morte chega sem aviso",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC1, 20, 0xFF, RARITY_RARE,
    {0,0,5,0,0,0,0,12,0},{0,0,0,0,0,0,0,1},0, EPASSIVE_CRIT_UP_10,10, 600,300 },

[EQUIP_CLOAK_SHADOW] = { "Manto das Sombras","Caçador lendário das trevas",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC2, 60,
    (1<<CLASS_HUNTER), RARITY_LEGENDARY,
    {0,0,15,0,0,0,18,15,10},{0,0,0,0,0,0,2,1},0, EPASSIVE_LIFESTEAL_10,15, 8000,4000 },

[EQUIP_CROWN_ARCANE] = { "Coroa Arcana","Conhecimento além dos mortais",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC2, 60,
    (1<<CLASS_ARCANIST), RARITY_LEGENDARY,
    {0,80,0,0,25,15,0,0,0},{0,3,0,0,3,2,0,0},0, EPASSIVE_SPELL_AMP,30, 8000,4000 },

[EQUIP_WARDEN_CREST] = { "Crista do Guardião","Símbolo do protetor eterno",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC2, 60,
    (1<<CLASS_GUARDIAN), RARITY_LEGENDARY,
    {80,0,10,20,0,10,0,0,5},{3,2,0,2,0,0,0,0},0, EPASSIVE_DEF_AURA,10, 8000,4000 },

[EQUIP_ARCSWORD_RELIC] = { "Relíquia Arcana","Herança do primeiro Espadachim Arcano",
    ITEM_ACCESSORY, EQUIP_SLOT_ACC2, 60,
    (1<<CLASS_ARCSWORD), RARITY_LEGENDARY,
    {20,30,20,5,20,5,8,8,0},{1,1,1,0,1,0,0,0},0, EPASSIVE_SPELL_AMP,20, 8000,4000 },
};

/* ---- API ------------------------------------------------ */

void equip_system_init(void) { /* ROM data — nothing to init */ }

const EquipDef* equip_get(u8 id)
{
    if (id >= EQUIP_COUNT) return &g_equip_table[EQUIP_NONE];
    return &g_equip_table[id];
}

BOOL equip_can_use(const Character* ch, u8 item_id)
{
    const EquipDef* def = equip_get(item_id);
    if (item_id == EQUIP_NONE) return FALSE;
    if (def->req_level > ch->level) return FALSE;
    if (def->class_mask == 0xFF) return TRUE; /* all classes */
    return (BOOL)((def->class_mask & (1u << ch->cls)) != 0);
}

BOOL equip_item(Character* ch, u8 slot, u8 item_id)
{
    const EquipDef* def;
    u8 old;

    if (slot >= EQUIP_SLOT_COUNT) return FALSE;
    if (item_id == EQUIP_NONE) { ch->equip[slot] = EQUIP_NONE; return TRUE; }
    if (!equip_can_use(ch, item_id)) return FALSE;

    def = equip_get(item_id);
    if (!class_can_equip(ch->cls, slot, def->type)) return FALSE;

    /* Unequip current item in slot first */
    old = ch->equip[slot];
    if (old != EQUIP_NONE) inventory_add(ch, old);

    /* Remove from inventory */
    inventory_remove(ch, item_id);
    ch->equip[slot] = item_id;
    return TRUE;
}

u8 equip_remove(Character* ch, u8 slot)
{
    u8 id;
    if (slot >= EQUIP_SLOT_COUNT) return EQUIP_NONE;
    id = ch->equip[slot];
    if (id == EQUIP_NONE) return EQUIP_NONE;
    ch->equip[slot] = EQUIP_NONE;
    inventory_add(ch, id);
    return id;
}

void equip_compute_bonus(const Character* ch, s16 out[DSTAT_COUNT])
{
    u8 s, d;
    for (d = 0; d < DSTAT_COUNT; d++) out[d] = 0;
    for (s = 0; s < EQUIP_SLOT_COUNT; s++) {
        const EquipDef* def = equip_get(ch->equip[s]);
        if (!def || ch->equip[s] == EQUIP_NONE) continue;
        for (d = 0; d < DSTAT_COUNT; d++) out[d] += def->bonus[d];
    }
}

void equip_compute_attr_bonus(const Character* ch, s8 out[ATTR_COUNT])
{
    u8 s, a;
    for (a = 0; a < ATTR_COUNT; a++) out[a] = 0;
    for (s = 0; s < EQUIP_SLOT_COUNT; s++) {
        const EquipDef* def = equip_get(ch->equip[s]);
        if (!def || ch->equip[s] == EQUIP_NONE) continue;
        for (a = 0; a < ATTR_COUNT; a++) out[a] += def->attr_bonus[a];
    }
}

const EquipDef* equip_get_slot(const Character* ch, u8 slot)
{
    if (slot >= EQUIP_SLOT_COUNT) return 0;
    if (ch->equip[slot] == EQUIP_NONE) return 0;
    return equip_get(ch->equip[slot]);
}

BOOL inventory_add(Character* ch, u8 item_id)
{
    u8 i;
    if (item_id == EQUIP_NONE) return FALSE;
    /* Find empty slot */
    for (i = 0; i < INVENTORY_SIZE; i++) {
        if (ch->inventory[i].item_id == 0) {
            ch->inventory[i].item_id = item_id;
            ch->inventory[i].count   = 1;
            if (i >= ch->inventory_count) ch->inventory_count = (u8)(i + 1);
            return TRUE;
        }
    }
    return FALSE; /* inventory full */
}

BOOL inventory_remove(Character* ch, u8 item_id)
{
    u8 idx = inventory_find(ch, item_id);
    if (idx == 0xFF) return FALSE;
    ch->inventory[idx].item_id = 0;
    ch->inventory[idx].count   = 0;
    return TRUE;
}

u8 inventory_find(const Character* ch, u8 item_id)
{
    u8 i;
    for (i = 0; i < INVENTORY_SIZE; i++)
        if (ch->inventory[i].item_id == item_id) return i;
    return 0xFF;
}
