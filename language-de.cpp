// This translation file is encoded with UTF-8

// Nouns.

// For each noun in English, provide:

// 1) the type (usually gender) of the noun. For example, in Polish each noun can be:

// masculine living (GEN_M),
// masculine object (GEN_O), 
// feminine (GEN_F), or 
// neuter (GEN_N).

// 2) all the forms required by HyperRogue. For example, in Polish the following are
// given:
// The following forms are given:
// nominative (referred to as %1), 
// nominative plural (%P1), 
// accusative (%a1),
// ablative (%abl1) (for land names, locative "in/on..." is given instead of ablative). 

// Feel free to add more or less forms or types if needed. Note that you don't
// have to provide all the forms in your language, only those used by HyperRogue
// (in Polish just 4 of 14 forms are used, and this is probably similar in other
// languages).

// Note from Arri:

// Each noun has its singular form (%1), plural form (%P1), and accusative (%a1) given.
// The types of nouns are GEN_M (masculine), GEN_F (feminine), and GEN_N (neuter).
// For land names, "in/on" is given instead of accusative.

// %Der1 means  Der  if GEN_M			%der1 is the same, but lowercase (der, die, das)
//				Die  if GEN_F
//				Das  if GEN_N

// %den1 means  den  if GEN_M
//				die  if GEN_F
// 				das  if GEN_N

// monsters
N("Icewolf", GEN_M, "Eiswolf", "Eiswölfe", "Eiswolf")
N("Yeti", GEN_M, "Yeti", "Yetis", "Yeti")
N("Ranger",  GEN_M, "Wildhüter", "Wildhüter", "Wildhüter")
N("Rock Troll", GEN_M, "Höhlentroll", "Höhlentrolle", "Höhlentroll")
N("Goblin", GEN_M, "Höhlen-Kobold", "Höhlen-Kobolde", "Höhlen-Kobold")
N("Sand Worm", GEN_M, "Sandwurm-Kopf", "Sandwurm-Köpfe", "Sandwurm-Kopf")
N("Sand Worm Tail", GEN_M, "Sandwurm-Schwanz", "Sandwurm-Schwänze", "Sandwurm-Schwanz")
N("Sand Worm W", GEN_M, "Sandwurm-Kopf W", "Sandwurm-Köpfe W", "Sandwurm-Kopf W")
//N("Sand Worm M", GEN_M, "Sandwurm-Kopf M", "Sandwurm-Köpfe M", "Sandwurm-Kopf M")
N("Desert Man", GEN_M, "Wüstenbewohner", "Wüstenbewohner", "Wüstenbewohner")
N("Ivy Root", GEN_F, "Efeu-Wurzel", "Efeu-Wurzeln", "Efeu-Wurzel")
N("Active Ivy", GEN_M, "Aktiver Efeu", "Aktive Efeu", "Aktiven Efeu")
N("Ivy Branch", GEN_M, "Efeu-Zweig", "Efeu-Zweige", "Efeu-Zweig")
N("Dormant Ivy", GEN_M, "Ruhender Efeu", "Ruhende Efeu", "Ruhenden Efeu")
N("Ivy N", GEN_M, "Efeu N", "Efeu N", "Efeu N")
N("Ivy D", GEN_M, " Efeu D", "Efeu D", "Efeu D")
N("Giant Ape", GEN_M, "Riesiger Menschenaffe", "Riesige Menschenaffen", "Riesigen Menschenaffen")
N("Slime Beast", GEN_M, "Schleim-Monster", "Schleim-Monster", "Schleim-Monster")
N("Mirror Image", GEN_N, "Spiegelbild", "Spiegelbilder", "Spiegelbild")
N("Mirage", GEN_N, "Trugbild", "Trugbilder", "Trugbild")
N("Golem", GEN_M, "Golem", "Golems", "Golem")
N("Eagle", GEN_M, "Adler", "Adler", "Adler")
N("Seep", GEN_M, "Sicker", "Sicker", "Sicker")
N("Zombie", GEN_M, "Zombie", "Zombies", "Zombie")
N("Ghost", GEN_M, "Geist", "Geister", "Geist")
N("Necromancer", GEN_M, "Totenbeschwörer", "Totenbeschwörer", "Totenbeschwörer")
N("Shadow", GEN_M, "Schatten", "Schatten", "Schatten")
N("Tentacle", GEN_M, "Tentakel", "Tentakel", "Tentakel")
N("Tentacle Tail", GEN_M, "Tentakel", "Tentakel", "Tentakel")
N("Tentacle W", GEN_M, " Tentakel W", " Tentakel W", " Tentakel W")
//N("Tentacle M", GEN_M, " Tentakel M", " Tentakel M", " Tentakel M")
N("Cultist", GEN_M, "Okkultist", "Okkultisten", "Okkultist")
N("Fire Cultist", GEN_M, "Feuer-Okkultist", "Feuer-Okkultisten", "Feuer-Okkultist")
N("Greater Demon", GEN_M, "Großer Dämon", "Großen Dämonen", "Großen Dämon")
N("Lesser Demon", GEN_M, "Kleiner Dämon", "Kleine Dämonen", "Kleinen Dämon")
N("Ice Shark", GEN_M, "Eishai", "Eishaie", "Eishai")
N("Running Dog", GEN_M, "Rennender Hund", "Rennende Hunde", "Rennenden Hund")
N("Demon Shark", GEN_M, "Dämonenhai", "Dämonenhaie", "Dämonenhai")
N("Fire Fairy", GEN_F, "Feuerfee", "Feuerfeen", "Feuerfee")
N("Crystal Sage", GEN_M, "Kristall-Weiser", "Kristall-Weise", "Kristall-Weisen")
N("Hedgehog Warrior", GEN_M, "Igelkrieger", "Igelkrieger", "Igelkrieger")
N("Tentacle (withdrawing)", GEN_M, "Zurückziehender Tentakel", "Zurückziehende Tentakel", "Zurückziehenden Tentakel")
// items

N("Ice Diamond", GEN_M, "Eisdiamant", "Eisdiamanten", "Eisdiamanten")
N("Gold", GEN_N, "Gold", "Gold", "Gold")
N("Spice", GEN_N, "Gewürz", "Gewürze", "Gewürz")
N("Ruby", GEN_M, "Rubin", "Rubine", "Rubin")
N("Elixir of Life", GEN_N, "Elixier des Lebens", "Elixiere des Lebens", "Elixier des Lebens")
N("Shard", GEN_F, "Scherbe", "Scherben", "Scherbe")
N("Necromancer's Totem", GEN_N, "Totem der Toten", "Totems der Toten", "Totem der Toten")
N("Demon Daisy", GEN_N, "Dämonenblümchen", "Dämonenblümchen", "Dämonenblümchen")
N("Statue of Cthulhu", GEN_F, "Statue von Cthulhu", "Statuen von Cthulhu", "Statue von Cthulhu")
N("Phoenix Feather", GEN_F, "Phönix-Feder", "Phönix-Federn", "Phönix-Feder")
N("Ice Sapphire", GEN_M, "Eis-Saphir", "Eis-Saphire", "Eis-Saphir")
N("Hyperstone", GEN_M, "Hyperstein", "Hypersteine", "Hyperstein")
N("Key", GEN_M, "Schlüssel", "Schlüssel", "Schlüssel")
N("Dead Orb", GEN_F, "Tote Perle", "Tote Perlen", "Tote Perle")
N("Fern Flower", GEN_F, "Farnblüte", "Farnblüten", "Farnblüte")
N("Orb of Yendor", GEN_F, "Perle von Yendor", "Perlen von Yendor", "Perle von Yendor")
// orbs: we are using a macro here
#define Orb(E, P) N("Orb of "E, GEN_F, "Perle "P, "Perlen "P, "Perle "P)
Orb("Storms", "des Donners")
Orb("Flash", "des Blitzes")
Orb("Winter", "des Winters")
Orb("Speed", "der Geschwindigkeit")
Orb("Life", "des Lebens")
Orb("Shielding", "des Schutzes")
Orb("Teleport", "des Teleports")
Orb("Safety", "der Sicherheit")
Orb("Thorns", "der Dornen")
// terrain features
N("none", GEN_N, "keines", "keines", "keines")
N("ice wall", GEN_F, "Eiswand", "Eiswände", "Eiswand")
N("great wall", GEN_F, "Große Mauer", "Große Mauern", "Große Mauer")
N("red slime", GEN_M, "Roter Schleim", "Rote Schleime", "Roten Schleim")
N("blue slime", GEN_M, "Blaue Schleim", "Blaue Schleime", "Blauen Schleim")
N("living wall", GEN_F, "Lebende Wand", "Lebende Wände", "Lebende Wand")
N("living floor", GEN_M, "Lebender Boden", "Lebende Böden", "Lebenden Boden")
N("dead troll", GEN_M, "Toter Troll" ,"Tote Trolle", "Toten Troll")
N("sand dune", GEN_F, "Sanddüne", "Sanddünen", "Sanddüne")
N("Magic Mirror", GEN_M, "Magischer Spiegel", "Magische Spiegel", "Magischen Spiegel")
N("Cloud of Mirage", GEN_F, "Trugbild-Wolke", "Trugbild-Wolken", "Trugbild-Wolke")
N("Thumper", GEN_M, "Thumper", "Thumper", "Thumper")
N("Bonfire", GEN_N, "Feuer", "Feuer", "Feuer")
N("ancient grave", GEN_N, "Altes Grab", "Alte Gräber", "Alte Grab")
N("fresh grave", GEN_N, "Frisches Grab", "Frische Gräber", "Frische Grab")
N("column", GEN_F, "Säule", "Säulen", "Säule")
N("lake of sulphur", GEN_M, "Schwefelsee", "Schwefelseen", "Schwefelsee")
N("lake", GEN_M, "See", "Seen", "See")
N("frozen lake", GEN_M, "gefrorener See", "gefrorene Seen", "gefrorenen See")
N("chasm", GEN_M, "Abgrund", "Abgründe", "Abgrund")
N("big tree", GEN_N, "Großer Baum", "Große Bäume", "Großen Baum")
N("tree", GEN_N, "Baum", "Bäume", "Baum")
// lands
N("Great Wall", GEN_F, "Große Mauer", "Große Mauern", "auf der Großen Mauer")
N("Crossroads", GEN_F, "Kreuzung", "Kreuzungen", "auf der Kreuzung")
N("Desert", GEN_F, "Wüste", "Wüste", "in der Wüste")
N("Icy Land", GEN_N, "Eisiges Land", "Eisige Länder", "im Eisigen Land")
N("Living Cave", GEN_F, "Lebende Höhle", "Lebende Höhlen", "in der Lebenden Höhle")
N("Jungle", GEN_M, "Dschungel", "Dschungel", "im Dschungel")
N("Alchemist Lab", GEN_N, "Labor", "Labor", "im Labor")
N("Mirror Land", GEN_N, "Land der Spiegel", "Länder der Spiegel", "im Land der Spiegel")
N("Graveyard", GEN_M, "Friedhof", "Friedhöfe", "im Friedhof")
N("R'Lyeh", GEN_N, "R'Lyeh", "R'Lyeh", "in R'Lyeh")
N("Hell", GEN_F, "Hölle", "Höllen", "in der Hölle")
N("Cocytus", GEN_M, "Kokytos", "Kokytos", "im Kokytos")
N("Land of Eternal Motion", GEN_N, "Land der Bewegung", "Länder der Bewegung", "im Land der Bewegung")
N("Dry Forest", GEN_M, "Trockener Wald", "Trockene Wälder", "im Trockenen Wald")
N("Game Board", GEN_N, "Spielbrett", "Spielbretter", "auf dem Spielbrett")

// Game messages

// fighting messages

// For each English form, provide a Polish form. Player is referred to via %...0, 
// and objects are referred to via %...1 and %...2. For example, in Polish:

// %a1 refers to the accusative form of the first object (as explained above in 'Nouns')
// likewise, %a2 refers to the accusative form of the second object
// %Å‚1 is replaced by "Å‚", "Å‚o" or "Å‚a", depending on the gender of %1
// %Å‚eÅ›0 adds "Å‚eÅ›" or "Å‚aÅ›" depending on the gender of the player

// Each noun has its singular form (%1), plural form (%P1), and accusative (%a1) given.

// Use whatever %xxx# codes you need for your language. Of course, 
// tell me how your codes should be expanded.

S("You kill %the1.", "Du tötest %den1 %a1.")
S("You would be killed by %the1!", "%Der1 %1 würde dich töten!")
S("%The1 would get you!", "%Der1 %1 würde dich fangen!")
S("%The1 destroys %the2!", "%Der1 %1 zerstört %den2 %a2!")
S("%The1 eats %the2!", "%Der1 %1 frisst %den2 %a2!")
S("The ivy destroys %the1!", "Der Efeu zerstört %den1 %a1!")
S("%The1 claws %the2!", "%Der1 %1 kratzt %den2 %a2!")
S("%The1 scares %the2!", "%Der1 %1 erschreckt %den2 %a2!")
S("%The1 melts away!", "%Der1 %1 schmilzt!")
S("%The1 stabs %the2.", "%Der1 %1 ersticht %den2 %a2.")
S("You stab %the1.", "Du erstichst %den1 %a1.")

S("You cannot attack %the1 directly!", "Du kannst %den1 %a1 nicht direkt angreifen!")
S("Stab them by walking around them.", "Ersteche ihn, indem du um ihn herumläufst.")

