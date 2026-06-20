#include "battle_formula.h"
#include "battle_status.h"
#include "battler.h"
#include "gba_types.h"

/* Element affinity matrix [atk_elem][def_elem], values in % */
const u8 ELEM_MATRIX[ELEM_COUNT][ELEM_COUNT] = {
/*            NONE FIRE WATER EARTH  AIR LIGHT DARK */
/* NONE  */ { 100, 100,  100,  100, 100,  100,  100 },
/* FIRE  */ { 100, 100,  200,   50, 100,  100,  150 },
/* WATER */ { 100,  50,  100,  150, 100,  100,  100 },
/* EARTH */ { 100, 100,   50,  100, 200,  100,  100 },
/* AIR   */ { 100, 100,  100,   50, 100,  100,  200 },
/* LIGHT */ { 100, 100,  100,  100, 100,  100,  200 },
/* DARK  */ { 100, 100,  100,  100, 100,  200,  100 },
};

static u32 s_rng = 0x1234ABCDu;
static u32 _rand(void) {
    s_rng = s_rng * 1664525u + 1013904223u;
    return s_rng;
}

s16 formula_variance(s16 value)
{
    /* ±10%: multiply by 90-110, divide by 100 */
    s32 pct = (s32)(90 + (_rand() % 21));
    return (s16)((s32)value * pct / 100);
}

s16 formula_apply_element(s16 raw_dmg, ElementType atk_elem, u8 defender_resist)
{
    /* defender_resist is the value from battler->resist[atk_elem] (0-255) */
    s32 v = (s32)raw_dmg * defender_resist / 100;
    return (s16)(v < 0 ? 0 : v);
}

static s16 _clamp(s32 v)
{
    if (v < 1)    return 1;
    if (v > 9999) return 9999;
    return (s16)v;
}

DamageResult formula_physical(const Battler* atk, const Battler* def,
                               u8 power, ElementType element)
{
    DamageResult r;
    s32 base, dmg;

    r.damage        = 0;
    r.is_critical   = FALSE;
    r.is_miss       = FALSE;
    r.is_absorb     = FALSE;
    r.element       = element;
    r.effectiveness = ELEM_NEUTRAL;

    /* Blind miss check */
    if (status_has(atk, STATUS_BLIND)) {
        if ((_rand() % 100) < 50) { r.is_miss = TRUE; return r; }
    }

    {
        s16 a = battler_eff_atk(atk);
        s16 d = battler_eff_def(def);
        base = ((s32)a * a * power) / ((s32)d * 32 + 1);
    }
    if (base < 1) base = 1;

    /* Critical */
    if ((_rand() % 100) < 15) {
        r.is_critical = TRUE;
        base = base * CRIT_MULT / 100;
    }

    /* Defend */
    if (def->defending) base = base * DEFEND_MULT / 100;

    /* Element */
    if (element != ELEM_NONE) {
        u8 res = def->resist[element];
        base = (s32)base * res / 100;
        if (res == 0)   { r.is_miss = TRUE; return r; }
        if (res >= 200) r.effectiveness = ELEM_VULN;
        else if (res <= 50) r.effectiveness = ELEM_RESIST;
        if (def->resist[element] == ELEM_ABSORB) {
            r.is_absorb = TRUE;
            r.damage    = (s16)-_clamp(base);
            return r;
        }
    }

    dmg = formula_variance((s16)base);
    r.damage = _clamp(dmg);
    return r;
}

DamageResult formula_magical(const Battler* atk, const Battler* def,
                              u8 power, ElementType element)
{
    DamageResult r;
    s32 base, dmg;

    r.damage        = 0;
    r.is_critical   = FALSE;
    r.is_miss       = FALSE;
    r.is_absorb     = FALSE;
    r.element       = element;
    r.effectiveness = ELEM_NEUTRAL;

    {
        s16 m   = battler_eff_mag(atk);
        s16 md  = battler_eff_mdef(def);
        base = ((s32)m * power) / ((s32)md / 2 + 1) * 2;
    }
    if (base < 1) base = 1;

    if (def->defending) base = base * DEFEND_MULT / 100;

    if (element != ELEM_NONE) {
        u8 res = def->resist[element];
        if (res == 0) { r.is_miss = TRUE; return r; }
        base = (s32)base * res / 100;
        if (res >= 200) r.effectiveness = ELEM_VULN;
        else if (res <= 50) r.effectiveness = ELEM_RESIST;
        if (res == ELEM_ABSORB) {
            r.is_absorb = TRUE;
            r.damage    = (s16)-_clamp(base);
            return r;
        }
    }

    dmg = formula_variance((s16)base);
    r.damage = _clamp(dmg);
    return r;
}

s16 formula_heal(const Battler* caster, u8 power)
{
    s32 v = ((s32)battler_eff_mag(caster) * power) / 12;
    return formula_variance((s16)(v < 1 ? 1 : v));
}

DamageResult formula_drain(const Battler* atk, const Battler* def,
                            u8 power, ElementType element)
{
    DamageResult r = formula_magical(atk, def, power, element);
    /* Caller handles the self-heal (half damage) */
    return r;
}

BOOL formula_status_hit(const Battler* attacker, const Battler* defender,
                        u8 base_chance)
{
    s32 chance = base_chance;
    if (status_has(defender, STATUS_BARRIER)) chance /= 2;
    (void)attacker;
    return (BOOL)((_rand() % 100) < (u32)chance);
}

BOOL formula_flee(const Battler* fleeing_party, u8 flee_mult)
{
    s32 chance = 60 - (s16)(flee_mult * 10);
    if (chance < 10) chance = 10;
    if (chance > 95) chance = 95;
    (void)fleeing_party;
    return (BOOL)((_rand() % 100) < (u32)chance);
}
