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
N("Sand Worm Tail", GEN_F, "Queue du ver des sables", "Queues du ver des sables", "Queue du ver des sables")
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
N("Tentacle (withdrawing)", GEN_M, "Tentacule (en recul)", "Tentacules (en recul)", "Tentacule (en retrait)")
N("Cultist", GEN_M, "Cultiste", "Cultistes", "Cultiste")
N("Fire Cultist", GEN_M, "Cultiste du feu", "Cultistes du feu", "Cultiste du feu")
N("Greater Demon", GEN_M, "Démon supérieur", "Démons supérieurs", "Démon supérieur")
N("Lesser Demon", GEN_M, "Démon inférieur", "Démons inférieurs", "Démon inférieur")
N("Ice Shark", GEN_M, "Requin des glaces", "Requins des glaces", "Requin des glaces")
N("Running Dog", GEN_M, "Chien courant", "Chiens courants", "Chien courant")
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
N("Demon Daisy", GEN_F, "Marguerite démoniaque", "Marguerites démoniaque", "Marguerite démoniaque")
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
Orb("Storms", "des Tempêtes")
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
N("red slime", GEN_M, "slime rouge", "slimes rouges", "slime rouge")
N("blue slime", GEN_M, "slime bleu", "slimes bleus", "slime bleu")
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
N("Cocytus", GEN_M, "Cocytus", "Cocytus", "dans le Cocytus")
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
S("You would be killed by %the1!", "%Le1 vous tuerais !")
S("%The1 would get you!", "%Le1 vous attraperais !")
S("%The1 destroys %the2!", "%Le1 détruit %le2 !")
S("%The1 eats %the2!", "%Le1 mange %le2 !")
S("The ivy destroys %the1!", "Le lierre détruit %le1 !")
S("%The1 claws %the2!", "%Le1 griffe %le2 !")
S("%The1 scares %the2!", "%Le1 effraie %le2 !")
S("%The1 melts away!", "%Le1 fond !")
S("%The1 stabs %the2.", "%Le1 poignarde %le2.")
S("You stab %the1.", "Vous poignardez %le1.")
S("You cannot attack %the1 directly!", "Vous ne pouvez pas attaquer %le1 directement !")
S("Stab them by walking around them.", "Poignardez les en marchant autour d'eux.")
S("You feel more experienced in demon fighting!", "Vous vous sentez plus expérimenté en combat contre les démons !")
S("Cthulhu withdraws his tentacle!", "Cthulhu retire son tentacule !")
S("The sandworm explodes in a cloud of Spice!", "Le ver des sables explose dans un nuage d'épice !")
S("%The1 is confused!", "%Le1 est confus%e1.")
S("%The1 raises some undead!", "%Le1 relève des morts !")
S("%The1 throws fire at you!", "%Le1 vous jette du feu dessus !")
S("%The1 shows you two fingers.", "%Le1 vous montre deux doigts.")
S("You wonder what does it mean?", "Vous vous demandez, ce que cela peut bien vouloir dire.")
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
S("Ice below you is melting! RUN!", "La glace en-dessous de vous fonds ! COUREZ !")
S("This spot will be burning soon! RUN!", "Cet endroit sera bientôt en feu ! COUREZ !")
S("The floor has collapsed! RUN!", "Le sol s'est effondré ! COUREZ !")
S("You need to find the right Key to unlock this Orb of Yendor!",
  "Vous devez trouver la bonne clé pour débloquer l'Orbe de Yendor !")