S("You feel more experienced in demon fighting!", "Du fühlst dich besser im Dämonen bekämpfen!")
S("Cthulhu withdraws his tentacle!", "Cthulhu zieht sein Tentakel zurück!")
S("The sandworm explodes in a cloud of Spice!", "Der Sandwurm explodiert in einer Wolke von Gewürzen!")
S("%The1 is confused!", "%Der1 %1 ist verwirrt.")
S("%The1 raises some undead!", "%Der1 %1 erweckt die Toten!")
S("%The1 throws fire at you!", "%Der1 %1 wirft Feuer nach dir!")
S("%The1 shows you two fingers.", "%Der1 %1 zeigt dir zwei Finger.")
S("You wonder what does it mean?", "Du wunderst dich, was es bedeutet...")
S("%The1 shows you a finger.", "%Der1 %1 zeigt dir einen Finger.")
S("You think about possible meanings.", "Du denkst über mögliche Bedeutungen nach...")
S("%The1 moves his finger downwards.", "%Der1 %1 bewegt seinen Finger nach unten.")
S("Your brain is steaming.", "Dein Hirn dampft!")
S("%The1 destroys %the2!", "%Der1 %1 zerstört %den2 %a2!")
S("You join %the1.", "Du führst die %P1 zusammen.")
S("Two of your images crash and disappear!", "Zwei %P1 stürzen gegeneinander!")
S("%The1 breaks the mirror!", "%Der1 %1 zerstört den Spiegel!")
S("%The1 disperses the cloud!", "%Der1 %1 verstreut die Wolke!")
S("You activate the Flash spell!", "Du aktivierst den Blitz-Zauber!")
S("You activate the Lightning spell!", "Du aktivierst den Donner-Zauber!")
S("Ice below you is melting! RUN!", "Das Eis unter dir schmilzt - LAUF!")
S("This spot will be burning soon! RUN!", "Dieser Ort wird bald brennen - LAUF!")
S("The floor has collapsed! RUN!", "Der Boden ist zusammengebrochen - LAUF!")
S("You need to find the right Key to unlock this Orb of Yendor!", 
  "Du musst den richtigen Schlüssel finden, um die Perle von Yendor zu öffnen!")
S("You fall into a wormhole!", "Du fällst in ein Wurmloch!")
S("You activate %the1.", "Du aktivierst %den1 %a1.")
S("No room to push %the1.", "Kein Platz um %den1 %a1 zu schieben.")
S("You push %the1.", "Du schiebst %den1 %a1.")
S("You start cutting down the tree.", "Du fängst an, den Baum zu schneiden.")
S("You cut down the tree.", "Du schneidest den Baum.")
S("You cannot attack Sandworms directly!", "Du kannst Sandwürmer nicht direkt angreifen!")
S("You cannot attack Tentacles directly!", "Du kannst Tentakel nicht direkt angreifen!")
S("You cannot defeat the Shadow!", "Du kannst den Schatten nicht besiegen!")
S("You cannot defeat the Greater Demon yet!", "Du kannst den Großen Dämon noch nicht besiegen!")
S("That was easy, but groups could be dangerous.", "Das war einfach, aber Gruppen könnten gefährlich sein.")
S("Good to know that your fighting skills serve you well in this strange world.", "Gut zu wissen, dass deine Kampfkünste dir in dieser ungewöhnlichen Welt helfen.")
S("You wonder where all these monsters go, after their death...", "Du wunderst dich, wo all diese Monster nach ihrem Tod hingehen...")
S("You feel that the souls of slain enemies pull you to the Graveyard...", "Du hast das Gefühl, dass die Seelen gefallener Gegner dich zum Friedhof führen...")
S("Wrong color!", "Falsche Farbe!")
S("You cannot move through %the1!", "Du kannst nicht durch %den1 %a1 gehen!")
S("%The1 would kill you there!", "%Der1 %1 würde dich dort töten!")
S("Wow! %1! This trip should be worth it!", "Wow, %1! Diese Reise sollte es wert sein!")
S("For now, collect as much treasure as possible...", "Aber sammle vorerst so viele Schätze wie möglich ein.")
S("Prove yourself here, then find new lands, with new quests...", "Beweise dich hier - dann suche neue Länder, mit neuen Herausforderungen...")
S("You collect your first %1!", "Du sammelst dein erstes %a1 ein!")
S("You have found the Key! Now unlock this Orb of Yendor!", "Du hast den Schlüssel gefunden! Jetzt musst du die Perle von Yendor finden und öffnen!")
S("This orb is dead...", "Diese Perle ist tot...")
S("Another Dead Orb.", "Noch eine tote Perle.")
S("You have found %the1!", "Du hast %den1 %a1 gefunden!")
S("You feel that %the2 become%s2 more dangerous.", "Du spürst, dass %der2 %2 gefährlicher wird,")
S("With each %1 you collect...", "mit jedem %a1, das du sammelst...")
S("Are there any magical orbs in %the1?...", "Gibt es irgendwelche magischen Perlen in %dem1 %d1?")
S("You feel that %the1 slowly become%s1 dangerous...", "Du spürst wie %der1 %1 langsam gefährlich wird...")
S("Better find some other place.", "Such dir lieber einen anderen Ort.")
S("You have a vision of the future, fighting demons in Hell...", "Du hast eine Vision der Zukunft... du bekämpfst Dämonen in der Hölle...")
S("With this Elixir, your life should be long and prosperous...", "Mit diesem Elixier sollte dein Leben lang und wohlhabend sein...")
S("The Necromancer's Totem contains hellish incantations...", "Das Totem der Toten enthält höllische Zauberformeln...")
S("The inscriptions on the Statue of Cthulhu point you toward your destiny...", "Die Gravuren auf der Statue von Cthulhu erzählen von deinen Schicksal...")
S("Still, even greater treasures lie ahead...", "Und dennoch erwarten dich immer grössere Schätze...")
S("You collect %the1.", "Du sammelst %den1 %a1.")
S("CONGRATULATIONS!", "GLÜCKWUNSCH!")
S("Collect treasure to access more different lands...", "Sammle Schätze ein, um neue Länder zu erreichen...")
S("You feel that you have enough treasure to access new lands!", "Du hast das Gefühl, dass du genug Schätze hast, um neue Länder zu erreichen!")
S("Collect more treasures, there are still more lands waiting...", "Sammle mehr Schätze ein, es gibt immer noch Länder, die nur auf dich warten...")
S("You feel that the stars are right, and you can access R'Lyeh!", "Du hast das Gefühl, dass die Sterne richtig geordnet sind... und du R'Lyeh erreichen kannst!")
S("Kill monsters and collect treasures, and you may get access to Hell...", "Töte Monster und sammle Schätze ein - vielleicht kannst du zur Hölle...")
S("To access Hell, collect 10 treasures each of 9 kinds...", "Um die Hölle zu erreichen, sammle 10 Schätze von 9 Arten...")
S("Abandon all hope, the gates of Hell are opened!", "Gib alle Hoffnung auf - die Tore zur Hölle sind offen!")
S("And the Orbs of Yendor await!", "Und die Perlen von Yendor erwarten dich!")
S("You switch places with %the1.", "Du tauscht Plätze mit %dem %a1.")

S("You rejoin %the1.", "Du vereinst %die1 %a1.")
S("The mirror shatters!", "Der Spiegel zerbricht!")
S("The cloud turns into a bunch of images!", "Die Wolke wandelt sich zu vielen Bildern!!")
S("The slime reacts with %the1!", "Der Schleim reagiert mit %dem %1!")
S("You drop %the1.", "Du lässt %den %a1 fallen.")
S("You feel great, like a true treasure hunter.", "Du fühlst dich toll - wie ein wahrer Schatzjäger.")
S("Your eyes shine like gems.", "Deine Augen leuchten wie Edelsteine.")
S("Your eyes shine as you glance at your precious treasures.", "Deine Augen leuchten, als du deine wunderbaren Schätze betrachtest.")
S("You glance at your great treasures.", "Du blickst auf deine tollen Schätze.")
S("You glance at your precious treasures.", "Du schaust auf deine hübschen Schätze.")
S("You glance at your precious treasure.", "Du untersuchst deine glänzenden Schätze.")
S("Your inventory is empty.", "Du besitzt nichts...")
S("You teleport to a new location!", "Du teleportierst dich an einen neuen Ort!!")
S("Could not open the score file: ", "Konnte die Highscore-Datei nicht öffnen: ")
S("Game statistics saved to %1", "Statistiken nach %1 gespeichert")
S("Game loaded.", "Spiel geladen.")
S("You summon some Mimics!", "Du beschwörst ein paar Mimics!")
S("You summon a golem!", "Du beschwörst einen Golem!")
S("You will now start your games in %1", "Du startest deine Spiele von nun an %a1")
S("Activated the Hyperstone Quest!", "Hyperstein-Quest aktiviert!")
S("Orb power depleted!", "Die Perle hat keine Kraft mehr!")
S("Orbs summoned!", "Perlen beschworen!")
S("Orb power gained!", "Perlen-Kraft erhalten!")
S("Dead orbs gained!", "Tote Perlen erhalten!")
S("Orb of Yendor gained!", "Perle von Yendor erhalten!!")
S("Treasure gained!", "Schatz erhalten!")
S("Lots of treasure gained!", "Verdammt viele Schätze erhalten!")
S("You summon a sandworm!", "Du beschwrörst einen Sandwurm!")
S("You summon an Ivy!", "Du beschwörst einen Efeu!")
S("You summon a monster!", "Du beschwörst ein Monster!")
S("You summon some Thumpers!", "Du platzierst einige Thumper!")
S("You summon a bonfire!", "Du platzierst ein Leuchtfeuer!")
S("Treasure lost!", "Schatz verloren!")
S("Kills gained!", "Kills erhalten!")
S("Activated Orb of Safety!", "Perle der Sicherheit aktiviert!")
S("Teleported to %1!", "Nach %1 teleportiert!")

S("Welcome to HyperRogue", "Willkommen bei HyperRogue")
S(" for Android", " ...für Android")
S(" for iOS", " ...für iOS")
S("! (version %1)\n\n", "! (Version %1)\n\n")
S(" (press ESC for some hints about it).", " (Drücke ESC für ein paar Tipps darüber).")
S("Press 'c' for credits.", "Drücke C für die Credits")
S("game design, programming, texts and graphics by Zeno Rogue <zeno@attnam.com>\n\n",
  "Spieldesign, Code, Texte und Grafiken: Zeno Rogue <zeno@attnam.com>\n\n")
S("add credits for your translation here", "Deutsche Übersetzung: Jeffrey Smith <cartfridge@gmail.com> und Arriator <saymon1@hotmail.de>\n\n")
S(" (touch to activate)", " (zum Aktivieren tippen)")
S(" (expired)", " (abgelaufen)")
S(" [%1 turns]", " [%1 Runden]")
S(", you", ", du")
S("0 = Klein model, 1 = Poincaré model", "0 = Klein-Modell, 1 = Poincaré-Modell")
S("you are looking through it!", "Du siehst durch!")
S("simply resize the window to change resolution", "Einfach das Fenster vergrößern, um die Auflösung zu ändern")
S("[+] keep the window size, [-] use the screen resolution", "[+] Fenstergröße behalten, [-] Bildschirmauflösung verwenden")
S("+5 = center instantly, -5 = do not center the map", "+5 = sofort zentrieren, -5 = nicht zentrieren")
S("press Space or Home to center on the PC", "Drücke LEER oder Pos1 um den Spieler zu fokussieren")
S("You need special glasses to view the game in 3D", "Du brauchst 3D-Brillen für den 3D-Modus")
S("You can choose one of the several modes", "Du kannst einen dieser Modi wählen")
S("ASCII", "ASCII")
S("black", "schwarz")
S("plain", "platt")
S("Escher", "Escher")
S("items only", "Gegenstände")
S("items and monsters", "Gegenstände + Monster")
S("no axes", "keine Achsen")
S("auto", "auto")
S("light", "leicht")
S("heavy", "schwer")
S("The axes help with keyboard movement", "Die Achsen helfen mit der Tastatursteuerung")
S("Config file: %1", "Config-Datei: %1")
S("joystick mode: automatic (release the joystick to move)", "Joystick: Automatisch (loslassen zum Bewegen)")
S("joystick mode: manual (press a button to move)", "Joystick: Manuell (Knopf drücken zum Bewegen)")
S("Reduce the framerate limit to conserve CPU energy", "Framerate-Limit reduzieren (spart CPU-Energie)")
S("Press F1 or right click for help", "Drücke F1 oder Rechtsklick für Hilfe")
S("No info about this...", "Keine Info hierüber...")
S("Press Enter or F10 to save", "Drücke Enter oder F10 zum Speichern")
S("Press Enter or F10 to quit", "Drücke Enter oder F10 zum Beenden")
S("or 'r' or F5 to restart", "oder R oder F5 zum Neustarten")
S("or 't' to see the top scores", "oder T für die Highscore-Liste")
S("or another key to continue", "oder irgendeine andere Taste zum Weitermachen")
S("It is a shame to cheat!", "Cheaten ist eine Schande!")
S("Showoff mode", "Angeber-Modus")
S("Quest status", "Quest-Status")
S("GAME OVER", "GAME OVER")
S("Your score: %1", "Punkte: %1")
S("Enemies killed: %1", "Kills: %1")
S("Orbs of Yendor found: %1", "Perlen von Yendor gefunden: %1")
S("Collect 30 $$$ to access more worlds", "Sammle 30 $$$ für neue Länder")
S("Collect at least 10 treasures in each of 9 types to access Hell", "Sammle 10 Schätze von 9 Arten für die Hölle")
S("Collect at least 10 Demon Daisies to find the Orbs of Yendor", "Sammle 10 Dämonenblümchen für die Perlen von Yendor")
S("Hyperstone Quest: collect at least 10 %1 in %the2", "Hyperstein-Quest: Sammle 10 %1 %a2")
S("Hyperstone Quest completed!", "Hyperstein-Quest abgeschlossen!")
S("Look for the Orbs of Yendor in Hell or in the Crossroads!", "Suche nach Perlen von Yendor in der Hölle oder Kreuzung!")
S("Unlock the Orb of Yendor!", "Öffne die Perle von Yendor!")
S("Defeat 100 enemies to access the Graveyard", "Töte 100 Feinde für den Friedhof")
S("(press ESC during the game to review your quest)", "(Drücke ESC während du spielst, um deine Quest anzuzeigen)")
S("you have cheated %1 times", "Du hast %1-mal Cheats benutzt")
S("%1 turns (%2)", "%1 Runden (%2)")
S("last messages:", "letzte Erinnerungen: ")
S("time elapsed", "Zeit")
S("date", "Datum")
S("treasure collected", "Gesammelte Schätze")
S("total kills", "Gesamt-Kills")
S("turn count", "Runden")
S("cells generated", "Generierte Zellen")
S("t/left/right - change display, up/down - scroll, s - sort by", "T/Links/Rechts - Anzeige ändern, Oben/Unten - Scrollen, S - Sortieren nach")
S("kills", "Kills")
S("time", "Zeit")
S("ver", "Ver.")
S("SORT", "SORT")
S("PLAY", "START")
S("Your total wealth", "Dein Gesamt-Reichtum")
S("treasure collected: %1", "Schätze gesammelt: %1")
S("objects found: %1", "Objekte gefunden: %1")
S("orb power: %1", "Perlen-Kraft: %1")
S(" (click to drop)", " (Klicke zum Fallenlassen)")
S("You can also scroll to the desired location and then press 't'.", "Du kannst auch an die gewünschte Position scrollen und T drücken.")
S("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n", 
  "Ist also potentiell nützlich für extrem lange Spiele, die sonst den Arbeitsspeicher deines Systems frittieren würden.")
