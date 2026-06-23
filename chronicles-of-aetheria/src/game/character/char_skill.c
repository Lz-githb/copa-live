#include "char_skill.h"
#include "character.h"
#include "../../game/battle/battle_data.h"
#include "../../game/battle/battle_anim.h"
#include "gba_types.h"

/* ---- Skill table (ROM) ---------------------------------- */
const CharSkillDef g_char_skill_table[CSKILL_COUNT] = {

/* --- GUARDIÃO --- */
[CSKILL_SHIELD_BASH] = {
    "Golpe de Escudo","Ataque físico que atordoa o alvo",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 6, 50, 5, 3, ATTR_STR,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_STATUS_ADD,
    STATUS_CONFUSE, 60, 0, ANIM_ATTACK, 1, FALSE, FALSE, "BASH" },

[CSKILL_TAUNT] = {
    "Provocação","Força todos os inimigos a atacar o Guardião por 2 turnos",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 8, 0, 0, 0, ATTR_CHA,
    DMG_PHYSICAL, ELEM_NONE, TGT_ALL_ENEMIES, EFF_STATUS_ADD,
    STATUS_CONFUSE, 90, 0, ANIM_STATUS, 5, FALSE, FALSE, "TAUT" },

[CSKILL_GUARD] = {
    "Guarda","Reduz dano recebido pela equipe em 30% por 1 turno",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 10, 0, 0, 0, ATTR_END,
    DMG_PHYSICAL, ELEM_NONE, TGT_ALL_ALLIES, EFF_NONE,
    0, 0, 0, ANIM_STATUS, 8, FALSE, FALSE, "GARD" },

[CSKILL_IRON_WALL] = {
    "Muro de Ferro","DEF ×2 por 2 turnos",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 12, 0, 0, 0, ATTR_END,
    DMG_PHYSICAL, ELEM_NONE, TGT_SELF, EFF_STAT_MOD,
    0, 0, 0, ANIM_STATUS, 12, FALSE, FALSE, "WALL" },

[CSKILL_GUARDIAN_STRIKE] = {
    "Golpe do Guardião","Dano físico bônus igual a 50% da DEF própria",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 10, 70, 8, 5, ATTR_STR,
    DMG_PHYSICAL, ELEM_LIGHT, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    0, 0, 0, ANIM_ATTACK, 15, FALSE, FALSE, "GSTR" },

[CSKILL_PROVOKE] = {
    "Provocar","Reduz ATK do alvo em 20% por 3 turnos",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 8, 0, 0, 0, ATTR_CHA,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_STAT_MOD,
    0, 0, 0, ANIM_STATUS, 20, FALSE, FALSE, "PROV" },

[CSKILL_FORTRESS] = {
    "Fortaleza","Anula TODO dano físico por 1 turno",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 20, 0, 0, 0, ATTR_END,
    DMG_PHYSICAL, ELEM_NONE, TGT_SELF, EFF_NONE,
    0, 0, 2, ANIM_STATUS, 30, FALSE, FALSE, "FORT" },

[CSKILL_SACRED_GROUND] = {
    "Terra Sagrada","AOE sagrado + regeneração de HP para aliados",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 25, 80, 10, 4, ATTR_WIS,
    DMG_MAGICAL, ELEM_LIGHT, TGT_ALL_ENEMIES, EFF_DAMAGE,
    STATUS_REGEN, 100, 0, ANIM_LIGHT, 40, FALSE, FALSE, "SACR" },

[CSKILL_LAST_STAND] = {
    "Último Bastião","Quando HP<25%: ATK e DEF dobram por 3 turnos",
    CLASS_GUARDIAN, SKTYPE_REACT, 0, 0, 0, 0, ATTR_VIT,
    DMG_PHYSICAL, ELEM_NONE, TGT_SELF, EFF_STAT_MOD,
    0, 0, 0, ANIM_STATUS, 50, FALSE, FALSE, "LAST" },

[CSKILL_WARDEN_OATH] = {
    "Juramento do Guardião","Equipe não pode morrer por 1 turno",
    CLASS_GUARDIAN, SKTYPE_ACTIVE, 40, 0, 0, 0, ATTR_CHA,
    DMG_PHYSICAL, ELEM_LIGHT, TGT_ALL_ALLIES, EFF_NONE,
    0, 0, 5, ANIM_COMBO_HOLY, 70, FALSE, FALSE, "OATH" },

/* --- ESPADACHIM ARCANO --- */
[CSKILL_RUNIC_SLASH] = {
    "Corte Rúnico","Dano físico com elemento de fogo básico",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 5, 55, 6, 4, ATTR_STR,
    DMG_HYBRID, ELEM_FIRE, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    0, 0, 0, ANIM_FIRE, 1, FALSE, FALSE, "RSLH" },

[CSKILL_INFUSE_FIRE] = {
    "Infusão: Fogo","Arma ganha elemento fogo por 5 turnos",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 8, 0, 0, 0, ATTR_INT,
    DMG_MAGICAL, ELEM_FIRE, TGT_SELF, EFF_NONE,
    0, 0, 0, ANIM_FIRE, 5, FALSE, FALSE, "INF↑" },

[CSKILL_INFUSE_ICE] = {
    "Infusão: Gelo","Arma ganha elemento água por 5 turnos",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 8, 0, 0, 0, ATTR_INT,
    DMG_MAGICAL, ELEM_WATER, TGT_SELF, EFF_NONE,
    0, 0, 0, ANIM_WATER, 10, FALSE, FALSE, "INF~" },

[CSKILL_INFUSE_STORM] = {
    "Infusão: Tempestade","Arma ganha elemento ar por 5 turnos",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 8, 0, 0, 0, ATTR_INT,
    DMG_MAGICAL, ELEM_AIR, TGT_SELF, EFF_NONE,
    0, 0, 0, ANIM_AIR, 18, FALSE, FALSE, "INF/" },

[CSKILL_ARCANE_STRIKE] = {
    "Golpe Arcano","Dano híbrido (média de ATK e MAG)",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 10, 65, 7, 4, ATTR_INT,
    DMG_HYBRID, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    0, 0, 0, ANIM_ATTACK, 8, FALSE, FALSE, "ARCN" },

[CSKILL_MANA_BLADE] = {
    "Lâmina de Mana","Drena MP do inimigo ao causar dano",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 12, 60, 6, 4, ATTR_STR,
    DMG_HYBRID, ELEM_DARK, TGT_SINGLE_ENEMY, EFF_DRAIN,
    0, 0, 0, ANIM_DRAIN, 14, FALSE, FALSE, "MBLAD" },

[CSKILL_SPELL_PARRY] = {
    "Aparar Magia","Próximo feitiço é refletido de volta",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 14, 0, 0, 0, ATTR_WIS,
    DMG_MAGICAL, ELEM_NONE, TGT_SELF, EFF_STATUS_ADD,
    STATUS_REFLECT, 100, 0, ANIM_LIGHT, 22, FALSE, FALSE, "PRRY" },

[CSKILL_RUNIC_BURST] = {
    "Explosão Rúnica","AOE híbrido; consome infusão ativa para bônus",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 22, 90, 10, 5, ATTR_INT,
    DMG_HYBRID, ELEM_NONE, TGT_ALL_ENEMIES, EFF_DAMAGE,
    0, 0, 0, ANIM_UNLEASH_STORM, 28, FALSE, FALSE, "BRST" },

[CSKILL_DUAL_RESONANCE] = {
    "Dupla Ressonância","Dois golpes rápidos com elemento atual",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 18, 75, 8, 4, ATTR_STR,
    DMG_HYBRID, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_MULTI_HIT,
    0, 0, 0, ANIM_ATTACK, 40, FALSE, FALSE, "DRSO" },

[CSKILL_ARCANE_UNLEASH] = {
    "Liberação Arcana","Gasta toda mana disponível em explosão devastadora",
    CLASS_ARCSWORD, SKTYPE_ACTIVE, 0, 160, 15, 8, ATTR_INT,
    DMG_HYBRID, ELEM_NONE, TGT_ALL_ENEMIES, EFF_DAMAGE,
    0, 0, 5, ANIM_UNLEASH_SOLEIL, 60, FALSE, FALSE, "UNLS" },

/* --- ARCANISTA --- */
[CSKILL_ARC_FIRE] = {
    "Chama Arcana","Feitiço de fogo escalável",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 8, 70, 8, 5, ATTR_INT,
    DMG_MAGICAL, ELEM_FIRE, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    STATUS_BURN, 25, 0, ANIM_FIRE, 1, FALSE, FALSE, "FIRE" },

[CSKILL_ARC_BLIZZARD] = {
    "Nevasca Arcana","AOE de gelo; pode congelar",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 14, 65, 7, 4, ATTR_INT,
    DMG_MAGICAL, ELEM_WATER, TGT_ALL_ENEMIES, EFF_DAMAGE,
    STATUS_FREEZE, 20, 0, ANIM_WATER, 5, FALSE, FALSE, "BLIZ" },

[CSKILL_ARC_QUAKE] = {
    "Tremor Arcano","AOE de terra; baixa DEF",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 14, 65, 7, 4, ATTR_INT,
    DMG_MAGICAL, ELEM_EARTH, TGT_ALL_ENEMIES, EFF_STAT_MOD,
    0, 0, 0, ANIM_EARTH, 8, FALSE, FALSE, "QUAK" },

[CSKILL_SPELLCHARGE] = {
    "Sobrecarga","Gasta 1 turno para dobrar dano do próximo feitiço",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 0, 0, 0, 0, ATTR_INT,
    DMG_MAGICAL, ELEM_NONE, TGT_SELF, EFF_NONE,
    0, 0, 0, ANIM_STATUS, 10, FALSE, FALSE, "CHRG" },

[CSKILL_ARC_TEMPEST] = {
    "Tempestade Arcana","AOE de ar; pode cegar todos",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 16, 70, 7, 4, ATTR_INT,
    DMG_MAGICAL, ELEM_AIR, TGT_ALL_ENEMIES, EFF_STATUS_ADD,
    STATUS_BLIND, 40, 0, ANIM_AIR, 14, FALSE, FALSE, "TMPT" },

[CSKILL_MANA_SHIELD] = {
    "Escudo de Mana","Absorve dano com MP em vez de HP",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 16, 0, 0, 0, ATTR_WIS,
    DMG_MAGICAL, ELEM_NONE, TGT_SELF, EFF_STATUS_ADD,
    STATUS_BARRIER, 100, 0, ANIM_STATUS, 18, FALSE, FALSE, "MSHL" },

[CSKILL_ARC_NOVA] = {
    "Nova Arcana","Explosão de luz que cega todos os inimigos",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 20, 85, 9, 5, ATTR_INT,
    DMG_MAGICAL, ELEM_LIGHT, TGT_ALL_ENEMIES, EFF_STATUS_ADD,
    STATUS_BLIND, 60, 0, ANIM_LIGHT, 22, FALSE, FALSE, "NOVA" },

[CSKILL_CHAIN_BOLT] = {
    "Raio em Cadeia","Atinge 3 inimigos em sequência",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 18, 80, 8, 5, ATTR_INT,
    DMG_MAGICAL, ELEM_AIR, TGT_SINGLE_ENEMY, EFF_MULTI_HIT,
    0, 0, 0, ANIM_AIR, 30, FALSE, FALSE, "COLT" },

[CSKILL_ARC_VOID] = {
    "Vazio Arcano","Trevas + drena HP do alvo",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 22, 95, 10, 6, ATTR_INT,
    DMG_MAGICAL, ELEM_DARK, TGT_SINGLE_ENEMY, EFF_DRAIN,
    0, 0, 0, ANIM_DARK, 40, FALSE, FALSE, "VOID" },

[CSKILL_GRAND_ARCANA] = {
    "Grande Arcana","Todos os elementos simultâneos; atordoa todos",
    CLASS_ARCANIST, SKTYPE_ACTIVE, 50, 160, 15, 8, ATTR_INT,
    DMG_MAGICAL, ELEM_NONE, TGT_ALL_ENEMIES, EFF_DAMAGE,
    STATUS_CONFUSE, 70, 6, ANIM_COMBO_HOLY, 65, FALSE, FALSE, "GARC" },

/* --- CAÇADOR --- */
[CSKILL_QUICK_SHOT] = {
    "Tiro Rápido","Ataque veloz com arco, age primeiro",
    CLASS_HUNTER, SKTYPE_ACTIVE, 4, 45, 5, 4, ATTR_DEX,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    0, 0, 0, ANIM_ATTACK, 1, FALSE, TRUE, "QSHT" },

[CSKILL_POISON_ARROW] = {
    "Flecha Envenenada","Envenena o alvo por 5 turnos",
    CLASS_HUNTER, SKTYPE_ACTIVE, 6, 35, 4, 3, ATTR_DEX,
    DMG_PHYSICAL, ELEM_DARK, TGT_SINGLE_ENEMY, EFF_STATUS_ADD,
    STATUS_POISON, 80, 0, ANIM_DARK, 10, FALSE, TRUE, "PARR" },

[CSKILL_SNIPE] = {
    "Snipe","Ataque preciso e devastador com arco",
    CLASS_HUNTER, SKTYPE_ACTIVE, 14, 110, 12, 5, ATTR_DEX,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    0, 0, 1, ANIM_ATTACK, 22, FALSE, TRUE, "SNIP" },

[CSKILL_SHADOW_STEP] = {
    "Passo Sombra","Esquiva+1 turno e golpe surpresa",
    CLASS_HUNTER, SKTYPE_ACTIVE, 10, 55, 6, 4, ATTR_DEX,
    DMG_PHYSICAL, ELEM_DARK, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    0, 0, 0, ANIM_ATTACK, 5, FALSE, FALSE, "SHDW" },

[CSKILL_DUAL_FANG] = {
    "Garra Dupla","Dois golpes com adagas; req empunhadura dupla",
    CLASS_HUNTER, SKTYPE_ACTIVE, 8, 50, 6, 4, ATTR_DEX,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_MULTI_HIT,
    0, 0, 0, ANIM_ATTACK, 8, TRUE, FALSE, "DFNG" },

[CSKILL_SMOKE_BOMB] = {
    "Bomba de Fumaça","Cega todos os inimigos",
    CLASS_HUNTER, SKTYPE_ACTIVE, 10, 0, 0, 0, ATTR_LCK,
    DMG_PHYSICAL, ELEM_NONE, TGT_ALL_ENEMIES, EFF_STATUS_ADD,
    STATUS_BLIND, 75, 0, ANIM_STATUS, 14, FALSE, FALSE, "SMKE" },

[CSKILL_TRAP_BIND] = {
    "Armadilha","Imobiliza alvo por 2 turnos",
    CLASS_HUNTER, SKTYPE_ACTIVE, 8, 0, 0, 0, ATTR_DEX,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_STATUS_ADD,
    STATUS_PARALYZE, 80, 0, ANIM_STATUS, 18, FALSE, FALSE, "TRAP" },

[CSKILL_MARKED_PREY] = {
    "Presa Marcada","Alvo recebe +30% de dano por 3 turnos",
    CLASS_HUNTER, SKTYPE_ACTIVE, 6, 0, 0, 0, ATTR_LCK,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_STAT_MOD,
    0, 0, 0, ANIM_STATUS, 28, FALSE, FALSE, "MARK" },

[CSKILL_RAIN_OF_ARROWS] = {
    "Chuva de Flechas","AOE de arco em todos os inimigos",
    CLASS_HUNTER, SKTYPE_ACTIVE, 20, 65, 7, 4, ATTR_DEX,
    DMG_PHYSICAL, ELEM_NONE, TGT_ALL_ENEMIES, EFF_DAMAGE,
    0, 0, 0, ANIM_COMBO_STORM, 40, FALSE, TRUE, "RAIN" },

[CSKILL_DEATHMARK] = {
    "Marca da Morte","Se alvo HP<20%: KO instantâneo",
    CLASS_HUNTER, SKTYPE_ACTIVE, 25, 0, 0, 0, ATTR_LCK,
    DMG_TRUE, ELEM_DARK, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    STATUS_DOOM, 100, 4, ANIM_DARK, 60, FALSE, FALSE, "DETH" },

/* --- COMPARTILHADAS --- */
[CSKILL_BASIC_ATTACK] = {
    "Ataque Básico","Ataque físico sem custo",
    CLASS_COUNT, SKTYPE_ACTIVE, 0, 60, 3, 3, ATTR_STR,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ENEMY, EFF_DAMAGE,
    0, 0, 0, ANIM_ATTACK, 1, FALSE, FALSE, "ATK " },

[CSKILL_DEFEND] = {
    "Defender","Reduz dano recebido este turno",
    CLASS_COUNT, SKTYPE_ACTIVE, 0, 0, 0, 0, ATTR_END,
    DMG_PHYSICAL, ELEM_NONE, TGT_SELF, EFF_NONE,
    0, 0, 0, ANIM_STATUS, 1, FALSE, FALSE, "DEF " },

[CSKILL_USE_ITEM] = {
    "Item","Usa um item do inventário",
    CLASS_COUNT, SKTYPE_ACTIVE, 0, 0, 0, 0, ATTR_LCK,
    DMG_PHYSICAL, ELEM_NONE, TGT_SINGLE_ALLY, EFF_NONE,
    0, 0, 0, ANIM_HEAL, 1, FALSE, FALSE, "ITEM" },
};

