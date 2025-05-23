namespace rogue_unlike {

randeff inc_str("Strength", "Increases your Strength by 50%.", "You feel stronger!", [] (data &d) { m.next.stats[stat::str] += m.current.stats[stat::str] / 3; });
randeff inc_dex("Dexterity", "Increases your Dexterity by 50%.", "You feel faster!", [] (data &d) { m.next.stats[stat::dex] += m.current.stats[stat::dex] / 3; });
randeff inc_con("Toughness", "Increases your Toughness by 50%.", "You feel tougher!", [] (data &d) { m.next.stats[stat::con] += m.current.stats[stat::con] / 3; });
randeff inc_wis("Wisdom", "Increases your Wisdom by 50%.", "You feel wiser!", [] (data &d) { m.next.stats[stat::wis] += m.current.stats[stat::wis] / 3; });
randeff dec_str("Weakness", "Decreases your Strength by 25%.", "You feel weaker!", [] (data &d) { m.next.stats[stat::str] -= m.current.stats[stat::str] / 3; });
randeff dec_dex("Clumsiness", "Decreases your Dexterity by 25%.", "You feel slower!", [] (data &d) { m.next.stats[stat::dex] -= m.current.stats[stat::dex] / 3; });
randeff dec_con("Frailness", "Decreases your Toughness by 25%.", "You feel frail!", [] (data &d) { m.next.stats[stat::con] -= m.current.stats[stat::con] / 3; });
randeff dec_wis("Stupidity", "Decreases your Wisdom by 25%.", "You feel stupid!", [] (data &d) { m.next.stats[stat::wis] -= m.current.stats[stat::wis] / 3; });
randeff hallux("Hallucination", "Makes you unable to recognize monsters.", "Oh wow, everything looks so cosmic!", [] (data &d) { m.next.hallucinating = true; });
randeff confux("Confusion", "Makes you unable to tell left and right.", "You feel confused!", [] (data &d) { 
  auto& ml = find_power("move left");
  auto& mr = find_power("move right");
  if(d.mode == rev::stop) {
    if(d.re->qty & 1) swap(ml.key, mr.key);
    return;
    }
  if(m.vel == m.zero_vel) { 
    if(!keyheld(ml.key) && !keyheld(mr.key)) {
      d.re->qty++;
      swap(ml.key, mr.key);
      }
    }
  });

// jump powers
randeff jump_double("Double Jump", "Lets you jump while already in air.", "You feel a strange force in your legs!", [] (data &d) { });
randeff jump_high("High Jump", "Lets you jump higher.", "Your legs feel stronger!", [] (data &d) { });
randeff jump_bubble("Bubble", "Lets you create bubbles to reach higher places.", "You feel strange bubbles growing in your mouth!", [] (data &d) { });
randeff jump_light("Lightness", "Causes you to be less affected by gravity.", "You feel lighter!", [] (data &d) { });

// trap powers
randeff trap_detect("Detect traps", "Lets you see traps and secret passages in a circle around you.", "You see things you could not see before!", [] (data &d) { });
randeff trap_snake("Snake Hair", "Lets you create snakes that can be used to disarm traps and secret passages.", "You grow snakes on your head!", [] (data &d) { });
randeff trap_disarm("Disarm traps", "Lets you see all traps on the level for a short time, and to attack them with your [weapon] to destroy them.", "You suddenly feel able to disarm traps with your [weapon]!", [] (data &d) { });

// health powers
randeff health_heal("Healing", "Instantly heals you.", "You feel healthier!", [] (data &d) {
  if(d.mode == rev::start) {
    m.hp += 3 * m.current.stats[stat::wis];
    m.hp += (m.max_hp() - m.hp) * m.current.stats[stat::wis] * 2 / 100;
    if(m.hp > m.max_hp()) m.hp = m.max_hp();
    }
  });
randeff health_regen("Regeneration", "Heals you over a period of time.", "You feel slowly healing!", [] (data &d) {
  auto& re = *d.re;
  auto& regen_qty = re.a;
  auto& regen_period = re.b;
  auto& regen_start = re.c;
  if(d.mode == rev::start) {
    regen_qty = 4 * m.current.stats[stat::wis] + m.max_hp() * m.current.stats[stat::wis] * 3 / 100;
    regen_period = game_fps;
    regen_start = gframeid;
    }
  if(gframeid >= regen_start && gframeid < regen_start + regen_period) {
    int t = gframeid - regen_start;
    int sco = (t+1) * regen_qty / regen_period - t * regen_qty / regen_period;
    m.hp = min(m.hp + sco, m.max_hp());
    }
  });
randeff health_protect("Protection", "Makes you more resistant to damage.", "You feel protected!", [] (data &d) {
  // m.protection += 3 * m.current.stats[stat::wis] + m.hp * m.current.stats[stat::wis] * 2 / 100;
  });
randeff health_vampire("Vampirism", "Attacks with your [weapon] restore your health.", "Your [weapon] wants blood!", [] (data &d) {
  // m.vampire += 4 * m.current.stats[stat::wis] + m.max_hp() * m.current.stats[stat::wis] * 3 / 100;
  });
randeff health_bubbles("Bubbles", "When you are attacked, you produce red bubbles that you can collect to heal yourself back.", "You feel something bouncy growing inside you!", [] (data &d) {
  // m.healbubbles += 4 * m.current.stats[stat::wis] + m.max_hp() * m.current.stats[stat::wis] * 3 / 100;
  });

// fire powers
randeff fire_spit("Fiery Spit", "Lets you spit fire.", "You feel fire in your mouth!", [] (data &d) { });
randeff fire_weapon("Fiery Weapon", "Attacks with your [weapon] set things on fire.", "Your hands glow, and your [weapon] burst into flame!", [] (data &d) { });

// morph powers
randeff morph_cat("Cat", "Turns you into a cat.", "You turn into a cat!", [] (data &d) { });
randeff morph_capy("Capybara", "Turns you into a capybara.", "You turn into a capybara!", [] (data &d) { });

vector<power*> all_weapons() {
  vector<power*> res;
  for(auto& w: powers) if(w.qty_filled && (w.flags & WEAPON))
    res.push_back(&w);
  if(res.empty()) res = { &find_power("dagger") };
  return res;
  }

void assign_potion_powers() {
  vector<randeff*> random_powers = { &inc_str, &inc_dex, &inc_con, &inc_wis, &dec_str, &dec_dex, &dec_con, &dec_wis, &hallux, &confux };
  hrandom_shuffle(random_powers);
  auto wpn = all_weapons();
  hrandom_shuffle(wpn);
  int idx = 0; while(isize(wpn) < 10) wpn.push_back(wpn[idx++]);

  fire_weapon.which_weapon = wpn[0];
  trap_disarm.which_weapon = wpn[1];
  using relist = vector<randeff*>;
  find_power("health").randeffs = relist{ pick(&health_heal, &health_regen, &health_protect), random_powers[0] };
  find_power("the thief").randeffs = relist{ pick(&trap_detect, &trap_snake, &trap_disarm), random_powers[1] };
  find_power("polymorph").randeffs = relist{ pick(&morph_cat, &morph_capy), random_powers[2] };
  find_power("reach").randeffs = relist{ pick(&jump_double, &jump_high, &jump_bubble, &jump_light), random_powers[3] };
  find_power("fire").randeffs = relist{ pick(&fire_spit, &fire_weapon), random_powers[4] };
  find_power("mystery").randeffs = relist{ random_powers[5], random_powers[6], random_powers[7] };
  }

}