S("You can touch the Dead Orb in your inventory to drop it.", "Du kannst die tote Perle in deinem Inventar berühren, um sie fallenzulassen..")
S("This might be useful for Android devices with limited memory.", "Könnte für Android-Geräte mit begrenztem Speicher nützlich sein.")
S("You can press 'g' or click them in the list to drop a Dead Orb.", "Du kannst G drücken (oder sie in der Liste anklicken) um eine tote Perle fallenzulassen.")
S("frames per second", "FPS")
S("monsters killed: %1", "Kills: %1")
S("Drawing %1 (layer %2), F1 for help", "Zeichne %1 (Schicht %2), F1 für Hilfe")
S("hepta floor", "Heptagonaler Boden")
S("hexa floor", "Hexagonaler Boden")
S("character", "Character")
S("ESC for menu/quest", "ESC für Menü/Quest")
S("vector graphics editor", "Vektoren-Grafiken-Editor")
S("cheat mode", "Cheats")
S("heptagonal game board", "Heptagonales Spielbrett")
S("triangular game board", "Triangulares Spielbrett")
S("HyperRogue game board", "HyperRogue-Spielbrett")
S("first page [Space]", "Erste Seite [LEER]")
S("exit configuration", "Einstellungen verlassen")
S("Configuration:", "Einstellungen:")
S("video resolution", "Video-Auflösung")
S("fullscreen mode", "Vollbild")
S("animation speed", "Animations-Geschwindigkeit")
S("dist from hyperboloid ctr", "Entf. vom hyperbolischen Zentrum")
S("scale factor", "Skalierungs-Faktor")
S("wall display mode", "Anzeige-Modus (Wände)")
S("monster display mode", "Anzeige-Modus (Monster)")
S("cross display mode", "Anzeige-Modus (Kreuz)")
S("background music volume", "Hintergrundmusik")
S("OFF", "AUS")
S("ON", "EIN")
S("distance between eyes", "Augen-Abstand")
S("framerate limit", "Framerate-Limit")
S("joystick mode", "Joystick")
S("automatic", "automatisch")
S("manual", "manuell")
S("language", "Sprache")
S("EN", "DE")
S("player character", "Charakter")
S("male", "männlich")
S("female", "weiblich")
S("use Shift to decrease and Ctrl to fine tune ", "Shift zum Verringen, Strg zum feinen")
S("(e.g. Shift+Ctrl+Z)", "Ändern (z.B. Strg+Shift+Z)")
S("the second page [Space]", "Zweite Seite [LEER]")
S("special features [Space]", "Spezial-Zeug [LEER]")
S("exit configuration", "Einstellungen verlassen")
S("see the help screen", "siehe Hilfe-Bildschirm")
S("save the current config", "momentane Einstellungen speichern")
S("(v) config", "(v) Konfig.")
S("Screenshot saved to %1", "Screenshot nach %1 gespeichert")
S("You need an Orb of Teleport to teleport.", "Du brauchst eine Perle um dich zu teleportieren.")
S("Use arrow keys to choose the teleport location.", "Benutze die Pfeiltasten um dein Teleport-Ziel zu wählen.")
S("openGL mode enabled", "OpenGL aktiviert")
S("openGL mode disabled", "OpenGL deaktiviert")
S("openGL & antialiasing mode", "OpenGL & Anti-Aliasing")
S("anti-aliasing enabled", "Anti-Aliasing aktiviert")
S("anti-aliasing disabled", "Anti-Aliasing deaktiviert")
S("You activate your demonic powers!", "Du aktivierst deine dämonischen Kräfte!")

// Steam achievement messages

S("New Achievement:", "Neuer Erfolg:")
S("Your total treasure has been recorded in the Steam Leaderboards.", "Deine Gesamt-Schätze wurden in die Steam Highscore-Liste aufgenommen.")
S("Congratulations!", "Gratulation!")
S("You have improved your total high score and %1 specific high scores!", "Du hast deinen Gesamt-Highscore und %1 spezifische Highscores verbessert!")
S("You have improved your total and '%1' high score!", "Du hast deinen Gesamt-Highscore und '%1' Highscore verbessert!")
S("You have improved your total high score on Steam. Congratulations!", "Du hast deinen Gesamt-Highscore auf Steam verbessert! Gratulation!")
S("You have improved %1 of your specific high scores!", "Du hast %1 deiner spezifischen Highscores verbessert!")
S("You have improved your '%1' high score on Steam!", "Du hast deinen Highscore bei '%1' auf Steam verbessert!")
S("You have collected 10 treasures of each type.", "Du hast 10 Schätze jedes Typs gesammelt.")
S("This is your first victory!", "Dies ist dein erster Sieg!")
S("This has been recorded in the Steam Leaderboards.", "Dies wurde in den Steam-Highscores vermerkt.")
S("The faster you get here, the better you are!", "Je schneller du es hierher schaffst, desto besser bist du!")
S("You have improved both your real time and turn count. Congratulations!", "Du hast sowohl weniger Echtzeit als auch Runden als je zuvor benötigt. Gratulation!")
S("You have used less real time than ever before. Congratulations!", "Du hast weniger Echtzeit als je zuvor benötigt. Gratulation!")
S("You have used less turns than ever before. Congratulations!", "Du hast sowohl weniger Runden als je zuvor benötigt. Gratulation!")

// help texts. These are separated into multiple lines just for convenience,
// you don't have to follow.

S(
 "You have been trapped in a strange, non-Euclidean world. Collect as much treasure as possible "
 "before being caught by monsters. The more treasure you collect, the more "
 "monsters come to hunt you, as long as you are in the same land type. The "
 "Orbs of Yendor are the ultimate treasure; get at least one of them to win the game!",

 "Du wurdest in einer seltsamen, nicht-euklidischen Welt gefangen... Sammle soviel Schätze "
 "wie du kannst - bevor dich die Monster fangen. Je mehr Schätze du hast, umso mehr Monster "
 "werden dich jagen - solange du das Land nicht verlässt. Die "
 "Perlen von Yendor sind der ultimative Schatz - sammle mindestens eine um zu gewinnen!"
 )

S(
 "You can fight most monsters by moving into their location. "
 "The monster could also kill you by moving into your location, but the game "
 "automatically cancels all moves which result in that.\n\n",
 
 "Du kannst die meisten Monster bekämpfen, indem du dich an ihre Position begibst. "
 "Besagtes Monster könnte dich auch töten, indem es sich an deine Position begibt - "
 "aber das Spiel lässt dies nicht zu.\n\n")

S(
 "Usually, you move by touching somewhere on the map; you can also touch one "
 "of the four buttons on the map corners to change this (to scroll the map "
 "or get information about map objects). You can also touch the "
 "numbers displayed to get their meanings.\n",
 "Normalerweise bewegst du dich durch Tippen auf die Karte; du kannst aber auch einen der "
 "vier Knöpfe in den Ecken antippen um dies zu ändern (um die Karte zu scrollen oder Infos "
 "über Objekte zu bekommen). Du kannst auch die Zahlen antippen um ihre Bedeutung zu erfahren.\n")

S("Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Home/Space. "
 "To save the game you need an Orb of Safety. Press 'v' for config, ESC for the quest status and menu.\n\n",
 "Bewege dich mit der Maus, dem Numpad, QWEADZXC, oder HJKLYUBN. Warte mit S oder '.'. Drehe die Welt mit den Pfeiltasten, PgUp/PgDn, und Pos1/Space."
 "Um zu speichern, benötigst du eine Perle der Sicherheit. Drücke V für Einstellungen, ESC für den Quest-Status und das Menü.\n\n")
 
S("See more on the website: ", "Mehr auf der Website: ")

S("special thanks to the following people for their bug reports, feature requests, porting, and other help:\n\n%1\n\n",
  "Besonderer Dank gebührt diesen Leuten für ihre Bug Reports, Feature-Anfragen, Ports, und andere Hilfe:\n\n%1\n\n")

S(
 "The total value of the treasure you have collected.\n\n"
 "Every world type contains a specific type of treasure, worth 1 $$$; "
 "your goal is to collect as much treasure as possible, but every treasure you find "
 "causes more enemies to hunt you in its native land.\n\n"
 "Orbs of Yendor are worth 50 $$$ each.\n\n",

  "Der Gesamtwert deiner Schätze.\n\n"
  "Jedes Land hat einen eigenen Schatztyp, der 1 $$$ wert ist. "
  "Dein Ziel ist soviel Schätze wie möglich zu sammeln -- aber jeder Schatz, den du findest "
  "erzeugt mehr Feinde in seinem Heimatland.\n\n"
  "Perlen von Yendor sind 50 $$$ pro Stück wert.\n\n")

S(
 "The higher the number, the smoother the animations in the game. "
 "If you find that animations are not smooth enough, you can try "
 "to change the options ",
 "Je höher die Zahl, desto flüssiger sind die Animationen. Sind sie nicht flüssig genug, "
 "versuche die Optionen zu ändern ")

S("(Menu button) and select the ASCII mode, which runs much faster. "
 "Depending on your device, turning the OpenGL mode on or off might "
 "make it faster, slower, or cause glitches.",
 "(Im Menü) Versuche den ASCII-Modus zu aktivieren - der läuft viel schneller!"
 "Je nach Gerät kann der OpenGL-Modus das Spiel verschnellern, verlangsamen, oder Glitches hervorrufen."
 )

S("(in the MENU). You can reduce the sight range, this should make "
 "the animations smoother.",
 "(im Menü) wie die Sichtweite zu reduzieren, das sollte die Animation flüssiger machen.")

S("(press v) and change the wall/monster mode to ASCII, or change "
 "the resolution.",
 "(drücke V) wie  den 'Wand/Monster'-Modus auf ASCII zu schalten, oder die Auflösung zu ändern.")

S(
 "In this mode you can draw your own player character and floors. "
 "Mostly for the development purposes, but you can have fun too.\n\n"
 "f - floor, p - player (repeat 'p' for layers)\n\n"
 "n - new shape, u - copy the 'player body' shape\n\n"
 "1-9 - rotational symmetries, 0 - toggle axial symmetry\n\n"
 "point with mouse and: a - add point, m - move nearest point, d - delete nearest point, c - nearest point again, b - add after nearest\n\n"
 "s - save in C++ format (but cannot be loaded yet without editing source)\n\n"
 "z - zoom, o - Poincaré model\n",
 
 "Hier kannst du deine eigenen Spielfiguren/Bodenplatten zeichnen. "
 "Eigentlich zur Entwicklung, aber hab Spaß.\n\n"
 "f - Boden, p - Spieler (wiederholt für Schichten)\n\n"
 "n - neue Form, u - kopiere die Körperform des Spielers\n\n"
 "1-9 - Dreh-Symmetrie, 0 - Achsen-Symmetrie umschalten\n\n"
 "Maus darüber und: a - Punkt hinzufügen, m - Nächsten Punkt verschieben, d - Nächsten Punkt löschen, c - Nächsten Punkt kopieren, b - Nach Nächstem Punkt hinzufügen\n\n"
 "s - im C++-Format speichern (kannst du aber noch nicht laden, ohne Quellcode zu ändern)\n\n"
 "z - Zoom, o - Poincaré-Modell\n")

S(
 "These huge monsters normally live below the sand, but your movements have "
 "disturbed them. They are too big to be slain with your "
 "weapons, but you can defeat them by making them unable to move. "
 "This also produces some Spice. They move two times slower than you.",

 "Diese riesigen Monster leben normalerweise unter dem Sand, aber deine Bewegungen haben sie "
 "geweckt. Sie sind zu groß, als dass du sie mit deinen Waffen erschlagen könntest, "
 "aber du kannst sie besiegen -- indem du ihre Bewegung verhinderst. "
 "Dies produziert auch etwas Gewürz. Sie bewegen sich halb so schnell wie du.")

S("The tentacles of Cthulhu are like sandworms, but longer. "
 "They also withdraw one cell at a time, instead of exploding instantly.",

 "Die Tentakel von Cthulhu sind wie Sandwürmer - nur länger. "
 "Anstatt zu explodieren, ziehen sie sich zurück - eine Zelle nach der anderen.")

S(
 "A huge plant growing in the Jungle. Each Ivy has many branches, "
 "and one branch grows per each of your moves. Branches grow in a clockwise "
 "order. The root itself is vulnerable.",
 "Eine riesige Pflanze, die im Dschungel wächst. Jeder Efeu hat viele Zweige, "
 "und jedes Mal, wenn du dich bewegst, wächst einer. Sie wachsen im Uhrzeigersinn. "
 "Die Wurzel selbst ist verwundbar.\n")

S("The Alchemists produce magical potions from pools of blue and red slime. You "
 "can go through these pools, but you cannot move from a blue pool to a red "
 "pool, or vice versa. Pools containing items count as colorless, and "
 "they change color to the PC's previous color when the item is picked up. "
 "Slime beasts also have to keep to their own color, "
 "but when they are killed, they explode, destroying items and changing "
 "the color of the slime and slime beasts around them.",
 "Die Alchemisten produzieren magische Tränke aus Lachen von blauem und rotem Schleim. "
 "Du kannst durch sie hindurchgehen, aber du kannst nicht von Blau zu Rot wechseln, oder von "
 "Rot zu Blau. Lachen mit Gegenständen darin sind farblos, und werden zu der Farbe, auf der "
 "du schreitest, sobald diese aufgehoben werden. Schleimmonster müssen auch auf ihrer Farbe "
 "bleiben - außer sie sterben, dann explodieren sie nämlich und zerstören Gegenstände, und "
 "ändern die Farbe von Schleim und Schleimmonstern um sie herum.\n")

