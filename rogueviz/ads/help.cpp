// Relative Hell: help
// Copyright (C) 2022-2025 Zeno Rogue, see '../../hyper.cpp' for details

namespace hr {
namespace ads_game {

string rsrc_name[rtGUARD] = {"", "Hull", "Ammo", "Fuel", "Air", "Platinum", "Plasteel", "Uranium" };

string get_resource_name(eResourceType id) {
  if(id == rtGoldRocks && main_rock) {
    return "main star history";
    }
  else return rsrc_name[id];
  }

string get_resource_help(eResourceType id, bool with_title) {
  string s = with_title ? helptitle(get_resource_name(id), rsrc_color[id] >> 8) : "";
  switch(id) {
    case rtHull:
      return s + "Your hull is damaged when you are hit by something. You lose immediately when the Hull reaches 0.";
    case rtAmmo:
      return s + "You use up a single unit of ammo every time you shoot.";
    case rtFuel:
      return s + "You use up fuel when you accelerate.";
    case rtOxygen:
      return s + "You use up oxygen constantly, and lose immediately when it reaches 0. Oxygen is lost according to your proper time, "
        "so you might conserve some oxygen by changing your speed, but, would that be ever worth it?";
    case rtGoldRocks:
      if(main_rock) return s + "Your goal is to always remain close to the home star, to see how it evolves. This is your current score.";
      return s + "Platinum is a precious resource mined from asteroids. Mine it to increase your score.";
    case rtGoldGate:
      return s + "Plasteel is a rare resource used in gates. Collect it to increase your score.";
    case rtGoldTurret:
      return s + "Uranium is a rare material found in war zones. Collect it to increase your score.";
    default:
      return s;
    }
  }

string get_land_help(cell *c) {
  string s = helptitle(land_name(c), help_color(c));
  switch(c->land) {
    case laJungle:
      s += 
        "For an unknown purpose, an ancient civilization has filled this space with gates. These gates periodically shut down, potentially damaging the ships which explore this zone. "
        "The whole gate shuts down at exactly the same time... but, of course, according to the gate's clock, not your ship's clock.";
      break;
    case laCaves:
      s += "An asteroid field, filled with some stationary rocks, as well as dangerous but beautiful asteroids, containing precious uranium.";
      break;
    case laHunting:
      s += "A dangerous zone filled with enemy turrets.";
      break;
    case laCrossroads:
      s += "A mostly empty zone that leads to other zones.";
      break;
    case laBarrier:
      s += "Just a great ancient wall separating different zones.";
      break;
    default:
      ;
    }
  return s;
  }

void explain_undermouse() {
  for(auto m: under_mouse) switch(m->type) {
    case oRock: {
      mouseovers += ", asteroid";
      if(!m->resource) {
        help = "Asteroids just fly freely, they will damage your ship if they hit you. You can shoot them to detroy them; sometimes they contain resources, but this particular one does not.";
        break;
        }
      help = "Asteroids just fly freely, they will damage your ship if they hit you. You can shoot them to detroy them and release resources. This one contains the resource " + get_resource_name(m->resource) + ":\n\n" + get_resource_help(m->resource, false);
      mouseovers += " (" + rsrc_name[m->resource] + ")";
      break;
      }
    case oResource: {
      help = get_resource_help(m->resource, true);
      if(m->resource) mouseovers += ", " + rsrc_name[m->resource];
      break;
      }
    case oTurret: {
      help = "An enemy turret. They shoot at you with perfect accuracy... but of course, due to the limited speed of light, they see where you were in the past, and hopefully you changed your route since then.";
      mouseovers += ", turret";
      break;
      }
    case oTurretMissile: {
      help = "A missile shot at you by an enemy turret.";
      mouseovers += ", enemy missile";
      break;
      }
    case oMissile: {
      help = "A missile shot by you. Destroys asteroids and enemy turrets.";
      mouseovers += ", missile";
      break;
      }
    case oMainRock: {
      help = "Your goal is to always remain close to the home star.";
      mouseovers += ", home star";
      break;
      }
    default: ;
    }
  }

int generate_mouseovers() {
  titlecolor = 0xFFFFFF;

  if(!paused) {
    int v = -1;
    for(int i=0; i<multi::SCANCODES; i++) if(multi::scfg_default.keyaction[i] == 16+pcPause) v = i;
    if(v >= 0) mouseovers = "press " + SDL12(dialog::keyname(v), string(SDL_GetScancodeName(SDL_Scancode(v)))) + " to pause";
    return 1;
    }

  if(outofmap(mouseh.h)) return 2;

  hyperpoint h1 = normalize(unshift(mouseh));
  hybrid::in_actual([&] {
    if(main_rock) return;
    h1[3] = h1[2]; h1[2] = 0;
    ads_point rel = ads_inverse(current * vctrV) * ads_point(h1, 0);
    cell *c = hybrid::get_at(vctr, 0);
    virtualRebase(c, rel.h);
    optimize_shift(rel);
    auto w = hybrid::get_where(c);
    auto& ci = ci_at[w.first];
    mouseovers = land_name(w.first);
    titlecolor = help_color(w.first);
    help = get_land_help(w.first);

    switch(ci.type) {
      case wtDestructible: case wtSolid:
        mouseovers += ", rock";
        break;
      case wtGate:
        mouseovers += ", gate";
        break;
      default: ;
      }
    });

  explain_undermouse();

  return 3;
  }

string get_main_help() {
  if(main_rock)
    return
      "Keep close to the yellow star! The space is expanding, so if you go too far away from it, you will never be able to get back.\n\n"
      "You have to avoid (or shoot) the white asteroids which are continuously thrown at you. In longer runs, you also need to catch items to replenish your resources.\n\n"
      "Good luck!\n\nSee the Guided Tour for more explanation about how de Sitter spacetime used in this game works.";
  else return
      "Explore the world, shoot asteroids and enemies, collect treasures and resources. The more treasures you collect, the harder the game becomes. Good luck!\n\n"
      "Right-click objects while paused for more information about them.\n\n"
      "See the Guided Tour for more explanation about how anti-de Sitter spacetime used in this game works.";
  }

}
}
