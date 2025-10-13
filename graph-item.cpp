// Hyperbolic Rogue -- item graphics file
// Copyright (C) 2011-2025 Zeno Rogue, see 'hyper.cpp' for details

#include "hyper.h"
namespace hr {

EX shiftmatrix face_the_player(const shiftmatrix V) {
  if(GDIM == 2) return V;
  if(mproduct) return bobbing ? orthogonal_move(V, cos(ptick(750)) * cgi.plevel / 16) : V;
  if(mhybrid) return bobbing ? V * zpush(cos(ptick(750)) * cgi.plevel / 16) : V;
  transmatrix dummy; /* used only in prod anyways */
  if(embedded_plane && !cgi.emb->is_same_in_same()) return V;
  if(nonisotropic) return shiftless(spin_towards(unshift(V), dummy, C0, 2, 0));
  #if CAP_VR
  if(vrhr::enabled) {
    shiftpoint h = tC0(V);
    hyperpoint uh = unshift(h);
    return shiftless(cspin90(1, 2) * rspintox(cspin90(2, 1) * uh) * xpush(hdist0(uh)) * cspin90(0, 2) * spin270());
    }
  #endif

  return rgpushxto0(tC0(V));
  }

EX hpcshape& orbshape(eOrbshape s) {
  if(vid.orbmode == 0) return cgi.shRing;
  switch(s) {
     case osLove: return cgi.shLoveRing;
     case osRanged: return cgi.shTargetRing;
     case osOffensive: return cgi.shSawRing;
     case osDirectional: return vid.orbmode == 2 ? cgi.shSawRing : cgi.shSpearRing;
     case osFriend: return cgi.shPeaceRing;
     case osUtility: return cgi.shGearRing;
     case osPowerUtility: return cgi.shPowerGearRing;
     case osWarping: return cgi.shHeptaRing;
     case osFrog: return cgi.shFrogRing;
     case osProtective: return cgi.shProtectiveRing;
     case osTerraform: return cgi.shTerraRing;
     case osMovement: return cgi.shMoveRing;
     default: return cgi.shRing;
     }
  }

EX void queue_ring(const shiftmatrix& V, hpcshape& sh, color_t col, PPR p) {
  queuepolyat(V, sh, col, p).outline = 0;

  auto& p1 = queuepolyat(V, sh, col, p);
  p1.cnt = cgi.orb_inner_ring;
  p1.color = 0;

  auto& p2 = queuepolyat(V, sh, col, p);
  p2.color = 0;
  p2.offset += cgi.orb_inner_ring;
  p2.cnt -= cgi.orb_inner_ring + 1;
  }

EX color_t orb_auxiliary_color(eItem it) {
  if(it == itOrbFire) return firecolor(200);
  if(it == itOrbWater) return 0x000060;
  if(it == itOrbFriend || it == itOrbDiscord) return 0xC0C0C0;
  if(it == itOrbFrog) return 0xFF0000;
  if(it == itOrbImpact) return 0xFF0000;
  if(it == itOrbPhasing) return 0xFF0000;
  if(it == itOrbDash) return 0xFF0000;
  if(it == itOrbFreedom) return 0xC0FF00;
  if(it == itOrbPlague) return 0x409040;
  if(it == itOrbChaos) return 0xFF00FF;
  if(it == itOrbAir) return 0xFFFFFF;
  if(it == itOrbUndeath) return minf[moFriendlyGhost].color;
  if(it == itOrbRecall) return 0x101010;
  if(it == itOrbLife) return 0x90B090;
  if(it == itOrbSlaying) return 0xFF0000;
  if(it == itOrbSide1) return 0x307080;
  if(it == itOrbDigging) return 0x606060;
  if(it == itOrbEnergy) return 0xFFFF80;
  return iinf[it].color;
  }

EX color_t orb_inner_color(eItem it) {
  if(it == itOrbWater) return 0x0070C0;
  if(it == itOrbEnergy) return 0x8B4513;
  // if(it == itOrbDash) return 0xFF0000;
  if(it == itOrbSide1) return 0x00FF00;
  // if(it == itOrbPhasing) return 0xFF0000;
  if(it == itOrbDigging) return 0x00FF00;
  if(it == itOrbLife) return 0x306000;
  return iinf[it].color;
  }

EX bool drawItemType(eItem it, cell *c, const shiftmatrix& V, color_t icol, int pticks, bool hidden) {
  if(!it) return false;
  char xch = iinf[it].glyph;

#if MAXMDIM >= 4
  if(c && GDIM == 3)
    addradar(V, xch, icol, kind_outline(it));
#endif

  if(WDIM == 3 && c == centerover && in_perspective() && hdist0(tC0(V)) < cgi.orbsize * 0.25) return false;

  if(!mmitem || !CAP_SHAPES) {
    draw_ascii_or_zh(V, iinf[it].glyph, iinf[it].name, icol, 1, 0.5);
    return true;
    }  
    
#if CAP_SHAPES
  auto sinptick = [c, pticks] (int period) { return c ? sintick(period) : sin(animation_factor * vid.ispeed * pticks / period);};
  auto spinptick = [c, pticks] (int period, ld phase) { return c ? spintick(period, phase) : spin((animation_factor * vid.ispeed * pticks) / period + phase * TAU); };
  int ct6 = c ? ctof(c) : 1;
  hpcshape *xsh = 
    (it == itPirate || it == itKraken) ? &cgi.shPirateX :
    (it == itBuggy || it == itBuggy2) ? &cgi.shPirateX :
    it == itHolyGrail ? &cgi.shGrail :
    isElementalShard(it) ? &cgi.shElementalShard :
    (it == itBombEgg || it == itTrollEgg || it == itCursed) ? &cgi.shEgg :
    (it == itFrog || it == itWhirlpool) ? &cgi.shDisk :
    it == itHunting ? &cgi.shTriangle :
    (it == itDodeca || it == itDice) ? &cgi.shDodeca :
    xch == '*' ? &cgi.shGem[ct6] : 
    xch == '(' ? &cgi.shKnife : 
    it == itShard ? &cgi.shMFloor.b[0] :
    it == itTreat ? &cgi.shTreat :
    it == itSlime ? &cgi.shEgg :
    xch == '%' ? &cgi.shDaisy : xch == '$' ? &cgi.shStar : xch == ';' ? &cgi.shTriangle :
    xch == '!' ? &cgi.shTriangle : it == itBone ? &cgi.shNecro : it == itStatue ? &cgi.shStatue :
    among(it, itIvory, itEclectic) ? &cgi.shFigurine : 
    xch == '?' ? &cgi.shBookCover : 
    it == itKey ? &cgi.shKey : 
    it == itRevolver ? &cgi.shGun :
    NULL;
   
  if(c && doHighlight()) 
    poly_outline = kind_outline(it);

  shiftmatrix Vit = V;
  if(embedded_plane && c && it != itBabyTortoise) Vit = orthogonal_move_fol(V, cgi.STUFF);
  if(c && mproduct)
    Vit = orthogonal_move(Vit, sin(ptick(750)) * cgi.plevel / 4);
  else if(c && sl2 && !embedded_plane)
    Vit = Vit * zpush(sin(ptick(750)) * cgi.plevel / 4);
  else
    if(GDIM == 3 && c && it != itBabyTortoise) Vit = face_the_player(Vit);
  // V * cspin(0, 2, ptick(618, 0));

#if CAP_SHAPES
  if(mapeditor::drawUserShape(Vit, mapeditor::sgItem, it, darkena(icol, 0, 0xFF), c)) return true;
#endif

  if(c && history::includeHistory && history::infindhistory.count(c)) poly_outline = OUTLINE_DEAD;

  else if(it == itSavedPrincess) {
    drawMonsterType(moPrincess, c, V, icol, 0, icol);
    return true;
    }
  
  else if(it == itStrongWind) {
    queuepoly(Vit * spinptick(750, 0), cgi.shFan, darkena(icol, 0, 255));
    }

  else if(it == itFatigue) {
    queuepoly(Vit * spinptick(750, 0), cgi.shFan, darkena(icol, 0, 255));
    }

  else if(it == itWarning) {
    queuepoly(Vit * spinptick(750, 0), cgi.shTriangle, darkena(icol, 0, 255));
    }
    
  else if(it == itCrossbow) {
    queuepoly(Vit, cgi.shCrossbowIcon, getcs().bowcolor);
    queuepoly(Vit, cgi.shCrossbowstringIcon, getcs().bowcolor2);
    }

  else if(it == itBabyTortoise) {
    int bits = c ? tortoise::babymap[c] : tortoise::last;
    int over = c && c->monst == moTortoise;
    tortoise::draw(Vit * spinptick(5000, 0) * ypush(cgi.crossf*.15), bits, over ? 4 : 2, 0);
    // queuepoly(Vit, cgi.shHeptaMarker, darkena(tortoise::getMatchColor(bits), 0, 0xC0));
    }
  
  else if(it == itCompass) {
    shiftmatrix V2;
    #if CAP_CRYSTAL
    if(cryst) {
      if(crystal::compass_probability <= 0) return true;
      if(cwt.at->land == laCamelot && celldistAltRelative(cwt.at) < 0) crystal::used_compass_inside = true;
      V2 = V * spin(crystal::compass_angle() + M_PI);
      }
    else
    #endif
    if(1) {
      shiftpoint P1;
      if(mark_compass(c, P1)) {
        V2 = V * lrspintox(inverse_shift(V, P1));
        }
      else V2 = V;
      }
    if(GDIM == 3) {
      queue_ring(Vit, cgi.shRing, 0xFFFFFFFF, PPR::ITEM);
      if(WDIM == 2) V2 = orthogonal_move_fol(V2, cgi.STUFF);
      V2 = V2 * cspin(1, 2, M_PI * sintick(100) / 39);
      queuepoly(V2, cgi.shCompass3, 0xFF0000FF);
      queuepoly(V2 * lpispin(), cgi.shCompass3, 0x000000FF);
      }
    else {
      if(c) V2 = V2 * spin(M_PI * sintick(100) / 30);
      color_t hider = hidden ? 0xFFFFFF20 : 0xFFFFFFFF;
      queuepoly(V2, cgi.shCompass1, 0xFF8080FF & hider);
      queuepoly(V2, cgi.shCompass2, 0xFFFFFFFF & hider);
      queuepoly(V2, cgi.shCompass3, 0xFF0000FF & hider);
      queuepoly(V2 * lpispin(), cgi.shCompass3, 0x000000FF & hider);
      }
    xsh = NULL;
    }

  else if(it == itPalace) {
    #if MAXMDIM >= 4
    if(GDIM == 3 && WDIM == 2) {
      ld h = cgi.human_height;
      dynamicval<qfloorinfo> qfi2(qfi, qfi);
      shiftmatrix V2 = V * spin(pticks * vid.ispeed / 1500.);
      /* divisors should be higher than in plate renderer */
      qfi.fshape = &cgi.shMFloor2;
      draw_shapevec(c, V2 * lzpush(-h/30), qfi.fshape->levels[SIDE::FLOOR], 0xFFD500FF, PPR::WALL);

      qfi.fshape = &cgi.shMFloor3;
      draw_shapevec(c, V2 * lzpush(-h/25), qfi.fshape->levels[SIDE::FLOOR], darkena(icol, 0, 0xFF), PPR::WALL);

      qfi.fshape = &cgi.shMFloor4;
      draw_shapevec(c, V2 * lzpush(-h/20), qfi.fshape->levels[SIDE::FLOOR], 0xFFD500FF, PPR::WALL);
      }
    else if(WDIM == 3 && c) {
      ld h = cgi.human_height;
      shiftmatrix V2 = Vit * spin(pticks * vid.ispeed / 1500.);
      draw_floorshape(c, V2 * lzpush(h/100), cgi.shMFloor3, 0xFFD500FF);
      draw_floorshape(c, V2 * lzpush(h/50), cgi.shMFloor4, darkena(icol, 0, 0xFF));
      queuepoly(V2, cgi.shGem[ct6], 0xFFD500FF);
      }
    else if(WDIM == 3 && !c) {
      queuepoly(Vit, cgi.shGem[ct6], 0xFFD500FF);
      }
    else 
    #endif
    {
      color_t hider = hidden ? 0xFFFFFF20 : 0xFFFFFFFF;
      shiftmatrix V2 = Vit * spin(pticks * vid.ispeed / 1500.);
      draw_floorshape(c, V2, cgi.shMFloor3, 0xFFD500FF & hider);
      draw_floorshape(c, V2, cgi.shMFloor4, darkena(icol, 0, 0xFF) & hider);
      queuepoly(V2, cgi.shGem[ct6], 0xFFD500FF & hider);
      }
    xsh = NULL;
    }
  
  else if(it == itRose) {
    for(int u=0; u<4; u++)
      queuepoly(Vit * spinptick(1500, 0) * spin(30._deg * u), cgi.shRoseItem, darkena(icol, 0, hidden ? 0x30 : 0xA0));
    }

  else if(it == itBarrow && c) {
    for(int i = 0; i<barrowCount(c); i++)
      queuepolyat(Vit * spin(TAU * i / barrowCount(c)) * xpush(.15 * cgi.scalefactor) * spinptick(1500, 0), *xsh, darkena(icol, 0, hidden ? 0x40 :
        (highwall(c) && wmspatial) ? 0x60 : 0xFF),
        PPR::HIDDEN);
    }
    
  else if(xsh) {
    if(it == itFireShard) icol = firecolor(100);
    if(it == itWaterShard) icol = watercolor(100) >> 8;
    
    if(it == itZebra) icol = 0xFFFFFF;
    if(it == itLotus) icol = 0x101010;
    if(it == itSwitch) icol = minf[active_switch()].color;
    
    shiftmatrix V2 = Vit * spinptick(1500, 0);
  
    if(xsh == &cgi.shBookCover && mmitem) {
      if(GDIM == 3)
        queuepoly(V2 * cpush(2, 1e-3), cgi.shBook, 0x805020FF);
      else
        queuepoly(V2, cgi.shBook, 0x805020FF);
      }
    
    PPR pr = PPR::ITEM;
    int alpha = hidden ? (it == itKraken ? 0xC0 : 0x40) : 0xF0;
    if(c && c->wall == waIcewall) pr = PPR::HIDDEN, alpha = 0x80;

    queuepolyat(V2, *xsh, darkena(icol, 0, alpha), pr);

    if(it == itZebra) {
      shiftmatrix Vx = Vit * spinptick(1500, .5/(ct6+6));
      if(GDIM == 3)
        Vx = Vx * cpush(2, -1e-3);
      queuepolyat(Vx, *xsh, darkena(0x202020, 0, hidden ? 0x40 : 0xF0), PPR::ITEMb);
      }
    }
  
  else if(xch == 'o' || xch == 'c' || it == itInventory) {
    if(it == itOrbFire) icol = firecolor(100);
    PPR prio = PPR::ITEM;
    bool inice = c && c->wall == waIcewall;
    if(inice) prio = PPR::HIDDEN;
    
    color_t icol1 = icol;
    icol = orb_auxiliary_color(it);
    color_t col = darkena(icol, 0, int(0x80 + 0x70 * sinptick(300)));

    if(it == itOrbFish && vid.orbmode == 2)
      queuepolyat(Vit * spinptick(1500, 0), cgi.shFishTail, col, PPR::ITEM_BELOW);
    
    if(xch == 'c')
      queuepolyat(Vit * spinptick(500, 0), cgi.shMoonDisk, darkena(0x801080, 0, hidden ? 0x20 : 0xC0), prio);
    else if(vid.orbmode < 2) {
      icol1 = orb_inner_color(it);
      queuepolyat(Vit, cgi.shDisk, darkena(icol1, 0, inice ? 0x80 : hidden ? 0x20 : 0xC0), prio);
      }
    else {
      icol1 = orb_inner_color(it);      
      auto dark = darkena(icol1, 0, inice ? 0x80 : hidden ? 0x20 : (it == itOrbBeauty) ? 0xA0 : 0xC0);
      auto dark1 = darkena(icol1, 0, inice ? 0x40 : hidden ? 0x10 : (it == itOrbBeauty) ? 0x50 : 0x60);
      if(c && GDIM == 2) Vit = rgpushxto0(tC0(Vit));
      auto Vit1 = Vit * spin90();

      if (it == itOrbBeauty) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        for(int u=0; u<3; u++)
          queuepolyat(Vit1 * spin(40._deg * u), cgi.shSmallRose, dark, prio);
        }
      else if (it == itOrbLife) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit1, cgi.shSmallPBody, dark, prio);
        queuepolyat(Vit1, cgi.shDiskM, dark, prio);
        }
      else if (it == itOrbBull) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit1, cgi.shTinyBullBody, dark, prio);
        queuepolyat(Vit1, cgi.shTinyBullHead, dark, prio);
        queuepolyat(Vit1, cgi.shTinyBullHorn, dark, prio);
        queuepolyat(Vit1 * lmirror(), cgi.shTinyBullHorn, dark, prio);
        }
      else if (it == itOrbFrog && false) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit1, cgi.shSmallFrogBody, dark, prio);
        queuepolyat(Vit1, cgi.shSmallFrogRearFoot, dark, prio);
        queuepolyat(Vit1, cgi.shSmallFrogRearLeg, dark, prio);
        queuepolyat(Vit1, cgi.shSmallFrogRearLeg2, dark, prio);
        queuepolyat(Vit1, cgi.shSmallFrogFrontFoot, dark, prio);
        queuepolyat(Vit1, cgi.shSmallFrogFrontLeg, dark, prio);
        queuepolyat(Vit1*lmirror(), cgi.shSmallFrogRearFoot, dark, prio);
        queuepolyat(Vit1*lmirror(), cgi.shSmallFrogRearLeg, dark, prio);
        queuepolyat(Vit1*lmirror(), cgi.shSmallFrogRearLeg2, dark, prio);
        queuepolyat(Vit1*lmirror(), cgi.shSmallFrogFrontFoot, dark, prio);
        queuepolyat(Vit1*lmirror(), cgi.shSmallFrogFrontLeg, dark, prio);
        }
      else if (it == itOrbSpeed) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        drawSpeed(Vit, 0.3);
        }
      else if (it == itOrbStunning) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit, cgi.shDiskM, dark, prio);
        for (int i=0; i<5; i++) {
          shiftmatrix V2 = Vit * spin(TAU * i / 5 + ptick(300));
          queuepolyat(V2, cgi.shSmallFlailBall, dark, prio);
          }
        }
      else if (it == itOrbDragon) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit1, cgi.shSmallDragonHead, dark, prio);
        queuepolyat(Vit1, cgi.shSmallDragonNostril, 0xFF, prio);
        queuepolyat(Vit1*lmirror(), cgi.shSmallDragonNostril, 0xFF, prio);
        queuepolyat(Vit1, cgi.shSmallDragonEyes, 0x60, prio);
        queuepolyat(Vit1*lmirror(), cgi.shSmallDragonEyes, 0x60, prio);
        }
      else if (it == itOrbDomination) {
        queuepolyat(Vit1*MirrorX, cgi.shSmallWormHead, dark, prio);
        queuepolyat(Vit1*MirrorX, cgi.shSmallWormEyes, 0x60, prio);
        queuepolyat(Vit1*MirrorX*lmirror(), cgi.shSmallWormEyes, 0x60, prio);
        }
      else if (it == itOrbMorph || it == itOrbChaos || it == itOrbPlague) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit1, cgi.shSmallTreat, dark, prio);
        }
      else if (it == itOrbWinter) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit1, cgi.shSnowflake, dark, prio);
        }
      else if (it == itOrbLuck)
        queuepolyat(Vit1, cgi.shSmallerDodeca, dark, prio);
      else if (it == itOrbAether) {
        queuepolyat(Vit1, cgi.shHalfDisk, dark, prio);
        queuepolyat(Vit1*lmirror(), cgi.shHalfDisk, 0xFF, prio);
        queuepolyat(Vit1*MirrorX, cgi.shHalfHumanoid, dark, prio);
        queuepolyat(Vit1*lmirror()*MirrorX, cgi.shHalfHumanoid, 0xFF, prio);
        }
      else if (it == itOrbFlash)
        queuepolyat(Vit1, cgi.shFlash, dark, prio);
      else if (it == itOrbMatter || it == itOrbStone) {
        queuepolyat(Vit, cgi.shDisk, dark1, prio);
        queuepolyat(Vit1, cgi.shDiskSq, dark, prio);
        }
      else if (it == itOrbSummon) {
        queuepolyat(Vit1, cgi.shHeptagon, dark, prio);
        queuepolyat(Vit1, cgi.shHeptagram, dark, prio);
        }
      else if (it == itOrbSafety) {
        queuepolyat(Vit, cgi.shDisk, dark, prio);
        dynamicval<color_t> p(poly_outline, dark);
        queuepolyat(Vit1, cgi.shHeptagram, 0, prio);
        }
      else {
        bool jump = (it == itOrbPhasing || it == itOrbDash || it == itOrbFrog);
        auto shape = (it == itOrbFriend) ? &cgi.shTinyBird :
                     (it == itOrbSide1) ? &cgi.shSmallPSword :
                     (it == itOrbDigging) ? &cgi.shSmallPickAxe :
                     (it == itOrbSword || it == itOrbSword2) ? &cgi.shSmallSword :
                     (it == itOrbThorns) ? &cgi.shSmallHedgehogBlade :
                     (it == itOrbSide2 || it == itOrb37 || it == itOrbLava) ? &cgi.shDiskT :
                     (it == itOrbGravity) ? &cgi.shTinyArrow :
                     (it == itOrbFreedom || it == itOrbRecall) ? &cgi.shDiskSq :
                     (it == itOrbEnergy) ? &cgi.shHalfDisk :
                     (it == itOrbSpace) ? &cgi.shSmallPirateHook :
                     (it == itOrbChoice || it == itOrbMirror || it == itOrbMagnetism || it == itOrbEmpathy || it == itOrbDiscord) ? &cgi.shEccentricDisk :
                     (it == itOrbPsi || it == itOrbSide3) ? &cgi.shDiskS :
                     (it == itOrbPurity) ? &cgi.shSmallEgg :
                     (it == itOrbLightning) ? &cgi.shLightningBolt :
                     (it == itOrbShield) ? &cgi.shShield :
                     (it == itOrbTime) ? &cgi.shHourglass :
                     (it == itOrbAir) ? &cgi.shSmallFan :
                     (it == itOrbWoods) ? &cgi.shTreeIcon :
                     (it == itOrbNature) ? &cgi.shLeafIcon :
                     (it == itOrbIllusion || it == itOrbInvis || it == itOrbTeleport) ? &cgi.shHumanoid :
                     jump ? &cgi.shDiskSegment :
                        NULL;
        queuepolyat(Vit, cgi.shDisk, dark, prio);
        bool reversed = (shape == &cgi.shTreeIcon || shape == &cgi.shHumanoid || it == itOrbSword2);
        bool left90 = (shape == &cgi.shLeafIcon || shape == &cgi.shLightningBolt);
        if (shape)
          queuepolyat(reversed ? Vit1 * MirrorX : left90 ? Vit1 * spin270() : Vit1, *shape, (it == itOrbInvis || it == itOrbTeleport) ? 0x20 : 0x80, prio);
        if (it == itOrbSide1 || (shape == &cgi.shEccentricDisk && it != itOrbDiscord))
          queuepolyat(Vit1*lmirror(), *shape, 0x80, prio);
        if (jump || it == itOrbEnergy)
          queuepolyat(Vit1*lmirror(), *shape, col, prio);
        if (it == itOrbIntensity || it == itOrbImpact)
          queuepolyat(Vit1, cgi.shDiskM, 0x80, prio);
        if (it == itOrbHorns) {
          queuepolyat(Vit1, cgi.shSmallBullHead, 0x80, prio);
          queuepolyat(Vit1, cgi.shSmallBullHorn, 0x80, prio);
          queuepolyat(Vit1*lmirror(), cgi.shSmallBullHorn, 0x80, prio);
          }
        if (it == itOrbUndeath) {
          dark = darkena(fghostcolor(c) /* minf[moFriendlyGhost].color */, 0, inice ? 0x80 : hidden ? 0x20 : 0xC0);
          queuepolyat(Vit1, cgi.shMiniGhost, dark, prio);
          queuepolyat(Vit1, cgi.shMiniEyes, 0xFF, prio);
          }
        if (it == itOrbSlaying) {
          queuepolyat(Vit1, cgi.shSmallFlailTrunk, 0x80, prio);
          queuepolyat(Vit1, cgi.shSmallHammerHead, 0x80, prio);
          }
        if (it == itOrbShell)
          for(int i = 1; i<8; i++) {
            queuepolyat(Vit1, cgi.shTortoise[i][2], 0x80, prio);
            if (i>=5 && i<=7)
              queuepolyat(Vit1*lmirror(), cgi.shTortoise[i][2], 0x80, prio);
            }
        }
      }

    queue_ring(Vit * spinptick(1500, 0), orbshape(iinf[it].orbshape), col, prio);
    }

  else {
    draw_ascii_or_zh(V, xch, iinf[it].name, icol, 1, 0.5);
    }

  return true;