S(
 "These creatures are slow, but very powerful... more powerful than you. "
 "You need some more experience in demon fighting before you will be able to defeat them. "
 "Even then, you will be able to slay this one, but more powerful demons will come...\n\n"
 "Each 10 lesser demons you kill, you become powerful enough to kill all the greater "
 "demons on the screen, effectively turning them into lesser demons.",
 
 "Diese Kreaturen sind langsam, aber sehr mächtig... mächtiger als du. "
 "Du brauchst Erfahrung im Kampf gegen Dämonen, bevor du diesen entgegentrittst. "
 "Und sogar dann kannst du vielleicht diesen töten, aber dann kommen noch stärkere...\n\n"
 "Jedes Mal, wenn zu 10 Kleine Dämonen tötest, wirst du stark genug, um alle Großen Dämonen "
 "auf dem Bildschirm zu töten (du verwandelst sie in Kleine Dämonen). ")

S("These creatures are slow, but they often appear in large numbers.",
  "Diese Geschöpfe sind langsam - aber tauchen oft in Gruppen auf.")

S("A big monster from the Living Caves. A dead Troll will be reunited "
 "with the rocks, causing some walls to grow around its body.",
 "Ein großes Monster aus den Lebenden Höhlen. Ein toter Troll wird eins mit dem Gestein, "
 "was dazu führt, dass er von Wänden umgeben wird.")

S("Huge, impassable walls which separate various lands.",
  "Gigantische, undurchdringbare Wände, die die verschiedenen Länder voneinander trennen.")

S(
 "This cave contains walls which are somehow living. After each turn, each cell "
 "counts the number of living wall and living floor cells around it, and if it is "
 "currently of a different type than the majority of cells around it, it switches. "
 "Items count as three floor cells, and dead Trolls count as five wall cells. "
 "Some foreign monsters also count as floor or wall cells.\n",
 "Diese Höhle enthält Wände, die (mehr oder weniger) leben. Nach jedem Zug zählt jede Zelle "
 "die Zahl von Wänden und Böden um sich ab - und wenn sie der Minderheit entspricht, wechselt sie. "
 "Gegenstände zählen als drei Böden, und tote Trolle zählen als fünf Wände. "
 "Manche fremden Monster zählen auch als Böden oder Wände.\n")

S(
 "This forest is quite dry. Beware the bushfires!\n"
 "Trees catch fire on the next turn. The temperature of the grass cells "
 "rises once per turn for each fire nearby, and becomes fire itself "
 "when its temperature has risen 10 times.\n"
 "You can also cut down the trees. Big trees take two turns to cut down.",
 "Dieser Wald ist ziemlich trocken. Vorsicht, Waldbrand!\n"
 "Bäume fangen im nächsten Zug Feuer. Die Temperatur von Gras steigt jeden Zug für jedes Feuer in der Nähe, "
 "und wird selbst zu Feuer wenn dies 10-mal geschehen ist.\n"
 "Du kannst auch die Bäume fällen. Große Bäume brauchen zwei Züge dafür."
 )

S("A big and quite intelligent monster living in the Icy Land.",
  "Ein großes - und ziemlich intelligentes - Monster des Eisigen Landes.")

S(
 "A nasty predator from the Icy Land. Contrary to other monsters, "
 "it tracks its prey by their heat.",
 "Ein ziemlich fieser Jäger des Eisigen Landes. Anders als andere Monster, finden diese ihre Beute anhand ihrer Temperatur.")

S("Rangers take care of the magic mirrors in the Land of Mirrors. "
 "They know that rogues like to break these mirrors... so "
 "they will attack you!",
 "Ranger nehmen die Spiegel im Land der Spiegel in Schutz. Sie wissen, dass Schurken wie du "
 "gerne die Spiegel zerbrechen - also greifen sie dich an!")

S("A nasty creature native to the Living Caves. They don't like you "
 "for some reason.",
 "Ein fieses Geschöpf, das in den Lebenden Höhlen haust. Irgendwie können sie dich nicht leiden.")

S("A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them.",
  "Ein Menschenstamm, der in der Wüste lebt. Sie zähmten sogar die gigantischen Sandwürmer, die ihnen nichts tun.")

S("This giant ape thinks that you are an enemy.", "Dieser Affe glaubt, du bist sein Feind.")

S("A magical being which copies your movements.", "Ein magisches Wesen, das deine Bewegungen kopiert.")

S("A magical being which copies your movements. "
 "You feel that it would be much more useful in an Euclidean space.",
 "Ein magisches Wesen, das deine Bewegungen kopiert.\n"
 "Du hast das Gefühl, als wären sie in einer euklidischen Umgebung viel nützlicher.")

S("You can summon these friendly constructs with a magical process.",
  "Du kannst diese freundlichen Steinriesen mit einem magischen Prozess beschwören.")

S("A majestic bird, who is able to fly very fast.",
  "Ein majestätischer Vogel - der auch sehr schnell fliegt.")

S("A monster who is able to live inside the living cave wall.",
  "Ein Monster, das innerhalb der Lebenden Wände leben kann.")

S("A typical Graveyard monster.", "Ein typisches Friedhof-Monster.")

S("A typical monster from the Graveyard, who moves through walls.\n\n"
  "There are also wandering Ghosts. They will appear "
  "if you do not explore any new places for a long time (about 100 turns). "
  "They can appear anywhere in the game.",
  "Ein Monster, das typisch für den Friedhof ist - und sich durch Wände bewegen kann.\n\n"
  "Es gibt auch Wandernde Geister - diese erschinen, wenn du dich lange (um die 100 Züge) "
  "am selben Ort aufhältst. Sie können auch überall auftauchen."
  )

S("Necromancers can raise ghosts and zombies from fresh graves.",
  "Totenbeschwörer können Geister und Zombies aus frischen Gräbern hervorrufen.")

S("A creepy monster who follows you everywhere in the Graveyard.",
  "Ein gruseliges Monster. Es folgt dir durch den ganzen Friedhof!")

S("People worshipping Cthulhu. They are very dangerous.",
  "Verehrer Cthulhus. Sie sind sehr gefährlich.")

S("People worshipping Cthulhu. This one is especially dangerous, "
 "as he is armed with a weapon which launches fire from afar.",
 "Verehrer Cthulhus. Dieser hier ist besonders gefährlich - er besitzt eine Waffe, die aus " 
 "der Ferne Feuer schleudern kann.")

S("This dangerous predator has killed many people, and has been sent to Cocytus.",
  "Dieser gefürchtete Jäger hat schon vielen das Leben gekostet, und wurde deshalb nach Kokytos entsandt.")

S("This white dog is able to run all the time. It is the only creature "
 "able to survive and breed in the Land of Eternal Motion.",
 "Dieser weiße Hund kann die ganze Zeit rennen! Deshalb ist es die einzige Kreatur, die im "
 "Land der Ewigen Bewegung überleben, und sich vermehren kann.")

S("Demons of Hell do not drown when they fall into the lake in Cocytus. "
 "They turn into demonic sharks, enveloped in a cloud of steam.",
 "Dämonen aus der Hölle ertrinken nicht, wenn sie in den See von Kokytos fallen. Stattdessen "
 "werden sie zu Dämonenhaien - eingewickelt in eine Wolke aus Dampf.")

S("These fairies would rather burn the forest, than let you get some Fern Flowers. "
 "The forest is infinite, after all...\n\n"
 "Fire Fairies transform into fires when they die.",
 "Diese Feen würden eher den ganzen Wald abfackeln, als dich Farnblüten sammeln zu lassen. "
 "Der Wald ist schließlich grenzenlos...\n\n"
 "Sie werden selbst zu Feuern, wenn sie sterben.")

S("These warriors of the Forest wield exotic weapons called hedgehog blades. "
 "These blades protect them from a frontal attack, but they still can be 'stabbed' "
 "easily by moving from one place next to them to another.",
 "Diese Krieger besitzen exotische Waffen - die Igelklingen. "
 "Diese schützen sie vor frontalen Attacken, aber Igelkrieger können dennoch 'erstochen' werden - indem du dich an einen Ort neben oder hinter sie begibst.")

S("This being radiates an aura of wisdom. "
 "It is made of a beautiful crystal, you would love to take it home. "
 "But how is it going to defend itself? Better not to think of it, "
 "thinking causes your brain to go hot...\n\n"
 "Crystal Sages melt at -30 °C, and they can rise the temperature around you from afar.",
 "Dieses Wesen strahlt eine Aura aus Weisheit aus. Es besteht aus einem schönen Kristall, "
 "am liebsten würdest du es mit nach Hause nehmen... Nur, wie verteidigt es sich? Besser "
 "nicht daran denken... Denken gibt dir Kopfschmerzen...\n\n"
 "Kristallweisen schmelzen bei -30 °C, und können deine Temperatur aus der Ferne steigern.")

S("Cold white gems, found in the Icy Land.", "Kalte, weiße Edelsteine aus den Eisigen Ländern.")

S("An expensive metal from the Living Caves. For some reason "
 "gold prevents the living walls from growing close to it.",
 "Ein teures Metall aus den Lebenden Höhlen. Aus irgendeinem Grund verhindert Gold, dass Lebende Wände in seiner Nähe entstehen.")

S("A rare and expensive substance found in the Desert. "
 "It is believed to extend life and raise special psychic powers.",
 "Eine seltene, teure Substanz aus der Wüste. Man sagt, es verlängert das Leben und verleiht besondere, psychische Kräfte.")

S("A beautiful gem from the Jungle.", "Ein schöner Edelstein aus dem Dschungel.")

S(
 "A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
 "healthy after drinking it, but you still fell that one hit of a monster is enough to kill you.",
 "Ein wundervolles Getränk, entstanden aus der Mischung blauen und roten Schleims. Du fühlst dich gesünder nach seinem Genuss - und dennoch zerbrechlich genug, dass dich ein Schlag eines Monsters töten könnte. ")

S("A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
 "in the Land of Mirrors leave these.",
 "Ein Stück eines Spiegels oder Trugbilds, das für magische Zwecke brauchbar ist. "
 "Spiegel und Wolken im Land der Spiegel hinterlassen diese.")

S("These sinister totems contain valuable gems.",
  "Diese schaurigen Totems enthalten wertvolle Edelsteine.")

S("These star-shaped flowers native to Hell are a valuable alchemical component.",
  "Diese sternförmigen Blumen, der Hölle heimisch, sind eine wertvolle alchemistische Komponente.")

S("This statue is made of materials which cannot be found in your world.",
  "Diese Statue ist aus Materialien, die in deiner Welt nicht existieren.")

S("One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable.",
  "Eines der wenigen Dinge, die den Boden im Land der Ewigen Bewegung nicht einstürzen lassen. Sie sind offensichtlich sehr wertvoll.")

S("Cold blue gems, found in the Cocytus.", "Kalte, blaue Edelsteine aus dem Kokytos.")

S("These bright yellow gems can be found only by those who have mastered the Crossroads.",
  "Diese hellen, gelben Edelsteine können nur von denen gesehen werden, die die Kreuzung meisterten.")

S("That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
 "Each key unlocks only the Orb of Yendor which led you to it.",
 "Das ist alles, was du brauchst, um die Perle von Yendor zu entsperren! ...solange du wieder zurückfindest...\n"
 "Jeder Schlüssel entsperrt nur die Perle von Yendor, die dich zu ihm geführt hat.")

S("These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
 "power is long gone. No way to use them, you could as well simply drop them...\n\n",
 "Diese Perlen sind aus dem Friedhof. Du schätzt, sie waren einst mächtig, aber vor langer Zeit schwand ihre Kraft. "
 "Es gibt keinen Weg sie zurückzuholen, du könntest die Perlen auch einfach dalassen...\n\n")

S(
 "This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
 "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
 "where the key is after you touch the Orb.",
 "Diese wundervolle Perle kann nur von denen gesammelt werden, die dieses hyperbolische Universum komplett gemeistert haben, da du den richtigen Schlüssel benötigst, um ihre Macht zu entfesseln. "
 "Deine psychischen Fähigkeiten erlauben es dir, zu sehen, wo sich der Schlüssel befindet, nachdem du die Kugel berührt hast.")

S(
  "This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions.",
  "Du kannst diese Perle verwenden um einen Blitz-Zauber zu wirken, der Blitze in sämtliche Richtungen um dich verschießt.")

S("This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2.",
  "Du kannst diese Perle verwenden um einen Leucht-Zauber zu wirken, der fast alles zerstört, was in einem Radius von 2 um dich ist.")

S("This orb can be used to invoke a wall of ice. It also protects you from fires.",
  "Du kannst diese Perle verwenden um einen Eis-Zauber zu wirken, der eine Wand aus Eis hervorruft. Die Perle schützt dich auch vor Feuer.")

S("This orb can be used to move faster for some time.",
  "Du kannst diese Perle verwenden um einen Hermes-Zauber zu wirken, der dich einige Zeit lang beschleunigt.")

S("This orb can be used to summon friendly golems. It is used instantly when you pick it up.",
  "Du kannst diese Perle verwenden um einen Stein-Zauber zu wirken, der freundliche Golems beschwört. (Sie wird sofort verwendet, wenn du sie aufhebst)")

S("This orb can protect you from damage.", "Du kannst diese Perle verwenden um dich vor Schaden zu schützen.")

S("This orb lets you instantly move to another location on the map. Just click a location which "
 "is not next to you to teleport there. ",
   "Du kannst diese Perle verwenden um einen Teleport-Zauber zu wirken. Klicke auf einen Ort, der nicht neben dir ist, um dich dorthin zu teleportieren. (Die Perle wird dabei verbraucht)")

S("This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
 "that you will ever find the way back...\n\n"
 "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
 "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). ",
 "Du kannst diese Perle verwenden um dich an einen weit entfernten, sicheren Ort zu bewegen. Dank der Weise dieser Welt bezweifelst du, dass du je zurückfindest...\n\n"
 "Dein Spiel wird gespeichert, wenn du es beendest während die Perle der Sicherheit noch beikräften ist.\n\n"
 "(Technische Anmerkung: Da es wirklich unmöglich ist, zurückzukehren, lässt diese Perle den Speicher, den die Welt bis jetzt braucht, neu durchlaufen - sogar wenn du sie nicht verwendest, um zu speichern.)")

S("This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n",
  "Du kannst diese Perle verwenden um Igelkrieger direkt anzugreifen, und andere Monster zu 'erstechen'.\n")
 
S("This flower brings fortune to the person who finds it.\n",
  "Diese Blume bringt seinem Finder Glück.\n")