/* ---- API ------------------------------------------------ */

void char_skill_init(void) { /* ROM data */ }

const CharSkillDef* skill_char_get(u8 id)
{
    if (id >= CSKILL_COUNT) return &g_char_skill_table[CSKILL_BASIC_ATTACK];
    return &g_char_skill_table[id];
}

s16 skill_effective_power(u8 skill_id, u8 char_level, s16 scaling_attr_val)
{
    const CharSkillDef* sk = skill_char_get(skill_id);
    s16 pow = sk->base_power;
    pow += (s16)((char_level / 10) * sk->power_growth);
    pow += (s16)((scaling_attr_val / 4) * sk->attr_scale);
    return pow < 1 ? 1 : pow;
}

BOOL skill_is_known(const Character* ch, u8 char_skill_id)
{
    u8 i;
    for (i = 0; i < ch->known_count; i++)
        if (ch->known_skills[i] == char_skill_id) return TRUE;
    return FALSE;
}

BOOL skill_can_use(const Character* ch, u8 char_skill_id)
{
    const CharSkillDef* sk;
    if (!skill_is_known(ch, char_skill_id)) return FALSE;
    sk = skill_char_get(char_skill_id);
    if (ch->level < sk->req_level) return FALSE;
    return TRUE;
}

BOOL skill_learn(Character* ch, u8 char_skill_id)
{
    if (skill_is_known(ch, char_skill_id)) return FALSE;
    if (ch->known_count >= CHAR_KNOWN_MAX) return FALSE;
    ch->known_skills[ch->known_count++] = char_skill_id;
    return TRUE;
}

