// HyperRogue French translation        -*-coding: utf-8-dos -*-
// by
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// This translation file is encoded with UTF-8.

// Nouns.

// For each noun in English, provide:

// 1) the type (usually gender) of the noun. For example, in Polish each noun can be:

// masculine living (GEN_M),
// masculine object (GEN_O), //FIXME: do we need this?
// feminine (GEN_F), or
// neuter (GEN_N).

// GENF_ELISION to use "l'" instead of "le" or "la"
// GENF_PLURALONLY in case the word is only usable in plural
// GENF_PROPER if the word is a proper name and can't be used with articles

// 2) all the forms required by HyperRogue. The following forms are given:
// nominative (referred to as %1),
// nominative plural (%P1),

// Feel free to add more or less forms or types if needed. Note that you don't
// have to provide all the forms in your language, only those used by HyperRogue
// (in Polish just 4 of 14 forms are used, and this is probably similar in other
// languages).  The forms being replaced are defined in language.cpp; search for
// "if(l == 7)".

// MONSTERS
// ========

N("Yeti", GEN_M, "Yéti", "Yétis", "Yéti")
N("Icewolf", GEN_M, "Loup des glaces", "Loups des glaces", "Loup des glaces")
N("Ranger",  GEN_M, "Gardien", "Gardiens", "Gardien")
N("Rock Troll", GEN_M, "Troll des cavernes", "Trolls des cavernes", "Troll des cavernes")
N("Goblin", GEN_M, "Gobelin", "Gobelins", "Gobelin")
N("Sand Worm", GEN_M, "Ver des sables", "Vers des sables", "Ver des sables")
N("Sand Worm Tail", GEN_F, "Queue du ver des sables", "Queues des vers des sables", "Queue du ver des sables")
N("Sand Worm W", GEN_M, "Ver des sables W", "Vers des sables W", "Ver des sables W")
N("Desert Man", GEN_M | GENF_ELISION, "Homme du désert", "Hommes du désert", "Homme du désert")
N("Ivy Root", GEN_F, "Racine de lierre", "Racines de lierre", "Racine de lierre")
N("Active Ivy", GEN_M, "Lierre actif", "Lierres actifs", "Lierre actif")
N("Ivy Branch", GEN_F, "Branche de lierre", "Branches de lierre", "Branche de lierre")
N("Dormant Ivy", GEN_M, "Lierre dormant", "Lierres dormants", "Lierre dormant")
N("Ivy N", GEN_M, "Lierre N", "Lierres N", "Lierre N")
N("Ivy D", GEN_M, "Lierre D", "Lierres D", "Lierre D")
N("Giant Ape", GEN_M, "Singe géant", "Singes géants", "Singe géant")
N("Slime Beast", GEN_F, "Bête gélatineuse", "Bêtes gélatineuses", "Bête gélatineuse")
N("Mirror Image", GEN_F | GENF_ELISION, "Image miroir", "Images miroirs", "Image miroir")
N("Mirage", GEN_M, "Mirage", "Mirages", "Mirage")
N("Golem", GEN_M, "Golem", "Golems", "Golem")
N("Eagle", GEN_M | GENF_ELISION, "Aigle", "Aigles", "Aigle")
N("Seep", GEN_F, "Goutte", "Gouttes", "Goutte")
N("Zombie", GEN_M, "Zombie", "Zombies", "Zombie")
N("Ghost", GEN_M, "Fantôme", "Fantômes", "Fantôme")
N("Necromancer", GEN_M, "Nécromancien", "Nécromanciens", "Nécromancien")
N("Shadow", GEN_F | GENF_ELISION, "Ombre", "Ombres", "Ombre")
N("Tentacle", GEN_M, "Tentacule", "Tentacules", "Tentacule")
N("Tentacle Tail", GEN_F, "Queue de tentacule", "Queues de tentacule", "Queue de tentacule")
N("Tentacle W", GEN_M, "Tentacule W", "Tentacules W", "Tentacule W")
N("Tentacle (withdrawing)", GEN_M, "Tentacule (en retrait)", "Tentacules (en retrait)", "Tentacule (en retrait)")
N("Cultist", GEN_M, "Cultiste", "Cultistes", "Cultiste")
N("Fire Cultist", GEN_M, "Cultiste du feu", "Cultistes du feu", "Cultiste du feu")
N("Greater Demon", GEN_M, "Démon majeur", "Démons majeurs", "Démon majeur")
N("Lesser Demon", GEN_M, "Démon mineur", "Démons mineurs", "Démon mineur")
N("Ice Shark", GEN_M, "Requin des glaces", "Requins des glaces", "Requin des glaces")
N("Running Dog", GEN_M, "Chien courant", "Chiens courant", "Chien courant")
N("Demon Shark", GEN_M, "Requin démoniaque", "Requins démoniaques", "Requin démoniaque")
N("Fire Fairy", GEN_F, "Fée du feu", "Fées du feu", "Fée du feu")
N("Crystal Sage", GEN_M, "Sage de cristal", "Sages de cristal", "Sage de cristal")
N("Hedgehog Warrior", GEN_M, "Guerrier hérisson", "Guerriers hérissons", "Guerrier hérisson")

// ITEMS
// =====

N("Ice Diamond", GEN_M, "Diamant des glaces", "Diamants des glaces", "Diamant des glaces")
N("Gold", GEN_M | GENF_ELISION, "Or", "Ors", "Or")
N("Spice", GEN_F | GENF_ELISION, "Épice", "Épices", "Épice")
N("Ruby", GEN_M, "Rubis", "Rubis", "Rubis")
N("Elixir of Life", GEN_M | GENF_ELISION, "Élixir de vie", "Élixirs de vie", "Élixir de vie")
N("Shard", GEN_M | GENF_ELISION, "Éclat", "Éclats", "Éclat")
N("Necromancer's Totem", GEN_M, "Totem du nécromancien", "Totems du nécromancien", "Totem du nécromancien")
N("Demon Daisy", GEN_F, "Marguerite démoniaque", "Marguerites démoniaques", "Marguerite démoniaque")
N("Statue of Cthulhu", GEN_F, "Statue de Cthulhu", "Statues de Cthulhu", "Statue de Cthulhu")
N("Phoenix Feather", GEN_F, "Plume de Phénix", "Plumes de Phénix", "Plume de Phénix")
N("Ice Sapphire", GEN_M, "Saphir des glaces", "Saphirs des glaces", "Saphir des glaces")
N("Hyperstone", GEN_F | GENF_ELISION, "Hyperpierre", "Hyperpierres", "Hyperpierre")
N("Key", GEN_F, "Clé", "Clés", "Clé")
N("Dead Orb", GEN_M | GENF_ELISION, "Orbe mort", "Orbes morts", "Orbe mort")
N("Fern Flower", GEN_F, "Fleur de fougère", "Fleurs de fougère", "Fleur de fougère")

// ORBS: we are using a macro here
// ===============================

#define Orb(E, P) N("Orb of " E, GEN_M | GENF_ELISION, "Orbe " P, "Orbes " P, "Orbe " P)
Orb("Yendor", "de Yendor")
Orb("Storms", "du Tonnerre")
Orb("Flash", "de l’Éclair")
Orb("Winter", "de l'Hiver")
Orb("Speed", "de Vitesse")
Orb("Life", "de Vie")
Orb("Shielding", "de Protection")
Orb("Teleport", "de Téléportation")
Orb("Safety", "de Sécurité")
Orb("Thorns", "des Épines")

// TERRAIN FEATURES
// ================

N("none", GEN_M | GENF_ELISION, "aucun", "aucuns", "aucun")
N("ice wall", GEN_M, "mur de glace", "murs de glace", "mur de glace")
N("great wall", GEN_M, "grand mur", "grands murs", "grand mur")
N("red slime", GEN_F, "gélatine rouge", "gélatines rouges", "gélatine rouge")
N("blue slime", GEN_F, "gélatine bleue", "gélatines bleues", "gélatine bleue")
N("living wall", GEN_M, "mur vivant", "murs vivants", "mur vivant")
N("living floor", GEN_M, "sol vivant", "sols vivants", "sol vivant")
N("dead troll", GEN_M, "troll mort" ,"trolls morts", "troll mort")
N("sand dune", GEN_F, "dune de sable", "dunes de sable", "dune de sable")
N("Magic Mirror", GEN_M, "Miroir Magique", "Miroirs Magiques", "Miroir Magique")
N("Cloud of Mirage", GEN_M, "Nuage des Mirages", "Nuages des Mirages", "Nuage des Mirages")
N("Thumper", GEN_M, "Marteleur", "Marteleurs", "Marteleur")
N("Bonfire", GEN_M, "Feu de joie", "Feux de joie", "Feu de joie")
N("ancient grave", GEN_F, "tombe ancienne", "tombes anciennes", "tombe ancienne")
N("fresh grave", GEN_F, "tombe fraîche", "tombes fraîches", "tombe fraîche")
N("column", GEN_F, "colonne", "colonnes", "colonne")
N("lake of sulphur", GEN_M, "lac de soufre", "lacs de soufre", "lac de soufre")
N("lake", GEN_M, "lac", "lacs", "lac")
N("frozen lake", GEN_M, "lac gelé", "lacs gelés", "lac gelé")
N("chasm", GEN_M, "gouffre", "gouffres", "gouffre")
N("big tree", GEN_M, "grand arbre", "grands arbres", "grand arbre")
N("tree", GEN_M | GENF_ELISION, "arbre", "arbres", "arbre")

// LANDS
// =====

N("Great Wall", GEN_M, "Grand Mur", "Grands Murs", "sur les Grands Murs")
N("Crossroads", GEN_M, "Croisement", "Croisements", "sur les Croisements")
N("Desert", GEN_M, "Désert", "Déserts", "dans les Déserts")
N("Icy Land", GEN_M, "Pays des Glaces", "Pays des Glaces", "dans les Pays des Glaces")
N("Living Cave", GEN_F, "Cave Vivante", "Caves Vivantes", "dans les Caves Vivantes")
N("Jungle", GEN_F, "Jungle", "Jungle", "dans la Jungle")
N("Alchemist Lab", GEN_M, "Laboratoire Alchimique", "Laboratoires Alchimiques", "dans le Laboratoire Alchimique")
N("Mirror Land", GEN_M, "Pays des Miroirs", "Pays des Miroirs", "dans le Pays des Miroirs")
N("Graveyard", GEN_M, "Cimetière", "Cimetières", "dans le Cimetière")
N("R'Lyeh", GEN_M | GENF_PROPER, "R'Lyeh", "R'Lyeh",  "à R'Lyeh")
N("Hell", GEN_M | GENF_ELISION, "Enfer", "Enfers", "en Enfer")
N("Cocytus", GEN_M, "Cocyte", "Cocyte", "dans le Cocyte")
N("Land of Eternal Motion", GEN_M, "Pays du Mouvement perpétuel", "Pays du Mouvement perpétuel", "dans le Pays du Mouvement perpétuel")
N("Dry Forest", GEN_F, "Forêt Sèche", "Forêts Sèches", "dans la Forêt Sèche")
N("Game Board", GEN_M, "Plateau de Jeu", "Plateaux de Jeu", "sur le Plateau de Jeu")

// GAME MESSAGES
// =============

// fighting messages
// ------------Plateau de Jeu-----

// For each English form, provide a French form. Player is referred to via %...0,
// and objects are referred to via %...1 and %...2. For example, in French:

// %?1 is replaced by "?", "?o" or "?a", depending on the gender of %1
// %?e?0 adds "?e?" or "?a?" depending on the gender of the player
// %a1 is exclusively used to automatically say "in the Icy Lands"

// Use whatever %xxx# codes you need for your language. Of course,
// tell me how your codes should be expanded.

S("You kill %the1.", "Vous tuez %le1.")
S("You would be killed by %the1!", "%Le1 vous tuerait !")
S("%The1 would get you!", "%Le1 vous attraperait !")
S("%The1 destroys %the2!", "%Le1 détruit %le2 !")
S("%The1 eats %the2!", "%Le1 mange %le2 !")
S("The ivy destroys %the1!", "Le lierre détruit %le1 !")
S("%The1 claws %the2!", "%Le1 griffe %le2 !")
S("%The1 scares %the2!", "%Le1 effraie %le2 !")
S("%The1 melts away!", "%Le1 fond !")
S("%The1 stabs %the2.", "%Le1 poignarde %le2.")
S("You stab %the1.", "Vous poignardez %le1.")
S("You cannot attack %the1 directly!", "Vous ne pouvez pas attaquer %le1 directement !")
S("Stab them by walking around them.", "Poignardez-les en marchant autour d'eux.")
S("You feel more experienced in demon fighting!", "Vous vous sentez plus expérimenté%e0 en combat contre les démons !")
S("Cthulhu withdraws his tentacle!", "Cthulhu retire son tentacule !")
S("The sandworm explodes in a cloud of Spice!", "Le ver des sables explose dans un nuage d'épice !")
S("%The1 is confused!", "%Le1 est confus%e1.")
S("%The1 raises some undead!", "%Le1 relève des morts !")
S("%The1 throws fire at you!", "%Le1 vous jette du feu dessus !")
S("%The1 shows you two fingers.", "%Le1 vous montre deux doigts.")
S("You wonder what does it mean?", "Vous vous demandez ce que cela peut bien vouloir dire.")
S("%The1 shows you a finger.", "%Le1 vous montre un doigt.")
S("You think about possible meanings.", "Vous pensez aux significations possibles.")
S("%The1 moves his finger downwards.", "%Le1 bouge son doigt vers le bas.")
S("Your brain is steaming.", "Votre cerveau est en ébullition.")
S("%The1 destroys %the2!", "%Le1 détruit %le2 !")
S("You join %the1.", "Vous rejoignez les %P1.")
S("Two of your images crash and disappear!", "Deux de vos images se cognent et disparaissent !")
S("%The1 breaks the mirror!", "%Le1 brise le miroir !")
S("%The1 disperses the cloud!", "%Le1 disperse le nuage !")
S("You activate the Flash spell!", "Vous activez le sort de l’Éclair !")
S("You activate the Lightning spell!", "Vous activez le sort du Tonnerre !")
S("Ice below you is melting! RUN!", "La glace en-dessous de vous fond ! COUREZ !")
S("This spot will be burning soon! RUN!", "Cet endroit sera bientôt en feu ! COUREZ !")
S("The floor has collapsed! RUN!", "Le sol s'est effondré ! COUREZ !")
S("You need to find the right Key to unlock this Orb of Yendor!",
  "Vous devez trouver la bonne clé pour débloquer cette Orbe de Yendor !")
S("You fall into a wormhole!", "Vous tombez dans un trou de ver !")
S("You activate %the1.", "Vous activez %le1.")
S("No room to push %the1.", "Aucun espace pour pousser %le1.")
S("You push %the1.", "Vous poussez %le1.")
S("You start chopping down the tree.", "Vous commencez à abattre l'arbre.")
S("You chop down the tree.", "Vous abattez l'arbre.")
S("You cannot attack Sandworms directly!", "Vous ne pouvez pas attaquer les vers des sables directement !")
S("You cannot attack Tentacles directly!", "Vous ne pouvez pas attaquer les tentacules directement !")
S("You cannot defeat the Shadow!", "Vous ne pouvez pas vaincre l'Ombre !")
S("You cannot defeat the Greater Demon yet!", "Vous ne pouvez pas encore vaincre le Démon Majeur !")
S("That was easy, but groups could be dangerous.", "C'était facile, mais des groupes pourraient être dangereux.")
S("Good to know that your fighting skills serve you well in this strange world.", "Il est bon de savoir que vos compétences de combat vous aident dans ce monde étrange.")
S("You wonder where all these monsters go, after their death...", "Vous vous demandez où vont tous ces monstres, après leur mort...")
S("You feel that the souls of slain enemies pull you to the Graveyard...", "Vous sentez les âmes de vos ennemis tombés vous entraîner vers le Cimetière...")
S("Wrong color!", "Mauvaise couleur !")
S("You cannot move through %the1!", "Vous ne pouvez pas passer à travers %le1 !")
S("%The1 would kill you there!", "%Le1 vous tuerait ici !")
S("Wow! %1! This trip should be worth it!", "Waouh, %un1 ! Ce voyage devrait valoir le coup !")
S("For now, collect as much treasure as possible...", "Pour l'instant, récoltons le plus de trésors possible...")
S("Prove yourself here, then find new lands, with new quests...", "Faites vos preuves ici, puis trouvez de nouveaux pays, avec de nouvelles quêtes....")
S("You collect your first %1!", "Vous ramassez votre premi%er1 %1 !")
S("You have found the Key! Now unlock this Orb of Yendor!", "Vous avez trouvé la Clé ! Il est temps de débloquer l'Orbe de Yendor !")
S("This orb is dead...", "Cet Orbe est mort...")
S("Another Dead Orb.", "Un autre Orbe mort.")
S("You have found %the1!", "Vous avez trouvé%e0 %le1 !")
S("You feel that %the2 become%s2 more dangerous.", "Vous sentez que %le2 devient plus dangereu%x1.")
S("With each %1 you collect...", "Avec chaque %1 ramassé%e1...")
S("Are there any magical orbs in %the1?...", "Il y a-t-il des orbes magiques %a1?")
S("You feel that %the1 slowly become%s1 dangerous...", "Vous sentez que %le1 devient peu à peu plus dangereu%x1...")
S("Better find some other place.", "Mieux aller voir ailleurs.")
S("You have a vision of the future, fighting demons in Hell...", "Vous avez une vision de l'avenir, combattant des démons en Enfer...")
S("With this Elixir, your life should be long and prosperous...", "Avec cet Élixir, votre vie devrait être longue et prospère...")
S("The Necromancer's Totem contains hellish incantations...", "Le Totem du Nécromancien contient des incantations démoniaques...")
S("The inscriptions on the Statue of Cthulhu point you toward your destiny...", "Les inscriptions sur la statue de Cthulhu vous montrent la voie vers votre destinée...")
S("Still, even greater treasures lie ahead...", "De plus grands trésors vous attendent...")
S("You collect %the1.", "Vous ramassez %un1.")
S("CONGRATULATIONS!", "FÉLICITATIONS !")
S("Collect treasure to access more different lands...", "Rassemblez des trésors pour atteindre encore plus de pays différents...")
S("You feel that you have enough treasure to access new lands!", "Vous sentez que vous avez assez de trésors pour accéder à de nouveaux pays !")
S("Collect more treasures, there are still more lands waiting...", "Rassemblez plus de trésors, il y a encore des pays à découvrir...")
S("You feel that the stars are right, and you can access R'Lyeh!", "Vous sentez que les étoiles s'alignent, et que vous pouvez accéder à R'Lyeh !")
S("Kill monsters and collect treasures, and you may get access to Hell...", "Tuez des monstres et ramassez des trésors, et vous gagnerez peut-être l'accès à l'Enfer...")
S("To access Hell, collect %1 treasures each of %2 kinds...", "Pour atteindre l'Enfer, rassemblez %1 trésors chacun de %2 sortes...")
S("Abandon all hope, the gates of Hell are opened!", "Abandonnez tout espoir, les portes de l'Enfer sont ouvertes !")
S("And the Orbs of Yendor await!", "Et les Orbes de Yendor vous attendent !")
S("You switch places with %the1.", "Vous échangez votre place avec %le1.")
S("You rejoin %the1.", "Vous rejoignez %le1.")
S("The mirror shatters!", "Le Mirroir éclate !")
S("The cloud turns into a bunch of images!", "Les nuages se transforment en une multitude d'images !")
S("The slime reacts with %the1!", "La gélatine réagit avec %le1 !")
S("You drop %the1.", "Vous lâchez %le1.")
S("You feel great, like a true treasure hunter.", "Vous vous sentez magnifiquement bien, comme un vrai chasseur de trésor.")
S("Your eyes shine like gems.", "Vous yeux brillent comme des pierres précieuses.")
S("Your eyes shine as you glance at your precious treasures.", "Vous yeux s'illuminent lorsque vous regardez vos précieux trésors.")
S("You glance at your great treasures.", "Vous regardez vos grands trésors.")
S("You glance at your precious treasures.", "Vous regardez vos précieux trésors.")
S("You glance at your precious treasure.", "Vous regardez votre précieux trésor.")
S("Your inventory is empty.", "Votre inventaire est vide.")
S("You teleport to a new location!", "Vous vous téléportez à un nouvel endroit !")
S("Could not open the score file: ", "Les meilleurs résultats n'ont pas pu être chargés : ")
S("Game statistics saved to %1", "Les statistiques de la partie ont été sauvegardées dans %1")
S("Game loaded.", "Partie chargée.")
S("You summon some Mimics!", "Vous avez invoqué des Mimiques !")
S("You summon a golem!", "Vous avez invoqué un Golem !")
S("You will now start your games in %1", "Vous commencerez maintenant vos parties %a1")
S("Activated the Hyperstone Quest!", "La quête de l'Hyperpierre est activée !")
S("Orb power depleted!", "Le pouvoir de l'orbe est épuisé !")
S("Orbs summoned!", "Orbes invoqués !")
S("Orb power gained!", "L'Orbe gagne du pouvoir !")
S("Dead orbs gained!", "Orbe mort récupéré !")
S("Orb of Yendor gained!", "Orbe de Yendor récupéré")
S("Treasure gained!", "Trésor récupéré !")
S("Lots of treasure gained!", "Plein de trésors récupérés !")
S("You summon a sandworm!", "Vous invoquez un Ver des sables !")
S("You summon an Ivy!", "Vous invoquez un Lierre !")
S("You summon a monster!", "Vous invoquez un monstre !")
S("You summon some Thumpers!", "Vous invoquez des Marteleurs !")
S("You summon a bonfire!", "Vous invoquez un feu de joie !")
S("Treasure lost!", "Trésor perdu !")
S("Kills gained!", "Kills acquis !")
S("Activated Orb of Safety!", "Orbe de Sécurité activé !")
S("Teleported to %1!", "Téléporté%e0 vers %1")

S("Welcome to HyperRogue", "Bienvenue dans HyperRogue")
S(" for Android", " pour Android")
S(" for iOS", " pour iOS")
S("! (version %1)\n\n", "! (Version %1)\n\n")
S(" (press ESC for some hints about it).", " (appuyer sur ESC pour afficher des astuces).")
S("Press 'c' for credits.", "Appuyer sur 'c' pour les crédits")
S("game design, programming, texts and graphics by Zeno Rogue <zeno@attnam.com>\n\n",
  "Conception du jeu, programmation, texte et graphiques par Zeno Rogue <zeno@attnam.com>\n\n")
S("add credits for your translation here", "Traduction Française : 3raven \n\n")
S(" (touch to activate)", " (toucher pour activer)")
S(" (expired)", " (expiré)")
S(" [%1 turns]", " [%1 tours]")
S(", you", ", vous")
S("0 = Klein model, 1 = Poincaré model", "0 = Modèle de Klein, 1 = Modèle de Poincaré")
S("you are looking through it!", "Vous comprenez !")
S("simply resize the window to change resolution", "Pour modifier la résolution, changer simplement la taille de la fenêtre")
S("[+] keep the window size, [-] use the screen resolution", "[+] garder la taille de la fenêtre, [-] utiliser la résolution de l'écran")
S("+5 = center instantly, -5 = do not center the map", "+5 = centrer instantanément, -5 = ne pas center la carte")
S("press Space or Home to center on the PC", "Appuyer sur Espace ou Début pour centrer sur l'écran")
S("You need special glasses to view the game in 3D", "Vous avez besoin de lunettes spéciales pour voir le jeu en 3D")
S("You can choose one of the several modes", "Vous pouvez choisir un des différents modes")
S("ASCII", "ASCII")
S("black", "noir")
S("plain", "simple")
S("Escher", "Escher")
S("items only", "seulement les items")
S("items and monsters", "items et monstres")
S("no axes", "pas d'axes")
S("auto", "auto")
S("light", "facile")
S("heavy", "difficile")
S("The axes help with keyboard movement", "Les axes aident avec le mouvement de clavier")
S("Config file: %1", "Fichier de config : %1")
S("joystick mode: automatic (release the joystick to move)", "Joystick : automatique (relâcher le joystick pour bouger)")
S("joystick mode: manual (press a button to move)", "Joystick : manuel (appuyer sur un bouton pour bouger)")
S("Reduce the framerate limit to conserve CPU energy", "Réduire la limite du framerate pour économiser l'énergie du CPU")
S("Press F1 or right click for help", "Appuyer sur F1 ou faire un clic droit pour l'aide")
S("No info about this...", "Pas d'info à ce sujet...")
S("Press Enter or F10 to save", "Appuyer sur Entrée ou F10 pour sauvegarder")
S("Press Enter or F10 to quit", "Appuyer sur Entrée ou F10 pour quitter le jeu")
S("or 'r' or F5 to restart", "ou 'r' ou bien F5 pour recommencer du début")
S("or 't' to see the top scores", "ou 't' pour la liste des meilleurs résultats")
S("or another key to continue", "ou une autre touche pour continuer.")
S("It is a shame to cheat!", "C'est une honte de tricher !")
S("Showoff mode", "Mode démonstration")
S("Quest status", "Status de la quête")
S("GAME OVER", "GAME OVER")
S("Your score: %1", "Points : %1")
S("Enemies killed: %1", "Ennemis tués : %1")
S("Orbs of Yendor found: %1", "Orbes de Yendor trouvés : %1")
S("Collect %1 $$$ to access more worlds", "Rassemblez %1 $$$ pour accéder à plus de pays")
S("Collect at least %1 treasures in each of %2 types to access Hell", "Rassemblez au moins %1 trésors chacun de %2 types différents pour accéder à l'Enfer")
S("Collect at least %1 Demon Daisies to find the Orbs of Yendor", "Ramassez au moins %1 marguerites démoniaques pour trouver les Orbes de Yendor")
S("Hyperstone Quest: collect at least %3 %1 in %the2", "Quête de l'Hyperpierre : Rassemblez au moins %3 %P1 %a2")
S("Hyperstone Quest completed!", "Quête de l'Hyperpierre complétée !")
S("Look for the Orbs of Yendor in Hell or in the Crossroads!", "Cherchez les Orbes de Yendor dans l'Enfer et les Croisements !")
S("Unlock the Orb of Yendor!", "Ouvrez l'Orbe de Yendor !")
S("Defeat %1 enemies to access the Graveyard", "Tuez %1 ennemis pour accéder au Cimetière")
S("(press ESC during the game to review your quest)", "(Appuyer sur ESC pendant le jeu pour voir votre quête)")
S("you have cheated %1 times", "vous avez triché %1 fois")
S("%1 turns (%2)", "%1 tours (%2)")
S("last messages:", "derniers messages : ")
S("time elapsed", "temps écoulé")
S("date", "date")
S("treasure collected", "trésor collecté")
S("total kills", "total d'ennemis tués")
S("turn count", "décompte des tours")
S("cells generated", "cellules générées")
S("t/left/right - change display, up/down - scroll, s - sort by", " t/gauche/droite - changer l'affichage, haut/bas - scroller, s - trier par")
S("kills", "kills")
S("time", "temps")
S("ver", "ver")
S("SORT", "TRI")
S("PLAY", "JOUER")
S("Your total wealth", "Votre richesse totale")
S("treasure collected: %1", "trésor collecté : %1")
S("objects found: %1", "objets trouvés : %1")
S("orb power: %1", "pouvoir de l'orbe : %1")
S(" (click to drop)", " (cliquer pour lâcher)")
S("You can also scroll to the desired location and then press 't'.", "Vous pouvez aussi scroller jusqu'à la position voulue puis appuyer sur 't'.")
S("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n",
  "Ainsi, ceci est parfois utile pour les très long jeux, qui pourraient accaparer toute la mémoire autrement.")
S("You can touch the Dead Orb in your inventory to drop it.", "Vous pouvez toucher l'Orbe mort dans votre inventaire pour le lâcher.")
S("This might be useful for Android devices with limited memory.", "Cela pourrait s'avérer utile pour les mobiles sous Android avec une mémoire limitée.")
S("You can press 'g' or click them in the list to drop a Dead Orb.", "Vous pouvez appuyer sur 'g' ou cliquer dessus dans la liste pour lâcher un Orbe mort.")
S("frames per second", "FPS")
S("monsters killed: %1", "Kills: %1")
S("Drawing %1 (layer %2), F1 for help", "Tracé %1 (couche %2), F1 pour l'aide")
S("hepta floor", "sol heptagonal")
S("hexa floor", "sol hexagonal")
S("character", "personnage")
S("ESC for menu/quest", "ESC pour le Menu/la Quête")
S("vector graphics editor", "éditeur de graphiques vectoriels")
S("cheat mode", "mode triche")
S("heptagonal game board", "plateau de jeu heptagonal")
S("triangular game board", "plateau de jeu triangulaire")
S("HyperRogue game board", "plateau de jeu HyperRogue")
S("first page [Space]", "première page [Espace]")
S("Configuration:", "Configuration :")
S("video resolution", "résolution vidéo")
S("fullscreen mode", "plein écran")
S("animation speed", "vitesse d'animation")
S("dist from hyperboloid ctr", "distance au centre hyperbolique")
S("scale factor", "facteur d'échelle")
S("wall display mode", "mode d'affichage des murs")
S("monster display mode", "mode d'affichage des monstres")
S("cross display mode", "mode d'affichage des croix")
S("background music volume", "volume de la musique de fond")
S("OFF", "OFF")
S("ON", "ON")
S("distance between eyes", "distance entre les yeux")
S("framerate limit", "limite du framerate")
S("joystick mode", "mode joystick")
S("automatic", "automatique")
S("manual", "manuel")
S("language", "langage")
S("EN", "FR")
S("player character", "personnage du joueur")
S("male", "masculin")
S("female", "féminin")
S("use Shift to decrease and Ctrl to fine tune ", "utiliser Shift pour diminuer, Ctrl pour écouter")
S("(e.g. Shift+Ctrl+Z)", "(par ex. Shift+Ctrl+Z)")
S("the second page [Space]", "la deuxième page [Espace]")
S("special features [Space]", "fonctionnalités spéciales [Espace]")
S("see the help screen", "voir l'aide")
S("save the current config", "sauvegarder la configuration actuelle")
S("(v) config", "(v) config.")
S("Screenshot saved to %1", "Capture d'écran enregistrée dans %1")
S("You need an Orb of Teleport to teleport.", "Vous avez besoin d'un Orbe de Téléportation pour vous téléporter.")
S("Use arrow keys to choose the teleport location.", "Utilisez les touches directionnelles pour choisir le lieu de téléportation.")
S("openGL mode enabled", "mode openGL activé")
S("openGL mode disabled", "mode openGL désactivé")
S("openGL & antialiasing mode", "mode openGL & antialiasing")
S("anti-aliasing enabled", "antialiasing activé")
S("anti-aliasing disabled", "antialiasing désactivé")
S("You activate your demonic powers!", "Vous activez vos pouvoirs démoniaques !")

// Steam achievement messages
// --------------------------

S("New Achievement:", "Nouvel Achievement :")
S("Your total treasure has been recorded in the Steam Leaderboards.", "Votre trésor total a été enregistré sur le classement Steam.")
S("Congratulations!", "Félicitations !")
S("You have improved your total high score and %1 specific high scores!", "Vous avez amélioré votre record total et %1 records spécifiques !")
S("You have improved your total and '%1' high score!", "Vous avez amélioré votre record total et votre record %de1 !")
S("You have improved your total high score on Steam. Congratulations!", "Vous avez amélioré votre record total sur Steam. Félicitations !")
S("You have improved %1 of your specific high scores!", "Vous avez amélioré %1 de vos records spécifiques !")
S("You have improved your '%1' high score on Steam!", "Vous avez amélioré votre record %de1 sur Steam !")
S("You have collected 10 treasures of each type.", "Vous avez rassemblé 10 trésors de chaque type.")
S("This is your first victory!", "C'est votre première victoire !")
S("This has been recorded in the Steam Leaderboards.", "Ceci a été enregistré sur le classement Steam.")
S("The faster you get here, the better you are!", "Plus vous arrivez là rapidement, meilleur%e0 vous êtes !")
S("You have improved both your real time and turn count. Congratulations!", "Vous avez amélioré à la fois votre record de temps et de nombre de tours. Félicitations !")
S("You have used less real time than ever before. Congratulations!", "Vous avez pris moins de temps que la dernière fois. Félicitations !")
S("You have used less turns than ever before. Congratulations!", "Vous avez pris moins de tours que la dernière fois. Félicitations !")

// help texts
// ----------

// These are separated into multiple lines just for convenience,
// you don't have to follow.

S("You have been trapped in a strange, non-Euclidean world. Collect as much treasure as possible "
  "before being caught by monsters. The more treasure you collect, the more "
  "monsters come to hunt you, as long as you are in the same land type. The "
  "Orbs of Yendor are the ultimate treasure; get at least one of them to win the game!",

  "Vous êtes piégé%e0 dans un monde étrange et non-euclidien. Ramassez autant de trésors que possible "
  "avant d'être attrapé%e0 par les monstres. Plus vous ramassez de trésors, plus les monstres "
  "viendront vous chasser jusqu'à ce que vous quittiez ce type de pays. Les "
  "Orbes de Yendor sont le trésor ultime ; obtenez en au moins un pour gagner !" )

S("You can fight most monsters by moving into their location. "
  "The monster could also kill you by moving into your location, but the game "
  "automatically cancels all moves which result in that.\n\n",

  "Vous pouvez combattre la plupart des monstres en allant vers leur position. "
  "Le monstre peut aussi vous tuer en venant vers votre position, mais le jeu "
  "annule automatiquement les mouvements qui mènent à votre mort.\n\n")

S("Usually, you move by touching somewhere on the map; you can also touch one "
  "of the four buttons on the map corners to change this (to scroll the map "
  "or get information about map objects). You can also touch the "
  "numbers displayed to get their meanings.\n",
  "Habituellement, vous bougez en touchant un endroit de la carte ; vous pouvez aussi touchez un "
  "des quatre coins de la carte pour changer ceci (pour scroller ou obtenir des informations "
  "sur les objets au loin). Vous pouvez aussi toucher les nombres affichés pour afficher leur signification.\n")

S("Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Space. To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n",
  "Bouger avec la souris, le pavé numérique, qweadzxc, ou hjklyubn. Attendre en appuyant sur 's' ou '.'. Tournez le monde avec des flèches, scroll, et Espace."
  "Pour sauvegarder la partie il faut un Orbe de Sécurité. Appuyez sur 'v' pour le menu (config, modes spéciaux, etc... ), ESC pour le statut de la quête.\n\n")

S("See more on the website: ", "Plus sur le site internet : ")

S("special thanks to the following people for their bug reports, feature requests, porting, and other help:\n\n%1\n\n",
  "remerciements spéciaux aux personnes suivantes pour leurs report de bugs, demandes de fonctionnalités, portage et autres aides :\n\n%1\n\n")

S("The total value of the treasure you have collected.\n\n"
  "Every world type contains a specific type of treasure, worth 1 $$$; "
  "your goal is to collect as much treasure as possible, but every treasure you find "
  "causes more enemies to hunt you in its native land.\n\n"
  "Orbs of Yendor are worth 50 $$$ each.\n\n",


  "La valeur totale du trésor que vous avez ramassé.\n\n"
  "Chaque pays a un type de trésor, qui vaut 1 $$$. "
  "Votre objectif est de rassembler autant de trésor que possible, mais chaque trésor trouvé "
  "mets plus d'ennemis à votre poursuite dans son pays d'origine.\n\n"
  "Les Orbes de Yendor valent 50 $$$ chacun.\n\n")

S("The higher the number, the smoother the animations in the game. "
  "If you find that animations are not smooth enough, you can try "
  "to change the options ",
  "Plus le nombre est grand, plus les animations sont fluides."
  "Si elles ne sont pas assez fluides, vous pouvez essayer de "
  "changer les options.")

S("(Menu button) and select the ASCII mode, which runs much faster. "
  "Depending on your device, turning the OpenGL mode on or off might "
  "make it faster, slower, or cause glitches.",
  "(Dans le Menu) sélectionnez le mode ASCII, qui va beaucoup plus vite. "
  "Selon l'appareil, utiliser ou non le mode openGL peut le rendre plus "
  "rapide, lent ou causer des glitches.")

S("(in the MENU). You can reduce the sight range, this should make "
  "the animations smoother.",
  "(Dans le Menu) vous pouvez réduire la distance de vue, cela devrait rendre les animations plus fluides.")

S("(press v) and change the wall/monster mode to ASCII, or change "
"the resolution.",
  "(appuyez sur v) pour changer le mode mur/monstre en ASCII, ou "
  "pour changer la résolution.")

S("In this mode you can draw your own player character and floors. "
  "Mostly for the development purposes, but you can have fun too.\n\n"
  "f - floor, p - player (repeat 'p' for layers)\n\n"
  "n - new shape, u - copy the 'player body' shape\n\n"
  "1-9 - rotational symmetries, 0 - toggle axial symmetry\n\n"
  "point with mouse and: a - add point, m - move nearest point, d - delete nearest point, c - nearest point again, b - add after nearest\n\n"
  "s - save in C++ format (but cannot be loaded yet without editing source)\n\n"
  "z - zoom, o - Poincaré model\n",

  "Dans ce mode vous pouvez tracer votre propre personnage joueur et sols. "
  "Principalement à des fins de développement, mais vous pouvez vous amusez avec.\n\n"
  "f - sols, p - joueur (répéter 'p' pour les couches)\n\n"
  "n - nouvelle forme, u - copier la forme corporelle du joueur\n\n"
  "1-9 - symétrie centrale, 0 - déclencher la symétrie axiale\n\n"
  "pointer avec la souris : a - ajouter un point, m - bouger le point le plus proche, d - supprimer le point le plus proche, c - copier le point le plus proche, b - ajouter après le point le plus proche\n\n"
  "s - sauver en format C++ (mais ne peut pas être chargé pour l'instant sans éditer le code source)\n\n"
  "z - zoom, o - modèle de Poincaré\n")

S("These huge monsters normally live below the sand, but your movements have "
  "disturbed them. They are too big to be slain with your "
  "weapons, but you can defeat them by making them unable to move. "
  "This also produces some Spice. They move two times slower than you.",

  "Ces monstres gigantesques vivent normalement sous le sable mais vos mouvements "
  "les ont dérangé. Ils sont trop grands pour être tués avec vos armes mais "
  "vous pouvez les vaincre en les empêchant de bouger. Cela produit aussi de "
  "l'épice. Ils vont deux fois plus lentement que vous.")

S("The tentacles of Cthulhu are like sandworms, but longer. "
  "They also withdraw one cell at a time, instead of exploding instantly.",

  "Les tentacules de Cthulhu sont comme les vers des sables, mais en plus long. "
  "Au lieu d'exploser ils se retirent une cellule après l'autre.")

S("A huge plant growing in the Jungle. Each Ivy has many branches, "
  "and one branch grows per each of your moves. Branches grow in a clockwise "
  "order. The root itself is vulnerable.",
  "Une plante gigantesque croissant dans la Jungle. Chaque  Lierre a de nombreuses branches, "
  "et une branche pousse pour chacun de vos mouvements. Les branches poussent dans le "
  "sens des aiguilles d'une montre. La racine elle-même est vulnérable.")

S("The Alchemists produce magical potions from pools of blue and red slime. You "
  "can go through these pools, but you cannot move from a blue pool to a red "
  "pool, or vice versa. Pools containing items count as colorless, and "
  "they change color to the PC's previous color when the item is picked up. "
  "Slime beasts also have to keep to their own color, "
  "but when they are killed, they explode, destroying items and changing "
  "the color of the slime and slime beasts around them.",
  "Les Alchimistes pruduisent des potions magiques à partir de bassins de gélatine "
  "rouge ou bleue. Vous pouvez traverser ces bassins, mais pas passer d'un bassin "
  "bleu vers un rouge, et inversement. Les bassins contenant des objets sont sans "
  "couleur, et changent de couleur vers leur couleur passée lorsque l'objet est "
  "ramassé. Les bêtes gélatineuses gardent également leur couleur, mais explosent lorsque tués "
  "ce qui qui détruit les items et change la couleur des bêtes gélatineuses et des bassins de "
  "gélatine autour.")

S("These creatures are slow, but very powerful... more powerful than you. "
  "You need some more experience in demon fighting before you will be able to defeat them. "
  "Even then, you will be able to slay this one, but more powerful demons will come...\n\n"
  "Each 10 lesser demons you kill, you become powerful enough to kill all the greater "
  "demons on the screen, effectively turning them into lesser demons.",

  "Ces créatures sont lentes, mais très puissantes... plus que vous. "
  "Vous avez besoin d'expérience de combat contre des démons, avant de pouvoir les vaincre. "
  "Même lorsque vous pourrez en tuer un, des démons plus puissants viendront...\n\n"
  "Pour chaque dizaine de démons tués, vous devenez assez puissant pour tuer tout les démons "
  "supérieurs à l'écran, ce qui les transforme en démon moindres.")

S("These creatures are slow, but they often appear in large numbers.",
  "Ces créatures sont lentes, mais apparaissent souvent en grands nombres.")

S("A big monster from the Living Caves. A dead Troll will be reunited "
  "with the rocks, causing some walls to grow around its body.",
  "Un grand monstre des Caves Vivantes. Un Troll mort retournera à la "
  "pierre, faisant pousser des murs autour de son corps.")

S("Huge, impassable walls which separate various lands.",
  "D'immenses, infranchissables murs, qui séparent différents pays.")

S("This cave contains walls which are somehow living. After each turn, each cell "
  "counts the number of living wall and living floor cells around it, and if it is "
  "currently of a different type than the majority of cells around it, it switches. "
  "Items count as three floor cells, and dead Trolls count as five wall cells. "
  "Some foreign monsters also count as floor or wall cells.\n",
  "Ces caves contiennent des murs d'une certaine manière vivants. Après chaque tour, "
  "chaque cellule compte le nombre de murs et sols vivants autour d'elle, et si elle "
  "est d'un type différent des cellules qui l'entoure, il change. "
  "Quelque monstres étrangers comptent également comme mur ou sol.\n")

S("This forest is quite dry. Beware the bushfires!\n"
  "Trees catch fire on the next turn. The temperature of the grass cells "
  "rises once per turn for each fire nearby, and becomes fire itself "
  "when its temperature has risen 10 times.\n"
  "You can also chop down the trees. Big trees take two turns to chop down.",
  "Cette forêt est plutôt sèche. Attention aux feux de forêt !\n"
  "Les arbres prennent feux au tour suivant. La température des cellules d'herbe "
  "augmente à chaque tour pour chaque feu à proximité, et prend feu elle-même "
  "lorsque la température a augmenté 10 fois.\n"
  "Vous pouvez aussi abattre les arbres. Les grands arbres prennent deux tours à abattre.")

S("A big and quite intelligent monster living in the Icy Land.",
  "Un monstre géant et plutôt intelligent vivant dans le Pays des Glaces.")

S("A nasty predator from the Icy Land. Contrary to other monsters, "
  "it tracks its prey by their heat.",
  "Un prédateur vicieux originaire du Pays des Glaces. Contrairement aux autres monstres,"
  "il traque ses proies par leur chaleur.")

S("Rangers take care of the magic mirrors in the Land of Mirrors. "
  "They know that rogues like to break these mirrors... so "
  "they will attack you!",
  "Les Gardiens protègent les miroirs magiques du Pays des Miroirs. "
  "Ils savent que les vagabonds comme vous aiment briser les miroirs... "
  "alors ils vous attaqueront !")

S("A nasty creature that lives in caves. They don't like you for some reason.",
  "Une créature vicieuse qui vit dans les caves. Elles ne vous aiment pas apparemment.")

S("A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them.",
  "Une tribu d'hommes originaires du désert. Ils ont même apprivoisé les vers des sables géants, qui ne "
  "les attaquent pas.")

S("This giant ape thinks that you are an enemy.", "Ce singe géant pense que vous êtes l'ennemi.")

S("A magical being which copies your movements.", "Un être magique qui copie vos mouvements.")

S("A magical being which copies your movements. "
  "You feel that it would be much more useful in an Euclidean space.",
  "Un être magique qui copie vos mouvements.\n"
  "Vous avez le sentiment que cela serait beaucoup plus utile dans un espace euclidien.")

S("You can summon these friendly constructs with a magical process.",
  "Vous pouvez invoquer ces constructs amicaux par un rituel magique.")

S("A majestic bird, who is able to fly very fast.",
  "Un oiseau majestueux capable de voler très vite.")

S("A monster who is able to live inside the living cave wall.",
  "Un monstre qui peut vivre à l'intérieur d'un mur vivant d'une cave.")


S("A typical Graveyard monster.", "Un monstre typique du Cimetière.")

S("A typical monster from the Graveyard, who moves through walls.\n\n"
  "There are also wandering Ghosts. They will appear "
  "if you do not explore any new places for a long time (about 100 turns). "
  "They can appear anywhere in the game.",
  "Un monstre typique du Cimetière, qui peut traverser les murs.\n\n"
  "Il y a aussi des fantômes vagabonds. Ils apparaîtront si vous n'explorez de nouveaux endroits (pendant 100 tours). "
  "Ils peuvent apparaître n'importe où dans le jeu.")

S("Necromancers can raise ghosts and zombies from fresh graves.",
  "Les Nécromanciens peuvent relever des fantômes et zombies des tombes fraîches.")

S("A creepy monster who follows you everywhere in the Graveyard and the Cursed Canyon.",
  "Un monstre effrayant qui vous suit partout dans le Cimetière et le Canyon Maudit.")

S("People worshipping Cthulhu. They are very dangerous.",
  "Des cultistes qui vénèrent Cthulhu. Ils sont très dangereux.")

S("People worshipping Cthulhu. This one is especially dangerous, "
  "as he is armed with a weapon which launches fire from afar.",
  "Un cultiste qui vénère Cthulhu. Celui là est particulièrement dangereux, "
  "car il est équipé d'une arme qui lance du feu à distance.")

S("This dangerous predator has killed many people, and has been sent to Cocytus.",
  "Ce dangereux prédateur a tué de nombreuses personnes, et a été envoyé au Cocytus.")

S("This white dog is able to run all the time. It is the only creature "
  "able to survive and breed in the Land of Eternal Motion.",
  "Ce chien blanc est capable de courir en permanence. C'est la seule créature "
  "capable de survivre et se reproduire dans le Pays du Mouvement perpétuel.")

S("Demons of Hell do not drown when they fall into the lake in Cocytus. "
  "They turn into demonic sharks, enveloped in a cloud of steam.",
  "Les Démons de l'Enfer ne se noient pas lorsqu'ils tombent dans le lac du Cocytus. "
  "Ils se transforment en requins démoniaques, enveloppés dans un nuage de vapeur.")

S("These fairies would rather burn the forest, than let you get some Fern Flowers. "
  "The forest is infinite, after all...\n\n"
  "Fire Fairies transform into fires when they die.",
  "Ces fées préfèreraient brûler la forêt, plutôt que de vous laisser cueillir quelques Fleurs de fougères. "
  "La forêt est infinie, après tout...\n\n"
  "Ces Fée du Feu se transforme en feu à leur mort.")

S("These warriors of the Forest wield exotic weapons called hedgehog blades. "
  "These blades protect them from a frontal attack, but they still can be 'stabbed' "
  "easily by moving from one place next to them to another.",
  "Ces guerriers de la Forêt manient des armes exotiques appelées lames hérisson. "
  "Ces lames les protègent des attaques frontales, mais ils peuvent toujours être 'poignardés' "
  "facilement en bougeant d'un case adjacente à eux à une autre.")

S("This being radiates an aura of wisdom. "
  "It is made of a beautiful crystal, you would love to take it home. "
  "But how is it going to defend itself? Better not to think of it, "
  "thinking causes your brain to go hot...\n\n"
  "Crystal Sages melt at -30 °C, and they can raise the temperature around you from afar.",
  "Cet être irradie une aura de sagesse. "
  "Ceci est fait d'un magnifique cristal, vous adoreriez le rapporter à la maison "
  "Mais comment va t il se défendre ? Mieux vaut ne pas y penser, \n\n"
  "Les Sages de Cristal fondent à -30 °C. Et ils peuvent augmenter la température autour de vous à distance.")

S("Cold white gems, found in the Icy Land.", "Une gemme blanche et froide, qui se trouve dans le Pays des Glaces.")

S("An expensive metal from the Living Caves. For some reason "
  "gold prevents the living walls from growing close to it. ",
  "Un métal coûteux venant des Caves Vivantes. Il semblerait que "
  " l'or empêche les murs vivants de croître à proximité.")

S("A rare and expensive substance found in the Desert. "
  "It is believed to extend life and raise special psychic powers.",
  "Une substance rare et coûteuse qui se trouve dans le Désert. "
  "Il se dit qu'elle allonge la durée de vie et éveille des pouvoirs psychiques spéciaux.")

S("A beautiful gem from the Jungle.", "Une gemme magnifique venant de la Jungle.")

S("A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
  "healthy after drinking it, but you still feel that one hit of a monster is enough to kill you.",
  "Une boisson merveilleuse, obtenue apparemment en mélangeant de la gélatine rouge et bleue. Vous vous sentez "
  "en bien meilleure forme après l'avoir bue, mais qu'un seul coup d'un monstre peut encore vous tuer.")

S("A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
  "in the Land of Mirrors leave these.",
  "Un morceau de miroir magique, ou de nuage de mirage, qui peut être utilisé pour des usages magiques. "
  "Seulement des miroirs et nuages du Pays des Miroirs permettent de les obtenir.")

S("These sinister totems contain valuable gems.",
  "Ces totems sinistres contiennent des gemmes précieuses.")

S("These star-shaped flowers native to Hell are a valuable alchemical component.",
  "Ces fleurs étoilées originaires de l'Enfer sont un ingrédient alchimique précieux.")

S("This statue is made of materials which cannot be found in your world.",
  "Cette statue est faite de matériaux qui ne peuvent être trouvés dans votre monde.")

S("One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable.",
  "Un des rares objets qui ne font pas s'effondrer le Pays du Mouvement perpétuel. Elles sont sûrement très précieuses.")

S("Cold blue gems, found in the Cocytus.", "Gemmes froides et bleues, qui se trouvent dans le Cocytus.")

S("These bright yellow gems can be found only by those who have mastered the Crossroads.",
  "Ces gemmes brillantes jaunes peuvent être trouvées seulement par ceux qui on maitrisé les Croisements.")

S("That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
  "Each key unlocks only the Orb of Yendor which led you to it.",
  "C'est tout ce dont vous avez besoin pour débloquer l'Orbe de Yendor ! ...à condition de retourner à l'Orbe que cette clé ouvre...\n"
  "Chaque clé ne débloque que l'Orbe de Yendor qui vous a y conduit.")

S("These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
  "power is long gone. No way to use them, you could as well simply drop them...\n\n",
  "Ces orbes ne se trouvent que dans le Cimetière. Vous pensez qu'il s'agissait autrefois de puissants orbes magiques, mais "
  "désormais leur pouvoir a disparu depuis longtemps. Aucun moyen de les utiliser, autant les jeter...\n\n")

S("This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
  "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
  "where the key is after you touch the Orb.",
  "Cet orbe merveilleux peut seulement être trouvé par ceux qui ont réellement maîtrisé l'univers hyperbolique, car "
  "vous avez besoin de la bonne clé pour la débloquer. Heureusement vos capacités psychiques vous diront où est la clé "
  "après avoir touché l'orbe.")

S("This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions.",
  "Cet orbe peut être utilisé pour invoquer le sort de foudre, qui envoie des éclairs dans toutes les directions.")

S("This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2.",
  "Cet orbe peut être utilisé pour invoquée le sort de lumière, qui détruit quasiment tout sur un rayon de 2.")

S("This orb can be used to invoke a wall of ice. It also protects you from fires.",
  "Cet orbe peut être utilisé pour invoquer un mur de glace. Il vous protège du feu.")

S("This orb can be used to move faster for some time.",
  "Cet orbe peut être utilisé pour courir plus vite pour un certain temps.")

S("This orb can be used to summon friendly golems. It is used instantly when you pick it up.",
  "Cet orbe peut être utilisé pour invoquer des golems amicaux. Il est utilisé instantanément lorsque ramassé.")

S("This orb can protect you from damage.", "Cet orbe peut vous protéger des dégâts.")

S("This orb lets you instantly move to another location on the map. Just click a location which "
  "is not next to you to teleport there. ",
  "Cet orbe vous permet d'aller instantanément à un autre endroit de la carte."
  "Cliquez une case non adjacente à votre position pour vous téléporter là.")

S("This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
  "that you will ever find the way back...\n\n"
  "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
  "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). ",
  "Cet Orbe vous téléporte vers un endroit sûr. Étant donné la nature de cet étrange monde, vous doutez pouvoir un jour retrouver votre chemin pour revenir en arrière...\n\n"
  "Votre jeu sera sauvegardé si vous quittez le jeu avec l'Orbe de Sécurité encore active.\n\n"
  "Note technique : étant donné qu'il est virtuellement impossible de faire demi-tour, cet Orbe recycle la mémoire utilisée jusque là pour le monde (même si vous ne l'utilisez pas pour sauvegarder le jeu.)")

S("This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n",
  "Cet orbe permet d'attaquer les guerriers-hérissons directement, ainsi que de poignarder d'autres monstres.\n")

S("This flower brings fortune to the person who finds it.\n",
  "Cette fleur porte chance à la personne qui la trouve.\n")

S("Ice Walls melt after some time has passed.", "Les murs de glace fondent après un certain temps.")

S("A natural terrain feature of the Desert.", "Une caractéristique naturelle du désert.")

S("You can go inside the Magic Mirror, and produce some mirror images to help you.",
  "Vous pouvez entrer dans le Miroir Magique et produire des images miroirs pour vous aider.")

S("Tiny droplets of magical water. You see images of yourself inside them. "
  "Go inside the cloud, to make these images help you.",
  "De minuscules gouttes d'eau magique. Vous voyez vos reflets à l'intérieur. "
  "Entrez dans le nuage, pour que ces images vous aident.")

S("A device that attracts sandworms and other enemies. You need to activate it.",
  "Un appareil qui attire les vers des sables et d'autres ennemis. Vous devez l'activer.")

S("A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it.",
  "Un tas de bois qui peut être utilisé pour faire du feu. Tout est déjà là, il faut juste toucher pour l'allumer.")

S("An ancient grave.", "Une ancienne tombe.")

S("A fresh grave. Necromancers like those.", "Une tombe fraîche. Les Nécromanciens les apprécient.")

S("A piece of architecture typical to R'Lyeh.", "Un élément d'architecture typique de R'Lyeh.")

S("An impassable lake in Cocytus.", "Un lac infranchissable dans le Cocytos.")

S("You can walk on it... but beware.", "Vous pouvez marcher dessus... mais prenez garde.")

S("It was a floor... until something walked on it.", "C'était autrefois un sol... jusqu'à ce que quelque chose ait marché dessus.")

S("This land is a quick gateway to other lands. It is very easy to find other lands "
  "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
  "As long as you have found enough treasure in their native lands, you can "
  "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
  "and other land types bring magical orbs.\n\n"
  "A special treasure, Hyperstone, can be found on the Crossroads, but only "
  "after you have found 10 of every other treasure.",
  "Ce pays est un accès rapide à d'autres pays. Il est très facile de trouver d'autres pays "
  "depuis les Croisements. Ce qui veut dire que vous trouverez des monstres d'autres pays ici !\n\n"
  "Dès lors que vous avez trouvé assez de trésors dans les pays d'origine, vous trouverez "
  "des objets magiques dans les Croisements. Le Pays des Miroirs amène des miroirs et nuages, "
  "et les autres pays des orbes magiques.\n\n"
  "Un trésor spécial, l'Hyperpierre, peut être trouvé dans les Croisements, mais seulement après "
  "avoir rassemblé 10 de tous les autres trésors.")

S("A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms.",
  "Un pays chaud, plein de dunes de sable, d'Épice mystérieuse, et de vers des sables géants et dangereux.")

S("A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
  "melt, even if you don't want it.",
  "Un pays très froid, plein de murs de glace. Votre simple présence fait fondre les murs, "
  "même si vous ne le voulez pas.")

S("A land filled with huge ivy plants and dangerous animals.",
  "Un pays plein de plantes de lierre géantes et d'animaux dangereux.")

S("A strange land which contains mirrors and mirages, protected by Mirror Rangers.",
  "Un pays étrange qui contient des miroirs et des mirages, protégés par les Gardiens des Miroirs.")

S("All the monsters you kill are carried to this strange land, and buried. "
  "Careless Rogues are also carried here...",
  "Tout les que vous tuez sont transportés vers cet étrange pays, et enterrés. "
  "Les vagabonds imprudents sont aussi emportés là-bas...")

S("An ancient sunken city which can be reached only when the stars are right.\n\n"
  "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "Une ancienne cité engloutie qui ne peut être atteinte que lorsque que les étoiles sont alignées.\n\n"
  "Vous trouverez des Temples de Cthulhu à R'Lyeh après avoir ramassé cinq statues de Cthulhu.")

S("A land filled with demons and molten sulphur. Abandon all hope ye who enter here!",
  "Un pays rempli de démons et de soufre en fusion. Vous qui entrez, abandonnez toute espérance !")

S("This frozen lake is a hellish version of the Icy Land. Now, your body heat melts the floor, not the walls.",
  "Ce lac gelé est une version infernale du Pays des Glaces. Désormais votre température corporelle fait "
  "fondre le sol au lieu des murs.")

S("A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
  "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
  "the floor.\n",
  "Un pays où il est impossible de s'arrêter car chaque morceau du sol est extrêmement instable. Seul les "
  "monstres capable de courir éternellement peuvent survivre ici et seules les plumes de phénix sont assez "
  "légères pour ne pas faire tomber le sol\n")

S("Affects looks and grammar", "Affecte l'apparence et grammaire")

S("first joystick: movement threshold", "Joystick 1: seuil de mouvement")
S("first joystick: execute movement threshold", "Joystick 1: exécuter le seuil de mouvement")
S("second joystick: pan threshold", "Joystick 2: seuil de déroulement")
S("second joystick: panning speed", "Joystick 2: vitesse de déroulement")
S("%The1 is frozen!", "%Le1 est gelé%e1 !")
S("%The1 burns!", "%Le1 brûle !")
S("message flash time", "Le flash info")

S("skin color", "couleur de peau")
S("weapon color", "couleur de l'arme")
S("hair color", "couleur des cheveux")
S("dress color", "couleur des vêtements")
S("Shift=random, Ctrl=mix", "Shift=aléatoire, Ctrl=mélange")

S("Euclidean mode", "Mode euclidien")
S("Return to the hyperbolic world", "Retour au monde hyperbolique")
S("go back", "retour")
S("Choose from the lands visited this game.", "Choisir parmi les pays visités dans ce jeu.")
S("Scores and achievements are not", "Les scores et accomplissements ne sont ")
S("saved in the Euclidean mode!", "pas sauvegardés en mode euclidien !")

// Android buttons
S("MOVE", "BOUGER")
S("BACK", "RETOUR")
S("DRAG", "TIRER")
S("INFO", "INFO")
S("MENU", "MENU")
S("QUEST", "QUÊTE")
S("HELP", "AIDE")
S("NEW", "NOUVEAU")
S("PLAY", "JOUER")
S("SHARE", "PARTAGER")
S("HyperRogue for Android", "HyperRogue pour Android")
S("Date: %1 time: %2 s ", "Date: %1 temps: %2 s ")
S("distance: %1\n", "distance: %1\n")
S("Cheats: ", "Triche: ")
S("Score: ", "Points: ")
S("Kills: ", "Kills: ")
S("Retrieving scores from Google Leaderboards...", "En train de récupérer les scores de des classements de Google...")
S("Scores retrieved.", "Records récupérés.")

S("Your total treasure has been recorded in the Google Leaderboards.", "Votre trésors total a été enregistré dans les classements Google.")
S("You have improved your total high score on Google. Congratulations!", "Vous avez amélioré votre record total sur Google ! Félicitations !")
S("You have improved your '%1' high score on Google!", "Vous avez amélioré votre record %de1 sur Google !")
S("This has been recorded in the Google Leaderboards.", "Ceci a été enregistrer dans les classements Google.")

// this text changed a bit:


S("Ever wondered how some boardgame would look on the hyperbolic plane? "
  "I wondered about Go, so I have created this feature. Now you can try yourself!\n"
  "Enter = pick up an item (and score), space = clear an item\n"
  "Other keys place orbs and terrain features of various kinds\n"
  "In the periodic editor, press 0-4 to switch walls in different ways\n",
  "Vous êtes-vous déjà demandé à quoi ressemblerait un jeu de plateau dans le plan hyperbolique ? "
  "Je me suis posé la question pour le Go et j'ai crée cette fonctionnalité. Maintenant vous pouvez essayer !\n"
  "Entrer = ramasser un objet (et les points), Espace = supprimer un objet\n"
  "D'autres touches placent les orbes et des terrains\n"
  "Dans l'éditeur périodique, appuyer sur 0-4 pour échanger les murs de différentes manières\n ")

S("Periodic Editor", "Éditeur Périodique")
// also translate this line:
// "In the periodic editor, press 0-4 to switch walls in different ways\n",
// "Dans l'éditeur périodique, appuyer sur 0-4 pour échanger les murs de différentes manières"\n"

S("Collect %1 $$$ to access even more lands", "Rassemblez %1 $$$ pour accéder à encore plus de pays.")

// Emerald Mine
// ------------

N("Emerald Mine", GEN_F, "Mine d'Émeraudes", "Mines d'Émeraudes", "dans la Mine d'Émeraudes")
N("Pikeman", GEN_M, "Piquier", "Piquiers", "Piquier")
N("Flail Guard", GEN_M, "Garde au Fléau", "Gardes au Fléau", "Gardes au Fléau")
N("Miner", GEN_M, "Mineur", "Mineurs", "Mineurs")
N("Emerald", GEN_F, "Émeraude", "Émeraudes", "Émeraudes")

Orb("the Mind", "de l'Esprit")

S("Evil people are mining for emeralds in this living cave. "
  "It does not grow naturally, but it is dug out in a regular "
  "pattern, which is optimal according to the evil engineers.",
  "Des gens maléfiques minent des émeraudes dans cette cave vivante. "
  "Elles ne poussent pas naturellement, mais sont extraites suivant un "
  "motif régulier, optimal d'après les ingénieurs maléfiques.")

S("Miners have special tools for dealing with the Living Cave. "
  "When they die, these tools activate, destroying the living cave "
  "around them.",
  "Les Mineurs ont des outils spéciaux pour creuser la Cave Vivante. "
  "A leur mort, ces outils s'activent, détruisant la cave vivant alentours.")

S("When Pikemen move, they attack all cells which are now adjacent to them. "
  "Luckily, they can be killed in the same way.\n\n"
  "They never move if this would attack their friends.",
  "Quand les Piquiers bougent, ils attaquent toutes les cellules adjacentes à eux. "
  "Heureusement, ils peuvent être tués de la même manière.\n\n"
  "Ils ne bougent pas s'ils risquent d'attaquer leurs amis.")

S("This guard of the Emerald Mine is wielding a huge flail. "
  "You cannot attack him directly, as the flail would still hit you then. "
  "Luckily, you have learned a trick: if you step away from him, "
  "he will hit himself with the flail!",

  "Ce garde de la Mine d'Émeraudes porte un fléau gigantesque. "
  "Vous ne pouvez l'attaquer directement car son fléau vous frapperait. "
  "Heureusement, vous avez appris une astuce : "
  "Lorsque vous vous éloignez de lui, il se frappe lui-me avec le fléau !")

S("A precious green gem from the Emerald Mines.", "Une gemme précieuse des Mines d'Emeraudes.")

S("%The1 pierces %the2!", "%Le1 transperce %le2 !")

S("Make him hit himself by walking away from him.",
  "Faites le se frapper lui-même en vous éloignant de lui.")

S("This orb allows you to instantly kill a non-adjacent enemy by clicking it. "
  "Each use drains 30 charges.",
  "Cet Orbe permet de tuer instantanément un ennemi à distance en le cliquant. "
  "Chaque utilisation consomme 30 charges.")

S("%The1 is immune to mental blasts!", "%Le1 est immunisé contre les attaques mentales !")
S("You kill %the1 with a mental blast!", "Vous tuez %le1 avec une attaque mentale !")

// Vineyard
// --------

N("Vineyard", GEN_M, "Vignoble", "Vignobles", "dans le Vignoble")
N("Vine Beast", GEN_F, "Bête des Vignes", "Bêtes des Vignes", "Bête des Vignes")
N("Vine Spirit", GEN_M | GENF_ELISION, "Esprit des Vignes", "Esprits des Vignes", "Esprit des Vignes")
N("vine", GEN_F, "vigne", "vignes", "vigne")
N("partial fire", GEN_M, "feu partiel", "feux partiels", "feu partiel")
N("Wine", GEN_M, "Vin", "Vins", "Vin")

S("This cell is partially on fire.", "Cette cellule est partiellement en feu.")

S("The Vineyard is filled with vines. A very dense pattern of straight lines here...\n\n"
  "Vine Beasts and Vine Spirits change vine cells to grass, and vice versa.",
  "Le Vignoble est rempli de vignes. Un réseau très dense de lignes droites...\n\n"
  "Les Bêtes des Vignes et les Esprits des Vignes changent les cellules de vigne en herbe, et inversement.")

S("A vine is growing here... but only on a half of the cell. How is that even possible?!"
  "Most monsters cannot move from this cell to the cell containing the other half. "
  "Vine spirits can move only to the adjacent cells which are also adjacent to the "
  "other half.",
  "Une vigne pousse ici... mais seulement sur la moitié de la cellule. Comment est-ce possible ?!"
  "La plupart des monstres ne peuvent pas se déplacer de cette cellule à celle contenant l'autre moitié. "
  "Les Esprits des Vignes peuvent seulement aller sur les cellules adjacentes, qui sont aussi adjacentes à "
  "la cellule contenant l'autre moitié. ")

S("A beast made of vines!\n\n"
  "Vine Beasts turn into vines when they die.",
  "Une bête faite de vignes !\n\n"
  "Les Bêtes de Vignes se transforment en vignes à leur mort.")

S("A spirit living in the vines!\n\n"
  "Vine Spirits destroy the vines when they die.",
  "Un esprit vivant dans les vignes !\n\n"
  "Les Esprits des Vignes détruisent les vignes à leur mort.")

S("Wine grown under hyperbolic sun would be extremely prized in your home location.",
  "Le vin vieilli sous le Soleil hyperbolique aurait beaucoup de valeur dans votre pays natal.")

S("This orb allows one to pass through all kinds of walls and chasms.",
  "Cet orbe permet de passer à travers toutes sortes de murs et gouffres.")

S("You cannot attack through the Vine!",
  "Vous ne pouvez pas attaquer à travers la Vigne !")

Orb("Aether", "de l'Ether")

// Dead Caves
// ----------

N("Dead Cave", GEN_F, "Cave Morte", "Caves Mortes", "dans les Caves Mortes")
N("Dark Troll", GEN_M, "Troll Sombre", "Trolls Sombres", "Troll Sombre")
N("Earth Elemental", GEN_M | GENF_ELISION, "Élémental de Terre", "Élémentaux de Terre", "Élémental de Terre")
N("dead wall", GEN_M, "mur mort", "murs morts", "mur mort")
N("dead floor", GEN_M, "sol mort", "sols morts", "sol mort")
N("rubble", GEN_M | GENF_PLURALONLY, "gravats", "gravats", "gravats")
N("Silver", GEN_M | GENF_ELISION, "Argent", "Argents", "Argent")

S("A precious metal from the Dead Caves.", "Un métal précieux des Caves Mortes.")

S("A Troll without the power of Life.", "Un Troll sans le pouvoir de Vie.")

S("A rare unliving construct from the Dead Caves. "
  "It instantly destroys cave walls next to its path, and also leaves "
  "an impassable wall behind it. You suppose that this impassable wall helps it to "
  "escape from some threats. You hope you won't meet these threats...",

  "Un rare construct non-mort originaire des Caves Mortes. "
  "Il détruit instantanément les murs de cave à coté de son chemin, et laisse aussi "
  "un mur infranchissable derrière lui. Vous supposez que ce mur l'aide à échapper "
  "à des menaces. Vous espérez ne pas rencontrer ces menaces...")

S("%The1 punches %the2!", "%Le1 frappe %le2 !")
S("%The1 is destroyed by the forces of Life!", "%Le1 est détruit par le pouvoir de la Vie !")
S("Somehow, this cave has not received the spark of Life yet.",
  "Cette cave n'a apparemment pas encore reçu l'étincelle de la Vie.")

// Hive
// ----

N("Hive", GEN_F, "Ruche", "Ruches", "dans la Ruche")
N("Red Hyperbug", GEN_M | GENF_ELISION, "Hyperinsecte Rouge", "Hyperinsectes Rouges", "Hyperinsecte Rouge")
N("Green Hyperbug", GEN_M | GENF_ELISION, "Hyperinsecte Vert", "Hyperinsectes Verts", "Hyperinsecte Vert")
N("Blue Hyperbug", GEN_M | GENF_ELISION, "Hyperinsecte Bleu", "Hyperinsectes Bleus", "Hyperinsecte Bleu")
N("Royal Jelly", GEN_F, "Gelée Royale", "Gelée Royale", "Gelée Royale")
N("weird rock", GEN_F, "pierre bizarre", "pierres bizarres", "pierres bizarres")

Orb("Invisibility", "d'Invisibilité")

S("A weirdly colored rock. Hyperentomologists claim that the "
  "Hyperbug armies use these rocks to navigate back home after a victorious battle.",
  "Une pierre colorée bizarre. Les hyperentomologues affirment que les armées d'hyperinsectes "
  "utilisent ces pierres pour trouver le chemin de la maison après une bataille victorieuse.")

S("%The1 fights with %the2!", "%Le1 combat %le2 !")

S("The Hive is filled with Hyperbugs. They are huge insects which look a bit like "
  "ants, a bit like bees, and a bit like roaches. "
  "They live in circular nests, and an army of Hyperbugs will attack any intruder, "
  "including you and enemy Hyperbugs. Will you manage to get to the "
  "heart of such a nest, and get the precious Royal Jelly?",
  "La Ruche est pleine d'Hyperinsectes. Ce sont des insectes géants qui ressemblent "
  "un peu aux fourmis, un peu aux abeilles, et un peu aux cafards. "
  "Ils vivent dans des nids circulaires, et une armée d'Hyperinsectes attaquerons tout intrus. "
  "Arriverez-vous au cœur d'un nid pour récolter la précieuse Gelée Royale ?")

S("This is what Hyperbug Queens eat. Very tasty and healthy.",
  "C'est ce que mangent les Reines Hyperinsectes. Délicieux et bon pour la santé.")

S("When you have this Orb, most monsters won't see you, unless "
  "you are standing still, attacking, or picking up items.",
  "Quand vous avez cet Orbe, la plupart des monstres ne vous voient pas, "
  "sauf si vous vous arrêtez, attaquez, ou ramassez des objets.")

Orb("Earth", "de Terre")

S("This orb lets you go through living walls. It also has powers in some of the other lands.",
  "Cet orbe permet de traverser les murs vivants. Elle a d'autres pouvoirs dans certains autres pays.")

// Land of Power
// -------------

N("Land of Power", GEN_M, "Pays du Pouvoir", "Pays du Pouvoir", "dans le Pays du Pouvoir")
N("Witch Apprentice", GEN_F | GENF_ELISION, "Apprentie Sorcière", "Apprenties Sorcières", "Apprentie Sorcière")
N("Speed Witch", GEN_F, "Sorcière de la Vitesse", "Sorcières de la Vitesse", "Sorcière de la Vitesse")
N("Flash Witch", GEN_F, "Sorcière du Tonnerre", "Sorcières du Tonnerre", "Sorcière du Tonnerre")
N("Fire Witch", GEN_F, "Sorcière du Feu", "Sorcières du Feu", "Sorcière du Feu")
N("Aether Witch", GEN_F, "Sorcière de l'Ether", "Sorcières de l'Ether", "Sorcière de l'Ether")
N("Winter Witch", GEN_F, "Sorcière de l'Hiver", "Sorcières de l'Hiver", "Sorcière de l'Hiver")
N("Evil Golem", GEN_M, "Golem Maléfique", "Golems Maléfiques", "Golem Maléfique")
N("Powerstone", GEN_F, "Pierre de Pouvoir", "Pierres de Pouvoir", "Pierre de Pouvoir")
N("crystal cabinet", GEN_F, "vitrine de cristal", "vitrines de cristal", "vitrine de cristal")

S("When you have this Orb, you will leave a trail of fire behind you.",
  "Quand vous avez cet Orbe, vous laissez un sillage de feu derrière vous.")

S("A Witch without any special powers. But watch out! She will "
  "pick up any basic Orbs on her path, and use their powers.",
  "Une Sorcière sans pouvoirs spéciaux. Mais attention ! "
  "Elle ramassera toutes les Orbes basiques sur son chemin, et utilisera leurs pouvoirs.")

S("A Witch with a Speed spell. She moves twice as fast as you. Unless you "
  "have an Orb of Speed too, of course!",
  "Une Sorcière avec un Sort de Vitesse. Elle bouge deux fois plus vite que vous. "
  "A moins d'avoir aussi un Orbe de Vitesse, bien sûr !")

S("A Witch with a Fire spell. She will leave a trail of fire behind her.",
  "Une Sorcière avec un Sort de Feu. Elle laissera un sillage de feu derrière elle.")

S("A Witch with a Winter spell. She is able to move through fire.",
  "Une Sorcière avec un Sort d'Hiver. Elle peut traverser le feu.")

S("A Witch with an Aether spell. She is able to move through fire and walls.",
  "Une Sorcière avec un Sort d'Ether. Elle peut traverser le feu et les murs.")

S("Somebody has summoned these evil constructs with a magical process.",
  "Quelqu'un a invoqué ces constructs maléfiques par in rituel magique.")

S("The Land of Power is filled with everburning fire, magical Orbs, and guarded by "
  "witches and golems. There are basic orbs lying everywhere, and more prized ones "
  "are kept in crystal cabinets.\n\n"
  "Witches are allowed to use all the powers of the "
  "basic orbs against intruders. These powers never expire, but a Witch "
  "can use only one power at a time (not counting Orbs of Life).\n\n"
  "Witches and Golems don't pursue you into other Lands. Also, most Orb powers "
  "are drained when you leave the Land of Power.",
  "Le Pays du Pouvoir est plein de flammes éternelles, d'Orbes magiques, et gardé par"
  "des sorcières et des golems. Il y a des orbes basiques partout et les plus précieux "
  "sont rangés dans des vitrines de cristal.\n\n"
  "Les sorcières peuvent utiliser tout les pouvoirs des orbes basiques contre les intrus "
  "Ces pouvoirs n'expirent pas, mais elles peuvent n'en utiliser qu'un à la fois (sans "
  "compter les Orbes de Vie). Les sorcières et les golems ne vous poursuivent pas dans les autres Pays."
  "La plupart des pouvoirs des Orbes sont épuisés lorsque vous quittez le Pays du Pouvoir.")

S("Witches use these crystal cabinets to protect Powerstones, as well as the more "
  "expensive Orbs. They are partially protected from thieves: they are too strong "
  "to be smashed by conventional attacks, and if you try to steal the item "
  "using an Orb of Aether, your Aether power will be completely drained.",
  "Les sorcières utilisent des vitrines de cristal pour protéger les Pierres de Pouvoir, "
  "ainsi que les Orbes les plus chers. Elles sont partiellement protégées des voleurs : "
  "elles sont trop solides pour céder aux attaques conventionnelles, et si vous essayez de voler "
  "l'objet avec un Orbe d'Ether votre pouvoir d'Ether sera entièrement drainé.")

S("A Stone from the Land of Power. You are not sure what it is exactly, but "
  "as the Powerstones are kept in crystal cabinets, they are surely valuable.",
  "Une Pierre du Pays du Pouvoir. Vous n'êtes pas sûre de ce que c'est exactement, "
  "mais comme elles sont gardées dans des vitrines, elles doivent avoir de la valeur.")

Orb("Fire", "de Feu")

// achievements:

// x "Head Witch", "Have 10 Orbs active at the same time."
// "Enemy Number One", "Have an army of 1000 hyperbugs pursue you."
// x "Lifebringer", "Brought Life into the Dead Caves."
// "Wall Gardener", "Make a living wall grow outside of a cavernous region."
// x "Master Swordsman", "Defeat a Hedgehog Warrior, a Pikeman, and a Flail Guard at the same time."
// x "Master of Melee", "Defeat five monsters in melee at the same time."

// for collection achievements:
// provide the required cases for descriptions:
// Spice, Royal Jelly, Silver, Powerstone, and Wine
// achievement names:
// Master of the Hive, Master of Power, Master of Vineyard, Master of Dead Caves, Master of the Mine
// "Hyperbug", "Hyperbug Warrior", "Hyperbug Queen",

// for the store description:

// (currently, five more lands are included!)

// lines forgotten previously

S("Your Aether power has expired! RUN!", "Votre pouvoir d'Ether s'est épuisé ! Fuyez !!!")

S("%The1 tricks %the2.", "%Le1 piège %le2.")
S("You trick %the1.", "Vous piégez %le1.")

S("A Witch with a Flash spell. Very dangerous!\n\nLuckily, she never uses the spell if it "
  "would kill her friends. She could destroy an Evil Golem, though.",
  "Une Sorcière avec un Sort de Tonnerre. Très dangereuse !\n\n"
  "Heureusement elle n'utilise jamais le sort s'il risque de tuer ses amies. Elle peut néanmoins détruire un Golem Maléfique.")

S("Your Aether powers are drained by %the1!", "%Le1 draine vos pouvoirs d'Ether !")
S("As you leave, your powers are drained!", "Alors que vous partez, vos pouvoirs sont drainés.")
S("%The1 picks up %the2!", "%Le1 ramasse %le2 !") // currently only Witches, so only female form

S("You can right click any element to get more information about it.\n\n",
  "Vous pouvez cliquer-droit n'importe quel élément pour avoir des informations à son sujet.\n\n")

S("The Hyperbugs are fighting!", "Les Hyperinsectes combattent !")

S("","") N("",GEN_N,"","","")

S("Cheat-changed the display.", "Affichage changé avec la triche.")
S("Dead floor, with some rubble.", "Sol mort, avec des gravats.")

// Camelot
// -------

N("Camelot", GEN_M | GENF_PROPER, "Camelot", "Camelot", "à Camelot")
N("wall of Camelot", GEN_M, "mur de Camelot", "murs de Camelot", "mur de Camelot")
N("moat of Camelot", GEN_F, "douve de Camelot", "douves de Camelot", "douve de Camelot")
N("Round Table", GEN_F, "Table Ronde", "Tables Rondes", "Table Ronde")
N("Knight", GEN_M, "Chevalier", "Chevaliers", "Chevalier")
N("Holy Grail", GEN_M, "Saint Graal", "Saints Graals", "Saint Graal")

S("The Knights of the Round Table are the greatest warriors of these lands. "
  "They are not very inventive with names though, as they call each of their "
  "castles Camelot. "
  "You are probably worthy of joining them, but they will surely give you "
  "some quest to prove yourself...\n\n"
  "Each castle contains a single treasure, the Holy Grail, in the center. "
  "The radius of the Round Table is usually 28, but after you find a Holy Grail "
  "successfully, each new castle (and each Round Table) you find will be bigger.",

  "Les Chevaliers de la Table Ronde sont les plus grands guerriers de ces pays. "
  "Mais ils ne sont pas très créatifs pour les noms, et appellent tout leurs châteaux Camelot. "
  "Vous êtes probablement dignes de les rejoindre, mais ils vous donneront probablement "
  "une quête pour prouver votre valeur...\n\n"
  "Chaque château contient un seul trésor, le Saint Graal, au centre. "
  "Le rayon de la Table Ronde est habituellement 28, mais après avoir trouvé un Saint Graal "
  "chaque nouveau château (et chaque Table Ronde) sera plus grand.")

S("It would be impolite to land on the table!",
  "Il serait impoli d'atterrir sur la table !")
S("You jump over the table!", "Vous sautez par-dessus la table !")
S("That was not a challenge. Find a larger castle!",
  "Ce n'était pas si dur. Allons chercher un plus grand château !")
S("The Knights congratulate you on your success!",
  "Les Chevaliers vous félicitent pour votre succès !")
S("The Knights laugh at your failure!",
  "Les Chevaliers se rient de votre échec !")
S("The Knights stare at you!", "Les Chevaliers vous fixent du regard...")
S("Come on, this is too easy... find a bigger castle!",
  "Allez, c'est trop facile... trouvez un plus grand château !")
S("The Knights wish you luck!", "Les Chevaliers vous souhaitent bonne chance !")

S("Congratulations! You have found the Holy Grail!",
  "Félicitations ! Vous avez trouvé le Saint Graal !")

S("\"I would like to congratulate you again!\"", "\"Je voudrais encore vous féliciter !\"")
S("\"Find the Holy Grail to become one of us!\"", "\"Trouvez le Saint Graal pour devenir l'un d'entre nous !\"")
S("\"The Holy Grail is in the center of the Round Table.\"", "\"Le Saint Graal est au centre de la Table Ronde.\"")
S("\"I enjoy watching the hyperbug battles.\"", "\"J'aime regarder les batailles d'hyperinsectes.\"")
S("\"Have you visited a temple in R'Lyeh?\"", "\"Avez vous visité un temple à R'Lyeh?\"")
S("\"Nice castle, eh?\"", "\"Joli château, pas vrai ?\"")

// Temple
// ------

N("Temple of Cthulhu", GEN_M, "Temple de Cthulhu", "Temples de Cthulhu", "dans le Temple de Cthulhu")
N("big statue of Cthulhu", GEN_F, "grande statue de Cthulhu", "grandes statues de Cthulhu", "grande statue de Cthulhu") // FIXME: 3/=4
N("Grimoire", GEN_M, "Grimoire", "Grimoires", "Grimoire")
N("Cult Leader", GEN_M, "Chef du Culte", "Chefs du Culte", "Chef du Culte")

S("The temple of Cthulhu consists of many concentric circles of columns. "
  "You will surely encounter many Cultists there, who believe that a pilgrimage "
  "to the inner circles will bring them closer to Cthulhu himself, and Grimoires "
  "which surely contain many interesting secrets.\n\n"
  "The circles in the temple of Cthulhu are actually horocycles. They are "
  "infinite, and there is an infinite number of them.",

  "Le temple de Cthulhu formé de nombreux cercles concentriques de colonnes. "
  "Vous rencontrerez sûrement de nombreux Cultistes ici, qui croient qu'un pèlerinage "
  "vers les cercles intérieurs les rapprochera de Cthulhu lui-même, et des Grimoires "
  "qui contiennent sûrement de nombreux secrets.\n\n"
  "Les cercles du temple sont en réalité des horocycles. Ils sont infinis et il y en a "
  "un nombre infini.")

S("The Grimoires contain many secrets of the Great Old Ones. "
  "Each new inner circle in the Temple of Cthulhu contains new Grimoires, with new secrets. "
  "You hope to read them when you return home, and to learn many things. "
  "The knowledge is valuable to you, but it is rather pointless to try to get "
  "several copies of the same Grimoire...",
  "Les grimoires contiennent les secrets des Grands Anciens. "
  "Chaque nouveau cercle intérieur du Temple de Cthulhu contient de nouveaux Grimoires, "
  "avec de nouveaux secrets. "
  "Ce savoir est précieux pour vous, mais il est plutôt inutile de ramasser plusieurs copies "
  "du même Grimoire...")

S("These statues of Cthulhu are too large to carry, and they don't look too "
  "valuable anyway. Most monsters will never go through them... they probably have "
  "their reasons. But you can go! When you move into the cell containing "
  "a statue, you push the statue to the cell you left.\n",

  "Ces statues de Cthulhu sont trop larges pour être transportées, mais elles "
  "n'ont pas l'air précieuses. La plupart des monstres ne les traverseront jamais... "
  "Ils ont probablement leurs raisons. Mais vous le pouvez ! Quand vous allez vers une "
  "case contenant une statue, vous la poussez sur la case que vous quittez.")

S("These Cultists can push the statues, just like you.",
  "Ces Cultistes peuvent pousser les statues, comme vous.")

S("You already have this Grimoire! Seek new tomes in the inner circles.",
  "Vous avez déjà ce Grimoire ! Cherchez de nouveau tomes dans les cercles intérieurs.")

S("You push %the1 behind you!", "Vous poussez %le1 derrière vous.")

// More texts
// ----------

S("Your total kills", "Votre total de tués")

S("In most lands, more treasures are generated with each enemy native to this land you kill. "
  "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
  "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
  "but are not counted in the total kill count.",

  "Dans la plupart des pays, plus de trésors sont générés avec chaque ennemis natifs tués. "
  "Néanmoins, 100 ennemis tués est un prérequis pour entrer dans la Ruche et le Cimetière.\n\n"
  "Les créatures amicales et les morceaux de monstres (comme du Lierre) apparaissent sur la liste, "
  "mais ne sont pas comptés dans le total de tués.")

S("There must be some temples of Cthulhu in R'Lyeh...",
  "Il doit y avoir quelques temples de Cthulhu à R'Lyeh...")

S("You overheard Hedgehog Warriors talking about emeralds...",
  "Vous entendez les Guerrier Hérissons parler d'émeraudes...")

S("You overhear miners talking about a castle...",
  "Vous entendez des mineurs parler d'un château...")

S("A castle in the Crossroads...", "Un Château dans les Croisements...")

/*
  S("You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "Vous pouvez trouver des Temples de Cthulhu à R'Lyeh, après avoir collecté cinq statues de Cthulhu.")
*/

S("You have to escape first!", "Vous devez vous échapper d'abord !")
S("There is not enough space!", "Pas assez de place !")

S("Customize character", "Personnaliser le caractère")
S("gender", "genre")

//

Orb("the Dragon", "le Dragon")

S("This Orb allows you to throw fire, just like the Fire Cultists.\n\n"
  "Each fire drains 5 charges. You are not allowed to throw fire into adjacent cells.",
  "Cet Orbe permet de jeter du feu, comme les Cultistes de Feu.\n\n"
  "Chaque feu draine 5 charges. Vous ne pouvez pas jeter du feu sur les cases voisines.")

S("You throw fire!", "Vous jetez du feu !")

Orb("Trickery", "de Ruse")

N("Illusion", GEN_F | GENF_ELISION, "Illusion", "Illusions", "Illusion")

S("This Orb allows you to create illusions of yourself. Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.\n\n"
  "Each illusion takes 5 charges to create, and one extra charge "
  "per turn. You can also click your illusion to take it away, restoring 4 charges.\n\n"
  "If you have both Orb of Teleport and Orb of Trickery, Illusion is cast "
  "first -- you can then teleport on your Illusion to switch places with it.",
  "Cet Orbe permet de créer des illusions à votre image. Les Illusions sont attaquées "
  "par la plupart des monstres.\n\n"
  "Chaque Illusion prends 5 charges à sa création, et une supplémentaire par tour. "
  "Vous pouvez aussi cliquer sur l'illusion pour la retirer et récupérer 4 charges.\n\n"
  "Si vous avez à la fois l'Orbe de Téléportation et de Ruse, l'Illusion est jetée "
  "en premier -- vous pouvez vous téléporter sur votre Illusion pour échanger vos places.")

S("Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.",
  "Les Illusions sont attaquées comme vous, les Marteleurs et vos alliés.")

S("You create an Illusion!", "Vous créer une Illusion !")
S("You take the Illusion away.", "Vous retirez l'Illusion.")

S("You cannot target that far away!", "Trop loin !")
S("You cannot target that close!", "Trop près !")
S("Cannot teleport on a monster!", "Vous ne pouvez pas vous téléporter sur un monstre !")
S("Cannot teleport on an item!", "Vous ne pouvez pas vous téléporter sur un objet !")
S("Cannot teleport here!", "Vous ne pouvez pas vous téléporter ici !")
S("Cannot cast illusion on an item!", "Vous ne pouvez pas créer une illusion sur un objet !")
S("Cannot cast illusion here!", "Vous ne pouvez pas créer une illusion ici !")
S("Cannot cast illusion on a monster!", "Vous ne pouvez pas créer une illusion sur un monstre !")
S("Cannot throw fire there!", "Vous ne pouvez pas jeter du feu ici !")

// teleport -> target
// You cannot target that close!

S("or ESC to see how it ended", "ou ESC pour voir comment ça s'est fini")
S("high contrast", "augmenter le contraste")
S("draw the heptagons darker", "noircir les heptagones")
S("targetting ranged Orbs Shift+click only", "tirer avec les Orbes à distance avec Shift-click")
S("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc.",
  "Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc.")
S("also hold Alt during the game to toggle high contrast",
  "maintenir ALT appuyé durant le jeu pour augmenter le contraste")

// Crossroads II
// -------------

N("Crossroads II", GEN_M, "Croisement II", "Croisements II", "dans les Croisements II")

S("An alternate layout of the Crossroads. It is more dense and more regular, "
  "although you won't find the castle of Camelot here.",

  "Des Croisements disposés différemment, de manière plus dense et régulière.")

// Caribbean
// ---------

N("Caribbean", GEN_F, "Caraïbe", "Caraïbes", "dans les Caraïbes")

N("Pirate", GEN_M, "Pirate", "Pirates", "Pirate")
N("Shark", GEN_M, "Requin", "Requins", "Requin")
N("Parrot", GEN_M, "Perroquet", "Perroquets", "Perroquet")
N("Pirate Treasure", GEN_M, "Trésor de Pirate", "Trésors de Pirate", "Trésor de Pirate")
N("Compass", GEN_F, "Boussole", "Boussoles", "Boussole")

N("sea", GEN_F, "Mer", "Mers", "Mer")
N("boat", GEN_M, "bateau", "bateaux", "bateau")
N("island", GEN_F | GENF_ELISION, "île", "îles", "île")
N("tree", GEN_M | GENF_ELISION, "arbre", "arbres", "arbre")

Orb("Time", "du Temps")

S("This dangerous sea contains nasty sharks and pirates. ",
  "Cette mer dangereuse contient des requins et des pirates.")

S("The islands of Caribbean are infinite and circular, and "
  "the pirates probably have hidden their treasures somewhere far from the coast.",
  "Les îles des Caraïbes sont infinies et circulaires, et "
  "les pirates ont probablement caché leur trésor quelque part loin de la côte.")

S("Hyperbolic pirates do not need huge ships, since so many lands to conquest "
  "are so close. These small boats are enough for them.\n\n"
  "Boats allow you to go through water. If you are in a boat, you can move into "
  "a water cell (and the boat will come with you).",

  "Les pirates hyperboliques n'ont pas besoin de grands navires, car de nombreux pays "
  "à conquérir sont si proches. Ces petits bateaux leur suffisent.\n\n"
  "Les bateaux permettent de traverser l'eau. Si vous êtes à bord d'un bateau, vous pouvez "
  "vous déplacer sur une case d'eau (et le bateau viendra avec vous).")

S("The forests of Caribbean are too dense to be traversed by humans, "
  "and they are hard to burn. Many colorful parrots can be found there.",
  "Les forêts des Caraïbes sont trop denses pour être traversées par des humains, "
  "et difficiles à brûler. Elles sont peuplées de nombreux perroquets colorés.")

S("Ye wonder where did th' Pirates find all these riches...",
  "Vous vous demandez où les Pirates trouvent toutes ces richesses...")

S("The hyperbolic pirates have no use for treasure maps. However, they have found "
  "out that a compass points to the center of the island. So they just go as "
  "far towards the center as they can, and hide their treasure there.",

  "Les pirates hyperboliques n'ont pas besoin de carte au trésor. Mais ils ont inventé "
  "une boussole qui pointe vers le centre de l'île. Alors ils vont juste aussi loin "
   "vers le centre qu'ils peuvent, et cachent leur trésor là.")

S("Just a nasty shark.", "Juste un vilain requin.")

S("Parrots feel safe in the forests of Caribbean, so they "
  "never leave them. But they will help the Pirates by attacking the intruders.",

  "Les perroquets se sentent en sécurité dans les forêts des Caraïbes, "
  "alors ils ne les quittent jamais. Mais ils aideront les pirates en attaquant les intrus.")

S("Just a typical hyperbolic pirate.", "Juste un pirate hyperbolique typique.")

S("Normally, the power of most Orbs slowly fades away, even when "
  "you are not actively using them. This Orb prevents this.\n\n"

  "When you have the Orb of Time, Orbs which are not active won't lose their power. "
  "Orbs are considered active if they have a continuous power which has actually "
  "affected something in the last turn.\n\n"

  "Orbs of Shielding remain active after being activated (even if you are no longer "
  "attacked), and Orbs of Time have a bigger cap inside their native Caribbean than "
  "outside.",

  "Normalement, le pouvoir de la plupart des Orbes s’évanouit lentement, même sans les "
  "utiliser. Cet Orbe empêche cela.\n\n"

  "Quand vous avez l'Orbe du Temps, les Orbes non actifs ne perdent pas leur pouvoir. "
  "Les Orbes sont considérés actifs si elles ont un pouvoir continu qui a effectivement "
  "affecté quelque chose au tour précédent.\n\n"

  "L'Orbe de Protection reste actif après activation (même si vous n'êtes plus attaqué), "
  "et les Orbes du Temps ont une plus grande durabilité dans leur Caraïbe natale qu'à l'extérieur.")

// Achievements:

// Parrot : Find and collect Pirate Treasure.
// Caribbean Shark : Collect 10 Pirate Treasures.
// Pirate : Collect 25 Pirate Treasures.
// Master of Caribbean : Collect 50 Pirate Treasures.

// Red Rock Valley
// ---------------

N("Red Rock Valley", GEN_F, "Vallée de Roche Rouge", "Vallées de Roche Rouge", "dans la Vallée de Roche Rouge")

N("Rock Snake", GEN_M, "Serpent des Roches", "Serpents des Roches", "Serpent des Roches")
N("Rock Snake Tail", GEN_F, "Queue du Serpent des Roches", "Queues du Serpent des Roches", "Queue du Serpent des Roches")
N("Red Troll", GEN_M, "Troll Rouge", "Trolls Rouges", "Troll Rouge")
N("Red Gem", GEN_F, "Gemme Rouge", "Gemmes Rouges", "Gemme Rouge")

N("rock I", GEN_F, "Roche I", "Roches I", "Roche I")
N("rock II", GEN_F, "Roche II", "Roches II", "Roche II")
N("rock III", GEN_F, "Roche III", "Roches III", "Roche III")

Orb("Space", "de l'Espace")

S("Not enough power for telekinesis!", "Pas assez de pouvoir pour la télékinésie !")

S("This land contains high rock formations. Most of the valley is at level 0, "
  "while items are found at level 3. It is impossible to gain two or more levels, "
  "or to lose three levels, in a single move, (attacks are possible at any "
  "difference, though). Kill Red Trolls and Rock Snakes to make a cell higher.",

  "Ce pays contient de hautes formations rocheuses. La plus grande part de la vallée est au niveau 0, "
  "tandis que les objets sont au niveau 3. Il est impossible de gagner deux ou plus de niveaux, ou de "
  "perdre trois niveaux en un seul mouvement, (mais les attaques sont possibles à n'importe quelle "
  "différence). Tuez des Trolls Rouges et des Serpents des Roches pour grimper d'un niveau.")

S("A kind of Troll native to the Red Rock Valley.",
  "Une espèce de Troll originaire de la Vallée de Roche Rouge.")

S("Rock snakes are similar to Sandworms, but they move each turn. However, they "
  "only move on the hexagonal cells. They are also somewhat longer than the Sandworms.",

  "Les serpents des roches sont semblables aux vers des sables, mais avancent à chaque tour. "
  "Néanmoins ils ne vont que sur les cases hexagonales. Ils sont aussi plus longs que les vers des sables.")

S("A gem from the Red Rock Valley.", "Une Gemme de la Vallée de Roche Rouge.")

S("This Orb is able to bring faraway items to your location, even if there are "
  "monsters or barriers on the way. The cost of "
  "bringing an item (in charges) equals the square of its distance to you. Contrary "
  "to some other Orbs, usage is not allowed if you have not enough power left.",

  "Cet Orbe peut amener des objets lointains à votre position, me s'il y a des monstres "
  "ou des obstacles sur le chemin. Le coût d'amener un objet (en charges) est le carré de la "
  "distance à votre position. Contrairement à d'autres Orbes l'usage n'est pas possible "
  "s'il n'y a plus assez de pouvoir.")

// Climber : Find and collect a Red Gem.
// Red Troll : Collect 10 Red Gems.
// Rock Snake : Collect 25 Red Gems.
// Master of Red Rock : Collect 50 Red Gems.

// missing sentences

S("%The1 drowns!", "%Le1 se noie !")
S("%The1 falls!", "%Le1 tombe !")

// these were missing from the translation for some reason

S("Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.",
  "Ces lacs sont partout en Enfer... Ils ont la forme d'étoiles malfaisantes, et sont remplies de soufre en fusion.")

// Hardcore Mode
// -------------

S("hardcore mode", "mode hardcore")

S("One wrong move and it is game over!", "Un faux mouvement et c'est fini !")
S("Not so hardcore?", "pas si hardcore ?")

// Shoot'em up Mode
// ----------------

S("shoot'em up mode", "mode shoot'em up")
S("Welcome to the Shoot'em Up mode!", "Bienvenue dans le mode shoot'em up !")
S("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move", "F/[;]/Espace/Entrée/[pavé numérique 5] = tirer, WASD/IJKL/Numpad = bouger")

N("Rogue", GEN_M, "Vagabond", "Vagabonds", "Vagabond")
N("Knife", GEN_M, "Couteau", "Couteaux", "Couteau")
N("Flail", GEN_M, "Fléau", "Fléaux", "Fléau")
N("Fireball", GEN_F, "Boule de Feu", "Boules de Feu", "Boule de Feu")
N("Tongue", GEN_F, "Langue", "Langues", "Langue")

S("In the Shoot'em Up mode, you are armed with thrown Knives.",
  "En mode shoot'em up, vous êtes armé%e0 de Couteaux de jet.")
S("A simple, but effective, missile, used by rogues.",
  "Une arme de jet simple, mais efficace, utilisée par les vagabonds.")
S("This attack is likely to hit the attacker.",
  "Cette attaque peut toucher l'attaquant.")
S("This magical missile burns whatever it hits.", "Ce projectile magique brûle tout ce qu'il touche.")
S("Some monsters have long tongues, which allow them to attack enemies in nearby cells.",
  "Certains monstres ont de longues langues qui leur permettent d'attaques les cases à proximité.")

// modes for the local highscores
// ------------------------------

S(", m - mode: normal", ", m - mode : normal")
S(", m - mode: hardcore only", ", m - mode : hardcore")
S(", m - mode: shoot'em up", "m - modus : shoot'em up")

// update description for Steam:

// (Shoot'em Up mode and two more lands than in the free version, and more are planned!)

// missing lines:

S("You would get hurt!", "Vous seriez blessé !")
S("PARTIAL", "PARTIEL")

S("Cannot drop %the1 here!", "Vous ne pouvez pas jeter %le1 ici !")

// Euclidean scores
// ----------------

S(" (E:%1)", " (E:%1)")

S("You cannot attack Rock Snakes directly!", "Vous ne pouvez pas attaquer les Serpents des Roches directement !")

S("\"I am lost...\"", "\"Je suis perdu...\"")

S("You are killed by %the1!", "%Le1 vous a tué !")

// new menu for Version 7.1
// ------------------------

S("(v) menu", "menu - v")
S("return to the game", "revenir au jeu")
S("or 'v' to see the main menu", "ou 'v' pour voir le menu principal")

S("HyperRogue %1", "HyperRogue %1")
S("basic configuration", "configuration basique")
S("advanced configuration", "configuration avancée")
S("local highscores", "meilleurs scores locaux")
S("help", "aide")
S("restart game", "recommencer le jeu")
S("special game modes", "modes de jeu spéciaux")
S("save the game", "sauvegarder")
S("quit the game", "quitter")
S("review your quest", "récapitulatif de la quête")
S("review the scene", "revoir la scène")
S("game over screen", "écran Game Over")
S("continue game", "continuer")
S("play the game!", "jouer !")

// fixed descriptions for Shmup achievements
// -----------------------------------------

S("You have used less knives than ever before. Congratulations!",
  "Vous avez utilisé moins de couteaux qu'avant. Félicitations !")

S("You have improved both your real time and knife count. Congratulations!",
  "Vous avez amélioré à la fois votre temps réel et nombre de couteaux. Félicitations !")

S("%1 knives (%2)", "%1 couteaux (%2)")

// temporary
S("No description yet." ,"Pas encore de description...")

// special case when no Spice is visible
S("The sandworm explodes!", "Le ver des sables explose !")

// Ocean
// -----

// achievements:
// Reached Ocean: Find and collect an Amber.
// Albatross: Collect 10 Ambers.
// Amber Albatross: Collect 25 Ambers.
// Master of the Tides: Collect 50 Ambers.

N("Ocean", GEN_M | GENF_ELISION, "Océan", "Océans", "dans l'Océan")

S("You can collect some valuable Ambers on the coast, but beware the tides!\n\n"
  "You can also take one of the boats and venture into the Ocean, "
  "to find other coasts, dangerous whirlpools, and islands inhabited by Pirates.",

  "Vous pouvez ramasser des Ambres précieuse sur la côte, mais attention aux marées !\n\n"
  "Vous pouvez aussi prendre un bateau et vous aventurer dans l'océan, "
  "pour trouver d'autres côtes, des tourbillons dangereux, et des îles peuplées de Pirates.")

N("Albatross", GEN_M | GENF_ELISION, "Albatros", "Albatros", "Albatros")

S("Those large seabirds attack you for some reason. At least they are not "
  "as fast as Eagles...",
  "Ces larges oiseaux vous attaquent pour une raison inconnue. Au moins "
  "ils sont moins rapides que des Aigles...")

N("stranded boat", GEN_M, "bateau échoué", "bateaux échoués", "bateau échoué")

S("This boat cannot go through the sand. But if you sit inside and "
  "wait for the tide, you will be able to use it to travel through the Ocean.",

  "Ce bateau ne peut pas traverser le sable. Mais si vous vous asseyez à l'intérieur "
  "et attendez la marée, vous pourrez l'utiliser pour voyager à travers l'Océan.")

N("Amber", GEN_M | GENF_ELISION, "Ambre", "Ambres", "Ambre")

S("When the tide is away, beautiful ambers can be found on the hyperbolic beaches. "
  "Perhaps there used to be a forest here?" ,
  "Quand la marée est basse, de magnifiques ambres peuvent être trouvées sur les "
  "plages hyperboliques. Peut-être y avait il une forêt autrefois...?")

Orb("Air", "de l'Air")

S("This Orb allows you to blow your enemies away.\n\n"
  "Click a monster to blow it one cell away. It cannot be used against mimics, ghosts, sharks and other monsters restricted to a specific terrain, and multi-tile monsters.",
  "Cet Orbe vous permet de soufflez vos ennemis au loin.\n\n"
  "Cliquer un monstre pour le reculer d'une case. Ne marche pas contre les mimiques, fantômes, requins et d'autres monstres restreints à un terrain spécial, et les monstres occupant plusieurs cases.")


N("Sea Border", GEN_M, "Bord de Mer", "Bords de Mer", "Bord de Mer")

S("Border between seas.", "Une frontière entre les mers.")

// Whirlpool
// ---------

// achievements:
// Escaped Whirlpool: Collect a Pearl, and escape the Whirlpool.
// Vortex Shark: Collect 10 Pearls.
// Vortex Pirate: Collect 25 Pearls.
// Master of the Whirlpool: Collect 50 Pearls.

N("Whirlpool", GEN_M, "Tourbillon", "Tourbillons", "dans le Tourbillon")

S("Many lost boats are spinning in this whirlpool. Some of them contain treasures "
  "and Pirates.\n\n"
  "Unmanned boats will go one cell clockwise in each turn. Sharks and manned boats "
  "are only allowed to go with the current, that is, towards the center, or clockwise.",

  "De nombreux bateaux perdus spiralent dans ce tourbillon. Certains contiennent des \n\n"
  "trésors et des Pirates. Les bateaux abandonnés se déplacent d'une case dans le sens "
  "des aiguilles d'une montre à chaque tour. Les requins et bateaux avec équipage peuvent "
  "seulement aller avec le courant, c'est à dire, vers le centre ou sens horaire.")

N("Pearl", GEN_F, "Perle", "Perles", "Perle")

S("You do not know exactly why, but there are valuable pearls on many boats "
  "in the whirlpool.",
  "Vous ne savez pas exactement pourquoi, mais il y a des perles précieuses "
  "sur de nombreux bateaux dans le tourbillon.")

Orb("Water", "d'Eau")

S("This Orb allows your boat to go against the current, "
  "and also to go into the land, creating water on the way.",
  "Cet Orbe permet à votre bateau d'aller contre le courant. "
  "et vous pouvez aussi aller sur terre, en créant de l'eau sur le passage.")

S("You cannot go against the current!", "Vous ne pouvez pas aller contre le courant !")

// Minefield
// ---------

// achievements:
// Reached Minefield: Collect a Bomberbird Egg.
// Minesweeper: Collect 10 Bomberbird Eggs.
// Bomberbird: Collect 25 Bomberbird Eggs.
// Master of the Minefield: Collect 50 Bomberbird Eggs.

N("Minefield", GEN_M, "Champ de Mines", "Champs de Mines", "dans le Champ de Mines")
N("minefield", GEN_F, "Mine ?", "Mines ?", "Mine ?")
N("cell without mine", GEN_F, "case sans mine", "cases sans mine", "case sans mine")

S("This minefield has been created by the Bomberbirds to protect their valuable eggs. "
  "Mines explode when they are stepped on, creating fire all around them "
  "(this fire never expires in the Minefield). If you don't step on a mine, you "
  "know the number of mines in cells around you. These numbers are marked with "
  "color codes on the cells you have visited.",
  "Ce champ de mines a été crée par les Oiseaux-Bombes pour protéger leurs précieux oeufs. "
  "Les mines explosent quand on marche dessus, enflammant les cases alentours "
  "(ce feu ne s'éteint jamais dans le champ de mines). Si vous ne marchez pas sur une mine, "
  "vous connaissez le nombre de mines des cases autour de vous. Ces nombres sont marqués avec "
  "un code couleur sur les cases déjà visitées.")

N("Bomberbird", GEN_M | GENF_ELISION, "Oiseau-Bombe", "Oiseaux-Bombe", "Oiseau-Bombe")

S("Dark red birds who have created the minefield.\n\n"
  "They create a mine on the spot where they are killed, provided "
  "that the terrain is suitable. Also note that mines are triggered "
  "by dead birds falling on them.",

  "Des oiseaux rouges sombre qui ont créé le champ de mines.\n\n"
  "Ils créent une mine à l'endroit où ils sont tués, si le terrain "
  "le permet. La chute des oiseaux morts déclenchent aussi les mines !")

N("Tame Bomberbird", GEN_M | GENF_ELISION, "Oiseau-Bombe apprivoisé", "Oiseaux-Bombe apprivoisés", "Oiseau-Bombe apprivoisé")

S("This bomberbird will follow you at some distance, and attack your enemies. "
  "You can kill it to place a mine.",

  "Cet Oiseau-Bombe vous suit à distance, et attaque vos ennemis. "
  "Vous pouvez le tuez pour placer une mine.")

N("Bomberbird Egg", GEN_M | GENF_ELISION, "Œuf d'Oiseau-Bombe", "Œufs d'Oiseaux-Bombe", "Œuf d'Oiseau-Bombe")

S("Bomberbird eggs are big and tasty, and thus valuable. "
  "They can hatch when left alone for some time (but this will never happen "
  "if you are watching).",

  "Les œufs d'Oiseau-Bombe sont gros et délicieux, et donc précieux. "
  "Ils peuvent éclore si laissés tranquille suffisamment longtemps "
  "(mais ça n'arrivera jamais pendant que vous regarder).")


Orb("Friendship", "de l'Amitié")

S("This Orb summons a friendly Bomberbird.",
  "Cet Orbe invoque un Oiseau-Bombe amical.")

S("Still confused? Read the FAQ on the HyperRogue website!\n\n",
  "Toujours confus ? Lisez la FAQ sur le site d'HyperRogue !\n\n")

S("Nowhere to blow %the1!", "Pas de place pour faire souffler %le1 !")
S("%The1 is immune to wind!", "%Le1 est immunisé contre le vent !")
S("You blow %the1 away!", "Vous soufflez %le1 !")

S("WARNING: you are entering a minefield!", "ATTENTION : vous entrez dans un champ de mines !")
S("No mines next to you.", "Pas de mine près de vous.")
S("A mine is next to you!", "1 mine près de vous !")
S("Two mines next to you!", "2 mine près de vous !")
S("Three mines next to you!", "3 mine près de vous !")
S("Four mines next to you!", "4 mine près de vous !")
S("Five mines next to you!", "5 mine près de vous !")
S("Six mines next to you!", "6 mine près de vous !")
S("Seven mines next to you!", "7 mine près de vous !")

S("You have to run away from the water!", "Vous devez fuir l'eau !")

// VERSION 7.2
// ===========

// Palace
// ------

N("Palace", GEN_M, "Palais", "Palais", "dans le Palais")
N("palace wall", GEN_M, "mur du palais", "murs du palais", "mur du palais")

S("Apparently a princess is kept locked somewhere, but you won't ever find her in this hyperbolic palace. ",
  "Apparemment une princesse est enfermée quelque part, mais vous ne la trouverez pas dans ce palais hyperbolique. ")

S("Apparently a prince is kept locked somewhere, but you won't ever find him in this hyperbolic palace. ",
  "Apparemment un prince est enfermé quelque part, mais vous ne le trouverez pas dans ce palais hyperbolique.  ")

S("So better just to concentrate on collecting treasure. "
  "Beware the traps, and the guards, who are hard to kill!",

  "Mieux vaut se concentrer sur la chasse au trésor. "
  "Attention aux pièges, et aux gardes qui sont durs à tuer !")

N("trapdoor", GEN_F, "trappe", "trappes", "trappe")
S("This floor will fall after someone goes there. Go quickly!",
  "Ce sol tombera après le passage d'une personne. Dépêchez vous !")

N("closed gate", GEN_F, "porte fermée", "portes fermées", "porte fermée")
N("open gate", GEN_F, "porte ouverte", "portes ouvertes", "porte ouverte")
N("closing plate", GEN_F, "plateforme de fermeture", "plateformes de fermeture", "plateforme de fermeture")
N("opening plate", GEN_F, "plateforme d'ouverture", "plateformes d'ouverture", "plateforme d'ouverture")

S("Green pressure plates open all gates in radius 3 around them. "
  "Similarly, red pressure plates close all gates in this radius.",

  "Les plateformes vertes ouvrent toutes les portes dans un rayon de 3. "
  "De même, les plateformes vertes ferment toutes les portes dans ce rayon.")

N("Hypersian Rug", GEN_M, "Tapis Hypersan", "Tapis Hypersan", "Tapis Hypersan")
S("Nice, a hyperbolic rug! It will not fit very well on your flat Euclidean floor, but who cares?",
  "Un tapis hyperbolique ! Il n'ira pas bien avec votre sol euclidien plat, mais quelle importance ?")

Orb("the Frog", "de la Grenouille")
S("This Orb lets you jump to a place which is two cell away from you, in a single turn. "
  "You can jump over water, chasms and fire, but not over walls.",
  "Cet Orbe permet de sauter vers une position à deux cases de vous, en un seul tour. "
  "Cet Orbe peut sauter par-dessus l'eau, les gouffres et le feu mais pas à travers les murs.")
S("Cannot jump through %the1!", "Vous ne pouvez pas sauter à travers %le1 !")
S("You jump!", "Vous sautez !")

S("\n\nNote: when pushing %the1 off a heptagonal cell, you can control the pushing direction "
  "by clicking left or right half of the heptagon.",
  "\n\nNote : quand vous poussez %le1 hors d'une case heptagonale, vous pouvez contrôler la direction de poussée "
  "en cliquant la moitié gauche ou droite de l'heptagone.")

S(" With the keyboard, you can rotate the view for a similar effect (Page Up/Down).",
  " Avec le clavier, vous pouvez pivoter l'affichage pour un effet similaire (Page Haut/Bas).")

N("Palace Guard", GEN_M, "Garde du Palais", "Gardes du Palais", "Garde du Palais")

S("You have to hit Palace Guards several times to kill them. After each hit, they "
  "are pushed away and stunned for some time.",
  "Vous devez frapper les Gardes du Palais plusieurs fois pour les tuer. Après chaque coup "
  "ils sont poussés et sonnés pour un certain temps.")

N("Fat Guard", GEN_M, "Gros Garde", "Gros Gardes", "Gros Garde")

S("Fat guards are too heavy to be pushed away by your hits.",
  "Les gros gardes sont trop lourds pour être repoussés par vos coups.")

N("Skeleton", GEN_M, "Squelette", "Squelettes", "Squelette")
S("Skeletons work similar to Palace Guards, but they won't die no matter how many "
  "times you hit them. Well, you have to be more creative...\n\n"
  "Skeletons attacked outside of their native land, Palace, are stunned for a longer time.",
  "Les squelettes sont similaires aux Gardes du Palais mais ne meurent pas quelque soit le "
  "nombre de fois où vous les frappez. Il faut être créatif... "
  "Les squelettes attaqués hors du Palais, leur pays natal, sont sonnés plus longtemps.s \n\n"  )

N("Vizier", GEN_M, "Vizir", "Vizirs", "Vizir")
S("Viziers are neither pushed away nor stunned. However, you attack them automatically when "
  "escaping from them.",
  "Les vizirs ne peuvent être repoussés ou assommés. Cependant, vous les attaquez automatiquement"
  " lorsque vous les fuyez.")

S("You stun %the1.", "Vous assommez %le1.")
S("The gate is closing right on you! RUN!", "La porte se referme sur vous ! Fuyez !")
S("%The1 is crushed!", "%Le1 est écrasé !")
S("You hit %the1.", "Vous frappez %le1.")
S("Hit him by walking away from him.", "Frappez le en vous éloignant de lui.")
S("Hmm, he has been training in the Emerald Mine. Interesting...", "Il s'est entraîné dans le Mine d'Émeraude. Intéressant...")

// Reached the Palace: Find and collect a Hypersian Rug.
// Palace Guard: Collect 10 Hypersian Rugs.
// Vizier: Colect 25 Hypersian Rugs.
// Prince: Colect 50 Hypersian Rugs.

// Living Fjord
// ------------

N("Living Fjord", GEN_M, "Fjord Vivant", "Fjords Vivants", "Fjord Vivant")

S("A coastal area, from where you can get both to the inland worlds and to the Ocean. "
  "Each turn, each cell will become water or earth, based on the majority of cells around it. "
  "Contrary to the Living Caves, this process is not affected by most items and monsters, "
  "but elementals, dead Trolls, and cells outside of the Living Fjord have "
  "a very powerful effect.",
  "Une région côtière, depuis laquelle vous pouvez allez dans les terres ou vers l'Océan. "
  "A chaque tour une case deviendra de l'eau ou de la terre, selon la majorité des cases qui l'entourent. "
  "Contrairement aux Caves Vivantes, ce processus n'est pas affecté par la plupart des objets et monstres, "
  "mais les élémentaux, Trolls morts, et cases extérieures ont un effet très puissant. " )

N("Fjord Troll", GEN_M, "Troll du Fjord", "Trolls du Fjord", "Troll du Fjord")
S("Fjord Trolls leave a wall when they die, causing the living fjord to rise around it. "
  "Contrary to Rock Trolls, items around are not destroyed.",
  "Les Trolls du Fjord laissent un mur à leur mort, causant une élévation du fjord vivant autour. "
  "Au contraire des Trolls des cavernes, les objets alentour ne sont pas détruits.")

N("Viking", GEN_M, "Viking", "Vikings", "Viking")
S("Mighty warriors from the Fjord, who can use boats.",
  "De puissants guerriers du Fjord, qui peuvent utiliser des bateaux.")

N("Water Elemental", GEN_M | GENF_ELISION, "Élémental d'Eau", "Élémentaux d'Eau", "Élémental d'Eau")
S("Wherever this powerful being goes, the living fjord "
  "sinks below water, non-magical boats are destroyed, and fires are extinguished.\n\n"
  "As a special case, you can attack the Water Elemental from the water, without drowning immediately.",
  "Là où cet être puissant va, le fjord vivant est submergé d'eau, les bateaux non-magiques détruits, "
  "et les feux éteints.\n\n"
  "Cas particulier : vous pouvez attaquer l'Élemental depuis l'eau sans vous noyer immédiatement.")


N("Garnet", GEN_M, "Grenat", "Grenats", "Grenat")
S("Vikings believe that garnets improve their strength.",
  "Les Viking croient que les grenats les rendent plus fort.")

Orb("the Fish", "du Poisson")
S("This Orb lets you dive into the water. While diving, you are able to see and collect underwater treasures, "
  "while being invisible to monsters yourself. You are afraid of jumping into the water straight from a boat, so usually you won't do it.",
  "Cet Orbe permet de plonger dans l'eau. En plongeant, vous pouvez voir et ramasser des trésors sous-marins, en étant invisible aux monstres. "
  "Vous avez peur de plonger dans l'eau depuis un bateau, donc vous ne le faites habituellement pas.")

S("%The1 is washed away!", "%Le1 a été emporté par les eaux !")
S("%The1 is extinguished!", "%Le1 s'est éteint !")

// Reached the Living Fjord: Find and collect a Garnet.
// Viking: Collect 10 Garnets.
// Water Elemental: Colect 25 Garnets.
// Master of the Fjord: Colect 50 Garnets.

// implemented, not appears in the game yet

Orb("Discord", "de Discorde")

S("Causes most monsters to attack other monsters, not only you and your friends.",
  "Cet Orbe pousse les monstres à s'attaquer entre eux, pas seulement vous et vos alliés.")

// Shmup Configuration
// -------------------

S("forward", "avancer")
S("backward", "reculer")
S("turn left", "tourner à gauche")
S("turn right", "tourner à droite")
S("move up", "aller en haut")
S("move right", "aller à droite")
S("move down", "aller en bas")
S("move left", "aller à gauche")
S("throw a knife", "jeter un couteau")
S("face the pointer", "tourner vers le pointeur")
S("throw at the pointer", "jeter au pointeur")
S("drop Dead Orb", "poser un Orbe mort")
S("center the map on me", "centrer la carte sur moi")
S("activate Orb power", "activer l'Orbe")

S("pan up", "glisser vers le haut")
S("pan right", "glisser vers la droite")
S("pan down", "glisser vers le bas")
S("pan left", "glisser vers la gauche")
S("rotate left", "pivoter vers la gauche")
S("rotate right", "pivoter vers la droite")
S("home", "home")

S("do nothing", "ne rien faire")
S("rotate view", "angle de vue")
S("panning X", "glisser X")
S("panning Y", "glisser Y")
S("player 1 X", "joueur 1 X")
S("player 1 Y", "joueur 1 Y")
S("player 1 go", "joueur 1 part")
S("player 1 spin", "joueur 1 tourne")
S("player 2 X", "joueur 2 X")
S("player 2 Y", "joueur 2 Y")
S("player 2 go", "joueur 2 part")
S("player 2 spin", "joueur 2 tourne")
S("player 3 X", "joueur 3 X")
S("player 3 Y", "joueur 3 Y")
S("player 3 go", "joueur 3 part")
S("player 3 spin", "joueur 3 tourne")
S("player 4 X", "joueur 4 X")
S("player 4 Y", "joueur 4 Y")
S("player 4 go", "joueur 4 part")
S("player 4 spin", "joueur 4 tourne")

S("Joystick %1, axis %2", "Joystick %1, axe %2")
S("one player", "un joueur")
S("two players", "deux joueur")
S("configure player 1", "configurer joueur 1")
S("configure player 2", "configurer joueur 2")
S("configure player 3", "configurer joueur 3")
S("configure player 4", "configurer joueur 4 ")
S("configure panning", "configurer le glissement")
S("configure joystick axes", "configurer les axes du joystick")
S("continue playing", "continuer le jeu")
S("start playing in the shmup mode", "démarrer le mode shoot 'em up")
S("return to the turn-based mode", "retour au mode par tours")
S("save the configuration", "sauvegarder la configuration")
S("press a key to unassign", "appuyer sur une touche à dé-assigner")
S("press a key for '%1'", "appuyer sur une touche pour '%1'")
S("unassign a key", "dé-assigner une touche")

// extra pattern modes
// -------------------

S("explore the Emerald Pattern", "explorer le Motif d'Émeraude")

// extra flavor/Knight lines
// -------------------------

S("Kill a Vizier in the Palace to access Emerald Mine", "Tuez un Vizir dans le Palais pour accéder à la Mine d'Émeraudes")
S("Collect 5 Emeralds to access Camelot", "Rassemblez 5 Émeraudes pour accéder à Camelot")
S("Some lands unlock at specific treasures or kills", "Certains pays sont débloqués par des trésors ou des ennemis tués spécifiques")

S("You will be fighting red rock snakes, too...", "Vous combattrez aussi les serpents des roches rouges...")

S("\"The Red Rock Valley is dangerous, but beautiful.\"",
  "\"La Vallée de Roche Rouge est dangereuse, mais belle.\"")
S("\"Train in the Desert first!\"", "\"Entraînez vous d'abord dans le Désert !\"")
S("\"I like collecting ambers at the beach.\"", "\"J'aime ramasser de l'ambre à la plage.\"")
S("\"Our Table seats %1 Knights!\"", "\"À notre Table siègent %1 Chevaliers !\"")
S("\"There are %1 floor tiles inside our Table!\"", "\"Il y a %1 pavés de sol à l'intérieur de notre Table !\"")
S("\"By now, you should have your own formula, you know?\"", "\"Maintenant, vous devriez avoir votre propre formule, vous savez ?\"")
S("\"Have you tried to take a boat and go into the Ocean? Try it!\"", "\"Avez vous déjà essayé de prendre un bateau et naviguer sur l'Océan ?\"")

// VERSION 7.3
// ===========

// new name for 'Ivy D'

N("Dead Ivy", GEN_M, "Lierre mort", "Lierres morts", "Lierre mort")

// new messages for living/nonliving monsters

S("%The1 bites %the2!", "%Le1 mort %le2 !")
S("%The1 attacks %the2!", "%Le1 attaque %le2 !")
S("You destroy %the1 with a mental blast!", "Vous détruisez %le1 avec une attaque mentale !")
S("The ivy kills %the1!", "Le lierre tue %le1 !")
S("You destroy %the1.", "Vous détruisez %le1.")
S("%The1 kills %the2!", "%Le1 tue %le2 !")
S("%The1 sinks!", "%Le1 coule !")

S("Cannot jump on %the1!", "Vous ne pouvez pas sauter sur %le1 !")

// renamed Fifty to Palace
S("explore the Palace Pattern (4 colors)", "explorer la structure du Palais (4 couleurs)")
S("explore the Palace Pattern (8 colors)", "explorer la structure du Palais (8 couleurs)")

// Map Editor
// ----------

S("map editor", "éditeur de carte")
S("You activate your terraforming powers!", "Vous activez vos pouvoirs de terraformation !")
S("Press F5 to clear the board.", "Presser F5 pour vider le plateau du jeu.")
S("Press F5 to restart the game normally.", "Presser F5 pour recommencer à jouer normalement.")
S("High quality shots not available on this platform", "Les captures d'écran à haute résolutions ne sont pas disponibles sur cette plateforme")
S("Saved the high quality shot to %1", "Capture d'écran à haute résolution enregistrée dans %1")
S("Map saved to %1", "Carte sauvegardée dans %1")
S("Map loaded from %1", "Carte chargée depuis %1")
S("copying", "copier")
S("boundary", "frontière")
S("clear monster", "enlever les monstres")
S("use at your own risk!", "utiliser à vos risques et périls !")

// Note: in English, these have been arranged from longest to shortest.
// This gives a somewhat nice effect on the top left corner of the screen.
// You may want to aim for the same effect when translating

S("0-9 = radius (%1)", "0-9 = rayon (%1)")
S("b = boundary", "i = frontière")
S("m = monsters", "m = monstre")
S("w = walls",    "w = murs")
S("i = items",    "l = objets")
S("l = lands",    "c = pays")
S("c = copy",     "b = copier")

// And this is from shortest to longest (bottom left corner)
S("F1 = help",                "F1 = aide")
S("F2 = save",                "F2 = sauvegarder")
S("F3 = load",                "F3 = charger")
S("F5 = restart",             "F5 = redémarrer")
S("F6 = HQ shot",             "F6 = capture HQ")
S("F7 = player on/off",       "F7 = joueur on/off")
S("ESC = return to the game", "ESC = retour au jeu")

S("This mode allows you to edit the map.\n\n"
  "NOTE: Use at your own risk. Combinations which never "
  "appear in the real game may work in an undefined way "
  "(do not work, look strangely, give strange messages, or crash the game).\n\n"
  "To get the most of this editor, "
  "some knowledge of inner workings of HyperRogue is required. "
  "Each cell has four main fields: land type, wall type, monster type, item type. "
  "The same wall type (especially \"none\", \"sea\", or \"bonfire\") may look or "
  "work a bit differently, based on the land it is in. Sometimes an object may "
  "appear twice on the list due to subtle differences (for example, Demons could "
  "move next turn or not).\n\n"
  "Press w, i, l, or m to choose which aspect of cells to change, "
  "then just click on the cells and they will change. Press 'c' while "
  "hovering over a cell to copy that cell, this copies all information about it. "
  "When copying large areas or placing multi-tile monsters, it might be important where "
  "on the cell you are clicking.\n\n"
  "You can also press 0-9 to apply your changes to a greater radius. "
  "This also affects the copy/paste feature, allowing to copy a larger area.\n\n"
  "Press F2 to save the current map (and F3 to load it). If you try this after "
  "a long game of HyperRogue (without using Orbs of Safety), the filesize will "
  "be very large! "
  "Note however that large structures, such as "
  "Great Walls, large circles and horocycles, are destroyed by this.\n\n"
  "Press 'b' to mark cells as boundaries. Such cells, and cells beyond "
  "them, are not copied by the copy/paste feature, nor saved by the "
  "save feature.\n\n",

  "Ce Mode vous permet d'éditer la carte.\n\n"
  "NOTE : à utiliser à vos risques et périls ! Les combinaisons qui n’apparaissent pas "
  "dans le jeu peuvent marcher de manière indéfinie (ne pas marcher, donner des messages"
  " étranges, ou faire crasher le jeu).\n\n "
  "Pour tirer le meilleur parti de cet éditeur , une compréhension du fonctionnement interne"
  "d'HyperRogue est nécessaire. Chaque cellule a quatre champs principaux : type de pays,"
  " type de mur, type de monstre, type d'objet. Le même type de mur (en particulier "
  "\"none\", \"sea\", ou \"bonfire\") peuvent apparaître et fonctionner un peu différemment"
  "selon le pays sur lequel il est placé. Parfois un objet peut apparaître deux fois sur la "
  "liste en raison de différences subtiles (par ex , les Démons pourrait bouger au tour "
  "suivant ou pas). Appuyer sur w, i, l, ou m pour choisir quel aspect des cellules changer, "
  "puis cliquer sur les cellules et elles changeront. Appuyer sur 'c' au-dessus d'une case "
  "pour la copier, ce qui copie toutes les informations la concernant. Lorsque l'on copie de "
  "grandes zones ou place des monstres sur plusieurs cases, l'endroit de la case où l'on clique "
  "peut importer.\n\n"
  "Vous pouvez aussi presser 0-9 pour appliquer les modifications sur un plus grand rayon. "
  "Cela affecte aussi le copier/coller, permettant de copier une plus grande zone.\n\n"
  "Presser F2 pour sauvegarder la carte actuelle (et F3 pour la charger). "
  "Si vous essayez ceci après un long jeu d'HyperRogue (sans utiliser d'Orbes de Sécurité), "
  "le fichier peut être très volumineux ! Cependant les grandes structures telles que les "
  "Grands Murs, grands cercles et horocycles sont détruits par ceci.\n\n"
  "Presser 'b' pour marquer des cases comme limites. Ces cases, et celles au-delà, ne sont "
  "pas copiées par la sauvegarde ou le copier/coller.\n\n")

// Princess Quest
// --------------

// Saved the Princess
// Princess Challenge
// Martial Artist

S("A mouse squeaks at you. It seems that it wants you to go somewhere. "
  "That's interesting, what could you find here?\n\n"
  "Note: in the part of the Palace that you are exploring during this "
  "quest, the distribution of monsters, pressure plates, and items has been "
  "changed somewhat, to make it a bit more suitable for less "
  "experienced players. The number of monsters does not depend on the "
  "number of Hypersian Rugs you have collected, and there are more "
  "opening plates.\n\n"
  "However, even with the normal distribution, it appears that it should be "
  "always possible to reach your goal. If you want this challenge, "
  "select it from the special game modes menu.",

  "Une souris couine dans votre direction. Elle semble vouloir vous emmener "
  "quelque part. Intéressant, qu'allez vous trouver ?\n\n"
  "Note : dans la partie du Palais que vous explorez lors de cette quête, "
  "la distribution des monstres, plateformes et objet a été modifiée pour "
  "faciliter le passage de joueurs moins expérimentés. "
  "Les nombre de monstres ne dépend pas du nombre de tapis Hypersans ramassés "
  "et il y a plus de plateformes d'ouverture'.\n\n"
  "Néanmoins, même avec la distribution normale, il devrait toujours être possible "
  "d'atteindre l'objectif. Ce mode peut être modifié dans le menu mode spécial pour "
  "ajouter de la difficulté.")

S("\"When I visited the Palace, a mouse wanted me to go somewhere.\"",
  "\"Alors que je visitais le Palais une souris voulais m'emmener quelque part.\"")

S("\"I wonder what was there...\"", "\"Je me demande ce qu'il y avait là-bas...\"")

N("Mouse", GEN_F, "Souris", "Souris", "Souris")

S("You hear a distant squeak!", "Vous entendez un couinement au loin !")
S("%The1 squeaks in a confused way.", "%Le1 couine confusément.")
S("%The1 squeaks gratefully!", "%Le1 couine avec gratitude !")
S("%The1 squeaks hopelessly.", "%Le1 couine sans espoir.")
S("%The1 squeaks in despair.", "%Le1 couine désespérément.")
S("%The1 squeaks sadly.", "%Le1 couine tristement.")
S("%The1 squeaks with hope!", "%Le1 couine plaine d'espoir !")
S("%The1 squeaks happily!", "%Le1 couine joyeusement !")
S("%The1 squeaks excitedly!", "%Le1 couine pleine d'excitation !")

N("giant rug", GEN_M, "tapis géant", "tapis géants", "tapis géant")

S("This is the biggest Hypersian Rug you have ever seen! "
  "Unfortunately, it is too large to take it as a trophy.",
  "C'est le plus grand tapis Hypersan que vous avez jamais vu ! "
  "Malheureusement il est trop grand pour être emporté comme trophée.")

N("Prince", GEN_M, "Prince", "Princes", "Prince")
N("Princess", GEN_F, "Princesse", "Princesses", "Princesse")

S("%1 takes %his1 revenge on %the2!", "%Le1 prend sa vengeance sur %le2 !")
S("\"That should teach you to take me seriously!\"",
  "\"Ça vous apprendra à me prendre au sérieux !\"")

S("%The1 kisses you, and begs you to bring %him1 away from here.",
  "%Le1 vous embrasse, et vous supplie de l'emmener loin d'ici.") // FIXME: La/le

S("\"I want my revenge. Stun a guard and leave him for me!\"",
  "Je veux ma vengeance. Assommez un garde et laissez-le moi !\"")

S("\"That felt great. Thanks!\"", "\"Merci, ça m'a soulagé%e1.\"")

S("\"Bring me out of here please!\"", "Sortez moi d'ici s'il vous plaît !\"")

S("%The1 kisses you, and thanks you for saving %him1.",
  "%Le1 vous embrasse, et vous remercie de l'avoir sauvé.")

S("\"I have been trained to fight with a Hypersian scimitar, you know?\"",
  "J'ai été entraîné%e1 à combattre au cimeterre Hypersan, vous savez ?\"")

S("\"I would love to come to your world with you!\"",
  "J'adorerai venir dans votre monde avec vous !\"")

S("\"Straight lines stay close to each other forever, this is so romantic!\"",
  "\"Les lignes parallèles restent proches à tout jamais, c'est si romantique !\"")

S("\"Maps... Just like the world, but smaller... how is that even possible?!\"",
  "\"Des cartes... comme le monde, mais plus petit... comment est-ce possible ?!\"")

Orb("Love", "de l'Amour")
S("Love takes time, but it heals all wounds, and transcends time and space.\n\n"
  "The Orb of Love is worth 30$$$, if you end the game with it.\n",

  "L'Amour prend du temps, mais guérit toutes les blessures, et transcende le temps et l'espace.\n\n"
  "L'Orbe d'Amour vaut 30$$$ quand vous terminez le jeu avec.\n")

// Princess Challenge
// ------------------

S("%1 Challenge", "Défi %1")
S("Save %the1 first to unlock this challenge!", "Sauvez d'abord %le1 pour débloquer ce défi !")
S("Welcome to %the1 Challenge!", "Bienvenue au défi de %le1 !")
S("The more Hypersian Rugs you collect, the harder it is.", "Plus vous ramassez de tapis Hypersans, plus c'est dur.")
S("Follow the Mouse and escape with %the1!", "Suivez la Souris et échappez vous avec %le1 !")
S("Hardness frozen at %1.", "Difficulté figée : %1.")
S("Congratulations! Your score is %1.", "Félicitations ! Votre score est de %1.")

S("u = undo",     "u = défaire")
S("f = flip %1",  "f = renverser %1")

S("Failed to save map to %1", "Échec de la sauvegarde de la carte vers %1")
S("Failed to load map from %1", "Échec du chargement de la carte depuis %1")
S("save whom", "sauver qui ?")
S("other", "autre") // other keys in the main menu


// VERSION 7.4
// ===========

// missing texts, refactored things, and rule changes
// --------------------------------------------------

S("%The1 activates her Flash spell!", "%Le1 active le sort de l’Éclair !")

N("Fire", GEN_M, "Feu", "Feux", "Feu")
S("This cell is on fire. Most beings and items cannot survive.",
  "Cette cellule est en feu. La plupart des êtres et objets ne peuvent y survivre.")

N("Eternal Fire", GEN_M, "Feu Éternel", "Feux Éternels", "Feu Éternel")
S("This fire never burns out.", "Ce feu ne s'éteint jamais.")

S("Some rubble.", "Des gravats.")

S("The Orb loses its power as it leaves the Land of Power!",
  "L'Orbe perd son pouvoir lorsqu'il quitte le Pays du Pouvoir !")

S("%The1 fills the hole!", "%Le1 remplit le trou !")

N("Tentacle+Ghost", GEN_M, "Tentacule+Fantôme", "Tentacules+Fantômes", "Tentacule+Fantôme")

// Land Overview
// -------------

S("world overview", "vue d'ensemble")
S("or 'o' to see the world overview", "ou 'o' pour la vue d'ensemble")

S("forbidden to find in %the1", "interdit de ramasser %a1")
S("too dangerous to use in %the1", "trop dangereux à utiliser %a1")
S("useless in %the1", "inutile %a1")
S("only native Orbs allowed in %the1", "Seules les Orbes locales sont autorisées %a1")
S("this Orb is never unlocked globally (only hubs)", "Cet Orbe n'est jamais débloqué globalement")
S("collect 25 %2 to unlock it in %the1", "rassembler 25x %P2 pour le débloquer %a1")
S("collect 3 %2 to unlock it in %the1", "rassembler 3x %P2 pour le débloquer %a1")
S("native to %the1 (collect 10 %2)", "originaire de %le1 (10x %P2)")
S("native to %the1 (collect 1 %2)", "originaire de %le1 (1x %P2)")
S("secondary in %the1 (collect 10 %3, or 25 %2)", "secondaire %a1 (10x %P3, ou 25x %P2)")
S("the native Orb of %the1", "l'Orbe originaire de %le1")
S("this Orb appears on floors and is used by witches", "Cet Orbe apparaît au sol et est utilisé par les sorcières")
S("a prized Orb, it appears only in cabinets", "un Orbe de valeur qui n'apparaît que dans les vitrines")
S("this Orb never appears in %the1", "Cet Orbe n'apparaît jamais %a1")
S("Hub Land: orbs appear here if unlocked in their native land", "Centre : les Orbes apparaissent ici si débloqués dans leur pays d'origine")
S("kill a monster, or collect 25 %2", "tuer un monstre ou rassembler 25x %P2")

S("Always available.\n", "Toujours disponible.\n")
S("Treasure required: %1 $$$.\n", "Trésor nécessaire : %1 $$$.\n")
S("Accessible only from %the1.\n", "Accessible seulement depuis %le1.\n")
S("Accessible only from %the1 (until finished).\n", "Accessible seulement depuis %le1 (jusqu'à ce que ce soit fini).\n")

S("Accessible only from %the1 or %the2.\n", "Accessible seulement depuis %le1 ou %le2.\n")

S("Kills required: %1.\n", "Nombre de monstres tués nécessaire : %1.\n")
S("Finished lands required: %1 (collect %2 treasure)\n",
  "Pays terminés nécessaires : %1 (rassembler %2 trésor)\n")

S("Treasure required: %1 x %2.\n", "Trésor nécessaire : %1 x %2.\n")

S("Alternatively: kill a %1.\n", "Alternative : tuer %un1.\n")

S("Kills required: any Elemental (Living Fjord/Dead Caves).\n",
  "Nombre de monstres tués nécessaire : Élémentaux (Fjord Vivant/Cave Mortes).")

S("Variants of %the1 are always available in the Random Pattern Mode.",
  "Des variantes de %le1 sont toujours disponibles dans le Mode Motif Aléatoire.")

S("Variants of %the1 are available in the Random Pattern Mode after "
  "getting a highscore of at least 10 %2P.",
  "Des variantes de %le1 sont toujours disponibles dans le Mode Motif Aléatoire après "
  "avoir obtenu un record d'au moins 10x %2P.")

S(" kills: %1", " Tués : %1")
S(" $$$: %1", " $$$: %1")
S(" Hyperstone: %1/%2", " Hyperpierres : %1/%2")
S(" Hell: %1/%2", " Enfers : %1/%2")

// improved editor
// ---------------

S("vector graphics editor -- press F1 for help", "éditeur graphique vectoriel -- appuyer sur F1 pour l'aide")
S("pics to save/load:", "images à sauvegarder/charger :")
S("level to save/load:", "niveau à sauvegarder/charger :")

S("F4 = extension", "F4 = extension")
S("Enter = back", "Entrée = retour")
S("SPACE = map/graphics", "Espace = carte/graphiques")

S("Emerald Pattern", "Motif d'Emeraude")
S("Palace Pattern", "Motif du Palais")
S("Zebra Pattern", "Motif Zébré")
S("rotational symmetry", "symétrie centrale")
S("symmetry 0-1", "symétrie 0-1")
S("symmetry 0-2", "symétrie 0-2")
S("symmetry 0-3", "symétrie 0-3")

S("display pattern codes (full)", "montrer les codes du motif (complet)")
S("display pattern codes (simplified)", "montrer les codes du motif (simplifié)")
S("display only hexagons", "ne montrer ne les hexagones")
S("display only heptagons", "ne montrer ne les heptagones")
S("predesigned patterns", "Motif prédéterminés")

S("Gameboard", "Plateau")
S("random colors", "couleurs aléatoires")
S("emerald pattern", "motif d'émeraude")
S("four elements", "quatre éléments")
S("eight domains", "huit domaines")
S("zebra pattern", "motif zébré")
S("three stripes", "trois bandes")
S("random black-and-white", "noir et blanc aléatoire")
S("p = paint", "p = couleurs")
S("r = regular", "r = régulier")

S("In this mode you can draw your own player characters, "
  "floors, monsters, and items. Press 'e' while hovering over "
  "an object to edit it. Start drawing shapes with 'n', and "
  "add extra vertices with 'a'. Press 0-9 to draw symmetric "
  "pictures easily. More complex pictures can "
  "be created by using several layers ('l'). See the edges of "
  "the screen for more keys.",

  "Dans ce mode vous pouvez dessiner vos personnages, sols, "
  "monstres et objets. Appuyer sur 'e' en passant au-dessus de "
  "l'objet pour l'éditer. Commencer par dessiner des formes avec 'n' "
  "et ajouter des sommets avec 'a'. Presser 0-9 pour dessiner des images "
  "symétriques facilement. Des images plus complexes peuvent être créées "
  "en utilisant plusieurs couches ('l'). Voir aux bords de l'écran pour plus "
  "de touches.")

S("Press 'r' to choose a regular pattern. When a pattern is on, "
  "editing a cell automatically edits all cells which are "
  "equivalent according to this pattern. You can choose from "
  "several patterns, and choose which symmetries matter "
  "for equivalence. Also, you can press Space to switch between "
  "the map and graphics editor quickly -- note that editing floors "
  "with the graphics editor also adheres to the pattern.",

  "Appuyer sur 'r' pour choisir un motif régulier. Quand un motif est sélectionné "
  "éditer une cellule édite automatiquement toutes les cellules équivalentes selon ce "
  "motif. Vous pouvez choisir entre plusieurs motifs, et quelle symétrie compte pour "
  "l'équivalence. Appuyer sur Espace pour passer rapidement entre l'éditeur graphique "
  "et de carte. Éditer les sols avec l'éditeur graphique fonctionne de la même manière.")

S("monster" ,"monstre")
S("item", "objet")
S("floor", "sol")
S("hexagonal", "hexagonal")
S("heptagonal", "heptagonal")
S("floor/pattern", "sol/motif")

S("l = layers: %1", "l = couches : %1")
S("1-9 = rotations: %1", "1-9 = rotations : %1")
S("0 = symmetry", "0 = symétrie")
S("0 = asymmetry", "0 = asymétrie")
S("%1 vertices", "sommets : %1")
S("a = add v", "a = ajouter v")
S("m = move v", "m = déplacer v")
S("d = delete v", "d = supprimer v")
S("c = readd v", "c = réajouter v")
S("b = reorder", "b = réordonner")
S("t = shift", "t = décaler")
S("y = spin", "y = tourner")

S("'n' to start", "n = démarrer")
S("z = zoom in", "z = agrandir")
S("o = zoom out", "o = rétrécir")
S("g = grid", "g = grille")
S("e = edit this", "e = éditer")

S("x: %1", "x: %1")
S("y: %1", "y: %1")
S("z: %1", "z: %1")
S("r: %1", "r: %1")
S("ϕ: %1°", "ϕ: %1°")

S("A fake Land with colored floors.",
  "Un faux Pays aux sols colorés.")

S("random pattern mode", "Mode motif aléatoire")

// Ivory Tower
// -----------

N("Ivory Tower", GEN_F, "Tour d'Ivoire", "Tours d'Ivoire", "Tour d'Ivoire")

S("Powerful wizards claimed this part of the world, to perform their magical "
  "experiments in peace and solitude. They have changed the direction of gravity, "
  "to make it even harder for intruders to reach them.\n\n"
  "Gravity works as follows: cells are unstable if they are empty, and there is "
  "no cell immediately below them which contains a wall. It is impossible to move "
  "from one unstable cell to another, except if moving down.",

  "De puissants magiciens ont pris possession de cette partie du monde, pour conduire "
  "leurs expériences dans le calme et la solitude. Ils ont changé la direction de la  "
  "gravité pour que les intrus ne puissent pas les atteindre facilement.\n\n"
  "Le gravité fonctionne de cette manière : les cases vides sans mur en-dessous sont "
  "instables. Ils est impossible de passer d'une cases instable vers une autre sauf "
  "vers le bas.")

N("Servant", GEN_M, "Serviteur", "Serviteurs", "Serviteur")
S("A simple servant of the master of the Ivory Tower.",
  "Un simple serviteur des maîtres de la Tour d'Ivoire.")

N("Gargoyle", GEN_F, "Gargouille", "Gargouilles", "Gargouille")
N("stone gargoyle", GEN_F, "gargouille de pierre", "gargouilles de pierre", "gargouille de pierre")
N("stone gargoyle floor", GEN_M, "sol de gargouille de pierre", "sols de gargouille de pierre", "sol de gargouille de pierre")
N("stone gargoyle bridge", GEN_M, "pont de gargouille de pierre", "ponts de gargouille de pierre", "pont de gargouille de pierre")

S("A being made of stone, who likes high buildings. It becomes normal stone when "
  "killed, but only if next to something stable -- otherwise it falls.",
  "Une être fait de pierre, qui aime les hautes constructions. Elle devient de la pierre "
  "normale lorsque tué, mais seulement si elle est près de quelque chose de stable, sinon elle tombe.")

N("Ivory Figurine", GEN_F, "Figurine d'Ivoire", "Figurines d'Ivoire", "Figurine d'Ivoire")

S("A beautiful figurine, made of ivory. Figurines close to the base of the Tower "
  "tend to disappear after you have collected many of them.",
  "Une belle figurine, faite d'ivoire. Les figurines proches du bas de la Tour "
  "disparaissent lorsque vous en avez ramassé beaucoup.")

N("platform", GEN_F, "plateforme", "plateformes", "plateforme")
S("You can stand here.", "Vous pouvez vous tenir ici.")

N("ladder", GEN_F | GENF_ELISION, "échelle", "échelles", "échelle")
S("You can use this ladder to climb the Tower.",
  "Vous pouvez utiliser cette échelle pour escalader la Tour.")

Orb("Matter", "de Matière")

S("This Orb allows one to temporarily create impassable matter, either to block paths or "
  "to build bridges across chasms and waters.",
  "Cet Orbe permet de créer temporairement une barrière infranchissable, pour bloquer un "
  "chemin ou construire un pont au-dessus d'un gouffre ou de l'eau.")

N("temporary wall", GEN_M, "mur temporaire", "murs temporaires", "mur temporaire")
N("temporary floor", GEN_M, "sol temporaire", "sols temporaires", "sol temporaire")
N("temporary bridge", GEN_M, "pont temporaire", "ponts temporaires", "pont temporaire")

S("This structure will disappear after some time.", "Cette structure disparaîtra après un certain temps.")

S("Nothing to stand on here!", "Il n'y a rien pour vous supporter ici !")
S("Gravity does not allow this!", "La gravité ne permet pas ceci !")


// Elemental Planes
// ----------------

N("Elemental Planes", GEN_M, "Plan Élémental", "Plans Élémentaux", "sur les Plans Élémentaux")
N("Plane of Fire", GEN_M, "Plan du Feu", "Plans du Feu", "Plan du Feu")
N("Plane of Earth", GEN_M, "Plan de Terre", "Plans de Terre", "Plan de Terre")
N("Plane of Air", GEN_M, "Plan de l'Air", "Plans de l'Air", "Plan de l'Air")
N("Plane of Water", GEN_M, "Plan de l'Eau", "Plans de l'Eau", "Plan de l'Eau")

S("The Elemental Planes are divided into four subzones: Planes of Fire, Water, Air, and Earth. "
  "You need to collect a Shard from each Plane to construct an Elemental Gem. "
  "It is dangerous to collect too many Shards of the same type without constructing a Gem.",

  "Les Plans élémentaux sont divisés en quatre zones : les Plans de Feu, d'Eau, d'Air, de Terre. "
  "Vous devez ramasser un Fragment de chaque Plan pour construire un Gemme Élémentale. "
  "Il est dangereux de ramasser trop de fragments du même type sans construire de Gemme.")

N("Fire Elemental", GEN_M | GENF_ELISION, "Élémental de Feu", "Élémentaux de Feu", "Élémental de Feu")

S("This monster leaves a trail of fire behind.",
  "Ce monstre laisse une trainée de feu derrière lui.")

N("Air Elemental", GEN_M | GENF_ELISION, "Élémental d'Air", "Élémentaux d'Air", "Élémental d'Air")

S("An Air Elemental looks like a live tornado. Once you are three (or less) cells from it, "
  "it is impossible to move closer, due to strong winds. You can stand and wait, though. "
  "This also affects most monsters.",

  "Un Élémental d'Air ressemble à une tornade vivante. A trois (ou moins) cases de distance "
  "il est impossible de s'approcher, à cause des vents puissants. Vous pouvez néanmoins attendre. "
  "Cela affecte la plupart des monstres.")

N("Fire Shard", GEN_M, "Fragment de Feu", "Fragments de Feu", "Fragment de Feu")
N("Air Shard", GEN_M, "Fragment d'Air", "Fragments d'Air", "Fragment d'Air")
N("Water Shard", GEN_M, "Fragment d'Eau", "Fragments d'Eau", "Fragment d'Eau")
N("Earth Shard", GEN_M, "Fragment de Terre", "Fragments de Terre", "Fragment de Terre")
N("Elemental Gem", GEN_F, "Gemme Élémentale", "Gemmes Élémentales", "Gemme Élémentale")

Orb("Summoning", "d'Invocation")

S("This orb allows you to summon monsters. Usually, they are either Elementals or "
  "native creatures. While the monsters do not like being summoned, and will "
  "attack you once they recover from summoning sickness, such summoning "
  "often has its uses.",
  "Cet Orbe permet d'invoquer des monstres. Il s'agit habituellement d'élémentaux, "
  "ou de monstres locaux. Alors que les monstres n'aiment pas être invoqués, au "
  "point de vous attaquer une fois qu'ils se remettent de leur mals d'invocation, "
  "ces invocations peuvent être souvent utiles.")

N("limestone wall", GEN_M, "mur de calcaire", "murs de calcaire", "mur de calcaire")

S("Simply a wall. Mostly.", "Un simple mur. Principalement.")

// Zebra
// -----

N("Zebra", GEN_M, "Zèbre", "Zèbres", "dans le Zèbre")
S("Everything in this Land has black and white stripes.",
  "Tout dans ce Pays a des bandes noires et blanches.")

N("Striped Dog", GEN_M, "Chien Rayé", "Chiens Rayés", "Chien Rayé")
S("A predator native to the Zebra.", "Un prédateur originaire du Zèbre.")

N("Onyx", GEN_M | GENF_ELISION, "Onyx", "Onyx", "Onyx")
S("A black gem with white stripes. It is beautiful.", "Une gemme noire avec des bandes blanches. C'est magnifique.")

// Crossroads III
// --------------

N("Crossroads III", GEN_M, "Croisement III", "Croisements III", "dans les Croisements III")

S("An alternate layout of the Crossroads. Great Walls cross here at right angles.",
  "Une disposition alternative des Croisements. Les Grands Murs se croisent ici à angle droit.")

S("Cannot create temporary matter on a monster!", "Impossible de créer de la matière temporaire sur un monstre !")
S("Cannot create temporary matter on an item!", "Impossible de créer de la matière temporaire sur un objet !")
S("Cannot create temporary matter here!", "Impossible de créer de la matière temporaire ici !")
S("Cannot summon on a monster!", "Impossible d'invoquer un monstre !")
S("No summoning possible here!", "L'invocation n'est pas possible ici !")
S("You summon %the1!", "Vous invoquez %le1 !")

S("F4 = file", "F4 = fichier")

// VERSION 8.0
// ===========

S("The Air Elemental blows you away!", "Les élémentaux d'air vous soufflent au loin !")

// appended to cave troll description
S(" Additionally, all items around the killed Troll will be destroyed.",
  " Tout les objets autour du Troll tué seront détruits.")

// 'dead cave troll' and 'dead troll' are separated now

N("dead rock troll", GEN_M, "Troll des roches mort" ,"Trolls des roches morts", "Troll des roches mort")

S("There are several species of trolls living in the hyperbolic world. "
  "Some of them leave this wall behind them when they die.",

  "Il y a plusieurs espèces de trolls différentes vivants dans le monde hyperbolique. "
  "Certains laissent un mur derrière eux à leur mort.")

// paper model creator
// -------------------

S("paper model creator", "créateur de modèle en papier")
S("synchronize net and map", "synchroniser le réseau et la carte")
S("display the scope", "afficher la sonde")
S("create the model", "créer le modèle")
S("design the net", "concevoir le réseau")

S("The paper model created as papermodel-*.bmp", "Ce modèle en papier a été créé en tant que papermodel-*.bmp")
S("Failed to load the file 'papermodeldata.txt'", "Le chargement de papermodeldata.txt a échoué.")
S("Could not save the paper model data", "Les données du modèle de papier n'ont pas pu être sauvegardées")

// pure tactics mode
// -----------------

S("pure tactics mode", "mode stratégie")
S("Not available in the pure tactics mode!", "Non disponible en mode stratégie !")
S("\"The Knights of the Horocyclic Table salute you!\"", "\"Les Chevaliers de la Table Horocycle vous saluent !\"")
S("press 0 to leave this mode", "Appuyez sur 0 pour quitter ce mode")
S("Collect %1x %2 to unlock", "Rassemblez %1x %P2 pour débloquer")

S("In the pure tactics mode, you concentrate on a specific land. "
  "Your goal to obtain as high score as possible, without using "
  "features of the other lands. You can then compare your score "
  "with your friends!\n\n"

  "You need to be somewhat proficient in the normal game to "
  "unlock the given land in this challenge "
  "(collect 20 treasure in the given land, or 2 in case of Camelot).\n\n"

  "Since getting high scores in some lands is somewhat luck dependent, "
  "you play each land N times, and your score is based on N consecutive "
  "plays. The value of N depends on how 'fast' the land is to play, and "
  "how random it is.\n\n"

  "In the Caribbean, you can access Orbs of Thorns, Aether, and "
  "Space if you have ever collected 25 treasure in their native lands.\n\n"

  "The rate of treasure spawn is static in this mode. It is not "
  "increased by killing monsters.\n\n"

  "Good luck, and have fun!",

  "Dans le mode stratégie, vous vous concentrez sur un pays spécifique. "
  "Votre but est d'obtenir le plus haut score possible, sans utiliser les "
  "fonctionnalités d'autres Pays. Vous pouvez alors comparer votre score avec celui "
  "de vos amis !\n\n"

  "Vous devez être doué dans le jeu normal pour débloquer "
  "un pays donné dans ce défi. "
  "(rassembler 20 trésors du pays donné, ou 2 dans le cas de Camelot).\n\n"

  "Comme l'obtention de hauts scores dans certains pays dépend du hasard, vous"
  "jouez chaque pays N fois et le score est basé sur les N parties consécutives. "
  "La valeur de N dépend de la 'durée' et du caractère aléatoire du pays.\n\n"

  "Dans les Caraïbes, vous pouvez trouver l'Orbe des Épines, de l'Ether "
  "et de l'Espace quand vous avez ramassé 25 trésors de leur pays d'origine.\n\n"

  "La probabilité d'apparition des trésors est statique dans ce mode "
  "et n'est pas augmentée en tuant des monstres.\n\n"

  "Bonne chance et amusez-vous !")

// Yendor Challenge
// ----------------

S("Yendor Challenge", "Le défi de Yendor")
S("Collect 10 treasures in various lands to unlock the challenges there",
  "Rassembler 10 trésors dans plusieurs pays pour débloquer ce défi")

S("easy", "facile")
S("challenge", "difficile")

S("Challenges do not get harder", "Le défi ne devient pas plus difficile")
S("Each challenge gets harder after each victory", "Chaque défi devient plus dur après chaque victoire.")

S(" (won!)", " (gagné !)")
S(" (won at level %1!)", " (gagné au niveau %1 !)")
S("(locked)", "(bloqué)")

S("There are many possible solutions to the Yendor Quest. In the Yendor "
  "Challenge, you will try many of them!\n\n"
  "Each challenge takes part in a specific land, and you have to use what "
  "you have available.\n\n"

  "You need to obtain an Orb of Yendor in the normal game to activate "
  "this challenge, and (ever) collect 10 treasures in one or two lands "
  "to activate a specific level.\n\n"

  "After you complete each challenge, you can try it again, on a harder "
  "difficulty level.\n\n"

  "All the solutions showcased in the Yendor Challenge work in the normal "
  "play too. However, passages to other lands, and (sometimes) some land features "
  "are disabled in the Yendor "
  "Challenge, so that you have to use the expected method. Also, "
  "the generation rules are changed slightly for the Palace "
  "and Minefield while you are looking for the Orb of Yendor, "
  "to make the challenge more balanced "
  "(but these changes are also active during the normal Yendor Quest).\n\n"

  "You get 1000 points for each challenge won, and 1 extra point for "
  "each extra difficulty level.",

  "Il y a de nombreuses solutions possibles à la Quête de Yendor. "
  "Dans le Défi de Yendor vous en essaierez beaucoup !\n\n"
  "Chaque Défi prend place dans un pays spécifique et vous devez "
  "utiliser ce qui disponible.\n\n"

  "Vous devez obtenir un Orbe de Yendor dans le jeu normal pour "
  "activer ce défi, et ramasser 10 trésors dans un ou deux pays "
  "pour activer un niveau spécifique.\n\n"

  "Après avoir réussi un défi, vous pouvez réessayer à un niveau plus difficile.\n\n"

  "Toutes les solutions au Défi de Yendor marchent dans le jeu normal. "
  "Cependant le passage vers d'autres pays, et certains fonctionnalités du pays sont "
  "désactivées pour le défi de Yendor, afin que vous utilisiez la méthode prévue. "
  "Les règles de génération sont modifiées dans le Palais et le Champ de Mines "
  "pour rendre le défi plus équilibré. "
  "(Ces changements sont aussi actifs lors de la Quête de Yendor normale).\n\n"

  "Vous obtenez 1000 points pour chaque défi gagné, et 1 point supplémentaire "
  "pour chaque niveau de difficulté en plus.")

S("Unlock this challenge by getting the Orb of Yendor!",
  "Débloquez ce défi en obtenant l'Orbe de Yendor !")

S("Collect 10 treasures in various lands to unlock the challenges there",
  "Rassemblez 10 trésors dans différents pays pour débloquer les défis")

// Wild West
// ---------

N("Wild West", GEN_M | GENF_ELISION, "Ouest Sauvage", "Ouests Sauvages", "dans l'Ouest Sauvage")
N("Outlaw", GEN_M, "Bandit" ,"Bandits", "Bandit")
N("Bounty", GEN_F, "Prime", "Primes", "Prime")
N("saloon wall", GEN_M, "mur de saloon", "murs de saloon", "mur de saloon")
N("Revolver", GEN_M, "Revolver", "Revolvers", "Revolver")

S("%The1 shoots you!", "%Le1 vous descend !")
S("You shoot %the1!", "Vous descendez %le1 !")

S("Take a revolver, kill outlaws, collect bounties.\n\n"
  "Note: since this land is anachronistic, it is not available in normal game. "
  "It is only available in special modes.",

  "Prenez un Revolver, tuez des Bandits, collectez les primes !\n\n"
  "Note : Ce pays est anachronique et disponible seulement en mode spécial.")

// Land of Storms
// --------------

S("Whenever after your move there is a connection between a charged and a "
  "grounded cell, there is a short circuit. All cells on any "
  "path connecting a charged and a grounded cell (without repeated cells, "
  "or two consecutive grounded/charged cells) become damaged.\n\n"

  "Sandstone walls and most creatures are conductive. Great Walls are "
  "isolators, but lands beyond them count as grounded.\n\n"

  "Fulgurite, the treasure, is created when you manage to short circuit "
  "a sandstone wall, or a Rich Metal Beast.\n\n"

  "Trolls leave conductive rocks when killed, and Metal Beasts can only "
  "be killed by electricity -- your attacks only stun them, or push "
  "them away if already stunned.",

  "A chaque mouvement s'il y a une connexion entre une case chargée et une "
  "à la terre, il y a un court circuit. Toutes les cases sur un chemin reliant "
  "une case chargée et à la terre (sans cases répétées, ou chargées/à la terre "
  "consécutives) sont endommagées.\n\n"

  "Les murs de grès et la plupart des créatures sont conductives. Les Grands Murs "
  "sont isolants, mais les pays autour sont à la terre.\n\n"

  "La Fulgurite, le trésor, est créé en court-circuitant un mur de grès, ou une "
  "Bête de Métal enrichi.\n\n"

  "Les Trolls laissent des roches conductives à leur mort, et les Bêtes de Métal "
  "ne peuvent être tuées que par l'électricité -- vos attaques les assomment ou les "
  "poussent quand elles sont assommées.")

N("Land of Storms", GEN_M, "Pays des Tempêtes", "Pays des Tempêtes", "dans le Pays des Tempêtes")
N("charged wall", GEN_M, "mur chargé", "murs chargés", "mur chargé")
N("grounded wall", GEN_M, "mur à la terre", "murs à la terre", "mur à la terre")
N("metal wall", GEN_M, "mur de métal", "murs de métal", "mur de métal")
N("sandstone wall", GEN_M, "mur de grès", "murs de grès", "mur de grès")
N("Storm Troll", GEN_M, "Troll des Tempêtes", "Trolls des Tempêtes", "Troll des Tempêtes")
N("Metal Beast", GEN_F, "Bête de Métal", "Bêtes de Métal", "Bête de Métal")
N("Rich Metal Beast", GEN_F, "Bête de Métal enrichi", "Bêtes de Métal enrichi", "Bête de Métal enrichi")
N("electric discharge", GEN_F, "décharge électrique", "décharges électriques", "décharge électrique")

S("There is a flash of thunder!", "Un Éclair !")

Orb("Stunning", "d'Etourdissement")

S("This Orb allows you to target monsters to stun them. "
  "10 charges are used to stun for 5 turns. Does not "
  "work against multi-tile monsters.",

  "Cet Orbe permet d'étourdir les monstres. "
  "10 charges étourdissement pendant 5 tours. Ne fonctionne "
  "pas sur les monstres occupant plus d'une case.")

S("You stun %the1!", "Vous étourdissez %le1 !")

// Overgrown Woods
// ---------------

Orb("Luck", "de Chance")

S("This Orb allows you to find new lands more easily. "
  "Lands where you have already collected less treasure, "
  "and especially the Crossroads, are more likely to "
  "spawn while you have this. Additionally, Orbs of Safety "
  "are more likely to spawn in the Whirlpool.",

  "Cet Orbe permet de découvrir de nouveaux pays plus facilement. "
  "Les pays où vous avez déjà collecté moins de trésors et en particulier "
  "les Croisements, qui sont plus susceptibles d'apparaître quand vous "
  "l'avez. De plus les Orbes de Sécurité ont plus de chances d'apparaître "
  "dans le Tourbillon.")

N("Overgrown Woods", GEN_F, "Forêt Sauvage", "Forêts Sauvages", "dans la Forêt Sauvage")
N("Mutant Ivy", GEN_M, "Lierre Mutant", "Lierres Mutants", "Lierre Mutant")
N("Mutant Sapling", GEN_F, "Pousse Mutante", "Pousses Mutantes", "Pousse Mutante")
N("Forest Troll", GEN_M, "Troll des Forêts", "Trolls des Forêts", "Troll des Forêts")

S("Forest Trolls create an impassable wall when they die.",
  "Les Trolls des Forêts créent un mur infranchissable à leur mort.")

S("The Overgrown Woods are filled with mutant ivies! These plants "
  "grow very fast. Each leaf, after being grown, can grow itself "
  "on the next turn. However, each part is only able to grow "
  "once in 16 turns. Outside of the Overgrown Woods, the Mutant Ivy "
  "may grow only on hexagonal cells.\n\n"
  "Maybe such fast growing plants could help you solve the problem "
  "of hunger in your world? Kill the Mutant Ivies to collect Mutant Saplings.",

  "La Forêt Sauvage est pleine de lierre mutant ! Ces plantes "
  "grandissent très vite. Chaque feuille, après avoir grandi peut grandir "
  "elle-même au tour suivant. Cependant chaque partie ne peut grandir qu'une "
  "fois tout les 16 tours. Hors de la Forêt Sauvage, le lierre mutant peut "
  "grandir seulement sur les cases hexagonales.\n\n"
  "Peut-être des plantes grandissant aussi vite peuvent aider à résoudre le "
  "problème de la faim dans votre monde ? Tuez le lierre mutant pour obtenir "
  "des pousses mutantes.")

S("Trees in this forest can be chopped down. Big trees take two turns to chop down.",
  "Les Arbres de cette Forêt peuvent être abattus. Les grands arbres prennent deux "
  "tours à abattre.")

S("Your power is drained by %the1!", "Votre pouvoir est drainé par %le1 !")
S("Wow! That was close.", "Wow! Ce n'est pas passé loin.")
S("Collect four different Elemental Shards!", "Rassemblez quatre Fragments Élémentaux différents !")
S("Unbalanced shards in your inventory are dangerous.",
  "Des fragments désequilibrés dans votre inventaire sont dangereux.")
S("You construct some Elemental Gems!", "Vous construisez des Gemmes Élémentales !")

N("Fulgurite", GEN_F, "Fulgurite", "Fulgurites", "Fulgurite")

S("Alternatively: kill a %1 in %the2.\n", "Alternative : tuer %un1 dans %le2.\n")


// VERSION 8.1
// ===========

// extra config
S("Prince", "Prince")
S("Princess", "Princesse")
S("cat" ,"chat")
S("dog", "chien")
S("dress color II", "couleur de vêtement II")
S("character", "caractère")

//missing texts
S("%The1 hits %the2.", "%Le1 frappe %le2.")
S("%The1 is destroyed by the Flash.", "%Le1 est détruit par l'éclair lumineux.")

// improved kill descriptions
S("parts destroyed: %1", "parties détruites : %1")
S("friends killed: %1", "alliés tués : %1")
S("monsters destroyed: %1", "monstres détruits : %1")
S("friends destroyed: %1", "alliés détruits : %1")
S("\n\nThis is a part of a monster. It does not count for your total kills.",
  "\n\nCeci est un morceau d'un monstre. Cela ne  compte pas pour le total de tués.")
S("\n\nThis is a friendly being. It does not count for your total kills.",
  "\n\nCeci est un être amical. Cela ne  compte pas pour le total de tués.")

// Overgrown Clearing
// ------------------

S("A clearing in the Overgrown Woods. Obviously, this gives "
  "the Mutant Ivies an infinite space to grow...\n\n"
  "Mutant Fruits rot if they are not adjacent to a Mutant Ivy.",

  "Une Clairière dans la Forêt Sauvage. Évidemment cela donne "
  "un espace infini pour la croissance du lierre mutant...\n\n"
  "Les fruits mutants pourrissent s'ils ne sont pas adjacents à un lierre.")

N("Clearing", GEN_F, "Clairière", "Clairière", "dans la Clairière")

N("Mutant Fruit", GEN_M, "Fruit Mutant", "Fruits Mutants", "Fruit Mutant")

N("Giant Fox", GEN_M, "Renard Géant", "Renards Géants", "Renard Géant")

S("What is freedom for you? A situation when you can walk wherever you want? "
  "Or a situation when you do not have to work, since you have as much tasty food "
  "as you want?\n\n"
  "Well, this creature has chosen the second option. It won't be happy "
  "if you destroy its prison.\n",

  "Qu'est ce que la liberté pour vous ? Une situation où vous pouvez aller où vous voulez ? "
  "Ou une situation où vous n'avez pas à travailler, car vous avez toute la nourriture "
  "délicieuse que vous voulez ?\n\n"
  "Cette créature a choisi la seconde option. Elle ne sera pas contente que vous "
  "détruisiez sa prison.")

Orb("Freedom", "de Liberté")

S("This orb is activated if you are unable to escape (radius 4) "
  "without making illegal moves or "
  "going through cells which are currently adjacent to enemy monsters. "
  "Most game over situations are covered by this, but generally, "
  "this orb is oversensitive...\n\n"
  "When activated, it creates a Flash effect of radius 5.",

  "Cet orbe s'active si vous ne pouvez pas vous échapper (rayon 4), "
  "sans mouvements interdits ou en passant par des cases adjacentes "
  "à des monstres. La plupart des situations sont couvertes par ceci, "
  "mais en général cet orbe est trop sensible. "
  "Une fois activée, elle crée un effet d’Éclair de rayon 5.")

S("Your %1 activates!", "Votre %1 s'active !")


// Haunted Woods
// -------------

S("You become a bit nervous...", "Vous devenez un peu nerveux...")
S("Better not to let your greed make you stray from your path.",
  "Mieux vaut ne pas laisser votre convoitise vous détourner de votre chemin !")

N("Haunted Woods", GEN_F, "Forêt Hantée", "Forêts Hantées", "dans la Forêt Hantée")

S("A dark forest filled with ghosts and graves. But there is also treasure hidden "
  "deep within... But don't let greed make you stray from your path, as "
  "you can get lost!\n\n"
  "The Haunted Woods are bounded by a single equidistant curve. It is not a circle or horocycle.\n\n",

  "Une sombre forêt pleine de fantômes et de tombes. Mais il y a aussi un trésor "
  "caché au plus profond des sous-bois... Mais ne laissez pas la convoitise vous "
  "détourner de votre chemin, car vous risquez de vous perdre !\n\n"
  "La Forêt Hantée est entourée par une simple courbe équidistante. Ce n'est ni un cercle ni un horocycle.\n\n")


N("Black Lotus", GEN_M, "Lotus Noir", "Lotus Noirs", "Lotus Noir")

S("This beautiful flower is greatly prized by wizards, as it allows them to cast powerful magical spells "
  "without preparation.\n",

  "Cette fleur magnifique est très prisée par les magiciens, car elle leur permet de jeter de puissants "
  "sortilèges sans préparation.\n")

Orb("Undeath", "de Non-Mort")

S("Monsters slain by you in melee are turned into friendly ghosts. "
  "Does not affect plants and friends.",

  "Les monstres que vous tuez en combat rapproché deviennent des fantômes alliés. "
  "N'agit pas sur les plantes et les alliés.")

N("Friendly Ghost", GEN_M, "Fantôme Allié", "Fantômes Alliés", "Fantôme Allié")

S("Friendly ghosts are friendly beings who can go through any obstacles. However, "
  "unlike most friends, they tend to fly away from you.",

  "Les Fantômes Alliés sont des êtres amicaux qui peuvent traverser n'importe quel "
  "obstacle. Contrairement aux autres alliés, ils ont tendance à s'éloigner de vous.")


// Windy Plains
// ------------

N("Windy Plains", GEN_F, "Plaine Venteuse", "Plaines Venteuses", "dans la Plaine Venteuse")

N("fan", GEN_M, "ventilateur", "ventilateurs", "ventilateur")

S("Someone has put air fans in these plains, causing strong winds everywhere. "
  "You think that the purpose is to harness the magical power of Air Elementals, but "
  "you are not sure.\n\n"
  "All cells except fans are grouped into three colors according to a pattern. "
  "Wind blows counterclockwise around each group of cells of a single color. "
  "Cells which are blocked by walls, or at distance at most 2 from an Air Elemental, "
  "do not count for this.\n\n"
  "It is illegal to move in a direction which is closer to incoming wind than to "
  "outgoing wind. However, you can move two cells with the wind in a single turn, "
  "and so can the birds.",

  "Quelqu'un a placé des ventilateurs dans ces plaines, créant de forts vents partout. "
  "Vous pensez que le but est de récolter le pouvoir magique des Élémentaux d'Air, mais "
  "vous n'êtes pas sûr'.\n\n"

  "Toutes les cases sauf les ventilateurs sont groupées en trois couleurs suivant un "
  "motif. Le vent souffle dans le sens horaire autour de chaque groupe de cases d'une "
  "seule couleur. Les cases sont bloquées par des murs, ou à une distance d'au plus 2 à "
  "d'un Élémental d'Air, ne comptent pas pour cela.\n\n"
  "Il est interdit d'aller dans la direction plus proche du vent arrivant que sortant. "
  "Cependant vous pouvez bouger de deux cases dans le sens du vent en un tour comme "
  "les oiseaux.")

N("White Dove Feather", GEN_F, "Plume de la Blanche Colombe", "Plumes de la Blanche Colombe", "Plume de la Blanche Colombe")
S("This feather is truly beautiful and strong.",
  "Cette Plume est vraiment belle et forte.")

N("Wind Crow", GEN_M, "Corbeau du Vent", "Corbeaux du Vent", "Corbeau du Vent")
S("A large bird who likes strong winds. Just as you, it can fly quickly in the wind.",
  "Un grand oiseau qui aime les vents forts. Comme vous il peut voler rapidement dans le vent.")

// Orb of Empathy replaces Orb of Air on the Beach
Orb("Empathy", "d'Empathie")
S(" %1", " %1") // ???
S(", %1", ", %1") // ???

S("This Orb lets your allies share your Orb powers.\n\n"
  "The following Orbs are affected:",

  "Cet Orbe permet de partager les pouvoirs d'Orbe avec vos alliés.\n\n"
  "Les Orbes suivantes sont concernés :")

N("strong wind", GEN_M, "vent fort", "vents forts", "vent fort")

S("In the Windy Plains, you can let the wind carry you, "
  "causing you to move two cells with the wind in a single turn. "
  "This cannot be done if you are standing at distance at most 2 "
  "from the Air Elemental, or if any of the three cells on the way "
  "has two wind directions.\n\n"
  "Press 't' or click the destination to activate.",

  "Dans les plaines venteuses, vous pouvez laisser le vent vous porter, "
  "ce qui vous permet de bouger de deux cases en un tour. "
  "Ceci ne fonctionne pas à une distance d'au plus 2 d'un Élémental d'Air, "
  "ou si une des trois cases sur le chemin ont deux directions de vent.\n\n "
  "Presser 't' ou cliquer la destination pour activer.")

S("Return to the normal game", "Retour au jeu normal")
S("three players", "trois joueurs")
S("four players", "quatre joueurs")
S("key Orb power", "pouvoir d'Orbe crucial")

S("Orb power (target: mouse)", "Pouvoir de l'Orbe (cible : souris)")
S("Orb power (target: facing)", "Pouvoir de l'Orbe (cible : en face)")
S("You cannot go against the wind!", "Vous ne pouvez pas aller contre le vent !")


// VERSION 8.2
// ===========

S("The ivy attacks %the1!", "Le lierre attaque %le1 !")

// heptagonal mode
// ---------------

S("heptagonal mode", "mode heptagonal")

S("\n\n(For the heptagonal mode, the radius has been reduced to 2 for closing plates.)", //(i.e. Palast)
  "\n\n(Pour le mode heptagonal, le rayon a été réduit à 2 pour les plaques de fermeture.)")// Palais

// Hypersian Rug mode
// ------------------

S("hypersian rug mode", "mode tapis hypersan")

S("In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
  "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n",
  "Dans ce mode, HyperRogue est joué sur un modèle 3D d'un morceau de plan hyperbolique, "
  "similaire à celui que vous obtiendriez avec le 'créateur de modèle papier' ou par "
  "crochetage hyperbolique.\n\n")

S("This requires some OpenGL extensions and may crash or not work correctly -- enabling "
  "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
  "will make the rendering faster, but the surface will be rendered only once, so "
  "you won't be able to play a game on it.\n\n",
  "Ce mode nécessite des extensions OpenGL et peut crasher ou ne pas fonctionner correctement "
  "-- activer l'option 'afficher les textures sans OpenGL' peut aider. "
  "L'option 'afficher les textures seulement une fois' rendra l'affichage plus rapide, mais la "
  "surface ne sera affichée qu'une fois donc vous ne pourrez pas jouer à un jeu dessus.\n\n")

S("Use arrow keys to rotate, Page Up/Down to zoom.",
  "Utiliser les touches flèches pour pivoter, Page Haut/Bas pour zoomer.")

S("what's this?", "qu'est ce que c'est ?")
S("enable the Hypersian Rug mode", "activer le mode tapis hypersan")
S("render the texture only once", "afficher les textures seulement une fois")
S("render texture without OpenGL", "afficher les textures sans OpenGL")
S("texture size", "taille de la texture")

// Crossroads IV & Chaos Mode
// --------------------------

N("Crossroads IV", GEN_M, "Croisement IV", "Croisements IV", "sur le Croisement IV")

S("Chaos mode", "mode Chaos")

S("In the Chaos mode, lands change very often, and there are no walls between them. "
  "Some lands are incompatible with this."
  "\n\nYou need to reach Crossroads IV to unlock the Chaos mode.",

  "Dans le mode Chaos, les pays changent très souvent, et il n'y a pas de murs entre eux. "
  "Certains pays sont incompatibles avec ceci."
  "\n\nVous devez atteindre les Croisements IV pour débloquer le mode Chaos.")

// Rose Garden
// -----------

N("Rose Garden", GEN_F, "Roseraie", "Roseraies", "dans la Roseraie")
N("False Princess", GEN_F, "Fausse Princesse", "Fausses Princesses", "Fausse Princesse")
N("False Prince", GEN_M, "Faux Prince", "Faux Princes", "Faux Prince")
N("Rose Lady", GEN_F, "Dame aux Roses", "Dames aux Roses", "Dame aux Roses")
N("Rose Lord", GEN_M, "Seigneur aux Roses", "Seigneurs aux Roses", "Seigneur aux Roses")
N("Rose Beauty", GEN_F, "Beauté aux Roses", "Beautés aux Roses", "Beauté aux Roses")
N("Handsome Gardener", GEN_M, "Jardinier Séduisant", "Jardiniers Séduisants", "Jardinier Séduisant")
N("rosebush", GEN_M, "Rosier", "Rosiers", "Rosier")

Orb("Beauty", "de Beauté")

N("Thornless Rose", GEN_F, "Rose sans épines", "Roses sans épines", "Rose sans épines")

S("Don't be fooled by this red-haired girl, or you will be stabbed if you come too close!",
  "Ne soyez pas dupe de cette fille aux cheveux rouges, ou vous serez poignardé si vous vous approchez !")
S("Don't be fooled by this red-haired boy, or you will be stabbed if you come too close!",
  "Ne soyez pas dupe de ce garçon aux cheveux rouges, ou vous serez poignardé si vous vous approchez !")
S("This false princess is immune to the alluring scent of roses.",
  "Cette fausse Princesse est immunisée contre l'odeur entêtante des roses.")
S("This false prince is immune to the alluring scent of roses.",
  "Ce faux Prince est immunisé contre l'odeur entêtante des roses.")
S("She has flowers in her long fair hair. You could not bring yourself to attack such a beautiful woman.",
  "Elle porte des fleurs dans ses longs cheveux clairs. Vous ne pouvez vous résoudre à attaquer une si belle femme.")
S("Tall, strong, and holding a flower in his hand. You could not bring yourself to attack such a handsome man.",
  "Grand, fort, avec une fleur à la main. Vous ne pouvez vous résoudre à attaquer un si bel homme. ")

S("This Orb makes you stunningly beautiful. "
  "Monsters which come next to you will be stunned for one turn. "
  "Multi-tile monsters are not affected. Additionally, it makes you immune to "
  "beauty.",

  "Cet Orbe vous confère une beauté stupéfiante. "
  "Les monstres qui viennent près de vous seront stupéfait pour un tour. "
  "Les monstres multi-cases ne sont pas affectés. De plus vous êtes immunisé%e0 "
  "à la 'Beauté'.")

S("A big, beautiful, magical flower.", "Une fleur grande, belle et magique.")

S("Each eight turns, each rosebush at distance at most 5 from you will "
  "release a wave of alluring scent. Creatures on the frontwave "
  "will move towards where the scent came from. Even if it causes them "
  "to attack their friends or beautiful creatures, or move into water, fire, chasm, or thorns of the rosebush. "
  "Ivies, Ghosts, Rock Snakes, Rose Ladies and Lords, and monsters restricted to a specific "
  "terrain are immune to scents.",

  "Tout les huit tours, chaque rosier à une distance de 5 au plus de vous "
  "émet un nuage d'odeur entêtante. Les créatures sur le nuage iront vers "
  "là d'où vient l'odeur. Même si cela leur fait attaquer leurs amis, de belles "
  "créatures, ou aller dans l'eau, le feu, les gouffres, ou les épines du rosier. "
  "Les lierres, les fantômes, les serpents des roches, les dames et seigneurs aux "
  "roses, et les monstres restreints à un terrain spécifique sont immunisés aux odeurs.")

S("This land is filled with beautiful, but dangerous, creatures and plants.",
  "Ce pays est plein de créatures et plantes magnifiques mais dangereuses.")

S("%The1 is killed by thorns!", "%Le1 est tué par des épines !")

S("You just cannot stand in place, those roses smell too nicely.",
  "Vous ne pouvez pas tenir en place, ces roses sentent trop bon.")
S("Those roses smell too nicely. You have to come towards them.",
  "Ces roses sentent trop ben. Vous devez vous en rapprocher.")


// Warped Sea/Coast
// ----------------

N("Warped Coast", GEN_F, "Côte Déformée", "Côtes Déformées", "dans la Côte Déformée")
N("Warped Sea", GEN_F, "Mer Déformée", "Mers Déformées", "sur la Mer Déformée")
N("Ratling", GEN_M | GENF_ELISION, "Homme-Rat", "Hommes-Rats", "Homme-Rat")
N("Coral", GEN_M, "Corail", "Coraux", "Corail")

Orb("the Warp", "de Déformation")

S("This land is warped. Ironically, the coast is completely straight...",
  "Ce pays est déformé. Ironiquement, la côte est complètement droite...")

S("This part of the world is warped, restricting the movement somewhat. "
  "\"Diagonal\" movement and attacking between triangular cells is not allowed. "
  "Flash, Storms, and Freedom spells ignore this, and Ghosts can move, attack, and "
  "be attacked diagonally.",
  "Cette partie du monde est déformée, restreignant le mouvement. "
  "\"Diagonal\" bouger et attaquer entre les cellules triangulaires n'est pas permis. "
  "Les sorts d'éclair, de tempêtes, et de liberté font exception, ainsi que les fantômes "
  "qui peuvent attaquer et bouger en diagonale.")

S("This Orb creates a warped zone of radius 5 around you, "
  "and also allows you to move diagonally in warped zones.",

  "Cet Orbe crée une zone déformée d'un rayon de 5 autour de vous "
  "et vous permet aussi de bouger en diagonale dans les zones déformée.")

S("Corals have a somewhat hyperbolic structure even in your home world, "
  "but natural corals from the Warped Sea have truly beautiful shapes. "
  "Ratlings know the value of corals, and thus keep them in boats for safety.",

  "Les Coraux ont une structure en quelque sorte hyperbolique même dans votre monde natal, "
  "mais les coraux naturels de la mer déformée ont des formes réellement magnifiques. "
  "Les Hommes-Rats connaissent la valeur des coraux, et les gardent donc dans leurs bateaux par sécurité.")

S("These warped humanoids are skilled warriors and sailors, and they "
  "feel at home at the Warped Coast. Their battle experience has taught them "
  "that enemies who wait without moving or attacking anything are the most deadly. "
  "If they see such an enemy, they become extremely suspicious, and they also wait.",

  "Ces humanoïdes déformés sont des guerriers et marins compétents, et ils se "
  "sentent chez eux sur la Côte Déformée. Leur expérience du combat leur a appris "
  "que les ennemis qui attendent sans bouger ou attaquer sont les plus mortels. "
  "S'ils voient un tel ennemi, ils deviennent très soupçonneux et attendent aussi.")


S("hexagonal #1", "hexagonal #1")
S("You cannot move between the triangular cells here!",
  "Vous ne pouvez pas bouger entre cellules triangulaires !")
S("display the triheptagonal grid", "Montrer la grille tri-heptagonale")

N("no wall", GEN_M | GENF_PROPER, "pas de mur", "pas de murs", "pas de mur")
N("no monster", GEN_M | GENF_PROPER, "pas de monstre", "pas de monstres", "pas de monstre")
N("no item", GEN_M | GENF_PROPER, "pas d'objet", "pas d'objets", "pas d'objet")

S("\n\nA Ghost never moves to a cell which is adjacent to another Ghost of the same kind.",
  "\n\nUn Fantôme ne va jamais sur une case adjacente à un autre Fantôme du même type.")

S("You cannot attack diagonally!", "Vous ne pouvez pas attaquer en diagonale !")

// for later...

Orb("Energy", "d'Énergie")

S("This Orb halves the power usage of orbs which cost some "
  "charges with each activation. It even affects the "
  "one-shot orbs such as Flash or Teleport. If such an activation "
  "normally costs x charges, it costs only x/2 (rounded up) "
  "if you have an Orb of Energy.",

  "Cet Orbe divise par deux l'usage des orbes qui coûtent des "
  "charges à chaque activation. Cela affecte même les orbes à usage "
  "unique comme l'éclair et la téléportation. Si une telle activation "
  "coûte normalement x charges, cela coûte seulement x/2 charges (arrondi),  "
  "si vous avez un Orbe d'Énergie.")

// missing....

N("warp gate", GEN_F, "porte déformée", "portes déformées", "porte déformée")
S("This gate separates the warped area from the normal land.",
  "Cette porte sépare la partie déformée du reste du pays.")
S("You cannot move between the cells without dots here!",
  "Vous ne pouvez pas bouger entre les cases sans points ici !")
S("Those roses smell too nicely. You can only target cells closer to them!",
  "Ces roses sentent trop bon. Vous pouvez seulement aller sur les cases plus proches d'elles !")
S("You need to move to give space to %the1!",
  "Vous avez besoin de bouger pour faire de la place à %le1 !")

N("Ratling Avenger", GEN_M | GENF_ELISION, "Homme-Rat Vengeur", "Hommes-Rats Vengeurs", "Homme-Rat Vengeur")

S("So, you have killed a Ratling on the unwarped sea? You will be punished for this! "
  "Luckily, if you run away from the Warped Sea quickly, the Ratling Avengers will lose track of you.",

  "Alors, vous avez tué un Homme-Rat sur la mer non-déformée ? Vous serez puni pour ça ! "
  "Avec un peu de chance, si vous fuyez la Mer Déformée assez vite, les Hommes-Rats Vengeurs "
  "perdront votre trace.")


// VERSION 8.3
// ===========

S("Kills required: %1 (%2).\n", "Nombre de tués nécessaire : %1 (%2).\n")

S("\"Be careful in the Rose Garden! It is beautiful, but very dangerous!\"",
  "\"Faites attention dans la Roseraie ! Elle est belle mais très dangereuse !\"")

S("\"Thank you very much for talking, and have a great rest of your day!\"",
  "\"Merci beaucoup pour la discussion, passez une bonne journée !\"")

// Conformal/history mode
// ----------------------

S("conformal/history mode", "mode conforme/histoire")
S("Preparing the line (%1/1000)...", "Préparation de la ligne (%1/1000)...")
S("Could not create an image of that size.", "Impossible de créer une image de cette taille.")
S("Saved the band image as: ", "Sauvegarder l'image ruban en tant que : ")
S("right", "droite")
S("left", "gauche")
S("up", "haut")
S("down", "bas")
S("counterclockwise", "sens antihoraire")
S("clockwise", "sens horaire")
S("zoom out", "dézoomer")
S("zoom in", "zoomer")
S("left to right", "de gauche à droite")
S("right to left", "de droite à gauche")
S("spin down", "tourner vers le haut")
S("spin up", "tourner vers le bas")
S("disk", "disque")
S("half-plane", "demi-plan")
S("band", "ruban")
S("polygonal", "polygonal")
S("polynomial", "polynomial")
S("include history", "inclure l'histoire")
S("model used", "modèle utilisé")
S("rotation", "rotation")

S("coefficient", "coefficient")
S("which coefficient", "quel coefficient")
S("polygon sides", "côté de polygone")
S("star factor", "facteur étoile")
S("degree of the approximation", "degré d'approximation")
S("prepare the line animation", "préparer l'animation de la ligne")
S("animation speed", "vitesse d'animation")
S("render bands automatically", "afficher les rubans automatiquement")
S("band width", "largeur de ruban")
S("length of a segment", "longueur d'un segment")
S("spiral on rendering", "afficher en spirale ")
S("render now (length: %1)", "afficher maintenant (longueur : %1)")
S("Enable cheat mode or GAME OVER to use this", "Activer le mode triche ou GAME OVER pour utiliser cette fonctionnalité")

S("see http://www.roguetemple.com/z/hyper/models.php", "voir http://www.roguetemple.com/z/hyper/models.php (en anglais)")


// Yendorian Forest
// ----------------

N("Yendorian Forest", GEN_F, "Forêt de Yendor", "Forêts de Yendor", "dans la Forêt de Yendor")

S("This forest was planted by one of the wizards from the Ivory Tower "
  "to conduct experiments with gravity.",

  "Cette forêt a été plantée par un des magiciens de la Tour d'Ivoire "
  "pour conduire des expériences sur la gravité.")

N("Yendorian Researcher", GEN_M, "Chercheur Yendorien", "Chercheurs Yendoriens", "Chercheur Yendorien")

S("These people study gravity and infinite trees. "
  "They have no special features, other than wearing a strange hat.",

  "Ces gens étudient la gravité et les arbres infinis. "
  "Ils n'ont pas de caractéristique particulière en dehors de leur chapeau étrange.")

N("Sparrowhawk", GEN_M | GENF_ELISION, "Épervier", "Éperviers", "Épervier")

S("A bird who hunts in the treetops of the Yendorian Forest.",
  "Un oiseau qui chasse dans les cimes de la Forêt de Yendor.")

N("Apple", GEN_F, "Pomme", "Pommes", "Pomme")

S("A fruit from the Yendorian Forest.", "Un fruit de la Forêt de Yendor.")

N("trunk", GEN_M, "tronc", "troncs", "tronc")

S("The skeleton of a tree.", "Le squelette d'un arbre.")

N("solid branch", GEN_F, "branche solide", "branches solides", "branche solide")

S("Branches here could bear your weight easily.",
  "Ces branches peuvent porter votre poids facilement.")

N("weak branch", GEN_F, "branche fragile", "branches fragiles", "branche fragile")

S("Branches here will bear you weight, but if you use them to move (not fall) to an unstable place, they will break.",
  "Ces branches peuvent porter votre poids, mais si vous les utiliser pour aller (sans tomber) à un endroit instable, elles se briseront.")

N("canopy", GEN_F, "canopée", "canopées", "canopée")

S("Only thin twigs and leaves here. They may bear fruits, but for you, these cells count as unstable.",
  "Seulement de fins rameaux et des feuilles ici. Ils peuvent porter des fruits, mais pour vous ces cases sont instables.")


// Dragon Chasms
// -------------

N("Dragon Chasms", GEN_M, "Gouffre du Dragon", "Gouffres du Dragon", "dans le Gouffre du Dragon")

N("Dragon", GEN_M, "Dragon", "Dragons", "Dragon")

N("Dragon Scale", GEN_F | GENF_ELISION, "Écaille de Dragon", "Écailles de Dragon", "Écaille de Dragon")

S("Dragon Scales are a prized material for armors. "
  "They are also prized by collectors, who would like to boast "
  "about how they have killed a Dragon.\n\n"
  "Dragon Scales disappear after 500 turns.",

  "Les Écailles de Dragon sont un matériel prisé pour les armures. "
  "Elles sont aussi prisée par les collectionneurs, qui aimeraient "
  "se vanter d'avoir tué un Dragon.\n\n"
  "Les Écailles de Dragon disparaissent après 500 tours.")

S(
  "Dragons are powerful monsters. They are slow, but evil, "
  "and love to pick on creatures who are even slower than "
  "them. They must be stopped!\n\n"

  "A Dragon moves each two turns. It may attack with all its segments, "
  "or move its whole body forwards or "
  "backwards, it may also move a frontal part backwards. To kill a Dragon, "
  "you need to hit each of its segments. "
  "The head will regenerate on the "
  "turns the Dragon is not moving, so you will usually have to hit it with "
  "your last attack; otherwise, if the head is healthy, it may breathe "
  "fire (at range 3), losing the hitpoint. Killing the Dragon "
  "while still in the Dragon Chasms gives you treasure.",

  "Les Dragons sont des monstres puissants. Ils sont lents, mais malfaisants, "
  "et aiment attaquer les créatures encore plus lentes qu'eux. Ils doivent "
  "être arrêtés !\n\n"

  "Un Dragon bouge tout les deux tours. Il peut attaquer avec tout ses segments "
  "ou bouger tout son corps en avant ou en arrière, ou une partie frontale en "
  "arrière. Pour tuer un Dragon, vous devez frapper chacun de ses segments. "
  "La tête se régénère au tour où le Dragon ne bouge pas, donc il faudra la frapper "
  "en dernier ; autrement, si la tête est en bonne santé, elle peut cracher du feu "
  "(rayon 3), ce qui fait perdre le point de vie. Tuer le Dragon dans le Gouffre du "
  "Dragon donne un trésor.")

S("%The1 breathes fire at you!", "%Le1 crache du feu sur vous !")
S("%The1 breathes fire!", "%Le1 crache du feu !")

Orb("Domination", "de Domination")

S("This Orb lets you ride Dragons and other worm-like creatures. "
  "Simply move onto such a creature to ride them; while riding, you are protected from dangerous terrains "
  "and partially from attacks (they cause you to lose half of your Domination power), "
  "but you cannot collect items.\n\n"
//  "When only one charge is left, "
//  "you have to dismount this turn -- be very careful to make this possible, "
//  "as your mount could attack you immediately!\n\n"
  "While riding, "
  "click on a location to order your mount to move or attack there.",

  "Cet Orbe permet de chevaucher les Dragons et d'autres créatures semblables à des vers. "
  "Allez vers une telle créature pour la chevaucher ; sur son dos, vous êtes protégé%e0 des terrains dangereux "
  "et partiellement des attques (elles vous font perdre la moitié de votre pouvoir de Domination). "
  "mais vous ne pouvez pas ramasser des objets.\n\n"
//  "Quand il ne reste qu'une charge, "
//  "vous devez descendre ce tour -- faites attention à ce que ce soit possible, "
//  "car votre monture vous attaquera immédiatement !\n\n"
  "Pendant que vous chevauchez, cliquez un endroit pour ordonner à votre monture "
  "d'attaquer ou d'aller là-bas.")

S("You command %the1!", "Vous commandez %le1 !")
S("You need to dismount %the1!", "Vous devez descendre de %le1 !")
S("%The1 tries to dismount you!", "%Le1 essaie de vous éjecter !")

S("Enemies killed: %1 (%2 types)", "Ennemis tués : %1 (%2 types)")
S(" kills: %1/%2", " Tués : %1/%2")
S("Different kills required: %1.\n", "Différents tués requis : %1.\n")


// Galapagos
// ---------

N("Galápagos", GEN_M, "Galápagos", "Galápagos", "dans les Galápagos")

N("Baby Tortoise", GEN_F, "Bébé Tortue", "Bébés Tortue", "Bébé Tortue")

Orb("the Shell", "de Carapace")

S("This Orb protects you from physical attacks. "
  "It lasts for more turns than the Orb of Shielding, but "
  "10 charges are lost whenever you are attacked. "
  "It also does not protect you from fires, scents, and being eaten.",

  "Cet Orbe vous protège des attaques physiques. "
  "Il dure plus de tours que l'Orbe de Protection, "
  "mais perd 10 charges à chaque attaque."
  "Il ne protège pas du feu, des odeurs, et d'être mangé.")

S("Galápagos is the land of Tortoises. "
  "They are very slow, which allows the Dragons to pick on them by "
  "stealing and eating their young. Bring the Baby Tortoises back, "
  "but, there is a catch: the Tortoises come in many varieties, depending "
  "on the part of Galápagos they live in -- there are 21 binary environmental "
  "factors, and thus "
  "2097152 varieties. You'll have to find a "
  "Tortoise which matches the baby exactly!\n\n"
  "Tortoises move each 3 turns, and attacks only stun them.\n\n"
  "Bringing back a Baby Tortoise counts as 5 $$$. The more factors agree in "
  "the given location of Galápagos, the brighter it is shown on your screen.",

  "Les Galápagos sont le pays des Tortues. "
  "Elles sont très lentes, ce qui permet aux Dragons de s'en prendre à elles "
  "en volant et mangeant leur petits. Ramenez les Bébés Tortue mais attention : "
  "Il en existe de nombreuses variétés selon la région des Galápagos où elles vivent -- "
  "il y a 21 facteurs environnementaux binaires, et donc 2097152 variétés. "
  "Vous devez trouver une Tortue qui correspond exactement au bébé !\n\n"
  "Les Tortues bougent tout les 3 tours, les attaques ne font que les assommer.\n\n"
  "Ramener un bébé Tortue rapporte 5 $$$. Plus les facteurs sont en accord avec "
  "la région des Galápagos, plus elle est brillante sur l'écran.")

S("%The1 scares %the2 a bit!", "%Le1 effraie un peu %le2 !")
S("%The1 attacks your shell!", "%Le1 attaque votre carapace !")

S("Hyperstone Quest: collect at least %3 points in %the2",
  "Quête des Hyperpierres : rassemblez au moins %3 points %a2")

S("animals killed: %1", "animaux tués : %1")
S("\n\nTortoises are not monsters! They are just annoyed. They do not count for your total kills.",
  "\n\nLes Tortues ne sont pas des monstres ! Elles sont justes agacées. Elles ne comptent pas pour votre total de tués.")

S("Aww, poor %1... where is your family?", "Oh, pauvre %1... où est ta famille ?")
S("You are now a tortoise hero!", "Vous êtes maintenant un héros des Tortues !")
S("Find a %1 in %the2.", "Trouvez un %1 %a2.")

/*
                "NEW_ACHIEVEMENT_6_27_NAME"     "Dragon Slayer"
                "NEW_ACHIEVEMENT_6_27_DESC"     "Collect a Dragon Scale."
                "NEW_ACHIEVEMENT_6_28_NAME"     "Dragon Lord"
                "NEW_ACHIEVEMENT_6_28_DESC"     "Collect 10 Dragon Slayers."
                "NEW_ACHIEVEMENT_6_29_NAME"     "Dragon Prince"
                "NEW_ACHIEVEMENT_6_29_DESC"     "Collect 25 Dragon Scales."
                "NEW_ACHIEVEMENT_6_30_NAME"     "Dragon King"
                "NEW_ACHIEVEMENT_6_30_DESC"     "Collect 50 Dragon Scales."
                "NEW_ACHIEVEMENT_6_31_NAME"     "Tortoise Hero"
                "NEW_ACHIEVEMENT_6_31_DESC"     "Save two Baby Tortoises."
                "NEW_ACHIEVEMENT_7_0_NAME"      "Tortoise Champion"
                "NEW_ACHIEVEMENT_7_0_DESC"      "Save five Baby Tortoises."
                "NEW_ACHIEVEMENT_7_1_NAME"      "Tortoise Ninja"
                "NEW_ACHIEVEMENT_7_1_DESC"      "Save ten Baby Tortoises."
                "NEW_ACHIEVEMENT_7_2_NAME"      "Reached the Yendorian Forest"
                "NEW_ACHIEVEMENT_7_2_DESC"      "Find and collect an Apple."
                "NEW_ACHIEVEMENT_7_3_NAME"      "Yendorian Researcher"
                "NEW_ACHIEVEMENT_7_3_DESC"      "Collect 10 Apples."
                "NEW_ACHIEVEMENT_7_4_NAME"      "Sparrowhawk"
                "NEW_ACHIEVEMENT_7_4_DESC"      "Collect 25 Apples."
                "NEW_ACHIEVEMENT_7_5_NAME"      "Yendorian Master"
                "NEW_ACHIEVEMENT_7_5_DESC"      "Collect 50 Apples."
*/

S("(includes co-op)", "(inclut co-op)")
S("Revived!", "Ressuscité !")

S("You hear a distant roar!", "Vous entendez un rugissement au loin !")

S("turn count = %1 last exploration = %2 ghost timer = %3",
  "nombre de tours = %1 dernière exploration = %2 durée des fantômes = %3")

S("You damage %the1 with a mental blast!", "Vous endommagez %le1 avec une attaque mentale !")

S("You are now a tortoise heroine!", "Vous devenez un héros des Tortues !")

S("Note: currently scores are saved only in the normal mode on Android",
  "Note : actuellement les scores ne sont stockés qu'en mode normal sous Android")
S("Note: you can play, but scores won't be saved on Android",
  "Note : vous pouvez jouer, mais les scores ne seront pas stockés sous Android")
S("settings set here won't be saved", "les réglages fait ici ne seront pas enregistrés")
S("-- use the Android menu instead", "-- utilisez le menu Android à la place")

S("summon Bonfire", "invoquer un Feu de Joie")
S("Hyperstone Quest", "Quête des Hyperpierres")
S("summon dead orbs", "invoquer des orbes morts")
S("summon a Monster", "invoquer un Monstre")
S("gain orb powers", "gagner des pouvoirs d'orbe")
S("summon a Golem", "invoquer un Golem")
S("summon Thumpers", "invoquer un Marteleur")
S("summon Ivy", "invoquer du Lierre")
S("lose all treasure", "perdre tout les trésors")
S("gain kills", "gagner des tués")
S("Select the land ---", "Choisir le pays ---")
S("summon Mimics", "invoquer des Mimiques")
S("summon orbs", "invoquer des orbes")
S("deplete orb powers", "épuiser les pouvoirs d'orbe")
S("Safety (quick save)", "Sécurité (sauvegarde rapide)")
S("summon treasure", "invoquer un trésor")
S("summon lots of treasure", "invoquer beaucoup de trésor")
S("--- and teleport there", "--- et se téléporter là-bas")
S("summon Sand Worm", "invoquer un vers des sables")
S("summon Orb of Yendor", "invoquer un Orbe de Yendor")
S("rotate the character", "pivoter le caractère")

S("gain Orb of Yendor", "gagner un Orbe de Yendor")
S("switch ghost timer", "éteindre/allumer la durée de vie des fantômes")
S("switch web display", "éteindre/allumer l'affichage web")


// just copy: "player 5 X"...
S("configure player 5", "configurer joueur 5")
S("configure player 6", "configurer joueur 6")
S("configure player 7", "configurer joueur 7")

S("player 5 X", "joueur 5 X")
S("player 5 Y", "joueur 5 Y")
S("player 5 go", "joueur 5 part")
S("player 5 spin", "joueur 5 tourne")
S("player 6 X", "joueur 6 X")
S("player 6 Y", "joueur 6 Y")
S("player 6 go", "joueur 6 part")
S("player 6 spin", "joueur 6 tourne")
S("player 7 X", "joueur 7 X")
S("player 7 Y", "joueur 7 Y")
S("player 7 go", "joueur 7 part")
S("player 7 spin", "joueur 7 tourne")


// VERSION 9.0
// ===========

// extra descriptions for Orbs and some other things
// -------------------------------------------------

S("\n\nThis Orb is triggered on your first attack or illegal move.",
  "\n\nCet Orbe est déclenché par votre première attaque ou mouvement "
  "interdit.")

S("\nYou can never target cells which are adjacent to the player character, "
  "or ones out of the sight range.",
  "\nVous ne pouvez jamais cibler des cases adjacentes au joueur, ou hors de "
  "son champ de vision.")

S("\nThis is a ranged Orb. ", "\nCeci est un Orbe qui fonctionne à distance.")

S("\nRanged Orbs can be targeted by long touching the desired location.",
  "\nLes Orbes à distance peuvent être déclenchés en touchant longtemps "
  "l'emplacement souhaité.")

S("\nRanged Orbs can be targeted by touching the desired location.",
  "\nLes Orbes à distance peuvent être déclenchés en touchant l'emplacement souhaité.")

S("\nRanged Orbs can be targeted by shift-clicking the desired location. ",
  "\nLes Orbes à distance peuvent être déclenchés en shift-cliquant l'emplacement souhaité. ")

S("\nRanged Orbs can be targeted by clicking the desired location. ",
  "\nLes Orbes à distance peuvent être déclenchés en cliquant l'emplacement souhaité. ")

S("\n\nOnce you collect 10 Bomberbird Eggs, "
  "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
  "Collecting even more Eggs will increase the radius. Additionally, collecting "
  "25 Bomberbird Eggs will reveal adjacent cells even in your future games.",

  "\n\nLorsque vous avez collecté 10 Œufs d'Oiseaux-Bombe, marcher sur "
  "une case sans mine adjacente révèle aussi les cases adjacentes. "
  "Rassembler plus d’œufs augmentera le rayon. De plus, collecter 25 Œufs "
  "révélera les cases adjacentes même dans les parties futures.")

S("This might be very useful for devices with limited memory.",
  "Ceci peut être très utile pour les appareils à mémoire limitée.")

S("(in the MENU). You can reduce the sight range, this should make the animations smoother.",
  "(dans le MENU). Vous pouvez réduire le champ de vision, cela devrait rendre les animations plus fluides.")

S("Unavailable in the shmup mode.\n", "Non disponible en mode shoot'em up.\n")
S("Unavailable in the multiplayer mode.\n", "Non disponible en mode multijoueur.\n")
S("Unavailable in the Chaos mode.\n", "Non disponible en mode Chaos.\n")
S("Bonus land, available only in some special modes.\n",
  "Pays Bonus, disponible seulement dans certains modes spéciaux.\n")

S("\n\nThis Orb protects you from attacks, scents, and insulates you "
  "from electricity. It does not let you go through deadly terrain, but "
  "if you are attacked with fire, it lets you stay in place in it.",

  "\n\nCet Orbe vous protège des attaques, des odeurs, et vous isole "
  "de l'électricité. Il ne vous permet pas d'aller sur un terrain mortel, "
  "mais si vous êtes attaqué%e0 par le feu, il vous permet de rester dedans.")

S("Completing the quest in this land is not necessary for the Hyperstone Quest.",
  "Compléter cette quête dans ce pays n'est pas nécessaire pour la Quête des Hyperpierres.")

// other options
// -------------

S("player", "joueur")
S("movement color", "couleurs des mouvements")
S("model used (not conformal!)", "modèle utilisé (non conforme !)")
S("azimuthal equidistant", "équidistant azimutal")
S("azimuthal equi-area", "équi-aire azimutale")
S("visit the website", "consulter le site internet")
S("leaderboards/achievements", "meilleurs scores/accomplissements")
S("return to the game", "retour au jeu")

// Android texts
// -------------

S("connected to Google Games", "connecté à Google Games")
S("background music", "musique de fond")
S("view your achievements", "voir vos accomplissements")
S("next page", "page suivante")

// new multiplayer
// ---------------

S("shoot'em up and multiplayer", "shoot'em up et multijoueur")

S("Two players cannot move/attack the same location!", "Deux joueurs ne peuvent pas aller/attaquer dans la même direction !")
S("Cannot move into the current location of another player!", "Impossible d'aller à l'emplacement actuel d'un autre joueur !")
S("Players cannot get that far away!", "Les joueurs ne peuvent pas aller si loin !")
S("Cannot push into another player!", "Impossible de pousser dans un autre joueur !")
S("Cannot push into the same location!", "Impossible de pousser dans le même emplacement !")

S("%The1 throws fire at %the2!",   "%Le1 jète du feu à %le2 !")
S("%The1 breathes fire at %the2!", "%Le1 crache du feu à %le2 !")

S("configure keys", "configurer les touches")
S("use default keys", "utiliser les touches par défaut")

S("Left the game.", "Quitter le jeu.")
S("shoot'em up mode", "mode shoot'em up")
S("turn-based mode", "mode tour par tour")
S("continue playing", "continuer à jouer")
S("start a new game", "commencer une nouvelle partie")

S("five players",  "cinq joueurs")
S("six players",   "six joueurs")
S("seven players", "sept joueurs")

S("You collect %the1. (%2)", "Vous ramassez %le1. (%2)")


S("Shmup (shoot'em up) mode: You can play a hyperbolic shoot'em up game. The game is based "
  "on the usual turn-based grid-based HyperRogue, but there are some changes. You fight by "
  "throwing knives, and you have three extra lives. There are no allies, so all Orbs "
  "related to allies give you extra lives instead (up to 5). Some other rules have been "
  "adapted too.\n\n",

  "Mode shoot'em up : Vous pouvez jouer à un jeu de shoot'em up hyperbolique. Le jeu est basé "
  "sur l'HyperRogue sur grille au tour par tour, mais il y a des changements. "
  "Vous combattez en jetant des couteaux, et vous avez trois vies en plus. Il n'y a pas d'alliés "
  "donc tous les Orbes reliés aux alliés donnent des vies en plus (max 5). D'autres règles sont "
  "aussi adaptées.\n\n")

S("Multiplayer: Play cooperatively (locally); treasures, kills, and deaths are calculated "
  "for each player too, for more competitive play. Orbs and treasures are shared, orbs drain "
  "faster, knives recharge slower, and player characters are not allowed to separate.\n\n",

  "Multijoueur : Jouer en coopération (localement); trésors, tués et morts sont calculées "
  "aussi pour chaque joueur, pour un jeu plus compétitif. Les Orbes et les trésors sont partagés, "
  "les orbes s'épuisent plus vite, les couteaux se rechargent plus lentement et les joueurs ne "
  "peuvent pas se séparer.\n\n")

S("Turn-based multiplayer: Turns are executed in parallel. A player can leave the game "
  "by pressing a designated key (useful when about to get killed or lost). The following "
  "Orbs work to bring such players back: ",
  "Multijoueur au tour par tour : Les tours sont exécutés en parallèle. Un joueur peut quitter le jeu "
  "en appuyant sur une touche (utile quand sur le point d'être tué ou perdu). Les Orbes suivants "
  "peuvent ramener de tels joueurs : ")

S("This menu can be also used to configure keys.\n\n",
  "Ce menu peut aussi être utilisé pour configurer les touches.")

// other stuff
// -----------

S("Warnings are issued when you try to do something that appears dangerous, "
  "like stepping on a known mine, or getting your boat destroyed by "
  "a Kraken without having Orb of the Fish. In some cases the action "
  "might actually be safe -- so you can ignore the warning and do it anyway, "
  "simply by repeating the action.",

  "Les Avertissements apparaissent lorsque vous essayez de faire quelque chose qui"
  "semble dangereux comme marcher sur une mine, ou avoir votre bateau détruit par un "
  "Kraken sans avoir l'Orbe du Poisson. Dans certains cas l'action peut être en réalité "
  "sûre. Vous pouvez ignorer l'avertissement et le faire quand même simplement en répétant "
  "l'action.")


// Kraken Depths
// -------------

N("Kraken Depths", GEN_M | GENF_ELISION, "Abysse du Kraken", "Abysses du Kraken", "dans les Abysses du Kraken")

S("A long time ago, this was a trade route. But then, Krakens have risen out of the "
  "depths. Many trading ships sank here. Legend says that you can uncover the secret "
  "of a magical weapon spell somewhere in the depths...\n\n"

  "You can find Sunken Treasures here, but they won't appear until you have killed "
  "a Kraken. You will also need Orb of the Fish to get the treasures, luckily you can "
  "steal one from the Viking treasure hunters.",

  "Il y a bien longtemps, ceci était une route de commerce. Mais un jour, les Krakens ont "
  "surgi des profondeurs. De nombreux navires marchands ont coulé ici. La légende dit que "
  "vous pouvez découvrir le secret d'une arme magique quelque part dans les abysses...\n\n"

  "Vous pouvez trouver des Trésors Engloutis ici, mais ils n'apparaîtrons pas avant que vous "
  "ayez tué un Kraken. Vous aurez aussi besoin de l'Orbe du Poisson pour obtenir les trésors, "
  "heureusement vous pouvez en voler un aux chasseurs de trésors Vikings.")

S("There are Krakens in your homeland too... huge sea monsters which "
  "could easily destroy ships. The geometry of this strange world "
  "prevents quick movement of huge objects, "
  "so there are no large ships, only small boats, and "
  "hyperbolic Krakens are relatively small too. Still, you suppose they might be "
  "the widest creatures which could still move at considerable speed...\n\n"

  "Kraken heads can move only on hexagons. You need to attack all the tentacles to "
  "kill the Kraken. A tentacle cannot attack if it has been attacked on the "
  "same turn. When a Kraken attacks you while you are in a boat, it "
  "destroys the boat, but does not kill you.",

  "Il y a aussi des Krakens dans votre pays natal... des monstres des mers géants qui "
  "détruisent des navires. La géométrie de ce monde étrange prévient le mouvement rapide "
  "de grands objets, donc il n'y a pas de grands navires, juste de petits bateaux, et "
  "les Krakens hyperboliques sont relativement petits. Cependant, vous supposez qu'il peut "
  "y avoir de plus larges créatures qui bougent très vite...\n\n"

  "Les têtes de Kraken ne peuvent aller que sur les hexagones. Vous devez attaquer tout les tentacules "
  "pour tuer le Kraken. Un tentacule ne peut pas attaquer s'il a été attaqué pendant le même tour. "
  "Quand un Kraken vous attaque lorsque vous êtes sur un bateau, il détruit le bateau mais ne vous "
  "tue pas.")

Orb("the Sword", "de l’Épée")

S("This Orb gives you a weapon made of pure magical energy. You do not hold "
  "it, it simply floats in the air next to you. When you go, the energy sword moves "
  "with you, pointing at the same relative angle it pointed before -- you cannot "
  "move or rotate it otherwise. Most monsters can be killed by moving the sword into them, "
  "and won't move into the spot with the sword.",

  "Cet Orbe vous donne une épée d'énergie magique pure. Vous ne la tenez pas, "
  "elle flotte simplement dans les airs près de vous. Quand vous partez, l'épée d'énergie bouge "
  "avec vous, pointant le même angle relatif qu'avant -- vous ne pouvez pas la bouger ou pivoter "
  "autrement. La plupart des monstres peuvent êtes tués en les transperçant de l'épée, et ils "
  "n'iront pas vers l'épée.")

N("Sunken Treasure", GEN_M, "Trésor Englouti", "Trésors Engloutis", "Trésor Englouti")

S("Cargo of a ship which was once destroyed by a Kraken.",
  "Cargaison d'un navire autrefois détruit par un Kraken.")


// Burial Grounds
// --------------

N("Burial Grounds", GEN_M, "Site Funéraire", "Sites Funéraires", "dans le Site Funéraire")

S("Ancient Viking heroes were buried here. Their graves have barrows raised over "
  "them, and are guarded by Draugar, animated corpses who are immune to mundane weapons. "
  "You will need to use a magical weapon spell to defeat them, and to rob the "
  "ancient jewelry buried in the graves.",

  "Les anciens héros Vikings sont enterrés ici. Leur tombes sont couvertes d'un tertre, "
  "et gardées par les Draugar, des corps non-morts immunisés aux armes ordinaires. "
  "Vous aurez besoin d'utiliser une arme magique pour les vaincre, et voler les bijoux "
  "anciens enfouis dans les tombes.")

N("Draugr", GEN_M, "Draugr", "Draugar", "Draugr")

S("Animated corpses of ancient Viking warriors. They are immune to mundane weapons, "
  "but they can be destroyed by your Orb of the Sword.",
  "Corps non-morts d'anciens guerriers Vikings. ils sont immunisés aux armes ordinaires "
  "mais peuvent être détruits par votre Orbe de l’Épée.")

Orb("the Sword II", "de l’Épée II")

S("An alternative version of Orb of the Sword. If you have both of them, "
  "you have two energy swords, facing in opposite directions.",
  "Une version alternative de l'Orbe de l’Épée. Quand vous avez les deux, "
  "vous avez deux épées d'énergie, dans des directions opposées.")

N("Ancient Jewelry", GEN_M, "Bijou Ancien", "Bijoux Anciens", "Bijou Ancien")

S("Precious belongings of ancient Viking heroes. Your Orb of the Sword can be "
  "used to dig these treasures out of the barrows.",
  "De précieuses possessions d’ancien héros Vikings. Votre Orbe de l’Épée peut "
  "être utilisé pour déterrez ces trésors des tertres.")

S("%The1 destroys your boat!", "%Le1 détruit votre bateau !")
S("Your mundane weapon cannot hurt %the1!", "Vos armes ordinaires ne peuvent blesser %le1 !")
S("This move appears dangerous -- are you sure?", "Ce mouvement parait dangereux -- êtes-vous sûr ?")
S("You slash %the1.", "Vous frappez %le1.")
S("You chop down %the1.", "Vous abattez %le1.")
S("Cannot slash another player!", "Impossible de frapper un autre joueur !")
S("%The1 suffocates!", "%Le1 suffoque !")

N("barrow wall", GEN_M, "mur de tertre", "mur de tertre", "mur de tertre")

S("This wall is quite strong. You will need another way in.",
  "Ce mur est plutôt solide. Vous devrez trouver une autre entrée.")

N("barrow", GEN_M, "tertre", "tertres", "tertre")

S("Your Orb of the Sword can be used to dig here.",
  "Votre Orbe de l’Épée peut être utilisé pour creuser ici.")

// Trollheim
// ---------

N("Trollheim", GEN_M | GENF_PROPER, "Trollheim", "Trollheim", "à Trollheim")

S("Many clans of Trolls spend their lives in this kingdom. You can find many "
  "statues of Trolls here. You suppose that they are not actually statues, but simply "
  "elderly Trolls, who have petrified upon death. Or maybe you have killed "
  "these Trolls yourself?",

  "De nombreux clans trolls vivent dans ce royaume. Vous pouvez trouver de nombreuses "
  "statues de troll ici. Vous supposez qu'il ne s'agit pas de vraies statues mais simplement "
  "de vieux trolls pétrifiés par la mort. Ou peut-être les avez vous tués vous-mêmes ?")


N("Golden Egg", GEN_M | GENF_ELISION, "Œuf d'Or", "Œufs d'Or", "Œuf d'Or")

S("Trolls of Trollheim are descendants of a bridge Troll, who collected "
  "payments from people crossing the bridge. One of them paid with "
  "golden eggs. The bridge Troll found the eggs beautiful, but he quickly lost them. "
  "Golden eggs are still revered by Trolls, and you can find them in their "
  "caves.",

  "Les Trolls de Trollheim descendent des trolls des ponts, qui collectaient les "
  "péages, de ceux qui traversaient le pont. On les paie en œufs d'or. "
  "Les trolls des ponts admirent ces oeufs mais les perdent vite. Les œufs "
  "sont toujours vénérés par les trolls, et on peut les trouver dans leurs caves.")

Orb("the Stone", "de Pierre")

S("Trolls turn into stone walls when they die. When you have this Orb, "
  "this happens to every monster you defeat. Statues created from this Orb "
  "have slightly different properties than Trolls who petrify naturally.",

  "Les Trolls se transforment en murs de pierre à leur mort. Quand vous avez "
  "cet orbe cela arrive à chaque monstre que vous tuez. Les Statues créée par "
  "cet orbe ont des propriétés légèrement différentes des Trolls pétrifiés naturellement.")

N("stone statue", GEN_F, "statue de pierre", "statues de pierre", "statue de pierre")

S("A petrified creature.", "Une créature pétrifiée.")


S("score: %1", "points : %1")
S("kills: %1", "tués : %1")

S("mark heptagons", "marque les heptagones")
S("help for keyboard users", "aide pour les utilisateurs de clavier")

// missing
// -------

S("You leave %the1.", "Vous quittez %le1.") // Baby Tortoise

S("%The1 says, \"I die, but my clan in Trollheim will avenge me!\"",
  "%Le1 dis : \"Je meurs, mais mon clan de Trollheim me vengera !\"")

S("After killing %the1, you feel able to reach the Elemental Planes!",
  "Après avoir tué %le1 vous vous sentez capable d'atteindre les Plans Elémentaux !")

S("You feel that a magical weapon is waiting for you...",
  "Vous sentez qu'une arme magique vous attend...")

// missing
// -------

S("reset per-player statistics", "réinitialise les statistiques par joueur")
S("An alternate layout of the Crossroads, without walls.", "Une disposition alternative des Croisements, sans murs.")
S("cheats", "triche")

S("Known mines may be marked by pressing 'm'. Your allies won't step on marked mines.", 
  "Les mines connues peuvent être marquées en appuyant sur 'm'. Vos alliés ne marcheront pas sur les mines marquées.")

S("Known mines may be marked by touching while in drag mode. Your allies won't step on marked mines.", 
  "Les mines connues peuvent être marquées en les touchant en mode tirer. Vos alliés ne marcheront pas sur les mines marquées.")

// 9.0 patches
// ===========

// extra help
// ----------

S("Coastal region -- connects inland and aquatic regions.\n",
  "Zone cotière -- connecte les îles et régions aquatiques.\n")

S("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n",
  "Région aquatique -- accessible seulement depuis les régions côtières et d'autres régions aquatiques.\n")

// barrow treasure protected from Orb of Space
// -------------------------------------------

S("%The1 is protected from this kind of magic!",
  "%Le1 est protégé contre cette forme de magie !")

// Orb is always available in the given land
// -----------------------------------------

S("always available", "toujours disponible")

S("\n\nAfter the Trolls leave, you have 750 turns to collect %the1, or it gets stolen.",
  "\n\nAprès que les Trolls soient partis, vous avez 750 pour collecter %le1, ou il est volé.")

S(
    "There are many lands in HyperRogue. Collect 10 treasure "
    "in the given land type to complete it; this enables you to "
    "find the magical Orbs of this land, and in some cases "
    "get access to new lands. At 25 treasures "
    "this type of Orbs starts appearing in other lands as well. Press 'o' to "
    "get the details of all the Lands.\n\n",
    
    "Il y a de nombreux pays dans HyperRogue. Collecter 10 trésors "
    "dans le pays donné pour le compléter ; ceci vous permet de "
    "trouver les Orbes de ce pays, et dans certains cas "
    "obtenir l'accès à de nouveaux pays. A 25 trésors "
    "Ce type d'Orbe commence à apparaître dans d'autre pays. Appuyer sur 'o' "
    "pour obtenir les détails de tout les Pays.\n\n"
    )

// welcome messages
// ----------------

S("Welcome to the Random Pattern mode!", "Bienvenue dans le mode motif aléatoire !")
S("You are playing %the1 in the Pure Tactics mode.", "Vous jouez %le1 dans le mode tactique pur.")
// %1 is the number
S("Welcome to the Yendor Challenge %1!", "Bienvenue au Défi de Yendor %1!")
S("Welcome to HyperRogue!", "Bienvenue dans HyperRogue!")
S("Press F1 or right-click things for help.", "Appuyer sur F1 puis cliquer-droit sur les éléments pour obtenir l'aide'.")
// for iOS
S("Press F1 or right-shift-click things for help.", "Appuyer sur F1 ou cliquer-droit sur les éléments pour obtenir l'aide'.")
S("Welcome to the Euclidean mode!", "Bienvenue dans le mode Euclidien !")
S("Good luck in the elliptic plane!", "Bonne chance dans le plan elliptique !")
S("Welcome to Spherogue!", "Bienvenue dans Sphèrogue!")

// extra help for joystick configuration
// -------------------------------------

S("joystick configuration", "configuration du joystick")
S("first joystick position (movement)", "première position du joystick (mouvement)")
S("second joystick position (panning)", "seconde position du joystick (panneau)")
S("old style joystick configuration", "configurer l'ancien style du joystick")

S("move up-right", "aller en haut à droite")
S("move up-left", "aller en haut à gauche")
S("move down-right", "aller en bas à droite")
S("move down-left", "aller en bas à gauche")
S("stay in place (left + right)", "rester sur place (gauche + droite)")
S("cancel move", "annuler le mouvement")
S("leave the game", "quitter le jeu")
S("drop Dead Orb (up + down)", "lâcher une Orbe Morte (haut + bas)")

// extra help for configuration / projection and geometry
// ------------------------------------------------------

S("Roughly 42% cells are on the edge of your sight range. Reducing "
  "the sight range makes HyperRogue work faster, but also makes "
  "the game effectively harder.",
  "Environ 42%cases sont au bord de votre limite de vue. Réduire "
  "la limite de vue fait effectivement marcher HyperRogue plus vite, "
  "mais rends le jeu aussi effectivement plus dur.")

S("Watch the Minkowski hyperboloid or the hypersian rug mode with the "
  "red/cyan 3D glasses.",
  "Viualisez l'hyperboloïde de Minkowski ou le mode tapis hypersan avec les lunettes rouges/cyans 3D.")

S("0 - return", "0 - retour")
S("go back", "revenir en arrière")
S("F1 - help", "F1 - aide")

// for the conformal polynomial
S("coefficient (imaginary)", "coefficient (imaginaire)")
S("Scale the displayed model.", "Changer l'échelle du mode affiché.")
S("Reenter HyperRogue to apply this setting", "Redémarrer HyperRogue pour appliquer ce réglage")

S(  "HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincaré models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
    "See also the conformal mode (in the special modes menu) "
    "for more models.",
    
    "HyperRogue utilise le modèle hyperboloïde de Minkowski. "
    "Les modèles de Klein et Poincaré peuvent être obtenus par perspective, "
    "et le modèle de Gans est obtenu par projection orthogonale. "
    "Voir aussi le mode conforme (dans le mode de menus spéciaux) pour plus de modèle.")

S("stereographic projection", "projection stéréographique")
S("Poincaré model", "modèle de Poincaré")
S("gnomonic projection", "projection gnomique")
S("Klein model", "modèle de Klein")
S("towards orthographic", "vers le modèle ortho-graphique")
S("towards Gans model", "vers le modèle de Gans")

S("Euclidean/elliptic mode", "mode euclidien/elliptique")
S("geometry", "géométrie")
S("spherical", "sphérique")
S("elliptic", "elliptique")
S("Euclidean", "euclidienne")
S("projection", "projection")

// VERSION 9.1
// ===========

#define N4(a,b,c,d) N(a,b,c,d,c)

N4("Familiar", GEN_M, "Familier", "Familiers")
S("Commanded %the1!", "A commandé %le1 !")
S("F8 = SVG shot", "F8 = photo SVG")
S("Saved the SVG shot to %1 (sightrange %2)", "A sauvegardé la photo SVG dans %1 (limite de vue %2)")

S("draw circle around the target", "dessine des cercles autour de la cible")
S("reverse pointer control", "inverse le contrôle du pointeur")
S("sight range", "limite de vue")
S("configure input", "configurer l'input")

// quote of Euclid
S("\"There is no royal road to geometry.\"",
   "Il n'y a pas de voie royale vers la géométrie.\"")

// quote of Lobachevsky
S("\"There is no branch of mathematics, however abstract, ",
  "\"Il n'y a aucune branche des mathématiques, aussi abstraite soit-elle ")
  
S("which may not some day be applied to phenomena of the real world.\"",
  "qui ne puisse un jour être appliquée aux phénomènes du monde réel.\"")
  
// quote of Gauss
S("\"It is not possession but the act of getting there, ",
  "\"Ce n'est pas la possession mais le processus permettant ")
  
S("which grants the greatest enjoyment.\"", "d'y arriver qui apporte le plus de joie.\"")

// quote of Escher
S("\"We live in a beautiful and orderly world, ",
  "\"Nous vivons dans un monde magnifique et ordonné, ")

S("and not in a chaos without norms.\"",
  "et non dans un chaos sans règles.\"")

/*
		"NEW_ACHIEVEMENT_7_21_NAME"	"Deadly Death"
		"NEW_ACHIEVEMENT_7_21_DESC"	"Defeat Goblin with an energy sword."
		"NEW_ACHIEVEMENT_7_22_NAME"	"Halloween Witch"
		"NEW_ACHIEVEMENT_7_22_DESC"	"Collect 50 Treats in Halloween."
		"NEW_ACHIEVEMENT_7_23_NAME"	"Halloween Vampire"
		"NEW_ACHIEVEMENT_7_23_DESC"	"Colelct 100 Treats in Halloween."
*/

S("You have enough treasure now to revive %the1!",
  "Vous avez désormais assez de trésors pour ressusciter %le1 !")

S("Killed %1 can be revived with an Orb of Love, after you collect 20 more $$$.",
  "%Le1 tué peut être ressuscité avec une Orbe d'Amour, après avoir collecté 20 $$$ de plus."
  )

S("%The1 will be revivable at %2 $$$", "%Le1 sera ressuscitable avec %2 $$$")

S("Halloween mini-game", "mini-jeu d'Halloween")

N4("Vampire Bat", GEN_F, "Chauve-souris vampire", "Chauve-souris vampires")
N("Halloween", GEN_O, "Halloween", "Halloween", "à Halloween")
N4("Treat", GEN_M, "Tour", "Tours")

S(
  "Halloween is a special land, that is available only in the spherical "
  "or elliptic geometry (press 'o' to switch). You play on the surface of "
  "a jack-o'-lantern, "
  "and have to collect as many Treats as possible. Each Treat you collect "
  "brings new monsters to fight, and new magical powers for you. You "
  "have to fight the monsters while effectively managing your limited "
  "resources.",
  
  "Halloween est un pays spécial, qui n'est disponible que dans la géométrie "
  "sphérique et elliptique (appuyer sur 'o'). Vous jouez sur la surface d'une "
  "citrouille lanterne,"
  "et devez collecter autant de Bonbons que possible. Chaque Bonbon collecté "
  "amène d'autre monstres à combattre, et de nouveaux pouvoirs magiques pour vous. "
  "Vous devez combattre les monstres en gérant efficacement vos ressources limités."
  )

S(
  "Vampire Bats don't attack normally, but they drain your magical powers if "
  "they are at distance at most 2 from you.",
  "Le Chauves-souris vampires n'attaquent pas normalement, mais draine vos pouvoirs "
  "magiques si elles sont à une distance d'au moins 2 de vous.")

S("Ghosts can move through chasms!", "Les Fantômes peuvent traverser les gouffres !")
S("Push Skeletons into the holes!", "Poussez les Squelettes dans les trous !")
S("You'll need your magical sword against the Draugar!", "Vous aurez besoin de votre épée magique contre le Draugr!")
S("Demons are slow, but you'll need the experience against stronger ones...",
  "Les démons sont lents, mais vous aurez besoin d'expérience contre les plus forts...")
S("You will need more experience to defeat the Greater Demon!",
  "Vous avez besoin de plus d'expérience pour vaincre le Démon Majeur !")
S("Cultists throw fire at you from distance!",
  "Les cultistes vous lancent du feu à distance !")
S("Defeat Flail Guards by moving away from them.",
  "Vainquez les Gardes au Fléau en vous éloignant d'eux'.")
S("Vampire Bats drain your magical powers!",
  "Les Chauves-souris vampires drainent vos pouvoirs magiques !")

S("Hint: use arrow keys to scroll.", "Astuce : utiliser les touches flèches pour faire défiler.")
S("Hint: press 1 2 3 4 to change the projection.", "Astuce : appuyer sur 1 2 3 4 pour changer la projection.")
S("You gain a protective Shell!", "Vous gagnez une Carapace protectrice !")
S("Hint: magical powers from Treats expire after a number of uses.",
  "Astuce: les pouvoirs magiques des Bonbons expirent après un nombre d'utilisation.")
S("A magical energy sword. Don't waste its power!",
  "Une épée d'énergie magique. Ne gaspillez pas son pouvoir !")
S("Another energy sword!", "Une autre épée d'énergie !")
S("You got Thorns! Stab monsters by moving around them.",
  "Vous avez des Epines ! Poignardez des monstres en vous déplaçant autour d'eux.")
S("Aethereal powers let you go through the holes.",
  "Les pouvoirs éthérés vous permettent de traverser les trous.")
S("The tasty treat increases your protection.",
  "Ce délicieux bonbon augmente votre protection.")
S("You gain your protective Shell back!", "Vous récupérez votre Carapace !")

S("rainbow landscape", "paysage arc-en-ciel")
S("Cannot use %the1 here!", "Vous ne pouvez pas utiliser %le1 !")
S("draw the grid", "dessiner la grille")
S("Escher/3D", "Escher/3D")
S("plain/3D", "simple/3D")
S("3D", "3D")
S("Welcome to Halloween!", "Bienvenue à Halloween!")

S("How long should the messages stay on the screen.",
  "Combien de temps les messages restent à l'écran.")
S("select this color", "sélectionnez cette couleur")
S("sound effects volume", "volume des effets sonores")

S("\n\nSee sounds/credits.txt for credits for sound effects", 
  "\n\nVoir sounds/credits.txt pour les crédits des effets sonores")

S("scrolling speed", "vitesse de défilement")
S("movement animation speed", "vitesse d'animation du mouvement")
S("+5 = move instantly", "+5 = bouge instantanément")
S("extra graphical effects", "effets graphiques supplémentaires")

// VERSION 9.3
// ===========

S("SORT", "TRIER")
S("PICK", "CHOISIR")
S("PLAY", "JOUER")

// 3D configuration
S("3D configuration", "configuration 3D")
S("High detail range", "Portée de la haute résolution")
S("Mid detail range", "Portée de la moyenne résolution")
S("Camera level above the plane", "Niveau de la caméra au-dessus du plan")
S("Ground level below the plane", "Niveau du sol en-dessous du plan")
S("Projection at the ground level", "Projection au niveau du sol")
S("Height of walls", "Hauteur des murs")
S("Rock-III to wall ratio", "rapport de la Roche-III au mur")
S("Human to wall ratio", "rapport de l'humain au mur")
S("Level of water surface", "Niveau de la surface de l'eau")
S("Level of water bottom", "Niveau de la profondeur de l'eau")
S("set 3D monsters or walls in basic config first",
  "régler les monstres ou murs 3D dans la configuration basique d'abord")
S("parameters set correctly", "paramètre réglé correctement")

S("Objects at distance less than %1 absolute units "
  "from the center will be displayed with high "
  "detail, and at distance at least %2 with low detail.",
  
  "Les Objets à une distance de moins de %1 en unité absolue "
  "du centre sera affiché avec une haute résolution, et à une distance "
  "d'au moins %2 avec une basse résolution.")

S("Ground level is actually an equidistant surface, "
  "%1 absolute units below the plane P. "
  "Theoretically, this value affects the world -- "
  "for example, eagles could fly %2 times faster by "
  "flying above the ground level, on the plane P -- "
  "but the actual game mechanics are not affected. ",

  "Le niveau au sol est en réalité une surface équidistante, "
  "%1 en unité absolue sous le plan P. "
  "Théoriquement cette valeur affecte le monde -- "
  "par exemple, les aigles peuvent voler %2 fois plus vite en "
  "volant au-dessus du niveau du sol, sur le plan P "
  "mais les mécaniques de jeu réelles ne sont pas affectées. ")
  
  
S("(Distances reported by the vector graphics editor "
  "are not about points on the ground level, but "
  "about the matching points on the plane P -- "
  "divide them by the factor above to get actual "
  "distances.)",
  
  "(Les distances rapportées par le vecteur de l'éditeur graphique "
  "Ne correspondent pas à des points au niveau du sol, mais à des "
  "points correspondants sur le plan P -- divisez les par le "
  "facteur au-dessus pour la vraie distance.)")

S( "If we are viewing an equidistant g absolute units below a plane, "
   "from a point c absolute units above the plane, this corresponds "
   "to viewing a Minkowski hyperboloid from a point "
   "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
   "the Poincaré model for g=c, and Klein-Beltrami model for g=0.",
   
   "Si on regarde un g équidistant en unité absolue sous un "
   "plan depuis un point c en unités absolues au-dessus du plan, ceci"
   "correspond à visualiser une hyperboloïde de Minkowski depuis un "
   "point à tanh(g)/tanh(c) unités sous le centre. Ceci correspond au "
   "modèle de Poincaré pour g=c, et le modèle Klein-Beltrami pour g=0.")

S( "The height of walls, in absolute units. For the current values of g and c, "
   "wall height of %1 absolute units corresponds to projection value of %2.",
   
   "La hauteur des murs en unités absolues. Pour les valeurs actuelles de g et c "
   "une hauteur de murs de %1 en unités absolues correspond à une valeur projetée de %2.")

S( "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
   "Length of paths on the Rock III level is %3 of the corresponding length on the "
   "ground level.",
   
   "Le ratio de Roche III par rapport au murs est de %1, donc les Rochers III font %2 "
   "de haut en unités absolues. La longueur des chemins sur le niveau de Rochers III "
   "est %3 de la longueur absolue correspondante au niveau du sol.")

S( "Humans are %1 "
   "absolute units high. Your head travels %2 times the distance travelled by your "
   "feet.",
   
   "Les humains mesurent %1. Votre tête parcours %2 la distance parcourue par vos "
   "pieds.")
  
// Euclidean regular patterns
// --------------------------

S("three colors", "trois couleurs")
S("three colors rotated", "trois couleurs échangées")
S("edit all three colors", "éditer toutes les trois couleurs")

S(" (local treasure)", " (trésor local)")
S(" (increases treasure spawn)", " (augmente l'apparition de trésors)")
S(" (appears here)", " (apparaît ici)")
S("%The1 is destroyed!", "%Le1 est détruit%e1 !") // generic message

S("page", "page")
S("all", "toutes")

S("Your Orb of Recall is blocked by something big!",
  "Votre Orbe de Rappel est bloquée par quelque chose de gros !")

S("You are recalled!", "Vous êtes rappelé%e0 !")
S("You vault over %the1!", "Vous sautez au-dessus du %1!")
S("You cannot attack %the1!", "Vous ne pouvez pas attaquer %le1!")

S("\n\nFast flying creatures may attack or go against gravity only in their first move.",
  "\n\nDe rapides créatures volantes qui peuvent attaquer ou aller contre la gravité "
  "seulement au premier mouvement.")

// Dungeon
// -------

N("Dungeon", GEN_M, "Donjon", "Donjons", "dans le Donjon")

S("The result of a collaboration of the Great Vizier and the Wizard of the Ivory Tower.",
  "Le résultat d'une collaboration entre le Grand Vizir et le Magicien de la Tour d'Ivoire.")

N4("Slime Mold", GEN_F, "Gélatine Moisie","Gélatine Moisie")

S("A very interesting species of slime mold.",
  "Une espèce intéressante de gélatine moisie.")

N4("Bat", GEN_M, "Chauve-souris", "Chauves-souris")

S("Someone has told you that one can get battle experience safely by "
  "killing tons of essentially harmless creatures, such as Bats. But "
  "does this make any sense?...\n\n"
  
  "It does not. Bats cannot hurt you, but may block your movement, or "
  "toggle switches if they fall on them.",
  
  "Quelqu'un vous a dit qu'on peut obtenir de l'expérience sans danger "
  "en tuant des tonnes de créatures essentiellement inoffensives, comme "
  "des chauves-souris. Mais est-ce que cela a du sens ?...\n\n"
  
  "Ça n'en a pas. Les chauves-souris ne peuvent pas vous blesser mais "
  "peuvent bloquer vos mouvements, ou déclencher des interrupteurs si "
  "elles tombent dessus."
  )

Orb("Recall", "Rappel")

S("When the charges on this Orb expire, "
  "you will be automatically returned to the place where you have found it. "
  "Extra Orbs of Recall delay this without changing the recall location. "
  "Picking up an Orb of Safety causes an immediate recall.",
  
  "Quand les charges de cette Orbe expirent, vous retournerez automatiquement"
  "à l'endroit où vous l'avez trouvée. Des Orbes supplémentaires de rappel "
  "délaient ceci sans changer le lieu de rappel. Ramasser une Orbe de Sécurité "
  "déclenche immédiatement le rappel."
  )

// 
//

// Lost Mountain
// -------------

//N("Pyramid", GEN_F, "Pyramide", "Pyramides", "dans la Pyramide")
N("Lost Mountain", GEN_F, "Montagne perdue", "Montagnes perdues", "dans la Montagne Perdue")

/*
S("An ancient civilization has created this pyramid in the Jungle by "
  "changing the gravity. The gravity is still changed, but "
  "the pyramid is overgrown with ivies and bushes. "
  "Will you dare to climb the ivies to get treasures waiting above?",
  
  "Une ancienne civilisation a créé cette pyramide dans la Jungle "
  "en changeant la gravité. La gravité est toujours changée, mais "
  "la pyramide est couverte de lierre et de buissons. "
  "Oserez vous escalader le lierre pour obtenir les trésors qui vous attendent au sommet ?"
  )
*/
  
S(
  "Gravitational anomalies in the Jungle create mountains "
  "overgrown with ivies and bushes. "
  "Will you dare to climb the ivies to get the amethysts hidden above?\n\n"
  
  "Cells adjacent to Ivies count as stable (but Ivies "
  "cannot climb themselves or other Ivies).",
  
  "Les anomalies gravitationnelles dans la Jungle créent des montagnes "
  "couvertes de lierre et de buissons. Oserez vous escalader le lierre "
  "pour obtenir les améthystes cachées au sommet ?\n\n"
  
  "Les cases adjacentes aux Lierres comptent comme stables (mais les Lierres "
  "ne peuvent pas s'escalader ou escalader d'autre Lierres)."
  )
  
N4("big bush", GEN_M, "grand buisson", "grands buissons")

S("You can hold this bush to climb the Lost Mountain. "
  "Bushes block the movement of birds.",
  "Vous pouvez vous agripper à ce buisson pour escalader la Montagne perdue. "
  "Les buissons bloquent le mouvement des oiseaux."
  )

S("You can hold this bush to climb the Lost Mountain, "
  "but it is not very strong -- it will get destroyed "
  "if you climb from it into an unstable location. "
  "Bushes block the movement of birds.",
  
  "Vous pouvez vous agripper à ce buisson pour escalader la Montagne perdue, "
  "mais il n'est pas très solide -- et sera détruit si vous "
  "l'escaladez depuis un endroit instable. "
  "Les buissons bloquent le mouvement des oiseaux.")

N4("Amethyst", GEN_F, "Améthyste", "Améthystes")

S("A beatiful purple gem from the Lost Mountain.",
  "Une magnifique gemme violette de la Montagne perdue.")

Orb("Nature", "Nature")
N4("Friendly Ivy", GEN_M, "Lierre Amical", "Lierres Amicaux")

S("This Orb allows you to grow like an Ivy. "
  "The Ivy is always rooted in your current location; "
  "moving among the Ivy cells will move the root. "
  "Moving to a new location will cause the Ivy to grow "
  ", if an Ivy could make that movement "
  "(otherwise it breaks). "
  "You can also target one of the cells adjacent to your ivy "
  "(not to you) to grow or attack there.",
  
  "Cette Orbe vous permet de grandir comme du Lierre. "
  "Le Lierre est toujours planté à votre position actuelle ; "
  "bouger le long des cases de lierre bougera la racine. "
  "Aller à une nouvelle position fera pousser le Lierre "
  "si un Lierre peut faire ce mouvement (autrement il casse) "
  "Vous pouvez aussi cibler une case adjacente à votre lierre "
  "(pas à vous) pour pousser ou attaquer là."
  )

// Reptiles
// --------

N("Reptiles", GEN_M, "Reptile", "Reptiles","dans le Reptile")
N4("Reptile", GEN_M, "Reptile", "Reptiles")
N4("Reptile floor", GEN_M, "sol en Reptile", "sols en Reptile")
N4("Reptile bridge", GEN_M, "pont en Reptile", "ponts en Reptile")

S("These reptiles are quite strange creatures. They "
  "spend most of their lives sleeping as floors "
  "that other creatures can walk on. "
  "Sometimes they wake up to hunt their prey, "
  "but they will happily go back to sleep if they "
  "happen to move into a hole on their way. "
  "Your attacks do not kill the Reptiles, but "
  "you can push and stun them.",
  
  "Ces reptiles sont des créatures plutôt étranges. Elles "
  "passent la plupart de leur temps à dormir au sol où les "
  "autre créatures marchent. Parfois elles se réveillent pour "
  "chasser leur proie, mais retourneraient bien dormir si "
  "elles rencontrent un trou sur leur chemin. Vos attaques "
  "ne les tueront pas mais sous pouvez les pousser et les "
  "assommez."
  )

N4("Dodecahedron", GEN_M, "Dodécaèdre", "Dodécaèdres")
S("These dodecahedra made of a mysterious material are the Reptiles' favorite toy.",
  "Ces dodécaèdres fait d'un mystérieux matériel sont le jouet favori des Reptiles.")

Orb("Vaulting", "Saut")

S("This Orb allows you to jump over an adjacent monster, killing or stunning it. "
  "You can only vault in a roughly straight line. "
  "Target a cell on the other side to use it.",
  
  "Cette Orbe vous permet de survoler un monstre adjacent, en le tuant ou l’assommant. "
  "Vous pouvez seulement sauter en ligne droite. "
  "Visez une case de l'autre côté pour l'utiliser."
  )

S("inventory/kill sorting", "tri de l'inventaire/des tués")
S("first on top", "premier au sommet")
S("first on bottom", "premier en bas")
S("last on top", "dernier au sommet")
S("last on bottom", "dernier en bas")
S("by land", "par pays")
S("by number", "par nombre")

N4("small bush", GEN_M, "petit buisson", "petits buissons")

/*
		"NEW_ACHIEVEMENT_7_24_NAME"	"Reached Reptiles"
		"NEW_ACHIEVEMENT_7_24_DESC"	"Find and collect a Dodecahedron."
		"NEW_ACHIEVEMENT_7_25_NAME"	"Lizard"
		"NEW_ACHIEVEMENT_7_25_DESC"	"Collect 10 Dodecahedra."
		"NEW_ACHIEVEMENT_7_26_NAME"	"Alligator"
		"NEW_ACHIEVEMENT_7_26_DESC"	"Collect 25 Dodecahedra."
		"NEW_ACHIEVEMENT_7_27_NAME"	"Master of Reptiles"
		"NEW_ACHIEVEMENT_7_27_DESC"	"Collect 50 Dodecahedra."
		"NEW_ACHIEVEMENT_7_28_NAME"	"Reached the Dungeon"
		"NEW_ACHIEVEMENT_7_28_DESC"	"Collect a Slime Mold."
		"NEW_ACHIEVEMENT_7_29_NAME"	"Bat"
		"NEW_ACHIEVEMENT_7_29_DESC"	"Collect 10 Slime Molds."
		"NEW_ACHIEVEMENT_7_30_NAME"	"Skeleton"
		"NEW_ACHIEVEMENT_7_30_DESC"	"Collect 25 Slime Molds."
		"NEW_ACHIEVEMENT_7_31_NAME"	"Master of the Dungeon"
		"NEW_ACHIEVEMENT_7_31_DESC"	"Collect 50 Slime Molds."
		"NEW_ACHIEVEMENT_8_0_NAME"	"Mountain Climber"
		"NEW_ACHIEVEMENT_8_0_DESC"	"Fidn and collect an Amethyst."
		"NEW_ACHIEVEMENT_8_1_NAME"	"Mountain Ivy"
		"NEW_ACHIEVEMENT_8_1_DESC"	"Collect 10 Amethysts."
		"NEW_ACHIEVEMENT_8_2_NAME"	"Mountain Eagle"
		"NEW_ACHIEVEMENT_8_2_DESC"	"Collect 25 Amethysts."
		"NEW_ACHIEVEMENT_8_3_NAME"	"Master of the Mountain"
		"NEW_ACHIEVEMENT_8_3_DESC"	"Collect 50 Amethysts."
*/

S("You feel something strange about gravity here...",
  "Vous sentez quelque chose d'étrange à propos de la gravité ici...")
S(
  "The rug depicts a man in a deep dungeon, unable to leave.",
  "Ce tapis représente un homme dans un donjon profond, incapable de partir.")
S(
  "You feel attuned to gravity, ready to face mountains and dungeons.",
  "Vous vous sentez habitué%e0 à la gravité, près à affronter les montagnes et donjons.")

// VERSION 9.4
// ===========

// not previously translated
S("shift+O to switch anti-aliasing", "shift+O pour activer l'antialiasing")
S("For some reason... cannot attack!", "Pour la même raison... impossible d'attaquer !")
S("Your energy swords get stronger!", "Votre épée d'énergie se renforce !")
S("You feel even more attuned to the magic of this land!",
  "Vous vous sentez plus habitué%e0 à la magie de ce pays !")
S("Quite tough, for your first fight.", "Plutôt dur pour votre premier combat.")
S("(You can also use right Shift)\n\n", "(Vous pouvez aussi utiliser Shift droit)\n\n")

// Crossroads V
// ------------

N("Crossroads V", GEN_M, "Croisement V", "Croisements V", "dans les Croisements V")
S("Extremely narrow Crossroads layout.\n", "Disposition extrêmement étroite.\n")

// Bull Dash/Prairie common
// ------------------------

N4("Sleeping Bull", GEN_M, "Taureau endormi", "Taureaux endormis")

S("Sleeping bulls wake up when you get into distance of two cells from them.",
  "Les taureaux endormis se réveillent quand vous arrivez à une distance de deux cases d'eux.")

N4("Raging Bull", GEN_M, "Taureau enragé", "Taureaux enragés")

S("Raging Bulls charge in a straight line: on heptagons, when they can choose one of two possible directions, "
  "they choose one closer to your current location. In the case of a tie, the cell where more neighbors is "
  "closer to your current location is chosen; if still a tie, past locations are considered. "
  "They can attack you in any direction, and monsters on their way are attacked even if friendly. "
  "When they crash into something, the obstacle is usually destroyed, and they are stunned for three turns, after "
  "which they charge at you again (in any direction). "
  "Raging Bulls cannot be killed or stunned conventionally.",
  
  "Les Taureaux enragés chargent en ligne droite : sur les heptagones, quand ils peuvent choisir entre deux directions, "
  "possibles ils choisissent la plus proche de votre position actuelle. Dans le cas d'un embranchement, la case avec le "
  "plus de voisins et plus proche de votre position est choisie ; si c'est encore un embranchement les emplacements "
  "passés sont considérés. Ils vous attaquent depuis n'importe quelle direction, et les monstres sur leur chemin sont"
  "attaqués même s'ils sont amicaux. Quand ils percutent quelque chose l'obstacle est habituellement détruit, et ils "
  "sont assommés pendant trois tours, après quoi ils peuvent charger à nouveau (dans toutes les directions) "
  "Les Taureaux enragés ne peuvent pas être tués de manière conventionnelles."
  )

N4("Herd Bull", GEN_M, "Taureau de troupeau", "Taureaux de troupeau")

S("Herds of these Bulls are running long distances for some reason. They become Raging Bulls if something stops them.",
  "Les troupeaux de ces taureaux parcourent de longues distances pour certaines raisons. Ils deviennent des taureaux "
  "enragés si quelque chose les arrête."
  )

N4("Gadfly", GEN_M, "Taon", "Taons")
S("Annoying insects. They can awaken Sleeping Bulls.", "Des insectes énervants. Ils réveillent les Taureaux Endormis.")
S("%The1 wakes up %the2.", "%Le1 réveille %le2.")

S("%The1 crashes into %the2!", "%Le1 s'écrase dans %le2!")
S("Fire is extinguished!", "Le feu s'éteint !")
S("%The1 is filled!", "%Le1 est rempli !")

// Prairie
// -------

N("Prairie", GEN_F, "Prairie", "Prairies", "dans la Prairie")
N4("Green Grass", GEN_F, "Herbe verte", "Herbes vertes")
S("You can find safety in some places in the Prairie, but if you want treasures, "
  "they can be found only on the other side of a giant herd of bulls.",
  "Vous pouvez être en sécurité dans la Prairie mais si vous voulez des trésors, "
  "ils sont de l'autre côté de ce troupeau géant de taureaux."
  )

Orb("the Bull", "du Taureau")

S("You get the powers of Shield, Horns, and Thorns after you move two moves in a straight line "
  "with this Orb.",
  
  "Vous obtenez les pouvoirs du Bouclier, des Cornes et des Épines après deux mouvement en ligne droite avec cette Orbe.")

// Bull Dash
// ---------

N4("Bull Dash", GEN_F, "Charge de taureau", "Charges de taureau")
N4("Butterfly", GEN_M, "Papillon", "Papillons")
N4("Spinel", GEN_M, "Spinelle", "Spinelles")

S(
  "Butterflies don't pursue you -- unless you get next to them, they just spin around the obstacles. "
  "They cannot be killed conventionally, but you get treasure when a Raging Bull crashes into a Butterfly. ",

  "Les papillons ne vous poursuivent pas -- sauf si vous vous ne approchez, ils contournent les obstacles. "
  "Ils ne peuvent pas être tués de manière conventionnelle, mais vous gagner un trésor quand un Taureaux en "
  "charge percute un papillon."
  )

Orb("Horns", "Cornes")

S(
  "After you move while having this Orb, you immediately attack the next cell in the straight line "
  "(or two cells, when moving on a heptagon). This attack is slightly stronger than your normal "
  "attack: it can stun some of the monsters which cannot be killed or stunned normally.",
  
  "Après avoir commencé à bouger avec cette Orbe, vous attaquez immédiatement la case suivante en "
  "ligne droite (ou deux cases, quand on bouge sur un heptagone). Cette attaque est légèrement plus forte  "
  "que votre attaque normale : elle peut assommer un des monstres qui ne peut être tué ou assommé normalement."
  )

S("You pierce %the1.", "Vous percez %le1.")

// new 3D options
// --------------

S("Y shift", "shift Y")
S("Don't center on the player character.", "Ne pas centrer sur le joueur.")

S("camera rotation", "rotation de la caméra")
S("Rotate the camera. Can be used to obtain a first person perspective, "
  "or third person perspective when combined with Y shift.",
  "Tourne la caméra. Peut être utilisé pour obtenir un point de vue à la "
  "première ou troisième personne lorsque combiné avec Y shift.")

S("ball model", "modèle balle")
S("hyperboloid model", "modèle hyperboloïde")

S("projection in ball model", "projection en modèle balle")
S("This parameter affects the ball model the same way as the projection parameter affects the disk model.",
  "Ce paramètre affecte le modèle balle de la même manière que le paramètre de projection affecte le modèle disque.")
  

S("camera rotation in ball model", "rotation de la caméra en modèle balle")
S("Rotate the camera in ball/hyperboloid model.", "Tourne la caméra en modèle balle/hyperboloïde.")

// extra help
// ----------

S("\nSpecial conduct (still valid)\n", "\nConduite spéciale (toujours de rigueur) :")
S("\nSpecial conduct failed:\n", "\nConduite spéciale (échec) :")
S("Avoid escaping from a discharge (\"That was close\").",
  "Eviter d'échapper à une décharge (\"ce n'est pas passé loin\").")
S("Avoid chopping trees, using Orbs, and non-graveyard monsters in the Haunted Woods.",
  "Éviter d'abattre des arbres, utiliser des Orbes, et des monstres extérieurs au cimetière dans la Forêt Hantée.")
S("\n\nEasy %1 might disappear when you collect more of its kind.",
  "\n\nFacile, %1 peut disparaître quand vous en collecter plus de son type.")
S(" You need to go deep to collect lots of them.", "Vous devez aller en profondeur pour en collecter beaucoup.")

S("\nSpawn rate (as prize Orb): %1%/%2\n", "\nTaux d'apparition (pour les Orbes): %1%/%2\n")
S("\nSpawn rate (in Hubs): %1%/%2\n", "\nTaux d'apparition (dans les Croisements): %1%/%2\n")

S("Orbs of Safety always appear here, and may be used to escape.\n",
  "Des Orbes de Sécurité apparaissent toujours ici, et peuvent être utilisées pour s'échapper.")
  
S("You may be unable to leave %the1 if you are not careful!\n",
  "Vous risquez d'être piégé%e0 dans %le1  si vous ne faites pas attention !")

N4("Cellular Automaton", GEN_M, "Automate cellulaire", "Automates cellulaires")

S("A land for people wanting to experiment with cellular automata in the HyperRogue grid. "
  "Rules can be given on the command line; the default rules are:\n"
  "-c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "(-c0 or -c1 can be given if the same rule is to be used for hexagonal "
  "and heptagonal cells).",
  "Un pays pour les gens qui veulent expérimenter avec des automates cellulaires dans la "
  "grille d'HyperRogue. Les règles peuvent être données en lignes de commandes ; les règles "
  "par défaut sont :\n -c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "(-c0 ou -c1, peuvent être données si la même règle est utilisée pour les cases hexagonales "
  "et heptagonales).")

S("compass size", "taille de la boussole")

S("Zebra quotient", "Quotient du zèbre")
S("field quotient", "Quotient du champ")

/*
                "NEW_ACHIEVEMENT_8_10_NAME"     "On the Other Side"
                "NEW_ACHIEVEMENT_8_10_DESC"     "Find and collect Green Grass."
		"NEW_ACHIEVEMENT_8_11_NAME"	"Herd Bull"
		"NEW_ACHIEVEMENT_8_11_DESC"	"Collect 10 Green Grass."
		"NEW_ACHIEVEMENT_8_12_NAME"	"Raging Bull"
		"NEW_ACHIEVEMENT_8_12_DESC"	"Collect 25 Green Grass."
		"NEW_ACHIEVEMENT_8_13_NAME"	"Master of Prairie"
		"NEW_ACHIEVEMENT_8_13_DESC"	"Collect 50 Green Grass."
		"NEW_ACHIEVEMENT_8_14_NAME"	"Sleeping Bull"
		"NEW_ACHIEVEMENT_8_14_DESC"	"Collect a Spinel."
		"NEW_ACHIEVEMENT_8_15_NAME"	"Gadfly"
		"NEW_ACHIEVEMENT_8_15_DESC"	"Collect 10 Spinels."
		"NEW_ACHIEVEMENT_8_16_NAME"	"Butterfly"
		"NEW_ACHIEVEMENT_8_16_DESC"	"Collect 25 Spinels."
		"NEW_ACHIEVEMENT_8_17_NAME"	"Master of Bull Dash"
		"NEW_ACHIEVEMENT_8_17_DESC"	"Collect 50 Spinels."
		"NEW_ACHIEVEMENT_8_18_NAME"	"Worm of the World"
		"NEW_ACHIEVEMENT_8_18_DESC"	"Defeat a Sandworm in the Zebra Quotient geometry."
*/

// additional texts for 9.4k
// -------------------------

N4("Tortoise", GEN_F, "Tortue", "Tortues")
S("line patterns", "motifs de ligne")
S("1 turn", "1 tour")
S("%1 turns", "tours : %1")
S("inventory/kill mode", "mode de l'inventaire/tués")
S("images", "images")
S("letters", "lettres")
S("input", "entrée")
S("default", "défaut")
S("config", "configuration")
S("expansion", "expansion")

S("continue", "continuer")
S("main menu", "menu principal")
S("restart", "recommencer")
S("quit", "quitter")
S("exit menu", "quitter le menu")

S(
  "A tourist from another world. They mutter something about the 'tutorial', "
  "and claim that they are here just to learn, and to leave without any treasures. "
  "Do not kill them!",
  
  "Un%e0 touriste d'un autre monde, qui n'arrête pas de marmonner à propos d'un "
  "tutoriel et dit n'être là que pour apprendre, et pars sans trésor. "
  "Ne le tuez pas !")

S(
  "This monster has come from another world, presumably to steal our treasures. "
  "Not as fast as an Eagle, not as resilient as the guards from the Palace, "
  "and not as huge as the Mutant Ivy from the Clearing; however, "
  "they are very dangerous because of their intelligence, "
  "and access to magical powers.\n\n",

  "Ce monstre est venu d'un autre monde, probablement pour voler nos trésors. "
  "Pas aussi rapide qu'un Aigle ni aussi résilient que les gardes du Palais, "
  "ni aussi immense que le Lierre Mutant de la Clairière ; cependant, ils sont "
  "dangereux en raison de leur intelligence, et de leur accès aux pouvoirs magiques.\n\n"
  )

S(
  "Actually, their powers appear god-like...\n\n",
  "En réalité, leurs pouvoirs semblent similaires à ceux d'un dieu...\n\n"
  )

S(
  "Rogues will never make moves which result in their immediate death. "
  "Even when cornered, they are able to instantly teleport back to their "
  "home world at any moment, taking the treasures forever... but "
  "at least they will not steal anything further!\n\n",
  
  "Les vagabonds ne feront jamais un mouvement qui résultera en leur mort immédiate. "
  "Même encerclés, ils sont toujours capables de se téléporter instantanément vers "
  "leur monde natal à tout moment, emportant les trésors à jamais... "
  "au moins ils ne voleront rien de plus !\n\n")

S(
  "Despite this intelligence, Rogues appear extremely surprised "
  "by the most basic facts about geometry. They must come from "
  "some really strange world.\n\n",
  
  "Malgré leur intelligence, les vagabonds semblent très surpris "
  "par les faits les plus basiques à propos de la géométrie. Ils "
  "doivent venir d'un monde vraiment étrange.")  

S("change the alpha parameter to show the lines",
  "changer le paramètre alpha pour montrer les lignes")

S("triangle grid: not rings", "grille triangulaire : sans anneaux")
S("triangle grid: rings", "grille triangulaire : avec anneaux")
S("heptagonal grid", "grille heptagonale")
S("rhombic tesselation", "pavage rhombique")
S("triheptagonal tesselation", "pavage triheptagonale")
S("normal tesselation", "pavage normal")
S("big triangular grid", "grande grille triangulaire")
S("underlying tree", "arbre sous-jacent")
S("circle/horocycle tree", "cercle/horocycle d'arbres")
S("zebra triangles", "triangles du zèbre")
S("zebra lines", "lignes du zèbre")
S("vineyard pattern", "motif des vignobles")
S("firewall lines", "lignes pare-feu")
S("firewall lines: Palace", "ligne pare-feu : Palais")
S("firewall lines: Power", "ligne pare-feu : Pouvoir")

S("(ESC) tour menu", "(ESC) menu visite")
S("Try the Guided Tour to help with understanding the "
  "geometry of HyperRogue (menu -> special modes).\n\n",
  "Essayez la Visite Guidée pour comprendre la géométrie d'HyperRogue (menu -> modes spéciaux).\n\n")

S("guided tour", "visite guidée")
S("spherical geometry", "géométrie sphérique")
S("Euclidean geometry", "géométrie euclidienne")
S("more curved hyperbolic geometry", "géométrie hyperbolique plus courbée")
S("teleport away", "se téléporter au loin")
S("Shift-click a location to teleport there.", "Shift-cliquer un endroit pour s'y téléporter.")
S("Click a location to teleport there.", "Cliquer un endroit pour s'y téléporter.")

S("move through walls", "passer à travers les murs")
S("flash", "éclair")
S("static mode", "mode statique")
S("Static mode enabled.", "Mode statique activée.")
S("Static mode disabled.", "Mode statique désactivée.")
S("enable/disable texts", "textes activés/désactivés")
S("Help texts disabled.", "Textes d'aides désactivés.")
S("next slide", "diapositive suivante")
S("previous slide", "diapositive précédente")

S("This guided tour is different than most other game tutorials -- "
  "you are not forced to do anything, and you can go wherever you want.\n\n"
  "However, %the1 is not what we are talking about now. "
  "We will not explain this land at the moment, and you could potentially "
  "get lost there.\n\n"
  "Remember that you can get to the next slide by pressing Enter.",
  
  "Cette visite guidée est différente de la plupart des autres tutoriels de jeu -- "
  "vous n'avez pas besoin de faire quoi que se soit, et pouvez aller où vous voulez.\n\n"
  "Cependant, %le1 n'est pas ce dont on parle maintenant. Nous n'expliquerons pas ce "
  "pays maintenant, et vous pourriez vous y perdre.\n\n"
  "Rappelez vous que vous pouvez aller à la diapositive suivante en appuyant sur Entrée.")

S("Introduction", "Introduction")
S("Welcome to the HyperRogue Guided Tour!", "Bienvenue dans la visite guidée d'HyperRogue!")
S(
  "This tour is mostly aimed to show what is "
  "special about the geometry used by HyperRogue. "
  "It also shows the basics of gameplay, and "
  "how is it affected by geometry.\n\n"
  "You decide when you want to stop playing with the "
  "current \"slide\" and go to the next one, by pressing Enter. You can also "
  "press ESC to see a "
  "menu with other options.",
  
  "Cette visite a surtout pour but de montrer ce qui "
  "est spécial avec la géométrie utilisée par HyperRogue. "
  "Elle montre aussi les bases des mécaniques de jeu, et "
  "comment il est affecté par la géométrie.\n\n"
  "Vous décidez quand vous voulez arrêter de jouer avec la "
  "\"diapositive\" actuelle et aller à la suivante, en appuyant sur Entrée. "
  "Vous pouvez aussi appuyer ESC pour voir un menu avec d'autres options.")

S("Basics of gameplay", "Bases du jeu")
S("gain Ice Diamonds", "gagnez des Diamants des glaces")
S("The game starts in the Icy Lands. Collect the Ice Diamonds "
  "(press F1 if you do not know how to move). "
  "After you collect many of them, monsters will start to pose a challenge.\n"
  "As is typical in roguelikes and other games based on tactical skill rather "
  "than story, if you lose, you have to start a new one from the start. "
  "However, in this tutorial, you can simply press '4' "
  "to teleport away from a bad situation."
  "In general, the tutorial is rigged to show you what it "
  "wants -- for example, in this slide, you can press '5' to get "
  "lots of Ice Diamonds quickly.",
  "Le Jeu commence dans le Pays des glaces. Collecter des Diamants des glaces "
  "(appuyer sur F1, si vous ne savez pas comment vous déplacer). "
  "Après avoir collecté beaucoup d'entre eux, les monstres commenceront à poser "
  "problème.\n Comme souvent dans les roguelike et autres jeux basés sur les "
  "compétences tactiques plutôt que sur l'histoire, si vous perdez il faut recommencer "
  "depuis le début. Cependant dans ce tutoriel vous pouvez appuyer sur '4' pour échapper "
  "à une situation délicate. En général le tutoriel est faussé afin de montrer ce qu'il "
  "veut -- par exemple dans cette diapositive, vous pouvez appuyer sur '5', pour avoir "
  "plein de Diamants des glaces rapidement.")

S("Hypersian Rug model", "Modèle de tapis Hypersans")
S(
  "New players think that the action of HyperRogue takes place on a sphere. "
  "This is not true -- the next slide will show the surface HyperRogue "
  "actually takes place on.\n\n"
  "Use arrow keys to rotate the model, and Page Up/Down to zoom.\n\n"
  "If you do not see anything, press '5' to try a safer renderer.",
  

  "Les nouveaux joueurs pensent que l'action d''HyperRogue a lieu sur une sphère. "
  "Ce n'est pas le cas -- la diapositive suivante montrera la surface sur laquelle "
  "HyperRogue se passe réellement. "
  "Utiliser les touches flèches pour faire tourner le modèle et Page Up/Down pour zoomer.\n\n"
  "Si vous ne voyez rien, appuyer sur '5' pour un rendu plus sûr."
  )

S("Expansion", "Expansion")
S(
  "The next slide shows the number of cells in distance 1, 2, 3, ... from you. "
  "It grows exponentially: there are more than 10^100 cells "
  "in radius 1000 around you, and you will move further away during the game!\n\n"
  "This is extremely important in the design of HyperRogue. "
  "HyperRogue has many navigational puzzles -- what would be simple in Euclidean world "
  "is extremely tricky "
  "in hyperbolic geometry (you want to reach a specific location 20 cells away, "
  "which of the thousands of possible directions should you take?); however, other things virtually impossible in Euclidean "
  "world become easy in HyperRogue. "
  "HyperRogue had to be specially designed so that it is impossible to grind the "
  "infinite world. There are almost no permanent upgrades; collecting treasures "
  "brings you benefits, but trying to get too many of the same kind is extremely dangerous.",
  
  "La diapositive suivante montre le nombre de cases à une distance 1, 2, 3, ... de vous. "
  "Cela croît exponentiellement : il y a plus de 10^100 cases dans le rayon 1000 autour de "
  "vous, et vous irez encore plus loin pendant le jeu !\n\n"
  "Ceci est extrêmement important dans le design d'HyperRogue. HyperRogue a de nombreux puzzles "
  "de navigation -- qui seraient très simples dans un monde Euclidien est très complexe en "
  "géométrie hyperbolique (vous voulez atteindre un lieu spécifique distant de 20 cases, quelle "
  "direction entre les milliers de possible choisir ?) ; cependant d'autre choses virtuellement "
  "impossible dans un monde euclidien devient facile dans HyperRogue. HyperRogue a été conçu pour "
  "qu'il soit impossible de farmer dans le monde infini. Il n'y a presque aucun upgrade permanent ; "
  "ramasser les trésors vous apporte des récompenses, mais essayer d'avoir trop de la même sorte est "
  "extrêmement dangereux.")

S("Tiling and Tactics", "Carrelage et Tactiques")
S(
  "The tactics of fighting simple monsters, such as the Yetis from the Icy Lands, "
  "might appear shallow, but hyperbolic geometry is essential even there. "
  "In the next slide, you are attacked by two monsters at once. "
  "You can make them line up simply by "
  "running away in a straight line. "
  "Press '2' to try the same in the Euclidean world -- it is impossible.",
  "Les tactiques pour combattre de simples monstres, telle que les Yétis du Pays des glaces "
  "peuvent paraître superficielles, mais la géométrie hyperbolique est essentielle même ici. "
  "Dans la diapositive suivante, vous êtes attaqués par deux monstres à la fois. Vous pouvez "
  "les faire s'aligner simplement en fuyant en ligne droite. Appuyer '2' pour essayer la même"
  "chose dans le monde Euclidien -- c'est impossible.")

S("Straight Lines", "Lignes Droites")
S("Hyperbolic geometry has been discovered by the 19th century mathematicians who "
  "wondered about the nature of paralellness. Take a line L and a point A. "
  "Can a world exist where there is more than one line passing through A "
  "which does not cross L?\n\n"
  "The Icy Land will be very dangerous if you have lots of Ice Diamonds -- "
  "lots of Yetis and Ice Wolves hunting you! But the other lands, where "
  "you have no treasures yet, will still be (relatively) safe.\n\n"
  "Wander further, and you should find Crossroads quickly -- "
  "the Great Walls are straight lines, and indeed, they work differently than in "
  "Euclidean. On the other side of Great Walls, you see other lands -- "
  "there are about 60 lands in HyperRogue, based "
  "on different mechanics and aspects of hyperbolic geometry.",
  
  "La géométrie hyperbolique a été découvert par des mathématiciens du 19ème siècle "
  "qui s'interrogeaient sur la nature du parallélisme. Prendre une ligne L et un point A. "
  "Peut il exister un monde où il y a plus d'une ligne passant par A, qui ne croise pas L ?\n\n"
  "Aventurez vous plus loin et vous trouverez les Croisements assez vite -- les Grands "
  "Murs sont des lignes droites, et en effet, elles fonctionnent différemment que dans le "
  "monde euclidien. De l'autre côté des Grands Murs vous voyez d'autres pays -- il "
  "y a environ 60 pays dans HyperRogue, basé sur différents mécanismes et aspects "
  "de la géométrie hyperbolique.")

S("Running Dogs", "Chiens Courant")
S(
  "To learn more about straight lines, "
  "wander further, and you should find the Land of Eternal Motion. "
  "Try to run in a straight line, with a Running Dog next to you. "
  "Even though the Running Dog runs at the same speed as you, "
  "it will appear to go slower -- this is because you are running "
  "in a straight line, and the Running Dog has to run in a curve "
  "called an equidistant.\n\n"
IF_MAC(
 "Remember that you can click with right Shift on anything to get more information."
,
 "Remember that you can right click on anything to get more information."
),
  "Pour en apprendre plus sur les lignes droites, aventurez "
  "vous plus loin, et vous trouverez le Pays du Mouvement Perpétuel. "
  "Essayer de courir en ligne droite avec un Chien Courant à côté de vous, "
  "Même si le Chien Courant va à la même vitesse que vous il semble plus "
  "lent -- c'est parce qu'il doit courir sur une courbe appelée une équidistante.\n\n"
IF_MAC(
 "Rappelez vous que vous pouvez cliquez droit avec Shift sur n'importe quoi pour plus d'information."
,
 "Rappelez vous que vous pouvez cliquez droit sur n'importe quoi pour plus d'information."
)
  )

S("Equidistants", "Équidistantes")
S(
  "Equidistants are curves which are at some fixed distance from a "
  "straight line. Some lands in HyperRogue are based on equidistants; "
  "you should see them after wandering a bit more.\n\n"
  "This tutorial gives you freedom to go wherever you choose, "
  "but we do not recommend going deep into the Dungeon or the Ocean -- "
  "getting back might be difficult.",
  
  "Les équidistantes sont des courbes qui sont à une distance fixée d'une. "
  "ligne droite. Certains pays d'HyperRogue sont basés sur des équidistantes ; "
  "Vous devriez les voir après un peu d'errance.\n\n"
  "Ce tutoriel vous donne la liberté d'aller là où vous choisissez, mais "
  "nous ne recommandons pas de vous enfoncer dans le Donjon ou l'Océan -- "
  "revenir peut être difficile.")

S("Circles", "Cercles")
S(
  "Circles are strange in hyperbolic geometry too. "
  "Look for the Castle of Camelot in the Crossroads; "
  "the Round Table inside is a circle of radius 28. "
  "Finding its center is a difficult challenge.\n\n"
  "Press '5' to cheat by seeing the smaller circles too.\n\n"
  "Note: Camelot and some other lands are unlocked earlier in the Tutorial than in a real game.",

  "Les cercles sont aussi étranges en géométrie hyperbolique. "
  "Regardez le château de Camelot dans les Croisements ; "
  "La Table Ronde à l'intérieur est un cercle de rayon 28. "
  "Trouver son centre est un vrai défi.\n\n"
  "Appuyez sur '5' pour tricher en voyant aussi des cercles plus petits.\n\n"
  "Note : Camelot et d'autres pays sont débloqués plus tôt dans le tutoriel que dans le jeu réel. ")

S("Horocycles", "Horocycle")
S("Horocycles are similar to circles, but you cannot reach their center at all -- "
  "they can be understood as limit circles of infinite radius centered in some point "
  "in infinity (also called an ideal point).\n\n"
  "Go to R'Lyeh, and you should quickly find a Temple of Cthulhu there. "
  "Each circle of columns is actually a horocycle. Horocycles in a given "
  "temple are concentric, and there is an infinite number of them.",

  "Les horocycles sont similaires aux cercles, mais vous ne pouvez atteindre leur centre -- "
  "ils peuvent êtres compris comme des cercles limites de rayon infini centrés en un point à "
  "l'infini (aussi appelés point idéal).\n\n"
  "Allez à R'Lyeh, et vous y trouverez rapidement un Temple de Cthulhu. Chaque cercle de colonnes "
  "est en réalité un horocycle. Les horocycles d'un temple donné sont concentriques, et il y en a "
  "un nombre infini.")

S("Half-plane model", "Modèle semi-planaire")
S("The game is normally displayed in the so called Poincaré disk model, "
  "which is a kind of a map of the infinite hyperbolic world. "
  "There are many projections of Earth, but since Earth is curved, "
  "all of them have to distort distances or angles in some way -- "
  "the same is true in hyperbolic geometry. "
  "The next slide shows another model, called the Poincaré upper half-plane model. In this model, "
  "horocycles centered at one specific ideal point are drawn as straight lines.",
  
  "Le jeu est normalement affiché dans le modèle du disque de Poincaré, "
  "qui est une sorte de carte du monde hyperbolique infini. "
  "Il y a de nombreuses projections de la Terre, mais comme elle est courbée, "
  "elles doivent toutes distordre les distances ou les angles d'une manière ou "
  "d'une autre -- c'est également le cas pour la géométrie hyperbolique. "
  "La diapositive suivante montre un autre modèle appelé le modèle de Poincaré "
  "semi-planaire supérieur. Dans ce modèle, les horocycles centrés autour d'un "
  "point idéal sont dessinés comme des lignes droites.")

S("Curvature", "Courbure")
S("gain Orb of the Sword", "gagner l'Orbe de l'Épée")
S(
  "Now, go to the Burial Grounds and find an Orb of the Sword. The Sword appears to "
  "always be facing in the same direction whatever you do, and it appears that "
  "you have to rotate the sword to excavate the treasures; "
  "yet, it is possible to excavate them! You might have already noticed "
  "that the world rotates after you move around a loop and return to an old "
  "place.\n\n"
  "This is related to the fact that the world of HyperRogue is curved, and "
  "the sum of angles in a triangle is not equal to 180 degrees.",

  "Maintenant, allez au Site Funéraire et trouvez une Orbe de l'Épée. L'épée "
  "semble toujours pointer dans la même direction quoi que vous fassiez, et "
  "il semble que vous deviez pivoter l'épée pour déterrer les trésors ; mais "
  "il est possible que de les déterrer ! Vous avez peut-être déjà remarqué que "
  "le monde pivote après que vous alliez autour d'une boucle et retourniez à un "
  "ancien endroit.\n\n"
  "C'est lié au fait que le monde d'HyperRogue est courbé, et la somme des angles "
  "d'un triangle n'est pas égale à 180 degrés.")

S("Periodic patterns", "Motifs périodiques")
S("Hyperbolic geometry yields much more interesting periodic patterns "
  "than Euclidean.",
  "La géométrie hyperbolique résulte en des motifs périodiques bien plus "
  "intéressant qu'en géométrie euclidienne.")

S("Periodic patterns: application", "Les motifs périodiques : application")
S("Many lands in HyperRogue are based around periodic patterns. "
  "For example, both Zebra and Windy Plains are based on the pattern "
  "shown in the previous slide. "
  "Such lands often have tree-like nature.",
  "De nombreux pays d'HyperRogue sont basés sur des motifs périodiques. "
  "Par exemple, le Zèbre et les Plaines Venteuses sont basées sur des motifs "
  "montrés dans la diapositive suivante.")

S("Fractal landscapes", "Paysages fractals")
S("On the following slide, the colors change smoothly in the whole infinite world. "
  "Again, this works better than in Euclidean geometry.",
  "Sur la diapositive suivante, les couleurs changent doucement dans le monde entier "
  "infini. A nouveau, cela marche mieux qu'en géométrie euclidienne.")

S("Fractal landscapes: application", "Paysages fractals : application")
S("create a baby tortoise", "créer un bébé tortue")
S(
  "This is applied in HyperRogue to create landscapes, such as the chasms in the "
  "land of Reptiles or the Dragon Chasms, which you should find quickly. "
  "Also in the Dragon Chasms, you can find a Baby Tortoise, and try to find "
  "a matching adult tortoise in the Galápagos. "
  "There are over two millions of species, but since there is so much space in "
  "hyperbolic geometry, finding a matching tortoise is possible. The brighter "
  "the color in Galápagos is, the more aspects of the tortoises in the given "
  "area are matching.",
  
  "Ceci est appliqué dans HyperRogue pour créer des paysages, comme les gouffres "
  "dans le pays des Reptiles ou les Gouffres aux Dragons, que vous devriez trouver "
  "rapidement. Aussi dans les Gouffres aux Dragons, vous pouvez trouver un Bébé "
  "Tortue, et essayer de trouver une tortue adulte correspondante dans les Galápagos. "
  "Il y a plus de deux millions d'espèces, mais comme il y a beaucoup d'espace avec "
  "la géométrie hyperbolique, il est possible de trouver une tortue correspondante. "
  "Plus la couleur des Galápagos est brillante, plus d'aspects des tortues dans un "
  "endroit donné correspondent.")

S("Poincaré Ball model", "Modèle de la balle de Poincaré")
S(
    "The Poincaré disk model is a model of a hyperbolic *plane* -- you "
    "might wonder why are the walls rendered in 3D then.\n\n"
    "HyperRogue actually assumes that the floor level is an equidistant surface "
    "in a three-dimensional hyperbolic world, and the camera is placed above the "
    "plane that the surface is equidistant to (which boils down to showing "
    "the floor level in Poincaré disk model).\n\n"
    "This is shown on the next slide, in the Poincaré ball model, which is "
    "the 3D analog of the Poincaré disk model.",
    
  "Le modèle du disque de Poincaré est le modèle d'un *plan* hyperbolique -- "
  "vous pouvez vous demander pourquoi les murs sont rendus en 3D.\n\n"
  "HyperRogue suppose en réalité que le niveau du sol est une surface équidistante "
  "d'un monde hyperbolique tri-dimensionnel, et la caméra est placée au-dessus du "
  "plan auquel la surface est équidistante (ce qui revient à montrer le niveau du sol "
  "dans le modèle du disque de Poincaré).\n\n"
  "Ceci est montré sur la diapositive suivante, dans le modèle de balle de Poincaré, "
  "qui est l'analogue 3D du modèle de disque de Poincaré.")

S("Hyperboloid model", "Modèle hyperboloïde")
S(
    "Let's see more models of the hyperbolic plane. "
    "This model uses a hyperboloid in the Minkowski geometry; "
    "it is used internally by HyperRogue.",
    
    "Découvrons plus de modèles du plan hyperbolique. "
    "Ce modèle utilise la géométrie hyperbolique dans la géométrie de Minkowski ; "
    "il est utilisé en interne par HyperRogue.")

S("Beltrami-Klein model", "Modèle de Beltrami-Klein")
S(
    "This model renders straight lines as straight, but it distorts angles.",
    "Ce modèle montre les lignes droites comme droites, mais il déforme les angles.")

S("Gans model", "Modèle de Gans")
S(
  "Yet another model, which corresponds to orthographic projection of the "
  "sphere. Poincaré disk model, Beltrami-Klein model, and the Gans "
  "model are all obtained by looking at either the hyperboloid model or an "
  "equidistant surface from various distances.",
  "Encore un autre modèle, qui correspond à la projection orthographique d'une "
  "sphère. Le modèle disque de Poincaré, de Beltrami-Klein et de Gans sont "
  "tous obtenus en regardant soit le modèle hyperboloïde soit une surface "
  "équidistante depuis différentes distances.")

S("Band model", "Modèle ruban")
S("render spiral", "afficher la spirale")
S(
  "The band model is the hyperbolic analog of the Mercator projection of the sphere: "
  "a given straight line is rendered as a straight line, and the rest of the "
  "world is mapped conformally, that is, angles are not distorted. "
  "Here, we take the straight line connecting your starting point and your "
  "current position -- usually the path taken by the player is surprisingly "
  "close to a straight line. Press '8' to see this path.\n\n"
  "If you want, press '5' to see it rendered as a spiral, although it takes lots of time and "
  "memory.",
  "Le modèle ruban est l'analogue hyperbolique de la projection de Mercator de la sphère : "
  "une ligne droite donnée reste une ligne droite, et le reste de la carte est créé de manière "
  "conforme, c.à.d les angles sont conservés. "
  "Ici, nous prenons la ligne droite connectant votre point de départ et votre "
  "position actuelle -- habituellement le chemin pris par le joueur est étonnement "
  "proche d'une ligne droite. Appuyez sur '8', pour voir ce chemin.\n\n"
  "Si vous voulez, appuyez sur '5' pour l'afficher en spirale, même si cela prend beaucoup de "
  "temps et de mémoire.")

S("Conformal square model", "Modèle du carré conforme")
S("The world can be mapped conformally to a square too.",
  "Ce monde peut aussi être cartographié conformément à un carré.")

S("Shoot'em up mode", "Mode shoot'em up")
S("In the shoot'em up mode, space and time is continuous. "
  "You attack by throwing knives. "
  "Very fun with two players!\n\n"
  "There are other special modes too which change the gameplay or "
  "focus on a particular challenge.",
  "Dans le mode shoot'em up le temps et l'espace sont continus. "
  "Vous attaquez en jetant des couteaux."
  "Très amusant à deux joueurs !\n\n"
  "Il y a d'autres modes spéciaux qui changent le gameplay ou se "
  "concentrent sur un défi en particulier.")

S("THE END", "LA FIN")
S("leave the tour mode", "quittez le mode visite guidée")
S(
  "This tour shows just a small part of what you can see in the world of HyperRogue. "
  "For example, "
  "hyperbolic mazes are much nicer than their Euclidean counterparts. "
  "Have fun exploring!\n\n"
  "Press '5' to leave the tour mode.",

  "Cette visite ne montre qu'une petite partie de ce que vous pouvez voir dans le monde "
  "d'HyperRogue. Par exemple, les labyrinthes hyperboliques sont bien mieux que leur "
  "équivalents euclidiens. Amusez vous à explorer ! "
  "Appuyer sur '5', pour quitter le mode visite guidée."
  )

/*
// for 10.0
*/

// Orb Strategy mode
// -----------------

S("Orb Strategy mode", "Mode stratégique des Orbes")

S(
  "You are playing in the Orb Strategy Mode. Collecting treasure "
  "gives you access to magical Orb powers. In this mode, "
  "unlocking requirements are generally higher, and "
  "several quests and lands "
  "give you extremely powerful Orbs of the Mirror.\n",

  "Vous jouez en Mode stratégique des Orbes. Ramasser des trésors "
  "vous donne accès à des pouvoirs d'Orbes magiques. Dans ce mode, "
  "les prérequis pour débloquer des pays sont en général plus hauts, "
  "et plusieurs quêtes et pays donnent l'extrêmement puissante Orbe "
  "du Miroir.\n")

S("The treasure gives your magical powers!", "Ce trésor vous donne des pouvoirs magiques !")
S("Press 'i' to access your magical powers.", "Appuyer sur 'i' pour accéder aux pouvoirs magiques.")
S("inventory", "inventaire")
S("mirror what?", "réfléchir quoi ?")
S("Which orb to use?", "Quelle orbe utiliser ?")
S("Unlocked by: %1 in %2", "Débloqué par : %1 dans %2")
S(" (next at %1)", " (suivant à %1)")
S(" (next at %1 to %2)", " (suivant à %1 jusqu'à %2)")
S("Number of uses left: %1", "Nombre d'utilisations restantes : %1")
S("You mirror %the1.", "Vous réfléchissez %le1.")
S("You need to stand next to a magic mirror or cloud to use %the1.", 
  "Vous devez vous tenir près d'un miroir magique ou nuage pour utiliser %le1.")
S("Each orb type can be mirrored only once.", "Chaque type d'orbe peut être réflétée une seule fois.")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Yendor appear in Hell after "
  "you collect 25 Demon Daisies in Hell, in Crossroads/Ocean after you collect 50, "
  "and everywhere after you collect 100.",

  "\n\nDans le mode stratégique des orbes, les orbes de Yendor apparaissent en Enfer "
  "après avoir ramassée 25 Marguerites démoniaques en Enfer, dans les Croisements/Océan "
  "après en avoir ramassé 50, et partout après 100."
  )  

S(
  "\n\nIn the Orb Strategy Mode, dead orbs are available once you collect "
  "10 Necromancer Totems in the Graveyard.",
  
  "\n\nDans le mode stratégique des orbes, les orbes mortes sont disponibles "
  "lorsque vous avez ramassé 10 Totem de Nécromancien dans le Cimetière.")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Safety can be gained by "
  "collecting Phoenix Feathers in the Land of Eternal Motion. "
  "You can also find unlimited Orbs of Safety in the Crossroads "
  "and the Ocean (after collecting 25 Phoenix Feathers) "
  "and in the Prairie.",
  
  "\n\nDans le mode stratégiques des orbes, les Orbes de Sécurité "
  "peuvent être gagnées en ramassant des Plumes de Phoenix dans le "
  "Pays du Mouvement perpétuel. Vous pouvez aussi trouver un nombre "
  "illimité d'Orbes de Sécurité dans les Croisements, l'Océan (après "
  "avoir ramassé 25 Plumes de Phoenix) et dans la Prairie."
  )

S(
  "\n\nCollect %the1 to gain an extra Orb of the Mirror. "
  "You can gain further Orbs of the Mirror by collecting 2, 4, 8...",

  "\n\nRamassez %le1 pour gagner une Orbe du Miroir en plus. "
  "Vous pouvez gagner plus d''Orbes du Miroir en ramassant 2, 4, 8..."
  )

S(
  "\n\nIn the Orb Strategy Mode, the Orb of Luck also "
  "significantly increases the frequency of Great Walls, Crossroads IV, "
  "and sub-lands.",
  
  "\n\nDans le mode stratégique des orbes, l'Orbe de Chance "
  "augmente aussi significativement la fréquence des Grands Murs, "
  "des Croisements IV et des sous-pays.")

S("\n\nIn the Orb Strategy Mode, each 25 Necromancer's Totems "
  "you are given a random offensive Orb.",
  
  "\n\nDans le mode stratégique des orbes, pour chaque 25 Totems "
  "de Nécromanciens vous recevez une orbe offensive aléatoire.")
 
S(
  "Use Orb of the Mirror to gain copies of one of your Orbs; "
  "mirroring weaker Orbs usually yields more copies. "
  "It can only be used once per Orb type, "
  "and only when you are next to a mirror.",
  
  "Utiliser une Orbe du Miroir pour gagner des copies d'une de "
  "vos Orbes ; réfléchir des orbes plus faibles génère habituellement "
  "plus de copies."
  "Cela peut être utilisé une fois par type d'Orbe, et seulement quand "
  "vous êtes près d'un miroir.")
  
S("Uses to gain: %1", "Utilisations à gagner : %1")
S("already mirrored", "déjà copié")
  
N4("your orbs", GEN_F, "Votre Orbe", "Vos Orbes")
S("Click this to see your orbs.", "Cliquer pour voir vos orbes.")                                                   

// peaceful mode
// -------------

S("configure keys/joysticks", "configurer raccourcis/joysticks")
S("peaceful mode", "mode pacifique")

// config changes
S("Press F5 or 'o' to try again!", "Appuyer sur F5 ou 'o' pour essayer à nouveau !")  
S("aura brightness", "brillance de l'aura")
S("aura smoothening factor", "facteur de lissage de l'aura")
S("graphics configuration", "configuration des graphiques")
S("special display modes", "modes d'affichage spéciaux")
S("openGL mode", "mode OpenGL")
S("anti-aliasing", "antialiasing")
S("line width", "épaisseur de ligne")
S("configure panning and general keys", "configurer le panorama et les touches générales")

S("\n\nHint: use 'm' to toggle cells quickly",
  "\n\nAstuce : utiliser 'm' pour changer de case rapidement")

// cell pattern names
S("football", "football")
S("dark rainbow landscape", "paysage d'arc-en-ciel sombre")
S("field pattern", "motif de champ")
S("field pattern C", "motif de champ C")
S("field pattern D", "motif de champ D")
S("field pattern N", "motif de champ N")
S("field pattern S", "motif de champ S")
S("four triangles", "quatre triangles")
S("big triangles: rings", "grands triangles : anneaux")

// missing for the Tutorial
S("This Orb is not compatible with the Tutorial.", "Cette Orbe n'est pas compatible avec le tutoriel.")

// local scores
S("turns", "tours")
S("cells", "cases")
S("sort", "trier")
S("choose", "choisir")
S("play", "jouer")

// draw editor
S("autochoose", "choisir automatiquement")
S("c = choose", "c = choisir")
S("b = switch auto", "b = déclencher auto")

// mission screen hints

S(
  "If you collect too many treasures in a given land, it will become "
  "extremely dangerous. Try other lands once you have enough!",
  "Si vous ramassez trop de trésors dans un pays donné, il deviendra "
  "extrêmement dangereux. Essayez d'autres pays lorsque vous en avez assez !")

S(
  "Remember that you can right click almost anything for more information.",
  "Rappelez vous que vous pouvez cliquer droit presque n'importe quoi pour "
  "plus d'informations.")

S("Want to understand the geometry in HyperRogue? Try the Guided Tour!",
  "Vous voulez comprendre la géométrie d'HyperRogue? Faites la Visite Guidée !")

S(
  "Collecting 25 treasures in a given land may be dangerous, "
  "but allows magical Orbs of this land to appear in other places!",
  "Ramasser 25 trésors dans un pays donné peut paraître dangereux, "
  "mais permet aux Orbes magiques de ce pays d’apparaître ailleurs !")

S(
  "Press ESC to view this screen during the game.",
  "Appuyer sur ESC pour voir cet écran pendant le jeu.")

S("The 'world overview' shows all the lands in HyperRogue.",
  "La 'vue d'ensemble du monde' montre tout les pays d'HyperRogue."
  )

S("Press 'o' to see all the lands in HyperRogue.",
  "Appuyer sur 'o' pour voir tout les pays d'HyperRogue.")

S(
  "Want another type of game? Want more challenge?\n"
  "HyperRogue has many special modes and challenges that "
  "significantly change the gameplay. Try them!",
  
  "Voulez vous un autre type de jeu ? Plus de défis ?\n"
  "HyperRogue a de nombreux modes spéciaux et défis qui "
  "changent significativement le jeu. Essayez les !")

S(
  "Hyperbolic geometry can be shown in many ways.",
  "La géométrie hyperbolique peut être montrée de nombreuses manières...")

S(
  "You do not want to lose the game from a single mistake?\n"
  "Do you want to use the Orbs strategically?\n"
  "Try the Orb Strategy mode!",
  
  "Vous ne voulez pas perdre le jeu à cause d'une simple erreur ?\n"
  "Voulez vous utiliser les Orbes de manière stratégique ? "
  "Essayer le mode stratégique des Orbes !")

S(
  "Do you think you are playing on a ball? "
  "This is far from the truth!\n",
  
  "Vous pensez jouer sur une sphère ? C'est loin de la vérité !\n")

S(
  "Did you know that the path you take during the game "
  "is usually very close to a straight line?\n",

  "Saviez vous que le chemin que vous suivez durant le jeu "
  "est habituellement très proche d'une ligne droite ?")

S("Show me!", "Montre moi !")

S(
  "You are %1 cells away from the starting point, or "
  "the place where you used an Orb of Safety last time. "
  "There are %2 such cells.\n",
  
  "Vous êtes à %1 cases du point de départ, ou "
  "du lieu où vous avez utilisé une Orbe de Sécurité "
  "pour la dernière fois. Il y a %2 cases de ce type.\n")

S("about ", "à propos ")
S(" (%1 more digits)", " (%1 nombres de plus)")

S("see how it ended", "voir comment ça s'est terminé")

// other missing/new things
S("Orb unlocked: %1", "Orbe débloquée : %1")
S("Secondary orb: %1", "Orbe secondaire : %1")
S(" to submerge", " à submerger")
S(" to surface", " à remonter")
S("%The1 says, \"not this place, it looks even worse...\"", 
  "%Le1 dit, \"pas cet endroit, ça a l'air encore pire...\"")
S("torus", "tore")
S(" (click to use)", " (cliquer pour utiliser)")
N4("Hall of Mirrors", GEN_F, "Salle des Miroirs", "Salles des Miroirs")
Orb("the Mirror", "Miroir")
N4("Reflection", GEN_M, "Reflet", "Reflets")
N4("mirror wall", GEN_M, "miroir mural", "miroirs muraux")

S("This would only move you deeper into the trap!",
  "Ceci ne ferait que vous jeter encore plus loin dans le piège !")

S("You swing your sword at the mirror.", "Vous balancez votre épée dans le miroir.")
N4("Mimic", GEN_F, "Mimique", "Mimiques")
N4("Narcissist", GEN_M, "Narcissique", "Narcissiques")
N4("Mirror Spirit", GEN_M, "Esprit de Miroir", "Esprits de Miroir")

S("This person loves to look at their own reflection in the mirror. "
  "He believes himself to be one of the most important creatures in this world, "
  "and hates those who do not admire him.",

  "Cette personne adore contempler son propre reflet dans le miroir. "
  "Il croit être une des créatures les plus importantes de ce monde, "
  "et hait ceux qui ne l'admirent."
  )

S(
    "A long time ago a mighty warrior was guarding the mirrors from being broken. "
    "While this warrior is no longer alive, his reflections have gained life of "
    "their own, and will punish the intruders.\n\n"
    "If you attack a Mirror Spirit physically, it is delayed, but not destroyed -- "
    "more reflections will come out of the mirror. Use Mimics to destroy them.",
    
    "Il y a bien longtemps un puissant guerrier protégeait ces miroirs de la destruction. "
    "Alors que ce guerrier n'était plus, ces reflets ont pris vie, et puniront les "
    "les intrus.\n\n"
    "Si vous attaquez un Esprit de Miroir physiquement, il est ralenti mais pas détruit -- "
    "plus de réflexions sortiront du miroir. Utilisez des Mimiques pour les détruire.")

// peaceful texts

S("memory game", "jeu de mémoire")
S("display hints", "afficher des astuces")
S("hyperbolic puzzles", "puzzles hyperboliques")

// missing descriptions

S(  "A strange land filled with mirrors. "
    "Break magic mirrors and enter clouds of mirage to "
    "gain treasures and helpful Mimics.",
    
    "Un étrange pays rempli de miroirs. "
    "Brisez des miroirs magiques et entrez dans les nuages de "
    "mirages pour gagner des trésors et des Mimiques bien utiles.")

S(
  "A perfect mirror wall. It is unbreakable "
  "and impassable "
  "even for aethereal beings, and everything "
  "you see inside is just an image of "
  "the real world; you can swing your sword "
  "at them, but that will not destroy them "
  "in the real world. "
  "Mirror walls reflect Mimics, lightning bolts, and "
  "missiles perfectly.",
  
  "Un miroir mural parfait. Il est incassable et "
  "infranchissable même pour les êtres éthérés, et "
  "et tout ce que vous voyez à l'intérieur est une "
  "image du monde réel ; vous pouvez les frapper à "
  "l'épée, mais cela ne les détruira pas dans le "
  "monde réel. "
  "Les Miroirs muraux réfléchissent les Mimiques, éclairs, "
  "et missiles parfaitement."
  )

S(
"In the peaceful mode, you just explore the world, "
    "without any battles; there are also several "
    "navigational puzzles available. In the memory game, "
    "you have to collect as many Dodecahedra as you can, "
    "and return to the starting point -- hyperbolic geometry "
    "makes this extremely difficult! Other hyperbolic puzzles "
    "include the Burial Grounds (excavate the treasures "
    "using your magical sword), Galápagos (try to find an adult "
    "tortoise matching the baby), Camelot (find the center of "
    "a large hyperbolic circle), and Palace (follow the mouse). "
    "Other places listed are for exploration.",

"Dans le mode pacifique, vous explorez juste le monde, "
  "sans aucun combat : il y a aussi plusieurs puzzles "
  "de navigation disponibles. Dans le jeu de mémoire, "
  "Vous devez ramasser autant de dodécaèdres que possible,"
  "et retourner au point de départ -- la géométrie hyperbolique "
  "rend cela extrêmement difficile ! D'autres puzzles hyperboliques "
  "incluant les Sites Funéraires (excaver les trésors avec "
  "votre épée magique), les Galápagos (essayer de trouver une"
  "tortue adulte correspondant au bébé) Camelot (trouver le "
  "centre d'un grand cercle hyperbolique), et le Palais (suivre "
  "la souris). D'autres endroits sont listés pour exploration.")

S("puzzles and exploration", "puzzles et exploration")

S("Collect as many Dodecahedra as you can, then return here!",
  "Ramassez autant de Dodécaèdres que possible, puis revenez ici !")

S("reset all configuration", "réinitialiser toute la configuration")
S("Are you sure?", "Êtes vous sûr%0e ?")
S("yes, and delete the config file", "oui, et supprimer le fichier de configuration")
S("yes", "oui")
S("cancel", "annuler")
S("reset the special game modes", "réinitialiser les modes de jeu spéciaux")

// extra flavor messages for the OSM
// ---------------------------------

S("You feel the presence of free saves on the Crossroads.",
  "Vous sentez la présence de sauvegardes gratuites dans les Croisements.")

S("You feel the Orbs of Yendor nearby...",
  "Vous sentez que les Orbes de Yendor sont près...")
  
S("You feel the Orbs of Yendor in the Crossroads...",
  "Vous sentez les Orbes de Yendor dans les Croisements...")

S("You feel the Orbs of Yendor everywhere...",
  "Vous sentez les Orbes de Yendor partout...")

S("You have gained an offensive power!",
  "Vous avez gagné un pouvoir offensif !")

S("A small reward for braving the Hell.",
  "Une petite récompense pour avoir bravé l'Enfer.")

S(" (mirrored)", " (réfléchi)")
S(" (used %1 times)", " (utilisé %1 fois)")
S("Extras:", "Suppléments :") // extra Orbs gained in OSM

// cheats
// ------

S("unlock Orbs of Yendor", "débloquez les Orbes de Yendor")
S("Collected the keys!", "Clés ramassées !")
S("Saved the Princess!", "Princesse sauvée!")
S("save a Princess", "sauve une princesse")

// other
// -----
  
S("Note for mobiles", "Note pour les téléphones")
S(
    "This tour is designed for computers, "
    "and keys are given for all actions. It will "
    "work without a keyboard though, although less "
    "comfortably -- just ignore the keys "
    "given and select options from MENU.\n\n"
    "Select 'next slide' from MENU.",

    "Cette visite guidée est conçue pour les "
    "ordinateurs et des touches sont données "
    "pour toutes les actions. Cela marchera "
    "néanmoins sans clavier, même si ce sera moins "
    "confortable -- ignorez juste les touches données "
    "et sélectionnez les options depuis le MENU.\n\n"
    "Sélectionner 'diapositive suivante' depuis le MENU.")

S("quick mouse", "souris rapide")
S("This combination is known to be buggy at the moment.", "Cette combinaison est connue pour être buguée pour l'instant.")

// extra Princess texts
// --------------------
 
S("\"I do not like butterflies. They are treacherous.\"", 
  "\"Je n'aime pas les papillons. Ils sont traîtres.\"")

S("\"I hate roses.\"", "\"Je hais les roses.\"")

S("\"In this world there is plenty of space for everyone. We do not need wars.\"", 
  "\"Dans ce monde il y a plein de place pour chacun. Nous n'avons pas besoin de guerres.\"")

S("\"Only the stupid hyperbugs do not understand this.\"", 
  "\"Seulement les stupides hyperinsectes ne comprennent pas cela.\"")

S("\"I have once talked to a Yendorian researcher... he was only interested in infinite trees.\"",
  "\"J'ai autrefois parlé à un chercheur Yendorien... il ne s'intéressait qu'aux arbres infinis.\"")

S("\"Infinite trees are boring. I prefer other graphs.\"", 
  "\"Les arbres infinis sont ennuyeux. Je préfère d'autres graphes.\"")

// new start menu
// --------------

S("skip the start menu", "passer le menu de démarrage")

S("HyperRogue classic", "HyperRogue classique")
S("explore the world, collect treasures", "explorez le monde, ramassez des trésors")
S("do not get checkmated", "ne vous laissez pas mettre en échec et mat")
S("use your Orbs in tough situations", "utilisez vos Orbes dans les situations difficiles")
S("continuous spacetime", "espace-temps continu")
S("(most achievements are not available)", "(la plupart des accomplissements ne sont pas disponibles)")
S("learn about hyperbolic geometry!", "apprenez à propos de la géométrie hyperbolique !")
S("more options", "plus d'options")

S(
  "A strange land filled with mirrors. Break magic mirrors and mirage clouds to "
  "gain treasures and helpful Mimics.",
  "Un pays étrange plein de miroirs. Brisez les miroirs magiques et les nuages de "
  "mirages pour gagner des trésors et des Mimiques bien utiles."
  )
  
// were missing

N4("Kraken", GEN_M, "Kraken", "Krakens")
N4("Kraken Tentacle", GEN_F, "Tentacule de Kraken", "Tentacules de Kraken")
S(" (killing increases treasure spawn)", " (tuer augmente l'apparition de trésors)")

N4("stepping stones", GEN_F, "pierre de gué", "pierres de gué")

// 10.1

// mew message options
S("centered", "centré")
S("left-aligned", "aligné à gauche")
S("line-broken", "ligne brisée")
S("message style", "style de message")
S("limit messages shown", "limite du nombre de messages affichés")
S("Maximum number of messages on screen.", "Nombre maximal de messages à l'écran.")

S("message log", "historique de messages")
S("real time", "temps réel")
S("game time", "temps du jeu")
S("precise", "précis")
S("no time", "pas le temps")
S("back", "retour")
S("clear", "effacer")

// 'u' now loads the current picture in the Map Editor
S("'u' to load current", "'u' pour charger l'actuel")

// Happy Hunting Ground
//----------------------

N("Hunting Ground", GEN_M, "Terrain de Chasse", "Terrains de Chasse","dans le Terrain de Chasse")

S("The Happy Hunting Ground is the place where hunting is easy; "
  "the spirits of hunters go here after their death, if their life was deemed worthy. "
  "You did not qualify though, so you will not ever be able to find your way to the places "
  "where the hunter spirits and game are... "
  "and their hunting dogs will hunt you instead!\n\n"
  "You hope to use this to learn some "
  "skills in fighting in open hyperbolic space, though.",
  
  "Dans le Terrain de la Chasse heureuse, chasser est facile : "
  "Les esprits des chasseurs viennent ici après la mort, s'ils ont prouvé leur valeur "
  "de leur vivant. Vous n'en êtes néanmoins pas digne, donc vous ne pourrez jamais trouver "
  "votre chemin vers les endroits où sont les esprits des chasseurs et le jeu..."
  "et leur chien de chasse vous chasseront à la place !\n\n"
  "Vous espérez néanmoins utiliser ceci pour apprendre quelques "
  "compétences de combat en espace hyperbolique ouvert."
  )
  
N4("Hunting Dog", GEN_M, "Chien de Chasse", "Chiens de Chasse")
N4("Hunting Dog (guarding)", GEN_M, "Chien de Chasse (monte la garde)", "Chiens de Chasse (montent la garde)")
N4("Turquoise", GEN_F, "Turquoise", "Turquoise")
Orb("Ferocity", "Férocité")

S("Hunters believe that wearing a Turquoise amulet will improve their accuracy. "
  "This one has been lost, but the hunting dogs are guarding it until the owner returns.",
  
  "Les chasseurs croient que porter une amulette de Turquoise améliore leur précision. "
  "Celle-ci a été perdue, mais les chiens de chasse la gardent jusqu'au retour du propriétaire."
  )

S("Whenever you attack with this Orb, you also hit the monsters adjacent to you and opposite to the monster you originally attacked.",
  "Lorsque vous attaquez avec cette Orbe, vous frappez aussi les monstres adjacents à vous et opposés au monstre attaqué à l'origine."
  )

// Volcanic Wasteland
//--------------------

N("Volcanic Wasteland", GEN_M, "Désert Volcanique", "Déserts Volcaniques","dans les Déserts Volcaniques")
N4("lava", GEN_F, "lave", "laves")

S("The volcanic activity in this land is extremely unstable. "
  "Lava is too hot to walk on. It cools quickly, but another "
  "flow will come soon...",
  
  "L'activité volcanique de ce pays est extrêmement instable. "
  "La lave est trop chaude pour marcher dessus. Cela refroidit "
  "vite mais une autre coulée arrive bientôt..."
  )

N4("Salamander", GEN_F, "Salamandre", "Salamandres")

S("Salamanders are tough lizard-like creatures. Their tough skin protects them "
  "from both physical attacks and heat. Salamanders "
  "are stunned for a longer time if you push them into lava, fire, or a solid obstacle.",
  
  "Les salamandres sont des créatures résistantes semblables à des lézards. Leur "
  "peau dure les protège à la fois des attaques physiques et de la chaleur. Les "
  "salamandres sont assommées plus longtemps si vous les pousser dans la lave, le feu "
  "ou un obstacle solide."
  )

N4("Lava Wolf", GEN_M, "Loup de Lave", "Loups de Lave")

S("While Ice Wolves love heat, their instincts usually will not let them leave the Icy Lands -- "
  "they are afraid that they will be unable to get back home, and that they will lose track of their prey. "
  "However, they find the Volcanic Wasteland so hot and attractive that they abandon their natural instincts... "
  "and try to track their prey using their other senses and intelligence.",
  
  "Alors que les Loup des Glaces aiment la chaleur, leurs instincts ne leur permettent habituellement "
  "pas de quitter le Pays des Glaces -- ils ont peur de ne pas pouvoir retourner à la maison, et de perdre la "
  "trace de leur proie. Cependant, ils trouvent le Désert Volcanique si chaud et attirant qu'ils abandonnent leurs "
  "instincts naturels et essaient de traquer leurs proies en utilisant leurs autres sens et leur intelligence. ")
  
N4("Lava Lily", GEN_M, "Lys de Lave", "Lys de Lave")

S("This plant, able to survive in the extreme conditions of the Volcanic Wasteland, "
  "is a valuable alchemical ingredient.",
  
  "Cette plante, capable de survivre dans les conditions extrèmes du Désert Volcanique, "
  "est un ingrédient alchimique de valeur.")

Orb("Lava", "Lave")
S("This Orb summons a minor volcanic activity around you. "
  "All the heptagonal cells adjacent to enemies in distance at most 5 to you "
  "will be set on fire. Does not affect the cell you are on, and enemies resistant to fire.",
  
  "Cette Orbe invoque une activité volcanique mineure autour de vous. "
  "Toutes les cases heptagonales adjacentes aux ennemis à une distante d'au plus 5 de vous "
  "prendra feu. N'affecte pas la case sur laquelle vous êtes, ni les ennemis résistants au feu."
  )

// Orbs would be destroyed in Volcanic Wasteland
S("would be destroyed in %the1", "serait détruite dans %le1") 
S(" to go cold", " pour se refroidir")
S("%The1 is destroyed by lava!", "%Le1 est détruit par la lave !")
S("%The1 is killed by lava!", "%Le1 est détruit par la lave !")
S("Run away from the lava!", "Fuyez la lave !")

// Terracotta Army
//-----------------

N("Terracotta Army", GEN_F, "Armée de Terre Cuite", "Armées de Terre Cuite","Dans les Armée de Terre Cuite")

S("The Emperor's mausoleum. The army "
  "of Terracotta Warriors is supposed to protect him in his "
  "afterlife, while the traps are to protect the treasures "
  "in the mausoleum from thieves.",

  "Les mausolée de l'Empereur. L'armée de "
  "guerriers de Terre Cuite est censée le protéger après "
  "sa mort, tandis que les pièges sont là pour protéger "
  "les trésors du mausolée des voleurs."
  )

S("Spending 3-5 turns next to the Terracotta Warriors will cause them to awaken. "
  "They have 7 HP, and get stunned for longer and longer time with each attack.",
  
  "Passer 3 à 5 tours près des Guerriers de Terre Cuite les réveillera. "
  "Ils ont 7 HP, et restent assommés de plus en plus longtemps à chaque attaque."
  )

N4("arrow trap", GEN_M, "piège à fléchettes", "pièges à fléchettes")

S("The arrow traps will automatically launch a deadly arrow when stepped on. They are quite old though, so the arrow will "
  "actually fly at the end of the next turn.",
  
  "Les pièges à fléchettes lanceront automatiquement une flèche mortelle lorsqu'on marche dessus. Ils sont assez vieux, donc "
  "la flèche volera à la fin du tour suivant."
  )

N4("mercury river", GEN_F, "rivière de mercure", "rivières de mercure")
S("A river of mercury.", "Une rivière de mercure.")

N4("Terracotta Warrior", GEN_M, "Guerrier de Terre Cuite", "Guerriers de Terre Cuite")

N4("Jiangshi", GEN_M, "Jiangshi", "Jiangshi")
S("You think this was one of the people who have constructed the Terracotta Warriors and the arrow traps. "
  "They have been locked inside, so that they will never divulge the secrets of the mausoleum. They would like to return their homes and families, though.",
  
  "Vous pensez qu'il s'agit d'une des personnes qui ont construits les Guerriers de Terre Cuite et les pièges à fléchettes. "
  "Ils ont été enfermés à l'intérieur, pour qu'ils ne révèlent jamais les secrets du mausolée. Ils aimeraient pourtant retourner à leurs foyers et familles." 
  )

Orb("Slashing", "Entaille")
S("Whenever you attack with this Orb, you also hit the monsters adjacent both to you and the monster you originally attacked.",
  "Lorsque vous attaquez avec cette Orbe, vous frappez aussi les monstres adjacents à la fois à vous et à celui attaqué à l'origine.")

N4("Ancient Weapon", GEN_F, "Arme Ancienne", "Armes Anciennes")
S("This ancient weapon is beautifully encrusted with precious gems, but you prefer your own -- it is much lighter.",
  "Cette arme ancienne est magnifiquement incrustée de gemmes précieuses, mais vous préférez la votre -- elle est bien plus légère.")

// Blizzard
//----------

N("Blizzard", GEN_M, "Blizzard", "Blizzards","Dans le Blizzard")
N4("Ice Golem", GEN_M, "Golem des Glaces", "Golems des Glaces")
N4("Void Beast", GEN_F, "Bêtes du Vide", "Bêtes du Vide")
N4("Forgotten Relic", GEN_F, "Relique Oubliée", "Reliques Oubliées")

S("The Ice Golems are powered by intense magical coldness. When destroyed in the Blizzard or another icy land, they become "
  "ice walls, and freeze the land around them.",
  
  "Les Golems des Glaces sont mus par un intense froid magique. Une fois détruits dans le Blizzard ou un autre pays glacé, "
  "ils deviennent des murs de glace, et gèle la terre autour d'eux.")

S("Are creatures of Void actual monsters, or just monster-shaped holes in the air?\n\nVoid Beasts move simply by letting the air move into their current location -- "
  "the hole will move to the place where the air came from! Void Beasts are great at moving against the wind, but they have problems "
  "moving with the wind.",
  
  "Les créatures du Vide sont-elles de vrais monstres ou juste des trous en forme de monstres dans l'air ?\n\n"
  "Les Bêtes du Vide se déplacent simplement en laissant l'air venir dans leur emplacement actuel -- "
  "le trou ira à l'endroit d'où vient l'air ! Les Bêtes du Vide vont facilement contre le vent, mais ont des problèmes à aller contre "
  "le vent."
  )

S("Once in the past there lived a tribe whose prophets warned about the great blizzard that was to come and freeze the world. "
  "They thought it was the wrath of the gods, so they put some altars, so that the gods could hear their pleas. "
  "The mighty golems guarded those altars, making sure nobody steals the jewels, because a robbed god is a angry god. "
  "Unfortunately winter has come (maybe because only the monks knew how to stop the golems, and they were said to had escaped earlier with some of the jewels). "
  "The tribe has frozen to death, altars got covered with snow and ice. Only ice golems still guard them waiting for the spring.",
  
  "Autrefois vivait ici une tribu dont les prophètes ont prévenu du grand blizzard à venir qui allait geler le monde. "
  "Ils pensaient que c'était la colère des dieux, donc ils ont dressé des autels, pour que les dieux entendent leur suppliques. "
  "De puissants golems gardaient ces autels, s'assurant que personne ne vole les joyeaux , car un dieu volé est un dieu en colère. "
  "Malheureusement l'hiver est venu (peut être seuls les moines savent  arrêter les golems, et il paraît qu'ils se sont enfuis plus tôt avec certains des joyeaux). "
  "La tribu est morte gelé, les autels ont été couverts de neige et de glace. Seuls les golems de glaces les gardent encore en attendant le printemps."
  )

// Winter becomes native in Blizzard, while Change replaces Winter in Cocytus
Orb("Change", "Changement")
S("This ranged Orb will transform the target monster into one without any special powers. It also stuns them for one turn. "
  "Does not affect multi-tile monsters.",
  "Cette orbe à distance transformera le monstre visé en un sans pouvoirs spéciaux. Elle les assomme aussi pendant un tour. "
  "N'affecte pas les monstres multi-cases."
  )

// yet unused orb
Orb("the Triangle", "Triangle")
S("Whenever you attack with this Orb, you also hit the monsters adjacent to you which are neither adjacent nor opposite to the monster "
  "you originally attacked.",
  "Quand vous attaquez avec cette orbe, vous frappez aussi les monstres adjacents à vous qui sont ni adjacents ni opposés au un monstre "
  "attaqué à l'origine.")
                                                                    
S("Using %the1 in %the2 sounds dangerous...", "Utiliser %le1 dans %le2 a l'air dangereux...")
S("%The1 is mostly useless in %the2...", "%Le1 est inutile la plupart du temps dans %le2...")
S("%The1 is forbidden in %the2 (disables some achievements)", "%Le1 est interdit dans le %le2 (désactive certains accomplissements")

S("You are ambushed!", "Vous tombez dans une embuscade !")
S("teleport", "téléporte")
S("ambush:", "piège :")

N4("Hunting Dog (regrouping)", GEN_M, "Chien de Chasse (se regroupant)", "Chiens de Chasse (se regroupant)")
S("When your plan has clearly failed, it is better to abandon it and go to a safe place, to have a chance of succeeding next time. This dog clearly knows this.",
  "Lorsque votre plan a clairement échoué, il vaut mieux l'abandonner et aller à un endroit sûr, pour avoir une chance de gagner la prochaine fois. "
  "Ce chien sait clairement cela.")

S("The Hunting Dogs give up.", "Les chiens de chasse abandonnent.")

// missing from previous versions:

S("\n\nThis orb also allows you to collect items encased in ice.",
  "\n\nCette orbe permet vous aussi de ramasser des objets encastrés dans la glace.")

// vampire attack
S("%The1 drains your powers!", "%Le1 draine vos pouvoirs !")

// additions
S(" This tour will not advance on its own -- you have to press Enter (not while reading help text).", 
  " Cette visite guidée n'avancera pas toute seule -- vous devez appuyer sur Entrée (pas pendant la lecture du texte d'aide)."
  )

S("Hint: hold Alt to highlights enemies and other important features.",
  "Astuce: maintenir Alt appuyé pour surligner les ennemis et d'autres fonctionnalités importantes.")

// Crystal World
N("Crystal World", GEN_M, "Monde de Cristal", "Mondes de Cristal","dans le Monde de Cristal")
N("Glowing Crystal", GEN_M, "Cristal Brillant", "Cristaux Brillants", "dans les Cristaux Brillants")

S("Yes, this is definitely a crystal. A very regular crystalline structure.\n\n"
  "This land has been designed as a tool to play with various geometries, and it does not appear during a normal gameplay.",
  
  "Oui, c'est définitivement un cristal. Une structure cristalline très régulière.\n\n"
  "Ce pays a été conçu comme un outil pour jouer avec des géométries variées, et n'apparaît pas durant le jeu normal.")

S("You cannot move there!", "Vous ne pouvez pas aller ici !")

// geometry stuff

S("experiment with geometry", "expérimenter avec la géométrie")

S(
  "If you want to know how much the gameplay is affected by the "
  "hyperbolic geometry in HyperRogue, this mode is for you!\n\n"
  
  "You can try many different geometries here. We start by gluing "
  "n-gons in such a way that k of them meet in every vertex. "
  "Depending on n and k, this either folds into a sphere, unfolds into a plane, "
  "or requires a hyperbolic space. The result may be then 'bitruncated' by "
  "replacing each vertex by a 2k-gon. Furthermore, you can play "
  "with quotient geometries. For example, the elliptic geometry is "
  "obtained from the sphere by making the antipodes be the same point, "
  "so you return to the same spot (but as a mirror image) after going there. "
  "Have fun experimenting! "
  "Achievements and leaderboards do not work in geometry experiments, "
  "except some specific ones.\n\n"
  "In standard geometry (bitruncated or not), you can play the full game, but in other geometries "
  "you select a particular land. Lands are unlocked by visiting them in this "
  "session, or permanently by collecting 25 treasure. Try Crossroads in Euclidean "
  "or chaos mode in non-standard non-quotient hyperbolic to visit many lands. "
  "Highlights:\n"
  "* Crystal World and Warped Coast can be understood as extra geometries.\n"
  "* Halloween is specially designed for spherical geometry.\n"
  "* To see the difference, try Hunting Grounds in Euclidean -- it is impossible.\n",
  
  "Si vous voulez savoir à quel point la géométrie hyperbolique affecte le gameplay "
  "dans HyperRogue, ce mode est pour vous !\n\n"
  "Vous pouvez essayer des géométries différentes ici. On commence par coller des n-gones "
  "de telle sorte que k d'entre eux se rencontrent à chaque sommet. "
  "Selon les valeurs de n et k, cela soit se plie en une spère, se déplie en un plan, ou "
  "prend un espace hyperbolique. Le résultat peut être 'bitronqué' en remplaçant chaque sommet "
  "par un 2k-gone. De plus vous pouvez jouer aves des géométries quotient. Par exemple, la géométrie "
  "elliptique est obtenue depuis la sphère en faisant des antipodes le même point, donc vous retournez "
  "au même endroit (mais comme image miroir) après être allé%e0 là. Amusez vous à expérimenter ! "
  "Les accomplissements et meilleurs scores ne marchent pas dans les expériences de géométrie, "
  "sauf certaines spécifiques.\n\n"
  "Dans la géométrie standard (bitronquée ou pas), vous pouvez jouer le jeu complet, mais dans d'autres "
  "géométries vous sélectionnez un pays particulier. Les pays sont débloqués en les visitant dans cette "
  "session ou de manière permanente en ramassant 25 trésors. Essayez les Croisements en mode euclidien "
  "ou chaos en hyperbolique non-standard non-quotient pour visiter de nombreux pays. "
  "Recommandés :\n"
  " Le Monde de Cristal et la Côte Tordue peuvent êtres comprises comme des géométries en plus.\n"
  "* Halloween est spécialement conçu pour les géométries sphériques.\n"
  "* Pour voir la différence, essayez les Terrains de Chasse en euclidien -- c'est impossible.\n"  
  )

// new geometries available:
S("standard", "standard") // renamed from 'hyperbolic'

S("octagons", "octogones")
S("four pentagons", "quatre pentagones")
S("four hexagons", "quatre hexagones")
S("four heptagons", "quatre heptagones")
S("cube", "cube")
S("tetrahedron (buggy)", "tétraèdre (bugué)")
S("tetrahedron", "tétraèdre") // in case if it gets better

S("land", "pays")
S("bitruncated", "bitronqué")
S("does not matter", "ne compte pas")
S("sides per face", "côtés par face")
S("faces per vertex", "faces par sommet")
S("none", "aucun")
S("zebra", "zèbre")
S("field", "champ")
S("quotient space", "espace quotient")
S("size of the world", "taille du monde")
S("flat", "plat")

// bitruncated or not bitrunc
S(" (b)", " (b)")
S(" (n)", " (n)")

S("(locked)", "(bloqué)")

S("stereographic/orthogonal", "projection stéréographique/orthographique")
S("Poincaré/Klein", "Poincaré/Klein")
  
// Paper Model Creator
S("Useless in Euclidean geometry.", "Inutile en géométrie euclidienne.")
S("Not implemented for spherical geometry. Please tell me if you really want this.",
  "Pas implémenté pour la géométrie sphérique. Merci de me dire si vous voulez vraiment ceci.")

// === 10.3 ===
// ============

// Missing/minor updated texts
//-----------------------------

// file selection dialog
S("Enter = choose", "Entrée = choisir")
S("Esc = cancel", "Esc = annuler")

S("hyperbolic", "hyperbolique")
S("reset special modes", "réinitialiser en modes spéciaux")
S("orthogonal projection", "projection orthogonale")
S("small Poincaré model", "petit modèle de Poincaré")
S("big Poincaré model", "grand modèle de Poincaré")
S("Klein-Beltrami model", "modèle de Klein-Beltrami")
S("bestiary of %the1", "bestiaire de %le1")

// for the map editor
N4("Dragon Head", GEN_F, "Tête de Dragon", "Têtes de Dragon")

S("score: %1", "score : %1")
S("kills: %1", "tués : %1")

// advanced config of quotient geometry
//--------------------------------------

S(
  "This geometry is obtained by applying the same 'generators' which "
  "lead to creating the given basic hyperbolic geometry, "
  "but using a fixed finite field instead of the field of reals. "
  "It can be also interpreted as a quotient of the given basic geometry. "
  "Warning: field patterns based on large primes might generate for a long time.",
  
  "Cette géométrie est obtenue en appliquant les même 'générateurs' qui, "
  "conduisent à créer les bases données d'une géométrie hyperbolique, "
  "mais en utilisant un corps fini fixé au lieu du corps des réels. "
  "Cela peut aussi être interprété comme quotient de la géométrie de base donnée. "
  "Avertissement : les motifs de corps basés sur de grands nombres premiers peuvent "
  "prendre longtemps à se générer.")

S("find the next prime", "trouver le nombre premier suivant")
S("activate", "activer")
S("order %1%2 (non-bitruncated cells: %3)", "ordonner %1%2 (cases non-bitronquées : %3)")

// Torus/Klein bottle, Euclidean Squares, and their advanced configuration
//-------------------------------------------------------------------------

S("square grid", "grille carrée")

S("torus/Klein bottle", "tore/bouteille de Klein")
S("mode", "mode")

S("single row (hex)", "un seul rang (hex)")
S("single row (squares)", "un seul rang (carrés)")
S("parallelogram (hex)", "parallélogramme (hex)")
S("rectangle (squares)", "rectangle (carrés)")
S("rectangle (hex)", "rectangle (hex)")
S("Klein bottle (squares)", "bouteille de Klein (carrés)")
S("Klein bottle (hex)", "bouteille de Klein (hex)")

S("this mode has bad patterns", "ce mode a de mauvais motifs")
S("incompatible with bitruncating", "incompatible avec la bitroncation")
S("best if %1 is divisible by %2", "mais si %1 est divisible par %2")

S("number of cells (n)", "nombres de cases (n)")
S("cell bottom-right from 0 (d)", "case en bas à droite de 0 (d)")
S("cell below 0 (d)", "case sous 0 (d)")
S("width (x)", "largeur (x)")
S("height (y)", "hauteur (y)")

// upgraded Hypersian Rug mode
//-----------------------------

S("In the perspective projection, you can use arrows to rotate the camera, Page Up/Down to go forward/backward, Shift+arrows to strafe, and Ctrl+arrows to rotate the model.",
  "Dans la projection en perspective, vous pouvez utiliser des flèches pour pivoter la caméra, Page Up/Down pour aller en avant/arrière, Shift+flèches pour mitrailler, et Ctrl+flèches pour pivoter le modèle.")

S("Note: this mode is based on what you see on the screen -- but re-rendered in another way. "
  "If not everything is shown on the screen (e.g., too zoomed in), the results will be incorrect "
  "(though possibly interesting). "
  "Use a different projection to fix this.",
  
  "Note : ce mode est basé sur ce que vous voyez à l'écran -- mais ré-afficher d'une autre manière. "
  "Si tout n'est pas montré à l'écran (par exemple trop zoomé), les résultats seront incorrects "
  "(mais peut-être intéressant). Utiliser une projection pour réparer ceci.")

S("vertex limit", "limite du nombre de sommets")
S("The more vertices, the more accurate the Hypersian Rug model is. "
  "However, a number too high might make the model slow to compute and render.",
  "Plus il y a de sommets, plus le modèle de tapis Hypersan est précis. "
  "Cependant, un nombre trop grand peut rendre le modèle lent à calculer et afficher.")

S("model distance", "distance du modèle")
S("In the perspective projection, this sets the distance from the camera to the center of the model. "
  "In the orthogonal projection this just controls the scale.",
  
  "Dans la projection en perspective, ceci contrôle la distance de la caméra au centre du modèle. "
  "Dans la projection orthogonale ceci contrôle juste l'échelle.")

S("maximum error", "erreur maximale")
S("New points are added when the current error in the model is smaller than this value.",
  "De nouveaux points sont ajoutés lorsque l'erreur actuelle du modèle est plus petite que cette valeur.")

S("field of view", "champ de vue")
S("Horizontal field of view, in the perspective projection. "
  "In the orthogonal projection this just controls the scale.",
  "Champ de vue horizontal, dans la projection en perspective. "
  "Dans la projection orthogonale, ceci contrôle l'échelle.")

S("native geometry", "géométrie native")
S("scale model", "échelle du modèle")
S("This is relevant when the native geometry is not Euclidean. "
  "For example, if the native geometry is spherical, and scale < 1, a 2d sphere will be rendered as a subsphere; "
  "if the native geometry is hyperbolic, and scale > 1, a hyperbolic plane will be rendered as an equidistant surface. ",
  
  "Ceci est pertinent quand la géométrie native n'est pas euclidienne. "
  "Par exemple, si la géométrie native est sphérique, et l'échelle < 1, une 2-sphère sera affichée comme subsphère ; "
  "si la géométrie est hyperbolique, et l'échelle > 1, un plan hyperbolique sera affiché comme surface équidistante.")

S("This just shows the 'z' coordinate of the selected point.",
  "Ceci montre juste la coordonnée 'z' du point sélectionné.")

S("radar", "radar")
S("model iterations", "itérations du modèle")
S("keep shape", "garder la forme")

// texture mode
//--------------

S("texture mode", "mode texture")

S(
  "This mode lets you change the floor tesselation easily -- "
  "select 'paint a new texture' and draw like in a Paint program. "
  "The obtained pattern can then be easily changed to another geometry, "
  "or saved.\n\n"
  "Instead of drawing, it is also possible to use an arbitrary image "
  "as a texture. "
  "Works best with spherical/Euclidean/hyperbolic tesselations "
  "(e.g., a photo of a soccerball, or one of the tesselations by M. C. "
  "Escher), but it can be also used on arbitrary photos to make them periodic "
  "(these probably work best with the 'large picture' setting in geometry selection). "
  "Again, tesselations can have their geometry changed.\n\n",
  
  "Ce mode vous permet de changer facilement le pavage du sol -- "
  "sélectionner 'peindre une nouvelle texture' et dessiner comme dans un programme "
  "comme Paint. Le motif obtenu peut alors être facilement changé dans une autre "
  "géométrie ou sauvegardée.\n\n"
  "Au lieu de dessiner, il est aussi possible d'utiliser une image arbitraire comme "
  "une texture. "
  "Marche mieux avec les pavages sphériques/euclidiens/hyperbolique (par exemple une "
  "photo de balle de foot, ou un des pavages par C. Eschera), mais peut aussi être utilisé "
  "sur des photos arbitraires pour les rendre périodiques (ceci marche probablement mieux "
  "avec le réglage 'grande image' dans la sélection de la géométrie). De même on peut changer "
  "le pavage de ces géométries.\n\n")

S("texture mode (off)", "mode texture (désactivé)")
S("select geometry/pattern", "sélectionner la géométrie/motif")
S("reactivate the texture", "réactiver la texture")
S("open PNG as texture", "ouvrir le PNG comme texture")
S("load texture config", "charger la configuration de la texture")
S("warning: unable to find the center", "avertissement : impossible de trouver le centre")
S("texture size", "taille de la texture")
S("paint a new texture", "peindre une nouvelle texture")
S("precision", "précision")

S("texture mode (overlay)", "mode texture (recouvrement)")
S("select the texture's pattern", "sélectionner le motif de la texture")
S("enable the texture", "activer la texture")
S("cancel the texture", "annuler la texture")
S("move the model", "déplacer le modèle")
S("move the texture", "déplacer la texture")
S("zoom/scale the texture", "zoomer/mettre à l'échelle la texture")
S("zoom/scale the model", "zoomer/mettre à l'échelle le modèle")
S("projection", "projection")
S("affine transformations", "transformations affines")
S("magic", "magique")

S("grid color (master)", "couleur de grille (original)")
S("grid color (copy)", "couleur de grille (copie)")
S("save the raw texture", "sauvegarder la texture brute")
S("Saved the raw texture to %1", "Sauvegarder la texture brute vers %1")

S("texture auto-adjustment", "auto-ajustement de la texture")
S("drag from the model to the texture", "tirer le modèle vers la texture")
S("affect model scale", "affecte l'échelle du modèle")
S("affect model projection", "affecte la projection du modèle")
S("affect model rotation", "affecte la rotation du modèle")
S("affect model position", "affecte la position du modèle")
S("affect texture slanting", "affecte l'inclinaison de la texture")
S("affect texture stretching", "affecte l'étirement de la texture")
S("delete markers", "effacer les marqueurs")
S("perform auto-adjustment", "effectuer l'auto-ajustement")

S("texture mode (active)", "mode texture (actif)")
S("deactivate the texture", "désactiver la texture")
S("back to overlay mode", "retour au mode recouvrement")
S("change the geometry", "changer la géométrie")
S("grid color", "couleur de grille")
S("mesh color", "couleur de maille")
S("color alpha", "couleur alpha")
S("The higher the value, the less important the color of underlying terrain is.",
  "Plus la valeur est grande, moins la couleur du terrain sous-jacent est importante.")
S("edit the texture", "editer la texture")
S("save the full texture image", "sauvegarder l'intégralité de l'image  de la texture")
S("Saving full texture to %1...", "Sauvegarder la texture entière dans %1...")
S("save texture config", "sauvegarder la configuration de la texture")

// pattern groups

S("football", "balle de foot")
S("three colors", "trois couleurs")
S("chessboard", "plateau d'échec")
S("single type", "un seul type")
S("large picture", "image large")
S("periodic patterns", "motifs périodiques")
S("more tuning", "plus d'ajustements")
S("BAD", "MAL")

// extra patterns

S("Docks pattern", "Motifs de docks")
S("broken Emerald pattern", "motif d'émeraude cassé")
S("coloring", "coloration")


// Docks
//-------

N("Docks", GEN_M, "Quai", "Quais", "sur les Quais")
N4("dock", GEN_M, "quai", "quais")
S("A dock.", "Un quai.")
N4("burning dock", GEN_F, "porte en feu", "portes en feu")
S("A burning dock.", "Un quai.")

N("Sea Glass", GEN_F, "Mer de Verre", "Mers de Verre","sur la Mer de Verre")

// Snake Nest

N("Snake Nest", GEN_M, "Nid de Serpents", "Nids de Serpents", "dans le Nid de Serpents")

N4("Snake Oil", GEN_F, "Huile de Serpent", "Huiles de Serpent")

S("Made of actual snakes!", "Fait avec des vrais serpents !")

// Ruined City

N("Ruined City", GEN_F, "Cité en Ruines", "Cités en Ruines","dans la Cité en Ruines")

S("Once a beautiful city... but now overrun by the mighty Raiders of unknown origin.\n\n"
 "Raiders cannot be harmed with mundane weapons, but each color has its movement restrictions.",
 
 "Autrefois une cité magnifique... désormais envahie par de puissants Pilleurs d'origine inconnue.\n\n"
 "Les Pilleurs ne peuvent être blessés par des armes ordinaires, mais chaque couleur a ses propres restrictions de mouvement.")

N4("ruin wall", GEN_M, "mur en ruines", "murs en ruines")

N4("Chrysoberyl", GEN_M, "Chrysobéryl", "Chrysobéryls")
S("Fragment of the past glory.", "des fragments de la gloire passée.")

N4("Red Raider", GEN_M, "Pilleur Rouge", "Pilleurs Rouges")
S("Red Raiders travel in pairs. They have promised to always watch each other's backs. They are able to destroy walls on their way.",
  "Les pilleurs rouges voyagent en binômes. Ils ont juré de toujours protéger les arrières de leur binôme. Ils sont capables de "
  "détruire des murs sur leur passage.")

N4("Gray Raider", GEN_M, "Pilleur Gris", "Pilleurs Gris")
S("Gray Raiders never step on gray cells.",
  "Les pilleurs gris ne marchent jamais sur les cases grises.")

N4("Green Raider", GEN_M, "Pilleur Vert", "Pilleurs Verts")
S("Green Raiders never step from one green cell to another.",
  "Les pilleurs verts ne vont jamais d'une case verte à une autre.")

N4("Brown Raider", GEN_M, "Pilleur Marron", "Pilleurs Marrons")
S("Brown Raiders never move adjacent to an item.",
  "Les pilleurs marrons ne vont jamais à côté d'un objet.")


N4("Blue Raider", GEN_M, "Pilleur Bleu", "Pilleurs Bleus")
S("Blue Raiders have a powerful attack which takes two turns to complete, and also makes the Blue Raider stunned "
  "for a long time. This attack can destroy other Raiders if it hits them.",
  "Les pilleurs bleus ont une attaque puissante qui prend deux tours à compléter, et assomme aussi le pilleur bleu "
  "pour un long moment. Cette attaque peut détruire d'autres pilleurs si elle les atteint."
  )

S("You cannot attack Raiders directly!", "Vous ne pouvez attaquer les Pilleurs directement !")

Orb("Slaying", "Massacre")

S("This Orb lets you defeat Raiders and other tough single-cell monsters in melee.",
  "Cette Orbe vous permet de vaincre les Pilleurs et d'autres monstres uni-case en groupe.")

// Jelly Kingdom

N("Jelly Kingdom", GEN_M, "Royaume de Gelée", "Royaumes de Gelée", "dans les Royaumes de Gelée")

S("Some of the Slime Beasts have decided to revolt against the color rules in the "
  "Alchemist Lab. They have changed their shape and consistency, declared independence, and established their own Kingdom.\n\n"
  "Jellies switch between being a wall and being a monster after every treasure you collect.",
  
  "Certaines des Bêtes Gélatineuses ont décidé de se rebeller contre les règles de couleur "
  "dans le Laboratoire de l'Alchimiste. Ils ont changé leur forme et consistance, déclaré leur indépendance, et établi leur propre royaume.\n\n"
  "Les gelées alternent entre être un mur et un monstre chaque fois que vous collectez un trésor."
  )

N4("Red Jelly", GEN_F, "Gelée Rouge", "Gelées Rouges")
N4("Blue Jelly", GEN_F, "Gelée Bleue", "Gelées Bleues")

N4("Tasty Jelly", GEN_F, "Délicieuse Gelée", "Délicieuses Gelées")
S("A tasty byproduct of the Jelly Revolution.",
  "Un délicieux produit dérivé de la révolution de gelée.")

Orb("Phasing", "Phase")
S("This orb lets you pass through walls (one cell wide), and also through monsters, as long as they will not attack you in transit.",
  "Cette orbe vous permet de passer à travers les murs (une case de largeur), et aussi à travers les monstres, tant qu'ils ne vous "
  "attaquent pas en transit.")

S("You cannot attack Jellies in their wall form!", "Vous ne pouvez attaquer les Gelée sous la forme d'un mur !")

// keyboard help
//---------------

S("advanced keyboard shortcuts", "raccourcis clavier avancés")

S("1 = orthogonal/Gans model","1 = modèle orthogonal/de Gans")
S("2 = small Poincare model/stereographic projection","2 = petit modèle de Poincaré/projection stéréographique")
S("3 = big Poincare model/stereographic projection","3 = grand modèle de Poincaré/projection stéréographique")
S("4 = Klein model/gnomonic projection","4 = modèle de Klein/projection gnomique")
S("5 = change wall display mode","5 = changer le mode d'affichage du mur")
S("6 = change grid","6 = changer la grille")
S("7 = change heptagon marking","7 = changer le marquage des heptagones")
S("8 = change background color","8 = changer la couleur du fond")
S("9 = hyperboloid model","9 = modèle hyperboloïde")
S("qweasdzxc, hjklyubn, numpad = move/skip turn","qweasdzxc, hjklyubn, numpad = bouger/passer tour")
S("arrows = panning","flèches = panorama")
S("o = world overview","o = vue d'ensemble du monde")
S("v = menu","v=menu")
S("F1 = help","F1 = aide")
S("F5 = restart game","F5 = redémarrer le jeu")
S("F10 = quit game","F10 = quitter le jeu")
S("Esc = quest status","Esc = statut de la quête")
S("Alt+Enter = full screen","Alt+Entrée = plein écran")
S("Alt = highlight interesting stuff","Alt = surligne les choses intéressantes")
S("t = use a ranged Orb (target center of the screen)","t = utiliser une orbe à distance (cibler le centre de l'écran)")
S("g = drop a Dead Orb","g = lâcher une orbe morte")
S("click left mouse button = move/skip","cliquer gauche = bouger/passer")
S("shift+click left mouse button = use ranged Orb","shift+clic gauche = utiliser une Orbe à distance")
S("click right mouse button = context help","cliquer droit = aide contextuelle")
S("mousewheel up = panning","scroller vers le haut = panorama")
S("hold middle mouse button = panning","maintenir appuyé le bouton scroll = panorama")
S("mousewheel down = move/skip","scroller vers le bas = bouger/passer")
S("shift + mousewheel = change projection","shift + scroller = changer la projection")
S("ctrl + mousewheel = change zoom","ctrl + scroller = zoomer")
S("ctrl + shift + mousewheel = change both projection and zoom","ctrl + shift + scroller = changer à la fois la projection et le zoom")
S("ctrl + hold middle button = move the screen","ctrl + tenir bouton scroll = déplacer l'écran")
S("shift + middle button = reset position","shift + bouton scroll = réinitialiser la position")

S("pics to save:", "images à sauvegarder :")
S("level to save:", "niveau à sauvegarder :")
S("pics to load:", "images à charger :")
S("level to load:", "niveau à charger :")

// new option
S("forget faraway cells", "oublier les cases lointaines")
N("Lost Memory", GEN_F, "Mémoire Perdue", "Mémoires Perdues","dans la Mémoire Perdue")
S(  "Because of the properties of hyperbolic geometry, it is extremely unlikely to randomly "
    "get back to a faraway place you have been to. However, you have managed to get there "
    "somehow. In the meantime, its memory has been cleared, since the 'forget faraway cells'"
    " option was on.",
    
    "A cause des propriétés de la géométrie hyperbolique, il est très improbable de revenir "
    "par hasard dans un endroit lointain où vous avez été. Cependant, vous avez réussi à venir "
    "ici. Entre-temps, la mémoire a été effacée, car l'option 'oublier les cases lointaines' "
    "était activée." )

// shmup 
N4("Blueball", GEN_F, "Balle Bleue", "Balles Bleues")
S("A powerful missile from a Blue Raider.", "Un puissant missile d'un Pillard Bleu.")

// missing
S(" fps: ", " fps : ")
S("default value", "valeur par défaut")
S("You can scroll with arrow keys -- Ctrl to fine-tune", "Vous pouvez scroller avec les touches flèches -- Ctrl pour régler finement")
S("font scale", "échelle de la police")
S("background particle effects", "effet de particules du fond")

// not yet translated for Hypersian Rug mode
S("orthogonal", "orthogonale")
S("perspective", "perspective")
S("model scale factor", "facteur d'échelle du modèle")

// stereo vision config
S("stereo vision config", "configurer la vision stéréographique")

S("This allows you to view the world of HyperRogue in three dimensions. "
  "Best used with the Hypersian Rug mode. When used in the disk model, "
  "this lets you look at the Minkowski hyperboloid (which means the "
  "depth of terrain features is actually reversed). It also works with non-disk models, "
  "from the conformal menu.",
  "Ceci vous permet de voir le monde d'HyperRogue en trois dimensions. "
  "Mieux vaut l'utiliser en mode tapis Hypersan. Utilisé dans le modèle "
  "disque, ceci permet de voir l'hyperboloïde de Minkowski (ce qui signifie "
  "que la profondeur des caractéristiques du terrain est en réalité inversée). Cela marche "
  "aussi avec les modèle non-disque, depuis le menu conforme."
  )

S("Currently, red-cyan anaglyph glasses and mobile VR googles are supported.",
  "Actuellement, les lunettes rouge-cyan anaglyphe et les lunettes de réalité virtuelle portable sont supportées.")

S("stereo mode", "mode stéréo")
S("anaglyph", "anaglyphe")
S("side-by-side", "côte à côte")

S("pupillary distance", "distance des pupilles")
S("The distance between your eyes in the represented 3D object. This is given in absolute units.",
  "La distance entre vos yeux dans l'objet 3D représenté. Ceci est donné en unités absolues.")

S("distance between images", "distance entre les images")
S("The distance between your eyes. 1 is the width of the screen.",
  "La distance entre vos yeux. 1 est la largeur de l'écran.")

// field of view
S("Horizontal field of view, in angles. "
  "This affects the Hypersian Rug mode (even when stereo is OFF) "
  "and non-disk models.",
  "Champ de vue horizontal, en angle. "
  "Ceci affecte le mode Tapis Hypersan (même quand la stéréo est désactivée) "
  "et les modèles non-disque.")

// more magic configuration
S("affect model central point", "affecte le mode point central")
S("affect texture position X", "affecte la position de la texture X")
S("affect texture position Y", "affecte la position de la texture Y")

S("select master cells", "sélectionner les cases d'origine")
S("select master triangles", "sélectionner les triangles d'origine")
S("fine tune vertices", "réglage fin des sommets")

// main menu
// an option to keep the current settings
S("use current/saved settings", "utiliser les réglages actuels/sauvegardés")
// randomly chosen mode: hypersian rug
S("see the true form", "voir la vrai forme")
// randomly chosen mode: heptagonal mode
S("more curvature", "plus de courbure")

// anti-crossing in the Hypersian Rug
S("anti-crossing", "anti-croisement")
S("The anti-crossing algorithm prevents the model from crossing itself, "
  "by preventing points which should not be close from being close. "
  "The bigger number, the more sensitive it is, but the embedding is slower. Set 0 to disable.",
  
  "L'algorithme anti-croisement empêche le modèle de se croiser lui-même, "
  "en prévenant des points qui ne devraient pas être proches de se rapprocher. "
  "Plus le nombre est grand, plus il est sensible, mais l'affichage est plus lent. Régler à 0 "
  "pour désactiver."
  )

// new 3D models

// various menu names

S("constant curvature surfaces", "surfaces de courbure constante")
S("smooth surfaces", "surfaces lisses")
S("models of hyperbolic geometry", "modèle de géométrie hyperbolique")
S("history mode", "mode histoire") // old 'conformal/history mode' has been split into two
S("other 3D models", "autres modèles 3D")

// various models

S("Minkowski hyperboloid", "hyperboloïde de Minkowski")
S("plane", "plan")
S("sphere", "sphère")
S("hemisphere", "hémisphère")
S("tractricoid", "tracticoïde")
S("concave barrel", "baril concave")
S("Dini's surface", "surface de Dini")
S("Kuen surface", "surface de Kuen")
S("inversion", "inversion")

S(
  "In this menu you can choose from several known smooth surfaces of constant negative curvature. "
  "Since the curvature of hyperbolic plane is also constant negative, these surfaces "
  "are great to draw hyperbolic tesselations on. While they look great, only a small part "
  "of the hyperbolic plane can be visibly represented in this way, so they are not "
  "good for playing HyperRogue; however, the coverage extends far away in specific directions, "
  "so first increasing sight range in graphics config and texture size in hypersian rug settings may improve the effect. "
  "For convenience, you can also choose other 3D models from this menu.",
  
  "Dans ce menu vous pouvez choisir plusieurs surfaces lisses de courbure négative constante. "
  "Comme la courbure d'un plan hyperbolique est aussi une constante négative, ces surfaces "
  "sont parfaites pour dessiner des pavages hyperboliques dessus. Même si ils sont beaux seule "
  "une petite partie du plan hyperbolique peut être représenté de cette manière, donc ils ne sont "
  "pas pratiques pour jouer à HyperRogue ; cependant la représentation s'étend loin dans des directions, "
  "spécifiées, donc augmenter d'abord la distance de vue dans la configuration graphique et taille de "
  "texture dans les réglages du tapis hypersan peut augmenter l'effet. Vous pouvez aussi choisir d'autres "
  "modèles 3D depuis ce menu."
  )

// coverage
S("display coverage", "afficher le recouvrement")
S("on surface", "sur la surface")
S("on H² (static)", "na H² (statique)")
S("on H² (dynamic)", "na H² (dynamique)")

S("Computing these models involves integrals and differential equations, which are currently solved numerically. This controls the precision.",
  "Calculer ces modèles implique des équations intégrales et différentielles, qui sont actuellement résolues numériquement. Ceci contrôle la précision.")

// too bored for declension of %1 (either a surface name or a a part of a surface)
S("solving the geodesics on: %1, %2/%3", "résoudre les géodésiques sur : %1, %2/%3")

S("the upper component", "la composante supérieure")
S("the lower center", "le centre inférieur")
S("the lower left", "la gauche inférieure")
S("the lower right", "la droite inférieure")

S("camera rotation in 3D models", "rotation de la caméra dans les modèles 3D")

S("Rotate the camera in 3D models (ball model, hyperboloid, and hemisphere). "
  "Note that hyperboloid and hemisphere models are also available in the "
  "Hypersian Rug surfaces menu, but they are rendered differently there -- "
  "by making a flat picture first, then mapping it to a surface. "
  "This makes the output better in some ways, but 3D effects are lost. "
  "Hypersian Rug model also allows more camera freedom.",
  
  "Pivoter la caméra dans les modèles 3D (modèle balle, hyperboloïde, et hémisphère). "
  "Notez que les modèles hyperboloïdes et hémisphères sont aussi disponibles dans le "
  "menu surfaces de tapis Hypersan, mais sont restitués différemment ici -- "
  "en faisant une photo plate d'abord, puis en l'appliquant à une surface. "
  "Ceci améliore le résultat sous certains aspects, mais les effets 3D sont "
  "perdus. Le modèle tapis Hypersan donne plus de liberté à la caméra.")

// parameter for the given model
S("parameter", "paramètre")
S( "Stereographic projection to a sphere. Choose the radius of the sphere.",
  "Projection stéréographique de la sphère. Choisir le rayon de la sphère.")
// parameter for Dini
S("The larger the number, the more twisted it is.",
  "Plus les nombres sont larges, plus c'est tordu.")
// parameter for concave barrel
S("Controls the inner radius.", "Contrôle le rayon interne.")

// improved rotation
S(
  "This controls the automatic rotation of the world. "
  "It affects the line animation in the history mode, and "
  "lands which have a special direction. Note that if finding this special direction is a part of the puzzle, "
  "it works only in the cheat mode.",
  
  "Ceci contrôle la rotation automatique du monde. "
  "Cela affecte l'animation des lignes en mode histoire, et "
  "les pays qui un direction privilégiée. Notez que si trouvez cette direction spéciale fait partie du puzzle, "
  "cela ne fonctionne qu'en mode triche."
  )
S("NEVER", "JAMAIS")

S("Mercator", "Mercator")
S("band equidistant", "ruban équidistante")
S("band equi-area", "ruban équi-aire")
S("sinusoidal", "sinusoïde")
S("two-point equidistant", "projection équidistante à deux points")

S(
  "This model maps the world so that the distances from two points "
  "are kept. This parameter gives the distance from the two points to "
  "the center.",
  
  "Ce modèle affiche le modèle de telle sorte que les distances entre deux "
  "points soient conservées. Ce paramètre donne la distance des deux points "
  "aux centre.")

S("enable the Camelot cheat", "activer la triche de Camelot")
S("disable the Camelot cheat", "désactiver la triche de Camelot")
S("gain Orb of the Sword", "acquisition de l'Orbe de l'Épée")
S("Static mode enabled.", "Mode statique activé.")
S("Static mode disabled.", "Mode statique désactivé.")

S("Returned to your game.", "Retourné à votre jeu.")
S("Spherical version of %the1. ", "Version sphérique de %le1. ")
S("Euclidean version of %the1. ", "Version euclidienne de %le1. ")
S("Click again to go back to your game.", "Cliquer à nouveau pour retourner à votre jeu.")
S("Press %1 again to go back to your game.", "Appuyez sur %1 à nouveau pour retourner à votre jeu.")
S("return to your game", "retourner à votre jeu")

S("orthogonal projection", "projection orthogonale")
S("stereographic projection", "projection stéréographique")
S("zoomed stereographic projection", "projection stéréographique zoomée")
S("zoomed out", "dézoomé")
S("zoomed in", "zoomé")
S("gnomonic projection", "projection gnomonique")
S("Goldberg", "Goldberg")
S("fisheye", "oeil de poisson")
S("paint pictures", "photos peintes")
S("list of slides", "liste des diapositives")
S("back to the start menu", "retour au menu de démarrage")
S("You juggle the Dead Orbs.", "Vous jonglez avec des Orbes Mortes.")
S("You give %the1 a grim look.", "Vous donnez au %1 un air sinistre.")

S("Strange Challenge", "Défi étrange")
S("compete with other players on random lands in random geometries", "concourir avec d'autres joueurs dans des pays aléatoires dans des géométries aléatoires.")
S("Strange Challenge #%1", "Défi étrange #%1")
S("next challenge in %1", "défi suivant dans : %1")
S("synchronizing scores...", "synchroniser les scores...")
S("already played", "déjà joué")
S("play!", "jouez !")
S("See past challenges", "Voir les défis passés")
S("Past Strange Challenges. You can see the setting and high scores, but you can no longer play them.",
  "Défis étranges passés. Vous pouvez voir les réglages et hauts scores, mais vous ne pouvez plus y jouer.")

// land compatibility guides

S("Cannot kill Hedgehog Warriors in this geometry.", "Impossible de tuer des Guerriers-Hérissons dans cette géométrie.")
S("No random pattern version.", "Pas de version motif aléatoire.")
S("Great Walls not implemented.", "Les grands murs ne sont pas implémentés.")
S("Pattern incompatible.", "Motif incompatible.")
S("Pattern not implemented -- using random.", "Motif non implémenté -- utiliser aléatoire.")
S("Pattern not implemented.", "Motif non implémenté.")
S("Not enough space.", "Pas assez d'espace.")
S("Does not work in this geometry.", "Ne marche pas dans cette géométrie.")
S("This land is designed for bounded worlds.", "Ce pays est conçu pour les mondes à bords.")
S("This land is designed for infinite worlds.", "Ce pays est conçu pour les modes infinis.")
S("This land is designed for infinite worlds or big spheres.", "Ce pays est conçu pour les mondes infinis ou les grandes sphères.")
S("Out of theme for the full game.", "Hors-thème pour le jeu complet.")
S("No game here.", "Pas de jeu ici.")
S("Does not work in chaos mode.", "Ne marche pas en mode chaos.")
S("Not in the full game.", "Pas dans le jeu complet.")
S("Special construction in the Chaos mode.", "Construction spéciale en mode Chaos.")
S("Special construction in the Euclidean mode.", "Construction spéciale dans le mode euclidien.")
S("Special construction in this geometry.", "Construction spéciale dans cette géométrie.")
S("Not implemented.", "Non implementé.")
S("Partially implemented.", "Implémenté partiellement.")
S("No comments.", "Sans commentaire.")
S("Does not work in pure tactics mode.", "Ne marche pas en mode tactique pur.")
S("Technical.", "Technique.")
S("Full game.", "Jeu complet.")
S("Somewhat inaccurate.", "Quelque peu imprécis.")
S("Mercury rivers not implemented (or could not work) in this geometry.", "Les rivières de mercures ne sont pas implémentée (ou ne pourrait pas marcher) dans cette géométrie.")
S("Patterns compatible.", "Motifs compatibles.")
S("This land is specially designed for this geometry.", "Ce pays est spécialement conçu pour cette géométrie.")
S("Three-colorability required.", "trois-colorabilité requise.")
S("Land not implemented.", "Pays non implémenté.")
S("Special interest.", "Interêt spécial.")
S("Better version exists.", "Une meilleure version existe.")
S("Grid does not work in this geometry.", "La grille ne marche pas dans cette géométrie.")
S("Graphics not implemented in this geometry.", "Les graphiques ne sont pas implémentés dans cette géométrie.")
S("This land does not work in the current settings. Reason not available.",
  "Ce pays ne marche pas dans les réglages actuels. La raison n'est pas disponible.")
S("This land does not work well in the current settings. Reason not available.",
  "Ce pays ne marche pas bien dans les réglages actuels. La raison n'est pas disponible.")
S("This combination is known to be buggy at the moment.", "Cette combinaison est connue pour être buguée en ce moment.")
S("Somewhat sloppy pattern.", "Un motif quelque peu négligé.")
S("Fractal landscapes not implemented in this geometry.", "Les paysages fractaux ne sont pas implémentés dans cette géométrie.")
S("Only simplified walls implemented.", "Seuls les murs simplifiés sont implémentés.")

S("cube/elliptic", "cubique/elliptique")
S("chamfered", "biseauté")
S("2x chamfered", "2x biseauté")
S("select", "sélectionner")

S("Goldberg polyhedra are obtained by adding extra hexagons to a dodecahedron. "
  "GP(x,y) means that, to get to a nearest non-hex from any non-hex, you should move x "
  "cells in any direction, turn right 60 degrees, and move y cells. "
  "HyperRogue generalizes this to any tesselation with 3 faces per vertex. "
  "By default HyperRogue uses bitruncation, which corresponds to GP(1,1).",
  
  "Les polyèdres de Goldberg sont obtenus en ajoutant des hexagones en plus à un dodécaèdre. "
  "GP(x,y) signifie que, pour aller du plus proche non-hex depuis n'importe quel non-hex, vous devez "
  "déplacer x cases dans n'importe quelle direction, tourner à droite de 60 degrés et déplacer y cases. "
  "HyperRogue généralise ceci à  n'importe quel pavage à 3 faces par sommet. Par défaut HyperRogue "
  "utilise la bitroncation, ce qui correspond à GP(1,1).")
  
S("extend the ends", "étend les fins")

S("Forbidden cells are marked with a different color.", "Les cases interdites sont marquées avec une couleur différente.")
S("This orb lets you attack adjacent cells %1 steps from the primary target.", "Cette orbe vous permet d'attaquer les cases adjacentes à %1 pas de la cible initiale.")

S("Note: the rules above correspond to the standard geometry; actual rules in other geometries may be different. ",
  "Notes : les règles ci-dessus correspondent à la géométrie standard ; les règles réelles dans d'autres géométries peuvent être différentes. ")

S("Note: the rules refer to colors which are not visible in other lands. ", "Notes : les règles se réfèrent aux couleurs qui ne sont pas visibles dans d'autres pays. ")

S("When the 'mark heptagons' option (hotkey '7') is on, forbidden moves are marked.", 
  "Quand l'option 'marquer les heptagones' (touche '7') est active les déplacements interdits sont marqués.")

S("When the 'mark heptagons' option (hotkey '7') is on, moves between unmarked cells are forbidden.",
  "Quand l'option 'marquer les heptagones' (touche '7') est active les déplacements entre les cases non marquées sont interdits.")

// 10.4p

// in the sight range dialog
S("enable the cheat mode for additional options", "activer les mode triche pour plus d'options")

// steal a baby tortoise with Orb of Space
S("No, that would be heartless!", "No, cela serait sans cœur !")

S("show quotient spaces", "montrer les espaces quotient")
S("minimal quotient", "quotient minimal")
S("Klein Quartic", "Quartique de Klein")
S("Bolza Surface", "Surface de Bolza")
S("Bolza Surface x2", "Surface de Bolza x2")

// extra texts for Goldberg 

S("variations", "variations")
S("auto-adjust in Goldberg grids", "auto-ajustement dans la grille de Goldberg")
S("2x bitruncated", "2x bitronqué")
S("expanded", "étendu")
S("rectified", "rectifié")

// irregular grids

S("irregular", "irrégulier")
S("bad cells: %1", "mauvaises cases : %1")
S("too many edges: %1", "trop d’arêtes : %1")
S("median edge: %1 minimum: %2", "arête médiane: %1 minimum: %2")
S("cells badly paired: %1", "cases mal appariées : %1")
S("cells not covered: %1", "cases non couvertes : %1")
S("adjacent cells from nonadjacent heptagons: %1", "cases adjacentes aux les heptagones non-adjacents : %1")
S("bad format or bad map geometry", "mauvais format ou mauvaise géométrie de carte")
S("irregular grid", "grille irrégulière")
S("density", "densité")
S("min edge to median", "arête minimale à médiane")
S("quality", "qualité")
S("save", "sauvegarder")
S("load", "charger")
S("irregular to save:", "carte irrégulière à sauvegarder :")
S("irregular to load:", "carte irrégulière à charger :")
S("Map saved to %1", "Carte sauvegarder dans %1")
S("Failed to save map to %1", "Échec de la sauvegarde de la carte dans %1")
S("Map loaded from %1", "Carte chargée depuis %1")
S("Failed to load map from %1", "Échec du chargement de la carte depuis %1")
S("reset", "réinitialiser")
S("OK", "OK")
S("ready", "près")

S("This option creates irregular grids to play the game on. "
  "Currently rather slow algorithms are used, "
  "so not recommended with too high density or "
  "with too large periodic base geometry. "
  "For technical reasons, the density cannot be too small.",

  "Cette option créé des grilles irrégulières pour jouer dessus. "
  "Actuellement des algorithmes plutôt lents sont utilisés, donc "
  "ce n'est pas recommandé avec une trop grande densité ou une "
  "géométrie périodique à base trop large. Pour des raisons techniques, "
  "la densité ne peut être trop petite."
  )

// vertical stretch

S("vertical stretch", "étirement vertical")
S("Vertical stretch factor.", "Facteur d'étirement vertical.")
S("Gall-Peters", "Gall-Peters")
S("The current value makes the map conformal at the latitude of %1 (%2°).", "La valeur actuelle affiche la carte conforme à la latitude de %1 (%2°).")
S("The current value makes the map conformal %1 units from the main line.", "La valeur actuelle affiche la carte conforme à %1 unités de la ligne principale.")

// line quality

S("line quality", "qualité de la ligne")

S("Higher numbers make the curved lines smoother, but reduce the performance.",
  "De plus grands nombres rendent les lignes courbes plus lisses, mais réduisent la performance.")

// new options in mobile

S("FIRE", "FEU")

S("scrolling by device rotation", "dérouler en pivotant l'appareil")

S(  "This lets you scroll the map by rotating your device. It can be e.g. used to "
    "play the spherical mode of HyperRogue in mobile VR goggles -- the \"spherical VR\" "
    "button configures this; this VR mode can be disabled by touching the screen for 1 second.",

  "Ceci vous permet de dérouler la carte en pivotant votre appareil. Il peut être par "
  "exemple utilisé pour jouer en mode sphérique d'HyperRogue dans les lunettes mobiles de "
  "réalité virtuelle -- le bouton \"sphérique VR\" configure ceci ; ce mode VR peut être "
  "désactivé en touchant l'écran pendant 1 seconde.")

S("sensitivity", "sensibilité")
S("1 means that rotating the device by 1 radian corresponds to scrolling by 1 unit. In spherical geometry, 1 unit = 1 radian.", 
  "1 signifie que pivoter l'appareil d'un radian signifie dérouler d'une unité. En géométrie sphérique 1 unité = 1 radian.")

S("spherical VR", "VR sphérique")



S("aura from texture", "aura de la texture")

// extra for irregular
S("bitruncation count", "compte de la bitroncation")

S("Bitruncation introduces some regularity, allowing more sophisticated floor tilings and textures.", 
  "La bitroncation introduit de la régularité, permettant un pavage du sol et des textures plus sophistiquées."
  )

S(
      "The smallest allowed ratio of edge length to median edge length. "
      "Tilings with low values are easier to generate, but tend to be more ugly.", 
      "Le plus petit ratio permis de longueur d'arête à longueur d'arête médiane. "
      "Les pavages à bas ratio sont plus faciles à générer mais plus moches."
      )

// missing

S("The Orb of Love gives no bonus score in the Strange Challenge.", "L'Orbe de l'Amour ne donne pas de bonus de score dans le Défi étrange.")
S("This does not work in non-orientable geometries", "Ceci ne marche pas dans les géométries non-orientables")
S("This pattern needs x-y divisible by 3", "Ce motif nécessite que x-y soit divisible par 3")
S("This pattern needs x-y divisible by 2", "Ce motif nécessite que x-y soit divisible par 2")

// missing from the texture editor
S("p = color", "p = couleur")
S("b = brush size: %1", "b = taille de brosse : %1")
S("d = draw", "d = dessiner")
S("l = line", "l = ligne")
S("c = circle", "c = cercle")

// new binary tiling

S("binary tiling", "pavage binaire")

// Archimedean tilings

S("Archimedean tilings", "pavage d'Archimède")
S("Archimedean", "Archimède")

S("This lets you generate tilings where each cell is a regular polygon, and each vertex is identical. "
  "You can choose from a big list of known Archimedean tilings, or create your own. "
  "The format of specifying Archimedean tilings is based on one used by the Hyperbolic Applet by Don Hatch.\n\n"
  "You can enhance the gameplay/land generation by adding special codes after faces: "
  "H ('heptagon' for Ruined City, Graveyard, etc.), L ('line' for Zebra, Power, Vineyard, etc.).\n"
  "Crystal World and Warped Coast work on (k,l,m) tilings by removing/enlarging the first tile listed ('k').",

  "Ceci vous permet de générer des pavages où chaque cases est un polygone régulier, et chaque sommet est identique. "
  "Vous pouvez choisir dans une grande liste de pavages d'Archimède, ou créer le votre. "
  "Le format d'un pavage d'Archimède spécifié est basé sur l'un de l'Applet Hyperbolique de Don Hatch.\n\n"
  "Vous pouvez modifier le gameplay/génération de pays en ajoutant des codes spéciaux après les faces : "
  "H ('heptagone' pour les Cités en Ruines, Cimetière, etc.), L ('ligne' pour le Zèbre, Pays du Pouvoir, Vignobles, etc.).\n"
  "Le Monde de Cristal et la Côte Tordue marchent sur les pavages (k,l,m) en retirant/agrandissant le premier pavé listé ('k').")

S("color by symmetries", "couleurs par symétries")
S("color by sides", "couleurs par côtés")

S("edit", "éditer")
S("full angle", "angle total")
S("not enough faces", "pas assez de faces")
S("not enough edges", "pas assez d'arêtes")
S("polygons match incorrectly", "les polygones ne correspondent pas")
S("currently no more than %1 edges", "actuellement moins que %1 arêtes")
S("currently no more than %1 faces in vertex", "actuellement moins que %1 faces sur un sommet")

N("Canvas", GEN_M, "Canevas", "Canevas", "dans le Canevas")
S("find the player", "trouver le joueur")
S("press SPACE to find the player", "appuyer sur ESPACE pour trouver le joueur")

S("edge length", "longueur d'arête")
S(" (arbitrary)", " (arbitraire)") // arbitrary edge length
S("previous page", "page précédente")
S("Defined pattern.", "Motif défini.")

// graphics editor
S(" (r = complex tesselations)", " (r = pavage complexe)")
S("area: %1", "aire : %1")

// short codes for variations
S(" (d)", " (d)") // dual
S(" (g)", " (g)") // Goldberg
S(" (i)", " (i)") // irregular

// pattern types
S("cell types", "type de cases")
S("siblings", "adelphes") // a pattern on the sphere
S("rotate the color groups", "pivoter les groupes de couleurs")
S("full symmetry", "symétrie complète")
S("alternate coloring", "coloration alternative")
S("display full floors", "afficher les couleurs complètes")
S("remove complete classification", "retirer la classification entière")

// animations
S("animations/history", "animations/histoire")
S("animations", "animations")
S("period", "période")
S("game animation period", "période d'animation du jeu")
S("no movement animation", "pas de mouvement d'animation")
S("translation", "traduction")
S("parabolic", "parabolique")
S("circle", "cercle")
S("animate parameter change", "changer le paramètre d'animation")
S("choose parameters to animate", "choisir les paramètres d'animation")
S("frames to record", "cadres à enregistrer")
S("record to a file", "enregistrer vers un fichier")

// improved worldsize demo dialog
S("which distance", "de quelle distance")
S("start", "démarrer")
S("land", "pays")

S("number codes", "codes numériques")
S("distance", "distance")
S("type", "type")
S("debug", "débug")

S("color codes", "codes couleur")
S("display distances from", "afficher les distances depuis")
S("disable", "désactiver")
S("move the player", "déplacer le joueur")
S("show number of cells by distance", "montrer le nombre de cases par distance")
S("show number of descendants by distance", "montrer le nombre de descendants par distance")

S("triangle grid: tree edges", "grille triangulaire : trois arrêtes")
S("triangle grid: other edges", "grille triangulaire : autres arrêtes")
S("dual grid", "grille duale")
S("horocycles", "horocycle")
S("configure", "configuration")
S("scroll", "dérouler")

// improved sight range dialog
S("minimum visible cell in pixels", "minimum de cases visibles en pixels")
S("draw range based on distance", "portée de tracé basé sur la distance")
S("draw based on size in the projection (no generation)",
  "tracé basé sur la taille dans la projection (pas de génération)")
S("draw based on size in the projection (generation)",
  "tracé basé sur la taille dans la projection (génération)")
S("cells drawn", "cases tracées")
S("limit on cells drawn", "limites sur les cellules tracées")
S("This limit exists to protect the engine from freezing when too many cells would be drawn according to the current options.",
  "Cette limite existe pour protéger l'engin de geler quand trop de case seraient tracées en raison des options actuelles.")
S("generation range bonus", "bonus de portée de la génération")
S("game range bonus", "bonus de portée du jeu")

// new options for models

S("Joukowsky transform", "transformation de Joukowsky")
S("Joukowsky+inversion", "Joukowsky + inversion")

S("maximum z coordinate to show", "coordonnées maximale z à montrer")

S("model transition", "transition de modèle")
S("You can change this parameter for a transition from another model to this one.",
  "Vous pouvez changer ce paramètre pour une transition depuis un autre modèle à celui là.")

S("model orientation", "orientation du modèle")

// 10.4z

S("left-handed", "gaucher")

// formula features

S(
  "This lets you specify the projection as a formula f. "
  "The formula has access to the value 'z', which is a complex number corresponding to the (x,y) coordinates in the currently selected model; "
  "the point z is mapped to f(z). You can also use the underlying coordinates ux, uy, uz.",
  
  "Ceci vous laisse spécifier la projection comme une formule f. "
  "La formule a accès à la valeur 'z', qui est un nombre complexe correspondant à des coordonnées (x,y) dans le modèle actuellement sélectionné ; "
  "Ce point z est mis en correspondance avec f(z). Vous pouvez aussi utiliser les coordonnées ux, uy, uz.")

S("Functions available: %1", "Fonctions disponibles : %1")

S("animate parameters", "paramètres animés")

S(
  "Most parameters can be animated simply by using '..' in their editing dialog. "
  "For example, the value of a parameter set to 0..1 will grow linearly from 0 to 1. "
  "You can also use functions (e.g. cos(0..2*pi)) and refer to other parameters.",
  
  "La plupart des paramètres peuvent être animés simplement en utilisant '..' dans le dialogue d'édition. "
  "Par exemple, la valeur d'un paramètre réglé à 0..1 croîtra linéairement de 0 à 1. "
  "Vous pouvez aussi utiliser les fonctions (par exemple cos(0..2*pi)) et vous référez à d'autres paramètres.")

// color edit dialog

S("background", "fond")
S("foreground", "premier plan")
S("projection boundary", "limite de projection")
S("projection background", "fond de la projection")
S("projection period", "période de la projection")
S("dialogs", "dialogues")
S("canvas colors", "couleurs du canevas")
S("minefield colors", "couleurs du champ de mines")
S("distance colors", "couleurs à distance")

// extra conformal models

S("rotated hyperboles", "hyperbole pivotée")
S("spiral", "spirale")

S("spiral angle", "angle de lą spirale")
S("spiral period: x", "période de la spirale : x")
S("spiral period: y", "période de la spirale : y")
S("match the period of the torus", "correspondance de la période du tore")

// extra patterns

S("circles", "cercles")
S("radii", "rayons")
S("meridians", "méridiens")
S("parallels", "parallèles")
S("seven-coloring", "sept-coloration")

S("half-plane scale", "échelle semi-planaire")
S("use GPU to compute projections", "utiliser le GPU pour calculer des projections")
S("N/A", "N/A")
S(" (2D only)", " (seulement 2D)")

// 10.5d

// this line was missing...
S("geometry experiments", "expérimentez avec la géométrie")

S("cylinder (squares)", "cylindre (carrés)")
S("cylinder (hex)", "cylindre (hex)")
S("Möbius band (squares)", "ruban de Möbius (carrés)")
S("Möbius band (hex)", "ruban de Möbius (hex)")

S("Macbeath Surface", "Surface de Macbeath")
S("Bring's Surface", "Surface de Bring")
S("Schmutz's M(3)", "M(3) de Schmutz")
S("Schmutz's M(4)", "M(4) de Schmutz")

S("dimensional crystal", "cristal dimensionnel")

S(
    "This geometry essentially lets you play in a d-dimensional grid. Pick three "
    "dimensions and '3D display' to see how it works -- we are essentially playing on a periodic surface in "
    "three dimensions, made of hexagons; each hexagon connects to six other hexagons, in each of the 6 "
    "possible directions. Normally, the game visualizes this from the point of view of a creature living inside "
    "the surface (regularized and smoothened somewhat), assuming that light rays are also restricted to the surface -- "
    "this will look exactly like the {2d,4} tiling, except that the light rays may thus "
    "sometimes make a loop, causing you to see images of yourself in some directions (in other words, "
    "the d-dimensional grid is a quotient of the hyperbolic plane tiling). The same construction works in other dimensions. "
    "Half dimensions are interpreted in the following way: the extra dimension only has two 'levels', for example 2.5D "
    "has a top plane and a bottom plane.\n\n"
    "You may also bitruncate this geometry -- which makes it work better with the rules of HyperRogue, but a bit harder to understand.",
    
    "Cette géométrie vous permet essentiellement de jouer dans une grille à d-dimensions. Choisir trois dimensions, "
    "faites d'hexagones ; chaque hexagone est relié à six autres hexagones, dans chacune des 6 directions possibles."
    "Normalement le jeu visualise ceci du point de vue d'une créature vivant à l'intérieur de la surface (régularisée "
    "et lissée), en supposant que les rayons de lumière sont aussi restreints à la surface -- ceci ressemble exactement "
    "au pavage {2,4} à part pour les rayons lumineux peuvent ainsi parfois faire une boucle, vous permettant de voir des "
    "images de vous-même dans d'autres directions (autrement dit, la grille d-dimensionnelle est un espace quotient du "
    "pavage hyperbolique). La même construction fonctionne dans d'autres dimensions. Les demi-dimensions peuvent être "
    "interprétées de la manière suivante : la dimension supplémentaires n'a que deux 'niveaux', par exemple 2.5D a un "
    "plan supérieur et un plan inférieur.\n\n"
    "Vous pouvez aussi bitronquer cette géométrie -- ce qui la fait mieux marcher avec les règles d'HyperRogue, mais un "
    "peu plus dur à comprendre.")

S("4D double bitruncated", "4 doublement bitronqué")
S("view coordinates in the cheat mode", "voir les coordonnées en mode triche")
S("compass probability", "probabilités de la boussole")
S("3D display", "affichage 3D")

S("\"Each piece of the Round Table is exactly %1 steps away from the Holy Grail.\"",
  "\"Chaque partie de la Table Ronde est à exactement %1 cases du Saint Graal.\"")

S("\"According to Merlin, the Round Table is a perfect Euclidean sphere in %1 dimensions.\"",
  "\"D'après Merlin, la Table Ronde est une sphère euclidienne parfaite en %1 dimensions.")

S(
  "Lands in this geometry are usually built on North-South or West-East axis. "
  "Compasses always point North, and all the cardinal directions to the right from compass North are East (this is not "
  "true in general, but it is true for the cells where compasses are generated). "
  "North is the first coordinate, while East is the sum of other coordinates.",
  
  "Les Pays dans cette géométrie sont habituellement construits le long d'un "
  "axe Nord-Sud ou Ouest-Est. Les boussoles pointent toujours au Nord, et toutes les directions cardinales à droite de la "
  "boussole sont l'Est (ce n'est pas vrai en général mais vrai pour les cases où les boussoles sont générées). "
  "Le Nord est la première coordonnée, tandis que l'Est est la somme des autres coordonnées.")

S("crystal coordinate colors", "couleurs des coordonnées du cristal")

S("3D monsters/walls on the surface", "monstres/murs 3D sur la surface")
S("no 3D effects available in this projection", "pas d'effets 3D disponibles dans cette projection")

// new screenshot menu

S("screenshots", "captures d'écran")
S("format", "format")
S("pixels (X)", "pixels (X)")
S("pixels (Y)", "pixels (Y)")
S("supersampling", "super-échantillonage")
S("transparent", "transparent")
S("gamma", "gamma")
S("higher value = darker", "plus grande valeur = plus sombre")
S("brightness", "luminosité")
S("higher value = lighter", "plus grande valeur = plus clair")
S("show the HUD", "montrer HUD")
S("SVG screenshots do not work in this 3D mode", "les captures d'écran SVG ne marchent pas dans ce mode 3D")
S("SVG screenshots do not work with textures", "les captures d'écran SVG ne marchent pas avec les textures")
S("take screenshot", "faire une capture d'écran")
S("screenshot", "capture d'écran")
S("shot settings", "montrer les réglages")

// new warnings

S("WARNING", "AVERTISSEMENT")
S("YES", "OUI")
S("NO", "NON")

S("Are you sure you want to hit %the1?", "Êtes vous sûr%e0 de vouloir frapper %le1 ?")
S("This will exit HyperRogue without saving your current game. Are you sure?",
  "Ceci vous fera quitter HyperRogue sans sauvegarder votre jeu actuel. Êtes vous sûr%e0 ?")
S("This will end your current game and start a new one. Are you sure?",
  "Ceci vous fera quitter votre jeu actuel et démarrer un nouveau. Êtes vous sûr%e0 ?")
S("This will enable the cheat mode, making this game ineligible for scoring. Are you sure?",
  "Ceci vous fera activer le mode triche, rendant le jeu inéligible aux hauts scores. Êtes vous sûr%e0 ?")

// 11.0

S("debug values", "valeurs de débogage")
S("internal details", "détails internes")
S("show debug data", "montrer les données de débogage")
S("click a cell to edit it", "cliquer sur une case pour l'éditer")
S("click a cell to view its data", "cliquer sur une case pour voir ses données")

S("(these lands are not in game)", "(ces pays ne sont pas dans le jeu)")

// new lands

N("Brown Island", GEN_F, "Île Marron", "Îles Marrons","dans les Îles Marrons")

S("The Brown Islands have been generated by ancient underground creatures, who moved randomly and raised lands in their path... "
  "adults spawned larvae in each move, which also moved randomly and also raised lands in their path, and eventually became adults and spawned their own larvae. "
  "The creatures spawned exponentially, but the space they lived in still grew faster, so there was enough space for the whole spawn...\n\n"
  "That was a long time ago. Now, the mountainous Brown Islands are inhabited by strange monsters. Good luck adventuring!",
  
  "Les Îles Marrons ont été générées par d'anciennes créatures souterraines, qui bougeaient aléatoirement et ont élevé des "
  "terres sur leur passage... les adultes pondaient des larves à chaque mouvement, qui bougeaient aussi aléatoirement et créaient "
  "des terres sur leur passage, et devenaient adultes avant de pondre leur propres larves. Les créatures apparaissaient exponentiellement, "
  "mais l'espace où elles vivaient croissait toujours plus vite, donc il y avait assez d'espace pour la portée entière...\n\n"
  "C'était il y a très longtemps. Maintenant, les îles marrons sont habitées par d'étranges monstres. Bonne chance dans vos aventures !")
  

N4("Bronze Beast", GEN_F, "Bête de Bronze", "Bêtes de Bronze")

S("A large bug native to the Brown Islands. Cannot be killed easily due to their tough armor; still, they can be killed by pushing them into water or from great heights. "
"Bronze Beasts are very bad at descending slopes -- it takes them extra time to get down, and they are stunned for a longer time if you push them down.",

  "Un grand insecte originaire des îles marrons. Ne peut être tué facilement à cause de leur dure armure ; ils peuvent toujours être tués en les poussant dans l'eau ou de très haut. "
  "Les bêtes de bronze sont très mauvaises pour descendre des pentes -- cela leur prend plus de temps de descendre, et elles sont assommées plus longtemps si vous les poussez vers le bas.")

N4("Acid Gull", GEN_F, "Mouette Acide", "Mouettes Acides")

S("Where did this strange bird come from?...\n\nAcid Gulls dissolve the land on which they fall when they die. ",

  "D'où vient cet étrange oiseau ?...\n\nLes Mouettes acides dissolvent le sol sur lequel elles tombent à leur mort. ")

N4("Tiger's Eye", GEN_N, "Œil de Tigre", "Yeux de Tigre")
S("A brown gem.", "Une gemme marron.")

Orb("Choice", "Choix")

S("Did you know that it is possible to break a magical sphere into finitely many parts, and then, out of the parts obtained, to make two magical spheres, just like the original? "
  "Hyperbolic geometry makes this even easier!\n\n"
  "When you pick up another Orb, it will not disappear from the ground -- Orb of Choice will be drained instead.",
  
  "Saviez vous qu'il était possible de briser une sphère magique en de nombreuses parties finies, et alors, avec les parties obtenues de faire deux sphères magique, juste comme l'original ? "
  "La géométrie hyperbolique rend cela encore plus facile !\n\n"
  "Quand vous choisissez une autre Orbe, elle ne disparaît pas du sol -- l'Orbe du Choix est drainée à la place.")
    
N("Free Fall", GEN_F, "Chute Libre", "Chutes Libres", "dans les Chutes Libres")

S("What on one side looks to be a normal (well, infinite) horizontal wall, on the other side turns out to be the vertical wall"
  " of an infinitely high tower. Jump from the window, and let the magical gravity carry you...",

  "Ce qui d'un côté semble être un mur horizontal normal (enfin, infini), se révèle être un  mur vertical d'une tour infiniment "
  "haute de l'autre côté. Sautez d'une fenêtre et laissez la gravité magique vous porter...")

N4("Falling Dog", GEN_M, "Chien Tombant", "Chiens Tombants")

S("Distant relatives of the Running Dogs.", "Parents lointains des Chiens Courants.")

N4("Western Hawk", GEN_M, "Faucon de l'Ouest", "Faucon de l'Ouest")

N4("Meteorite", GEN_F, "Météorite", "Météorites")

S("These rocks falling from the sky have been captured to fall forever in the artificial gravity. Meteorite iron is believed to be a valuable material for magical weapons.",
  "Ces rochers tombant du ciel ont été capturés dans une chute éternelle dans la gravité artificielle. Certains croient que le fer de météorite est un matériau de valeur pour des armes magiques.")

Orb("Gravity", "Gravité")

S("This Orb lets you magically control gravity around you. In lands with unusual gravity, the options are: usual gravity (no change, except that it may disrupt birds), "
  "anti-gravity (causes the direction of gravity to be reversed), levitation (you can move in directions which are neither up nor down, "
  "or between cells if one of them has a wall in a 'neutral' direction). In lands with standard gravity, levitation lets creatures to avoid traps, chasms, "
  "and water, while anti-gravity makes it possible to move only when next to a wall (movement between cells without adjacent walls is not allowed). "
  "For each move, the gravity is based on what you do, and all enemies in the effective range are affected (for example, if you move next to a wall in a non-gravity land, "
  "anti-gravity kicks in and monsters will not be able to move in open space).",
  
  "Cette Orbe vous permet de contrôler magiquement la gravité autour de vous. Dans les pays à gravité inhabituelle les options sont : gravité habituelle (pas de "
  "changement, mais peut déranger les oiseaux), l'anti-gravité (inverse la direction de la gravité), lévitation (vous pouvez aller dans des directions qui ne sont "  
  "ni le haut ni le bas, ou entre les cases si l'une des deux a un mur dans une direction 'neutre'). Dans les pays à gravité standard, la lévitation permet aux créatures "
  "d'éviter les pièges, gouffres, eau, tandis que l'anti-gravité ne permet de se déplacer que le long d'un mur (les déplacements entre cases sans murs adjacents né "
  "sont pas permis). Pour chaque déplacement, la gravité est basée sur ce que vous faites, et tout les ennemis dans le rayon d'action (par exemple, si vous vous "
  "déplacez près d'un mur dans un pays sans-gravité, l'anti-gravité entre en action et les monstres en terrain découvert ne peuvent bouger).")

N("Irradiated Field", GEN_M, "Champ Radioactif", "Champs Radioactifs","dans les Champs Radioactifs")

S("These fields are ravaged with many kinds of magical radiation, which not only make the ground glow nicely in various colors, "
  "but also cause the inhabitants to protect the treasures of their land in various ways. In some areas of the Irradiated Fields, "
  "you will meet powerful enemies; in other areas, you will find arrow traps, fire traps, or pools, which can kill you if you are "
  "careless, or help you if you know how to use them to your advantage. Will you walk through the Irradiated Fields randomly, or "
  "try to find areas where treasures are common but nasty monsters are not, and keep to them? It is your choice!\n\n",
  
  "Ces champs sont ravagés avec de nombreuses sortes de radiations magiques, qui non seulement font briller le sol de couleurs variées, "
  "mais cela conduit aussi que les habitants à protéger les trésors de leur pays de différentes manières. Dans certaines régions des Champs "
  "Radioactifs, vous rencontrerez des ennemis puissants ; dans d'autres vous trouverez des pièges à fléchettes, pièges de feu, ou bassins, "
  "qui peuvent vous tuer si vous êtes maladroit%e0, ou vous aider si vous savez les utiliser à votre avantage. Marcherez vous à travers les "
  "Champs Radioactifs aléatoirement, ou essayer de trouver des zones où les trésors sont communs mais pas les vilains monstres, et les garder "
  "à leur place ? À vous de choisir !")

N4("Mutant", GEN_M, "Mutant", "Mutants")

S("These guys look a bit strange, but they have no special properties.",
  "Ces gens ont l'air un peu bizarres, mais ils n'ont pas de particularités spéciales.")
  
N4("fire trap", GEN_F, "pièges de feu", "pièges de feu")

S("This trap will explode when stepped on, setting all the adjacent cells on fire. However, this happens on the next turn, "
  "so you can safely escape if you are fast enough.",
  "Ce piège explosera quand on marchera dessus, ce qui mettra le feu à toutes les cases adjacentes. Cependant, ceci arrivera "
  "au tour suivant donc vous pouvez vous échapper en sécurité si vous êtes assez rapides.")

N4("Explosive Barrel", GEN_M, "Tonneau d'explosifs", "Tonneaux d'explosifs")
S("These barrels can be pushed, and they will explode if next to a fire, or under some attacks.",
  "Ces tonneaux peuvent être poussés, et ils exploseront près d'un feu ou s'ils sont attaqués.")

Orb("Intensity", "Intensité")

S("When you have this, initial and maximal charge amounts of all Orbs are increased by 20%.",
  "Quand vous avez ceci, les charges initiales et maximales de toutes les Orbes sont augmentées de 20%.")

// new mode: Racing

S("Racing available only in unbounded worlds.", "La course n'est disponible que pour les mondes sans bords.")
S("select the track and start!", "sélectionner la piste et démarrez !")
S("configure the projection", "configurer la projection")
S("racing projections", "projection de course")
S("Poincaré disk model", "modèle du disque de Poincaré")
S("third-person perspective", "vue à la troisième personne")
S("point of view", "point de vue")
S("track", "piste")
S("race angle", "angle de course")
S("show more in front", "montrer plus à l'avant")
S("guiding line", "ligne de guidage")
S("track seed", "graine de la piste")
S("play the official seed", "jouer avec la graine officielle")
S("play a random seed", "jouer avec une graine aléatoire")
S("best scores to show as ghosts", "meilleurs scores à montrer en tant que fantômes")
S("best scores to save", "meilleurs scores à sauvegarder")
S("disable the racing mode", "désactiver le mode course")
S("racing menu", "menu de la course")
S("Race did not generate correctly for some reason -- not ranked",
  "La course ne s'est pas générée correctement -- non prise en compte dans le classement")

// 11.0b

// updated descriptions:

S(
    "The Brown Islands have been generated by ancient underground creatures, who moved randomly and raised lands in their path... "
    "adults spawned larvae in each move, which also moved randomly and also raised lands in their path, and eventually became adults and spawned their own larvae. "
    "The creatures spawned exponentially, but the space they lived in still grew faster, so there was enough space for the whole spawn...\n\n"
    "That was a long time ago. Now, the mountainous Brown Islands are inhabited by strange monsters. Good luck adventuring!\n\n"
    "Parts of the Brown Islands have different elevations, from 0 to 3. It is impossible to gain two or more levels, "
    "or to lose three levels, in a single move, (attacks are possible at any "
    "difference, though). Killed Bronze Beasts rise the land by one level.",
    
    "Les Îles Marrons ont été générées par d'anciennes créatures souterraines, qui bougeaient aléatoirement et ont élevé des "
    "terres sur leur passage... les adultes pondaient des larves à chaque mouvement, qui bougeaient aussi aléatoirement et créaient "
    "des terres sur leur passage, et devenaient adultes avant de pondre leur propres larves. Les créatures apparaissaient exponentiellement, "
    "mais l'espace où elles vivaient croissait toujours plus vite, donc il y avait assez d'espace pour la portée entière...\n\n"
    "C'était il y a très longtemps. Maintenant, les îles marrons sont habitées par d'étranges monstres. Bonne chance dans vos aventures !\n\n"
    "Ces parties des Îles Marrons ont des hauteurs différentes de 0 à 3. Il est impossible de gagner deux ou plus niveaux, "
    "ou de perdre trois niveaux, en un seul mouvement, (les attaques sont cependant possibles quelle que soit la différence "
    "de hauteur). Les Bêtes de Bronze tuées élèvent le terrain d'un niveau")

S(
    "Some readers misinterpreted the early maps of Free Fall, thinking that it is a land to the west from some wall. "
    "The name Western Hawks remained.",
    
    "Certains lecteurs ont mal compris les premières cartes des Chutes Libres, pensant qu'il s'agissait d'un pays à l'ouest d'un mur. "
    "Le nom Faucons de l'Ouest est resté.")

N4("Torbernite", GEN_F, "Torbernite", "Torbernites")

S("Crystals emitting magical radiation.", "Des cristaux émettant des radiations magiques.")

// other things:

S("pure", "pure") // non-bitruncated

S(" (level %1)", " (niveau %1)")

S("fixed facing", "face réparée")
S("configure TPP automatically", "configurer TPP automatiquement")

S("Finished the race! Time: %1, place: %2 out of %3", "Arrivé%e0 à la fin de la course ! Temps : %1, place : %2 sur %3")
S("Finished the race in time %1!", "Arrivé%e0 à la fin de la course en un temps %1!")

S("Welcome to HyperRogue! (cheat mode on)", "Bienvenue dans HyperRogue! (mode triche activé)")
S("Welcome to the Heptagonal Mode!", "Bienvenue dans le mode Heptagonal !")

S("adult Tortoise flags", "drapeaux de Tortues adultes")
S("baby Tortoise flags", "drapeaux de bébé Tortues")

S("Map settings", "réglages de carte")

S("disable wandering monsters","désactiver les monstres vagabonds")
S("disable ghost timer", "désactiver le compteur fantôme")
S("simple pattern generation", "génération de motif simple")
S("(e.g. pure Reptile pattern)", "(par exemple motif du Reptile)")
S("safety generation", "génération sûre")
S("(no treasure, no dangers)", "(pas de trésors, pas de dangers)")
S("god mode", "mode divin")

S("(unlock all, allow cheats, normal character display, cannot be turned off!)",
  "(débloque tout, autorise la triche, affichage de caractère normal, ne peut être éteint !)")

S("change the pattern/color of new Canvas cells", "changer le motif/la couleur de nouvelles cases du Canevas")

S("racing mode", "mode course")
S("Racing", "Course")

S("octahedron", "octaèdre")

S("\"Did you know that the Cultists are relatives of the Desert Men?\"", 
  "\"Saviez vous que les Cultistes sont apparentés aux Hommes du Désert ?\"")                      

S("do not use special centering for racing", "ne pas centrer spécialement pour la course")

S("F8 = settings", "F8 = réglages")

// standarization of geometry names

S("{7,3} (standard HyperRogue map)", "{7,3} (carte HyperRogue standard)")
S("{6,3} (Euclidean grid)", "{6,3} (grille euclidienne)")
S("{5,3} (dodecahedron)", "{5,3} (dodécaèdre)")
S("elliptic geometry in {5,3}", "géométrie elliptique en {5,3}")
S("torus/Klein bottle/...", "tore/bouteille de Klein/...")
S("{8,3} (like standard but with octagons)", "{8,3} (comme standard mais avec des octogones)")
S("{5,4} (four pentagons)", "{5,4} (quatre pentagones)")
S("{6,4} (four hexagons)", "{6,4} (quatre hexagones")
S("{7,4} (four heptagons)", "{7,4} (quatre heptagones)")
S("{4,3} (cube)", "{4,3} (cube)")
S("{3,3} (tetrahedron)", "{3,3} (tétraèdre)")
S("{4,4} (Euclidean square grid)", "{4,4} (grille euclidienne standard)")
S("elliptic geometry in {4,3}", "géométrie elliptique {4,3}")
S("{3,4} (octahedron)", "{3,4} (octaèdre)")

// new 3D geometries
                     
S("variant of the binary tiling", "variante du pavage binaire")
S("{4,3,4} cube tiling", "{4,3,4} pavage cubique")
S("rhombic dodecahedral honeycomb", "nid d'abeilles rhombique dodécaèdral")
S("bitruncated cubic honeycomb", "nid d'abeilles cubique bitronqué")

S("{4,4} on horospheres", "{4,4} sur les horosphères")
S("{3,6} on horospheres", "{3,6} sur les horosphères")
S("{6,3} on horospheres", "{6,3} sur les horosphères")
S("rectangles on horospheres", "rectangles sur les horosphères")

// quotient space abbreviations
S("minimal", "minimal")
S("Klein", "Klein")
S("Bolza", "Bolza")
S("Bolza2", "Bolza2")
S("Macbeath", "Macbeath")
S("Bring", "Bring")
S("M3", "M3")
S("M4", "M4")
S("Crystal", "Cristal")

#define Cell(x) \
  S(x "-cell", x "-case") \
  S(x "-cell (elliptic space)", x "-case (espace elliptique)")

Cell("{3,3,3} 5") Cell("{4,3,3} 8") Cell("{3,3,4} 16") Cell("{3,4,3} 24") Cell("{5,3,3} 120") Cell("{3,3,5} 600")
#undef Cell

#define Honeycomb(x) \
  S(x " hyperbolic honeycomb", x " nid d'abeilles hyperbolique") \
  S(x " field quotient space", x " champ d'espace quotient")

Honeycomb("{5,3,4}") Honeycomb("{4,3,5}") Honeycomb("{3,3,6}") Honeycomb("{3,4,4}") Honeycomb("{5,3,5}") Honeycomb("{5,3,6}") Honeycomb("{4,3,6}") Honeycomb("{5,3,6}")

#undef Honeycomb

// new or previously untranslated options
S("dimension", "dimension")
S("pattern", "motif")
S("Euler characteristics", "caractéristique d'Euler")
S("demigenus", "genre non-orientable")
S("number of mines", "nombre de mines") // for Minefield in bounded spaces
S("advanced parameters", "paramètres avancés")

// models & projections

S("disk/Gans", "modèle disque/de Gans")
S("spiral/ring", "spirale/anneau")
S("native perspective", "perspective d'origine")
S("azimuthal equi-volume", "azimutal équi-volume")
S("ball model/Gans", "modèle balle/de Gans")
S("perspective in 4D", "perspective en 4D")
S("half-space", "demi-espace")

// projection configuration

S("projection distance", "distance de projection")
S("rotation (x/z plane)", "rotation du plan x/z")
S("model orientation (y/z plane)", "orientation du modèle (plan y/z)")

S("near clipping plane", "près du plan découpé")
S("far clipping plane", "loin du plan découpé")
S(
  "Your view of the 3D model is naturally bounded from four directions by your window. "
  "Here, you can also set up similar bounds in the Z direction. Radius of the ball/band "
  "models, and the distance from the center to the plane in the halfspace model, are 1.\n\n",
  
  "Votre vue du modèle 3D est naturellement limitée dans quatre direction par votre fenêtre. "
  "Ici, vous pouvez aussi régler des limites similaires dans la direction Z. Le rayon du modèle "
  "balle/ruban, et la distance depuis le centre du plan dans le modèle demi-espace, est 1.\n\n")

S("Objects with Z coordinate "
  "bigger than this parameter are not shown. This is useful with the models which "
  "extend infinitely in the Z direction, or if you want things close to your character "
  "to be not obscured by things closer to the camera.",
  
  "Les objets avec des coordonnées Z plus grandes que ce paramètre ne sont pas montrées. "
  "Ceci est utile avec les modèles qui s'étendent infiniment dans la direction Z, ou si "
  "vous voulez que les parties proches de votre caractère ne soient pas cachées par des "
  "parties encore plus proches de la caméra.")

S("Objects with Z coordinate "
  "smaller than this parameter are not shown; it also affects the fog effect"
  " (near clipping plane = 0% fog, far clipping plane = 100% fog).",
  
  "Les objets avec des coordonnées Z plus petites que ce paramètre ne sont "
  "pas montrés ; cela affecte aussi l'effet brouillard (près du plan découpé "
  "= 0% brouillard, loin du plan découpé = 100% brouillard).")

S("set to 90° for the ring projection", "régler à 90° pour la projection en anneau")

S("spiral multiplier", "multiplieur de la spirale")
S(
  "This parameter has a bit different scale depending on the settings:\n"
  "(1) in spherical geometry (with spiral angle=90°, 1 produces a stereographic projection)\n"
  "(2) in hyperbolic geometry, with spiral angle being +90° or -90°\n"
  "(3) in hyperbolic geometry, with other spiral angles (1 makes the bands fit exactly)",
  
  "Ce paramètre a une échelle un peu différente selon les réglages :\n"
  "(1) dans la géométrie sphérique (avec l'angle de la spirale=90°, 1 donne la projection stéréographique)\n"
  "(2) dans la géométrie hyperbolique, avec l'angle de la spirale +90° ou -90°\n"
  "(3) dans la géométrie hyperbolique, avec d'autres angles de la spirale (1 ajuste les rubans exactement)\n"
  )

// line animation

S("animate from start to current player position", "animer depuis le début vers la position actuelle du joueur")
S("animate from last recenter to current view", "animer depuis le dernier recentrage vers la vue actuelle")

// 3D settings for 3D mode

S("3D sight range", "portée de vue en 3D")
S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "La portée de vue pour les géométries 3D est spécifiée en unités absolues. Cette valeur affecte aussi l'effet brouillard.\n\n"
  "Dans les géométries sphériques, la portée de vue de 2π vous permettra de voir des choses derrières vous comme s'ils étaient "
  "en face de vous, et la portée de vue de π (ou plus) vous permettra de voir des choses à un point antipodal comme si elles "
  "étaient juste à côté de vous.\n\n"
  "Dans les géométries hyperboliques, le nombre des cases à afficher dépend de manière exponentielle de la portée de vue. Plus "
  "cases à tracer réduisent la performance.\n\n"
  "La portée de vue affecte le gameplay, et les monstres agissent comme s'ils étaient visibles. La génération de monstres "
  "prend cela en compte.")

S("limit generation", "limite de génération")
S("Cells over this distance will not be generated, but they will be drawn if they are already generated and in the sight range.",
  "Les cases à cette distance ne seront pas générées, mais tracées comme si elles étaient déjà générées et à portée de vue.")

S("inverted Poincaré model", "modèle de Poincaré inversé")
S("mouse aiming sensitivity", "sensibilité du ciblage de la souris")
S("set to 0 to disable", "régler à 0 pour désactiver")
S("radar size", "taille de radar")
S("Height to width", "Hauteur à largeur")
S("Creature scale", "échelle de la créature")

// not only in 3D mode
S("desaturate colors", "désaturer les couleurs")
S("Make the game colors less saturated. This is useful in the anaglyph mode.",
  "Rend les couleurs du jeu moins saturées. Ceci est utile en mode anaglyphe.")

S("brightness behind the sphere", "brillance derrière la sphère")
S("In the orthogonal projection, objects on the other side of the sphere are drawn darker.",
  "Dans la projection orthogonale, les objets de l'autre côté de la sphère sont affichés plus sombres.")

// restructured menus

S("colors & aura", "couleur & aura")
S("colors of some game objects can be edited by clicking them.", "Les couleurs de certains objets du jeu peuvent être éditées en les cliquant.")
S("standard grid color", "grille de couleurs standard")
S("mouse & touchscreen", "souris & écran tactile")
S("settings", "réglages")
S("interface", "interface")
S("general graphics", "graphiques généraux")
S("quick options", "options rapides")
S("models & projections", "modèle & projections")
S("keyboard & joysticks", "clavier & manettes")
S("other settings", "autres réglages")
S("special modes", "modes spéciaux")
S("creative mode", "mode créatif")
S("multiplayer", "multiplayer")

// extra / previously untranslated options

S("smooth scrolling", "déroulement lisse")
S("change slideshow", "changer l'affichage de diapositives")
S("move by clicking on compass", "se déplacer en cliquant sur la boussole")
S("disable the HUD", "désactiver l'interface utilisateur (HUD)")
S("hide the player", "cacher le joueur")

// redefinable key in 3D shmup mode
S("scroll forward", "dérouler en avant")
S("scroll backward", "dérouler en arrière")

S("note: configured input is designed for", "note : l'entrée configurée est conçue pour ")
S("multiplayer and shmup mode; some features", "modes multijoueur et shoot'em up : quelques fonctionnalités")
S("work worse if you use it.", "fonctionne moins bien si vous l'utilisez.")

// quick options in 3D
S("first person perspective", "point de vue à la première personne")
S("fixed point of view", "point de vue réparé")
S("third person perspective", "point de vue à la troisième personne")

S("Hint: these keys usually work during the game", "Astuce : ces touches marchent habituellement pendant le jeu")

// 3D graphics editor

S("l = color group: %1", "l = groupe de couleur : %1")
S("z = z-level", "z = niveau Z")
S("Changing the z-level will make this layer affected by the parallax effect.", "Changer le niveau de Z fait que cette couche peut être affectée par l'effet de parallaxe")
S("The distance from the camera to added points.", "Distance de la caméra aux points ajoutés.")

S("This land has been disabled with compilation flags.", "Ce pays a été désactivé avec les flags de compilation.")
S("This land does not make much sense in 3D.", "Ce pays ne marche pas en 3D.")
S("This land does not make much sense in binary tiling.", "Ce pays ne marche pas avec un pavage binaire.")
S("Special pattern implemented for this geometry.", "Motif spécial implémenté pour cette géométrie.")
S("This land works only in the shmup mode.", "Ce pays marche uniquement en mode shoot'em up.")

// new land

N("Space Rocks", GEN_M, "Rochers de l'Espace", "Rochers de l'Espace", "dans les Rochers de l'Espace")
N4("Space Rock", GEN_M, "Rocher de l'Espace", "Rochers de l'Espace")
N4("Fuel", GEN_M, "Carburant", "Carburants")
S("Shoot the Space Rocks for score. Large Rocks will split into two smaller rocks.",
  "Tirez les Rochers de l'Espace pour gagner des points. Les grands Rochers se séparent en deux plus petits.")

N4("editable statue", GEN_F, "edytowalny pomnik", "edytowalne pomniki")
S("These statues are designed to have their graphics edited in the Vector Graphics Editor. Each number has its own, separate graphics.",
  "Ces statues sont conçues pour avoir leurs graphiques édités dans l'Éditeur Graphique Vectoriel. Chaque nombre a ses propres graphiques séparés.")
S("The Orb of Yendor is locked in with powerful magic.", "L'Orbe de Yendor est bloquée par une puissante magie.")

S("general perspective", "perspective générale")
S("formula", "formule")

S("limit generated cells per frame", "limite de cases générées par cadre")

S("In the 3D mode, lowering this value may help if the game lags while exploring new areas.",
  "Dans le mode 3D, abaisser cette valeur peut aider si le jeu lague lors de l'exploration de nouvelles zones.")
  
S("sloppy range checking", "vérification de la portée flottante")

S("fog effect", "effet brouillard")
S("memory configuration", "configuration de la mémoire")

S("wall quality", "qualité de mur")

S(
  "Controls the number of triangles used for wall surfaces. "
  "Higher numbers reduce the performance. "
  "This has a strong effect when the walls are curved indeed "
  "(floors in 2D geometries, honeycombs based on horospheres, and projections other than native perspective), "
  "but otherwise, usually it can be set to 1 without significant adverse effects other "
  "than slightly incorrect texturing.",
  
  "Contrôle le nombre de triangles utilisés pour la surface des murs. "
  "De plus grands nombres réduisent la performance. "
  "Ceci a un fort effet quand les murs sont effectivement courbés "
  "(sols en géométries 2D, nids d'abeilles basés sur des horosphères, et les projections autres qu'en perspective native), "
  "mais autrement, cela peut être réglé à 1 sans effets négatifs importants autres qu'une texture légèrement incorrecte.")

S("use the full 3D models", "utiliser les modèles 3D complets")

S("Z shift", "translation Z")
S("fixed Y/Z rotation", "rotation Y/Z réparées")

S("configure FPP automatically", "configurer FPP automatiquement")

S("Look from behind.", "Vue de derrière.")

S("reduce if walls on the way", "réduite s'il y a des murs sur le chemin")

S("The camera is currently exactly on the plane P. "
  "The horizon is seen as a straight line.",
  
  "La caméra est actuellement exactement sur un plan P. "
  "L'horizon est vu comme une ligne droite.")

S(
  "The camera is currently %1 units above the plane P. "
  "This makes you see the floor level as in general perspective projection "
  "with parameter %2.", 
  
  "La caméra est actuellement à %1 unités au-dessus du plan P. "
  "Ceci vous fait voir le niveau du sol comme en projection en "
  "perspective générale avec le paramètre %2.")

S("In the FPP mode, the camera will be set at this altitude (before applying shifts).", 
  "Dans le mode point de vue à la première personney (FPP) la caméra sera réglée à cette "
  "altitude (avant d'appliquer les translations).")

S("auto-adjust to eyes on the player model", "auto-ajuster aux yeux sur le modèle de joueur")

S("eye color", "couleur des yeux")

// rotation (line animation etc.)

S("XY plane", "plan XY")
S("XZ plane", "plan XZ")
S("XY plane #2", "plan XY #2")

// cube honeycomb

S("twisting implemented only for diagonal matrices", "torsion implémentée seulement pour les matrices diagonales")
S("nothing to twist", "rien à tordre")
S("change the bottom left corner", "changer le coin en bas à gauche")

S("flip X coordinate", "retourner la coordonnée X")
S("flipping X impossible", "impossible de retourner X")
S("flip Y coordinate", "retourner la coordonnée Y")
S("flipping Y impossible", "impossible de retourner Y")
S("swap X and Y", "échanger X et Y")
S("swapping impossible", "échange impossible")

S(
  "This matrix lets you play on the quotient spaces of three-dimensional. "
  "Euclidean space. Every column specifies a translation vector which "
  "takes you back to the starting point. For example, if you put "
  "set 2, 6, 0 on the diagonal, you get back to the starting point "
  "if you move 2 steps in the X direction, 6 steps in the Y direction "
  "(the quotient space is infinite in the Z direction).\n\n"
  "You can also introduce twists for diagonal matrices: after going "
  "the given number of steps in the Z direction, the space is also "
  "mirrored or rotated. (More general 'twisted' spaces are currently "
  "not implemented.)",

  "Cette matrice vous permet de jouer sur les espaces quotients "
  "d'espaces euclidiens 3D. Chaque colonne correspond à un vecteur "
  "de translation qui vous ramène au point de départ. Par exemple, "
  "si vous pouvez juste régler 2, 6, 0 sur la diagonale, vous revenez "
  "au point de départ si vous vous déplacez de deux pas dans la "
  "direction X, 6 pas dans la direction Y (l'espace quotient est "
  "infini dans la direction Z).\n\n"
  "Vous pouvez aussi introduire des torsions pour les matrices "
  "diagonales : après vous être déplacés le nombre donné de pas "
  "dans la direction Z, l'espace est aussi reflété ou pivoté. "
  "(Les espaces 'tordus' plus généraux ne sont actuellement pas "
  "implémentés.)")

// map editor 

S("pick something", "choisir quelque chose") // to edit

S("z = camera", "z = caméra")
S("Z = spheres", "z = sphère")
S("z = equi", "z = équi")

S("The distance from the camera to added points.", "Distance de la caméra aux points ajoutés")
S("place points at fixed radius", "placer les points à un rayon fixé")
S("place points on equidistant surfaces", "placer les points sur des surfaces équidistantes")
S("mousewheel step", "pas du bouton scroll")
S("hint: shift for finer steps", "Astuce : shift pour des pas plus petits")

S("random walk", "marche aléatoire")

// dual like two at once, not in mathematical sense
S("dual geometry mode", "mode géométrie duale")

S("Impossible.", "Impossible.")

S("subgame affected", "sous-jeu affecté")
S("both", "les deux à la fois")

// changing the rug model distance
S("adjust points", "ajuster les points")
S("center on camera", "centrer sur la caméra")
S("adjust edges", "ajuster les arrêtes")
S("adjust distance", "ajuster la distance")

S("memory", "mémoire")

S(
  "HyperRogue takes place in a world that is larger than anything Euclidean. "
  "Unfortunately, in some cases running it on an Euclidean computer might be "
  "a problem -- the computer could simply run out of memory. Some lands (such as the Ocean or the Brown Islands) "
  "may use up memory very fast!\n\n",
  
  "HyperRogue se passe dans un monde plus grand que n'importe quoi d'euclidien. "
  "Malheureusement, dans certains cas le faire tourner sur un ordinateur euclidien "
  "peut être un problème -- l'ordinateur pourrait simplement manquer de mémoire. Certains "
  "pays (comme l'Océan ou les Îles Marrons) peuvent utiliser la mémoire très vite !\n\n")
  
S("You are playing a 32-bit HyperRogue executable, which can only use 4GB of memory.\n\n",
  "Vous jouez à un exécutable d'HyperRogue 32-bit, qui ne peut utiliser que 4GB de mémoire.")

S(
  "Although you are extremely unlikely to return to a place you have already been to, "
  "the game never forgets these areas, unless you start a new game, use an Orb of "
  "Safety (found in Land of Eternal Motion, the Prairie, and the Ocean), or activate the memory "
  "saving mode, which tries to intelligently predict which cells you will never find "
  "again and can be safely forgotten.\n\n",
  
  "Même s'il est très invraisemblable que vous retourniez à un endroit où vous avez déjà été, "
  "le jeu n'oublie jamais ces zones, à moins de démarrer un nouveau jeu, utiliser une Orbe de "
  "Sécurité (se trouve dans le pays du Mouvement Perpétuel, la Prairie, l'Océan), ou active le "
  "mode économie de mémoire , qui essaie de prédire quelles cases vous ne trouverez jamais "
  "à nouveau et qui peuvent être oubliées de manière sûre.")

S("cells in memory", "cases en mémoire")

S("memory saving mode", "mode économie de mémoire")
S("show memory warnings", "montrer l'avertissement de mémoire")

S("just let me find Orb of Safety or finish the game", "me permet juste de trouver une Orbe de Sécurité ou terminer le jeu")
S("memory reserve", "réserve de mémoire")
S("When to show a memory warning.", "A quel moment montrer un avertissement mémoire")
S("clear caches", "effacer les caches")

// in animation menu
S("shmup action", "mode shoot'em up")

// 11.2

// extra projections
S("central inversion", "projection centrale inversée")
S("two-point azimuthal", "projection azimutale à deux-points")
S("two-point hybrid", "projection hybride à deux-points")
S("Mollweide", "projection de Mollweide")
S("central cylindrical", "projection centrale cylindrique")
S("Collignon", "projection de Collignon")

// extra settings

S("draw crosshair", "tracer la croix")

S("Display a targetting reticle in the center of the screen. Might be useful when exploring 3D modes, "
  "as it precisely shows the direction we are going. However, the option is available in all modes.",
  
  "Affiche un réticule de visée au centre de l'écran. Peut être utile pour explorer les modes 3D, car "
  "il montre précisément la direction dans laquelle on va. Cependant cette option est disponible dans "
  "tout les modes.")

S("radar range", "portée du radar")

S("boundary width multiplier", "multiplieur de la largeur de limite")
S("grid width multiplier", "multiplieur de largeur de grille")
S("highlight the cell forward", "surligne la case devant")
S("edit widths individually", "édite les largeurs individuellement")

// extra tilings

S("kite-and-dart", "kite-and-dart")
S("kite-and-dart on horospheres", "kite-and-dart sur les horosphères")
S("standard binary tiling", "pavage binaire standard")

S("show quotient spaces", "montrer les espaces quotients")
S("three-dimensional", "tridimensionnel")

// non-isotropic geometries:

S("Solv geometry", "géométrie Solv")
S("Sol", "Sol")

S(
  "Solv (aka Sol) is a 3D space where directions work in different ways. It is described by the following metric:\n"
  "ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  "You are currently displaying Solv in the perspective projection based on native geodesics. You can control how "
  "the fog effects depends on the geodesic distance, and how far object in X/Y/Z coordinates are rendered.",
  
  "Solv (aka Sol) est un espace 3D où les directions marchent différemment. Il est décrit par la métrique suivante :\n"
  " ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  
  "Vous affichez actuellement Solv dans la projection perspective basée sur la géodésique native. Vous pouvez "
  "contrôler comment les effets brouillard dépendent de la distance géodésique, et à quelle distance les objets "
  "en coordonnées X/Y/Z sont affichés.")

S("max difference in X/Y coordinates", "différence maximale en coordonnées X/Y")
S("max difference in Z coordinate", "différence maximale en coordonnée Z")

S("geodesic movement in Sol/Nil", "mouvement géodésique en Sol/Nil")

// ---

S("Nil geometry", "géométrie Nil")
S("Nil", "Nil")

// ---

S("product space", "espace produit")
S("current geometry x E", "géométrie actuelle x E")
S("Only works with 2D non-Euclidean geometries", "Marche seulement avec les géométries 2D non-euclidiennes")
S("%1 x E", "%1 x E")
S("Z-level height factor", "facteur taille de niveau Z")

S("precision of S2xE rings", "précision des anneaux S2xE")

S(
  "In S2xE, objects at spherical distances which are multiples of π will look like "
  "rings, and objects close to these will look like crescents. "
  "This setting controls the quality of rendering these rings and crescents.",
  
  "Dans S2xE, les objets à des distances sphériques qui sont des multiples de π "
  "auront l'air d'anneaux, et les objets proches de croissants. "
  "Ce réglage contrôle la qualité d'affichage de ces anneaux et croissants.")

// ---

S("rotation space", "espace de rotation")
S("steps", "pas")

// 11.3
//======

// new geometries

S("space of rotations in current geometry", "espaces des rotations dans la géométrie actuelle")
S("~SL(2,R)~", "~SL(2,R)~")
S("Only works with (semi-)regular tilings", "Ne marche qu'avec les pavages (semi-)réguliers")
S("That would have %1/%2 levels", "Cela aurait %1/%2 niveaux")
S("number of levels", "nombre de niveaux")
S("Hint: this is more playable with pure {7,3} or pure {5,4}", "Astuce : ceci est plus jouable avec {7,3} ou {5,4} non modifiés")

S("view the underlying geometry", "voir la géométrie sous-jacente")

S("The space you are currently in is the space of rotations of the underlying hyperbolic or spherical geometry. ",
  "L'espace dans lequel vous êtes actuellement est l'espace des rotations de l'espace hyperbolique sous-jacent ou la géométrie sphérique. ")

S("This option lets you see the underlying space. Lands and some walls (e.g. in the Graveyard) are based on "
  "the respective features in the underlying world, but details such as monsters or items are ignored.",
  
  "Cette option vous permet de voir l'espace sous-jacent. Les pays et certains murs (par ex. dans le Cimetière) "
  "sont basés les fonctionnalités respectives du monde sous-jacent, mais les détails tels que les monstres et "
  "objets sont ignorés.")

// map editor in 3D nonisotropic

S("place points on surfaces of const Z", "placer des points sur les surfaces de Z constant")
S("place points on surfaces of const X", "placer des points sur les surfaces de X constant")
S("place points on surfaces of const Y", "placer des points sur les surfaces de Y constant")

S("parallels to draw", "parallèles à tracer")
S("meridians to draw", "méridiens à tracer")
S("range of grid to draw", "portée de la grille à tracer")

S("current filter", "filtre actuel")
S("Solv quotient spaces", "espace quotient Solv")
S("%1 period", "période %1")
S("Note: the value 0 functions effectively as the size of int (2^32).", "Note : la valeur 0 fonctionne comme la taille de l'entier (2^32).")

S("standard ternary tiling", "pavage standard ternaire")
S("stretched hyperbolic", "hyperbolique étirée")
S("stretched Solv", "géométrie Solv étirée")

S("{3,∞} (infinite triangles)", "{3,∞} (triangles infinis)")
S("{4,∞} (infinite squares)", "{4,∞} (carrés infinis)")
S("{3/4,∞} (infinite triangles and squares)", "{3/4,∞} (triangles et carrés infinis)")

S("4D crystal in H3", "4D cristal en H3")
S("6D crystal in H3", "6D cristal en H3")

S("Arnold's cat mapping torus", "tore projection du chat d'Arnaud ")
S("{5,3,5} quotient", "espace quotient {5,3,5}")
S("Seifert-Weber space", "espace de Seifert-Weber")
S("Poincaré homology sphere", "sphère d'homologie de Poincaré")

S("load from file", "charger depuis un fichier")

S("Crystal torus", "Tore de cristal")
S("Z_k^d instead of Z^d. Only works with k even.", "Z_k^d au lieu de Z^d. Marche seulement avec k pair.")

S("Euclidean quotient spaces", "Espaces quotient euclidiens")
S("columns specify periods", "périodes de colonnes spécifiques")
S("(vectors you need to take to get back to start)", "(vecteurs que vous avez besoin de suivre pour revenir au début)")
S("third-turn space", "espace d'un tiers de tour")
S("make it even", "le rendre pair")
S("or for columns : (A,B,C), (B,C,A), (D,D,D) where A+B+C=0", "ou pour les colonnes : (A,B,C), (B,C,A), (D,D,D) où A+B+C=0")
S("quarter-turn space", "espace d'un quart de tour")

S("change the second column for Möbius bands and Klein bottles", "changer la seconde colonne pour les rubans de Möbius et bouteilles de Klein")
S("second period is chiral -- cannot be mirrored", "la seconde période est chirale -- ne peut être reflétée")
S("periods must be orthogonal for mirroring", "les périodes doivent être orthogonales pour la réflexion")
S("mirror flip in the second period", "renversement miroir dans la seconde période")
S("single-cell torus", "tore d'une seule case")
S("Klein bottle", "bouteille de Klein")
S("large regular torus", "grand tore régulier")
S("cylinder", "cylindre")
S("Möbius band", "ruban de Möbius")
S("seven-colorable torus", "tore sept-colorable")
S("HyperRogue classical torus", "HyperRogue tore classique")
S("no quotient", "pas de quotient")
S("standard rotation", "rotation standard")

S("field quotient", "espace de champ quotient")
S("order %1%2 (%3 %4s)", "ordre %1%2 (%3 %4s)")
S("order %1%2 (%3 %4-gons)", "ordre %1%2 (%3 %4-gones)")

S("non-isotropic", "géométrie anisotrope")
S("regular 2D tesselations", "pavage régulier 2D")
S("regular 3D honeycombs", "nids d'abeilles réguliers 3D")
S("interesting quotient spaces", "espaces quotients intéressants")
S("geometries", "géométries")
S("geometry", "géométrie")

S("start discovery", "commencer la découverte")
S("resume discovery", "reprendre la découverte")
S("suspend discovery", "suspendre la découverte")

S("#%1, cells: %2", "#%1, cases : %2")

S("hyperbolic (3:2)", "hyperbolique (3:2)")
S("Sol (3:2)", "Sol (3:2)")

S("quotient spaces in ", "espaces quotient dans ")
S("No quotient spaces available in the current tiling.",
  "Pas d'espaces quotients disponibles dans le pavage actuel.")

S("You are currently in a product space.", "Vous êtes actuellement dans un espace produit.")

S("Nil quotient spaces", "espaces quotients de Nil")
S("honeycomb", "grille")
S("Y period must be divisible by Z period", "La période Y doit être divisible par la période Z")
S("quotient product spaces", "quotient d'espaces produits")
S("Set to 0 to make it non-periodic.", "Régler à 0 pour le rendre apériodique.")
S("Works if the underlying space is symmetric.", "Marche si l'espace sous-jacent est symétrique.")
S("reflect", "reflète")
S("the current rotation is invalid", "la rotation actuelle est invalide")

S("Note: adjacency rule affects environmental effects, but not movement.",
  "Note : la règle d'adjacence affecte les effets environnementaux, mais pas le mouvement.")

S("\n\nOption -mineadj 1 can be added for Moore neighborhoods.", "\n\nOption -mineadj 1 peut être ajouté pour les voisinages de Moore.")

// WFC generation

S("configure WFC", "configurer WFC")

S("Wave Function Collapse", "Effondrement de la Fonction d'Onde (WFC)")
S("import the current map", "importer la carte actuelle")
S("generate a map using WFC", "générer une carte avec WFC")

// racing in Thurston geometries

S("Racing", "Course")
S("how fast can you reach the end?", "à quelle vitesse pouvez vous atteindre la fin ?")
S("Racing in Thurston geometries", "Courses en géométries de Thurston")
S("race through a maze in exotic 3D geometry!", "courir à travers un labyrinthe dans une géométrie 3D exotique !")

S("periodic Solv", "Solv périodique")
S("hyperbolic crystal", "cristal hyperbolique")
S("hyperbolic regular", "grille hyperbolique régulière")
S("S2xE regular", "S2xE régulier")
S("H2xE regular", "H2xE régulier")
S("periodic Nil", "Nil périodique")
S("alternate versions", "versions alternatives")

// extra option

S("floors in 2D geometries", "sols en géométries 2D")

S("slides", "diapositives")

// raycasting

S("configure raycasting", "configurer le tracé de rayons")
S("raycasting configuration", "configuration du tracé de rayons")
S("available in current geometry", "disponible dans la géométrie actuelle")
S("use raycasting?", "utiliser le tracé de rayons ?")
S("comparison mode", "mode comparaison")
S("exponential range", "potée exponentielle")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))", "formule de brillance : max(1-d/sightrange, s*exp(-d/r))")
S("exponential start", "démarrage exponentiel")
S("hard limit", "limite dure")
S("no limit", "pas de limite")
S("reflective walls", "murs réfléchissants")
S("max step", "pas maximal")
S("affects the precision of solving the geodesic equation in Solv", "affecte la précision de résolution de l'équation géodésique dans Solv")
S("min step", "pas minimal")
S("how precisely should we find out when do cross the cell boundary", "avec quelle précision trouver quand traverser la frontière de la case")
S("iterations", "itération")
S("in H3/H2xE/E3 this is the number of cell boundaries; in nonisotropic, the number of simulation steps",
  "dans H3/H2xE/E3 ceci est le nombre de frontières de cases ; en anisotrope, le nombre d'étapes de simulation")
S("max cells", "nombre maximal de cases")
S("generate", "genérer")
S("out-of-range color", "couleur hors palette")

// new graphical styles

S("standard graphics", "graphiques standard")
S("neon mode", "mode néon")
S("no boundary mode", "mode sans frontière")
S("neon mode II", "mode néon II")
S("illustration mode", "mode illustration")
S("hint: Press Alt while testing modes", "Astuce : appuyer sur Alt en testant des modes")
S("disable shadows", "désactiver les ombres")
S("bright mode", "mode lumineux")
S("colorblind simulation", "simulation daltonisme")
S("ASCII/3D", "ASCII/3D")

// new configuration options

S("particles on attack", "paticules à l'attaque")
S("floating bubbles: special", "bulles flottantes : spécial")
S("floating bubbles: treasure  thresholds", "bulles flottantes : seuil de trésors")
S("floating bubbles: all treasures", "bulles flottantes : tout les trésors")

S("variable width", "largeur variable")
S("lines longer than this value will be split into shorter lines, with width computed separately for each of them.",
  "les lignes plus larges que cette valeyr sont divisées en lignes plus courtes, dont la largeur de ligne est calculée séparement pour chacune.")

S("smoothen", "lisser")
S("large values help if the joystick is imprecise", "de plus grandes valeurs aident si la manette est imprécise")

S("level lines", "lignes de niveau")
S("This feature superimposes level lines on the rendered screen. These lines depend on the Z coordinate. In 3D hyperbolic the Z coordinate is taken from the Klein model. "
  "Level lines can be used to observe the curvature: circles correspond to positive curvature, while hyperbolas correspond to negative. See e.g. the Hypersian Rug mode.",
  
  "Cette fonctionnalité superpose les lignes de niveau sur l'affichage à l'écran. Ces lignes dépendent des coordonnées Z. Dans l'espace 3D hyperbolique la "
  "coordonnée Z est prise du modèle de Klein. Les lignes de niveau peuvent être utilisées pour observer la courbure : les cercles correspondent à une courbure "
  "positive, tandis que les hyperboles correspondent à une négative. Voir par exemple le mode Tapis Hypersan.")

S("disable textures", "désactiver les textures")
S("disable level lines", "désactiver les lignes de niveau")

S("Size of the fish eye.", "Taille de l’œil de poisson.")
S("product Z stretch", "produit Z étiré")
S("horocyclic coordinates", "coordonnées horocycliques")

// dual geometry puzzles

S("You won!", "Vous gagnez !")
S("dual geometry puzzles", "puzzles en géométrie duale")
S("move both characters to marked squares at once!", "déplacer les deux personnages aux carrés marqués à la fois !")
S("enter seed", "entrer une graine")
S("Euclidean size", "taille euclidien")
S("hyperbolic size", "taille hyperbolique")

// another new mode

S("play this Strange Challenge unofficially", "jouer à ce Défi étrange de manière non officielle")

// new gameplay lines

S("\n\nIn the Orb Strategy Mode, the effect is increased to +100%.",
  "\n\nDans le mode stratégique des Orbes, l'effet est augmenté de +100%.")

S("\n\nAdditionally, your allies are protected from your indirect attacks.", 
  "\n\nDe plus, vos alliés sont protégés de vos attaques indirectes.")

S("\n\nLeaves cut directly: %1", "\n\nFeuilles coupées directement : %1")
S("\n\nLeaves cut onscreen: %1", "\n\nFeuilles coupées à l'écran : %1")
S("\n\nLeaves cut offscreen (approximately): %1", "\n\nFeuilles coupées hors écran (approximativement) : %1")
S("leaves cut: %1", "feuilles coupées : %1")

// explanations for the start screen

S("(random option)", "(option aléatoire)")
S("(seasonal option)", "(option saisonnière)")

S("spinning in the band model", "tourner dans le modèle ruban")
S("spinning around", "tourner autour")
S("row of ghosts", "rangée de fantômes")
S("army of ghosts", "armée de fantômes")
S("ghost spiral", "spirale de fantômes")
S("Fibonacci ghosts", "fantômes de Fibonacci")

// new welcome messages

S("Welcome to Spherindrogue!", "Bienvenue  dans Sphérindrogue !")
S("Welcome to Hyper-X-R-Rogue!", "Bienvenue dans Hyper-X-R-Rogue !")
S("Good luck in the elliptic space!", "Bonne chance dans l'espace elliptique !")
S("Welcome to SolvRogue!", "Bienvenue dans SolvRogue !")
S("Welcome to NilRogue!", "Bienvenue dans NilRogue !")
S("Welcome to PSL(2,R)-ogue!", "Bienvenue dans PSL(2,R)-ogue !")

// new patterns and line patterns

S("empty", "vide")
S("rainbow by distance", "arc-en-ciel par distance")
S("Cage", "Cage")
S("Hyperplanes", "Hyperplans")
S("Honeycomb", "Nid d'abeilles")
S("Diagonal", "Diagonale")
S("Penrose staircase", "Escalier de Penrose")
S("nearer end", "fin plus proche")
S("furthest from start", "le plus loin du début")
S("types", "types")
S("display the inner walls", "afficher les murs internes")
S("floor type", "type de sol")
S("(no wall)", "(pas de murs)")
S("Goldberg tree", "arbre de Goldberg")
S("irregular master", "maître irrégulier")
S("sub-lines", "sous-lignes")
S("fat edges", "bords épais")
S("You chop down the shrub.", "Vous abattez l'arbuste.")

// new lands

N("Wetland", GEN_M, "Marécage", "Marécages", "dans les Marécages")

S("Some people have definitely drowned in this treacherous area. Better be careful!",
  "Certaines personnes se sont noyées définitivement dans cet endroit traître. Faites attention !")

N4("Water Lily", GEN_M, "Nénuphar", "Nénuphars")
S("A beautiful plant from the Wetland.", "Une magnifique plante des marécages.")

N4("shallow water", GEN_F, "eau peu profonde", "eaux peu profondes")
S("Shallow water is passable both for land creatures and for water creatures.",
  "L'eau peu profonde peut être traversée par des créatures terrestres et aquatiques.")

N4("deep water", GEN_F, "eau profonde", "eaux profondes")

S("This body of water is calm and beautiful, but too deep to pass.",
  "Cette pièce d'eau est calme et magnifique, mais trop profonde à traverser.")

S("You remember anglers from your hometown showing the impressive pikes they have caught. This one is much larger.",
  "Vous vous rappelez les pêcheurs de votre ville natale vous montrer les brochets qu'ils ont attrapé. Celui là est "
  "bien plus large.")

S("World Turtle", "Tortue Monde")

S("Have you reached the edge of the Poincaré disk? No, it must just be the edge of this world. "
  "Which happens to be floating on the backs of some turtles. Wondering how such not very big turtles could "
  "support such a large world? Well, there are LOTS of them! "
  "This one got so annoyed by the fact that you discovered this that it left its post. However, the world still "
  "stands, because there are so many turtles.",
  
  "Avez vous atteint le bord du disque de Poincaré ? Non, mais cela doit juste être le bord de ce monde. "
  "Qui semble flotter sur le dos de tortues. Comment des tortues pas si immenses peuvent supporter un "
  "monde aussi grand ? Eh bien il y en a BEAUCOUP ! "
  "Celle-là semble si ennuyée par le fait d'avoir été découverte qu'elle a quitté son poste. Cependant, ce "
  "tient encore, car il y a tellement de tortues.")

// gender-switched
N4("Rusałka", GEN_F, "Roussalka", "Roussalki")
S("A malicious water being. When you kill her, she changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "Une créature aquatique malveillante. Quand vous la tuez, elle change la case sur laquelle vous vous tenez, de terre à eau peu profonde, ou eau peu profonde à eau profonde.")
  
N4("Topielec", GEN_M, "Topielec", "Topielce")
S("A malicious water being. When you kill him, he changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "Une créature aquatique malveillante. Quand vous le tuez, il change la case sur laquelle vous vous tenez, de terre à eau peu profonde, ou eau peu profonde à eau profonde.")

// przet

Orb("Plague", "Peste")
S(
  "When you kill an enemy, adjacent enemies are also attacked, as long as they are further away from you than from the originally attacked enemy. "
  "These enemies may in turn infect further enemies, up to distance of at most 4 from you.",
  "Quand vous tuez un ennemi, les ennemis adjacent sont aussi attaqués, tant qu'ils sont plus loin de vous que l'ennemi initialement attaqué. "
  "Ces ennemis peuvent à leur tour infecter plus d'ennemis, jusqu'à une distance d'au plus 4 de vous.")

//---

N("Frog Park", GEN_M, "Parc à grenouilles", "Parcs à grenouilles", "dans le Parc à grenouilles")

S("This park is a perfect place for frogs to hide from predators. "
  "So perfect that the frogs have grown in size, gained magical abilities, "
  "and become very dangerous themselves.",
  "Ce parc est parfait pour que les grenouilles se cachent des prédateurs. "
  "Si parfait que les grenouilles ont grandi, gagné des pouvoirs magiques, "
  "et devenues elles-même très dangereuses."
  )

N4("Gold Ball", GEN_F, "Balle d'Or", "Balles d'Or")
S("A toy lost in the Frog Park by a spoiled child.", "Un jouet perdu dans le parc à grenouilles par un enfant gâté.")

Orb("Impact", "d'Impact")
S("Whenever you use a ranged Orb to achieve an effect, all the single-tile monsters adjacent to the target "
  "location are stunned.",
  "Lorsque vous utilisez une Orbe à distance pour obtenir un effet, tout les monstres d'une seule case adjacents "
  "à la cible sont assommés.")

N4("shrub", GEN_M, "arbuste", "arbustes")
S("A strange small tree that cannot be attacked with mundane weapons nor phased though. "
  "It can be vaulted over, though.",
  "Un étrange petit arbre qui ne peut être attaqué avec des armes ordinaires ou traversés. "
  "On peut néanmoins sauter au-dessus.")

N4("Giant Frog", GEN_F, "Grenouille Géante", "Grenouilles Géantes")

S(
  "At first, you are shocked by the agility of this frog. Such a large creature "
  "should not be able to jump that quickly!\n\n"
  "Then, you notice the green glow around its legs. This frog must be magical... "
  "it has sacrificed its swimming abilities for superfrog jumping powers.",
  
  "Au début, vous êtes choqués par l'agilité de cette grenouille. Une créature "
  "aussi large ne devrait pas pouvoir sauter si vite !\n\n"
  "Alors, vous remarquez la lueur verte autour de ses jambes. Cette grenouille doit "
  "être magique... elle a sacrifié sa capacité de nager pour des pouvoirs de saut "
  "suramphibiens.")

N4("Yellow Frog", GEN_F, "Grenouille Jaune", "Grenouilles Jaunes")

S("A slightly transparent yellow frog. It has mastered the magical power of phasing through solid "
  "obstacles such as rock.",
  "Une grenouille légèrement transparente. Elle a maîtrisé le pouvoir magique de traverser à travers "
  "des obstacles solides comme de la roche.")

N4("Blue Frog", GEN_F, "Grenouille Bleue", "Grenouilles Bleues")

S(
  "This frog is able to vault over the shrubs in the Frog Park, destroying the shrub in the process. "
  "While it can also vault over monsters, it will never hurt the other frogs!",

  "Cette grenouille doit être capable de sauter au-dessus des arbustes du Parc à grenouilles, détruisant "
  "l'arbuste au passage. Tandis qu'elle peut sauter au-dessus des monstres, elle ne blessera jamais d'autres "
  "grenouilles !")

S("\n\nThis Frog uses the power of %the1. You get 5 charges yourself for killing it.", "\n\nCette grenouille utilise le pouvoir de %le1. Vous obtenez 5 charges pour l'avoir tuée.")

//---

N4("Eclectic City", GEN_F, "Cité éclectique", "Cités éclectiques")

S("Many kinds of creatures are living here, each with its own architectural style.",
  "De nombreuses sortes de créatures vivent ici, chacune avec son propre style architectural.")

N4("Lazurite Figurine", GEN_F, "Figurine de Lazurite", "Figurines de Lazurite")

S("A beautiful blue figurine. Every figurine is different.",
  "Une magnifique figurine bleue. Chaque figurine est différente.")
  
Orb("Chaos", "Chaos")
S(
  "When you move, the contents of the two cells adjacent to your both locations are swapped. "
  "If they contain creatures, these creatures will be stunned for some time, before they understand what happened.\n\n"
  "Does not work if one of the cells contains a "
  "multi-tile creature or a barrier-like wall.",
  
  "Quand vous bouger, les contenus des cases adjacentes aux deux emplacements sont échangés. "
  "Si elles contiennent des créatures, elles seront assommées pendant un certain temps, avant de comprendre ce qui s'est passé.\n\n"
  "Ne marche pas si l'une des cases contient une créature multi-case ou un mur barrière.")

S("digon", "digone")
S("triangle", "triangle")
S("square", "carré")
S("pentagon", "pentagone")
S("hexagon", "hexagone")
S("heptagon", "heptagone")
S("octagon", "octogone")

// 12.0
//======

// missing Android buttons

S("WAIT", "PAUSE")
S("RUG", "TAPIS")
S("CAM", "CAM")

// missing UI texts

S("error: ", "erreur : ")
S("rules: %1", "règles : %1") // WFC

// missing game texts

S("You polymorph %the1 into %the2!", "Vous transformez %le1 en %le2!")
S("All players are in the game!", "Tous les joueurs sont dans le jeu !")
S("\"Are there Temples of Cthulhu in your world? Why not?\"", 
  "\"Il y a t il des temples de Cthulhu dans votre monde ? Pourquoi pas ?\"")
S("saves", "sauvegardes")
S("players", "joueurs")
S("You swing your sword at %the1.", "Vous balancez votre épée vers %le1.")
S("You swing your sword.", "Vous balancez votre épée.")
S("There is no room for %the1!", "Il n'y a pas de place pour %le1!")
S("Don't play with slime and electricity next time, okay?", "Ne jouez pas avec de la gélatine et de l'électricité la prochaine fois, OK?")
S("The Orb of Safety from the Land of Eternal Motion might save you.", 
  "L'Orbe de Sécurité du Pays du Mouvement Perpétuel pourrait vous sauver.")
S("%the1 protects your boat!", "%Le1 protège votre bateau !")
S("Are you sure you want to step there?", "Êtes vous sûr%e0 de vouloir marcher là ?")
S("Hint: press 1 to change the projection.", "Astuce : appuyer sur 1 pour changer la projection.")
S("%The1 turned out to be an illusion!", "%Le1 est en fait une illusion !")
S("You feel that these directions are %1 turns old.", "Vous sentez que ces direction ont %1 tours de retard.")
S("%The1 raises his weapon...", "%Le1 lève son arme...")
S("%The1 takes %his1 revenge on %the2!", "%Le1 prend sa revanche sur %le2!")

// parameters

S("You can enter formulas in this dialog.", "Vous pouvez entrer des formules dans ce dialogue.")
S("Functions available:", "Fonctions disponibles :")
S("Constants and variables available:", "Contenus et variables disponibles : ")
S("Animations:", "Animations : ")
S("a..b -- animate linearly from a to b", "a..b -- anime linéairement de a à b")
S("a..b..|c..d -- animate from a to b, then from c to d", "a..b..|c..d -- anime de a à b, puis de c à d")
S("a../x..b../y -- change smoothly, x and y are derivatives", "a../x..b../y -- change doucement, x et y sont dérivées")
S("Parameter names, e.g. '%1'", "Noms des paramètres, par exemple '%1'")
S("Parameter names", "Noms des paramètres")
S("These can be combined, e.g. %1", "Ceux là peuvent être combinés, par exemple %1")

// manifolds

S("alternate manifolds", "variétés alternatives")
S("generators (%1,%2), size %3", "genérateurs (%1,%2), taille %3")
S("This option finds alternate solutions. For example, there are three {7,3} manifolds with 156 heptagons each (\"first Hurwitz triplet\").", 
  "Cette option trouve des solutions alternatives. Par exemple, il y a trois {7,3} variétés avec 156 heptagones chacune (\"premier triplet de Hurwitz\").")

// unrecognized nonliteral: f->name in geom-exp.cpp:274
S("Only works with 2D geometries", "Ne marche qu'avec les géométries 2D")
S("stereographic Panini", "projection stéréographique de Panini")
S("orthographic projection", "projection orthographique")
S("#%1, cells: %2, p=%3", "#%1, cases : %2, p=%3")
S("affine", "affine")
// unrecognized nonliteral: qstring in geom-exp.cpp:625
S("geometry/topology/tiling", "géométrie/topologie/pavage")
S("binary tiling width", "largeur du pavage binaire")
S("Nil width", "largeur nulle")
S("stretched geometry", "géométrie étirée")
S(
  "Stretch the metric along the fibers. This can currently be done in rotation spaces and in 8-cell, 24-cell and 120-cell. "
  "Value of 0 means not stretched, -1 means S2xE or H2xE (works only in the limit). (Must be > -1)",
  
  "Étire la métrique le long des fibres. Ceci peut actuellement être fait dans les espaces de rotation et dans 8-cases, 24-cases, et 120-cases. "
  "La valeur de 0 signifie non étiré, -1 signifie S2xE ou H2xE (fonctionne seulement dans la limite). Doit être > -1.")
  
S("basic tiling", "pavage basique")
S("fake curvature", "fausse courbure")
S("tessellation sliders", "curseurs de tesselation")
S("truncate ultra-vertices with mirrors", "tronquer les ultra-sommets avec des miroirs")
S("adjacency rule", "règle d'adjacence")
S("vertex", "sommet")
S("face", "face")
S("edge", "bord")
S("info about: %1", "info sur : %1")
S("genus", "genre")

S("VR demos", "démo VR")
S("warning: these will restart your game!", "avertissement : ceci va redémarrer votre jeu !")
S("standard HyperRogue but in VR", "HyperRogue standard mais en VR")
S("HyperRogue FPP", "HyperRogue FPP")
S("Hypersian Rug", "Tapis Hypersan")
S("sphere from the inside", "sphère de l'intérieur")
S("sphere from the outside", "sphère de l'extérieur")
S("Thurston racing", "course dans la géométrie de Thurston")
S("raytracing in H3", "tracé de rayon dans H3")

S("VR settings", "réglages de la VR")
S("VR enabled", "VR activée")
S("VR initialized correctly", "VR initialisée correctement")
S("(this setting is for 3D geometries only, use 'model viewing' instead)", 
  "(ce réglage est pour les géométries 3D uniquement, utiliser 'visualiser modèle' à la place)")
  
S("absolute unit in meters", "unités absolues en mètres")

S(
  "The size of the absolute unit of the non-Euclidean geometry in meters. "
  "This affects the headset movement and binocular vision.\n\n"
  "In spherical geometry, the absolute unit is the radius of the sphere. "
  "The smaller the absolute unit, the stronger the non-Euclidean effects.\n\n"
  "Elements of the HyperRogue world have fixed size in terms of absolute units, "
  "so reducing the absolute unit makes them smaller. "
  "If you are playing in the Euclidean mode, this feature just scales everything "
  "(e.g., in the cube tiling, the 'absolute unit' is just the edge of the cube). "
  "Only perspective projections are affected, other models use the 'VR scale' setting "
  "from the Projections menu.",
  
  "La taille de l'unité absolue la géométrie non-euclidienne en mètres. "
  "Cela affecte le mouvement du casque et la vision binoculaire.\n\n"
  "Dans la géométrie sphérique, l'unité absolue est le rayon de la sphère. "
  "Plus l'unité absolue est petite, plus les effets non-euclidiens sont forts.\n\n"
  "Les éléments du monde d'HyperRogue ont une taille fixée en unités absolues, "
  "donc réduire l'unité absolue les rend plus petits. "
  "Si vous jouez en mode euclidien, cette fonctionnalité change tout d'échelle "
  "(par exemple dans le pavage en cube, 'l'unité absolue' est juste l'arrête du cube) "
  "Seules les projections en perspective sont affectées, les autres modèles utilisent "
  "le réglage 'échelle VR' depuis le menu Projections.")
        
S("reset the reference point", "réinitialiser le point de référence")
S("pointer length", "longueur du pointeur")
S(
  "If the pointer length is 0.5m, the object pointed to is 0.5 meter from the controller. "
  "This is used in situations where the controller is used as a 3D mouse, e.g., "
  "the drawing tool in three-dimensional geometries. When pointing at two-dimensional surfaces, "
  "this is not relevant (the pointer is as long as needed to hit the surface).",
  
  "Si la longueur du pointeur est 0.5m, l'objet pointé est à 0.5 mètres du contrôleur. "
  "Ceci est utilisé dans les situations où le contrôleur est utilisé comme une souris 3D, "
  "par exemple comme outil de dessin en géométrie 3D. Lorsque l'on pointe des surfaces 2D "
  "ce n'est pas utile (le pointeur est suffisant pour la surface).")
  
S("UI size", "taille de l'UI")
S("How big is a pixel of the user interface (HUD and menus). The user interface is as big as the window on the desktop.",
  "Taille d'un pixel de l'interface utilisateur (HUD et menu). L'interface utilisateur est aussi grande qu'une fenêtre "
  "d'ordinateur.")

S("UI depth", "profondeur de l'UI")
S("How far to show the user interface (HUD and menus).", "A quelle distance montrer l'interface utilisateur (HUD et menu).")

//

S("Not available in the daily challenge!", "Non disponible dans le défi du jour !")
S("Welcome to CoverRogue!", "Bienvenue dans RecouvrementRogue !")
S("Welcome to SL(2,R)-ogue!", "Bienvenue dans SL(2,R)-ogue !")
S("Emergency save at ", "Sauvegarde d'urgence dans ")

// tour

S("You cannot change geometry in this slide.", "Vous ne pouvez pas changer la géométrie dans cette diapositive.")
S("This does not work in bounded geometries.", "Ceci ne marche pas en géométrie à bords")
S("This does not work in Euclidean geometry.", "Ceci ne marche pas en géométrie euclidienne.")
S("This works only in hyperbolic geometry.", "Ceci ne marche qu'en géométrie hyperbolique.")
S("This land has no Euclidean version.", "Ce pays n'a pas de version euclidienne.")
S("This land has no spherical version.", "Ce pays n'a pas de version sphérique.")
S("Changed the projection.", "Projection changée.")
S("HyperRogue Guided Tour", "Visite guidée d'HyperRogue")
S("slideshows", "affichage de diapositive")
S("go up", "vers le haut")

S(" (edges too short: %1)", " (arrêtes trop courtes : %1)")
S("wait...", "attendez...")
S("bitruncation const", "constante de bitroncation")
S("too small period -- irregular tiling generation fails", "période trop petite -- échec de la génération irrégulière du pavage")
S("To create Euclidean irregular tesselations, first enable a torus", 
  "Pour créer le pavage euclidien irrégulier, activez d'abord un tore")

S("Error: period mismatch", "Erreur : la période ne correspond pas")
S("Hantzsche-Wendt space", "espace de Hantzsche-Wendt")
S("HyperRogue classic torus", "tore classique d'HyperRogue")

// archimedean & arbitrile

S("invalid dihedron", "dièdre invalide")
S("face mismatch", "non-correspondance des faces")
S("face mismatch II ", "non-correspondance des faces II ")
S("face mismatch II", "non-correspondance des faces II")

S("Polygon number %1 did not close correctly (%2 %3). Here is the picture to help you understand the issue.\n\n", 
  "Le nombre de poygone %1 ne se ferme pas correctement (%2 %3). Voici l'image pour comprendre le problème.\n\n")
S("connection debugger", "connections du débogueur")
S("open a tiling", "ouvrir un pavage")

// goldberg

S("inverse rectify", "rectifier l'inverse")
S("inverse truncate", "tronquer l'inverse")
S("warped version", "version tordue")
S("swap x and y", "échanger x et y")
S("dual of current", "duel de l'actuel")
S("dual", "duel")
S("unrectified", "non-rectifié")
S("warped", "tordu")
S("untruncated", "non-tronqué")

// rug/embeddings
S("save the current embedding", "sauvegarder le plongement actuel")
S("load a saved embedding", "charger un plongement sauvegardé")
S("save embedding to:", "sauvegarder le plongement dans : ")
S("load embedding from:", "charger le plongement depuis : ")
S("Failed to save embedding to %1", "Échec de la sauvegarde du plongement vers %1")
S("Failed to load embedding from %1", "Échec du chargement depuis %1")

S("Failed to enable", "A échoué à activer")
S("protractor", "rapporteur")
S("move on touch", "se déplacer au toucher")
S("finger range", "portée du doigt")
S("Press 1 to enable the finger mode.", "Appuyer sur 1 pour activer le mode doigt.")
S("finger force", "force du doigt")
S("Press 1 to enable the finger force.", "Appuyer sur 1 pour activer la force du doigt.")

// patterns

S("probability of a wall (%)", "probabilité d'un mur (%)")
S("single color", "couleur unique")
S("distance from origin", "distance de l'origine")
S("nice coloring", "jolie coloration")
S("Crystal coordinates", "coordonnées de Cristal")
S("sides", "côtés")
S("display only chessboard white", "afficher seulement le plateau d'échec blanc")
S("display only chessboard black", "afficher seulement le plateau d'échec noir")

S(
          "This lets you specify the color pattern as a function of the cell. "
          "Available parameters:\n\n"
          "x, y, z (hyperboloid/sphere/plane coordinates in non-crystal geometries)\n"
          "ex, ey, ez (in Euclidean geometries)\n"
          "x0, x1, x2... (crystal geometry only)\n"
          "0 is black, 1 is white, rgb(1,0,0) is red, ifp(p-2,1,0) is blue (p=1 for red, 2 for green, 3 for blue).",

          "Vous laisse choisir le motif de couleur comme fonction de la case. "
	  "Paramètres disponibles : \n\n"
          "x, y, z (coordonnées hyperboloïde/sphère/plan dans des géométries non-cristallines)\n"
          "ex, ey, ez (dans une géométrie euclidienne)\n"
          "x0, x1, x2... (dans les géométries cristallines seulement)\n"
          "0 est noir, 1 est blanc, rgb(1,0,0) est rouge, ifp(p-2,1,0) est bleu (p=1 pour rouge, 2 pour vert, 3 pour bleu)."
          )
          
S(
          "w (fourth coordinate)\n"
          "wallif(condition, color)\n", 

          "w (quatrième coordonnée)\n"
          "wallif(condition, couleur)\n"
          )

S("see compute_map_function in pattern2.cpp for more\n", "voir la fonction compute_map_function dans pattern2.cpp")

S("broken Emerald Pattern", "Motif d'émeraude cassé")
S("single cells", "cases seules")
S("edit both bitrunc colors", "éditer les deux couleurs bitronquées")
S("extra symmetries", "symétries supplémentaires")
S("odd/even", "pair/impair")
S("cheat", "triche")

S("cannot adjust depth", "impossible d'ajuster la profondeur")
S("cannot adjust camera", "impossible d'ajuster la caméra")
S("does not work with perfect Klein", "ne marche pas avec le modèle de Klein")

S("randomize", "randomiser")
S("seed", "graine")
S("select a puzzle", "sélectionner un puzzle")

// texture mode

S("Failed to load %1", "Échec du chargement de %1")
S("scale/rotate the texture", "changer l'échelle/pivoter la texture")
S("scale/rotate the model", "changer l'échelle/pivoter le modèle")
S("Unexpected missing cell #%1/%1", "Case manquante inattendue #%1/%1")
S("racing in Thurston geometries", "course en géométrie de Thurston")
S("S2xE", "S2xE")
S("H2xE", "H2xE")
S("PSL(2,R)", "PSL(2,R)")
S("torus x E", "tore x E")

// crystal geometry

S(
    "This space essentially lets you play in a d-dimensional grid. Pick three "
    "dimensions and '3D display' to see how it works -- we are essentially playing on a periodic surface in "
    "three dimensions, made of hexagons; each hexagon connects to six other hexagons, in each of the 6 "
    "possible directions. Normally, the game visualizes this from the point of view of a creature living inside "
    "the surface (regularized and smoothened somewhat), assuming that light rays are also restricted to the surface -- "
    "this will look exactly like the {2d,4} tiling, except that the light rays may thus "
    "sometimes make a loop, causing you to see images of yourself in some directions (in other words, "
    "the d-dimensional grid is a quotient of the hyperbolic plane). The same construction works in other dimensions. "
    "Half dimensions are interpreted in the following way: the extra dimension only has two 'levels', for example 2.5D "
    "has a top plane and a bottom plane.\n\n"
    "You may also bitruncate this tessellation -- which makes it work better with the rules of HyperRogue, but a bit harder to understand.",
    
    "Cet espace vous permet de jouer dans une grille dimensionnelle. Choisir trois "
    "dimensions et 'affichage 3D' pour voir comment ça marche -- on est en gros en train de jouer sur une surface périodique dans "
    "trois dimensions, faites d'hexagones ; chaque hexagone est connecté à six autres hexagones, dans chacun des 6 "
    "directions possibles. Normalement le jeu visualise ceci du point de vue d'une créature vivant dans la surface "
    "(régularisé et lissée), en supposant que les rayons de lumière peuvent aussi être restreints à la surface -- "
    "ceci ressemblera exactement au pavage {2d,4}, à l'exception des rayons de lumière qui peuvent ainsi parfois "
    "faire une boucle, vous faisant voir des images de vous dans certaines directions (autrement dit, la grille "
    "d-dimensionnelle est un quotient du plan hyperbolique). La même construction marche dans d'autres dimensions. "
    "Les demi-dimensions sont interprétées de la manière suivante : la dimension en plus a seulement deux 'niveaux', par "
    "exemple 2.5D a un plan haut et un plan bas.\n\n"
    "Vous pouvez aussi bitronquer ce pavage -- ce qui le fait fonctionner mieux avec les règles d'HyperRogue, mais un peu plus dur à comprendre."    
    )
S(
    "This is a representation of four-dimensional geometry. Can you find the Holy Grail in the center of the Round Table?\n\n"
    "In 'Knight of the 16-Cell Table', each cell has 8 adjacent cells, "
    "which correspond to 8 adjacent points in the four-dimensional grid. The Round Table has the shape of a 16-cell.\n\n"
    "In 'Knight of the 3-Spherical Table', it is the same map, but double bitruncated. The Round Table has the shape of a hypersphere.\n\n",
    
    
    "Ceci est une représentation de la géométrie en quatre dimensions. Pouvez vous trouvé le Saint Graal au centre de la Table Ronde ?\n\n"
    "Dans le 'Chevalier de la Table 16-cases', chaque case a 8 cases adjacentes, qui correspondent à 8 points adjacents dans la grille en "
    "quatre dimensions. La Table Ronde a la forme de 16-cases.\n\n"
    "Dans le 'Chevalier de la Table 3-Sphérique', il s'agit de la même carte mais bitronquée. La Table Ronde a la forme d'une hypersphère.\n\n"
    )
S("let me understand how the coordinates work", "me fait comprendre comment les coordonnées marchent")
S("thanks, I need no hints (achievement)", "merci, je n'ai plus besoin d'astuces (accomplissement)")
S("more about this geometry...", "plus au sujet de cette géométrie...")
S("render a cut", "afficher une coupe")
S("cut level", "niveau de coupe")

S("Do not use compases.\n\n", "Ne pas utiliser de boussoles.\n\n")
S("Crystal Camelot is an octahedron in 'pure' 3D crystal geometry (and a similar polytope in other pure crystals), "
      "and an Euclidean ball in bitruncated/Goldberg crystals.", 
  "Le Camelot en cristal est un octaèdre en géométrie 3D cristalline 'pure', (et un polytope similaire dans d'autre "
  "cristaux purs) et une balle euclidienne dans un cristal bitronqué/Goldberg.")
S("puzzle/exploration mode", "mode puzzle/exploration")

// puzzle mode

S("exploration", "exploration")
S("puzzles", "puzzles")
S("other puzzles", "autres puzzles")
S("hyperbolic Minesweeper", "Démineur hyperbolique")
S("dual geometry puzzle", "puzzle en géométrie duale")
S("Uncover all cells which do not contain mines", "Découvrir toutes les cases qui ne contiennent pas de mines")
S("Cells to uncover: %1", "Cases à découvrir : %1")
S("You won in %1", "Vous avez gagné dans %1")
S("Dropped floors: %1/%2", "Sols tombés : %1/%2")

// shmup control
S("camera forward", "caméra en avant")
S("camera rotate X", "pivoter la caméra X")
S("camera rotate Y", "pivoter la caméra Y")
S("Configure dead zones", "Configurer les zones mortes") // for joysticks

// keyboard help
S("arrows", "flèches")
S("WASD keys", "touches WASD")
S("VI keys", "touches VI")

// sight range settings

S("sight range bonus", "bonus de portée de vue")
S("3D sight range for the fog effect", "portée de vue 3D pour l'effet brouillard")

S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "La portée de vue pour les géométries 3D est spécifiée en unités absolues. Cette valeur affecte aussi l'effet brouillard.\n\n"
  "Dans les géométries sphériques, la portée de vue 2π vous permettra de voir les choses derrière vous comme si elles étaient devant. "
  "et la portée de vue de π (ou plus) vous permettra de voir les choses au point antipodal comme si elles étaient très proche de vous.\n\n"
  "Dans les géométries hyperboliques, le nombre de cases à afficher dépend exponentiellement de la portée de vue. Plus de cases à "
  "dessiner réduit la performance.\n\n"
  "La portée de vue affecte le gameplay, et les monstres agissent comme s'ils étaient visibles. La génération de monstres prend cela en compte.")
  
S("max difference in Z coordinates", "différence maximale en coordonnées Z")
S("area-based range", "distance basée sur l'aire")
S("draw range based on", "distance de tracé basée sur")
S("size (no gen)", "taille (pas de génération)")
S("size", "taille")
S("sight range settings", "réglage du champ de vision")

// other settings

S("play music when out of focus", "jouer la musique quand la fenêtre est inactive")
S("floating bubbles: treasure thresholds", "bulles flottantes : seuil des trésors")
S("vector settings", "réglages des vecteurs")
S("Line width setting is only taken into account in OpenGL.", "Le réglage d'épaisseur de ligne n'est pris en compte qu'en OpenGL")
S("shots only", "seulement les clichés")
S("hint: press Alt while testing modes", "astuce : appuyer Alt en testant des modes")
S("no fill in neon mode", "pas de remplissage en mode néon")
S("(vsync disabled in VR)", "(vsync désactivé en VR)")
S("apply changes", "appliquer les changements")
S("width", "largeur")
S("camera movement speed", "vitesse de mouvement de la caméra")
S("camera rotation speed", "vitesse de rotation de la caméra")
S("send scores to Steam leaderboards", "envoyer les scores au tableaux de Steam")
S("crosshair size", "taille de la croix")
S("crosshair color", "couleur de la croix")

S("Must be less than %1°. Panini projection can be used to get higher values.", 
  "Doit être moins de %1°. La projection de Panini peut être utilisé pour  obtenir de plus grandes valeurs.")

S("Panini projection", "projection de Panini")
S(
  "The Panini projection is an alternative perspective projection "
  "which allows very wide field-of-view values.\n\n",
  
  "La projection de Panini est une perspective alternative de projection "
  "ce qui permet des valeurs de champ de vision très large.\n\n")

S("spherical perspective projection", "projection en perspective sphérique")
S(
  "Set to 1 to get stereographic projection, "
  "which allows very wide field-of-view values.\n\n",
  
  "Régler à 1 pour obtenir la projection stéréographique, "
  "ce qui permet des valeurs de champ de vision très large.\n\n"  
  )

S(
  "HyperRogue uses "
  "a quick implementation, so parameter values too close to 1 may "
  "be buggy (outside of raycasting); try e.g. 0.9 instead.", 
  
  
  "HyperRogue utilise une implémentation rapide, donc les valeurs de "
  "paramètres trop proches de 1 peuvent être bugués (hors du tracé de "
  "rayons) ; essayez 0.9 à la place.")

S("Berger sphere limit", "limite de la sphère de Berger")

S("Primitive-based rendering of Berger sphere is currently very slow and low quality. "
  "Here you can choose how many images to draw.", 
  "L'affichage de la sphère de Berger basé sur les primitives est actuellement lent et "
  "de mauvaise qualité. Ici vous pouvez choisir combien d'images dessiner.")
  
S(
  "Camera is placed %1 absolute units above a plane P in a three-dimensional "
  "world. Ground level is actually an equidistant surface, %2 absolute units "
  "below the plane P. The plane P (as well as the ground level or any "
  "other equidistant surface below it) is viewed at an angle of %3 "
  "(the tangent of the angle between the point in "
  "the center of your vision and a faraway location is 1/cosh(c) = %4).", 
  
  "La caméra est placée à %1 unités absolues au-dessus d'un plan P dans un monde "
  "tridimensionnel. Le niveau du sol est une surface équidistante, %2 unités absolues "
  "sous le plan P. Le plan P (ainsi que le niveau du sol ou n'importe quelle autre surface "
  "équidistante en-dessous) est vue avec un angle de %3 (la tangente d'un angle entre le "
  "centre de votre vision et un endroit distant est 1/cosh(c) = %4).")
  
S("cool fog effect", "effet brouillard froid")
S("borders", "frontières")
S("pattern colors", "motif des couleurs")
S("Galápagos shading", "ombres dans les Galápagos")
S("targetting ranged Orbs long-click only", "viser avec les Orbes à distance avec un long clic seulement")

S("0 to disable", "0 pour désactiver")
S("recently changed settings", "réglages récemment modifiés")
S("find a setting", "trouver un réglage")

// nonisotropic

S("geodesic table missing", "table géodésique manquante")
S(
  "If the 2D underlying manifold is bounded, the period should be a divisor of the 'rotation space' "
  "value (PSL(2,R)) times the Euler characteristics of the underlying manifold. "
  "For unbounded underlying manifold, any value should work (theoretically, "
  "the current implementation in HyperRogue is not perfect).\n\n"
  "We won't stop you from trying illegal numbers, but they won't work correctly.", 
        
  "Si la variété 2D sous-jacente est à bord, la période devrait être un diviseur de 'l'espace de rotation' "
  "(PSL(2,R)) fois la caractéristique d'Euler de la variété sous-jacente. Pour la variété sans bord "
  "sous-jacente, n'importe quelle valeur devrait marcher (en théorie, l'implémentation actuelle "
  "d'HyperRogue n'est pas parfait ).\n\n"
  "Nous ne vous empêcheront pas d'essayer des nombres non valides, mais ils ne marcheront pas correctement."
  )
S("SL(2,R)", "SL(2,R)")
S("universal cover", "couverture universelle")
S("maximum", "maximum")
S("works correctly so far", "marche correctement pour l'instant")

S("non-periodic", "non-périodique")
S("patched Chaos", "Chaos avec corrections")
S("excessive walls", "mur excessifs")
S("total chaos", "chaos total")
S("random-walk chaos", "chaos en marche aléatoire")
S("single land: ", "un seul pays : ")
S("single land", "un seul pays")
S("wall-less", "sans murs")

S("highlight stuff", "souligner des chose")
S("press Alt", "appuyer sur Alt")
S("highlight", "souligné")
S("super-highlight", "super-souligné")

S("shape editor", "éditeur des formes")
S("drawing tool", "outil de dessin")
S("land structure", "structure du terrain")
S("highlights & achievements", "soulignés et accomplissements")
S("classic game except hyperbolic", "jeu classique à part l'aspect hyperbolique")
S("can you find the Heptagonal Grail?", "pouvez vous trouvez le Graal heptagonal ?")
S("General Euclid", "Euclide général")
S("Worm of the World", "Vers du Monde")
S("Lovász Conjecture", "Conjecture de Lovász")
S("Knight of the 16-Cell Table", "Chevalier de la table 16-Cases")
S("Knight of the 3-Spherical Table", "Chevalier de la Table 3-Sphérique")
S("Emerald Mine in {5,3,4}", "Mine d'Émeraudes dans {5,3,4}")

// help

S("credits", "crédits")

S("\n\nAdditionally, the probabilities of generating terrain features are subtly changed in the following lands:", 
  "\n\nDe plus, les probabilités de générer des caractéristiques du paysage sont subtilement changées dans les pays suivants : ")
S(
    "\n\nOnce you collect a Bomberbird Egg, "
    "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
    "Collecting even more Eggs will increase the radius.",
    
    "\n\nUne fois que vous avez ramassé un Œuf d'Oiseau-Bombe, marcher sur une "
    "case sans mines adjacentes révèle aussi ces cases. Ramasser plus d’œufs augmente le rayon.")
    
S(" (shadow in %1)", " (ombre dans %le1 %1)")
S(", %1 the Knight", ", Le chevalier %1")
S("Hypersian Rug menu", "menu tapis Hypersan")
S("HyperRogue help", "aide d'HyperRogue")

// history mode

S("s = save to .png", "s = sauvegarder au format .png")
S("rendered band ($ID=segment, $DATE=date)", "affichage du ruban ($ID=segment, $DATE=date)")
S("animate from last recenter to precise current view", "animer depuis le dernier recentrage pour préciser la vue actuelle")
S("save the animation starting point", "sauvegarder le point de départ de l'animation'")
S("animate from the starting point", "animer à partir du point de départ")
S("reset animation", "réinitialiser l'animation")
S("include history when auto-rendering", "inclure l'historique lors du rendu automatique")
S("more options in hyperbolic geometry", "plus d'options en géométrie hyperbolique")
S("more options in band projections", "plus d'options en projection de rubans")
S("filename format to use ($ID=segment, $DATE=date)", "format de nom de fichier à utiliser ($ID=segment, $DATE=date)")
S("band segment", "segment de ruban")

// screenshot/animations

S("PNG", "PNG")
S("SVG", "SVG")
S("WRL", "WRL")
S("centering", "centrer")
S("You can pick the angle. Note: the direction the PC is facing matters.", 
  "Vous pouvez choisir l'angle. Note : l'orientation de l'ordinateur compte.")
S("rotate PC", "pivoter l'ordinateur")
S(
  "The value of 1 means that the period of this animation equals the period set in the animation menu. "
  "Larger values correspond to faster animations.", 
  "La valeur de 1 signifie que la période de cette animation est égale à celle réglable depuis le menu d'animation.")
S("shift", "shift")
S("movement angle", "angle du mouvement")
S("This is the period of the whole animation, though in some settings the animation can have a different period or be aperiodic. "
      "Changing the value will make the whole animation slower or faster.",
  "Ceci est la période totale de l'animation, néanmoins dans certains réglages les animations peuvent avoir une période différente ou être apériodiques. "
  "Changer la valeur rendra l'animation entière plus lente ou rapide.")
S("Least common multiple of the animation periods of all the game objects on screen, such as rotating items.", 
  "Multiple le moins courant des périodes d'animation de tous les objets de jeu affichés à l'écran, tels que les éléments en rotation.")
S("Möbius transformations", "transformation de Möbius")
S("circle spins", "tour du cercle")
S("circle radius", "rayon du cercle")
S("double spin", "double tour")
S("triple spin", "triple tour")
S("draw the circle", "tracer le cercle")
S("cycle length", "longueur de cycle")
S("full circle", "cercle entier")
S("Zebra period", "période du Zèbre")
S("Bolza period", "période de Bolza")
S("angle to screen normal", "angle à l'écran normal")
S("distance from rotation center", "distance au centre de rotation")
S("monster turns", "tours des monstres")
S("Number of turns to pass. Useful when simulating butterflies or cellular automata.", 
  "Nombre de tours à passer. Utile pour simuler des papillons et automates cellulaires.")
S("screen-relative rotation", "rotation relative à l'écran")
S("angle", "angle")
S("model-relative rotation", "rotation relative au modèle")
S("automatic move speed", "vitesse de déplacement automatique")
S("Move automatically without pressing any keys.", "Se déplacer automatiquement sans appuyer sur aucune touche")
S("synchronize", "synchroniser")
S("model rotation", "rotation du modèle")
S("3D rotation", "rotation 3D")
S("some parameters will only change in recorded animation", "certains paramètres ne changeront qu'en animation enregistrée")
S("record to sequence of image files", "enregistrer en une suite d'images")
S("record to video file", "enregistrer en fichier vidéo")
S("explore this animation", "explorer cette animation")

// raycaster

S("volumetric raycasting", "tracé de rayons volumétrique")
S("active", "actif")
S("intensity of random coloring", "intensité de la coloration aléatoire")
S("color randomly", "couleur aléatoire")
S("SMART", "SMART")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))\n", "formule de brillance : max(1-d/sightrange, s*exp(-d/r))\n")
S("unfortunately this honeycomb is too complex for the current implementation (%1>%2)", 
  "malheureusement ce nid d'abeille est trop complexe pour l'implémentation actuelle (%1>%2)")

// extra projections

S("geodesic", "géodésique")
S("quadrant coordinates", "coordonnées quadrant")
S("axial coordinates", "coordonnées axiales")
S("anti-axial coordinates", "coordonnées anti-axiales")
S("Werner projection", "projection de Werner")
S("Aitoff projection", "projection d'Aitoff")
S("Hammer projection", "projection de Hammer")
S("loximuthal projection", "projection loximutale")
S("Miller projection", "projection de Miller")
S("Gall stereographic", "projection stéréographique de Gall")
S("Winkel tripel", "projection de Winkel-Tripel")
S("Poor man's square", "la place du pauvre")
S("Craig retroazimuthal", "projection de Craig rétroazimutale")
S("Littrow retroazimuthal", "projection de Littrow rétroazimutale")
S("Hammer retroazimuthal", "projection de Hammer rétroazimutale")
S("three-point equidistant", "projection équidistante à trois-points")
S("stereographic projection [VR]", "projection stéréographique [VR]")

// projection settings

S("simple model: projection", "modèle simple : projection")
S("simple model: perspective", "modèle simple : perspective")
S("projection type", "type de projection")
S("edit VR or non-VR settings", "changer les réglages VR ou non-VR")
S("rotational or Heisenberg", "modèle rotationnel ou Heisenberg")
S("1 = Heisenberg, 0 = rotational", "1 = Heisenberg, 0 = rotationnel")
S("use atan to make it finite", "utiliser atan() pour le rendre fini")
S("VR: rotate the 3D model", "VR: pivoter le modèle 3D")
S("VR: shift the 3D model", "VR: décaler le modèle 3D")
S("VR: scale the 3D model", "VR: mettre le modèle 3D à l'échelle")
S("match the period", "faire correspondre la période")
S("(zoomed out)", "(dézoomer)")
S("zoom 2x", "zoom 2x")
S("zoom 1x", "zoom 1x")
S("zoom 0.5x", "zoom 0.5x")
S("cylinder/donut view", "vue cylindre/donut")
S("first-person perspective", "perspective à la première personne (FPP)")
S("toggle full 3D graphics", "activer les graphiques intégralement 3D")
S("advanced projections", "projection avancée")

// map editor
S("F5 = clear", "F5 = effacer")
S("Are you sure you want to clear the map?", "Êtes vous sûr de vouloir effacer la carte ?")
S("select area to copy", "sélectionner la zone à copier")
S("statue", "statue")
S("c = reuse", "c = réutiliser")
S("d = delete", "d = effacer")
S("p = grid color", "p = couleur de la grille")
S("f = fill", "f = remplir")
S("T = text", "T = texte")
S("e = erase", "e = effacer")
S("z = spheres", "z = sphère")
S("z-level", "niveau z")

S("Failed to load pictures from %1", "Échec du chargement des images depuis %1")
S("Pictures loaded from %1", "Images chargées depuis %1")
S("Failed to save pictures to %1", "Échec de la sauvegarde des images vers %1")
S("Pictures saved to %1", "Images sauvegardées vers %1")
S("Hint: use F7 to edit floor under the player", "Astuce : utiliser F7 pour éditer le sol sous le joueur")
S("Are you sure you want to restart? This will let you draw on a blank screen.", "Êtes vous sûrs de vouloir redémarrer ? Ceci vous permettra de dessiner sur un écran blanc.")
S("brush size", "taille de la brosse")

// checking configurables
S("Aitoff parameter", "paramètre Aitoff")
S("The Aitoff projection is obtained by multiplying the longitude by 1/2, using azimuthal equidistant projection, and then dividing X by 1/2. Hammer projection is similar but equi-area projection is used instead. Here you can change this parameter.", 
  "La projection Aitoff est obtenue en multipliant la longitude par 1/2, en utilisant la projection azimutale équidistante, et puis en divisant X par 1/2. La projection de Hammer est similaire mais la projection équiaire est utilisée à la place. Vous pouvez changer ce paramètre ici.")
S("any spiral multiplier", "n'importe quel multiplieur de spirale")
S("Collignon parameter", "paramètre Collignon")
S("Collignon reflect", "reflet Collignon")
S("ETS parameter", "paramètre ETS")
S("eye level", "niveau des yeux")
S("off", "off")
S("spam", "spam")
S("normal monsters", "monstres normaux")
S("high-threat monsters only", "seulement les monstres les plus menaçants")
S("faraway highlight color", "couleur d'accent lointaine")
S("0 = monster color, 100 = red-light oscillation", "0 = couleur des monstres, 100 = oscillation rouge-vert")
S("font size", "taille de police")
S("use specific fullscreen resolution", "utiliser la résolution spécifique au plein écran")
S("fullscreen resolution to use (X)", "résolution à utiliser en plein écran (X)")
S("fullscreen resolution to use (Y)", "résolution à utiliser en plein écran (Y)")
S("hyperbolic|spherical|Euclidean", "hyperbolique|sphérique|Euclidien")
S("show flat", "afficher l'image aplatie")
S("loximuthal parameter", "paramètre pour la projection loximutale")
S("Loximuthal is similar to azimuthal equidistant, but based on loxodromes (lines of constant geographic direction) rather than geodesics. The loximuthal projection maps (the shortest) loxodromes to straight lines of the same length, going through the starting point. This setting changes the latitude of the starting point.\n\nIn retroazimuthal projections, a point is drawn at such a point that the azimuth *from* that point to the chosen central point is correct. For example, if you should move east, the point is drawn to the right. This parameter is the latitude of the central point.\n\n(In hyperbolic geometry directions are assigned according to the Lobachevsky coordinates.)", 
  "La projection loximutale est similaire à l'azimutale équidistante, mais basée sur les loxodromes (lignes de direction géographique constante) plutôt que les géodésiques. "
  "La projection loximutale fait correspondre les loxodromes les plus courts aux lignes droites de la même longueur, traversant le même point de départ. Ce réglage change la latitude du point de départ.\n\n"
  "Dans les projections rétroazimutales, un point est dessiné de telle sorte que l’azimut *provenant de* ce point au point central choisi est correct. "
  "Par exemple, si vous devriez aller à l'est le point est dessiner à droite. Le paramètre est la latitude du point central.\n\n"
  "(En géométrie hyperbolique les directions sont attribuées en suivant les coordonnées de Lobachevsky.")
 
S("onscreen keyboard", "clavier à l'écran")
S("projection distance|Gans Klein Poincare orthographic stereographic", "distance de projection|Gans Klein Poincaré orthographique stéréographique")
S("set relative font size", "régler la taille relative de la police")
S("resizable window", "fenêtre redimensionnable")
S("right spiral multiplier", "multiplieur de la spirale à droite")
S("fisheye parameter", "paramètre œil de poisson")
S("two-point parameter", "paramètre pour le modèle à deux point")
S("In two-point-based models, this parameter gives the distance from each of the two points to the center.", 
  "Dans le modèle basé sur deux points, ce paramètre donne la distance de chacun des deux points par rapport au centre.")
S("Winkel Tripel mixing", "mélange Winkel Tripel")
S("The Winkel Tripel projection is the average of Aitoff projection and equirectangular projection. Here you can change the proportion.", 
  "La projection Winkel-Tripel est la moyenne de la projection d'Aitoff et équirectangulaire. Ici vous pouvez changer la proportion.")
S("sphere spiral multiplier", "multiplicateur de la spirale de la sphère")
S("spiral cone", "cône spiralant")
S("variation|dual|bitruncated", "variation|duel|bitronqué")
S("screen size percentage to use (X)", "pourcentage de la taille d'écran à utiliser (X)")
S("screen size percentage to use (Y)", "pourcentage de la taille d'écran à utiliser (Y)")
S("specify relative window size", "spécifie la taille relative de la fenêtre")
S("window resolution to use (X)", "résolution de la fenêtre (X)")
S("window resolution to use (Y)", "résolution de la fenêtre (Y)")

// ...

// fixed text

S("select the starting land", "sélectionner le pays de départ")

// missing text in animation

S("cells to go", "cases à déplacer")

// missing translations for Hardcore, and the new Casual mode

S("casual mode", "mode détente")
S("Switching casual allowed only before saving the game. Do you want to restart?", 
  "Le passage au mode détente est permis seulement avant de sauvegarder le jeu. Voulez vous redémarrez le jeu ?")
S("You are in the casual mode! Achievements are disabled.", 
  "Vous êtes en mode détente ! Les Accomplissements sont désactivés.")
S("Collect an Orb of Safety to save a checkpoint.", 
  "Collecter une Orbe de Sécurité pour sauvegarder un point de retour.")
S("load (%1 turns passed)", "charger (tours passés : %1)")

// missing texts in searching

S("press letters to search", "appuyer les lettres à chercher")
S("matching items", "objets correspondants : ")

// YASC codes

S("YASC codes", "codes YASC")
S(" YASC code: ", "code YASC : ")
S("YASC codes: Sides-Entity-Restrict-Threat-Wall", "codes YASC : Sides-Entity-Restrict-Threat-Wall")

// Dice Reserve

N("Dice Reserve", GEN_F, "Réserve de Dés", "Réserves de Dés", "dans la Réserve de Dés")
S("This land is full of sentient dice. Do not disturb them!",
  "Ce pays est plein de dés intelligents. Ne les dérangez pas !")

N4("Crystal Die", GEN_M, "Dé de cristal", "Dés de cristal")
S("A nice souvenir from the Dice Reserve. Make sure to collect the whole set!",
  "Un joli souvenir de la Réserve de Dés. Assurez vous d'avoir toute la collection !")

N4("Unhappy Die", GEN_M, "Dé malheureux", "Dés malheureux")
S("Sentient dice like to be in a position such that their highest number is on top. "
  "Unfortunately, someone has rolled this one into a wrong position, and did not fix this. "
  "It will reward you if you roll it so that the highest number is on top again!",

  "Les dés intelligents aiment être dans une position telle que leur plus grand nombre est au sommet. "
  "Manque de chance, quelqu'un a roulé celui-ci dans la mauvaise position, et n'a pas arrangé ça. "
  "Il vous récompensera si vous le roulez avec le plus haut nombre au sommet à nouveau !")

N4("Happy Die", GEN_M, "Dé heureux", "Dés heureux")

S(
  "A happy sentient die. Dice are happy when they are in their correct position "
  "(the highest number on the top); happy dice with one roll from their best position are sometimes found too."
  "Other positions are much less convenient for them and expose their "
  "weak spots. You can roll Happy Dice, but it may become angry!",

  "Un dé intelligent heureux. Les dés sont heureux quand ils sont dans leur position correcte. "
  "(le plus haut nombre au sommet) ; des dés heureux à un tour de leur meilleure position peuvent être trouvés aussi. "
  "D'autres positions sont moins pratiques pour eux et exposent leurs points faibles. "
  "Vous pouvez rouler un Dé heureux, mais il peut s'énerver !")

N4("Animated Die", GEN_M, "Dé animé", "Dés animés")

S("When sentient dice are too long in an incorrect position, they start to move on their own, "
  "and attack everyone. You can still convince Animated Dice of your good intentions by "
  "rolling them into a position such that the highest number is on top. "
  "If you do, the die will stop moving and (if it happens in the Dice Reserve) you will be rewarded. "
  "Other rolls and attacks are not allowed.",
  
  "Quand les dés intelligents sont trop longtemps dans une position incorrecte, ils commencent à bouger d'eux-mêmes, "
  "et attaquent tout le monde. Vous pouvez encore convaincre le Dé animé de vos bonnes intentions en les roulant "
  "dans une position où leur plus grand nombre est au sommet. Si vous le faites, le dé s'arrêtera de bouger et "
  "(si cela arrive dans la Réserve de Dés) vous serez récompensés. Les autres roulements et attaques ne sont pas"
  "permises."
  )

N4("Angry Die", GEN_M, "Dé énervé", "Dés énervés")

S("You have made a die unhappy. Taste the revenge! This one won't forgive you, no matter what you do.",
  "Vous avez rendu un dé malheureux. Goûtez à sa vengeance ! Celui-là ne vous pardonne pas, quoi que vous fassiez.")

S("The die is now happy, and you are rewarded!", "Ce dé est maintenant heureux, et vous êtes récompensés.")
S("You have made a Happy Die angry!", "Vous avez énervé un Dé heureux !")
S("You can only push this die if the highest number would be on the top!", 
  "Vous pouvez seulement pousser ce dé si le plus haut nombre sera au sommet !")
S("This die is really angry at you!", "Ce dé est vraiment énervé contre vous !")
S("The die is now happy, but won't reward you outside of the Dice Reserve!", "Ce dé est maintenant heureux, mais ne vous récompensera pas hors de la réserve de dés!")

// Cursed Canyon
//===============

N("Cursed Canyon", GEN_M, "Canyon Maudit", "Canyons Maudits", "dans le Canyon Maudit")
S("This canyon is full of curses! Curses are like Orbs, but grant negative effects.",
  "Ce canyon est plein de malédictions ! Les malédictions sont comme les Orbes, mais donnent des effets négatifs.")
  
N4("Canyon Hag", GEN_F, "La Sorcière du Canyon", "Les Sorcières du Canyon")
S("A Canyon Hag can curse you from afar, if standing on top of a Curse and close enough.",
  "Une Sorcière du Canyon peut vous maudire de loin, si elles sont au sommet d'une Malédiction et assez près.")
S("%The1 curses you with %the2!", "%Le1 vous maudit avec %le2!")

N4("Capon Stone", GEN_F, "Pierre de Chapon", "Pierres de Chapon")
S("A lapis alectorius, alectoria or capon stone is a non-precious stone found in the gizzard of capons (young, castrated roosters). In magic it is believed to be an effective amulet, granting the wearer a heightened sense of courage and boldness. These properties are unproven. [Wikipedia]",
  "Un lapis alectorius, alectoria ou pierre de chapon est une pierre non précieuse qui se trouve dans le gésier des chapons (jeunes poulets castrés). Dans le domaine de la magie elle est censée être une amulette  efficace, accordant au porteur un sens accru du courage et de l'audace. Ces propriétés ne sont pas prouvées. [Wikipédia]")

#define Curse(E, P) N4("Curse of " E, GEN_F, "Malédiction de" P, "Malédictions de " P)
Curse("Weakness", "Faiblesse")
S("This Curse weakens your normal attack, which will only stun monsters. For monsters which are normally only stunned anyway, they don't push the victim away. You are also unable to cut trees or fight Ivy.",
  "Cette Malédiction affaiblit votre attaque normale, qui ne fait qu’assommer les monstres. Pour les monstres qui ne sont qu’assommés normalement, ils ne projettent plus la victime au loin. Vous êtes aussi incapables d'abattre des arbres et de combattre le Lierre.")

Curse("Draining", "Épuisement")
S("Drains the powers of all your orbs (and curses) by 2 charges per turn.",
  "Draine le pouvoir de toutes vos orbes (et malédictions) avec 2 charges par tour.")

Curse("Water", "Eau")
S("Makes you leave a trail of shallow water. You also become afraid of water. You are unable to step into water, including shallow water and boats on water. "
  "On the good side, you are protected from fire.",
  
  "Vous fait laisser une traînée d'eau peu profonde. Vous développez une phobie de l'eau. Vous êtes incapable d'aller dans l'eau, même peu profonde et les "
  "bateaux sur l'eau. Le bon côté c'est que vous êtes protégé%e0 du feu.")
  
Curse("Fatigue", "Fatigue")
S("With this Curse you cannot move too quickly. Every move increases your fatigue by 1; fatigue cannot go over 10. "
  "Resting reduces your fatigue by 5.",
  "Avec cette Malédiction vous ne pouvez pas vous déplacer trop vite. Chaque mouvement augmente votre fatigue de 1 ; "
  "la fatigue ne peut pas dépasser 10. Se reposer réduit votre fatigue de 5.")
N4("fatigue", GEN_F, "fatigue", "fatigues")
S("See the Curse of Fatigue.", "Vous voyez la Malédiction de Fatigue.")
S("You are too fatigued!", "Vous êtes trop fatigué%e0 !")

Curse("Repulsion", "Répulsion")
S("If you have this Curse, all the items on adjacent cells will try to escape from you.",
  "Si vous avez cette Malédiction, tout les objets sur les cases adjacentes essaient de vous échapper.")

Curse("Gluttony", "Gloutonnerie")
S("The first item you pick up is consumed without any effect.",
  "Le premier objet que vous ramassez est consumé sans aucun effet.")
S("%The1 is consumed!", "%Le1 est consumé !")

Orb("Purity", "Pureté")
S("Reverses all the curses. When found far away from the Cursed Canyon, you gain low amounts of various reversed curse powers.",
  "Lève toutes les malédictions. Lorsque trouvé loin du Canyon Maudit, vous gagnez une basse quantité des pouvoirs inversés des malédictions.")
#undef Curse
#undef Orb

// chapter names of the Guided Tour
S("Hyperbolic shapes", "Formes Hyperboliques")
S("Projections of hyperbolic space", "Projections de l'espace hyperbolique")
S("Procedural generation", "Génération procédurale")

// newbie-friendly Safety

S("Find an Orb of Safety to save your game",
  "Trouvez une Orbe de Sécurité pour sauvegarder votre jeu")

S("how to find an Orb of Safety?", "comment trouver une Orbe de Sécurité ?")

S(
  "To save the game you need an Orb of Safety.\n\n"
  "Orbs of Safety appear:\n\n"
  "* in the Crossroads and the Land of Eternal Motion, after you collect %1 Phoenix Feathers in the Land of Eternal Motion\n\n"
  "* in the Ocean after you unlock it (%2 treasures)\n\n"
  "* in the Prairie after you unlock it (%3 treasures)\n\n",
  
  "Pour sauvegarder un jeu vous avez besoin d'une Orbe de Sécurité.\n\n"
  "Les Orbes de Sécurité apparaissent :\n\n"
  "* dans les Croisements et le Pays du Mouvement Perpétuel, après que vous ayez collecté %1 Plumes de Phoenix dans le Pays du Mouvement Perpétuel\n\n"
  "* dans l''Océan après l'avoir débloqué (%2 trésors)\n\n"
  "* dans la Prairie après l'avoir débloqué (%3 trésors)\n\n"
  )

N("Palace Quest", GEN_F, "Quête du Palais", "Quêtes du Palais", "dans la Quête du Palais")

N4("Pike", GEN_M, "Brochet", "Brochets")