S("Ice Walls melt after some time has passed.", "Eiswände schmelzen nach gewisser Zeit.")

S("A natural terrain feature of the Desert.", "Eine natürliche Terrain-Form der Wüste.")

S("You can go inside the Magic Mirror, and produce some mirror images to help you.",
  "Du kannst den Spiegel betreten, um Spiegelbilder zu erzeugen, die dir helfen.")

S(
 "Tiny droplets of magical water. You see images of yourself inside them. "
 "Go inside the cloud, to make these images help you.",
 "Kleine Tropfen aus magischem Wasser, in dem du dich selbst sehen kannst. "
 "Betrete den Dampf, um diese Bilder real werden zu lassen.")

S("A device that attracts sandworms and other enemies. You need to activate it.",
  "Ein Gerät, das Sandwürmer und andere Feinde anzieht. Allerdings musst du es einschalten.")

S("A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it.",
  "Ein Haufen Holz, den du verwenden kannst, um ein Feuer zu machen. Alles ist schon da - du musst das Feuer schon fast nur berühren, und es brennt.")

S("An ancient grave.", "Ein uraltes Grab.")

S("A fresh grave. Necromancers like those.", "Ein frisches Grab. Totenbeschwörer mögen die.")

S("A piece of architecture typical to R'Lyeh.", "Ein Stück R'Lyeh-typische Architektur.")

S("An impassable lake in Cocytus.", "Ein undurchdringbarer See in Kokytos.")

S("You can walk on it... but beware.", "Du kannst darauf gehen... aber pass auf.")

S("It was a floor... until something walked on it.", "Das WAR mal Boden. Dann ist was darauf gegangen.")

S(
 "This land is a quick gateway to other lands. It is very easy to find other lands "
 "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
 "As long as you have found enough treasure in their native lands, you can "
 "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
 "and other land types bring magical orbs.\n\n"
 "A special treasure, Hyperstone, can be found on the Crossroads, but only "
 "after you have found 10 of every other treasure.",
 "Dieses Land ist eine Art Schnellreise zu anderen Ländern. Es ist sehr einfach, diese von hier aus zu finden - das bedeutet, dass hier auch Monster aus den meisten anderen Ländern hier sein können!\n\n"
 "Solange du genug Schätze in den Ländern gefunden hast, erscheinen magische Gegenstände in der Kreuzung. Das Land der Spiegel bringt Spiegel und Wolken, und andere Länder bringen magische Perlen.\n\n"
 "Ein besonderer Schatz - der Hyperstein - ist hier auffindbar, aber du musst erst 10 von allen anderen Schätzen haben.")

S("A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms.",
  "Ein heißes Land, voller Sanddünen, geheimnisvollem Gewürz, und riesigen sowie gefährlichen Sandwürmern.")

S(
 "A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
 "melt, even if you don't want it.",
 "Ein ziemlich kaltes Land, voller Eiswände. Deine Gegenwart allein wird sie allerdings zum Schmelzen bringen - ob du willst oder nicht.")

S("A land filled with huge ivy plants and dangerous animals.",
  "Ein Land voll mit Efeu und gefährlichen Tieren.")

S("A strange land which contains mirrors and mirages, protected by Mirror Rangers.",
  "Ein seltsames Land mit Spiegelbildern und Trugbildern, die von den Rangern geschützt werden.")

S("All the monsters you kill are carried to this strange land, and buried. "
 "Careless Rogues are also carried here...",
 "Alle Monster, die du tötest, werden hier begraben. Genauso wie achtlose Schurken...")

S("An ancient sunken city which can be reached only when the stars are right.\n\n"
  "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "Eine antike, versunkene Stadt, die du nur erreichen kannst, wenn die Sterne gut liegen.\n\n"
  "Du findest Tempel von Cthulhu in R'Lyeh - sobald du fünf Cthulhu-Statuen hast.")

S("A land filled with demons and molten sulphur. Abandon all hope ye who enter here!",
  "Ein Land voller Dämonen und geschmolzenem Schwefel. Verliere alle Hoffnung, wer hier eintritt!")

S("This frozen lake is a hellish version of the Icy Land. Now, your body heat melts the floor, not the walls.",
  "Dieser gefrorene See ist wie eine höllische Version des Eisigen Lands. Hier schmilzt deine Körperwärme den Boden, nicht die Wände.")

S("A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
 "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
 "the floor.\n",
 "Ein Land, in dem du nicht stehen bleiben darfst - denn jedes Stück Boden ist extrem instabil. Nur Monster, die ewig laufen können, sind auch in der Lage, hier zu überleben. "
 "Nur Phönixfedern sind leicht genug, den Boden nicht zu erschüttern.\n")

S("Affects looks and grammar", "Aussehen und Grammatik")

S("first joystick: movement threshold", "Joystick 1: Bewegungsgrenze")
S("first joystick: execute movement threshold", "Joystick 1: Bewegung-Ausführen-Grenze")
S("second joystick: pan threshold", "Joystick 2: Scroll-Grenze")
S("second joystick: panning speed", "Joystick 2: Scroll-Geschwindigkeit")
S("%The1 is frozen!", "%Der1 %1 ist eingefroren!")
S("%The1 burns!", "%Der1 %1 brennt!")
S("message flash time", "Nachrichten-Zeit")

S("skin color", "Hautfarbe")
S("weapon color", "Waffen-Farbe")
S("hair color", "Haarfarbe")
S("dress color", "Kleidungs-Farbe")
S("Shift=random, Ctrl=mix", "Shift=Zufällig, Ctrl=Mischen")

S("Euclidean mode", "Euklidischer Modus")
S("Return to the hyperbolic world", "Zur hyperbolischen Welt zurückkehren")
S("Choose from the lands visited this game.", "Wähle aus den Ländern, die diese Sitzung besucht wurden.")
S("Scores and achievements are not", "PUNKTE UND ERRUNGENSCHAFTEN WERDEN IM");
S("saved in the Euclidean mode!", "EUKLIDISCHEN MODUS NICHT GESPEICHERT!");

// Android buttons (some are not translated because there are no good short words in Polish)
S("MOVE", "GEHE")
S("BACK", "RÜCK")
S("DRAG", "ZIEH")
S("INFO", "INFO")
S("MENU", "MENÜ")
S("QUEST", "QUEST")
S("HELP", "HILFE")
S("NEW", "NEU")
S("PLAY", "SPIELE")
S("SHARE", "TEILE")
S("HyperRogue for Android", "HyperRogue für Android")
S("Date: %1 time: %2 s ", "Datum: %1 Zeit: %2 s ")
S("distance: %1\n", "Distanz: %1\n")
S("Cheats: ", "Cheats: ")
S("Score: ", "Punkte: ")
S("Kills: ", "Kills: ")
S("Retrieving scores from Google Leaderboards...", "Hole Rekorde aus Google Leaderboards...")
S("Scores retrieved.", "Rekorde erhalten.")

S("Your total treasure has been recorded in the Google Leaderboards.", "Deine Gesamt-Schätze wurden in die Google Leaderboards aufgenommen.")
S("You have improved your total high score on Google. Congratulations!", "Du hast deinen Gesamt-Rekord auf Google verbessert! Gratulation!")
S("You have improved your '%1' high score on Google!", "Du hast deinen '%P1'-Rekord auf Google verbessert!")
S("This has been recorded in the Google Leaderboards.", "Dies wurde in die Google Leaderboards aufgenommen.")

// this text changed a bit:

S("Ever wondered how some boardgame would look on the hyperbolic plane? "
 "I wondered about Go, so I have created this feature. Now you can try yourself!\n"
 "Enter = pick up an item (and score), space = clear an item\n"
 "Other keys place orbs and terrain features of various kinds\n"
 "In the periodic editor, press 0-4 to switch walls in different ways\n",
 "Hast du dich je gewundert, wie ein Brettspiel auf der hyperbolischen Ebene aussehen würde? "
 "Ich habe mich über Go gewundert - und dieses Feature erstellt. Probiere es selbst!\n"
 "Enter = Gegenstand aufheben (und punkten), [LEER] = Gegenstand löschen\n"
 "Andere Tasten erzeugen alles mögliche an Perlen und Terrain-Formen\n"
 "Im periodischen Editor kannst du 0-4 verwenden um die Wände zu verändern\n")

S("Periodic Editor", "Periodischer Editor")
// also translate this line:
// "In the periodic editor, press 0-4 to switch walls in different ways\n",
// "Im periodischen Editor kannst du 0-4 verwenden um die Wände zu verändern\n"

S("Collect 60 $$$ to access even more lands", "Sammle 60 $$$ um noch mehr Länder zu besuchen.")

// Emerald Mine

N("Emerald Mine", GEN_F, "Smaragdmine", "Smaragdminen", "in der Smaragdmine")
N("Pikeman", GEN_M, "Pikenier", "Pikeniere", "Pikenier")
N("Flail Guard", GEN_M, "Morgenstern-Wächter", "Morgenstern-Wächter", "Morgenstern-Wächter")
N("Miner", GEN_M, "Minenarbeiter", "Minenarbeiter", "Minenarbeiter")
N("Emerald", GEN_M, "Smaragd", "Smaragde", "Smaragd")

Orb("the Mind", "der Psyche")

S(
     "Evil people are mining for emeralds in this living cave. "
     "It does not grow naturally, but it is dug out in a regular "
     "pattern, which is optimal according to the evil engineers.",
     "Böse Leute bauen Smaragde in dieser lebenden Höhle ab. "
     "Sie ensteht nicht natürlich, sondern ist gemustert ausgegraben. "
     "Dies sehen die bösen Ingenieure als \"optimal\" an.")

S(
    "Miners have special tools for dealing with the Living Cave. "
    "When they die, these tools activate, destroying the living cave "
    "around them.",
    "Minenarbeiter haben spezielle Werkzeuge für den Umgang mit der Lebenden Höhle. "
    "Sterben sie, werden diese aktiviert, was die Höhle um sie einbrechen lässt.")

S(
    "When Pikemen move, they attack all cells which are now adjacent to them. "
    "Luckily, they can be killed in the same way.\n\n"
    "They never move if this would attack their friends.",
    "Bewegen sich Pikeniere, greifen sie alle jetzt umliegenden Zellen an. "
    "Glücklicherweise können sie genauso getötet werden.\n\n"
    "Sie bewegen sich nicht wenn sie so ihre Freunde angreifen würden.")

S(
    "This guard of the Emerald Mine is wielding a huge flail. "
    "You cannot attack him directly, as the flail would still hit you then. "
    "Luckily, you have learned a trick: if you step away from him, "
    "he will hit himself with the flail!",
    
    "Dieser Wächter der Smaragdmine hat einen riesigen Morgenstern. "
    "Ihn direkt anzugreifen wäre dämlich - der Stern würde dich treffen. "
    "Allerdings kannst du von ihm wegtreten. Dann trifft er sich selbst!")

S("A precious green gem from the Emerald Mines.", "Ein schöner, grüner Stein aus den Smaragdminen.")

S("%The1 pierces %the2!", "%Der1 durchdringt %den2 %a2!")

S("Make him hit himself by walking away from him.",
  "Bringe ihn dazu, sich selbst zu treffen (geh von ihm weg).")

S("This orb allows you to instantly kill a non-adjacent enemy by clicking it. "
    "Each use drains 30 charges.",
  "Diese Perle erlaubt es dir, sofort einen nicht-angrenzenden Feind zu töten, indem du auf ihn klickst. "
  "Jede Verwendung verbraucht 30 Ladungen.")

S("%The1 is immune to mental blasts!", "%Der1 ist immun gegen mentale Korruption!")
S("You kill %the1 with a mental blast!", "Du tötest %den1 %a1 mit mentaler Korruption!")

// Vineyard

N("Vineyard", GEN_M, "Weingarten", "Weingärten", "im Weingarten")
N("Vine Beast", GEN_F, "Reben-Bestie", "Reben-Bestien", "Reben-Bestie")
N("Vine Spirit", GEN_M, "Reben-Seele", "Reben-Seelen", "Reben-Seele")
N("vine", GEN_F, "Rebe", "Reben", "Rebe")
N("partial fire", GEN_N, "partielles Feuer", "partielle Feuer", "partielle Feuer")
N("Wine", GEN_M, "Wein", "Wein", "Wein")

S(  "This cell is partially on fire.", "Diese Zelle brennt teilweise.")

S(
  "The Vineyard is filled with vines. A very dense pattern of straight lines here...\n\n" 
  "Vine Beasts and Vine Spirits change vine cells to grass, and vice versa.",
  "Der Weingarten ist voller Reben. Hier gibt es viele gerade Linien...\n\n"
  "Reben-Bestien und Reben-Seelen ändern Zellen von Reben zu Gras, und umgekehrt.")

S("A vine is growing here... but only on a half of the cell. How is that even possible?!"
  "Most monsters cannot move from this cell to the cell containing the other half. "
  "Vine spirits can move only to the adjacent cells which are also adjacent to the "
  "other half.",
  "Hier wächst eine Rebe... AUF DER HALBEN ZELLE?! WIE IST DAS MÖGLICH!?"
  "Die meisten Monster können sich nicht von dieser Zelle aus auf die bewegen, die die andere Hälfte enthält. "
  "Reben-Seelen können sich nur auf angrenzende Zellen bewegen, die auch der anderen Hälfte anliegen. "
  "(Klingt kompliziert, oder?)")

S(
    "A beast made of vines!\n\n"
    "Vine Beasts turn into vines when they die.",
    "Eine Bestie aus Reben!\n\n"
    "Wird zu Reben, wenn sie stirbt.")

S(  "A spirit living in the vines!\n\n"
    "Vine Spirits destroy the vines when they die.",
    "Eine Seele in den Reben!\n\n"
    "Zerstört die Reben, wenn sie stirbt.")

S("Wine grown under hyperbolic sun would be extremely prized in your home location.",
  "Wein, der unter hyperbolischer Sonne wächst, könnte in deiner Heimatwelt ziemlich viel wert sein.")

S("This orb allows one to pass through all kinds of walls and chasms.",
  "Diese Perle erlaubt es, durch sämtliche Wände und über Abgründe zu segeln.")

S("You cannot attack through the Vine!",
  "Du kannst durch die Reben nicht angreifen!")

Orb("Aether", "des Aethers")

// Dead Caves