S("You fall into a wormhole!", "Vous tombez dans un trou de ver !")
S("You activate %the1.", "Vous activez %le1.")
S("No room to push %the1.", "Aucun espace pour pousser %le1.")
S("You push %the1.", "Vous poussez %le1.")
S("You start chopping down the tree.", "Vous commencez à abattre l'arbre.")
S("You chop down the tree.", "Vous abattez l'arbre.")
S("You cannot attack Sandworms directly!", "Vous ne pouvez pas attaquer les vers des sables directement !")
S("You cannot attack Tentacles directly!", "Vous ne pouvez pas attaquer les tentacules directement !")
S("You cannot defeat the Shadow!", "Vous ne pouvez pas vaincre l'Ombre !")
S("You cannot defeat the Greater Demon yet!", "Vous ne pouvez pas encore vaincre le Démon Supérieur !")
S("That was easy, but groups could be dangerous.", "C'était facile, mais des groupes peuvent être dangereux.")
S("Good to know that your fighting skills serve you well in this strange world.", "Bon à savoir, que vos compétences de combat vous aident dans ce monde étrange.")
S("You wonder where all these monsters go, after their death...", "Vous vous demandez où vont tous ces monstres, après leur mort...")
S("You feel that the souls of slain enemies pull you to the Graveyard...", "Vous sentez les âmes de vos ennemis tombés vous entraîner vers le Cimetière...")
S("Wrong color!", "Mauvaise couleur !")
S("You cannot move through %the1!", "Vous ne pouvez pas passer à travers %le1 !")
S("%The1 would kill you there!", "%Le1 vous tuerais ici !")
S("Wow! %1! This trip should be worth it!", "Waouh, %un1! Ce voyage devrait valoir le coup !")
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
S("To access Hell, collect %1 treasures each of %2 kinds...", "Pour atteindre l'Enfer, rassemblez %1 trésors de chaque %2 sortes...")
S("Abandon all hope, the gates of Hell are opened!", "Abandonnez tout espoir, les portes de l'Enfer sont ouvertes !")
S("And the Orbs of Yendor await!", "Et les Orbes de Yendor vous attendent !")
S("You switch places with %the1.", "Vous échangez votre place avec %le1.")
S("You rejoin %the1.", "Vous rejoignez %le1.")
S("The mirror shatters!", "Le Mirroir éclate !")
S("The cloud turns into a bunch of images!", "Les nuages se transforment en une multitude d'images !")
S("The slime reacts with %the1!", "Le slime réagit avec %le1 !")
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
S("You will now start your games in %1", "Vous allez maintenant commencer le jeu %a1")
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
  "Conception du jeu, programmation, texte et graphiques par Zeno Rogue \n\n")
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
S("no axes", "pas de axes")
S("auto", "auto")
S("light", "facile")
S("heavy", "difficile")
S("The axes help with keyboard movement", "Les axes aident avec le mouvement de clavier")
S("Config file: %1", "Fichier de config : %1")
S("joystick mode: automatic (release the joystick to move)", "Joystick : automatique (relâcher le joystick pour bouger)")
S("joystick mode: manual (press a button to move)", "Joystick : manuel (appuyer sur un bouton pour bouger)")
S("Reduce the framerate limit to conserve CPU energy", "Réduire la limite du framerate pour économiser l'énergie du CPU")
S("Press F1 or right click for help", "Appuyer sur F1 ou cliquer droit pour l'aide")
S("No info about this...", "Pas d'info à ce sujet...")
S("Press Enter or F10 to save", "Appuyer sur Entrer ou F10 pour sauvegarder")
S("Press Enter or F10 to quit", "Appuyer sur Entrer ou F10 pour quitter le jeu")
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
S("Collect at least %1 treasures in each of %2 types to access Hell", "Rassemblez au moins %1 différents trésors de chacun des %2 types pour accéder à l'Enfer")
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
S("This might be useful for Android devices with limited memory.", "Cela pourrais s'avérer utile pour les mobiles sous Android avec une mémoire limitée.")
S("You can press 'g' or click them in the list to drop a Dead Orb.", "Vous pouvez appuyer sur 'g' ou cliquer dessus dans la liste pour lâcher un Orbe mort.")
S("frames per second", "FPS")
S("monsters killed: %1", "Kills: %1")
S("Drawing %1 (layer %2), F1 for help", "Tracé %1 (couche %2), F1 pour l'aide")
S("hepta floor", "sol hepatgonal")
S("hexa floor", "sol hexagonal")
S("character", "personnage")
S("ESC for menu/quest", "ESC pour le Menu/la Quête")
S("vector graphics editor", "vecteurs graphiques éditeur")
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
S("Use arrow keys to choose the teleport location.", "Utilisez les touches flèches pour choisir le lieu de téléportation.")
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
  "Les Alchimistes pruduisent des potions magiques à partir de bassins de slime "
  "rouge ou bleu. Vous pouvez traverser ces bassins, mais pas passer d'un bassin "
  "bleu vers un rouge, et inversement. Les bassins contenant des objets sont sans "
  "couleur, et changent de couleur vers leur couleur passée lorsque l'objet est "
  "ramassé. Les slimes gardent également leur couleur, mais explosent lorsque tués "
  "ce qui qui détruit les items et change la couleur des slimes et des bassins de slime autour.")

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
  "Ces caves contiennent des murs d'une certaine mainère vivants. Après chaque tour, "
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
  "Une boisson merveilleuse, obtenue apparemment en mélangeant du slime rouge et bleu. Vous vous sentez "
  "en bien meilleure forme après l'avoir bue, mais qu'un seul coup d'un monstre peut encore vous tuer.")

S("A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
  "in the Land of Mirrors leave these.",
  "Un morceau de mirroir magique, ou de nuage de mirage, qui peut être utilisé pour des usages magiques. "
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

S("Affects looks and grammar", "Apparence et grammaire")

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
S("Retrieving scores from Google Leaderboards...", "En train de récupérer les scores de Google Leaderboards...")
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
  "Faites le se frapper lui-même en vous éloignat de lui.")

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
  "Cet orbe permet de passer à travers toutes sortes de murs et cavités.")

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
N("rubble", GEN_M | GENF_PLURALONLY, "gravats", "gravats", "gravat")
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
  "Arriverez-vous au coeur d'un nid pour récolter la précieuse Gelée Royale ?")

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
  "compter les Orbes de Vie). Les sorcières et les golems ne vous porsuivent pas dans les autres Pays."
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
  "Les Illusions sont attaquées comme vous, les Marteleurs et vos alliésS.")

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
  "Un tapis hypersan ! Il n'ira pas bien avec votre sol euclidien plat, mais quelle importance ?")

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

S("You stun %the1.", "Vous assomez %le1.")
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
S("only native Orbs allowed in %the1", "Seules les Orbes locals sont autorisés %a1")
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
S("Collect four different Elemental Shards!", "Rassemblez quatre Fragments Elementaux différents !")
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
S("Saved the band image as: ", "Sauvegarder l'image bande en tant que : ")
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
S("band", "bande")
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
S("render bands automatically", "afficher les bandes automatiquement")
S("band width", "largeur de bande")
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
S("Revived!", "Rescussité !")

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


#undef Orb

S("score: %1", "points : %1")
S("kills: %1", "tués : %1")
