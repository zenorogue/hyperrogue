namespace rogue_unlike {

// -1 to downgrade
int stat_upgrade_cost(stat s, int bonus = 0) {
  return 10 * (m.base_stats[s] + bonus);
  }

void randomize_stats() {
  for(auto s: allstats) while(m.base_stats[s] > 1) {
    m.base_stats[s]--; m.experience += stat_upgrade_cost(s);
    }
  auto mainstat = stat::str;
  for(int i=0; i<500; i++) {
    auto s = hrand_elt(allstats);
    auto uc = stat_upgrade_cost(s);
    if(uc <= m.experience) {
      m.experience -= uc;
      m.base_stats[s]++;
      if(m.base_stats[s] > m.base_stats[mainstat]) mainstat = s;
      }
    }
  m.profession = mainstat;
  for(auto s: allstats)
    m.current.stats[s] = m.base_stats[s];
  }

struct statinfo {
  char key;
  string name;
  string desc;
  };

statarray<statinfo> statdata;

string prettystat(ld cur) {
  int curi = cur + 0.01;
  if(abs(cur-curi) < 1e-6) return its(curi);
  if(abs(cur-curi-.5) < 1e-6) return its(curi) + "½";
  return format("%.lf", cur);
  }

struct profinfo {
  char key;
  string name;
  string desc;
  };

statarray<statinfo> profdata;

vector<string> professions = { "Warrior", "Morpher", "Sorcerer", "Rogue" };

void stat_screen(bool editable) {

  statdata[stat::str] = {'s', "Strength", "Affects the strength of your physical attacks."};
  statdata[stat::con] = {'t', "Toughness", "Affects the amount of hitpoints you have."};
  statdata[stat::wis] = {'w', "Wisdom", "Affects the power of your alchemy."};
  statdata[stat::dex] = {'d', "Dexterity", "Improves your 'chill time' power."};

  profdata[stat::str] = {'w', "Warrior", "Warriors start with a powerful axe."};
  profdata[stat::con] = {'m', "Morpher", "Morpher start with an ability to transform into small animals."};
  profdata[stat::wis] = {'s', "Sorcerer", "Sorcerers start with an ability to cast fire spells."};
  profdata[stat::dex] = {'r', "Rogue", "Rogues start with an ability to detect hidden passages and traps."};

  render_the_map();
  draw_inventory_frame();
  dialog::init("the Alchemist", 0xC000C0);

  dialog::addSelItem("class", profdata[m.profession].name, 'c');
  dialog::add_action_push([editable] {
    dialog::init("the Alchemist: class", 0xC000C0);
    dialog::addHelp(
      "During their apprenticeship, Alchemists focus on one of multiple specializations.\n\n"
      "Game-wise, this affects the abilities you start with, which also affects what things you can do close to the start of the game. "
      "You should be able to find the basic abilities of other classes early. However, you will also find the basic ability of your "
      "own class, making it more powerful, so it will always be the strongest."
      );
    dialog::addBreak(100);
    for(auto st: allstats) {
      if(!editable && st != m.profession) continue;
      dialog::addBoolItem(profdata[st].name, st == m.profession, profdata[st].key);
      dialog::add_action([st] { m.profession = st; });
      dialog::addBreak(50);
      dialog::addHelp(profdata[st].desc);
      }
    dialog::addBack();
    dialog::display();
    });

  dialog::addSelItem("free experience", its(m.experience), 'x');
  
  for(auto st: allstats) {
    auto bas = m.base_stats[st];
    auto cur = m.current.stats[st];
    string s = its(bas);
    if(cur != bas) s += " (" + prettystat(cur) + ")";
    dialog::addSelItem(statdata[st].name, s, statdata[st].key);
    dialog::add_action_push([st, editable] {
      render_the_map();
      draw_inventory_frame();
      dialog::init(statdata[st].name, 0xC000C0);
      dialog::addHelp(statdata[st].desc);
      dialog::addBreak(100);
      dialog::addSelItem("base value", its(m.base_stats[st]), 0);
      dialog::addSelItem("current value", prettystat(m.current.stats[st]), 0);
      dialog::addBreak(100);
      int uc = stat_upgrade_cost(st);
      dialog::addSelItem("upgrade", its(uc) + " XP", 'u');
      dialog::add_action([uc, st] {
         if(m.experience < uc) { addMessage("Not enough XP!"); return; }
         m.base_stats[st]++; m.current.stats[st]++; m.experience -= uc;
         });
      if(editable) {
        uc = stat_upgrade_cost(st, -1);
        dialog::addSelItem("downgrade", its(uc) + " XP", 'd');
        dialog::add_action([uc, st] {
           if(m.base_stats[st] <= 1) { addMessage("Impossible to go lower!"); return; }
           m.base_stats[st]--; m.current.stats[st]--; m.experience += uc;
           });
        }
      dialog::addBreak(100);
      dialog::addBack();
      dialog::display();
      });
    }

  if(!editable) dialog::addBack();
  if(editable) {
    // need to delete the handler so that we cannot exit this dialog
    keyhandler = [] (int sym, int uni) { dialog::handleNavigation(sym, uni); };
    dialog::addItem("start the game!", 'z');
    dialog::add_action([] {
      popScreen();
      cmode = mode::playing;
      switch(m.profession) {
        case stat::str:
          find_power("dagger").gain(1, 1); // no axe yet
          break;
        case stat::con:
          find_power("polymorph").gain(1, 1).flags |= IDENTIFIED | PARTIAL;
          break;
        case stat::dex:
          find_power("the thief").gain(1, 1).flags |= IDENTIFIED | PARTIAL;
          break;
        case stat::wis:
          find_power("fire").gain(1, 1).flags |= IDENTIFIED | PARTIAL;
          break;
        }
      });

    dialog::addItem("reroll", 'r');
    dialog::add_action(randomize_stats);
    }

  dialog::display();
  }

void initial_stats() { stat_screen(true); }

void draw_stats() { stat_screen(false); }

}