N("Dead Cave", GEN_F, "Tote Höhle", "Tote Höhlen", "in der Toten Höhle")
N("Dark Troll", GEN_M, "Dunkler Troll", "Dunkle Trolle", "Dunklen Troll")
N("Earth Elemental", GEN_M, "Erd-Elementar", "Erd-Elementare", "Erd-Elementar")
N("dead wall", GEN_F, "tote Wand", "tote Wände", "tote Wand")
N("dead floor", GEN_F, "toter Boden", "tote Böden", "toten Boden")
N("rubble", GEN_N, "Geröll", "Geröll", "Geröll")
N("Silver", GEN_N, "Silber", "Silber", "Silber")

S("A precious metal from the Dead Caves.", "Ein wertvolles Metall aus den toten Höhlen.")

S(  "A Troll without the power of Life.", "Ein Troll ohne die Macht des Lebens.")

S(
    "A rare unliving construct from the Dead Caves. "
    "It instantly destroys cave walls next to its path, and also leaves "
    "an impassable wall behind it. You suppose that this impassable wall helps it to "
    "escape from some threats. You hope you won't meet these threats...",
    
    "Ein seltenes, unlebendes Geschöpf aus den Toten Höhlen. "
    "Es zerstört sofort Höhlenwände, die ihm im Weg sind, und hinterlässt "
    "undurchdringbare Wände, die (so vermutest du) bei der Flucht verhelfen sollen. "
    "Hoffentlich wirst du nicht sehen, wovor es flieht...")

S("%The1 punches %the2!", "%Der1 schlägt %den2 %a2!")
S("%The1 is destroyed by the forces of Life!", "Die Kräfte des Lebens zerstören %den1 %a1!")
S(  "Somehow, this cave has not received the spark of Life yet.",
    "Diese Höhle hat irgendwie noch nicht den Funken des Lebens erhalten.")

// Hive
N("Hive", GEN_M, "Stock", "Stöcke", "im Stock")
N("Red Hyperbug", GEN_M, "Roter Hyperkäfer", "Rote Hyperkäfer", "Roten Hyperkäfer")
N("Green Hyperbug", GEN_M, "Grüner Hyperkäfer", "Grüne Hyperkäfer", "Grünen Hyperkäfer")
N("Blue Hyperbug", GEN_M, "Blauer Hyperkäfer", "Blaue Hyperkäfer", "Blauen Hyperkäfer")
N("Royal Jelly", GEN_N, "Königliches Gelee", "Königliche Gelees", "Königliche Gelee")
N("weird rock", GEN_M, "Seltsamer Stein", "Seltsame Steine", "Seltsamen Stein")

Orb("Invisibility", "der Transparenz")

S("A weirdly colored rock. Hyperentomologists claim that the "
  "Hyperbug armies use these rocks to navigate back home after a victorious battle.",
  "Ein seltsam gefärbter Stein. Hyperentomologen behaupten, die Hyperkäfer-Armeen "
  "verwenden sie, um nach einer siegreichen Schlacht nach Hause zu finden.")

S("%The1 fights with %the2!", "%Der1 %1 bekämpft %den2 %a2!")

S("The Hive is filled with Hyperbugs. They are huge insects which look a bit like "
  "ants, a bit like bees, and a bit like roaches. "
  "They live in circular nests, and an army of Hyperbugs will attack any intruder, "
  "including you and enemy Hyperbugs. Will you manage to get to the "
  "heart of such a nest, and get the precious Royal Jelly?",
  "Der Stock ist randvoll mit Hyperkäfern - riesige Insekten, die etwas wie Ameisen, "
  "Bienen, und Kakerlaken aussehen. Sie leben in kreisförmigen Nestern, und eine Armee "
  "von ihnen greift sämtliche Eindringlinge an (auch dich und andere Hyperkäfer). "
  "Schaffst du es zum Zentrum eines Nests, und das Königliche Gelee zu holen?")

S("This is what Hyperbug Queens eat. Very tasty and healthy.",
  "Dieses Gelee wird hauptsächlich von Hyperkäfer-Königinnen genossen. Sehr schmackhaft und gesund.")

S("When you have this Orb, most monsters won't see you, unless "
    "you are standing still, attacking, or picking up items.",
  "Besitzt du diese Perle, sehen dich die meisten Monster nicht - "
  "es sei denn, du stehst still, greifst an, oder hebst Gegenstände auf.")

Orb("Earth", "der Erde")

S("This orb lets you go through living walls. It also has powers in some of the other lands.",
  "Diese Perle erlaubt es dir, dich durch lebende Wände zu bewegen - und einige andere Sachen in anderen Ländern zu machen.")

// Land of Power

N("Land of Power", GEN_N, "Land der Macht", "Länder der Macht", "im Land der Macht")
N("Witch Apprentice", GEN_M, "Hexenlehrling", "Hexenlehrlinge", "Hexenlehrling")
N("Speed Witch", GEN_F, "Geschwindigkeits-Hexe", "Geschwindigkeits-Hexen", "Geschwindigkeits-Hexe")
N("Flash Witch", GEN_F, "Blitz-Hexe", "Blitz-Hexen", "Blitz-Hexe")
N("Fire Witch", GEN_F, "Feuer-Hexe", "Feuer-Hexen", "Feuer-Hexe")
N("Aether Witch", GEN_F, "Aether-Hexe", "Aether-Hexen", "Aether-Hexe")
N("Winter Witch", GEN_F, "Winter-Hexe", "Winter-Hexen", "Winter-Hexe")
N("Evil Golem", GEN_M, "Böser Golem", "Böse Golems", "Bösen Golem")
N("Powerstone", GEN_M, "Macht-Stein", "Macht-Steine", "Macht-Stein")
N("crystal cabinet", GEN_N, "Kristall-Kabinett", "Kristall-Kabinette", "Kristall-Kabinett")

S("When you have this Orb, you will leave a trail of fire behind you.",
  "Mit dieser Perle hinterlässt du eine Spur aus Feuer hinter dir.")

S(
    "A Witch without any special powers. But watch out! She will "
    "pick up any basic Orbs on her path, and use their powers.",
    "Eine Hexe ohne besondere Kräfte. Sie wird allerdings jegliche Perlen in "
    "ihrer Nähe aufhebene, und ihre verwenden.")

S(  "A Witch with a Speed spell. She moves twice as fast as you. Unless you "
    "have an Orb of Speed too, of course!",
    "Eine Hexe mit einem Geschwindigkeits-Zauber. Sie bewegt sich doppelt so schnell wie du. "
    "Außer natürlich, du hast auch eine Perle der Geschwindigkeit!")

S(  "A Witch with a Fire spell. She will leave a trail of fire behind her.",
    "Eine Hexe mit einem Feuer-Zauber. Sie hinterlässt eine Spur aus Feuer.")

S(  "A Witch with a Winter spell. She is able to move through fire.",
    "Eine Hexe mit einem Winter-Zauber. Sie kann durch Feuer gehen.")

S(  "A Witch with an Aether spell. She is able to move through fire and walls.",
    "Eine Hexe mit einem Aether-Zauber. Sie kann durch Feuer und Wände gehen.")

S(  "Somebody has summoned these evil constructs with a magical process.",
    "Jemand hat diese bösen Konstrukte mit einem magischen Ritual beschworen.")
    
S(  "The Land of Power is filled with everburning fire, magical Orbs, and guarded by "
    "witches and golems. There are basic orbs lying everywhere, and more prized ones "
    "are kept in crystal cabinets.\n\n"
    "Witches are allowed to use all the powers of the "
    "basic orbs against intruders. These powers never expire, but a Witch "
    "can use only one power at a time (not counting Orbs of Life).\n\n"
    "Witches and Golems don't pursue you into other Lands. Also, most Orb powers "
    "are drained when you leave the Land of Power.",
    "Das Land der Macht ist voll mit ewigen Flammen, magischen Perlen, und wird von Hexen und Golems bewacht. Überall liegen einfache Perlen, und wertvollere werden in Kristall-Kabinetten gehalten.\n\n"
    "Hexen dürfen alle Perlen-Kräfte gegen Angreifer einsetzen. Diese laufen nie aus, allerdings kann eine Hexe (mit Ausnahme der Perle des Lebens) jederzeit nur eine besitzen.\n\n"
    "Hexen und Golems verfolgen dich nicht in andere Länder. Außerdem werden die meisten Kräfte von dir genommen, sobald du das Land verlässt.")

S(
    "Witches use these crystal cabinets to protect Powerstones, as well as the more "
    "expensive Orbs. They are partially protected from thieves: they are too strong "
    "to be smashed by conventional attacks, and if you try to steal the item "
    "using an Orb of Aether, your Aether power will be completely drained.",
  "Hexen verwenden diese Kristall-Kabinette um Macht-Steine, und die teureren  "
  "Perlen zu schützen. Sie sind teilweise diebstahlsicher: zu stark, um von "
  "normalen Angriffen zerschmettert zu werden - und versuchst du, den Inhalt "
  "mit einem Aether-Zauber zu stehlen, wird dieser komplett aufgesogen.")

S(
    "A Stone from the Land of Power. You are not sure what it is exactly, but "
    "as the Powerstones are kept in crystal cabinets, they are surely valuable.",
    "Ein Stein aus dem Land der Macht. Du bist dir nicht ganz sicher WAS es it, "
    "aber da er in einem Kristall-Kabinett war, ist er sicher wertvoll!")

Orb("Fire", "des Feuers")

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

S("Your Aether power has expired! RUN!", "Deine Aether-Kraft ist zuende! LAUF!")

S("%The1 tricks %the2.", "%Der1 %1 überlistet %den2 %a2.")
S("You trick %the1.", "Du überlistest %den1 %a1.")

S(  "A Witch with a Flash spell. Very dangerous!\n\nLuckily, she never uses the spell if it "
    "would kill her friends. She could destroy an Evil Golem, though.", 
    "Eine Hexe mit einem Blitz-Zauber. Sehr gefährlich!\n\n"
    "Glücklicherweise setzt sie den Zauber nicht ein, wenn dies ihre Freunde gefährden würde. Aber gegen Böse Golems.")

S("Your Aether powers are drained by %the1!", "%Der1 %1 saugt deine Aether-Kraft auf!")
S("As you leave, your powers are drained!", "Deine Kraft verblasst, als du gehst...")
S("%The1 picks up %the2!", "%Der1 %1 hebt %den2 %a2 auf!") // currently only Witches, so only female form

S("You can right click any element to get more information about it.\n\n",
  "Du kannst auf alles rechtsklicken, um Informationen darüber zu bekommen.\n\n")

S("TRANSLATIONWARNING", "VORSICHT: Die deutsche Übersetzung enthält keine Funktionen nach Version 7.2! (Arbeite dran <3)")

S("The Hyperbugs are fighting!", "Die Hyperkäfer kämpfen!")

S("","") N("",GEN_N,"","","")

S("Cheat-changed the display.", "Anzeige geändert (Cheater!).")
S("Dead floor, with some rubble.", "Toter Boden, mit etwas Geröll.")

// Camelot

N("Camelot", GEN_N, "Camelot", "Camelot", "in Camelot")
N("wall of Camelot", GEN_F, "Wand von Camelot", "Wände von Camelot", "Wand von Camelot")
N("moat of Camelot", GEN_M, "Graben von Camelot", "Gräben von Camelot", "Graben von Camelot")
N("Round Table", GEN_F, "Runde Tafel", "Runde Tafeln", "Runde Tafel")
N("Knight", GEN_M, "Ritter", "Ritter", "Ritter")
N("Holy Grail", GEN_M, "Heiliger Gral", "Heilige Grale", "Heiligen Gral")

S(
  "The Knights of the Round Table are the greatest warriors of these lands. "
  "They are not very inventive with names though, as they call each of their "
  "castles Camelot. "
  "You are probably worth of joining them, but they will surely give you "
  "some quest to prove yourself...\n\n"
  "Each castle contains a single treasure, the Holy Grail, in the center. "
  "The radius of the Round Table is usually 28, but after you find a Holy Grail "
  "successfully, each new castle (and each Round Table) you find will be bigger.",

  "Die Ritter der Runden Tafel sind die größten Krieger dieser Lande. "
  "...sie sind nur nicht sehr originell, irgendwie heißen alle Schlösser Camelot... "
  "Du bist wahrscheinlich ihrer würdig, allerdings musst du ihnen das sicher beweisen.\n\n"
  "Jedes Schloss enthält einen einzigen Schatz - den Heiligen Gral. Dieser ist in der Mitte. "
  "Der Radius einer Tafel ist normalerweise 28, allerdings wächst mit jedem Gral, den du findest, "
  "das nächste Schloss (und seine Tafel).")

S("It would be impolite to land on the table!",
  "Es wäre unhöflich, auf dem Tisch zu landen.")
S("You jump over the table!", "Du kannst nicht über die Tafel springen!")
S("That was not a challenge. Find a larger castle!",
  "Das war keine Herausforderung. Finde ein größeres Schloss!")
S("The Knights congratulate you on your success!",
  "Die Ritter gratulieren dir zu deinem Erfolg!")
S("The Knights laugh at your failure!",
  "Die Ritter amüsieren sich über deinen Fehlschlag!")
S("The Knights stare at you!", "Die Ritter starren dich an...")
S("Come on, this is too easy... find a bigger castle!",
  "Komm schon, das ist ja ZU einfach... such ein größeres Schloss!")
S("The Knights wish you luck!", "Die Ritter wünschen dir Glück!")

S("Congratulations! You have found the Holy Grail!",
  "Gratulation! Du hast den Heiligen Gral gefunden!")

S("\"I would like to congratulate you again!\"", "\"Ich möchte dir erneut gratulieren!\"")
S("\"Find the Holy Grail to become one of us!\"", "\"Finde den Gral, um einer von uns zu werden!\"")
S("\"The Holy Grail is in the center of the Round Table.\"", "\"Der Heilige Gral ist im Zentrum der Runden Tafel.\"")
S("\"I enjoy watching the hyperbug battles.\"", "\"Ich mag es, den Hyperkäfer-Schlachten zuzusehen.\"")
S("\"Have you visited a temple in R'Lyeh?\"", "\"Warst du schon in einem der Tempel in R'Lyeh?\"")
S("\"Nice castle, eh?\"", "\"Nettes Schloss, nicht?\"")

// Temple