#endif
  }

EX void queue_goal_text(shiftpoint P1, ld sizemul, const string& s, color_t color) {
  #if CAP_VR
  if(vrhr::enabled) {
    auto e = inverse_exp(P1);
    e = e * 3 / hypot_d(GDIM, e);
    auto T = face_the_player(shiftless(rgpushxto0(direct_exp(e))));
    queuestrn(T, sizemul * mapfontscale / 100, s, color);
    return;
    }
  #endif
  queuestr(P1, vid.fsize * sizemul, s, color);
  }

EX bool mark_compass(cell *c, shiftpoint& P1) {
  cell *c1 = c ? findcompass(c) : NULL;
  if(!c1) return false;

  shiftmatrix P = ggmatrix(c1);
  P1 = tC0(P);

  if(isPlayerOn(c)) {
    queue_goal_text(P1, 2, "X", 0x10100 * int(128 + 100 * sintick(150)));
//  queuestr(V, 1, its(compassDist(c)), 0x10101 * int(128 - 100 * sin(ticks / 150.)), 1);
    queue_goal_text(P1, 1, its(-compassDist(c)), 0x10101 * int(128 - 100 * sintick(150)));
    addauraspecial(P1, 0xFF0000, 0);
    addradar(P, 'X', iinf[itCompass].color, 0xFF, true);
    }
  return true;
  }

}
