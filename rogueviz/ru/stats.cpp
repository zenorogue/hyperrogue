namespace rogue_unlike {

struct statinfo {
  char key;
  string name;
  string desc;
  };

statarray<statinfo> statdata;

void draw_stats() {

  statdata[stat::str] = {'s', "Strength", "Affects the strength of your physical attacks."};
  statdata[stat::con] = {'t', "Toughness", "Affects the amount of hitpoints you have."};
  statdata[stat::wis] = {'w', "Wisdom", "Affects the power of your alchemy."};
  statdata[stat::dex] = {'d', "Dexterity", "Improves your 'chill time' power."};

  render_the_map();
  draw_inventory_frame();
  dialog::init("the Alchemist", 0xC000C0);

  dialog::addSelItem("free experience", its(m.experience), 'x');
  
  for(auto st: allstats) {
    auto bas = m.base_stats[st];
    auto cur = m.current.stats[st];
    string s = its(bas);
    int curi = cur + 0.01;
    if(cur != bas && abs(cur-curi) < 1e-6) s += " (" + its(curi) + ")";
    else if(cur != bas) s += " (" + format("%.2f", cur) + ")";
    dialog::addSelItem(statdata[st].name, s, statdata[st].key);
    dialog::add_action_push([st] {
      render_the_map();
      draw_inventory_frame();
      dialog::init(statdata[st].name, 0xC000C0);
      dialog::addHelp(statdata[st].desc);
      dialog::addBreak(100);
      dialog::addSelItem("base value", its(m.base_stats[st]), 0);
      dialog::addSelItem("current value", its(m.current.stats[st]), 0);
      dialog::addBreak(100);
      dialog::addBack();
      dialog::display();
      });
    }

  dialog::addBack();
  dialog::display();
  }

}