N("Temple of Cthulhu", GEN_M, "Cthulhus Tempel", "Cthulhus Tempel", "in Cthulhus Tempel")
N("big statue of Cthulhu", GEN_F, "große Cthulhu-Statue", "große Cthulhu-Statuen", "große Cthulhu-Statue")
N("Grimoire", GEN_N, "Grimoire", "Grimoires", "Grimoire")
N("Cult Leader", GEN_M, "Kult-Anführer", "Kult-Anführer", "Kult-Anführer")

S("The temple of Cthulhu consists of many concentric circles of columns. "
  "You will surely encounter many Cultists there, who believe that a pilgrimage "
  "to the inner circles will bring them closer to Cthulhu himself, and Grimoires "
  "which surely contain many interesting secrets.\n\n"
  "The circles in the temple of Cthulhu are actually horocycles. They are "
  "infinite, and there is an infinite number of them.",
  
  "Der Tempel von Cthulhu. Er besteht aus mehreren, konzentrischen Kreisen aus Säulen. "
  "Du wirst sicherlich einige Okkultisten hier finden - diese glauben, eine Reise in die "
  "inneren Kreise bringt sie näher zu Cthulhu selbst - und Grimoires - die sicher viele "
  "sinteressante Geheimnisse verbergen.\n\n"
  "Die Kreise hier sind eigentlich Horozyklen - unendlich groß, und unendlich viele davon."
  )

S("The Grimoires contain many secrets of the Great Old Ones. "
  "Each new inner circle in the Temple of Cthulhu contains new Grimoires, with new secrets. "
  "You hope to read them when you return home, and to learn many things. "
  "The knowledge is valuable to you, but it is rather pointless to try to get "
  "several copies of the same Grimoire...",
  "Die Grimoires enthalten viele Geheimnisse der Großen Alten. "
  "Jeder neue innere Kreis des Tempels von Cthulhu enthält neue Grimoires, mit mehr Geheimnissen. "
  "Das Wissen ist dir sehr kostbar, aber es wäre natürlich sinnlos, mehrmals dasselbe Grimoire aufzuheben..."
  )

S("These statues of Cthulhu are too large to carry, and they don't look too "
  "valuable anyway. Most monsters will never go through them... they probably have "
  "their reasons. But you can go! When you move into the cell containing "
  "a statue, you push the statue to the cell you left.\n",
  
  "Diese Statuen von Cthulhu sind zu groß um sie zu tragen (und sehen sowieso nicht wertvoll aus). "
  "Die meisten Monster gehen nie durch sie... die haben schon ihre Gründe. "
  "Du aber schon! Wenn du in die Zelle gehst, in der eine steht, schiebst du sie an deinen Ursprungsort."
  )

S("These Cultists can push the statues, just like you.",
  "Diese Kultisten können die Statuen schieben, genau wie du.")

S("You already have this Grimoire! Seek new tomes in the inner circles.",
  "Du hast dieses Grimoire schon! Suche neue in den inneren Kreisen.")
  
S("You push %the1 behind you!", "Die schiebst %den1 %1 hinter dich.")

// More texts

S("Your total kills", "Deine Gesamt-Kills")

S(
  "In most lands, more treasures are generated with each enemy native to this land you kill. "
  "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
  "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
  "but are not counted in the total kill count.",

  "In den meisten Ländern werden mehr Schätze generiert, je mehr Feinde du darin tötest. "
  "Darüber hinaus benötigst du 100 Kills für den Friedhof und den Stock.\n\n"
  "Freundliche Kreaturen und Monster-Teile (beispielsweise Efeu) werden zwar gezeigt, zählen aber nicht zur Gesamt-Punktzahl.")
  
S("There must be some temples of Cthulhu in R'Lyeh...",
  "Es muss duch ein paar Tempel von Cthulhu in R'Lyeh geben...")

S("You overheard Hedgehog Warriors talking about emeralds...",
  "Du überhörst Igelkrieger, die über Smaragde sprechen...")

S("You overhear miners talking about a castle...",
  "Du hörst Minenarbeiter... sie sprechen über ein Schloss...")
  
S("A castle in the Crossroads...", "Ein Schloss in der Kreuzung...")

/*
S("You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "Du kannst Tempel von Cthulhu in R'Lyeh finden, sobald du fünf seiner Statuen besitzt.")
*/

S("You have to escape first!", "Du musst erst entkommen!");
S("There is not enough space!", "Zu wenig Platz!");

S("Customize character", "Character anpassen")
S("gender", "Geschlecht")

// 

Orb("the Dragon", "des Drachen")

S("This Orb allows you to throw fire, just like the Fire Cultists.\n\n"
  "Each fire drains 5 charges. You are not allowed to throw fire into adjacent cells.",
  "Diese Perle erlaubt es dir, Feuer zu werfen (genau wie die Feuer-Okkultisten).\n\n"
  "Jedes Feuer verbraucht 5 Ladungen. Du darfst kein Feuer auf angrenzende Zellen werfen."
  )

S("You throw fire!", "Du wirfst Feuer!")

Orb("Trickery", "des Truges")

N("Illusion", GEN_F, "Illusion", "Illusionen", "Illusion")

S("This Orb allows you to create illusions of yourself. Illusions are targetted "
  "by most monsters, just like yourself, Thumpers, and your friends.\n\n"
  "Each illusion takes 5 charges to create, and one extra charge "
  "per turn. You can also click your illusion to take it away, restoring 4 charges.\n\n"
  "If you have both Orb of Teleport and Orb of Trickery, Illusion is cast "
  "first -- you can then teleport on your Illusion to switch places with it.",
  "Diese Perle erlaubt dir, Illusionen deiner selbst zu erschaffen. Diese werden "
  "von den meisten Monstern verfolgt (wie du, Thumper, und deine Freunde).\n\n"
  "Jede Illusion benötigt 5 Ladungen zum Erschaffen, plus eine Ladung pro Zug. "
  "Du kannst deine Illusion anklicken um sie zu entfernen, wofür du 4 Ladungen bekommst.\n\n"
  "Besitzt du eine Perle des Teleports und eine des Truges, wird Illusion bevorzugt -- "
  "du kannst dich dann auf deine Illusion teleportieren, um die Plätze zu tauschen."
  )

S("Illusions are targetted "
  "by most monsters, just like yourself, Thumpers, and your friends.",
  "Illusionen werden von den meisten Monstern verfolgt "
  "(wie du, Thumper, und deine Freunde).")

S("You create an Illusion!", "Du erschaffst eine Illusion!")
S("You take the Illusion away.", "Du entfernst die Illusion.")

S("You cannot target that far away!", "Zu weit weg!")
S("You cannot target that close!", "Zu nah!")
S("Cannot teleport on a monster!", "Kann nicht auf Monster teleportieren!")
S("Cannot teleport on an item!", "Kann nicht auf Gegenstände teleportieren!")
S("Cannot teleport here!", "Du kannst dich hierhin nicht teleportieren!")
S("Cannot cast illusion on an item!", "Kann Illusion nicht auf Gegenständen erschaffen!")
S("Cannot cast illusion here!", "Du kannst hier keine Illusion erschaffen!")
S("Cannot cast illusion on a monster!", "Kann Illusion nicht auf Monstern erschaffen!")
S("Cannot throw fire there!", "Du kannst dort kein Feuer hinwerfen!")

// teleport -> target
// You cannot target that close!

S("or ESC to see how it ended", "oder ESC um zu sehen, wie alles endete")
S("high contrast", "Hoher Kontrast")
S("draw the heptagons darker", "Heptagone verdunkeln");
S("targetting ranged Orbs Shift+click only", "Distanz-Perlen per Shift-Klick")
S("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc.",
  "Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc.")
  S("also hold Alt during the game to toggle high contrast",
  "Du kannst auch ALT im Spiel halten, um hohen Kontrast umzuschalten")

// Crossroads II

N("Crossroads II", GEN_N, "II. Kreuzung", "II. Kreuzungen", "auf der II. Kreuzung")

S(
  "An alternate layout of the Crossroads. It is more dense and more regular, "
  "although you won't find the castle of Camelot here.",
  
  "Ein alternatives Layout der Kreuzung. Es ist beständiger und dichter, aber du findest Camelot hier nicht.")

// Caribbean

N("Caribbean", GEN_F, "Karibik", "Karibiken", "auf der Karibik")

N("Pirate", GEN_M, "Pirat", "Piraten", "Pirat")
N("Shark", GEN_M, "Hai", "Haie", "Hai")
N("Parrot", GEN_F, "Papagei", "Papageien", "Papagei")
N("Pirate Treasure", GEN_M, "Piratenschatz", "Piratenschätze", "Piratenschatz")
N("Compass", GEN_M, "Kompass", "Kompasse", "Kompass")

N("sea", GEN_M, "See", "Seen", "See")
N("boat", GEN_N, "Boot", "Boote", "Boot")
N("island", GEN_F, "Insel", "Inseln", "Insel")
N("tree", GEN_M, "Baum", "Bäume", "Baum")

Orb("Time", "der Zeit")

S("This dangerous sea contains nasty sharks and pirates. ",
  "Dieser gefährliche See enthält fiese Haie und Piraten.")

S("The islands of Caribbean are infinite and circular, and "
  "the pirates probably have hidden their treasures somewhere far from the coast.",
  "Die Inseln der Karibik sind unzählig und kreisförmig -- die Piraten haben "
  "ihre Schätze sicher fernab der Küste versteckt.")

S(
    "Hyperbolic pirates do not need huge ships, since so many lands to conquest "
    "are so close. These small boats are enough for them.\n\n"
    "Boats allow you to go through water. If you are in a boat, you can move into "
    "a water cell (and the boat will come with you).",
    
    "Hyperbolische Piraten brauchen keine riesigen Schiffe -- so viele Länder zum "
    "Erobern sind ja ziemlich dicht zusammen. Diese kleinen Boote reichen ihnen.\n\n"
    "Boote lassen dich durch Wasser bewegen. Bist du in einem, kannst du auf eine Wasser-"
    "Zelle gehen (und das Boot kommt natürlich mit).")

S(
    "The forests of Caribbean are too dense to be traversed by humans, "
    "and they are hard to burn. Many colorful parrots can be found there.",
    "Die karibischen Wälder sind zu dicht um von Menschen durchquert zu werden, "
    "und brennen nur schwerlich. Es gibt hier aber viele, farbenfrohe Papageien.")

S("Ye wonder where did th' Pirates find all these riches...",
    "Du fragst dich, woher die Piraten all diese Schätze haben...")

S(
  "The hyperbolic pirates have no use for treasure maps. However, they have found "
  "out that a compass points to the center of the island. So they just go as "
  "far towards the center as they can, and hide their treasure there.",
  
  "Die hyperbolischen Piraten haben für Schatzkarten keinen Gebrauch. Allerdings "
  "fanden sie heraus, dass ein Kompass in die Mitte einer Insel zeigt -- "
  "also gehen sie so weit dort hin, wie sie können, und verstecken dort ihre Schätze.")

S("Just a nasty shark.", "Ist bloß ein fieser Hai.")

S("Parrots feel safe in the forests of Caribbean, so they "
    "never leave them. But they will help the Pirates by attacking the intruders.",

  "Papageien fühlen sich sicher in den karibischen Wäldern, daher verlassen sie diese nie. "
  "Allerdings helfen sie den Piraten beim Angreifen der Eindringlinge.")

S("Just a typical hyperbolic pirate.", "Ist bloß ein typischer, hyperbolischer Pirat. Mit Augenklappe und so.")

S(
    "Normally, the power of most Orbs slowly fades away, even when "
    "you are not actively using them. This Orb prevents this.\n\n"
    
    "When you have the Orb of Time, Orbs which are not active won't lose their power. "
    "Orbs are considered active if they have a continuous power which has actually "
    "affected something in the last turn.\n\n"
    
    "Orbs of Shielding remain active after being activated (even if you are no longer "
    "attacked), and Orbs of Time have a bigger cap inside their native Caribbean than "
    "outside.",
    
    "Normalerweise schwindet die Kraft der meisten Perlen, sogar wenn du sie nicht einsetzt. "
    "Die Perle der Zeit verhindert dies.\n\n"
    
    "Besitzt du eine, verlieren nicht-aktive Perlen keine Kraft. "
    "Perlen zählen als aktiv, wenn sie eine andauernde Kraft haben, die letzten Zug "
    "etwas beeinflusst hat.\n\n"
    
    "Perlen des Schutzes bleiben nach Aktivierung aktiv (auch wenn du nicht mehr angegriffen wirst), "
    "und Perlen der Zeit halten in der Karibik länger als an anderen Orten.")

// Achievements:

// Parrot : Find and collect Pirate Treasure.
// Caribbean Shark : Collect 10 Pirate Treasures.
// Pirate : Collect 25 Pirate Treasures.
// Master of Caribbean : Collect 50 Pirate Treasures.

// Red Rock Valley

N("Red Rock Valley", GEN_N, "Rotfelsental", "Rotfelsentäler", "im Rotfelsental")

N("Rock Snake", GEN_F, "Felsenschlange", "Felsenschlangen", "Felsenschlange")
N("Rock Snake Tail", GEN_M, "Felsenschlangenschweif", "Felsenschlangenschweifs", "Felsenschlangenschweif")
N("Red Troll", GEN_M, "Roter Troll", "Rote Trolle", "Roten Troll")
N("Red Gem", GEN_M, "Roter Stein", "Rote Steine", "Roten Stein")

N("rock I", GEN_M, "Fels I", "Felsen I", "Fels I")
N("rock II", GEN_M, "Fels II", "Felsen II", "Fels II")
N("rock III", GEN_M, "Fels III", "Felsen III", "Fels III")

Orb("Space", "des Raumes")

S("Not enough power for telekinesis!", "Nicht genügend Macht für Telekinese!")

S("This land contains high rock formations. Most of the valley is at level 0, "
  "while items are found at level 3. It is impossible to gain two or more levels, "
  "or to lose three levels, in a single move, (attacks are possible at any "
  "difference, though). Kill Red Trolls and Rock Snakes to make a cell higher.",

  "Dieses Land enthält hohe Fels-Formationen. Der Großteil des Tals ist auf Ebene 0, "
  "aber die meisten Gegenstände finden sich auf Ebene 3 (III). "
  "Du kannst immer nur eine Ebene rauf (0 > I > II > III), oder bis zu zwei runter. "
  "Angriffe sind auf jeder Höhe möglich. Töte Rote Trolle und Felsenschlangen, um eine "
  "Zelle zu erhöhen.")

