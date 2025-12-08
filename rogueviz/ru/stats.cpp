namespace rogue_unlike {

struct backstory_element {
  string text;
  reaction_t effect;
  };

struct backstory_choice {
  string part;
  vector<backstory_element> elts;
  };

stat mainstat;

extern vector<backstory_choice> backstories;

int statseed;
vector<int> story_choices;

// -1 to downgrade
int stat_upgrade_cost(stat s, int bonus = 0) {
  return 10 * (m.base_stats[s] + bonus);
  }

void randomize_stories() {
  story_choices = {};
  for(auto& v: backstories)
    story_choices.push_back(hrand(isize(v.elts)));
  }

void upgrade(stat s) {
  auto uc = stat_upgrade_cost(s);
  if(uc <= m.experience) {
    m.experience -= uc;
    m.base_stats[s]++;
    if(m.base_stats[s] > m.base_stats[mainstat]) mainstat = s;
    }
  }

void randomize_stats() {
  for(auto s: allstats) while(m.base_stats[s] > 1) {
    m.base_stats[s]--; m.experience += stat_upgrade_cost(s);
    }

  std::mt19937 statgen;
  statgen.seed(statseed);
  swap(statgen, hrngen);

  mainstat = stat::str;

  m.backstory = "";
  for(int i=0; i<isize(backstories); i++) {
    auto& ch = backstories[i].elts[story_choices[i]];
    m.backstory += ch.text;
    ch.effect();
    }

  for(int i=0; i<500; i++) {
    auto s = hrand_elt(allstats);
    upgrade(s);
    }
  m.profession = mainstat;
  for(auto s: allstats)
    m.current.stats[s] = m.base_stats[s];

  swap(statgen, hrngen);
  }

void generate_character() {
  statseed = time(NULL);
  randomize_stories();
  randomize_stats();
  }

statarray<statinfo> statinfos;

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

void init_stats() {
  statinfos[stat::str] = {'s', "Strength", "Affects the strength of your physical attacks."};
  statinfos[stat::con] = {'t', "Toughness", "Affects the amount of hitpoints you have."};
  statinfos[stat::wis] = {'w', "Wisdom", "Affects the power of your alchemy."};
  statinfos[stat::dex] = {'d', "Dexterity", "Improves your 'chill time' power."};

  profdata[stat::str] = {'w', "Warrior", "Warriors start with a powerful axe."};
  profdata[stat::con] = {'m', "Morpher", "Morpher start with an ability to transform into small animals."};
  profdata[stat::wis] = {'s', "Sorcerer", "Sorcerers start with an ability to cast fire spells."};
  profdata[stat::dex] = {'r', "Rogue", "Rogues start with an ability to detect hidden passages and traps."};
  }

void stat_screen(bool editable) {

  render_the_map();
  draw_inventory_frame();
  dialog::init("the Alchemist", 0xC000C0);

  dialog::addHelp(m.backstory);

  if(editable) {
    dialog::addItem("customize the character", 'e');
    dialog::add_action_push([] {
      dialog::init("the Alchemist backstory", 0xC000C0);
      dialog::addItem("randomize all", 'r');
      dialog::add_action([] { randomize_stories(); randomize_stats(); popScreen(); });
      char key = 'a';
      for(int i=0; i<isize(backstories); i++) if(backstories[i].part != "") {
        dialog::addItem(backstories[i].part, key++);
        dialog::add_action([i] {
          story_choices[i] = gmod(story_choices[i] + 1, isize(backstories[i].elts));
          randomize_stats(); popScreen();
          });
        }
      dialog::addBreak(100);
      dialog::addBack();
      dialog::display();
      });
    }

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
    dialog::addSelItem(statinfos[st].name, s, statinfos[st].key);
    dialog::add_action_push([st, editable] {
      render_the_map();
      draw_inventory_frame();
      dialog::init(statinfos[st].name, 0xC000C0);
      dialog::addHelp(statinfos[st].desc);
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
      m.current.reset(); m.next.reset();
      m.hp = m.max_hp();
      switch(m.profession) {
        case stat::str:
          find_power("axe").gain(1, 1);
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
    dialog::add_action([] { statseed++; randomize_stats(); });
    }

  dialog::display();
  }

void initial_stats() { stat_screen(true); }

void draw_stats() { stat_screen(false); }

// -- backstories follow --

vector<backstory_choice> backstories = {

  backstory_choice{ "family",
    {
      {"You are the only child from a family of workers. ", [] { upgrade(stat::str); upgrade(stat::str); }},

      {"You come from a family with long traditions in alchemy. ", [] { upgrade(stat::wis); upgrade(stat::wis); }}
      }
   },

  backstory_choice{ "hair",
    {
      {"You have blonde hair ", [] { m.hair = hc_blonde; }},

      {"You have red hair ", [] { m.hair = hc_red; }},

      {"You have dark hair ", [] { m.hair = hc_brownhair; }},
      }
   },

  backstory_choice{ "eyes",
    {
      {"and green eyes. ", [] { m.eye = ec_green; }},

      {"and blue eyes. ", [] { m.eye = ec_blue; }},

      {"and brown eyes. ", [] { m.eye = ec_browneye; }},
      }
   },

  backstory_choice{ "childhood stories",
    {
      {"As a child, you loved to listen to your grandfather's stories about how he battled demons to find a cure for your mother's illness when she was a child.\n\n",
      [] { upgrade(stat::wis); upgrade(stat::wis); }},

      {"As a child, you loved to listen to your grandfather's stories about how he found your grandmother, a princess from another world.\n\n",
      [] { upgrade(stat::str); upgrade(stat::str); }},

      {"As a child, you loved to listen to your grandmother's stories. The coolest one was about how she almost died from a mad magician's scythe attack.\n\n",
      [] { upgrade(stat::con); upgrade(stat::con); }}

      }
    },

  backstory_choice{ "motivation for studying alchemy",
    {
      {"You chose to study alchemy, believing it may help to cure the sick. ",
      [] { upgrade(stat::con); upgrade(stat::con); }},

      {"You chose to study alchemy, believing that the more people know, the better for them. ",
      [] { upgrade(stat::wis); upgrade(stat::wis); }},

      {"You chose to study alchemy. Good alchemists earn lots of money, after all. ",
      [] { upgrade(stat::dex); upgrade(stat::dex); }},

      {"You chose to study alchemy. Some potions can make you stronger. ",
      [] { upgrade(stat::str); upgrade(stat::str); }},
      }
    },

  backstory_choice{ "motivation for Second Life",
    {
      {"During your studies, you started to question the inevitability of death. You started to work on a potion of second life.\n\n",
      [] { upgrade(stat::con); upgrade(stat::con); }},

      {"But you were bored by your fellow alchemists creating mundane stuff, such as potions of love and dyes. You realized that it should be possible to create a potion of second life.\n\n",
      [] { upgrade(stat::wis); upgrade(stat::wis); }},

      {"You realized that your potions are easier to sell when more people want them, and started researching something that everyone would want: a potion of second life.\n\n",
      [] { upgrade(stat::dex); upgrade(stat::dex); }}
      }
    },

  backstory_choice{ "motivation for adventuring",
    {
      {"But some ingredients could be found only in the magic fountains in Dungeons of Rogorama. The dungeons are full of dangerous creatures that you are eager to fight. ",
      [] { upgrade(stat::str); }},

      {"But some ingredients could be found only in the magic fountains in the Dungeons of Rogorama. The dungeons are full of legendary artifacts that you are eager to study. ",
      [] { upgrade(stat::wis); }}
      }
    },

  backstory_choice{ "",
    {
      {"While almost nobody has ever returned from these dungeons, the fountains are rumored to be easy to find, so you should be able to create a potion of second life quite easily. "
       "And then, nothing would be able to stop you!\n\nYou wake up in the last inn before entering the dungeons...",
      [] { }}
      }
    }

  };

}
