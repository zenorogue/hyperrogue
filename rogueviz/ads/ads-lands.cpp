namespace hr {

namespace ads_game {

// Crossroads: empty

// Caves: walls and asteroids

// Hunting: turrets

eLand ads_nextland(eLand old) {
  vector<eLand> lands = { laCrossroads, laCaves, laHunting, laJungle };
  while(true) {
    eLand res = hrand_elt(lands);
    if(res != old) return res;
    }
  }

color_t empty_color(cell *c) {
  switch(c->land) {
    case laCrossroads:
      return 0x101030FF;
    case laCaves:
      return 0x303010FF;
    case laHunting:
      return 0x301010FF;
    case laJungle:
      return 0x103010FF;
    default:
      return 0x181818FF;
    }
  }

color_t empty_outline(cell *c) {
  switch(c->land) {
    case laCrossroads:
      return 0x8080C0FF;
    case laCaves:
      return 0xC0C080FF;
    case laHunting:
      return 0xC08080FF;
    case laJungle:
      return 0x80C080FF;
    default:
      return 0x181818FF;
    }
  }

color_t help_color(cell *c) {
  switch(c->land) {
    case laBarrier:
      return 0xFFFFFF;
    default:
      return empty_outline(c) >> 8;
    }
  }

string land_name(cell *c) {
  switch(c->land) {
    case laCrossroads:
      return "Space Roads";
    case laCaves:
      return "Asteroid Field";
    case laHunting:
      return "War Zone";
    case laJungle:
      return "Gate Zone";
    default:
      return dnameof(c->land);
    }
  }

int wall_frequency(cell *c) {
  switch(c->land) {
    case laCrossroads:
      return 0;
    case laCaves:
      return 6;
    case laHunting:
      return 0;
    default:
      return 0;
    }
  }

ld turret_frequency(cell *c) {
  switch(c->land) {
    case laHunting:
      forCellEx(c1, c) forCellEx(c2, c) if(c2->land != laHunting) return 0;
      return 0.02;
    default:
      return 0;
    }
  }

ld rock_frequency(cell *c) {
  switch(c->land) {
    case laCaves:
      return 1;
    default:
      return 0;
    }
  }

int gate_frequency(cell *c) {
  switch(c->land) {
    case laJungle:
      return 50;
    default:
      return 0;
    }
  }

int treasure_id(eResourceType r) {
  switch(r) {
    case rtGoldRocks: return 0;
    case rtGoldGate: return 1;
    case rtGoldTurret: return 2;
    default: return -1;
    }
  }

eResourceType treasure_of(cell *c) {
  switch(c->land) {
    case laCaves:
      return rtGoldRocks;
    case laHunting:
      return rtGoldTurret;
    case laJungle:
      return rtGoldGate;
    default:
      return rtNone;
    }
  }

}
}