S("A kind of Troll native to the Red Rock Valley.",
  "Eine spezielle Troll-Gattung, die im Rotfelsental lebt.")

S(
  "Rock snakes are similar to Sandworms, but they move each turn. However, they "
  "only move on the hexagonal cells. They are also somewhat longer than the Sandworms.",
  
  "Felsenschlangen sind Sandwürmern ähnlich, allerdings bewegen sie sich jeden Zug. "
  "Dafür meiden sie allerdings heptagonale Zellen. Sie sind auch länger als Sandwürmer.")

S("A gem from the Red Rock Valley.", "Ein Edelstein aus dem Rotfelsental.")

S(
    "This Orb is able to bring faraway items to your location, even if there are "
    "monsters or barriers on the way. The cost of "
    "bringing an item (in charges) equals the square of its distance to you. Contrary "
    "to some other Orbs, usage is not allowed if you have not enough power left.",
    
  "Diese Perle kann entfernte Gegenstände zu dir bringen, auch wenn Monster oder Barrieren im Weg sind. "
  "Die Ladungs-Kosten dafür ist die Distanz des Gegenstands zu dir zum Quadrat. "
  "Anders als ein paar andere Perlen kannst du diese nicht benutzen, wenn sie nicht genug Ladungen hat.")

// Climber : Find and collect a Red Gem.
// Red Troll : Collect 10 Red Gems.
// Rock Snake : Collect 25 Red Gems.
// Master of Red Rock : Collect 50 Red Gems.

// missing sentences

S("%The1 drowns!", "%Der1 %1 ertrinkt!");
S("%The1 falls!", "%Der1 %1 fällt!");

// these were missing from the translation for some reason

S("Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.",
  "Diese Seen sind überall in der Hölle... Sie sehen aus wie böse Sterne, und sind voller flammendem Schwefel.")

// Hardcore Mode

S("hardcore mode", "HARDCORE-MODUS");

S("One wrong move and it is game over!", "Eine falsche Bewegung kostet dich den Kopf!");
S("Not so hardcore?", "Doch nicht so hardcore?");

// Shoot'em up Mode

S("shoot'em up mode", "Baller-Modus");
S("Welcome to the Shoot'em Up mode!", "Willkommen im Baller-Modus!");
S("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move", "F/[;]/[LEER]/[ENTER]/[Numpad 5] = schießen, WASD/IJKL/[Numpad] = bewegen");

N("Rogue", GEN_M, "Schurke", "Schurken", "Schurken")
N("Knife", GEN_N, "Messer", "Messer", "Messer")
N("Flail", GEN_M, "Morgenstern", "Morgensterne", "Morgenstern")
N("Fireball", GEN_M, "Feuerball", "Feuerbälle", "Feuerball")
N("Tongue", GEN_F, "Zunge", "Zungen", "Zunge")

S("In the Shoot'em Up mode, you are armed with thrown Knives.",
  "Im Baller-Modus bist du mit Wurfmessern bewaffnet.")
S("A simple, but effective, missile, used by rogues.",
  "Eine einfache, aber effektive, Wurfwaffe. Schurken verwenden sie.")
S("This attack is likely to hit the attacker.",
  "Dieser Angriff trifft wahrscheinlich den Angreifer.")
S("This magical missile burns whatever it hits.", "Dieses magische Geschoss verbrennt alles, was sie trifft.")
S("Some monsters have long tongues, which allow them to attack enemies in nearby cells.",
  "Manche Monster haben lange Zungen, mit denen sie Feinde in nahen Zellen angreifen können.")
  
// modes for the local highscores

S(", m - mode: normal", ", m - Modus: Normal")
S(", m - mode: hardcore only", ", m - Modus: Hardcore")
S(", m - mode: shoot'em up", "m - Modus: Ballern")

// update description for Steam:

// (Shoot'em Up mode and two more lands than in the free version, and more are planned!)

// missing lines:

S("You would get hurt!", "Du würdest dir wehtun!")
S("PARTIAL", "TEILWEISE")

S("Cannot drop %the1 here!", "Du kannst %den1 %1 hier nicht fallenlassen!");

// Euclidean scores
S(" (E:%1)", " (E:%1)");

S("You cannot attack Rock Snakes directly!", "Du kannst Felsenschlangen nicht direkt angreifen!");

S("\"I am lost...\"", "\"Ich bin verloren...\"");

S("You are killed by %the1!", "%Der1 %1 tötet dich!")

// new menu for Version 7.1

S("(v) menu", "Menü - V")
S("return to the game", "zurück zum Spiel")
S("or 'v' to see the main menu", "oder V um das Menü einzublenden")

S("HyperRogue %1", "HyperRogue %1")
S("basic configuration", "Einstellungen")
S("advanced configuration", "(Erweitert)")
S("local highscores", "Highscores (Lokal)")
S("help", "Hilfe")
S("restart game", "Neustart")
S("special game modes", "Spielmodi")
S("save the game", "Speichern")
S("quit the game", "Beenden")
S("review your quest", "Quest anzeigen")
S("review the scene", "Die Szene anzeigen")
S("game over screen", "Game-Over-Bildschirm")
S("continue game", "Weiter")
S("play the game!", "Spielen!")

// fixed descriptions for Shmup achievements

S("You have used less knives than ever before. Congratulations!",
  "Du hast weniger Messer als zuvor benutzt. Gratulation!")
  
S("You have improved both your real time and knife count. Congratulations!",
  "Du hast sowohl weniger Zeit als auch Messer als zuvor benutzt. Gratulation!")
  
S("%1 knives (%2)", "%1 Messer (%2)")

// temporary
S("No description yet." ,"Noch keine Beschreibung...")

// special case when no Spice is visible
S("The sandworm explodes!", "Der Sandwurm explodiert!")

// Ocean

// achievements:
// Reached Ocean: Find and collect an Amber.
// Albatross: Collect 10 Ambers.
// Amber Albatross: Collect 25 Ambers.
// Master of the Tides: Collect 50 Ambers.

N("Ocean", GEN_M, "Ozean", "Ozeane", "im Ozean")

S(
    "You can collect some valuable Ambers on the coast, but beware the tides!\n\n"
    "You can also take one of the boats and venture into the Ocean, "
    "to find other coasts, dangerous whirlpools, and islands inhabited by Pirates.",

  "Du kannst Bernsteine auf den Küsten sammeln, aber achte auf die Gezeiten!\n\n"
  "Du kannst ebenso eines der Boote nehmen und in den Ozean reisen "
  "um andere Küsten, gefährliche Strudel, und piratenbesetzte Inseln zu finden.")

N("Albatross", GEN_M, "Albatross", "Albatrosse", "Albatross")

S(
  "Those large seabirds attack you for some reason. At least they are not "
  "as fast as Eagles...",
  "Diese großen Seevögel greifen dich aus irgendeinem Grund an. "
  "Wenigstens sind sie langsamer als Adler...")

N("stranded boat", GEN_N, "gestrandetes Boot", "gestrandete Boote", "gestrandete Boot")

S(
  "This boat cannot go through the sand. But if you sit inside and "
  "wait for the tide, you will be able to use it to travel through the Ocean.",

  "Dieses Boot kann nicht durch den Sand schwimmen. Wartest du allerdings bis zur Flut, "
  "kannst du damit durch den Ozean reisen."
  )    

N("Amber", GEN_M, "Bernstein", "Bernsteine", "Bernstein")

S(
  "When the tide is away, beautiful ambers can be found on the hyperbolic beaches. "
  "Perhaps there used to be a forest here?" ,
  "Bei Ebbe können wunderschöne Bernsteine auf den hyperbolischen Stränden gefunden werden. Vielleicht war hier früher ein Wald...?")

Orb("Air", "der Luft")

S("This Orb allows you to blow your enemies away.\n\n"
  "Click a monster to blow it one cell away. It cannot be used against mimics, ghosts, sharks and other monsters restricted to a specific terrain, and multi-tile monsters.",
  "Diese Perle erlaubt es dir, deine Feinde wegzupusten.\n\n"
  "Klicke auf ein Monster, um es eine Zelle wegzupusten. Die Perle ist wirkungslos gegenüber Mimiken, Geistern, Haien, und anderen Monstern, die nur auf bestimmten Terrain leben können. Monster, die größer als eine Zelle sind, sind auch immun."
  )
   

N("Sea Border", GEN_F, "Seegrenze", "Seegrenzen", "Seegrenze")

S("Border between seas.", "Eine Grenze zwischen Seen.")

// Whirlpool

// achievements:
// Escaped Whirlpool: Collect a Pearl, and escape the Whirlpool.
// Vortex Shark: Collect 10 Pearls.
// Vortex Pirate: Collect 25 Pearls.
// Master of the Whirlpool: Collect 50 Pearls.

N("Whirlpool", GEN_M, "Strudel", "Strudel", "im Strudel")

S(
  "Many lost boats are spinning in this whirlpool. Some of them contain treasures "
  "and Pirates.\n\n"
  "Unmanned boats will go one cell clockwise in each turn. Sharks and manned boats "
  "are only allowed to go with the current, that is, towards the center, or clockwise.",
  
  "Viele verlorene Boote wirbeln in diesem Strudel... manche enthalten Schätze oder Piraten.\n\n"
  "Unbemannte Boote bewegen sich jeden Zug eine Zelle im Uhrzeigersinn. "
  "Haie und bemannte Boote dürfen nur mit der Strömung schwimmen (das heißt, zur Mitte oder im Uhrzeigersinn)."
  )

N("Pearl", GEN_F, "Perle", "Perlen", "Perle")

S(
  "You do not know exactly why, but there are valuable pearls on many boats "
  "in the whirlpool.",
  "Du weißt nicht genau warum, aber es gibt schöne Perlen auf vielen Booten im Strudel."
  )

Orb("Water", "des Wassers")

S( "This Orb allows your boat to go against the current, "
   "and also to go into the land, creating water on the way.",
   "Diese Perle erlaubt es dir, gegen die Strömung zu schwimmen. "
   "Du kannst damit auch durch Land schwimmen (wobei du Wasser kreierst).")

S("You cannot go against the current!", "Du kannst nicht gegen die Strömung schwimmen!")

// Minefield

// achievements:
// Reached Minefield: Collect a Bomberbird Egg.
// Minesweeper: Collect 10 Bomberbird Eggs.
// Bomberbird: Collect 25 Bomberbird Eggs.
// Master of the Minefield: Collect 50 Bomberbird Eggs.

N("Minefield", GEN_N, "Minenfeld", "Minenfelder", "im Minenfeld")
N("minefield", GEN_F, "Mine?", "Minen?", "Mine?")
N("cell without mine", GEN_F, "minenlose Zelle", "minenlose Zellen", "minenlose Zelle")

S(
  "This minefield has been created by the Bomberbirds to protect their valuable eggs. "
  "Mines explode when they are stepped on, creating fire all around them "
  "(this fire never expires in the Minefield). If you don't step on a mine, you "
  "know the number of mines in cells around you. These numbers are marked with "
  "color codes on the cells you have visited.",
  "Dieses Minenfeld wurde von den Bombenvögeln erschaffen, um ihre wertvollen Eier zu schützen. "
  "Minen explodieren, wenn sie betreten werden, was Feuer um sie herum erzeugt (dieses erlischt im Minenfeld nicht). "
  "Wenn du nicht auf eine trittst, weißt du, wie viele Minen um dich herum sind. "
  "Diese Zahlen werden farblich markiert auf Zellen, die du besucht hast."
  )

N("Bomberbird", GEN_M, "Bombenvogel", "Bombenvögel", "Bombenvogel")

S(
    "Dark red birds who have created the minefield.\n\n"
    "They create a mine on the spot where they are killed, provided "
    "that the terrain is suitable. Also note that mines are triggered "
    "by dead birds falling on them.",

    "Dunkelrote Vögel, die das Minenfeld erschufen.\n\n"
    "Sie erzeugen eine Mine dort, wo sie sterben (vorausgesetzt, das Terrain passt). "
    "Tote, fallende Vögel lösen Minen auch aus!")

N("Tame Bomberbird", GEN_M, "Zahmer Bombenvogel", "Zahme Bombenvögel", "Zahmen Bombenvogel")
    
S(
  "This bomberbird will follow you at some distance, and attack your enemies. "
  "You can kill it to place a mine.",

  "Dieser Bombenvogel folgt dir auf Distanz, und attackiert deine Feinde. "
  "Du kannst ihn töten, um eine Mine zu legen.")

N("Bomberbird Egg", GEN_N, "Bombenvogel-Ei", "Bombenvogel-Eier", "Bombenvogel-Ei")
  
S(
  "Bomberbird eggs are big and tasty, and thus valuable. "
  "They can hatch when left alone for some time (but this will never happen "
  "if you are watching).",

  "Bombenvogel-Eier sind groß und schmackhaft - daher wertvoll. "
  "Sie können nach einiger Zeit schlüpfen (aber machen dies nie, wenn du zusiehst).")
  

Orb("Friendship", "der Freundschaft")

S("This Orb summons a friendly Bomberbird.",
  "Diese Perle ruft einen freundlich gesinnten Bombenvogel herbei.")

S("Still confused? Read the FAQ on the HyperRogue website!\n\n", 
  "Noch verwirrt? Lies das FAQ auf der HyperRogue-Webseite.\n\n")

S("Nowhere to blow %the1!", "Kein Platz um %den1 %a1 wegzupusten!")
S("%The1 is immune to wind!", "%Der1 %1 ist gegen immun gegen Wind!")
S("You blow %the1 away!", "Du bläst %den1 %a1 weg!")

S("WARNING: you are entering a minefield!", "ACHTUNG: Du betrittst ein Minenfeld!")
S("No mines next to you.", "Keine Minen um dich.")
S("A mine is next to you!", "1 Mine um dich!")
S("Two mines next to you!", "2 Minen um dich!")
S("Three mines next to you!", "3 Minen um dich!")
S("Four mines next to you!", "4 Minen um dich!")
S("Five mines next to you!", "5 Minen um dich!")
S("Six mines next to you!", "6 Minen um dich!")
S("Seven mines next to you!", "7 Minen um dich!")

S("You have to run away from the water!", "Du musst vor dem Wasser fliehen!")
#undef Orb