BOOL skill_equip_to_slot(Character* ch, u8 hotbar_slot, u8 char_skill_id)
{
    if (hotbar_slot >= CHAR_SKILL_SLOTS) return FALSE;
    if (!skill_is_known(ch, char_skill_id)) return FALSE;
    ch->hotbar[hotbar_slot] = char_skill_id;
    return TRUE;
}

u8 skill_to_battle_id(u8 char_skill_id, u8 char_level,
                       const Character* ch)
{
    /* Map character skill to battle system skill ID.
     * For most skills this is a direct lookup; some map to
     * battle skills with adjusted power based on char_level. */
    (void)char_level; (void)ch;
    switch (char_skill_id) {
    case CSKILL_BASIC_ATTACK: return SKILL_ATTACK;
    case CSKILL_SHIELD_BASH:  return SKILL_SHATTER;
    case CSKILL_GUARD:        return SKILL_BARRIER_CAST;
    case CSKILL_IRON_WALL:    return SKILL_BARRIER_CAST;
    case CSKILL_GUARDIAN_STRIKE: return SKILL_HOLY;
    case CSKILL_SACRED_GROUND:   return SKILL_CURA;
    case CSKILL_FORTRESS:         return SKILL_BARRIER_CAST;
    case CSKILL_WARDEN_OATH:     return SKILL_CURA;
    case CSKILL_RUNIC_SLASH:     return SKILL_FIRE;
    case CSKILL_ARCANE_STRIKE:   return SKILL_SLASH;
    case CSKILL_MANA_BLADE:      return SKILL_DRAIN;
    case CSKILL_SPELL_PARRY:     return SKILL_BARRIER_CAST;
    case CSKILL_RUNIC_BURST:     return SKILL_FIRAGA;
    case CSKILL_DUAL_RESONANCE:  return SKILL_DOUBLE_CUT;
    case CSKILL_ARCANE_UNLEASH:  return SKILL_UNLEASH_SOLEIL;
    case CSKILL_ARC_FIRE:        return SKILL_FIRAGA;
    case CSKILL_ARC_BLIZZARD:    return SKILL_TSUNAMI;
    case CSKILL_ARC_QUAKE:       return SKILL_QUAKE;
    case CSKILL_ARC_TEMPEST:     return SKILL_CYCLONE;
    case CSKILL_ARC_NOVA:        return SKILL_RADIANCE;
    case CSKILL_ARC_VOID:        return SKILL_ECLIPSE;
    case CSKILL_CHAIN_BOLT:      return SKILL_CYCLONE;
    case CSKILL_GRAND_ARCANA:    return SKILL_ECLIPSE;
    case CSKILL_SPELLCHARGE:     return SKILL_HASTE_CAST;
    case CSKILL_MANA_SHIELD:     return SKILL_BARRIER_CAST;
    case CSKILL_QUICK_SHOT:      return SKILL_ATTACK;
    case CSKILL_POISON_ARROW:    return SKILL_DOOM;
    case CSKILL_SNIPE:           return SKILL_SLASH;
    case CSKILL_SHADOW_STEP:     return SKILL_SLASH;
    case CSKILL_DUAL_FANG:       return SKILL_DOUBLE_CUT;
    case CSKILL_SMOKE_BOMB:      return SKILL_FLASH;
    case CSKILL_TRAP_BIND:       return SKILL_SLOW_CAST;
    case CSKILL_MARKED_PREY:     return SKILL_BREAK;
    case CSKILL_RAIN_OF_ARROWS:  return SKILL_CYCLONE;
    case CSKILL_DEATHMARK:       return SKILL_DOOM;
    default:                     return SKILL_ATTACK;
    }
}
