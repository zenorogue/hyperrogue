// HyperRogue Czech translation, by Marek Čtrnáct <marek14@seznam.cz>
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

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

// MONSTERS
// ========

N("Yeti", GEN_M, "Yeti", "Yetiové", "Yetiho", "Yetim")
N("Icewolf", GEN_M, "Ledový vlk", "Ledoví vlci", "Ledového vlka", "Ledovým vlkem")
N("Ranger",  GEN_M, "Strážce", "Strážci", "Strážce", "Strážcem")
N("Rock Troll", GEN_M, "Šutroll", "Šutrollové", "Šutrolla", "Šutrollem")
N("Goblin", GEN_M, "Goblin", "Goblini", "Goblina", "Goblinem")
N("Sand Worm", GEN_M, "Písečný červ", "Píseční červi", "Písečného červa", "Písečným červem")
N("Sand Worm Tail", GEN_O, "Červí ocas", "Červí ocasy", "Červí ocas", "Červím ocasem")
N("Sand Worm W", GEN_M, "Písečný červ W", "Píseční červi W", "Písečného červa W", "Písečným červem W")
N("Desert Man", GEN_M, "Pouštní muž", "Pouštní muži", "Pouštního muže", "Pouštním mužem")
N("Ivy Root", GEN_O, "Kořen břečťanu", "Kořeny břečťanu", "Kořen břečťanu", "Kořenem břečťanu")
N("Active Ivy", GEN_O, "Aktivní břečťan", "Aktivní břečťany", "Aktivní břečťan", "Aktivním břečťanem")
N("Ivy Branch", GEN_F, "Větev břečťanu", "Větve břečťanu", "Větev břečťanu", "Větví břečťanu")
N("Dormant Ivy", GEN_M, "Neaktivní břečťan", "Neaktivní břečťany", "Neaktivní břečťan", "Neaktivním břečťanem")
N("Ivy N", GEN_O, "Břečťan N", "Břečťany N", "Břečťan N", "Břečťanem N")
N("Ivy D", GEN_O, "Břečťan D", "Břečťany D", "Břečťan D", "Břečťanem D")
N("Giant Ape", GEN_F, "Obří opice", "Obří opice", "Obří opici", "Obří opicí")
N("Slime Beast", GEN_M, "Slizový netvor", "Slizoví netvoři", "Slizového netvora", "Slizovým netvorem")
N("Mirror Image", GEN_O, "Zrcadlový obraz", "Zrcadlové obrazy", "Zrcadlový obraz", "Zrcadlovým obrazem")
N("Mirage", GEN_O, "Přelud", "Přeludy", "Přelud", "Přeludem")
N("Golem", GEN_M, "Golem", "Golemové", "Golema", "Golemem")
N("Eagle", GEN_M, "Orel", "Orli", "Orla", "Orlem")
N("Seep", GEN_M, "Průsak", "Průsakové", "Průsaka", "Průsakem")
N("Zombie", GEN_F, "Zombie", "Zombie", "Zombii", "Zombií")
N("Ghost", GEN_M, "Duch", "Duchové", "Ducha", "Duchem")
N("Necromancer", GEN_M, "Nekromancer", "Nekromanceři", "Nekromancera", "Nekromancerem")
N("Shadow", GEN_M, "Stín", "Stínové", "Stína", "Stínem")

//N("Tentacle", GEN_N, "Chapadlo", "Chapadla", "Chapadlo", "Chapadlem")
//N("Tentacle Tail", GEN_O, "Začátek chapadla", "Začátky chapadel", "Začátek chapadla", "Začátkem chapadla")

N("Tentacle", GEN_O, "Konec chapadla", "Konce chapadel", "Konec chapadla", "Koncem chapadla")
N("Tentacle Tail", GEN_N, "Chapadlo", "Chapadla", "Chapadlo", "Chapadlem")

N("Tentacle W", GEN_N, "Chapadlo W", "Chapadla W", "Chapadlo W", "Chapadlem W")
N("Tentacle (withdrawing)", GEN_F, "Chapadlo (ustupující)", "Chapadla (ustupující)", "Chapadlo (ustupující)", "Chapadlem (ustupujícím)")
N("Cultist", GEN_M, "Kultista", "Kultisté", "Kultistu", "Kultistou")
N("Fire Cultist", GEN_M, "Kultista ohně", "Kultisté ohně", "Kultistu ohně", "Kultistou ohně")
N("Greater Demon", GEN_M, "Ďábel", "Ďáblové", "Ďábla", "Ďáblem")
N("Lesser Demon", GEN_M, "Čert", "Čerti", "Čerta", "Čertem")
N("Ice Shark", GEN_M, "Ledový žralok", "Ledoví žraloci", "Ledového žraloka", "Ledovým žralokem")
N("Running Dog", GEN_M, "Běžící pes", "Běžící psi", "Běžícího psa", "Běžícím psem")
N("Demon Shark", GEN_M, "Ďábelský žralok", "Ďábelští žraloci", "Ďábelského žraloka", "Ďábelským žralokem")
N("Fire Fairy", GEN_M, "Ohnivý mužíček", "Ohniví mužíčci", "Ohnivého mužíčka", "Ohnivým mužíčkem")
N("Crystal Sage", GEN_M, "Křišťálový mudrc", "Křišťáloví mudrci", "Křišťálového mudrce", "Křišťálovým mudrcem")
N("Hedgehog Warrior", GEN_M, "Ježčí válečník", "Ježčí válečníci", "Ježčího válečníka", "Ježčím válečníkem")

// ITEMS
// =====

N("Ice Diamond", GEN_O, "Ledový diamant", "Ledové diamanty", "Ledový diamant", "Ledovým diamantem")
N("Gold", GEN_O, "Zlatý valoun", "Zlaté valouny", "Zlatý valoun", "Zlatým valounem")
N("Spice", GEN_N, "Koření", "Koření", "Koření", "Kořením")
N("Ruby", GEN_O, "Rubín", "Rubíny", "Rubín", "Rubínem")
N("Elixir of Life", GEN_O, "Elixír života", "Elixíry života", "Elixír života", "Elixírem života")
N("Shard", GEN_F, "Střepina", "Střepiny", "Střepinu", "Střepinou")
N("Necromancer's Totem", GEN_O, "Nekromancerův totem", "Nekromancerovy totemy", "Nekromancerův totem", "Nekromancerovým totemem")
N("Demon Daisy", GEN_N, "Čertovo kvítko", "Čertova kvítka", "Čertovo kvítko", "Čertovým kvítkem")
N("Statue of Cthulhu", GEN_F, "Cthulhuova soška", "Cthulhuovy sošky", "Cthulhuovu sošku", "Cthulhuovou soškou")
N("Phoenix Feather", GEN_N, "Fénixovo pírko", "Fénixova pírka", "Fénixovo pírko", "Fénixovým pírkem")
N("Ice Sapphire", GEN_O, "Ledový safír", "Ledové safíry", "Ledový safír", "Ledovým safírem")
N("Hyperstone", GEN_O, "Hyperkam", "Hyperkamy", "Hyperkam", "Hyperkamem")
N("Key", GEN_O, "Klíč", "Klíče", "Klíč", "Klíčem")
N("Dead Orb", GEN_F, "Mrtvá sféra", "Mrtvé sféry", "Mrtvou sféru", "Mrtvou sférou")
N("Fern Flower", GEN_O, "Kapradinový květ", "Kapradinové květy", "Kapradinový květ", "Kapradinové květy")

// ORBS: we are using a macro here
// ===============================

#define Orb(E, P) N("Orb of " E, GEN_F, "Sféra " P, "Sféry " P, "Sféru " P, "Sférou " P)
N("Orb of Yendor", GEN_F, "Yendorská sféra", "Yendorské sféry", "Yendorskou sféru", "Yendorskou sférou")
Orb("Storms", "Bouří")
Orb("Flash", "Záblesku")
Orb("Winter", "Zimy")
Orb("Speed", "Rychlosti")
Orb("Life", "Života")
Orb("Shielding", "Štítu")
// Orb("Digging", "Kopání")
Orb("Teleport", "Teleportace")
Orb("Safety", "Bezpečnosti")
Orb("Thorns", "Trnů")

// TERRAIN FEATURES
// ================

N("none", GEN_N, "nic", "nic", "nic", "ničím")
N("ice wall", GEN_F, "ledová zeď", "ledové zdi", "ledovou zeď", "ledovou zdí")
N("great wall", GEN_F, "velká zeď", "velké zdi", "velkou zeď", "velkou zdí")
N("red slime", GEN_O, "červený sliz", "červené slizy", "červený sliz", "červeným slizem")
N("blue slime", GEN_O, "modrý sliz", "modré slizy", "modrý sliz", "modrým slizem")
N("living wall", GEN_F, "živoucí zeď", "živoucí zdi", "živoucí zeď", "živoucí zdí")
N("living floor", GEN_F, "živoucí podlaha", "živoucí podlahy", "živoucí podlahu", "živoucí podlahou")
N("dead troll", GEN_M, "mrtvý troll" ,"mrtví trollové", "mrtvého trolla", "mrtvým trollem")
N("sand dune", GEN_F, "písečná duna", "písečné duny", "písečnou dunu", "písečnou dunou")
N("Magic Mirror", GEN_N, "Kouzelné zrcadlo", "Kouzelná zrcadla", "Kouzelné zrcadlo", "Kouzelným zrcadlem")
N("Cloud of Mirage", GEN_O, "Oblak přeludů", "Oblaky přeludů", "Oblak přeludů", "Oblakem přeludů")
N("Thumper", GEN_O, "Tlouk", "Tlouky", "Tlouk", "Tloukem")
N("Bonfire", GEN_N, "Ohniště", "Ohniště", "Ohniště", "Ohništěm")
N("ancient grave", GEN_O, "starý hrob", "staré hroby", "starý hrob", "starým hrobem")
N("fresh grave", GEN_O, "čerstvý hrob", "čerstvé hroby", "čerstvý hrob", "čerstvým hrobem")
N("column", GEN_O, "sloup", "sloupy", "sloup", "sloupem")
N("lake of sulphur", GEN_N, "sirné jezírko", "sirná jezírka", "sirné jezírko", "sirným jezírkem")
N("lake", GEN_N, "jezero", "jezera", "jezero", "jezerem")
N("frozen lake", GEN_N, "zamrzlé jezero", "zamrzlá jezera", "zamrzlé jezero", "zemrzlým jezerem")
N("chasm", GEN_F, "propast", "propasti", "propast", "propastí")
N("big tree", GEN_O, "velký strom", "velké stromy", "velký strom", "velkým stromem")
N("tree", GEN_O, "strom", "stromy", "strom", "stromem")

// LANDS
// =====

N("Great Wall", GEN_F, "Velká zeď", "Velké zdi", "Velkou zeď", "na Velké zdi")
N("Crossroads", GEN_F, "Křižovatka", "Křižovatky", "Křižovatku", "na Křižovatce")
N("Desert", GEN_F, "Poušť", "Pouště", "Poušť", "na Poušti")
N("Icy Land", GEN_O, "Ledový kraj", "Ledové kraje", "Ledový kraj", "v Ledovém kraji")
N("Living Cave", GEN_F, "Živoucí jeskyně", "Živoucí jeskyně", "Živoucí jeskyni", "v Živoucí jeskyni")
N("Jungle", GEN_F, "Džungle", "Džungle", "Džungli", "v džungli")
N("Alchemist Lab", GEN_F, "Alchymistická laboratoř", "Alchymistické laboratoře", "Alchymistickou laboratoř", "v Alchymistické laboratoři")
N("Mirror Land", GEN_O, "Kraj zrcadel", "Kraje zrcadel", "Kraj zrcadel", "v Kraji zrcadel")
N("Graveyard", GEN_O, "Hřbitov", "Hřbitovy", "Hřbitov", "na Hřbitově")
N("R'Lyeh", GEN_N, "R'Lyeh", "R'Lyeh", "R'Lyeh", "v R'Lyeh")
N("Hell", GEN_N, "Peklo", "Pekla", "Peklo", "v Pekle")
N("Cocytus", GEN_O, "Cocytus", "Cocyty", "Cocytus", "v Cocytu")
N("Land of Eternal Motion", GEN_O, "Kraj věčného pohybu", "Kraje věčného pohybu", "Kraj věčného pohybu", "v Kraji věčného pohybu")
N("Dry Forest", GEN_O, "Suchý hvozd", "Suché hvozdy", "Suchý hvozd", "v Suchém hvozdu")
N("Game Board", GEN_F, "Herní deska", "Herní desky", "Herní desku", "na Herní desce")

// GAME MESSAGES
// =============

// fighting messages

// For each English form, provide a Polish form. Player is referred to via %...0,
// and objects are referred to via %...1 and %...2. For example, in Polish:

// %a1 refers to the accusative form of the first object (as explained above in 'Nouns')
// likewise, %a2 refers to the accusative form of the second object
// %ł1 is replaced by "ł", "ło" or "ła", depending on the gender of %1
// %łeś0 adds "łeś" or "łaś" depending on the gender of the player

// Use whatever %xxx# codes you need for your language. Of course,
// tell me how your codes should be expanded.

// OK, in Czech let's use %l1 for "l", "la" or "lo", depending on the gender of %1 (masculine, feminine or neuter)
// %ý1 is "ý" for masculina, "á" for feminine and "é" for neuter.
// %l0 would be "l" or "la" for the player
// %ůj1 is "ého" for masculine living (probably won't appear), "ůj" for masculine object, "ou" for feminine and "é" for neuter.
// %ým1 is "ým" for masculine, "ou" for feminine and "ým" again for neuter.
// %d0 is nothing for male player and "a" for female player.

S("You kill %the1.", "Zabi%l0 jsi %a1.")
S("You would be killed by %the1!", "To by tě zabi%l1 %1!")
S("%The1 would get you!", "To by tě dosta%l1 %1!")
S("%The1 destroys %the2!", "%1 zniči%l1 %a2!")
S("%The1 eats %the2!", "%1 sežra%l1 %a2!")

// Here, Czech has two possible words for eat, one used for humans and one for other creatures. I presume human enemies don't eat objects in the game.

S("The ivy destroys %the1!", "Břečťan zničil %a1!")
S("%The1 claws %the2!", "%1 sek%l1 %a2!")
S("%The1 scares %the2!", "%1 vyděsi%l1 %a2!")
S("%The1 melts away!", "%1 roztá%l1!")
S("%The1 stabs %the2.", "%1 probod%l1 %a2.")
S("You stab %the1.", "Probod%l0 jsi %a1.")

S("You cannot attack %the1 directly!", "Na %a1 nemůžeš útočit přímo!")
S("Stab them by walking around them.", "Probodni ho tím, že ho obejdeš.")

S("You feel more experienced in demon fighting!", "Cítíš se zkušenější v boji proti pekelným démonům!")
S("Cthulhu withdraws his tentacle!", "Cthulhu stahuje své chapadlo!")
S("The sandworm explodes in a cloud of Spice!", "Písečný červ vybuchl v oblaku Koření!")
S("%The1 is confused!", "%1 je zmaten%ý1.")
S("%The1 raises some undead!", "%1 probudil nemrtvé!")
S("%The1 throws fire at you!", "%1 po tobě vystřelil oheň!")
S("%The1 shows you two fingers.", "%1 ti ukáza%l1 dva prsty.")
S("You wonder what does it mean?", "Co to může znamenat?")
S("%The1 shows you a finger.", "%1 ti ukáz%l1 jeden prst.")
S("You think about possible meanings.", "Přemýlíš, co by to mohlo znamenat.")
S("%The1 moves his finger downwards.", "%1 pohnu%l1 prstem dolů.")
S("Your brain is steaming.", "Ze tvého mozku se kouří.")
S("%The1 destroys %the2!", "%1 zniči%l1 %a2!")
S("You join %the1.", "Spoji%l0 ses %s1 %abl1.")

// This is a bit troublesome. The "s" preposition ("with") has two forms, "s" and "se", depending on what it's used with. And Mirror Image and Mirage use different forms.

S("Two of your images crash and disappear!", "Dva tvoje obrazy se srazily a zmizely!")
S("%The1 breaks the mirror!", "%1 rozbi%l1 zrcadlo!")
S("%The1 disperses the cloud!", "%1 rozptýli%l1 oblak!")
S("You activate the Flash spell!", "Aktivova%l0 jsi kouzlo Záblesk!")
S("You activate the Lightning spell!", "Aktivova%l0 jsi kouzlo Blesk!")
S("Ice below you is melting! RUN!", "Led pod tebou taje! UTÍKEJ!")
S("This spot will be burning soon! RUN!", "Za chvíli to tu začne hořet! UTÍKEJ!")
S("The floor has collapsed! RUN!", "Podlaha se propadla! UTÍKEJ!")
S("You need to find the right Key to unlock this Orb of Yendor!",  "Abys moh%l0 odemknout tuto Yendorskou sféru, musíš najít správný klíč!")
S("You fall into a wormhole!", "Spad%l0 jsi do červí díry!")
S("You activate %the1.", "Aktivova%l0 jsi %a1.")
S("No room to push %the1.", "%a1 není kam zatlačit.")
S("You push %the1.", "Zatlači%l0 jsi %a1.")
S("You start chopping down the tree.", "Zača%l0 jsi kácet strom.")
S("You chop down the tree.", "Pokáce%l0 jsi strom.")
S("You cannot attack Sandworms directly!", "Na Písečné červy nemůžeš útočit přímo!")
S("You cannot attack Tentacles directly!", "Na Chapadla nemůžeš útočit přímo!")
S("You cannot defeat the Shadow!", "Stín nemůžeš porazit!")
S("You cannot defeat the Greater Demon yet!", "Ďábla ještě porazit nemůžeš!")
S("That was easy, but groups could be dangerous.", "To bylo snadné, ale skupiny by mohly být nebezpečné.")
S("Good to know that your fighting skills serve you well in this strange world.", "Je dobré vědět, že tvé bojové dovednosti ti v tomto podivném světě slouží dobře.")
S("You wonder where all these monsters go, after their death...", "Přemýšlíš, kam všichni ti netvoři odejdou, když zemřou...")
S("You feel that the souls of slain enemies pull you to the Graveyard...", "Cítíš, že tě duše zabitých nepřátel táhnou na Hřbitov...")
S("Wrong color!", "Špatná barva!")
S("You cannot move through %the1!", "Nemůžeš překonat %a1!")
S("%The1 would kill you there!", "Tam by tě zabi%l1 %1!")
S("Wow! %1! This trip should be worth it!", "Páni! %1! Tenhle výlet by měl stát za to!")
S("For now, collect as much treasure as possible...", "Prozatím sesbírej co nejvíc pokladů......")
S("Prove yourself here, then find new lands, with new quests...", "Ukaž tady, co umíš, a pak najdi nové kraje a nové úkoly...")
S("You collect your first %1!", "Naše%l0 jsi sv%ůj1 první %a1!")
S("You have found the Key! Now unlock this Orb of Yendor!", "Naše%l0 jsi Klíč! Teď odemkni tuto Yendorskou sféru!")
S("This orb is dead...", "Tato sféra je mrtvá...")
S("Another Dead Orb.", "Další mrtvá sféra.")
S("You have found %the1!", "Naše%l0 jsi %a1!")
S("You feel that %the2 become%s2 more dangerous.", "Cítíš, že %2 začíná být nebezpečnější.")
S("With each %1 you collect...", "S každ%ým1 %abl1, který sebereš...")
S("Are there any magical orbs in %the1?...", "Jsou %abl1 nějaké magické sféry?")
S("You feel that %the1 slowly become%s1 dangerous...", "Máš pocit, že %1 pomalu začíná být nebezpečn%ý...")
S("Better find some other place.", "Raději bys mě%l0 najít nějaké jiné místo.")
S("You have a vision of the future, fighting demons in Hell...", "Máš vizi budoucnosti; vidíš se, jak bojuješ s pekelnými démony...")
S("With this Elixir, your life should be long and prosperous...", "S tímto elixírem bys měl žít dlouho a blaze...")
S("The Necromancer's Totem contains hellish incantations...", "Nekromancerův totem obsahuje pekelná zaklínání...")
S("The inscriptions on the Statue of Cthulhu point you toward your destiny...", "Nápis na Cthulhuově sošce tě směruje ke tvému osudu...")
S("Still, even greater treasures lie ahead...", "Ale dál leží ještě větší poklady...")
S("You collect %the1.", "Sebra%l0 jsi %a1.")
S("CONGRATULATIONS!", "GRATULUJEME!")
S("Collect treasure to access more different lands...", "Sbírej poklady, abys získal přístup do dalších, odlišných krajů...")
S("You feel that you have enough treasure to access new lands!", "Cítíš, že už máš dost pokladů na to aby ses dostal do nových krajů!")
S("Collect more treasures, there are still more lands waiting...", "Sbírej další poklady, další kraje stále čekají...")
S("You feel that the stars are right, and you can access R'Lyeh!", "Cítíš, že postavení hvězd je správné, a ty můžeš vstoupit do R'Lyeh!")
S("Kill monsters and collect treasures, and you may get access to Hell...", "Zabíjej netvory a sbírej poklady a možná najdeš cestu do Pekla...")
S("To access Hell, collect %1 treasures each of %2 kinds...", "Aby ses dostal do Pekla, sesbírej %1 pokladů od každého z %2 různých typů...")
S("Abandon all hope, the gates of Hell are opened!", "Zanech vší naděje, brány Pekla jsou otevřeny!")
S("And the Orbs of Yendor await!", "A Yendorské sféry čekají!")
S("You switch places with %the1.", "Vyměni%l0 sis místo s %abl1.")
S("You rejoin %the1.", "Znovu ses spoji%l0 %s1 %abl1.")

// This is a bit troublesome. The "s" preposition ("with") has two forms, "s" and "se", depending on what it's used with. And Mirror Image and Mirage use different forms.

S("The mirror shatters!", "Zrcadlo se roztříštilo!")
S("The cloud turns into a bunch of images!", "Oblak se proměnil v několik obrazů!")
S("The slime reacts with %the1!", "Sliz reaguje s %abl1!")
S("You drop %the1.", "Položi%l0 jsi %a1.")
S("You feel great, like a true treasure hunter.", "Cítíš se skvěle, jako pravý lovec pokladů.")
S("Your eyes shine like gems.", "Tvé oči září jako drahokamy.")
S("Your eyes shine as you glance at your precious treasures.", "Tvé oči září, když se díváš na své převzácné poklady.")
S("You glance at your great treasures.", "Díváš se na své velké poklady.")
S("You glance at your precious treasures.", "Díváš se na své převzácné poklady.")
S("You glance at your precious treasure.", "Díváš se na svůj převzácný poklad.")
S("Your inventory is empty.", "Nic u sebe nemáš.")
S("You teleport to a new location!", "Teleportuješ se na nové místo!")
S("Could not open the score file: ", "Nepodařilo se otevřít soubor se skóre: ")
S("Game statistics saved to %1", "Herní statistika uložena do %1")
S("Game loaded.", "Hra nahrána.")
S("You summon some Mimics!", "Vyvola%l0 jsi několik Přeludů!")
S("You summon a golem!", "Vyvola%l0 jsi golema!")
S("You will now start your games in %1", "Odteď budeš začínat hru %abl1")
S("Activated the Hyperstone Quest!", "Aktivoval jsi Hyperkamový úkol!")
S("Orb power depleted!", "Síla sféry se vyčerpala!")
S("Orbs summoned!", "Vyvola%l0 jsi sféry!")
S("Orb power gained!", "Získa%l0 jsi moc sféry!")
S("Dead orbs gained!", "Získa%l0 jsi Mrtvé sféry!")
S("Orb of Yendor gained!", "Získa%0l jsi Yendorskou sféru!")
S("Treasure gained!", "Získa%l0 jsi poklad!")
S("Lots of treasure gained!", "Získa%l0 jsi spoustu pokladů!")
S("You summon a sandworm!", "Vyvola%l0 jsi Písečného červa!")
S("You summon an Ivy!", "Vyvola%l0 jsi Břečťan!")
S("You summon a monster!", "Vyvola%l0 jsi netvora!")
S("You summon some Thumpers!", "Vyvola%l0 jsi několikl Tlouků!")
S("You summon a bonfire!", "Vyvola%l0 jsi ohniště!")
S("Treasure lost!", "Ztrati%l0 jsi poklad!")
S("Kills gained!", "Získa%l0 jsi zabité netvory!")
S("Activated Orb of Safety!", "Aktivova%l0 jsi Sféru bezpečí!")
S("Teleported to %1!", "Teleportova%l0 ses; nyní jsi %abl1")

// Had to modify this a bit to avoid necessity of another string for lands. Now it's "You teleported; now you are at <land>"

S("Welcome to HyperRogue", "Vítej ve hře HyperRogue")
S(" for Android", " na Android")
S(" for iOS", " na iOS")
S("! (version %1)\n\n", "! (verze %1)\n\n")
S(" (press ESC for some hints about it).", " (stiskni ESC pro trochu nápovědy).")
S("Press 'c' for credits.", "Stiskni 'c' pro zobrazení autorů")
S("game design, programming, texts and graphics by Zeno Rogue <zeno@attnam.com>\n\n",
  "design, programování, texty a grafika: Zeno Rogue <zeno@attnam.com>\n\n")
S("add credits for your translation here", "Český překlad: Marek Čtrnáct <marek14@seznam.cz>\n\n")
S(" (touch to activate)", " (aktivuj dotekem)")
S(" (expired)", " (vypršelo)")
S(" [%1 turns]", " [počet kol: %1]")
S(", you", ", ty")
S("0 = Klein model, 1 = Poincaré model", "0 = Kleinův model, 1 = Poincarého model")
S("you are looking through it!", "díváš se skrz něj!")
S("simply resize the window to change resolution", "rozlišení můžeš změnit prostou změnou velikosti okna")
S("[+] keep the window size, [-] use the screen resolution", "[+] zachovej rozměr okna, [-] použij rozlišení obrazovky")
S("+5 = center instantly, -5 = do not center the map", "+5 = okamžité vycentrování, -5 = žádné centrování")
S("press Space or Home to center on the PC", "na PC můžeš vycentrovat obraz stisknutím mezerníku nebo klávesy Home")
S("You need special glasses to view the game in 3D", "Abys moh%l0 hru sledovat ve 3D, potřebuješ speciální brýle")

//Would %thel0 still work here?

S("You can choose one of the several modes", "Můžeš si vybrat jeden z několika módů")
S("ASCII", "ASCII")
S("black", "černý")
S("plain", "obyčejný")
S("Escher", "escherovský")
S("items only", "pouze předměty")
S("items and monsters", "předměty i netvoři")
S("no axes", "bez os")
S("auto", "auto")
S("light", "lehký")
S("heavy", "těžký")
S("The axes help with keyboard movement", "Osy pomáhají při pohybu pomocí klávesnice")
S("Config file: %1", "Soubor s nastavením: %1")
S("joystick mode: automatic (release the joystick to move)", "mód joysticku: automatický (pohybuješ se uvolněním joysticku)")
S("joystick mode: manual (press a button to move)", "mód joysticku: ruční (pohybuješ se stiskem tlačítka)")
S("Reduce the framerate limit to conserve CPU energy", "Sniž fps, abys šetřil energii procesoru")
S("Press F1 or right click for help", "Pro vyvolání nápovědy stiskni F1 nebo klikni pravým tlačítkem myši")
S("No info about this...", "O tomhle nemám žádné informace...")
S("Press Enter or F10 to save", "Enter nebo F10: uložit hru")
S("Press Enter or F10 to quit", "Enter nebo F10: ukončit hru")
S("or 'r' or F5 to restart", "'r' nebo F5: začít znovu od začátku")
S("or 't' to see the top scores", "'t': tabulka nejvyšších skóre")
S("or another key to continue", "jakákoli jiná klávesa: pokračovat")
S("It is a shame to cheat!", "Cheatovat je hanba!")

//While the word "cheat" can be translated into Czech, it's tricky using it as countable noun, and the English-derived terms are well-known.

S("Showoff mode", "Předváděcí mód")
S("Quest status", "Stav úkolů")
S("GAME OVER", "KONEC HRY")
S("Your score: %1", "Tvé skóre: %1")
S("Enemies killed: %1", "Zabitých netvorů: %1")
S("Orbs of Yendor found: %1", "Nalezených Yendorských sfér: %1")
S("Collect %1 $$$ to access more worlds", "Další kraje zpřístupníš sesbíráním %1 $$$")
// S("Collect %1 $$$ to access R'Lyeh and Dry Forest", "R'Lyeh a Suchý hvozd zpřístupníš sesbíráním %1 $$$")
S("Collect at least %1 treasures in each of %2 types to access Hell", "Peklo zpřístupníš sesbíráním %1 pokladů od každého z %2 typů")
S("Collect at least %1 Demon Daisies to find the Orbs of Yendor", "Yendorské sféry zpřístupníš sesbíráním nejméně %1 Čertových kvítek")
S("Hyperstone Quest: collect at least %3 %1 in %the2", "Hyperkamový úkol: sesbírej nejméně %3 pokladů %abl2")
S("Hyperstone Quest completed!", "Hyperkamový úkol splněn!")
S("Look for the Orbs of Yendor in Hell or in the Crossroads!", "Hledej Yendorské sféry v Pekle nebo na Křižovatce!")
S("Unlock the Orb of Yendor!", "Odemkni Yendorskou sféru!")
S("Defeat %1 enemies to access the Graveyard", "Hřbitov zpřístupníš poražením %1 nepřátel")
S("(press ESC during the game to review your quest)", "(své úkoly můžeš během hry zobrazit stiskem ESC)")
S("you have cheated %1 times", "počet cheatů: %1")
S("%1 turns (%2)", "počet kol: %1 (%2)")
S("last messages:", "poslední zprávy: ")
S("time elapsed", "čas")
S("date", "datum")
S("treasure collected", "získané poklady")
S("total kills", "celkový počet zabitých nepřátel")
S("turn count", "počet kol")
S("cells generated", "počet vygenerovaných políček")
S("t/left/right - change display, up/down - scroll, s - sort by", "t/doleva/doprava - změna zobrazení, nahoru/dolů - posunování, s - řazení")
S("kills", "zab")
S("time", "čas")
S("ver", "ver")
S("SORT", "ŘAZ")
S("PLAY", "HRA")
S("Your total wealth", "Tvé celkové bohatství")
S("treasure collected: %1", "pokladů získáno: %1")
S("objects found: %1", "předmětů nalezeno: %1")
S("orb power: %1", "moc sfér: %1")
S(" (click to drop)", " (polož kliknutím)")
S("You can also scroll to the desired location and then press 't'.", "Také můžeš posunout obraz do požadované lokace a stisknout 't'.")
S("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n", "To je tedy potenciálně užitečné pro extrémně dlouhé hry, které by jinak sežraly všechnu paměť tvého počítače.")
S("You can touch the Dead Orb in your inventory to drop it.", "Mrtvou sféru ve svém inventáři můžeš položit tím, že se jí dotkneš.")
S("This might be useful for Android devices with limited memory.", "To se může hodit na zařízeních Android s omezenou pamětí.")
S("You can press 'g' or click them in the list to drop a Dead Orb.", "Mrtvou svéru můžeš položit stiskem klávesy 'g' nebo tím, že na ni klikneš v seznamu.")
S("frames per second", "snímky za sekundu")
S("monsters killed: %1", "netvorů zabito: %1")
S("Drawing %1 (layer %2), F1 for help", "Kreslím %1 (vrstva %2), F1 - nápověda")
S("hepta floor", "hepta-podlaha")
S("hexa floor", "hexa-podlaha")
S("character", "postava")
S("ESC for menu/quest", "ESC - menu/úkoly")
S("vector graphics editor", "editor vektorové grafiky")
S("cheat mode", "cheat mód")
S("heptagonal game board", "sedmiúhelníková herní deska")
S("triangular game board", "Trojúhelníková herní deska")
S("HyperRogue game board", "herní deska HyperRogue")
S("first page [Space]", "první stránka [mezerník]")
S("Configuration:", "Nastavení:")
S("video resolution", "rozlišení")
S("fullscreen mode", "zobrazení na celou obrazovku")
S("animation speed", "rychlost animace")
S("dist from hyperboloid ctr", "vzdálenost od středu hyperboloidu")
S("scale factor", "měřítko")
S("wall display mode", "mód zobrazování zdí")
S("monster display mode", "mód zobrazování netvorů")
S("cross display mode", "mód zobrazování os")
S("background music volume", "hlasitost hudby")
S("OFF", "VYP")
S("ON", "ZAP")
S("distance between eyes", "vzdálenost očí od sebe")
S("framerate limit", "limit fps")
S("joystick mode", "mód joysticku")
S("automatic", "automatický")
S("manual", "ruční")
S("language", "jazyk")
S("EN", "CZ")
S("player character", "tvoje postava")
S("male", "muž")
S("female", "žena")
S("use Shift to decrease and Ctrl to fine tune ", "použij Shift ke zmenšení a Ctrl k jemnému")
S("(e.g. Shift+Ctrl+Z)", "ladění (například Shift+Ctrl+Z)")
S("the second page [Space]", "druhá stránka [mezerník]")
S("special features [Space]", "zvláštní možnosti [mezerník]")
S("see the help screen", "zobrazit obrazovku s nápovědou")
S("save the current config", "uložit současné nastavení")
S("(v) config", "(v) nast.")
S("Screenshot saved to %1", "Snímek obrazovky uložen do %1")
S("You need an Orb of Teleport to teleport.", "K teleportaci potřebuješ Sféru teleportace.")
S("Use arrow keys to choose the teleport location.", "Vyber cíl teleportace pomocí šipek.")
S("openGL mode enabled", "mód openGL zapnut")
S("openGL mode disabled", "mód openGL vypnut")
S("openGL & antialiasing mode", "openGL & antialiasing")
S("anti-aliasing enabled", "anti-aliasing zapnut")
S("anti-aliasing disabled", "anti-aliasing vypnut")
S("You activate your demonic powers!", "Aktivoval jsi svou démonickou moc!")

// Steam achievement messages
// --------------------------

S("New Achievement:", "Nový achievement:")
S("Your total treasure has been recorded in the Steam Leaderboards.", "Tvůj celkový poklad byl zaznamenán na žebříčky na Steamu.")
S("Congratulations!", "Gratulujeme!")
S("You have improved your total high score and %1 specific high scores!", "Zlepši%l0 jsi své celkové nejvyšší skóre a %1 specifických nejvyšších skóre!")
S("You have improved your total and '%1' high score!", "Zlepši%l0 jsi své celkové nejvyšší skóre a své nejvyšší skóre v kategorii '%P1!'")
S("You have improved your total high score on Steam. Congratulations!", "Zlepši%l0 jsi své celkové nejvyšší skóre na Steamu. Gratulujeme!")
S("You have improved %1 of your specific high scores!", "Zlepši%l0 jsi %1 svých specifických nejvyšších skóre!")
S("You have improved your '%1' high score on Steam!", "Zlepši%l0 jsi své nejvyšší skóre na Steamu v kategorii '%P1'!")
S("You have collected 10 treasures of each type.", "Získa%l0 jsi 10 pokladů od každého typu.")
S("This is your first victory!", "Toto je tvé první vítězství!")
S("This has been recorded in the Steam Leaderboards.", "Bylo zaznamenáno na žebříčky Steamu.")
S("The faster you get here, the better you are!", "Čím rychleji se sem dostaneš, tím lepší jsi!")
S("You have improved both your real time and turn count. Congratulations!", "Zlepši%l0 jsi svůj reálný čas i svůj počet kol. Gratulujeme!")
S("You have used less real time than ever before. Congratulations!", "Spotřebova%l0 jsi méně reálného času než kdy předtím. Gratulujeme!")
S("You have used less turns than ever before. Congratulations!", "Spotřebova%l0 jsi méně kol než kdy předtím. Gratulujeme!")

// help texts
// ----------

// These are separated into multiple lines just for convenience,
// you don't have to follow.

S(
 "You have been trapped in a strange, non-Euclidean world. Collect as much treasure as possible "
 "before being caught by monsters. The more treasure you collect, the more "
 "monsters come to hunt you, as long as you are in the same land type. The "
 "Orbs of Yendor are the ultimate treasure; get at least one of them to win the game!",

 "Jsi uvězněn v podivném, neeukleidovském světě. Sesbírej co nejvíce pokladů, "
 "než tě dostanou netvoři. Čím víc pokladů sebereš, tím víc netvorů po tobě půjde, "
 "dokud budeš ve stejném typu kraje. Největším pokladem jsou Yendorské sféry: "
 "získáš-li alespoň jednu z nich, vyhraješ hru!"
 )

S(
 "You can fight most monsters by moving into their location. "
 "The monster could also kill you by moving into your location, but the game "
 "automatically cancels all moves which result in that.\n\n",

 "Na většinu netvorů můžeš zaútočit tím, že se pohneš na jejich políčko. "
 "Netvor může zabít tebe, pokud se pohne na tvé políčko, ale hra automaticky "
 "zruší každý tah, který by vedl k takové situaci.\n\n")

S(
 "Usually, you move by touching somewhere on the map; you can also touch one "
 "of the four buttons on the map corners to change this (to scroll the map "
 "or get information about map objects). You can also touch the "
 "numbers displayed to get their meanings.\n",
 "Pohybuješ se obvykle tak, že se dotkneš nějakého místa na mapě; to můžeš změnit "
 "tím, že se dotkneš jednoho ze čtyř tlačítek v rohu mapy (pokud chceš posunovat mapu "
 "nebo získat informace o objektech na mapě). Také se můžeš dotknout zobrazených čísel, "
 "abys zjistil jejich význam.\n")

S("Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Space. "
    "To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n",
 "Pohybuješ se pomocí myši, numerické klávesnice, kláves qweadzxc nebo kláves hjklyubn. Klávesy 's' nebo '.' ti umožňují čekat na místě. "
 "Šipky, klávesy PgUp/PgDn a mezerník ti umožňují otáčet svět. Nastavení můžeš otevřít klávesou 'v', úkoly a menu klávesou ESC.\n\n")

S("See more on the website: ", "Více informací najdeš na webové stránce: ")

S("special thanks to the following people for their bug reports, feature requests, porting, and other help:\n\n%1\n\n",
  "zvláštní poděkování následujícím lidem za hlášení chyb, žádosti o nové funkce, porty a další pomoc:\n\n%1\n\n")

S(
 "The total value of the treasure you have collected.\n\n"
 "Every world type contains a specific type of treasure, worth 1 $$$; "
 "your goal is to collect as much treasure as possible, but every treasure you find "
 "causes more enemies to hunt you in its native land.\n\n"
 "Orbs of Yendor are worth 50 $$$ each.\n\n",

  "Celková hodnota pokladů, které jsi sesbíral.\n\n"
  "V každém typu kraje se nachází specifický typ pokladů, který má vždy cenu 1 $$$; "
  "tvým cílem je sesbírat co nejvíce pokladů, ale každý poklad, který získáš, "
  "způsobí, že tě v krajích příslušného typu bude pronásledovat více nepřátel.\n\n"
  "Každá Yendorská sféra má cenu 50 $$$.\n\n")

S(
 "The higher the number, the smoother the animations in the game. "
 "If you find that animations are not smooth enough, you can try "
 "to change the options ",
 "Čím vyšší číslo, tím hladší budou animace ve hře. Pokud se ti zdá, že animace "
 "nejsou dostatečně hladké, můžeš zkusit změnit nastavení ")

S("(Menu button) and select the ASCII mode, which runs much faster. "
 "Depending on your device, turning the OpenGL mode on or off might "
 "make it faster, slower, or cause glitches.",
 "(tlačítko 'Menu') a zvolit ASCII mód, který je mnohem rychlejší. Zapnutí "
 "či vypnutí módu OpenGL může, v závislosti na tvém zařízení, hru zrychlit, "
 "zpomalit, nebo vyvolat grafické chyby."
 )

S("(in the MENU). You can reduce the sight range, this should make "
 "the animations smoother.",
 "(v MENU). Můžeš snížit viditelnost - to by mělo vést k hladším animacím.")

S("(press v) and change the wall/monster mode to ASCII, or change "
 "the resolution.",
 "(stiskni 'v') a změň mód zdí/netvorů na ASCII, nebo změň rozlišení.")

S(
 "In this mode you can draw your own player character and floors. "
 "Mostly for the development purposes, but you can have fun too.\n\n"
 "f - floor, p - player (repeat 'p' for layers)\n\n"
 "n - new shape, u - copy the 'player body' shape\n\n"
 "1-9 - rotational symmetries, 0 - toggle axial symmetry\n\n"
 "point with mouse and: a - add point, m - move nearest point, d - delete nearest point, c - nearest point again, b - add after nearest\n\n"
 "s - save in C++ format (but cannot be loaded yet without editing source)\n\n"
 "z - zoom, o - Poincaré model\n",

 "V tomto módu můžeš nakreslit vlastní postavu a podlahu. "
 "Slouží především pro vývoj hry, ale můžeš se s ním i pobavit.\n\n"
 "f - podlaha, p - postava (opakovaným stiskem 'p' přepínáš mezi vrstvami)\n\n"
 "n - nový tvar, u - zkopíruj tvar 'tělo postavy'\n\n"
 "1-9 - rotační symetrie, 0 - zapnout/vypnout osovou symetrii\n\n"
 "zamiř myší a: a - přidej bod, m - přesuň nejbližší bod, d - vymaž nejbližší bod, c - znovu nejbližší bod, b - přidej za nejbližším\n\n"
 "s - ulož ve formátu C++ (bez editování zdroje však prozatím soubor nelze nahrát)\n\n"
 "z - přiblížení/vzdálení, o - Poincarého model\n")

S(
 "These huge monsters normally live below the sand, but your movements have "
 "disturbed them. They are too big to be slain with your "
 "weapons, but you can defeat them by making them unable to move. "
 "This also produces some Spice. They move two times slower than you.",

 "Tito obrovští netvoři normálně žijí pod pískem, ale tvé pohyby je vyrušily. Jsou "
 "tak velcí, že je nemůžeš zabít svými zbraněmi, ale můžeš je porazit tím, že jim "
 "znemožníš pohyb. Tím také vznikne nějaké Koření. Píseční červi se pohybují "
 "dvakrát pomaleji než ty.")

S("The tentacles of Cthulhu are like sandworms, but longer. "
 "They also withdraw one cell at a time, instead of exploding instantly.",
 "Cthulhuova chapadla jsou něco jako Píseční červi, ale delší. "
 "Kromě toho neexplodují okamžitě, ale ustupují políčko za políčkem.")

S(
 "A huge plant growing in the Jungle. Each Ivy has many branches, "
 "and one branch grows per each of your moves. Branches grow in a clockwise "
 "order. The root itself is vulnerable.",
 "Obrovská rostlina, která roste v Džungli. Každý Břečťan má mnoho "
 "větví, a po každém tvém pohybu jedna z nich povyroste. Větve "
 "rostou v pořadí po směru hodinových ručiček. Kořen "
 "Břečťanu je také zranitelný.")

S("The Alchemists produce magical potions from pools of blue and red slime. You "
 "can go through these pools, but you cannot move from a blue pool to a red "
 "pool, or vice versa. Pools containing items count as colorless, and "
 "they change color to the PC's previous color when the item is picked up. "
 "Slime beasts also have to keep to their own color, "
 "but when they are killed, they explode, destroying items and changing "
 "the color of the slime and slime beasts around them.",
 "Alchymisté vytvářejí magické lektvary z kaluží modrého a červeného slizu. "
 "Ty můžeš těmito kalužemi procházet, ale nemůžeš se pohnout z modré "
 "kaluže do červené nebo obráceně. Kaluže s předměty se berou jako bezbarvé "
 "a po sebrání předmětu změní barvu na tu, na které jste byl předtím. I Slizoví "
 "netvoři se mohou pohybovat jen v kalužích své vlastní barvy, ale když zemřou, "
 "vybuchnou, což zničí předměty v jejich blízkosti a změní barvu okolního slizu "
 "a Slizových netvorů.")

S(
 "These creatures are slow, but very powerful... more powerful than you. "
 "You need some more experience in demon fighting before you will be able to defeat them. "
 "Even then, you will be able to slay this one, but more powerful demons will come...\n\n"
 "Each 10 lesser demons you kill, you become powerful enough to kill all the greater "
 "demons on the screen, effectively turning them into lesser demons.",

 "Tito tvorové jsou pomalí, ale velmi silní... silnější než ty. Abys je mohl porazit, "
 "musíš mít větší zkušenosti v boji proti démonům. I pak budeš sice schopný zabít "
 "tohoto Ďábla, ale přijdou ještě silnější...\n\n"
 "Za každých 10 Čertů, které zabiješ, získáš dost síly na to, abys mohl zabít všechny "
 "Ďábly na obrazovce -- v podstatě se promění v Čerty.")

S("These creatures are slow, but they often appear in large numbers.",
  "Tito tvorové jsou pomalí, ale často se objevují ve velkém počtu.")

S("A big monster from the Living Caves. A dead Troll will be reunited "
 "with the rocks, causing some walls to grow around its body.",
 "Velký netvor z Živoucích jeskyní. Mrtvý Šutroll splyne se skálou a kolem "
 "jeho těla se rozrostou zdi.")

S("Huge, impassable walls which separate various lands.",
  "Obrovské, neprostupné zdi, které oddělují různé kraje.")

S(
 "This cave contains walls which are somehow living. After each turn, each cell "
 "counts the number of living wall and living floor cells around it, and if it is "
 "currently of a different type than the majority of cells around it, it switches. "
 "Items count as three floor cells, and dead Trolls count as five wall cells. "
 "Some foreign monsters also count as floor or wall cells.\n",
 "Zdi této jeskyně jsou, kdovíjak, živé. Po každém kole každé políčko spočítá "
 "počet políček s živoucími zdmi a políček s živoucími podlahami kolem sebe "
 "a pokud má jiný typ než většina jeho sousedů, změní se ze zdi na podlahu nebo "
 "opačně. Předměty se počítají jako tři políčka podlahy, mrtví Šutrollové jako "
 "pět políček zdí. Někteří cizí netvoři se také počítají jako podlaha nebo zeď.\n")

S(
 "This forest is quite dry. Beware the bushfires!\n"
 "Trees catch fire on the next turn. The temperature of the grass cells "
 "rises once per turn for each fire nearby, and becomes fire itself "
 "when its temperature has risen 10 times.\n"
 "You can also chop down the trees. Big trees take two turns to chop down.",
 "Tento hvozd je velice suchý. Pozor na požáry!\n"
 "Stromy začnou hořet jedno kolo poté, co se ocitnou v sousedství ohně. Teplota "
 "políček s trávou vzrůstá každé kolo o 1 za každé sousední políčko s ohněm "
 "a poté, co vzroste desetkrát, začne tráva hořet.\n"
 "Stromy v Suchém hvozdu můžeš kácet. Pokácení velkých stromů trvá dvě kola.")

S("A big and quite intelligent monster living in the Icy Land.",
  "Velký a dosti inteligentní netvor, který žije v Ledovém kraji.")

S(
 "A nasty predator from the Icy Land. Contrary to other monsters, "
 "it tracks its prey by their heat.",
 "Ošklivý predátor z Ledového kraje. Na rozdíl od jiných netvorů "
 "sleduje kořist pomocí tepla, které vydává.")

S("Rangers take care of the magic mirrors in the Land of Mirrors. "
 "They know that rogues like to break these mirrors... so "
 "they will attack you!",
 "Strážci se starají o kouzelná zrcadla v Kraji zrcadel. Vědí, že lupiči "
 "jako ty rádi tato zrcadla rozbíjejí... a proto na tebe útočí!")

// TODO update translation
S("A nasty creature that lives in caves. They don't like you for some reason.",
 "Hnusná potvora žijící v Živoucích jeskyních. Z nějakého důvodu "
 "tě nemají rády.")

S("A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them.",
  "Kmen domorodců z Pouště. Dokázali dokonce zkrotit i obrovské Písečné červy, kteří na ně neútočí.")

S("This giant ape thinks that you are an enemy.", "Tato obrovská opice si myslí, že jsi její nepřítel.")

S("A magical being which copies your movements.", "Magická bytost, které kopíruje tvé pohyby.")

S("A magical being which copies your movements. "
 "You feel that it would be much more useful in an Euclidean space.",
 "Magická bytost, která kopíruje tvé pohyby. Máš pocit, že v eukleidovském prostoru by byla mnohem užitečnější.")

S("You can summon these friendly constructs with a magical process.",
  "Tyto přátelské konstrukty můžeš vyvolat pomocí magického procesu.")

S("A majestic bird, who is able to fly very fast.",
  "Majestátní pták, který dokáže velmi rychle létat.")

S("A monster who is able to live inside the living cave wall.",
  "Netvor, který dokáže žít ve zdech Živoucí jeskyně.")

S("A typical Graveyard monster.", "Typický hřbitovní netvor.")

S("A typical monster from the Graveyard, who moves through walls.\n\n"
  "There are also wandering Ghosts. They will appear "
  "if you do not explore any new places for a long time (about 100 turns). "
  "They can appear anywhere in the game.",
  "Typický hřbitovní netvor, který dokáže procházet zdmi.\n\n"
   "Existují také bludní Duchové, kteří se objeví, pokud dlouho (přibližně 100 kol) neprozkoumáš žádné nové místo. Ti se mohou objevit kdekoli ve hře.")

S("Necromancers can raise ghosts and zombies from fresh graves.",
  "Nekromanceři dokážou probouzet duchy a zombie z čerstvých hrobů.")

S("A creepy monster who follows you everywhere in the Graveyard and the Cursed Canyon.",
  "Děsivý netvor, který tě sleduje po celém Hřbitově.") //TODO UPDATE

S("People worshipping Cthulhu. They are very dangerous.",
  "Lidé uctívající Cthulhua. Jsou velmi nebezpeční.")

S("People worshipping Cthulhu. This one is especially dangerous, "
 "as he is armed with a weapon which launches fire from afar.",
 "Lidé uctívající Cthulhua. Tenhle je zvlášť nebezpečný, protože "
 "je vybavený zbraní, která dokáže metat oheň na dálku.")

S("This dangerous predator has killed many people, and has been sent to Cocytus.",
  "Tento nebezpečný predátor zabil spoustu lidí, a tak byl poslán do Cocytu.")

S("This white dog is able to run all the time. It is the only creature "
 "able to survive and breed in the Land of Eternal Motion.",
 "Tento bílý pes dokáže neustále běžet. Je to jediný tvor, který dokáže přežít a rozmnožovat se v Kraji věčného pohybu.")

S("Demons of Hell do not drown when they fall into the lake in Cocytus. "
 "They turn into demonic sharks, enveloped in a cloud of steam.",
 "Když pekelní démoni spadnou do jezera v Cocytu, neutopí se. Místo "
 "toho se promění v Ďábelské žraloky zahalené v oblaku páry.")

S("These fairies would rather burn the forest, than let you get some Fern Flowers. "
 "The forest is infinite, after all...\n\n"
 "Fire Fairies transform into fires when they die.",
 "Tito mužíčkové by raději spálili celý hvozd, než aby tě nechali sebrat nějaké "
 "Kapradinové květy. Konečně, hvozd je přece nekonečný...\n\n"
 "Ohniví mužíčkové se promění v oheň, když zemřou.")

S("These warriors of the Forest wield exotic weapons called hedgehog blades. "
 "These blades protect them from a frontal attack, but they still can be 'stabbed' "
 "easily by moving from one place next to them to another.",
 "Tito válečníci z Hvozdu vládnou exotickými zbraněmi zvanými 'ježčí čepele'. "
 "Tyto čepele je chrání proti útoku zepředu, ale můžeš je snadno 'probodnout' tím, "
 "že se pohneš z jednoho políčka, které s nimi sousedí, na druhé.")

S("This being radiates an aura of wisdom. "
 "It is made of a beautiful crystal, you would love to take it home. "
 "But how is it going to defend itself? Better not to think of it, "
 "thinking causes your brain to go hot...\n\n"
 "Crystal Sages melt at -30 °C, and they can raise the temperature around you from afar.",
 "Tato bytost vyzařuje auru moudrosti. Je vytvořená z krásného krystalu -- "
 "moc rád%d0 by sis ji vza%l0 domů. Ale jak se bude bránit? Raději na to "
 "nemyslet, z myšlení se ti zahřívá mozek...\n\n"
 "Křišťáloví mudrci tají při teplotě -30 °C a dokážou kolem tebe na dálku "
 "zvyšovat teplotu.")

S("Cold white gems, found in the Icy Land.", "Chladné bílé drahokamy, které se nacházejí v Ledovém kraji.")

S("An expensive metal from the Living Caves. For some reason "
 "gold prevents the living walls from growing close to it.",
 "Valouny drahého kovu z Živoucí jeskyně. Zlato z nějakého důvodu "
 "zamezuje tomu, aby v jeho blízkosti rostly živoucí zdi.")

S("A rare and expensive substance found in the Desert. "
 "It is believed to extend life and raise special psychic powers.",
 "Drahá a vzácná látka, která se nachází v Poušti. Věří se, že prodlužuje život a zlepšuje zvláštní psychické schopnosti.")

S("A beautiful gem from the Jungle.", "Nádherný drahokam z Džungle.")

S(
 "A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
 "healthy after drinking it, but you still feel that one hit of a monster is enough to kill you.",
 "Úžasný nápoj, prý vytvořený smísením červeného a modrého slizu. Po jeho vypití se "
 "rozhodně cítíš zdravější, ale pořád máš pocit, že by tě dokázal zabít jediný úder netvora.")

S("A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
 "in the Land of Mirrors leave these.",
 "Kousek kouzelného zrcadla nebo oblaku přeludů, který je možné používat pro magické účely."
 "Zanechávají je po sobě pouze zrcadla a oblaky v Kraji zrcadel.")

S("These sinister totems contain valuable gems.",
  "Tyto zlověstné totemy obsahují cenné drahokamy.")

S("These star-shaped flowers native to Hell are a valuable alchemical component.",
  "Tyto hvězdám podobné květiny z Pekla jsou cennou alchymistickou surovinou.")

S("This statue is made of materials which cannot be found in your world.",
  "Tato soška je vytvořena z materiálů, které se ve tvém světě nenacházejí.")

S("One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable.",
  "Jedna z mála věcí, pod kterou se v Kraji věčného pohybu nepropadá země. Tato pírka jsou samozřejmě velice cenná.")

S("Cold blue gems, found in the Cocytus.", "Chladné modré drahokamy, které se nacházejí v Cocytu.")

S("These bright yellow gems can be found only by those who have mastered the Crossroads.",
  "Tyto jasně žluté drahokamy mohou najít pouze ti, kdo ovládli celou Křižovatku.")

S("That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
 "Each key unlocks only the Orb of Yendor which led you to it.",
 "To je všechno, co potřebuješ k odemčení Yendorské sféry! No... pokud se tedy dokážeš vrátit ke sféře, kterou tenhle klíč odemyká...\n"
 "Každý klíč odemyká pouze tu Yendorskou sféru, která tě k němu zavedla.")

S("These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
 "power is long gone. No way to use them, you could as well simply drop them...\n\n",
 "Tyto sféry můžeš najít na Hřbitově. Máš dojem, že to kdysi byly mocné magické sféry, "
 "ale jejich moc už je dávno pryč. Nedají se nijak použít, můžeš je rovnou zase položit...\n\n")

S(
 "This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
 "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
 "where the key is after you touch the Orb.",
 "Tuto úžasnou sféru mohou sebrat jenom ti, kdo se stali skutečnými pány tohoto "
 "hyperbolického vesníru, protože na její odemčení potřebuješ správný klíč. Tvé "
 "psychické schopnosti ti naštěstí prozradí, kde se klíč nachází, jakmile se této sféry dotkneš.")

S(
  "This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions.",
  "Tuto sféru můžeš použít k seslání kouzla Blesk, které způsobí, že z tebe do všech směrů vyletí blesky.")

S("This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2.",
  "Tuto sféru můžeš použít k seslání kouzla Záblesk, které zničí téměř vše do vzdálenosti 2 políček od tebe.")

S("This orb can be used to invoke a wall of ice. It also protects you from fires.",
  "Tuto sféru můžeš použít k vytvoření ledové zdi. Kromě toho tě chrání před ohněm.")

S("This orb can be used to move faster for some time.",
  "Tato sféra ti umožňuje pohybovat se po určitou dobu rychleji.")

S("This orb can be used to summon friendly golems. It is used instantly when you pick it up.",
  "Tuto sféru můžeš použít k vyvolání přátelských golemů. Použije se automaticky, jakmile ji sebereš.")

S("This orb can protect you from damage.", "Tato sféra tě dokáže chránit před zraněním.")

//S("This orb lets you go through the living wall. Only works in the Living Caves.",
//  "Tato sféra ti umožňuje procházet živoucími zdmi. Funguje pouze v Živoucí jeskyni.")

S("This orb lets you instantly move to another location on the map. Just click a location which "
 "is not next to you to teleport there. ",
   "Tato sféra ti umožňuje se okamžitě přemístit na jiné místo na mapě. Stačí, abys "
 "klikl na libovolné nesousední políčko.")

S("This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
 "that you will ever find the way back...\n\n"
 "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
 "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). ",
 "Tato sféra ti umožňuje se okamžitě přemístit na bezpečné, vzdálené místo. Vzhledem "
 "k povaze tohoto podivného světa pochybuješ, že ještě někdy najdeš cestu zpátky...\n\n"
 "Pokud před vypršením efektu Sféry bezpečnosti ukončíš hru, uloží se.\n\n"
 "Technická poznámka: vzhledem k tomu, že návrat do jakéhokoli již navštíveného místa je stejně prakticky nemožný, použití Sféry bezpečnosti uvolní veškerou paměť, kterou herní svět až dosud využíval, a to i tehdy, pokud ji nepoužiješ k uložení hry.")

S("This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n",
  "Tato sféra umožňuje útočit na Ježčí válečníky přímo a probodávat i jiné netvory.\n")

S("This flower brings fortune to the person who finds it.\n",
  "Tato rostlina přináší štěstí tomu, kdo ji najde.\n")

S("Ice Walls melt after some time has passed.", "Ledové zdi časem tají.")

S("A natural terrain feature of the Desert.", "Přírodní terénní prvek v Poušti.")

S("You can go inside the Magic Mirror, and produce some mirror images to help you.",
  "Do Kouzelného zrcadla můžeš vstoupit a vytvořit několik zrcadlových obrazů, které ti pomohou.")

S(
 "Tiny droplets of magical water. You see images of yourself inside them. "
 "Go inside the cloud, to make these images help you.",
 "Maličké kapičky magické vody. Vidíš v nich své obrazy. Když vejdeš do "
 "oblaku, tyto obrazy ti pomohou.")

S("A device that attracts sandworms and other enemies. You need to activate it.",
  "Zařízení, které přitahuje písečné červy a jiné nepřátele. Aby začalo pracovat, musíš ho aktivovat.")

S("A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it.",
  "Hromada dřeva, která se dá použít k zapálení ohně. Všechno už tu je, na podpálení se jí stačí jen dotknout.")

S("An ancient grave.", "Starý hrob.")

S("A fresh grave. Necromancers like those.", "Čerstvý hrob. Ty mají rádi nekromanceři.")

S("A piece of architecture typical to R'Lyeh.", "Typický architektonický prvek R'Lyeh.")

S("An impassable lake in Cocytus.", "Neprostupné jezero v Cocytu.")

S("You can walk on it... but beware.", "Můžeš po něm chodit... ale pozor.")

S("It was a floor... until something walked on it.", "Tady byla podlaha... než po ní něco přešlo.")

S(
 "This land is a quick gateway to other lands. It is very easy to find other lands "
 "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
 "As long as you have found enough treasure in their native lands, you can "
 "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
 "and other land types bring magical orbs.\n\n"
 "A special treasure, Hyperstone, can be found on the Crossroads, but only "
 "after you have found 10 of every other treasure.",
 "Tento kraj je rychlou branou do jiných krajů. Na Křižovatce je velmi snadné najít jiné "
 "kraje. Což znamená, že tu najdeš i netvory z většiny jiných krajů!\n\n"
 "Na Křižovatce můžeš najít magické předměty, pokud jsi v jejich krajích nalez%l0 dost "
 "pokladů. Můžeš tu najít zrcadla a oblaky z Kraje zrcadel a magické sféry z jiných typů "
 "krajů.\n\n"
 "Na Křižovatce můžeš najít zvláštní poklad -- Hyperkam -- ale pouze poté, co najdeš "
 "10 kusů od všech ostatních pokladů.")

S("A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms.",
  "Horký kraj plný písečných dun, tajemného Koření a obrovských a nebezpečných Písečných červů.")

S(
 "A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
 "melt, even if you don't want it.",
 "Velmi chladný kraj plný ledových zdí. Tyto ledové zdi tají už tvou pouhou přítomností, ať už si to přeješ nebo ne.")

S("A land filled with huge ivy plants and dangerous animals.",
  "Kraj plný obrovských rostlin břečťanu a nebezpečných zvířat.")

S("A strange land which contains mirrors and mirages, protected by Mirror Rangers.",
  "Podivný kraj se zrcadly a přeludy, který chrání Zrcadloví strážci.")

S("All the monsters you kill are carried to this strange land, and buried. "
 "Careless Rogues are also carried here...",
 "Všichni netvoři, které zabiješ, jsou odneseni do tohoto podivného kraje, "
 "a tam pohřbeni. A také neopatrní lupiči...")

S("An ancient sunken city which can be reached only when the stars are right.\n\n"
  "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "Prastaré potopené město, kam se lze dostat pouze při správném postavení hvězd.\n\n"
  "Po znalezieniu 5 Statuetek Cthulhu możesz w R'Lyeh trafić na Świątynie Cthulhu.")

S("A land filled with demons and molten sulphur. Abandon all hope ye who enter here!",
  "Kraj plný démonů a roztavené síry. Zanech vší naděje, ty, kdož vstupuješ!")

S("This frozen lake is a hellish version of the Icy Land. Now, your body heat melts the floor, not the walls.",
  "Toto zamrzlé jezero je pekelnou verzí Ledového kraje. Tady tvým tělesným teplem netají zdi, ale podlaha.")

S("A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
 "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
 "the floor.\n",
 "Kraj, ve kterém se nemůžeš zastavit, protože každý kousek podlahy je nesmírně nestabilní. "
 "Mohou tu přežít pouze netvoři, kteří dokážou věčně běžet, a pouze fénixova pírka jsou tak lehká, že nenaruší podlahu.")

S("Affects looks and grammar", "Ovlivňuje vzhled a gramatiku")

S("first joystick: movement threshold", "joystick 1: práh pohybu")
S("first joystick: execute movement threshold", "joystick 1: práh provedení pohybu")
S("second joystick: pan threshold", "joystick 2: práh posuvu")
S("second joystick: panning speed", "joystick 2: rychlost posouvání")
S("%The1 is frozen!", "%1 zamrz%l1!")
S("%The1 burns!", "%1 shoře%l1!")
S("message flash time", "doba blikání zprávy")

S("skin color", "barva pleti")
S("weapon color", "barva zbraně")
S("hair color", "barva vlasů")
S("dress color", "barva šatů")
S("Shift=random, Ctrl=mix", "Shift=náhodná, Ctrl=směs")

S("Euclidean mode", "Eukleidovský mód")
S("Return to the hyperbolic world", "Návrat do hyperbolického světa")
S("Choose from the lands visited this game.", "Vyber si z krajů, které jsi v této hře navštívil.")
S("Scores and achievements are not", "V Eukleidovském módu se")
S("saved in the Euclidean mode!", "neukládají skóre a achievementy!")

S("MOVE", "POHYB")
S("BACK", "ZPĚT")
S("DRAG", "TAŽENÍ")
S("INFO", "INFORMACE")
S("MENU", "MENU")
S("QUEST", "ÚKOLY")
S("HELP", "NÁPOVĚDA")
S("NEW", "NOVÁ HRA")
S("PLAY", "HRAJ")
S("SHARE", "SDÍLEJ")
S("HyperRogue for Android", "HyperRogue pro Android")
S("Date: %1 time: %2 s ", "Datum: %1 čas: %2 s ")
S("distance: %1\n", "vzdálenost: %1\n")
S("Cheats: ", "Cheaty: ")
S("Score: ", "Skóre: ")
S("Kills: ", "Zabitých netvorů: ")
S("Retrieving scores from Google Leaderboards...", "Stahuji skóre z žebříčků Google...")
S("Scores retrieved.", "Skóre stažena.")

S("Your total treasure has been recorded in the Google Leaderboards.", "Tvůj celkový poklad byl zaznamenán na žebříčky na Googleu.")
S("You have improved your total high score on Google. Congratulations!", "Zlepši%l0 jsi své celkové nejvyšší skóre na Googleu. Gratulujeme!")
S("You have improved your '%1' high score on Google!", "Zlepši%l0 jsi své nejvyšší skóre na Googleu v kategorii '%P1'!")
S("This has been recorded in the Google Leaderboards.", "Bylo zaznamenáno na žebříčky Googleu.")

S("Ever wondered how some boardgame would look on the hyperbolic plane? "
 "I wondered about Go, so I have created this feature. Now you can try yourself!\n"
 "Enter = pick up an item (and score), space = clear an item\n"
 "Other keys place orbs and terrain features of various kinds\n"
 "In the periodic editor, press 0-4 to switch walls in different ways\n",
 "Přemýšle%l0 jsi někdy o tom, jak by vypadala desková hra na hyperbolické rovině? "
 "Mě zajímalo Go, a tak jsem vytvořil tuhle funkci. Teď si to můžeš vyzkoušet i ty!\n"
 "Enter = seber předmět (a skóre), mezerník = vymaž předmět\n"
 "Další klávesy umísťují sféry a nejrůznější terénní prvky\n"
 "Stiskem kláves 0-4 můžete různě přepínat zdi\n")

S("Periodic Editor", "Periodický editor")
// also translate this line:
// "Stiskem kláves 0-4 můžete různě přepínat zdi\n",

S("Collect %1 $$$ to access even more lands", "Sesbíráním %1 $$$ získáš přístup do dalších krajů")

// Emerald Mine
// ------------

N("Emerald Mine", GEN_O, "Smaragdový důl", "Smaragdové doly", "Smaragdový důl", "ve Smaragdovém dole")
N("Pikeman", GEN_M, "Kopiník", "Kopiníci", "Kopiníka", "Kopiníkem")
N("Flail Guard", GEN_M, "Strážce s řemdihem", "Strážci s řemdihy", "Strážce s řemdihem", "Strážcem s řemdihem")
N("Miner", GEN_M, "Horník", "Horníci", "Horníka", "Horníkem")
N("Emerald", GEN_O, "Smaragd", "Smaragdy", "Smaragd", "Smaragdem")

Orb("the Mind", "Mysli")

S(
     "Evil people are mining for emeralds in this living cave. "
     "It does not grow naturally, but it is dug out in a regular "
     "pattern, which is optimal according to the evil engineers.",
     "V této živoucí jeskyni těží zlí lidé smaragdy. "
     "Neroste přirozeně, ale je vykopána v pravidelném vzoru, "
     "který zlí inženýři pokládají za optimální.")

S(
    "Miners have special tools for dealing with the Living Cave. "
    "When they die, these tools activate, destroying the living cave "
    "around them.",
    "Horníci mají speciální nástroje pro práci v Živoucí jeskyni. "
    "Když zemřou, jejich nástroje se aktivují a zničí živoucí "
    "jeskyni kolem nich.")

S(
    "When Pikemen move, they attack all cells which are now adjacent to them. "
    "Luckily, they can be killed in the same way.\n\n"
    "They never move if this would attack their friends.",
    "Kopiníci po každém svém pohybu zaútočí na všechna okolní políčka "
    "Naštěstí se dají zabít stejným způsobem.\n\n"
    "Kopiníci se nepohnou, pokud by to znamenalo, že zaútočí na své přátele.")

S(
    "This guard of the Emerald Mine is wielding a huge flail. "
    "You cannot attack him directly, as the flail would still hit you then. "
    "Luckily, you have learned a trick: if you step away from him, "
    "he will hit himself with the flail!",

    "Tento strážce Smaragdového dolu je vyzbrojen obrovským řemdihem. "
    "Nemůžeš na něj útočit přímo, protože by tě řemdih i tak zasáhl. "
    "Naštěstí ses naučil jeden trik: pokud se pohneš směrem od něj, "
    "zasáhne řemdihem sám sebe!")

S("A precious green gem from the Emerald Mines.", "Cenný zelený drahokam ze Smaragdového dolu.")

S("%The1 pierces %the2!", "%1 probod%l1 %a2!")

S("Make him hit himself by walking away from him.",
  "Když se pohneš směrem od něj, zasáhne sám sebe.")

S("This orb allows you to instantly kill a non-adjacent enemy by clicking it. "
    "Each use drains 30 charges.",
  "Tato sféra ti umožňuje zabít kliknutím libovolného nepřítele, který není na "
  "sousedním políčku. Každé použití sníží náboj sféry o 30.")

S("%The1 is immune to mental blasts!", "%1 je imunní proti mentálnímu útoku!")
S("You kill %the1 with a mental blast!", "Zabi%l1 jsi %a2 mentálním útokem!")

// Vineyard
// --------

N("Vineyard", GEN_F, "Vinice", "Vinice", "Vinici", "na Vinici")
N("Vine Beast", GEN_M, "Révový netvor", "Révoví netvoři", "Révového netvora", "Révovým netvorem")
N("Vine Spirit", GEN_M, "Révový duch", "Révoví duchové", "Révového ducha", "Révovým duchem")
N("vine", GEN_F, "réva", "réva", "révu", "révou")
N("partial fire", GEN_M, "částečný oheň", "částečné ohně", "částečný oheň", "částečným ohněm")
N("Wine", GEN_N, "Láhev vína", "Lahve vína", "Láhev vína", "Lahví vína")

S(  "This cell is partially on fire.", "Toto políčko částečně hoří.")

S(
  "The Vineyard is filled with vines. A very dense pattern of straight lines here...\n\n"
  "Vine Beasts and Vine Spirits change vine cells to grass, and vice versa.",
  "Vinice je plná révy. Je tu velmi hustý vzor rovných čar...\n\n"
  "Révoví netvoři a Révoví duchové mění políčka s révou na trávu a obráceně.")

S("A vine is growing here... but only on a half of the cell. How is that even possible?!"
  "Most monsters cannot move from this cell to the cell containing the other half. "
  "Vine spirits can move only to the adjacent cells which are also adjacent to the "
  "other half.",
  "Tady roste réva... ale pouze na půlce políčka. Jak je to vůbec možné?!"
  "Většina netvorů nemůže přecházet z tohoto políčka na políčko s druhou polovinou révy. "
  "Révoví duchové se mohou pohybovat pouze na ta sousední políčka, která sousedí "
  "i s druhou polovinou.")

S(
    "A beast made of vines!\n\n"
    "Vine Beasts turn into vines when they die.",
    "Netvor vytvořený z révy!\n\n"
    "Révoví netvoři se po smrti promění v révu.")

S(  "A spirit living in the vines!\n\n"
    "Vine Spirits destroy the vines when they die.",
    "Duch žijící v révě!\n\n"
    "Révoví duchové ničí révu, v níž se nacházejí, když zemřou.")

S("Wine grown under hyperbolic sun would be extremely prized in your home location.",
  "Víno vypěstované pod hyperbolickým sluncem by mělo ve tvé domovině velkou cenu.")

S("This orb allows one to pass through all kinds of walls and chasms.",
  "Tato sféra ti umožňuje procházet skrz libovolné zdi a přes propasti.")

S("You cannot attack through the Vine!",
  "Nemůžeš útočit skrz révu!")

Orb("Aether", "Éteru")

// Dead Caves
// ----------

N("Dead Cave", GEN_F, "Mrtvá jeskyně", "Mrtvé jeskyně", "Mrtvou jeskyni", "v Mrtvé jeskyni")
N("Dark Troll", GEN_M, "Temný troll", "Temní trollové", "Temného trolla", "Temným trollem")
N("Earth Elemental", GEN_M, "Elementál Země", "Elementálové Země", "Elementála Země", "Elementálem Země")
N("dead wall", GEN_F, "mrtvá zeď", "mrtvé zdi", "mrtvou zeď", "mrtvou zdí")
N("dead floor", GEN_F, "mrtvá podlaha", "mrtvé podlahy", "mrtvou podlahu", "mrtvou podlahou")
N("rubble", GEN_F, "sutiny", "sutiny", "sutiny", "sutinami")
N("Silver", GEN_N, "Kus stříbra", "Kusy stříbra", "Kus stříbra", "Kusem stříbra")

S("A precious metal from the Dead Caves.", "Vzácný kov z Mrtvé jeskyně.")

S(  "A Troll without the power of Life.", "Troll bez síly Života.")

S(
    "A rare unliving construct from the Dead Caves. "
    "It instantly destroys cave walls next to its path, and also leaves "
    "an impassable wall behind it. You suppose that this impassable wall helps it to "
    "escape from some threats. You hope you won't meet these threats...",

    "Vzácný neživý konstrukt z Mrtvých jeskyní. "
    "Okamžitě ničí stěny jeskyně kolem své dráhy a zanechává "
    "za sebou neprostupnou zeď. Tato neprostupná zeď mu zřejmě umožňuje uniknout "
    "před nebezpečím. Doufáš, že na tato nebezpečí nenarazíš...")

S("%The1 punches %the2!", "%1 udeři%l1 %a2!")
S("%The1 is destroyed by the forces of Life!", "Síla Života zničila %a1!")
S(  "Somehow, this cave has not received the spark of Life yet.",
    "Tato jeskyně z nějakého důvodu dosud nedostala jiskru Života.")

// Hive
// ----

N("Hive", GEN_O, "Úl", "Úly", "Úl", "v Úlu")
N("Red Hyperbug", GEN_M, "Červený hyperhmyzák", "Červení hyperhmyzáci", "Červeného hyperhmyzáka", "Červeným hyperhmyzákem")
N("Green Hyperbug", GEN_M, "Zelený hyperhmyzák", "Zelení hyperhmyzáci", "Zeleného hyperhmyzáka", "Zeleným hyperhmyzákem")
N("Blue Hyperbug", GEN_M, "Modrý hyperhmyzák", "Modří hyperhmyzáci", "Modrého hyperhmyzáka", "Modrým hyperhmyzákem")
N("Royal Jelly", GEN_F, "Mateří kašička", "Mateří kašičky", "Mateří kašičku", "Mateří kašičkou")
N("weird rock", GEN_F, "podivná skála", "podivné skály", "podivnou skálu", "podivnou skálou")

Orb("Invisibility", "Neviditelnosti")

S("A weirdly colored rock. Hyperentomologists claim that the "
  "Hyperbug armies use these rocks to navigate back home after a victorious battle.",
  "Podivně zbarvená skála. Hyperentomologové tvrdí, že armády Hyperhmyzáků "
  "je používají k navigaci, když se vracejí domů po vítězné bitvě.")

S("%The1 fights with %the2!", "%1 se pustil do boje s %a2!")

S("The Hive is filled with Hyperbugs. They are huge insects which look a bit like "
  "ants, a bit like bees, and a bit like roaches. "
  "They live in circular nests, and an army of Hyperbugs will attack any intruder, "
  "including you and enemy Hyperbugs. Will you manage to get to the "
  "heart of such a nest, and get the precious Royal Jelly?",
  "Úl je plný Hyperhmyzáků. Tento obří hmyz vypadá trochu jako mravenci, "
  "trochu jako včely a trochu jako švábi. Žijí v kruhových úlech a jejich armáda "
  "zaútočí na každého vetřelce včetně tebe a nepřátelských Hyperhmyzáků. "
  "Dokážeš se dostat do srdce takového kruhového úlu a získat vzácnou Mateří kašičku?")

S("This is what Hyperbug Queens eat. Very tasty and healthy.",
  "Tohle je potrava, kterou jedí Královny hyperhmyzáků. Velmi chutná a zdravá.")

S("When you have this Orb, most monsters won't see you, unless "
  "you are standing still, attacking, or picking up items.",
  "Dokud máš tuto Sféru, většina netvorů tě neuvidí, pokud nebudeš "
  "stát na místě, nezaútočíš nebo nesebereš předmět.")

Orb("Earth", "Země")

S("This orb lets you go through living walls. It also has powers in some of the other lands.",
  "Tato sféra ti umožňuje procházet živoucími zdmi. Má také určité schopnosti v některých jiných krajích.")

// Land of Power
// -------------

N("Land of Power", GEN_O, "Kraj Moci", "Kraje Moci", "Kraj Moci", "v Kraji Moci")
N("Witch Apprentice", GEN_F, "Učednice", "Učednice", "Učednici", "Učednicí")
N("Speed Witch", GEN_F, "Čarodějnice Rychlosti", "Čarodějnice Rychlosti", "Čarodějnici Rychlosti", "Čarodějnicí Rychlosti")
N("Flash Witch", GEN_F, "Čarodějnice Záblesku", "Čarodějnice Záblesku", "Čarodějnicí Záblesku", "Čarodějnicí Záblesku")
N("Fire Witch", GEN_F, "Čarodějnice Ohně", "Čarodějnice Ohně", "Čarodějnici Ohně", "Čarodějnicí Ohně")
N("Aether Witch", GEN_F, "Čarodějnice Éteru", "Čarodějnice Éteru", "Čarodějnici Éteru", "Čarodějnicí Éteru")
N("Winter Witch", GEN_F, "Čarodějnice Zimy", "Čarodějnice Zimy", "Čarodějnici Zimy", "Čarodějnicí Zimy")
N("Evil Golem", GEN_M, "Zlý golem", "Zlí golemové", "Zlého golema", "Zlým golemem")
N("Powerstone", GEN_O, "Mocikam", "Mocikamy", "Mocikam", "Mocikamem")
N("crystal cabinet", GEN_F, "křišťálová vitrína", "křišťálové vitríny", "křišťálovou vitrínu", "křišťálovou vitrínou")

S("When you have this Orb, you will leave a trail of fire behind you.",
  "Když máš tuto Sféru, zanecháváš za sebou ohnivou stopu.")

S(
    "A Witch without any special powers. But watch out! She will "
    "pick up any basic Orbs on her path, and use their powers.",
    "Čarodějnice bez jakýchkoli zvláštních schopností. Ale pozor! "
    "Učednice sbírají jakékoli základní Sféry, na které narazí "
    "a používají jejich schopnosti.")

S(  "A Witch with a Speed spell. She moves twice as fast as you. Unless you "
    "have an Orb of Speed too, of course!",
    "Čarodějnice s kouzlem Rychlosti. Pohybuje se dvakrát rychleji než ty. "
    "Samozřejmě, pokud náhodou také nemáš Sféru Rychlosti!")

/* S(  "A Witch with a Flash spell. Very dangerous!\n\nLuckily, she never uses the spell if it "
    "would kill her friends.",
    "Čarodějnice s kouzlem Záblesk. Velmi nebezpečná, ale nepoužije své kouzlo, pokud by to zasáhlo její přátele.") */

S(  "A Witch with a Fire spell. She will leave a trail of fire behind her.",
    "Čarodějnice s kouzlem Ohně. Zanechává za sebou ohnivou stopu.")

S(  "A Witch with a Winter spell. She is able to move through fire.",
    "Čarodějnice s kouzlem Zimy. Dokáže se pohybovat skrz oheň.")

S(  "A Witch with an Aether spell. She is able to move through fire and walls.",
    "Čarodějnice s kouzlem Éteru. Dokáže procházet ohněm a zdmi.")

S(  "Somebody has summoned these evil constructs with a magical process.",
    "Tyto zlé konstrukty kdosi vyvolal pomocí magického procesu.")

S(  "The Land of Power is filled with everburning fire, magical Orbs, and guarded by "
    "witches and golems. There are basic orbs lying everywhere, and more prized ones "
    "are kept in crystal cabinets.\n\n"
    "Witches are allowed to use all the powers of the "
    "basic orbs against intruders. These powers never expire, but a Witch "
    "can use only one power at a time (not counting Orbs of Life).\n\n"
    "Witches and Golems don't pursue you into other Lands. Also, most Orb powers "
    "are drained when you leave the Land of Power.",
    "Kraj Moci je plný věčně planoucích ohňů a magických Sfér a střeží ho "
    "čarodějnice a golemové. Základní Sféry se válejí všude kolem a ty cennější "
    "jsou schované v křišťálových vitrínách.\n\n"
    "Čarodějnice mohou v boji proti vetřelcům používat všechny základní sféry. "
    "Jejich síla se nikdy nevyčerpá, ale každá Čarodějnice může mít vždy pouze "
    "jednu schopnost (Sféry Života se nepočítají).\n\n"
    "Čarodějnice a golemové tě nikdy nebudou pronásledovat do jiných Krajů. "
    "Kromě toho většina sfér při opuštění Kraje Moci svou sílu ztratí.")

S(
    "Witches use these crystal cabinets to protect Powerstones, as well as the more "
    "expensive Orbs. They are partially protected from thieves: they are too strong "
    "to be smashed by conventional attacks, and if you try to steal the item "
    "using an Orb of Aether, your Aether power will be completely drained.",
  "Čarodějnice používají tyto vitríny k ochraně Mocikamů, a také dražších  "
  "typů Sfér. Vitríny jsou částečně chráněné proti zlodějům: jsou příliš odolné na to, "
  "aby se daly rozbít běžným útokem, a pokud se pokusíš předmět ukrást pomocí Sféry Éteru, "
  "zcela to vyčerpá její sílu.")

S(
    "A Stone from the Land of Power. You are not sure what it is exactly, but "
    "as the Powerstones are kept in crystal cabinets, they are surely valuable.",
    "Kámen z Kraje Moci. Nejsi si jistý, co je přesně zač, ale když dávají "
    "Mocikamy do křišťálových vitrín, musejí mít jistě velkou cenu.")

Orb("Fire", "Ohně")

// achievements
// ------------

// "Head Witch", "Have 10 Orbs active at the same time."
// "Hlavní Čarodějnice", "Měj aktivních 10 Sfér najednou."

// "Enemy Number One", "Have an army of 1000 hyperbugs pursue you."
// "Nepřítel číslo jedna", "Měj v patách armádu 1000 hyperhmyzáků."

// "Lifebringer", "Brought Life into the Dead Caves."
// "Životabudič", "Přinesl jsi Život do Mrtvých jeskyní."

// "Wall Gardener", "Make a living wall grow outside of a cavernous region."
// "Zedník a zahradník", "Vypěstuj živoucí zeď mimo jeskynní oblast."

// "Master Swordsman", "Defeat a Hedgehog Warrior, a Pikeman, and a Flail Guard at the same time in melee."
// "Mistr meče", "Zabij Ježčího válečníka, Kopiníka a Strážce s řemdihem bez použití speciálních útoků."

// "Master of Melee", "Defeat five monsters in melee at the same time."
// "Mistr boje nablízko", "Zabij pět netvorů naráz bez použití speciálních útoků."

// for collection achievements:
// provide the required cases for descriptions:
// Emerald, Royal Jelly, Silver, Powerstone, and Wine
// achievement names:
// Master of the Hive, Master of Power, Master of Vineyard, Master of Dead Caves, Master of the Mine
// "Hyperbug", "Hyperbug Warrior", "Hyperbug Queen",

// Pán Úlu, Pán Moci, Pán Vinice, Pán Mrtvých jeskyní, Pán dolu
// "Hyperhmyzák", "Hyperhmyzácký válečník", "Královna hyperhmyzáků",

// for the store description:

// (currently, five more lands are included!)
// (nyní s pěti dalšími Kraji!)

S("Your Aether power has expired! RUN!", "Tvá síla Éteru vyprchala. UTÍKEJ!")

S("%The1 tricks %the2.", "%1 přelsti%l1 %a2.") // this is for attacks against Pikemen and Flail Guards

S("You trick %the1.", "Přelsti%l0 jsi %a1.")

S(  "A Witch with a Flash spell. Very dangerous!\n\nLuckily, she never uses the spell if it "

    "would kill her friends. She could destroy an Evil Golem, though.",

    "Čarodějnice s kouzlem Záblesk. Velmi nebezpečná!\n\nNaštěstí toto kouzlo nepoužije, "

    " pokud by to zabilo její přátele. Zlé golemy ale ničit může.")

S("Your Aether powers are drained by %the1!", "%1 vysála tvou sílu Éteru!")

// currently only crystal cabinets, so only female form

S("As you leave, your powers are drained!", "Opouštíš Kraj Moci... Tvá síla mizí!")

S("%The1 picks up %the2!", "%1 sebrala %a2!") // currently only Witches, so only female form

S("You can right click any element to get more information about it.\n\n",
  "Informace o jakémkoli herním prvku můžeš zobrazit tak, že na něj klikneš pravým tlačítkem myši.\n\n"
)

S("TRANSLATIONWARNING", "")
S("TRANSLATIONWARNING2", "")
// S("TRANSLATIONWARNING", "Warning: new features from 7.2 are not yet translated to Czech")

S("The Hyperbugs are fighting!", "Hyperhmyzáci bojují mezi sebou!")

S("","") N("",GEN_N,"","","","")

S("Cheat-changed the display.", "Cheat-changed the display.")
S("Dead floor, with some rubble.", "Mrtvá podlaha se sutinami.")

// Camelot
// -------

N("Camelot", GEN_O, "Camelot", "Cameloty", "Camelotu", "v Camelotu")
N("wall of Camelot", GEN_F, "hradba Camelotu", "hradby Camelotu", "hradbu Camelotu", "hradbou Camelotu")
N("moat of Camelot", GEN_O, "vodní příkop", "vodní příkopy", "vodní příkop", "vodním příkopem")
N("Round Table", GEN_O, "Kulatý stůl", "Kulaté stoly", "Kulatý stůl", "Kulatým stolem")
N("Knight", GEN_M, "Rytíř", "Rytíři", "Rytíře", "Rytířem")
N("Holy Grail", GEN_O, "Svatý grál", "Svaté grály", "Svatý grál", "Svatým grálem")

S(
  "The Knights of the Round Table are the greatest warriors of these lands. "
  "They are not very inventive with names though, as they call each of their "
  "castles Camelot. "
  "You are probably worthy of joining them, but they will surely give you "
  "some quest to prove yourself...\n\n"
  "Each castle contains a single treasure, the Holy Grail, in the center. "
  "The radius of the Round Table is usually 28, but after you find a Holy Grail "
  "successfully, each new castle (and each Round Table) you find will be bigger.",

  "Rytíři Kulatého stolu jsou největší bojovníci těchto krajů. Ale nemají zrovna "
  "talent na vymýšlení jmen, protože všem svým hradům říkají Camelot. "
  "Pravděpodobně jsi hoden toho, aby ses k nim připojil, ale "
  "určitě budeš muset splnit nějaký úkol, abys jim to dokázal...\n\n"
  "Na každém hradě je jeden poklad (Svatý grál), který se nachází v jeho středu. "
  "Poloměr Kulatého stolu je obvykle 28, ale poté, co najdeš Svatý grál, bude "
  "každý další hrad (a každý další Kulatý stůl) větší a větší.")

S("It would be impolite to land on the table!",
  "Přístát na stole by bylo nezdvořilé!")
S("You jump over the table!", "Přeskočil jsi stůl!")
S("That was not a challenge. Find a larger castle!",
  "To nebylo nic těžkého. Najdi si větší hrad!")
S("The Knights congratulate you on your success!",
  "Rytíři ti gratulují ke tvému úspěchu!")
S("The Knights laugh at your failure!",
  "Rytíři se smějí tvému neúspěchu!")
S("The Knights stare at you!", "Rytíři na tebe zírají!")
S("Come on, this is too easy... find a bigger castle!",
  "Ale no tak, tohle je příliš jednoduché... najdi si větší hrad!")
S("The Knights wish you luck!", "Rytíři ti přejí hodně štěstí!")

S("\"I would like to congratulate you again!\"", "\"Rád bych ti znovu pogratuloval!\"")
S("\"Find the Holy Grail to become one of us!\"", "\"Najdi svatý grál a staneš se jedním z nás!\"")
S("\"The Holy Grail is in the center of the Round Table.\"", "\"Svatý grál je ve středu Kulatého stolu.\"")
S("\"I enjoy watching the hyperbug battles.\"", "\"Rád se dívám na bitvy Hyperhmyzáků.\"")
S("\"Have you visited a temple in R'Lyeh?\"", "\"Navštívi%l0 jsi už chrám v R'Lyeh?\"")
S("\"Nice castle, eh?\"", "\"Fajn hrad, co?\"")

S("Congratulations! You have found the Holy Grail!",
  "Gratulujeme! Naš%el0 jsi Svatý grál!")

// Temple
// ------

N("Temple of Cthulhu", GEN_O, "Cthulhuův chrám", "Cthulhuovy chrámy", "Cthulhuův chrám", "v Cthulhuově chrámu")
N("big statue of Cthulhu", GEN_F, "Cthulhuova socha", "Cthulhuovy sochy", "Cthulhuovu sochu", "Cthulhuovou sochou")
N("Grimoire", GEN_O, "Grimoár", "Grimoáry", "Grimoár", "Grimoárem")
N("Cult Leader", GEN_M, "Vůdce kultu", "Vůdcové kultu", "Vůdce kultu", "Vůdcem kultu")

S("The temple of Cthulhu consists of many concentric circles of columns. "
  "You will surely encounter many Cultists there, who believe that a pilgrimage "
  "to the inner circles will bring them closer to Cthulhu himself, and Grimoires "
  "which surely contain many interesting secrets.\n\n"
  "The circles in the temple of Cthulhu are actually horocycles. They are "
  "infinite, and there is an infinite number of them.",

  "Cthulhuův chrám sestává z mnoha soustředných kruhů sloupů. "
  "Jistě tu narazíš na mnoho Kultistů, kteří věří, že je vykonání pouti do vnitřních kruhů "
  "přivede blíž k samotnému Cthulhuovi a ke Grimoárům, které jistě obsahují "
  "mnoho zajímavých tajemství.\n\n"
  "Kruhy v Cthulhuově chtámu jsou ve skutečnosti horocykly. Jsou nekonečně velké "
  "a je jich nekonečně mnoho."
  )

S("The Grimoires contain many secrets of the Great Old Ones. "
  "Each new inner circle in the Temple of Cthulhu contains new Grimoires, with new secrets. "
  "You hope to read them when you return home, and to learn many things. "
  "The knowledge is valuable to you, but it is rather pointless to try to get "
  "several copies of the same Grimoire...",
  "Grimoáry obsahují mnoho tajemství Starých Velkých. "
  "Každý další vnitřní kruh Cthulhuova chrámu obsahuje nové Grimoáry s novými tajemstvími. "
  "Doufáš, že si je všechny přečteš, až se vrátíš domů, a že se naučíš mnoho věcí. "
  "Znalosti z Grimoárů jsou pro tebe cenné, ale nemá smysl snažit se získat několik kopií "
  "stejného Grimoáru..."
  )

S("These statues of Cthulhu are too large to carry, and they don't look too "
  "valuable anyway. Most monsters will never go through them... they probably have "
  "their reasons. But you can go! When you move into the cell containing "
  "a statue, you push the statue to the cell you left.\n",

  "Tyto Cthulhuovy sochy jsou příliš velké na to, abys je unesl, a navíc ani nevypadají "
  "příliš cenné. Většina netvorů skrz ně nikdy nepůjde... asi mají své důvody. Ale ty "
  "přes ně chodit můžeš! Když vstoupíš na políčko se sochou, odstrčíš ji na to políčko, "
  "které jsi opusti%l0."
  )

S("These Cultists can push the statues, just like you.",
  "Tito kultisté dokážou strkat sochy, stejně jako ty.")

S("You already have this Grimoire! Seek new tomes in the inner circles.",
  "Tenhle Grimoár už máš! Hledej nové Grimoáry ve vnitřních kruzích.")

S("You push %the1 behind you!", "Odstrčil jsi %a1 za sebe.")

// More texts
// ----------

S("Your total kills", "Celkový počet zabitých netvorů")

S(
  "In most lands, more treasures are generated with each enemy native to this land you kill. "
  "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
  "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
  "but are not counted in the total kill count.",

  "Ve většině krajů se objevuje tím víc pokladů, čím víc zabijete nepřátel, kteří v nich žijí. "
  "Kromě toho musíš zabít 100 netvorů, aby ses mohl dostat na Hřbitov a do Úlu.\n\n"
  "Přátelští tvorové a části netvorů (jako je Břečťan) se v seznamu objevují, ale "
  "do celkového počtu zabitých netvorů se nepočítají.")

S("There must be some temples of Cthulhu in R'Lyeh...",
  "Cthulhu musí mít V R'Lyeh nějaké Chrámy...")

S("You overheard Hedgehog Warriors talking about emeralds...",
  "Zaslechl jsi Ježčí válečníky, jak mluví o smaragdech...")

S("You overhear miners talking about a castle...",
  "Zaslechl jsi Horníky, jak mluví o nějakém hradu...")

S("A castle in the Crossroads...", "O hradu na Křižovatce...")

//S("You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
//  "Po sebrání pěti Cthulhuových sošek můžeš v R'Lyeh najít Cthulhuovy chrámy.")

S("You have to escape first!", "Nejprve musíš utéct!")
S("There is not enough space!", "Není tady dost místa!")

S("Customize character", "upravit postavu")
S("gender", "pohlaví")

Orb("the Dragon", "Draka")

S("This Orb allows you to throw fire, just like the Fire Cultists.\n\n"
  "Each fire drains 5 charges. You are not allowed to throw fire into adjacent cells.",
  "Tato sféra ti umožňuje vrhat oheň jako Kultisté ohně.\n\n"
  "Každé použití sníží náboj sféry o 5. Oheň nelze vrhnout na sousední políčko."
  )

S("You throw fire!", "Vrhl jsi oheň!")

Orb("Trickery", "Klamu")

N("Illusion", GEN_F, "Iluze", "Iluze", "Iluzi", "Iluzí")

S("This Orb allows you to create illusions of yourself. Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.\n\n"
  "Each illusion takes 5 charges to create, and one extra charge "
  "per turn. You can also click your illusion to take it away, restoring 4 charges.\n\n"
  "If you have both Orb of Teleport and Orb of Trickery, Illusion is cast "
  "first -- you can then teleport on your Illusion to switch places with it.",
  "Tato sféra ti umožňuje vytvářet své iluzorní dvojníky. Většina netvorů půjde po tvých "
  "iluzích, stejně jako jdou po tobě, po Tloucích a po tvých přátelích.\n\n"
  "Stvoření každé iluze sníží náboj sféry o 5 a vyčerpává 1 náboj navíc za kolo. "
  "Můžeš také kliknout na svou iluzi a zrušit ji, čímž se náboj sféry zvýší o 4.\n\n"
  "Pokud máš Sféru Teleportace a Sféru Klamu zároveň, nejprve použiješ Sféru Klamu -- "
  "poté se můžeš teleportovat na svou iluzi a vyměnit si s ní místo."
  )

S("Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.",
  "Většina netvorů půjde po tvých "
  "iluzích, stejně jako jdou po tobě, po Tloucích a po tvých přátelích.")

S("You create an Illusion!", "Vytvořil jsi iluzi!")
S("You take the Illusion away.", "Zrušil jsi iluzi.")

S("Cannot teleport on a monster!", "Nemůžeš se teleportovat na netvora!")
S("Cannot teleport on an item!", "Nemůžeš se teleportovat na předmět!")
S("Cannot teleport here!", "Sem se teleportovat nemůžeš!")
//S("You cannot teleport that far away!", "Tak daleko se teleportovat nemůžeš!")

S("You cannot target that far away!", "Nemůžeš zamířit tak daleko!")
S("You cannot target that close!", "Nemůžeš zamířit tak blízko!")
S("Cannot teleport on a monster!", "Nemůžeš se teleportovat na netvora!")
S("Cannot teleport on an item!", "Nemůžeš se teleportovat na předmět!")
S("Cannot teleport here!", "Sem se teleportovat nemůžeš!")
S("Cannot cast illusion on an item!", "Nemůžeš seslat iluzi na předmět!")
S("Cannot cast illusion here!", "Sem nemůžeš seslat iluzi!")
S("Cannot cast illusion on a monster!", "Nemůžeš seslat iluzi na netvora!")
S("Cannot throw fire there!", "Sem nemůžeš vrhnout oheň!")

S("or ESC to see how it ended", "nebo ESC, abys vidě%l0, jak to skončilo")
S("high contrast", "vysoký kontrast")
S("draw the heptagons darker", "tmavší sedmiúhelníky")
S("targetting ranged Orbs Shift+click only",
  "zaměřování dálkových Sfér pouze kombinací Shift + kliknutí")
S("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc.",
  "Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, atd.")
S("also hold Alt during the game to toggle high contrast",
  "Podržením klávesy Alt během hry lze přepnout vysoký kontrast")

// Crossroads II
// -------------

N("Crossroads II", GEN_N, "Křižovatka II", "Křižovatky II", "Křižovatku II", "na Křižovatce II")

S(
  "An alternate layout of the Crossroads. It is more dense and more regular, "
  "although you won't find the castle of Camelot here.",

  "Alternativní plán Křižovatky. Je hustší a pravidelnější, ale hrad Camelot tady nenajdete.")

// Caribbean
// ---------

N("Caribbean", GEN_M, "Karibik", "Karibiky", "Karibuku", "v Karibiku")

N("Pirate", GEN_M, "Pirát", "Piráti", "Piráta", "Pirátem")
N("Shark", GEN_M, "Žralok", "Žraloci", "Žraloka", "Žralokem")
N("Parrot", GEN_M, "Papoušek", "Papoušci", "Papouška", "Papouškem")
N("Pirate Treasure", GEN_M, "Pirátský poklad", "Pirátské poklady", "Pirátský poklad", "Pirátským pokladem")
N("Compass", GEN_M, "Kompas", "Kompasy", "Kompas", "Kompasem")

N("sea", GEN_N, "moře", "moře", "moře", "mořem")
N("boat", GEN_O, "člun", "čluny", "člun", "člunem")
N("island", GEN_O, "ostrov", "ostrovy", "ostrov", "ostrovem")
N("tree", GEN_O, "strom", "stromy", "strom", "stromem")

Orb("Time", "Času")

S("This dangerous sea contains nasty sharks and pirates. ",
  "V tomto nebezpečném moři se vyskytují nebezpeční žraloci a piráti.")

S("The islands of Caribbean are infinite and circular, and "
  "the pirates probably have hidden their treasures somewhere far from the coast.",
  "Karibské ostrovy jsou nekonečné a kulaté a piráti pravděpodobně ukryli své poklady kdesi daleko od pobřeží.")

S(
    "Hyperbolic pirates do not need huge ships, since so many lands to conquest "
    "are so close. These small boats are enough for them.\n\n"
    "Boats allow you to go through water. If you are in a boat, you can move into "
    "a water cell (and the boat will come with you).",

    "Hyperboličtí piráti nepotřebují velké lodě, protože tolik zemí, které mohou dobývat, "
    "je tak blízko. Tyto malé čluny jim stačí.\n\n"
    "Čluny ti umožňují pohybovat se po vodě. Pokud jsi ve člunu, můžeš vstoupit na "
    "vodní políčko (člun se přesune spolu s tebou).")

S(
    "The forests of Caribbean are too dense to be traversed by humans, "
    "and they are hard to burn. Many colorful parrots can be found there.",
    "Karibské lesy jsou příliš husté na to, aby jimi mohl projít člověk, "
    "a špatně hoří. Lze v nich najít mnoho barevných papoušků.")

S("Ye wonder where did th' Pirates find all these riches...",
    "Říkáš si, kde asi ti piráti našli všechno tohle bohatství...")

S(
  "The hyperbolic pirates have no use for treasure maps. However, they have found "
  "out that a compass points to the center of the island. So they just go as "
  "far towards the center as they can, and hide their treasure there.",

  "Mapy jsou hyperbolickým pirátům k ničemu. Zjistili ale, že kompas vždy ukazuje do středu ostrova. "
  "A tak se prostě vydávají tak daleko do středu, jak to jen jde, a tam ukrývají své poklady.")

S("Just a nasty shark.", "Obyčejný, nebezpečný žralok.")

S("Parrots feel safe in the forests of Caribbean, so they "
    "never leave them. But they will help the Pirates by attacking the intruders.",

  "Papoušci se v karibských lesích cítí bezpečně, a tak je nikdy neopouštějí. "
  "Pomáhají ale pirátům tím, že útočí na vetřelce.")

S("Just a typical hyperbolic pirate.", "Typický hyperbolický pirát.")

S(
    "Normally, the power of most Orbs slowly fades away, even when "
    "you are not actively using them. This Orb prevents this.\n\n"

    "When you have the Orb of Time, Orbs which are not active won't lose their power. "
    "Orbs are considered active if they have a continuous power which has actually "
    "affected something in the last turn.\n\n"

    "Orbs of Shielding remain active after being activated (even if you are no longer "
    "attacked), and Orbs of Time have a bigger cap inside their native Caribbean than "
    "outside.",

    "Síla většiny Sfér se většinou pomalu vytrácí, a to i tehdy, když je aktivně nepoužíváte. "
    "Tato sféra tomu brání.\n\n"

    "Když máš Sféru Času, nebudou neaktivní Sféry ztrácet sílu. 'Aktivní Sféra' je taková Sféra, "
    "která má stálý efekt a která v minulém kole něco skutečně ovlivnila.\n\n"

    "Sféry Štítu zůstávají trvale aktivní, jakmile jednou zablokují útok (i když na tebe právě nikdo "
    "neútočí a Sféry Času mohou ve svém domovském Karibiku dosahovat vyšších hodnot síly než mimo něj.")

// Achievements
// ------------

// Parrot : Find and collect Pirate Treasure.
// Caribbean Shark : Collect 10 Pirate Treasures.
// Pirate : Collect 25 Pirate Treasures.
// Master of Caribbean : Collect 50 Pirate Treasures.

// Papoušek : Najdi a získej Pirátský poklad.
// Karibský žralok : Získej 10 Pirátských pokladů.
// Pirát : Získej 25 Pirátských pokladů.
// Pán Karibiku : Získej 50 Pirátských pokladů.

// Red Rock Valley
// ---------------

N("Red Rock Valley", GEN_N, "Údolí červených skal", "Údolí červených skal", "Údolí červených skal", "v Údolí červených skal")

N("Rock Snake", GEN_M, "Kamenný had", "Kamenní hadi", "Kamenného hada", "Kamenným hadem")
N("Rock Snake Tail", GEN_O, "Ocas Kamenného hada", "Ocasy Kamenných hadů", "Ocas Kamenného hada", "Ocasem Kamenného hada")
N("Red Troll", GEN_M, "Červený troll", "Červení trollové", "Červeného trolla", "Červeným trollem")
N("Red Gem", GEN_O, "Červený drahokam", "Červené drahokamy", "Červený drahokam", "Červeným drahokamem")

N("rock I", GEN_F, "skála I", "skály I", "skálu I", "skálou I")
N("rock II", GEN_F, "skála II", "skály II", "skálu II", "skálou II")
N("rock III", GEN_F, "skála III", "skály III", "skálu III", "skálou III")

Orb("Space", "Prostoru")

S("Not enough power for telekinesis!", "Nemáš dost síly na telekinezi!")

S("This land contains high rock formations. Most of the valley is at level 0, "
  "while items are found at level 3. It is impossible to gain two or more levels, "
  "or to lose three levels, in a single move, (attacks are possible at any "
  "difference, though). Kill Red Trolls and Rock Snakes to make a cell higher.",

  "V tomto kraji se nacházejí vysoké skalní útvary. Většina údolí je na úrovni 0, "
  "ale předměty se nacházejí pouze na úrovni 3. Jedním pohybem se můžete dostat "
  "maximálně o jednu úroveň výš nebo o dvě úrovně níž, ale útočit je možné bez "
  "ohledu na výškový rozdíl. Zabíjením Červených trollů a Skalních hadů můžete "
  "pole posouvat nahoru.")

S("A kind of Troll native to the Red Rock Valley.",
  "Typ trolla, který se vyskytuje v Údolí červených skal.")

S(
  "Rock snakes are similar to Sandworms, but they move each turn. However, they "
  "only move on the hexagonal cells. They are also somewhat longer than the Sandworms.",

  "Kamenní hadi jsou podobní Písečným červům, ale pohybují se v každém kole. Mohou se "
  "však pohybovat pouze po šestiúhelníkových polích. Kromě toho jsou oproti Písečným "
  "červům o něco delší.")

S("A gem from the Red Rock Valley.", "Drahokam z Údolí červených skal.")

S(
    "This Orb is able to bring faraway items to your location, even if there are "
    "monsters or barriers on the way. The cost of "
    "bringing an item (in charges) equals the square of its distance to you. Contrary "
    "to some other Orbs, usage is not allowed if you have not enough power left.",

  "Tato Sféra vám dokáže přinášet vzdálené předměty, a to i skrz netvory nebo bariéry. "
  "Přinesení předmětu stojí tolik síly, kolik je druhá mocnina vzdálenosti předmětu. "
  "Na rozdíl od některých jiných Sfér, Sféru Prostoru není možné použít, pokud vám už "
  "nezbývá dostatečné množství síly.")

// Climber : Find and collect a Red Gem.
// Red Troll : Collect 10 Red Gems.
// Rock Snake : Collect 25 Red Gems.
// Master of Red Rock : Collect 50 Red Gems.

// Horolezec : Najdi a získej Červený drahokam.
// Červený troll : Získej 10 Červených drahokamů.
// Kamenný had : Získej 25 Červených drahokamů.
// Pán Červených skal: Získej 50 Červených drahokamů.

// missing sentences

S("%The1 drowns!", "%1 se utopi%l1!")
S("%The1 falls!", "%1 spad%l1!")

// these were missing from the translation for some reason

S("Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.",
  "Tato jezera se nacházejí po celém Pekle... Mají tvar zlých hvězd a jsou naplněna hořící sírou.")

// Hardcore Mode
// -------------

S("hardcore mode", "hardcore mód")

S("One wrong move and it is game over!", "Jediný špatný tah a hra končí!")
S("Not so hardcore?", "Že bys nebyl zas až tak hardcore?")

// Shoot'em up Mode
// ----------------

S("shoot'em up mode", "mód střílečky")
S("Welcome to the Shoot'em Up mode!", "Vítej v módu střílečky!")
S("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move", "F/;/Space/Enter/KP5 = střelba, WASD/IJKL/Numpad = pohyb")

N("Rogue", GEN_M, "Chytrák", "Chytráci", "Chytráka", "Chytrákem")
N("Knife", GEN_O, "Nůž", "Nože", "Nůž", "Nožem")
N("Flail", GEN_O, "Řemdih", "Řemdihy", "Řemdih", "Řemdihem")
N("Fireball", GEN_F, "Ohnivá koule", "Ohnivé koule", "Ohnivou kouli", "Ohnivou koulí")
N("Tongue", GEN_O, "Jazyk", "Jazyky", "Jazyk", "Jazykem")

S("In the Shoot'em Up mode, you are armed with thrown Knives.",
  "V módu střílečky jsi vyzbrojený vrhacími noži.")
S("A simple, but effective, missile, used by rogues.",
  "Prostá, ale efektivní vrhací zbraň, kterou používají lovci pokladů.")
S("This attack is likely to hit the attacker.",
  "Tento útok může velmi snadno zasáhnout i samotného útočníka.")
S("This magical missile burns whatever it hits.", "Tato magická střela spálí všechno, co zasáhne.")
S("Some monsters have long tongues, which allow them to attack enemies in nearby cells.",
  "Někteří netvoři mají dlouhé jazyky, které jim umožňují útočit na sousední pole.")

// modes for the local highscores
// ------------------------------

S(", m - mode: normal", ", m - normální mód")
S(", m - mode: hardcore only", ", m - pouze hardcore mód")
S(", m - mode: shoot'em up", "m - mód střílečky")

// update description for Steam
// ----------------------------

// (Mód střílečky a dva nové kraje oproti neplacené verzi. Další kraje jsou v plánu!)

S("You would get hurt!", "To by bolelo!")
S("PARTIAL", "CZĘŚCIOWO")

S("Cannot drop %the1 here!", "Tady nelze položit Mrtvou Sféru!")

// Euclidean scores
// ----------------

S(" (E:%1)", " (E:%1)")

S("You cannot attack Rock Snakes directly!", "Na Kamenné hady nemůžeš útočit přímo!")

S("\"I am lost...\"", "\"I am lost...\"")

S("You are killed by %the1!", "Zabi%l1 tě %1!")

// achievements from Version 7.0 which have not yet been translated

// Hardcore : Získej Yendorskou sféru v hardcore módu
// Mistr nožů : Získej Yendorskou sféru v módu střílečky

// new menu for Version 7.1
// ------------------------

S("(v) menu", "(v) menu")
S("return to the game", "vrať se do hry")
S("or 'v' to see the main menu", "nebo stiskni 'v' pro hlavní menu")

S("HyperRogue %1", "HyperRogue %1")
S("basic configuration", "základní konfigurace")
S("advanced configuration", "pokročilá konfigurace")
S("local highscores", "lokální hi-score")
S("help", "nápověda")
S("restart game", "restartovat hru")
S("special game modes", "speciální herní módy")
S("save the game", "uložit a ukončit hru")
S("quit the game", "ukončit hru")
S("review your quest", "přehled úkolů")
S("review the scene", "přehled scény")
S("game over screen", "obrazovka konce hry")
S("continue game", "pokračuj ve hře")
S("play the game!", "hraj!")

// fixed descriptions for Shmup achievements
// -----------------------------------------

S("You have used less knives than ever before. Congratulations!",
  "Použil jsi méně nožů než kdykoli předtím. Gratulujeme!")

S("You have improved both your real time and knife count. Congratulations!",
  "Zlepšil jsi jak svůj čas, tak počet nožů. Gratulujeme!")

S("%1 knives (%2)", "nožů: %1 (%2)")

// temporary
S("No description yet." ,"Zatím bez popisu.")

// special case when no Spice is visible
S("The sandworm explodes!", "Písečný červ vybuchl!")

// Ocean
// -----

// achievements:
// Došel jsi k Oceánu: Najdi a získej Jantar.
// Albatros: Získej 10 Jantarů.
// Jantarový albatros: Získej 25 Jantarů.
// Pán přílivu: Získej 50 Jantarů.

N("Ocean", GEN_O, "Oceán", "Oceány", "Oceán", "na Oceánu")

S(
    "You can collect some valuable Ambers on the coast, but beware the tides!\n\n"
    "You can also take one of the boats and venture into the Ocean, "
    "to find other coasts, dangerous whirlpools, and islands inhabited by Pirates.",

  "Na pobřeží můžeš sbírat cenný Jantar, ale pozor na příliv a odliv!\n\n"
  "Také si můžeš vzít člun a vyplout na Oceán, "
  "kde najdeš další pobřeží, nebezpečné víry a ostrovy obývané piráty.")

N("Albatross", GEN_O, "Albatros", "Albatrosové", "Albatrosa", "Albatrosem")

S(
  "Those large seabirds attack you for some reason. At least they are not "
  "as fast as Eagles...",
  "Tito velcí mořští ptáci na tebe z nějakého důvodu útočí. Aspoň že nejsou tak rychlí jako Orli...")

N("stranded boat", GEN_O, "člun na mělčině", "čluny na mělčině", "člun na mělčině", "ve člunu na mělčině")

S(
  "This boat cannot go through the sand. But if you sit inside and "
  "wait for the tide, you will be able to use it to travel through the Ocean.",

  "Tento člun nemůže plout po písku. Ale pokud si do něj sedneš a počkáš na příliv, "
  "můžeš na něm cestovat přes Oceán."
  )

N("Ocean", GEN_O, "Oceán", "Oceány", "Oceán", "na Oceánu")

N("Amber", GEN_O, "Jantar", "Jantary", "Jantar", "Jantarem")

S(
  "When the tide is away, beautiful ambers can be found on the hyperbolic beaches. "
  "Perhaps there used to be a forest here?",
  "Za odlivu můžeš na hyperbolické pláži najít krásné kusy jantaru."
  "Možná tu kdysi býval les?")

Orb("Air", "Vzduchu")

S("This Orb allows you to blow your enemies away.\n\n"
  "Click a monster to blow it one cell away. It cannot be used against mimics, ghosts, sharks and other monsters restricted to a specific terrain, and multi-tile monsters."
,
  "Tato sféra ti umožňuje odfouknout nepřátele daleko od tebe.\n\n"
  "Kliknutím na netvora ho můžeš odfouknout o jedno pole od sebe. Tuto schopnost nelze použít na mimiky, duchy, žraloky a jiné netvory, kteří jsou omezení pouze na specifický terén nebo zabírají více než jedno pole."
  )

N("Sea Border", GEN_O, "Mořská hranice", "Mořské hranice", "Mořskou hranici", "na Mořské hranici")

S("Border between seas.", "Hranice mezi moři.")

// Whirlpool
// ---------

// achievements:
// Útěk z Víru: Získej Perlu a unikni z Víru.
// Vírový žralok: Získej 10 Perel.
// Vírový pirát: Získej 25 Perel.
// Pán Víru: Získej 50 Perel.

N("Whirlpool", GEN_O, "Vír", "Víry", "Vír", "ve Víru")

S(
  "Many lost boats are spinning in this whirlpool. Some of them contain treasures "
  "and Pirates.\n\n"
  "Unmanned boats will go one cell clockwise in each turn. Sharks and manned boats "
  "are only allowed to go with the current, that is, towards the center, or clockwise.",

  "V tomto víru se točí mnoho ztracených člunů. Na některých z nich se nachází poklad nebo piráti.\n\n"
  "Čluny bez posádky se v každém kole posunou o jedno pole po směru hodinových ručiček. Žraloci a čluny s posádkou "
  "se smějí pohybovat pouze po proudu, tedy do středu nebo po směru hodinových ručiček "
  )

N("Pearl", GEN_F, "Perla", "Perly", "Perlu", "Perlou")

S(
  "You do not know exactly why, but there are valuable pearls on many boats "
  "in the whirlpool.",
  "Nevíš tak úplně jistě proč, ale na mnoha člunech ve Víru jsou cenné perly."
  )

Orb("Water", "vody")

S( "This Orb allows your boat to go against the current, "
   "and also to go into the land, creating water on the way.",
   "Tato Sféra tvému člunu umožňuje plout proti proudu. "
   "Také můžeš vyplout na souš a přitom vytvářet vodu, kamkoli se vydáš.")

S("You cannot go against the current!", "Nemůžeš plout proti proudu!")

// Minefield
// ---------

// achievements:
// Došel jsi na Minové pole: Získej Vejce Bombarďáka.
// Minolovka: Získej 10 Vajec Bombarďáka.
// Bombarďák: Získej 25 Vajec Bombarďáka.
// Pán Minového pole: Získej 50 Vajec Bombarďáka.

N("Minefield", GEN_N, "Minové pole", "Minová pole", "Minové pole", "v Minovém poli")
N("minefield", GEN_N, "minové pole", "minová pole", "minové pole", "minovým polem")
N("cell without mine", GEN_N, "pole bez miny", "pole bez min", "pole bez miny", "polem bez min")

S(
  "This minefield has been created by the Bomberbirds to protect their valuable eggs. "
  "Mines explode when they are stepped on, creating fire all around them "
  "(this fire never expires in the Minefield). If you don't step on a mine, you "
  "know the number of mines in cells around you. These numbers are marked with "
  "color codes on the cells you have visited.",
  "Toto minové pole vytvořili ptáci Bombarďáci, aby ochránili svá cenná vejce. "
  "Mina exploduje, když na ni někdo nebo něco šlápne, čímž zapálí oheň na svém poli a na všech sousedních polích "
  "(v Minovém poli tento oheň nikdy neuhasne). A samozřejmě ji také odpálí jakýkoli oheň! Pokud "
  "nešlápneš na minu, budeš vždy vědět, kolik min se nachází na polích okolo tebe. "
  "Tento počet bude označený barevnými kódy na polích, která jsi už navštívil."
  )

N("Bomberbird", GEN_M, "Bombarďák", "Bombarďáci", "Bombarďáka", "Bombarďákem")

S(
    "Dark red birds who have created the minefield.\n\n"
    "They create a mine on the spot where they are killed, provided "
    "that the terrain is suitable. Also note that mines are triggered "
    "by dead birds falling on them.",

    "Tmavočervení ptáci, kteří vytvořili minové pole.\n\n"
    "Když zemřou, vytvoří na poli, kam spadnou, minu, pokud to terén dovoluje. "
    "Měj na paměti, že i pád mrtvého ptáka na minu stačí k tomu, aby ji odpálil!")

N("Tame Bomberbird", GEN_M, "Ochočený Bombarďák", "Ochočení Bombarďáci", "Ochočeného Bombarďáka", "Ochočeným Bombarďákem")

S(
  "This bomberbird will follow you at some distance, and attack your enemies. "
  "You can kill it to place a mine.",

  "Tento Bombarďák tě bude sledovat v určité vzdálenosti a útočit na tvé nepřátele. "
  "Kromě toho ho můžeš zabít, a tím položit minu.")

N("Bomberbird Egg", GEN_N, "Vejce Bombarďáka", "Vejce Bombarďáka", "Vejce Bombarďáka", "Vejcem Bombarďáka")

S(
  "Bomberbird eggs are big and tasty, and thus valuable. "
  "They can hatch when left alone for some time (but this will never happen "
  "if you are watching).",

  "Vejce Bombarďáků jsou velká a chutná -- a tudíž i cenná. Mohou se vylíhnout, "
  "když je necháš nějaký čas na pokoji (ale to se nikdy nestane, když se na ně díváš)."
  )

Orb("Friendship", "Přátelství")

S("This Orb summons a friendly Bomberbird.",
  "Tato sféra vyvolá přátelského Bombarďáka.")

S("Nowhere to blow %the1!", "%a1 není kam odfouknout!")
S("%The1 is immune to wind!", "%1 je imunní proti větru!")
S("You blow %the1 away!", "Odfoukl jsi %a1!")

S("WARNING: you are entering a minefield!", "VAROVÁNÍ: vstupuješ do minového pole!")
S("No mines next to you.", "Ve tvé blízkosti se nenacházejí žádné miny.")
S("A mine is next to you!", "Ve tvé blízkosti se nachází jedna mina!")
S("Two mines next to you!", "Ve tvé blízkosti se nacházejí dvě miny!")
S("Three mines next to you!", "Ve tvé blízkosti se nacházejí tři miny!")
S("Four mines next to you!", "Ve tvé blízkosti se nacházejí čtyři miny!")
S("Five mines next to you!", "Ve tvé blízkosti se nachází pět min!")
S("Six mines next to you!", "Ve tvé blízkosti se nachází šest min!")
S("Seven mines next to you!", "Ve tvé blízkosti se nachází sedm min!")

S("Still confused? Read the FAQ on the HyperRogue website!\n\n",
  "Stále zmatení? Přečtěte si FAQ na webové stránce HyperRogue (prozatím pouze v angličtině)\n\n")

S("You have to run away from the water!", "Musíš utéct před vodou!")

// VERSION 7.2
// ===========

// Palace
// ------

N("Palace", GEN_O, "Palác", "Paláce", "Palác", "v Paláci")
N("palace wall", GEN_F, "zeď paláce", "zdi paláce", "zeď paláce", "zeď paláce")

S("Apparently a princess is kept locked somewhere, but you won't ever find her in this hyperbolic palace. ",
  "Někde je tu prý uvězněná princezna, ale tu v tomhle hyperbolickém paláci nikdy nenajdeš. ")

S("Apparently a prince is kept locked somewhere, but you won't ever find him in this hyperbolic palace. ",
  "Někde je tu prý uvězněný princ, ale toho v tomhle hyperbolickém paláci nikdy nenajdeš. ")

S("So better just to concentrate on collecting treasure. "
  "Beware the traps, and the guards, who are hard to kill!",

  "Takže se raději soustřeď jen na sbírání pokladů. "
  "Pozor na pasti a na stráže -- je těžké je zabít!"
  )

N("trapdoor", GEN_N, "propadlo", "propadla", "propadlo", "propadlem")
S("This floor will fall after someone goes there. Go quickly!",
  "Tato podlaha se propadne poté, co přes ni někdo projde. Běž rychle!")

N("closed gate", GEN_F, "zavřená brána", "zavřené brány", "zavřenou bránu", "zavřenou bránou")
N("open gate", GEN_F, "otevřená brána", "otevřené brány", "otevřenou bránu", "otevřenou bránou")
N("closing plate", GEN_F, "zavírací deska", "zavírací desky", "zavírací desku", "zavírací deskou")
N("opening plate", GEN_F, "otvírací deska", "otvírací desky", "otvírací desku", "otvírací deskou")

S("Green pressure plates open all gates in radius 3 around them. "
  "Similarly, red pressure plates close all gates in this radius.",

  "Zelené desky otevřou všechny brány v okruhu 3 polí, když na ně někdo šlápne. "
  "Červené desky naopak všechny brány v tomto okruhu zavřou.")

N("Hypersian Rug", GEN_O, "Hyperský koberec", "Hyperské koberce", "Hyperský koberec", "Hyperským kobercem")
S("Nice, a hyperbolic rug! It will not fit very well on your flat Euclidean floor, but who cares?",
  "Prima, hyperbolický koberec! Na tvoji placatou eukleidovskou podlahu moc dobře pasovat nebude, ale co?")

Orb("the Frog", "Žáby")
S("This Orb lets you jump to a place which is two cell away from you, in a single turn. "
  "You can jump over water, chasms and fire, but not over walls.",
  "Tato Sféra ti umožňuje skočit v jednom kole dvě pole daleko. "
  "Můžeš skákat přes vodu, propasti a oheň, ale ne přes zdi.")
S("Cannot jump through %the1!", "Nemůžeš skočit přes %a1!")
S("You jump!", "Skočil jsi!")

S("\n\nNote: when pushing %the1 off a heptagonal cell, you can control the pushing direction "
  "by clicking left or right half of the heptagon.",
  "\n\nPoznámka: Když tlačíš %a1 ze sedmiúhelníku, můžeš ovládat směr tlačení "
  "tím, že klikneš na levou či pravou polovinu tohoto sedmiúhelníku."
  )

S(" With the keyboard, you can rotate the view for a similar effect (Page Up/Down).",
  " Na klávesnici můžeš podobného efektu dosáhnout otočením hrací plochy (Page Up/Down).")

N("Palace Guard", GEN_M, "Strážce Paláce", "Strážci Paláce", "Strážce Paláce", "Strážcem Paláce")

S("You have to hit Palace Guards several times to kill them. After each hit, they "
  "are pushed away and stunned for some time.",
  "Na zabití Strážce Paláce potřebuješ několik útoků. Každý zásah Strážce "
  "odstrčí dozadu a na chvíli ho omráčí.")

N("Fat Guard", GEN_M, "Tlustý strážce", "Tlustí strážci", "Tlustého strážce", "Tlustým strážcem")

S("Fat guards are too heavy to be pushed away by your hits.",
  "Tlustí strážci jsou příliš těžcí na to, aby je tvé zásahy dokázaly odstrčit.")

N("Skeleton", GEN_M, "Kostlivec", "Kostlivci", "Kostlivce", "Kostlivcem")
S("Skeletons work similar to Palace Guards, but they won't die no matter how many "
  "times you hit them. Well, you have to be more creative...\n\n"
  "Skeletons attacked outside of their native land, Palace, are stunned for a longer time.",
  "Kostlivci fungují podobně jako Strážci Paláce, ale ať na ně zaútočíš, "
  "kolikrát chceš, nikdy je nezabiješ. No, musíš být vynalézavější...\n\n"
  "Mimo Palác, jejich rodný kraj, budou Kostlivci útokem omráčení na delší dobu."
  )

N("Vizier", GEN_M, "Vezír", "Vezírové", "Vezíra", "Vezírem")
S("Viziers are neither pushed away nor stunned. However, you attack them automatically when "
  "escaping from them.",
  "Vezíry nemůžeš odstrčit ani omráčit, ale automaticky na ně útočíš, když od nich utíkáš.")

S("You stun %the1.", "Omráčil jsi %a1.")
S("The gate is closing right on you! RUN!", "Brána se zavírá! UTÍKEJ!")
S("%The1 is crushed!", "Brána rozdrtila %a1!")
S("You hit %the1.", "Zasáhl jsi %a1.")
S("Hit him by walking away from him.", "Zasáhni ho tak, že se pohneš směrem od něj.")
S("Hmm, he has been training in the Emerald Mine. Interesting...", "Hmm, tak on trénoval ve Smaragdovém dole. Zajímavé...")

// Reached the Palace: Find and collect a Hypersian Rug.
// Palace Guard: Collect 10 Hypersian Rugs.
// Vizier: Colect 25 Hypersian Rugs.
// Prince: Colect 50 Hypersian Rugs.

// Došel jsi do Paláce: Najdi a získej Hyperský koberec.
// Strážce Paláce: Získej 10 Hyperských koberců.
// Vezír: Získej 25 Hyperských koberců.
// Princ: Získej 50 Hyperských koberců.

// Living Fjord
// ------------

N("Living Fjord", GEN_O, "Živoucí fjord", "Živoucí fjordy", "Živoucí Fjord", "v Živoucím Fjordu")

S("A coastal area, from where you can get both to the inland worlds and to the Ocean. "
  "Each turn, each cell will become water or earth, based on the majority of cells around it. "
  "Contrary to the Living Caves, this process is not affected by most items and monsters, "
  "but elementals, dead Trolls, and cells outside of the Living Fjord have "
  "a very powerful effect.",
  "Pobřežní oblast, odkud se můžeš dostat jak do vnitrozemských světů, tak do Oceánu. "
  "V každém kole se každé pole změní na vodu nebo na souš podle toho, kterého typu je většina polí kolem něj. "
  "Na rozdíl od Živoucí jeskyně nemá většina předmětů a netvorů na tento proces žádný vliv, "
  "ale elementálové, mrtví trollové a pole mimo Živoucí fjord mají "
  "velmi silný efekt."
  )

N("Fjord Troll", GEN_M, "Fjordtroll", "Fjordtrollové", "Fjordtrolla", "Fjordtrollem")
S("Fjord Trolls leave a wall when they die, causing the living fjord to rise around it. "
  "Contrary to Rock Trolls, items around are not destroyed.",
  "Fjordtrollové se po smrti promění v zeď, kolem které se zvedají vody Živoucího fjordu. "
  "Fjordtrollové na rozdíl od Šutrollů neničí předměty kolem sebe."
  )

N("Viking", GEN_M, "Viking", "Vikingové", "Vikinga", "Vikingem")
S("Mighty warriors from the Fjord, who can use boats.",
  "Mocní válečníci z Fjordu, kteří se dokážou plavit na člunech.")

N("Water Elemental", GEN_M, "Elementál Vody", "Elementálové Vody", "Elementála Vody", "Elementálem Vody")
S("Wherever this powerful being goes, the living fjord "
  "sinks below water, non-magical boats are destroyed, and fires are extinguished.\n\n"
  "As a special case, you can attack the Water Elemental from the water, without drowning immediately.",
  "Kamkoli se vydá tato mocná bytost, tam se Živoucí fjord potápí pod vodu, všechny čluny kromě magických "
  "jsou zničeny a veškerý oheň je uhašen.\n\n"
  "Na Elementála Vody můžeš zaútočit z vody, aniž by ses okamžitě utopil; to je speciální případ.")


N("Garnet", GEN_O, "Granát", "Granáty", "Granát", "Granátem")
S("Vikings believe that garnets improve their strength.",
  "Vikingové věří, že granáty zvyšují jejich sílu.")

Orb("the Fish", "Ryby")
S("This Orb lets you dive into the water. While diving, you are able to see and collect underwater treasures, "
  "while being invisible to monsters yourself. You are afraid of jumping into the water straight from a boat, so usually you won't do it.",
  "Tato Sféra ti umožňuje potopit se pod vodu. Když jsi potopený, můžeš vidět a získávat podvodní poklady, "
  "zatímco netvoři tě nevidí. Obvykle ale nebudeš skákat do vody přímo ze člunu -- to tě trochu děsí.")

S("%The1 is washed away!", "Elementál Vody spláchl %a1!")
S("%The1 is extinguished!", "Elementál Vody uhasil %a1!")

// Reached the Living Fjord: Find and collect a Garnet.
// Viking: Collect 10 Garnets.
// Water Elemental: Colect 25 Garnets.
// Master of the Fjord: Colect 50 Garnets.

// Došel jsi do Živoucího fjordu: Najdi a získej Granát.
// Viking: Získej 10 Granátů.
// Elementál Vody: Získej 25 Granátů.
// Pán Fjordu: Získej 50 Granátů.

Orb("Discord", "Nesváru")

S("Causes most monsters to attack other monsters, not only you and your friends.",
  "Díky této Sféře bude většina netvorů útočit nejen na tebe a na tvé kamarády, ale i na jiné netvory.")

// Shmup Configuration
// -------------------

S("forward", "dopředu")
S("backward", "dozadu")
S("turn left", "otočení vlevo")
S("turn right", "otočení vpravo")
S("move up", "nahoru")
S("move right", "doprava")
S("move down", "dolů")
S("move left", "doleva")
S("throw a knife", "házení nože")
S("face the pointer", "otočení ke kurzoru")
S("throw at the pointer", "házení na kurzor")
S("drop Dead Orb", "položení Mrtvé sféry")
S("center the map on me", "vycentrování mapy na postavě")
S("activate Orb power", "aktivace schopnosti Sféry")

S("pan up", "scrollování nahoru")
S("pan right", "scrollování doprava")
S("pan down", "scrollování dolů")
S("pan left", "scrollování doleva")
S("rotate left", "otočení vlevo")
S("rotate right", "otočení vpravo")
S("home", "vycentrování")

S("do nothing", "nic")
S("rotate view", "otočení hrací plochy")
S("panning X", "scrollování ve směru X")
S("panning Y", "scrollování ve směru Y")
S("player 1 X", "hráč 1 X")
S("player 1 Y", "hráč 1 Y")
S("player 1 go", "hráč 1 pohyb")
S("player 1 spin", "hráč 1 otáčení")
S("player 2 X", "hráč 2 X")
S("player 2 Y", "hráč 2 Y")
S("player 2 go", "hráč 2 pohyb")
S("player 2 spin", "hráč 2 otáčení")

S("Joystick %1, axis %2", "Joystick %1, osa %2")
S("one player", "jeden hráč")
S("two players", "dva hráči")
S("configure player 1", "konfigurace hráče 1")
S("configure player 2", "konfigurace hráče 2")
S("configure panning", "konfigurace scrollování")
S("configure joystick axes", "konfigurace os joysticku")
S("continue playing", "pokračuj ve hře")
S("start playing in the shmup mode", "začni hrát v módu střílečky")
S("return to the turn-based mode", "vrať se do módu na kola")
S("save the configuration", "ulož konfiguraci")
S("press a key to unassign", "stiskni klávesu, kterou chceš zrušit")
S("press a key for '%1'", "stiskni klávesu pro '%1'")
S("unassign a key", "zruš klávesu")

// extra pattern modes
// -------------------

S("explore the Emerald Pattern", "Zkoumání Smaragdového vzoru")
S("explore the Palace Pattern (4 colors)", "Zkoumání Palácového vzoru (4 barvy)")
S("explore the Palace Pattern (8 colors)", "Zkoumání Palácového vzoru (8 barev)")

// extra flavor/Knight lines
// -------------------------

S("Kill a Vizier in the Palace to access Emerald Mine", "Zabij Vezíra v Paláci a dostaneš se do Smaragdového dolu")
S("Collect 5 Emeralds to access Camelot", "Získej 5 Smaragdů a dostaneš se do Camelotu")
S("Some lands unlock at specific treasures or kills", "Některé kraje se otevřou získáním určitých pokladů nebo zabitím určitých netvorů.")

S("You will be fighting red rock snakes, too...", "Budeš bojovat i s kamennými hady...")

S("\"The Red Rock Valley is dangerous, but beautiful.\"",
  "\"Údolí červených skal je nebezpečné, ale krásné.\"")
S("\"Train in the Desert first!\"", "\"Nejdřív se natrénuj v Poušti!\"")
S("\"I like collecting ambers at the beach.\"", "\"Rád sbírám Jantar na pláži.\"")
S("\"Our Table seats %1 Knights!\"", "\"U našeho stolu sedí %1 rytířů!\"")
S("\"There are %1 floor tiles inside our Table!\"", "\"Uvnitř našeho stolu je %1 dlaždic!\"")
S("\"By now, you should have your own formula, you know?\"", "\"Teď už bys mě%l0 mít svůj vlastní vzorec, ne?\"")
S("\"Have you tried to take a boat and go into the Ocean? Try it!\"", "\"Už jsi zkouše%l0 vzít člun a vyplout na Oceán?\"")

// VERSION 7.3
// ===========

// new name for 'Ivy D'

N("Dead Ivy", GEN_M, "Mrtvý břečťan", "Mrtvé břečťany", "Mrtvý břečťan", "Mrtvým břečťanem")

// new messages for living/nonliving monsters

S("%The1 bites %the2!", "%1 kous%l1 %2!")
S("%The1 attacks %the2!", "%1 útočí na %a2!")
S("You destroy %the1 with a mental blast!", "Zniči%l0 jsi %a1 mentálním úderem!")
S("The ivy kills %the1!", "Břečťan zabil %a1!")
S("You destroy %the1.", "Zniči%l0 jsi %a1.")
S("%The1 kills %the2!", "%1 zabi%l1 %a2!")
S("%The1 sinks!", "%1 zmize%l1 pod vodou!")

S("Cannot jump on %the1!", "Nemůžeš skočit na %1!")

// Map Editor
// ----------

S("map editor", "mapový editor")
S("You activate your terraforming powers!", "Aktivova%l0 jsi své terramorfické schopnosti!")
S("Press F5 to clear the board.", "F5 vymaže desku.")
S("Press F5 to restart the game normally.", "Stisknutím F5 restartuješ hru v normálním módu.")
S("High quality shots not available on this platform", "HQ snímky nejsou na této platformě k dispozici")
S("Saved the high quality shot to %1", "HQ snímek uložen do %1")
S("Map saved to %1", "Mapa uložena do %1")
S("Map loaded from %1", "Mapa načtena z %1")
S("copying", "kopíruje")
S("boundary", "hranice")
S("clear monster", "vymazat netvore")
S("use at your own risk!", "použití na vlastní nebezpečí!")

// Note: in English, these have been arranged from longest to shortest.
// This gives a somewhat nice effect on the top left corner of the screen.
// You may want to aim for the same effect when translating

S("0-9 = radius (%1)", "0-9 = poloměr (%1)")
S("b = boundary", "c = kopírování")
S("m = monsters", "i = předměty")
S("w = walls",    "b = hranice")
S("i = items",    "m = netvoři")
S("l = lands",    "l = kraje")
S("c = copy",     "w = zdi")

// And this is from shortest to longest (bottom left corner)
S("F1 = help",                "F1 = nápověda")
S("F2 = save",                "F2 = uložení")
S("F3 = load",                "F3 = nahrání")
S("F5 = restart",             "F5 = restart")
S("F6 = HQ shot",             "F6 = HQ snímek")
S("F7 = player on/off",       "F7 = zapnout/vypnout hráče")
S("ESC = return to the game", "ESC = návrat do hry")

S(
  "This mode allows you to edit the map.\n\n"
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

  "V tomto módu můžeš editovat mapup.\n\n"
  "POZOR: Používej na vlastní nebezpečí. Kombinace, které se v normální hře "
  "nikdy nevyskytují, mohou fungovat nedefinovaným způsobem "
  "(nefungovat, divně vypadat, generovat podivné zprávy nebo způsobit spadnutí hry).\n\n"
  "Abys z tohoto editoru získal co nejvíce, "
  "musíš vědět něco o tom, jak HyperRogue funguje \"pod kapotou\". "
  "Každé políčko má čtyři atributy: typ kraje, typ zdi, typ netvora, a typ předmětu. "
  "Ten samý typ zdi (především \"nic\", \"moře\" nebo \"oheň\") může vypadat nebo se chovat "
  "trochu jinak podle toho, ve kterém kraji se nachází. Některé položky "
  "mohou být v seznamu dvakrát díky drobným rozdílům (například Čerti mají dvě formy podle toho, "
  "zda se v příštím kole mají pohnout nebo ne).\n\n"
  "Klávesami 'w', 'i', 'l' a 'm' udáváš, který aspekt políček chceš změnit; "
  "pak na políčko jednoduše klikni, a ono se změní. Stiskem klávesy 'c' zkopíruješ "
  "políčko pod kurzorem (zkopírují se všechny informace o tomto políčku). "
  "Při kopírování nebo vkládání vícepolíčkových netvorů může být důležité, kam přesně v políčku klikáš.\n\n"
  "Své změny můžeš také aplikovat na větší oblast pomocí kláves 0-9, které nastavují poloměr. "
  "To také ovlivňuje kopírování/vkládání, což ti umožní kopírovat větší oblast.\n\n"
  "F2 uloží současnou mapu (a F3 ji nahraje). Pokud to uděláš po dlouhé hře HyperRogue (kde jsi nepoužil Sféru Bezpečnosti), "
  "bude vzniklý soubor hodně velký! "
  "Ovšem pozor -- velké struktury, jako jsou Velké zdi, velké kruhy a horocykly budou uložením a nahráním mapy zničeny.\n\n"
  "Klávesou 'b' můžeš políčka označit jako hraniční. Tato políčka a políčka za nimi nebudou při operacích kopírování a "
  "vkládání kopírovány a nebudou ani uloženy.\n\n"
  )

// Princess Quest
// --------------

S(
  "A mouse squeaks at you. It seems that it wants you to go somewhere. "
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

  "Vykvíkla na tebe myška. Zdá se, že chce, aby ses někam vyda%l0. "
  "To je zajímavé, copak tam asi najdeš?\n\n"
  "Poznámka: V části Paláce, kterou budeš procházet při plnění této mise, "
  "je poněkud upravené rozdělení netvorů, desek a předmětů, aby byla mise "
  "o něco vhodnější i pro méně zkušené hráče. Počet netvorů nebude záviset na "
  "počtu získaných Hyperských koberců a bude tu více otvíracích desek.\n\n"
  "Nicméně, dosažení cíle by mělo být vždy možné i při normálním rozdělení. "
  "Pokud si chceš tuto obtížnější misi vyzkoušet, vyber ji z menu speciálních herních módů.\n\n")

S("\"When I visited the Palace, a mouse wanted me to go somewhere.\"",
  "\"Když jsem navštívil Palác, byla tam myška, co chtěla, abych se někam vydal.\"")

S("\"I wonder what was there...\"", "\"Copak tam asi bylo...?\"")

N("Mouse", GEN_F, "Myška", "Myška", "Myšku", "Myškou")

S("You hear a distant squeak!", "V dálce slyšíš zapištění!")
S("%The1 squeaks in a confused way.", "%1 zmateně piští.")
S("%The1 squeaks gratefully!", "%1 vděčně piští!")
S("%The1 squeaks hopelessly.", "%1 zdrceně piští.")
S("%The1 squeaks in despair.", "%1 zoufale piští.")
S("%The1 squeaks sadly.", "%1 smutně piští.")
S("%The1 squeaks with hope!", "%1 nadějně piští!")
S("%The1 squeaks happily!", "%1 šťastně piští!")
S("%The1 squeaks excitedly!", "%1 vzrušeně piští!")

N("giant rug", GEN_O, "obří koberec", "obří koberce", "obří koberec", "obřím kobercem")

S("This is the biggest Hypersian Rug you have ever seen! "
  "Unfortunately, it is too large to take it as a trophy.",
  "Tohle je ten největší Hyperský koberec, jaký jsi kdy vidě%l0!  "
  "Bohužel je ale příliš velký na to, aby sis ho moh%l0 vzít jako trofej.")

N("Prince", GEN_M, "Princ", "Princové", "Prince", "Princem")
N("Princess", GEN_F, "Princezna", "Princezny", "Princeznu", "Princeznou")

S("%1 takes %his1 revenge on %the2!", "%1 se mstí!")
S("\"That should teach you to take me seriously!\"",
  "\"To by tě mělo naučit brát mě vážně!\"")

S("%The1 kisses you, and begs you to bring %him1 away from here.",
  "%1 tě políbi%l1 a prosí tě, abys ji odsud odved%l0.")

S("\"I want my revenge. Stun a guard and leave him for me!\"",
  "Chci se pomstít. Omrač nějakého strážce a nech mi ho!\"")

S("\"That felt great. Thanks!\"", "\"To byl skvělý pocit. Děkuji!\"")

S("\"Bring me out of here please!\"", "Prosím tě, odveď mě odsud!\"")

S("%The1 kisses you, and thanks you for saving %him1.",
  "%1 tě políbi%l1 a děkuje ti, že jsi %ho1 zachránil.")

S("\"I have been trained to fight with a Hypersian scimitar, you know?\"",
  "Jsem vycvičen%ý1 v boji s hyperskou šavlí, víš?\"")

S("\"I would love to come to your world with you!\"",
  "Chtě%l1 bych se s tebou vrátit do tvého světa!\"")

S("\"Straight lines stay close to each other forever, this is so romantic!\"",
  "\"Přímky, které navěky zůstávají blízko sebe, to je tak romantické!\"")

S("\"Maps... Just like the world, but smaller... how is that even possible?!\"",
  "\"Mapy... stejné jako svět, ale menší... jak je to vůbec možné?!\"")

Orb("Love", "Lásky")
S("Love takes time, but it heals all wounds, and transcends time and space.\n\n"
  "The Orb of Love is worth 30$$$, if you end the game with it.\n",

  "Láska chce čas, ale zahojí všechny rány a překoná čas i prostor.\n\n"
  "Sféra lásky má cenu 30$$$, pokud ji máš u sebe na konci hry.\n")

// Princess Challenge
// ------------------

S("%1 Challenge", "Mise: %1")
S("Save %the1 first to unlock this challenge!", "Pro aktivaci této mise musíš nejprve zachránit %a1!")
S("Welcome to %the1 Challenge!", "Vítej v Misi: %1!")
S("The more Hypersian Rugs you collect, the harder it is.", "Čím víc Hyperských koberců sebereš, tím obtížnější bude.")
S("Follow the Mouse and escape with %the1!", "Následuj Myšku a uteč s %abl1!")
S("Hardness frozen at %1.", "Obtížnost nastavená na: %1.")
S("Congratulations! Your score is %1.", "Gratulujeme! Tvé skóre je %1.")

S("u = undo",     "u = zpět")
S("f = flip %1",     "f = překlopit %1")

S("Failed to save map to %1", "Uložení mapy do %1 se nezdařilo")
S("Failed to load map from %1", "Nahrání mapy z %1 se nezdařilo")
S("save whom", "koho zachraňuješ")
S("other", "další") // other keys in the main menu

// VERSION 7.4
// ===========

// missing texts, refactored things, and rule changes
// --------------------------------------------------

S("%The1 activates her Flash spell!", "%1 aktivova%l1 kouzlo Záblesk!")

N("Fire", GEN_O, "Oheň", "Ohně", "Oheň", "Ohněm")
S("This cell is on fire. Most beings and items cannot survive.",
  "Toto políčko hoří. Většina tvorů a předmětů zde nepřežije.")

N("Eternal Fire", GEN_O, "Věčný Oheň", "Věčné Ohně", "Věčný Oheň", "Věčným Ohněm")
S("This fire never burns out.", "Tento oheň nikdy neuhasne.")

S("Some rubble.", "Sutiny.")

S("The Orb loses its power as it leaves the Land of Power!",
  "Sféra opustila Kraj Moci a ztratila svou sílu!")

S("%The1 fills the hole!", "%1 vyplni%l1 díru!")

N("Tentacle+Ghost", GEN_M, "Chapadlo+Duch", "Chapadla+Duchové", "Chapadlo+Ducha", "Chapadlem+Duchem")

// Land Overview
// -------------

S("world overview", "přehled krajů")
S("or 'o' to see the world overview", "nebo stiskni 'o' pro přehled krajů")

S("forbidden to find in %the1", "zakázaná %abl1")
S("too dangerous to use in %the1", "příliš nebezpečná %abl1")
S("useless in %the1", "k ničemu %abl1")
S("only native Orbs allowed in %the1", "%abl1 jsou povoleny pouze náležící Sféry")
S("this Orb is never unlocked globally (only hubs)", "tato Sféra není nikdy odemčena globálně (pouze v centrálních krajích)")
S("collect 25 %2 to unlock it in %the1", "získej 25x %2, abys ji odemkl %abl1")
S("collect 3 %2 to unlock it in %the1", "získej 3x %2, abys ji odemkl %abl1")
S("native to %the1 (collect 10 %2)", "standardní %abl1 (získej 10x %2)")
S("native to %the1 (collect 1 %2)", "standardní %abl1 (získej 1x %2)")
S("secondary in %the1 (collect 10 %3, or 25 %2)", "sekundární %abl1 (získej 10x %3 nebo 25x %2)")
S("the native Orb of %the1", "standardní Sféra %abl1")
S("this Orb appears on floors and is used by witches", "tato Sféra se objevuje na zemi a využívají ji čarodějnice czarownice")
S("a prized Orb, it appears only in cabinets", "cenná Sféra, která se objevuje pouze ve vitrínách")
S("this Orb never appears in %the1", "tato Sféra, se %abl1 nikdy neobjevuje")
S("Hub Land: orbs appear here if unlocked in their native land", "Centrální kraj: Sféry se zde objevují, pokud jsi je odemkl v jejich rodné krajině")
S("kill a monster, or collect 25 %2", "zabij netvora nebo získej 25x %2")

S("Always available.\n", "Tento kraj je vždy k dispozici.\n")
S("Treasure required: %1 $$$.\n", "Potřebuješ %1 $$$.\n")
S("Accessible only from %the1.\n", "Tento kraj je dostupný pouze skrz %a1.\n")
S("Accessible only from %the1 (until finished).\n", "Tento kraj je dostupný pouze skrz %a1 (dokud ho nedohraješ).\n")

S("Accessible only from %the1 or %the2.\n", "Tento kraj je dostupný pouze skrz %a1 nebo %a2.\n")

S("Kills required: %1.\n", "Potřebuješ zabít %1 nepřátel.\n")
S("Finished lands required: %1 (collect %2 treasure)\n",
  "Potřebuješ dokončit %1 krajů (získat v nich %2 pokladů)\n")

S("Treasure required: %1 x %2.\n", "Potřebuješ %1 x %2.\n")

S("Alternatively: kill a %1.\n", "Alternativně: zabij %a1.\n")

S("Kills required: any Elemental (Living Fjord/Dead Caves).\n",
  "Potřebuješ zabít libovolného Elementála (Živoucí fjord/Mrtvé jeskyně).")

S("Variants of %the1 are always available in the Random Pattern Mode.",
  "Varianty kraje '%1' jsou vždy k dispozici v Módu náhodných vzorů.")

S("Variants of %the1 are available in the Random Pattern Mode after "
  "getting a highscore of at least 10 %2.",
  "Varianty kraje '%1' jsou dostupné v Módu náhodných vzorů po získání nejméně 10x %2.")

S(" kills: %1", " zabito nepřátel: %1")
S(" $$$: %1", " $$$: %1")
S(" Hyperstone: %1/%2", " Hyperkamy: %1/%2")
S(" Hell: %1/%2", " Peklo: %1/%2")

// improved editor
// ---------------

S("vector graphics editor -- press F1 for help", "editor vektorové grafiki -- nápovědu zobrazíš stiskem F1")
S("pics to save/load:", "obrázky k uložení/nahrání:")
S("level to save/load:", "mapa k uložení/nahrání:")

S("F4 = extension", "F4 = rozšíření")
S("Enter = back", "Enter = zpět")
S("SPACE = map/graphics", "SPACE = mapa/grafika")

S("Emerald Pattern", "Smaragdový vzor")
S("Palace Pattern", "Palácový vzor")
S("Zebra Pattern", "Vzor 'Zebra'")
S("rotational symmetry", "rotační symetrie")
S("symmetry 0-1", "symetrie 0-1")
S("symmetry 0-2", "symetrie 0-2")
S("symmetry 0-3", "symetrie 0-3")

S("display pattern codes (full)", "zobraz kódy vzoru (úplné)")
S("display pattern codes (simplified)", "zobraz kódy vzoru (zjednodušené)")
S("display only hexagons", "zobraz pouze šestiúhelníky")
S("display only heptagons", "zobraz pouze sedmiúhelníky")
S("predesigned patterns", "předem vytvořené vzory")

S("Gameboard", "Herní desky")
S("random colors", "náhodné barvy")
S("emerald pattern", "smaragdový vzor")
S("four elements", "čtyři živly")
S("eight domains", "osm domén")
S("zebra pattern", "vzor 'zebra'")
S("three stripes", "tři pásy")
S("random black-and-white", "náhodný černobílý vzor")
S("p = paint", "p = obarvit")
S("r = regular", "r = vzory")

S(
   "In this mode you can draw your own player characters, "
   "floors, monsters, and items. Press 'e' while hovering over "
   "an object to edit it. Start drawing shapes with 'n', and "
   "add extra vertices with 'a'. Press 0-9 to draw symmetric "
   "pictures easily. More complex pictures can "
   "be created by using several layers ('l'). See the edges of "
   "the screen for more keys.",

   "V tomto módu si můžeš nakreslit vlastní postavy, podlahu, netvory "
   "a předměty. Objekt můžeš editovat tak, že na něj najedeš myší a "
   "stiskneš 'e'. 'n' začne kreslit tvar a 'a' přidává nové vrcholy. "
   "Klávesy 0-9 umožňují snadnou kresbu symetrických obrázků. Složitější "
   "obrázky je možné vytvořit s použitím ně kolika vrstev ('l'). Další "
   "ovládací klávesy můžeš najít na okrajích obrazovky.")

S(
   "Press 'r' to choose a regular pattern. When a pattern is on, "
   "editing a cell automatically edits all cells which are "
   "equivalent according to this pattern. You can choose from "
   "several patterns, and choose which symmetries matter "
   "for equivalence. Also, you can press Space to switch between "
   "the map and graphics editor quickly -- note that editing floors "
   "with the graphics editor also adheres to the pattern.",

   "Stiskem klávesy 'r' můžeš vybrat pravidelný vzor. Když je vzor "
   "zapnutý, editace buňky automaticky ovlivní i všechny ostatní buňky, "
   "které jsou podle tohoto vzoru ekvivalentní. Můžeš si vybrat "
   "z několika možných vzorů, a také to, které symetrie budou důležité "
   "pro ekvivalenci. Také můžeš rychle přepínat mezerníkem mezi "
   "editorem mapy a grafiky -- i editování podlahy grafickým editorem "
   "se řídí pravidly vzoru.")

S("monster", "netvor")
S("item", "předmět")
S("floor", "podlaha")
S("hexagonal", "šestiúhelníky")
S("heptagonal", "sedmiúhelníky")
S("floor/pattern", "podlaha/vzor")

S("l = layers: %1", "l = vrstvy: %1")
S("1-9 = rotations: %1", "1-9 = rotace: %1")
S("0 = symmetry", "0 = symetrie")
S("0 = asymmetry", "0 = asymetrie")
S("%1 vertices", "vrcholy: %1")
S("a = add v", "a = přidej v")
S("m = move v", "m = přesuň v")
S("d = delete v", "d = vymaž v")
S("c = readd v", "c = znovu přidej v")
S("b = reorder", "b = změň uspořádání")
S("t = shift", "t = přesuň")
S("y = spin", "y = otoč")

S("'n' to start", "n = start")
S("z = zoom in", "z = přiblížení")
S("o = zoom out", "o = oddálení")
S("g = grid", "g = síť")
S("e = edit this", "e = editace")

S("x: %1", "x: %1")
S("y: %1", "y: %1")
S("z: %1", "z: %1")
S("r: %1", "r: %1")
S("ϕ: %1°", "ϕ: %1°")

S("A fake Land with colored floors.",
  "Falešný kraj s barevnou podlahou.")

S("random pattern mode", "mód náhodných vzorů")

// Ivory Tower
// -----------

N("Ivory Tower", GEN_F, "Věž ze slonové kosti", "Věže ze slonové kosti", "Věž ze slonové kosti", "ve Věži ze slonové kosti")

S(
  "Powerful wizards claimed this part of the world, to perform their magical "
  "experiments in peace and solitude. They have changed the direction of gravity, "
  "to make it even harder for intruders to reach them.\n\n"
  "Gravity works as follows: cells are unstable if they are empty, and there is "
  "no cell immediately below them which contains a wall. It is impossible to move "
  "from one unstable cell to another, except if moving down.",

  "Tuto část světa si pro sebe zabrali mocní mágové, aby zde mohli v klidu a osamění "
  "provádět své magické experimenty. Změnili směr gravitace, aby bylo pro vetřelce "
  "ještě obtížnější se k nim dostat.\n\n"
  "Gravitace funguje následovně: prázdné políčko, pod kterým není žádné políčko se zdí, "
  "je 'nestabilní'. Není dovoleno pohybovat se z jednoho nestabilního políčka do druhého, "
  "leda směrem dolů."
  )

N("Servant", GEN_M, "Služebník", "Služebníci", "Služebníka", "Služebníkem")
S("A simple servant of the master of the Ivory Tower.",
  "Prostý služebník pána Věže ze slonové kosti.")

N("Gargoyle", GEN_O, "Chrlič", "Chrliče", "Chrlič", "Chrličem")
N("stone gargoyle", GEN_O, "kamenný chrlič", "kamenné chrliče", "kamenného chrliče", "kamenným chrličem")
N("stone gargoyle floor", GEN_F, "podlaha z chrliče", "podlahy z chrličů", "podlahu z chrliče", "podlahou z chrliče")
N("stone gargoyle bridge", GEN_O, "most z chrliče", "mosty z chrličů", "most z chrliče", "mostem z chrliče")

S(
  "A being made of stone, who likes high buildings. It becomes normal stone when "
  "killed, but only if next to something stable -- otherwise it falls.",
  "Kamenná bytost, která má ráda vysoké budovy. Po smrti se promění v obyčejný kámen, "
  "ale pouze tehdy, když je vedle něčeho stabilního -- jinak spadne.")

N("Ivory Figurine", GEN_F, "Figurka", "Figurky", "Figurku", "Figurkou")

S("A beautiful figurine, made of ivory. Figurines close to the base of the Tower "
  "tend to disappear after you have collected many of them.",
  "Nádherná figurka ze slonové kosti. Figurky poblíž základů Věže mají tendenci "
  "mizet, pokud jich získáš hodně.")

N("Platform", GEN_F, "Plošinka", "Plošinky", "Plošinku", "Plošinkou")
N("platform", GEN_F, "plošinka", "plošinky", "plošinku", "plošinkou")
S("You can stand here.", "Na této plošince můžeš stát.")

N("ladder", GEN_O, "žebřík", "žebříky", "žebřík", "žebříkem")
S("You can use this ladder to climb the Tower.",
  "Po tomto žebříku můžeš lézt na Věž.")

Orb("Matter", "Hmoty")

S("This Orb allows one to temporarily create impassable matter, either to block paths or "
  "to build bridges across chasms and waters.",
  "Tato Sféra ti umožňuje dočasně vytvářet neprostupnou hmotu, kterou můžeš zatarasit "
  "cestu nebo využít jako most přes propasti nebo přes vodu.")

N("temporary wall", GEN_F, "dočasná zeď", "dočasné zdi", "dočasnou zeď", "dočasnou zdí")
N("temporary floor", GEN_F, "dočasná podlaha", "dočasné podlahy", "dočasnou podlahu", "dočasnou podlahou")
N("temporary bridge", GEN_O, "dočasný most", "dočasné mosty", "dočasný most", "dočasným mostem")

S("This structure will disappear after some time.", "Tato struktura za nějaký čas zmizí.")

S("Nothing to stand on here!", "Není tu na čem stát!")
S("Gravity does not allow this!", "Tohle ti gravitace nedovolí!")

// Elemental Planes
// ----------------

N("Elemental Planes", GEN_F, "Elementální roviny", "Elementální roviny", "Elementální roviny", "v Elementálních rovinách")
N("Plane of Fire", GEN_F, "Rovina Ohně", "Roviny Ohně", "Rovinu Ohně", "v Rovině Ohně")
N("Plane of Earth", GEN_F, "Rovina Země", "Roviny Země", "Rovinu Země", "v Rovině Země")
N("Plane of Air", GEN_F, "Rovina Vzduchu", "Roviny Vzduchu", "Rovinu Vzduchu", "v Rovině Vzduchu")
N("Plane of Water", GEN_F, "Rovina Vody", "Roviny Vody", "Rovinu Vody", "v Rovině Vody")

S("The Elemental Planes are divided into four subzones: Planes of Fire, Water, Air, and Earth. "
  "You need to collect a Shard from each Plane to construct an Elemental Gem. "
  "It is dangerous to collect too many Shards of the same type without constructing a Gem.",

  "Elementální roviny se dělí na čtyři subzóny: Rovinu Ohně, Vody, Vzduchu a Země. "
  "Musíš v každé z Rovin najít Úlomek, ze kterých pak vytvoříš Elementální drahokam. "
  "Pozor, je nebezpečné mít příliš mnoho Úlomků stejného typu a nepoužít je k sestavení Drahokamu!"
  )

N("Fire Elemental", GEN_M, "Elementál Ohně", "Elementálové Ohně", "Elementála Ohně", "Elementálem Ohně")

S("This monster leaves a trail of fire behind.",
  "Tento netvor za sebou zanechává ohnivou stopu.")

N("Air Elemental", GEN_M, "Elementál Vzduchu", "Elementálové Vzduchu", "Elementála Vzduchu", "Elementálem Vzduchu")

S("An Air Elemental looks like a live tornado. Once you are three (or less) cells from it, "
  "it is impossible to move closer, due to strong winds. You can stand and wait, though. "
  "This also affects most monsters.",

  "Elementál Vzduchu vypadá jako oživlé tornádo. Pokud jsi od něj tři nebo méně políček, "
  "silný vítr ti brání v tom, aby ses k němu přiblížil. Můžeš ovšem stát a čekat. "
  "Tento efekt také ovlivňuje většinu netvorů.")

N("Fire Shard", GEN_O, "Úlomek Ohně", "Úlomky Ohně", "Úlomek Ohně", "Úlomkem Ohně")
N("Air Shard", GEN_O, "Úlomek Vzduchu", "Úlomky Vzduchu", "Úlomek Vzduchu", "Úlomkem Vzduchu")
N("Water Shard", GEN_O, "Úlomek Vody", "Úlomky Vody", "Úlomek Vody", "Úlomkem Vody")
N("Earth Shard", GEN_O, "Úlomek Země", "Úlomky Země", "Úlomek Země", "Úlomkem Země")
N("Elemental Gem", GEN_O, "Elementální drahokam", "Elementální drahokamy", "Elementální drahokam", "Elementálním drahokamem")

Orb("Summoning", "Vyvolání")

S(
    "This orb allows you to summon monsters. Usually, they are either Elementals or "
    "native creatures. While the monsters do not like being summoned, and will "
    "attack you once they recover from summoning sickness, such summoning "
    "often has its uses.",
  "Tato Sféra ti umožňuje vyvolávat netvory. Obvykle se jedná buď o Elementály nebo "
  "o domorodé obyvatele kraje, kde ji použiješ. Netvoři nejsou vyvoláním nijak nadšeni "
  "a poté, co se zorientují, na tebe zaútočí, ale i tak má vyvolávání často své využití."
  )

N("limestone wall", GEN_F, "vápencová stěna", "vápencové stěny", "vápencovou stěnu", "vápencovou stěnou")

S("Simply a wall. Mostly.", "Po prostu ściana. W zasadzie.")

// Zebra
// -----

N("Zebra", GEN_F, "Zebra", "Zebry", "Zebru", "v Zebře")
S("Everything in this Land has black and white stripes.",
  "Všechno v tomto kraji má černé a bílé pruhy.")

N("Striped Dog", GEN_M, "Pruhovaný pes", "Pruhovaní psi", "Pruhovaného psa", "Pruhovaným psem")
S("A predator native to the Zebra.", "Predátor pocházející ze Zebry.")

N("Onyx", GEN_O, "Onyx", "Onyxy", "Onyx", "Onyxem")
S("A black gem with white stripes. It is beautiful.", "Černý drahokam s bílými pásky. Velice krásný.")

// Crossroads III
// --------------

N("Crossroads III", GEN_F, "Křižovatka III", "Křižovatky III", "Křižovatku III", "na Křižovatc III")

S("An alternate layout of the Crossroads. Great Walls cross here at right angles.",
  "Alternativní uspořádání Křižovatky. Velké zdi se tu protínají v pravých úhlech.")

// Steam achievements
// ------------------

/*
		"NEW_ACHIEVEMENT_5_12_NAME"	"Došel jsi do Věže ze slonové kosti"
		"NEW_ACHIEVEMENT_5_12_DESC"	"Najdi a získej Figurku."
		"NEW_ACHIEVEMENT_5_13_NAME"	"Služebník"
		"NEW_ACHIEVEMENT_5_13_DESC"	"Získej 10 Figurek."
		"NEW_ACHIEVEMENT_5_14_NAME"	"Chrlič"
		"NEW_ACHIEVEMENT_5_14_DESC"	"Získej 25 Figurek."
		"NEW_ACHIEVEMENT_5_15_NAME"	"Pán Věže"
		"NEW_ACHIEVEMENT_5_15_DESC"	"Získej 50 Figurek."
		"NEW_ACHIEVEMENT_5_16_NAME"	"Došel jsi do Zebry"
		"NEW_ACHIEVEMENT_5_16_DESC"	"Najdi a získej Onyx."
		"NEW_ACHIEVEMENT_5_17_NAME"	"Pruhovaný pes"
		"NEW_ACHIEVEMENT_5_17_DESC"	"Získej 10 Onyxů."
		"NEW_ACHIEVEMENT_5_18_NAME"	"Skvrnitý pes"
		"NEW_ACHIEVEMENT_5_18_DESC"	"Získej 25 Onyxů."
		"NEW_ACHIEVEMENT_5_19_NAME"	"Kostkovaný pes"
		"NEW_ACHIEVEMENT_5_19_DESC"	"Získej 50 Onyxů."
		"NEW_ACHIEVEMENT_5_20_NAME"	"Elementalista"
		"NEW_ACHIEVEMENT_5_20_DESC"	"Sestav Elementální drahokam."
		"NEW_ACHIEVEMENT_5_21_NAME"	"Nižší elementál"
		"NEW_ACHIEVEMENT_5_21_DESC"	"Sestav 10 Elementálních drahokamů."
		"NEW_ACHIEVEMENT_5_22_NAME"	"Vyšší elementál"
		"NEW_ACHIEVEMENT_5_22_DESC"	"Sestav 25 Elementálních drahokamů."
		"NEW_ACHIEVEMENT_5_23_NAME"	"Pán elementů"
		"NEW_ACHIEVEMENT_5_23_DESC"	"Sestav 50 Elementálních drahokamů."
*/

S("Cannot create temporary matter on a monster!", "Dočasnou hmotu nelze vytvořit na netvorovi!")
S("Cannot create temporary matter on an item!", "Dočasnou hmotu nelze vytvořit na předmětu!")
S("Cannot create temporary matter here!", "Zde nelze vytvořit dočasnou hmotu!")
S("Cannot summon on a monster!", "Na políčko s netvorem nelze nic vyvolat!")
S("No summoning possible here!", "Sem se nic vyvolat nedá!")

S("You summon %the1!", "Vyvola%l1 jsi %a1!")

S("F4 = file", "F4 = soubor")

// VERSION 8.0
// ===========

S("The Air Elemental blows you away!", "Elementál Vzduchu tě odfoukl!")

// appended to cave troll description
S(" Additionally, all items around the killed Troll will be destroyed.",
  " Zabitím Šutrolla se navíc zničí všechny předměty kolem něj.")

// 'dead cave troll' and 'dead troll' are separated now

N("dead rock troll", GEN_M, "mrtvý Šutroll" ,"mrtví Šutrollové", "mrtvého Šutrolla", "mrtvým Šutrollem")

S("There are several species of trolls living in the hyperbolic world. "
  "Some of them leave this wall behind them when they die.",

  "V hyperbolickém světě žije několik druhů trollů. Někteří z nich "
  "se po smrti promění v tuto zeď.")

// paper model creator
// --------------------

S("paper model creator", "generátor papírových modelů")
S("synchronize net and map", "synchronizuj síť a mapu")
S("display the scope", "zobraz rozsah")
S("create the model", "vytvoř model")
S("design the net", "vytvoř síť")

S("The paper model created as papermodel-*.bmp", "Papírový model byl vytvořen jako papermodel-*.bmp")
S("Failed to load the file 'papermodeldata.txt'", "Soubor 'papermodeldata.txt' se nepodařilo nahrát")
S("Could not save the paper model data", "Nepodařilo se uložit data pro papírový model")

// pure tactics mode
// -----------------

S("pure tactics mode", "taktický mód")
S("Not available in the pure tactics mode!", "Toto není v taktickém módu k dispozici!")
S("\"The Knights of the Horocyclic Table salute you!\"", "\"Rytíři Horocyklického stolu tě zdraví!\"")
S("press 0 to leave this mode", "Pro opuštění tohoto módu stiskni klávesu '0'")
S("Collect %1x %2 to unlock", "Pro odemknutí získej %1x %2")

S(
        "In the pure tactics mode, you concentrate on a specific land. "
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

        "V taktickém módu se soustředíš pouze na jeden specifický kraj. Tvým cílem  "
        "je získání co nejvyššího skóre, bez použití jakýchkoli prvků "
        "jiných krajů. Potom si své skóre můžeš porovnat s přáteli!\n\n"

        "Abys v tomto módu odemkl určitý kraj, musíš být alespoň trochu dobrý "
        "ve standardní hře (získat 20 pokladů v daném kraji, 2 v případě Camelotu).\n\n"

        "Vzhledem k tomu, že získání vysokého skóre v některých krajích poněkud závisí "
        "na štěstí, budeš každý kraj hrát N-krát a tvé skóre bude založeno na N "
        "po sobě jdoucích hrách. Hodnota N se odvíjí od 'rychlosti' a náhodnosti "
        "daného kraje.\n\n"

        "V Karibiku budeš mít k dispozici Sféry Trnů, Éteru a Prostoru, pokud "
        "jsi někdy získal 25 pokladů v jejich rodných krajích.\n\n"

        "Generování pokladů je v tomto módu statické. Nezvyšuje se zabíjením netvorů.\n\n"

        "Hodně štěstí, a dobře se bavte!")

// Yendor Challenge
// ----------------

S("Yendor Challenge", "Mise: Yendor")
S("Collect 10 treasures in various lands to unlock the challenges there",
  "Tyto mise odemkneš získáním 10 pokladů v různých krajích")

S("easy", "snadná")
S("challenge", "obtížná")

S("Challenges do not get harder", "Obtížnost misí zůstává stále stejná")
S("Each challenge gets harder after each victory", "Obtížnost každé mise se zvýší, když ji splníš")

S(" (won!)", " (vítězství!)")
S(" (won at level %1!)", " (vítězství -- úroveň %1!)")
S("(locked)", "(uzamčeno)")

S(
    "There are many possible solutions to the Yendor Quest. In the Yendor "
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

    "Je mnoho různých způsobů, jak získat Yendorskou Sféru. V Misi: Yendor si "
    "jich vyzkoušíš celou řadu!\n\n"
    "Každá mise se odehrává v určitém kraji a ty musíš využít to, co tam máš "
    "k dispozici.\n\n"

    "Abys mohl hrát Misi: Yendor, musíš získat Yendorskou Sféru v normální hře; "
    "pro aktivaci konkrétních misí musíš také (alespoň jednou) získat 10 pokladů "
    "v jednom či dvou krajích.\n\n"

    "Po splnění každé mise se o ni můžeš pokusit znovu, na vyšší obtížnosti.\n\n"

    "Všechna řešení v Misi: Yendor fungují i v normální hře. Průchody do jiných krajin a "
    "(někdy) některé jejich prvky jsou však v Misi: Yendor vypnuty, takže musíte použít "
    "metodu, jaká se od vás očekává. Kromě toho se při hledání Yendorské sféry poněkud "
    "mění pravidla pro generování Paláce a Minového pole, aby byla mise více vyvážená, "
    "(tyto změny však platí i během normálního hledání Yendorské Sféry).\n\n"
    )

S("Unlock this challenge by getting the Orb of Yendor!",
  "Tuto misi odemkneš získáním Yendorské sféry!")

S("Collect 10 treasures in various lands to unlock the challenges there",
  "Tyto mise odemkneš získáním 10 pokladů v různých krajích")

// Wild West
// ---------

N("Wild West", GEN_O, "Divoký západ", "Divoké západy", "Divoký západ", "na Divokém západě")
N("Outlaw", GEN_M, "Psanec" ,"Psanci", "Psance", "Psancem")
N("Bounty", GEN_F, "Odměna", "Odměny", "Odměnu", "Odměnou")
N("saloon wall", GEN_F, "zeď saloonu", "zdi saloonu", "zeď saloonu", "zdí saloonu")
N("Revolver", GEN_O, "Revolver", "Revolvery", "Revolver", "Revolverem")

S("%The1 shoots you!", "%1 tě střeli%l!")
S("You shoot %the1!", "Zastřeli%l0 jsi %a1!")

S(
  "Take a revolver, kill outlaws, collect bounties.\n\n"
  "Note: since this land is anachronistic, it is not available in normal game. "
  "It is only available in special modes.",

  "Vezmi si revolver, zabíjej psance, sbírej odměny.\n\n"
  "Poznámka: tento kraj je anachronistický, a proto není k dispozici v normální hře. "
  "Je dostupný pouze ve speciálních módech.")

// Land of Storms
// --------------

S(
  "Whenever after your move there is a connection between a charged and a "
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

  "Po každém tvém tahu se zkontroluje, zda nevzniklo spojení mezi nějakým "
  "nabitým a uzemněným políčkem; pokud ano, dojde ke zkratu. Všechna políčka "
  "na jakékoli dráze spojující nabité a uzemněné políčko (pokud se na ní žádné "
  "políčko nevyskytuje dvakrát a pokud na ní nejsou dvě uzemněná/nabitá políčka "
  "za sebou, dostanou zásah.\n\n"

  "Pískovcové zdi a většina bytostí je vodivá. Velké zdi jsou izolanty, ale kraje "
  "za nimi se počítají jako uzemněné.\n\n"

  "Zdejší poklad, Fulgurit, vznikne, když se ti podaří zkratovat pískovcovou zeď "
  "nebo Bohatou bestii.\n\n"

  "Trollové se po smrti promění ve vodivé kameny a Kovové a Bohaté bestie je možné "
  "zabít pouze elektřinou -- tvé útoky je pouze omráčí nebo, pokud už omráčené "
  "jsou, odstrčí.")

N("Land of Storms", GEN_F, "Kraj Bouří", "Kraje Bouří", "Kraj Bouří", "v Kraji Bouří")
N("charged wall", GEN_F, "nabitá zeď", "nabité zdi", "nabitou zeď", "nabitou zdí")
N("grounded wall", GEN_F, "uzemněná zeď", "uzemněné zdi", "uzemněnou zeď", "uzemněnou zdí")
N("metal wall", GEN_F, "kovové zeď", "kovové zdi", "kovovou zeď", "kovovou zdí")
N("sandstone wall", GEN_F, "pískovcová zeď", "pískovcové zdi", "pískovcovou zeď", "pískovcovou zdí")
N("Storm Troll", GEN_M, "Bouřkotroll", "Bouřkotrollové", "Bouřkotrolla", "Bouřkotrollem")
N("Metal Beast", GEN_F, "Kovová bestie", "Kovové bestie", "Kovovou bestii", "Kovovou bestií")
N("Fulgurite", GEN_O, "Fulgurit", "Fulgurity", "Fulgurit", "Fulguritem")

N("Rich Metal Beast", GEN_F, "Bohatá bestie", "Bohaté bestie", "Bohatou bestii", "Bohatou bestií")
N("electric discharge", GEN_O, "výboj", "výboje", "výboj", "výbojem")

S("There is a flash of thunder!", "Zablesklo se!")

Orb("Stunning", "Omráčení")

S("This Orb allows you to target monsters to stun them. "
  "10 charges are used to stun for 5 turns. Does not "
  "work against multi-tile monsters.",

  "Tato Sféra ti umožňuje zacílit a omráčit netvory. "
  "Každé omráčení trvá 5 kol a stojí 10 nábojů. "
  "Tato Sféra nepůsobí na netvory, kteří zabírají více "
  "než jedno políčko."
  )

S("You stun %the1!", "Omráči%l0 jsi %a1!")

// Overgrown Woods
// ---------------

Orb("Luck", "Štěstí")

S("This Orb allows you to find new lands more easily. "
  "Lands where you have already collected less treasure, "
  "and especially the Crossroads, are more likely to "
  "spawn while you have this. Additionally, Orbs of Safety "
  "are more likely to spawn in the Whirlpool.",

  "Tato Sféra ti umožňuje snadněji najít nové kraje. "
  "Dokud ji máš, budou se s větší pravděpodobností generovat "
  "kraje, ve kterých jsi získal méně pokladů, zvláště pak Křižovatky "
  "Kromě toho se ve Víru bude generovat více Sfér Bezpečí."
  )

N("Overgrown Woods", GEN_O, "Prales", "Pralesy", "Prales", "v Pralese")
N("Mutant Ivy", GEN_O, "Mutantní břečťan", "Mutantní břečťany", "Mutantní břečťan", "Mutantním břečťanem")
N("Mutant Sapling", GEN_O, "Mutantní semenáček", "Mutantní semenáčky", "Mutantní semenáček", "Mutantním semenáčkem")
N("Forest Troll", GEN_M, "Lestroll", "Lestrollové", "Lestrolla", "Lestrollem")

S("Forest Trolls create an impassable wall when they die.",
  "Lesní trollové se po smrti promění v neprostupnou zeď.")

S(
  "The Overgrown Woods are filled with mutant ivies! These plants "
  "grow very fast. Each leaf, after being grown, can grow itself "
  "on the next turn. However, each part is only able to grow "
  "once in 16 turns. Outside of the Overgrown Woods, the Mutant Ivy "
  "may grow only on hexagonal cells.\n\n"
  "Maybe such fast growing plants could help you solve the problem "
  "of hunger in your world? Kill the Mutant Ivies to collect Mutant Saplings.",

  "Prales je plný mutantních břečťanů! Tyto rostliny velice rychle rostou. "
  "Jakmile nějaký list vyroste, hned v následujícím kole může sám růst. "
  "Každá část však může vyrůst pouze jednou za 16 kol. Mimo Prales může "
  "Mutantní břečťan růst pouze na šestiúhelníkových políčkách.\n\n"
  "Rostliny s takovou rychlostí růstu by ti možná mohly pomoci vyřešit problém "
  "s hladem ve tvém světě. Zabíjej Mutantní břečťany, abys získal Mutantní semenáčky."
  )

S("Trees in this forest can be chopped down. Big trees take two turns to chop down.",
  "Stromy v tomto lese je možné kácet. Kácení velkých stromů trvá dvě kola."
  )

S("Your power is drained by %the1!", "%1 vysává tvou sílu!")

S("Wow! That was close.", "Uf! To bylo o chlup!")

S("Collect four different Elemental Shards!", "Získej čtyři různé Elementální úlomky!")
S("Unbalanced shards in your inventory are dangerous.",
  "Nevyrovnané Úlomky ve tvém inventáři jsou nebezpečné.")

S("Alternatively: kill a %1 in %the2.\n", "Alternativně: zabij %a1 %abl2.\n")

S("player 3 X", "hráč 3 X")
S("player 3 Y", "hráč 3 Y")
S("player 3 go", "hráč 3 pohyb")
S("player 3 spin", "hráč 3 otáčení")
S("player 4 X", "hráč 4 X")
S("player 4 Y", "hráč 4 Y")
S("player 4 go", "hráč 4 pohyb")
S("player 4 spin", "hráč 4 otáčení")

S("configure player 3", "konfigurace hráče 3")
S("configure player 4", "konfigurace hráče 4")

// VERSION 8.1
// ============

// extra config
S("Prince", "Princ")
S("Princess", "Princezna")
S("cat" ,"kočka")
S("dog", "pes")
S("dress color II", "barva šatů II")
S("character", "postava")

//missing texts
S("%The1 hits %the2.", "%1 udeři%l1 %a2.")
S("%The1 is destroyed by the Flash.", "Záblesk zničil %a1.")

// improved kill descriptions
S("parts destroyed: %1", "zničené části: %1")
S("friends killed: %1", "zabití kamarádi: %1")
S("monsters destroyed: %1", "zničení nepřátelé: %1")
S("friends destroyed: %1", "zničení kamarádi: %1")
S("\n\nThis is a part of a monster. It does not count for your total kills.",
  "\n\nToto je část netvora. Nepočítá se do celkového počtu zabitých netvorů.")
S("\n\nThis is a friendly being. It does not count for your total kills.",
  "\n\nToto je přátelská bytost. Nepočítá se do celkového počtu zabitých netvorů.")

// Overgrown Clearing
// ------------------

S("A clearing in the Overgrown Woods. Obviously, this gives "
  "the Mutant Ivies an infinite space to grow...\n\n"
  "Mutant Fruits rot if they are not adjacent to a Mutant Ivy.",

  "Mýtina v Pralese. Mutantní břečťan tu samozřejmě má nekonečný prostor k růstu...\n\n"
  "Mutantní ovoce shnije, pokud se nenachází vedle Mutantního břečťanu.")

N("Clearing", GEN_F, "Mýtina", "Mýtiny", "Mítinu", "na Mýtině")

N("Mutant Fruit", GEN_N, "Mutantní ovoce", "Mutantní ovoce", "Mutantní ovoce", "Mutantní ovoce")

N("Giant Fox", GEN_F, "Obří liška", "Obří lišky", "Obří lišku", "Obří liškou")

S(
    "What is freedom for you? A situation when you can walk wherever you want? "
    "Or a situation when you do not have to work, since you have as much tasty food "
    "as you want?\n\n"
    "Well, this creature has chosen the second option. It won't be happy "
    "if you destroy its prison.\n",

    "Co je pro tebe svoboda? To, že si můžeš chodit, kam chceš? "
    "Nebo to, že nemusíš pracovat, protože máš tolik dobrého jídla, kolik chceš?\n\n"
    "Inu, tento tvor si vybral tu druhou možnost. Nebude rád, když zničíš jeho vězení.\n"
    )

Orb("Freedom", "Svobody")

S(
    "This orb is activated if you are unable to escape (radius 4) "
    "without making illegal moves or "
    "going through cells which are currently adjacent to enemy monsters. "
    "Most game over situations are covered by this, but generally, "
    "this orb is oversensitive...\n\n"
    "When activated, it creates a Flash effect of radius 5.",

    "Tato Sféra se aktivuje, pokud nemůžeš utéct (do vzdálenosti 4 políček), "
    "aniž bys provedl ilegální pohyby nebo prošel políčky, která momentálně "
    "sousedí s nepřátelskými netvory. To pokrývá většinu situací 'konec hry', ale "
    "obvykle tato Sféra reaguje poněkud přehnaně...\n\n"
    "Při aktivaci vytvoří efekt Záblesku s poloměrem 5 políček."
  )

S("Your %1 activates!", "Tvoje %1 se aktivovala!")

// Haunted Woods
// -------------

S("You become a bit nervous...", "Začínáš být trošku nervózní...")
S("Better not to let your greed make you stray from your path.",
  "Raději nedopusť, aby tě tvá chamtivost svedla z cesty!")

N("Haunted Woods", GEN_O, "Strašidelný hvozd", "Strašidelné hvozdy", "Strašidelný hvozd", "ve Strašidelném hvozdu")

S(
  "A dark forest filled with ghosts and graves. But there is also treasure hidden "
  "deep within... But don't let greed make you stray from your path, as "
  "you can get lost!\n\n"
  "The Haunted Woods are bounded by a single equidistant curve. It is not a circle or horocycle.\n\n",

  "Temný hvozd plný duchů a hrobů. Ale hluboko uvnitř se skrývá poklad... "
  "Nedopusť však, aby tě tvá chamtivost svedla z cesty, protože tu můžeš zabloudit!\n\n"
  "Strašidelný hvozd je ohraničený jednou ekvidistantou. Není to kruh ani horocyklus.\n\n")


N("Black Lotus", GEN_O, "Černý lotos", "Černé lotosy", "Černý lotos", "Černým lotosem")

S(
    "This beautiful flower is greatly prized by wizards, as it allows them to cast powerful magical spells "
    "without preparation.\n",

    "Tento nádherný květ si velmi cení čarodějové, neboť jim umožňuje sesílat "
    "silná kouzla bez jakékoli přípravy.\n"
    )

Orb("Undeath", "Nemrtvých")

S(
    "Monsters slain by you in melee are turned into friendly ghosts. "
    "Does not affect plants and friends.",

    "Netvoři, které zabiješ v boji na blízko, se promění v přátelské duchy. "
    "Nepůsobí na rostliny a přátelské netvory."
    )

N("Friendly Ghost", GEN_M, "Přátelský duch", "Přátelští duchové", "Přátelského ducha", "Přátelským duchem")

S("Friendly ghosts are friendly beings who can go through any obstacles. However, "
  "unlike most friends, they tend to fly away from you.",

  "Přátelští duchové jsou přátelské bytosti, kteří dokážou procházet jakýmikoli překážkami. "
  "Na rozdíl od většiny tvých kamarádů ale mají tendenci létat pryč od tebe."
   )

// Windy Plains
// ------------

N("Windy Plains", GEN_F, "Větrná pláň", "Větrné pláně", "Větrnou pláň", "na Větrné pláni")

N("fan", GEN_O, "Větrák", "Větráky", "Větrák", "Větrákem")

S(
  "Someone has put air fans in these plains, causing strong winds everywhere. "
  "You think that the purpose is to harness the magical power of Air Elementals, but "
  "you are not sure.\n\n"
  "All cells except fans are grouped into three colors according to a pattern. "
  "Wind blows counterclockwise around each group of cells of a single color. "
  "Cells which are blocked by walls, or at distance at most 2 from an Air Elemental, "
  "do not count for this.\n\n"
  "It is illegal to move in a direction which is closer to incoming wind than to "
  "outgoing wind. However, you can move two cells with the wind in a single turn, "
  "and so can the birds.",

  "Někdo umístil na tuto pláň větráky, díky kterým tu všude vanou silné větry. "
  "Myslíš si, že jejich účelem je zkrotit magickou moc Elementálů vzduchu, ale "
  "s jistotou to nevíš.\n\n"

  "Všechna políčka s výjimkou větráků jsou rozdělena na tři barvy podle určitého vzoru. "
  "Vítr vane proti směru hodinových ručiček kolem každé skupiny stejnobarevných políček. "
  "Políčka zablokovaná zdmi nebo políčka ve vzdálenosti 2 nebo méně od Elementála vzduchu "
  "se nepočítají.\n\n"
  "Není možné pohybovat se směrem, který je víc proti větru než po větru. Při pohybu po "
  "větru se však můžeš (stejně jako libovolný typ ptáka) pohnout o dvě políčka v jediném kole."
  )

N("White Dove Feather", GEN_N, "Holubičí pero", "Holubičí pera", "Holubičí pero", "Holubičím perem")
S("This feather is truly beautiful and strong.",
  "Pero bílé holubice. Opravdu nádherné a silné.")

N("Wind Crow", GEN_F, "Větrná vrána", "Větrné vrány", "Větrnou vránu", "Větrnou vránou")
S("A large bird who likes strong winds. Just as you, it can fly quickly in the wind.",
  "Welký pták, který má rád silný vítr. Dokáže v něm rychle létat, stejně jako ty."
  )

// Orb of Empathy replaces Orb of Air on the Beach
Orb("Empathy", "Empatie")
S(" %1", " %1")
S(", %1", ", %1")

S(  "This Orb lets your allies share your Orb powers.\n\n"
    "The following Orbs are affected:",

    "Tato Sféra ti umožňuje dělit se o moc tvých Sfér s kamarády.\n\n"
    "Ovlivňuje následující Sféry:")

N("strong wind", GEN_O, "silný vítr", "silné větry", "silný vítr", "silným větrem")

S(
    "In the Windy Plains, you can let the wind carry you, "
    "causing you to move two cells with the wind in a single turn. "
    "This cannot be done if you are standing at distance at most 2 "
    "from the Air Elemental, or if any of the three cells on the way "
    "has two wind directions.\n\n"
    "Press 't' or click the destination to activate.",

    "Na Větrné pláni se můžeš nechat unášet větrem "
    "a pohybovat se o dvě políčka v jednom kole. "
    "Toto není možné, pokud se nacházíš ve vzdálenosti 2 nebo méně od Elementála vzduchu "
    "nebo pokud jsou na kterémkoli ze tří políček, kterými bys prošel, "
    "dva směry větru.\n\n"
    "Pro unášení větrem stiskni klávesu 't' nebo klikni na cílové políčko."
  )

S("Return to the normal game", "Návrat do normální hry")
S("three players", "tři hráči")
S("four players", "čtyři hráči")
S("key Orb power", "moc sfery jednym klawiszem")

S("Orb power (target: mouse)", "Použití Sféry (zaměření myší)")
S("Orb power (target: facing)", "Použití Sféry (zaměření před postavou)")

S("You cannot go against the wind!", "Nemůžeš jít proti větru!")

S("You construct some Elemental Gems!", "Vytvoři%l0 jsi Elementální drahokamy!")

// VERSION 8.2
// ============

S("The ivy attacks %the1!", "Břečťan zaútočil na %a1!")

// heptagonal mode
// ---------------

S("heptagonal mode", "sedmiúhelníkový mód")

S("\n\n(For the heptagonal mode, the radius has been reduced to 2 for closing plates.)",
  "\n\n(V sedmiúhelníkovém módu mají zavírací desky dosah pouze 2.)")

// Hypersian Rug mode
// ------------------

S("hypersian rug mode", "mód hyperského koberce")

S("In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
  "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n",
  "V tomto módu můžete hrát HyperRogue na 3D modelu části hyperbolické roviny "
  "podobném tomu, co vytváří generátor papírových modelů nebo hyperbolické háčkování.\n\n")

S("This requires some OpenGL extensions and may crash or not work correctly -- enabling "
  "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
  "will make the rendering faster, but the surface will be rendered only once, so "
  "you won't be able to play a game on it.\n\n",
  "Je k tomu zapotřebí OpenGL a je možné, že v tomto módu hra spadne nebo nepoběží "
  "správně -- v takovém případě může pomoci zapnout možnost 'renderuj textury bez OpenGL'. "
  "Kromě toho je možné renderování urychlit volbou 'renderuj jednou', ale v takovém případě "
  "bude povrch vygenerován pouze jednou, takže na něm není možné hrát.\n\n")
  
S("Use arrow keys to rotate, Page Up/Down to zoom.",
  "Model se otáčí šipkami a lze ho zoomovat klávesami Page Up/Down.")

S("what's this?", "co to je?")
S("enable the Hypersian Rug mode", "aktivuj mód hyperského koberce")
S("render the texture only once", "renderuj textury pouze jednou")
S("render texture without OpenGL", "renderuj textury bez OpenGL")
S("texture size", "rozměr textury")

// Crossroads IV & Chaos Mode
// --------------------------

N("Crossroads IV", GEN_O, "Křižovatka IV", "Křižovatky IV", "Křižovatku IV", "na Křižovatce IV")

S("Chaos mode", "chaotický mód")

S("In the Chaos mode, lands change very often, and there are no walls between them. "
  "Some lands are incompatible with this."
  "\n\nYou need to reach Crossroads IV to unlock the Chaos mode."
  ,

  "V chaotickém módu se kraje mění velice často a nejsou mezi nimi žádné stěny. "
  "Některé kraje nejsou s tímto módem kompatibilní."
  "\n\nChaotický mód musíš odemknout tím, že dojdeš na Křižovatku IV."
)

// Rose Garden
// -----------

N("Rose Garden", GEN_F, "Růžová zahrada", "Růžové zahrady", "Růžovou zahradu", "v Růžové zahradě")
N("False Princess", GEN_F, "Falešná princezna", "Falešné princezny", "Falešnou princeznu", "Falešnou princeznou")
N("False Prince", GEN_M, "Falešný princ", "Falešní princové", "Falešného prince", "Falešným princem")
N("Rose Lady", GEN_F, "Růžová paní", "Růžové paní", "Růžovou dámu", "Růžovou paní")
N("Rose Lord", GEN_F, "Růžový pán", "Rúžoví pánové", "Růžového pána", "Růžovým pánem")
N("Rose Beauty", GEN_F, "Růžová kráska", "Růžové krásky", "Růžovou krásku", "Růžovou krásku")
N("Handsome Gardener", GEN_F, "Pohledný zahradník", "Pohlední zahradníci", "Pohledného zahradníka", "Pohledným zahradníkem")
N("rosebush", GEN_O, "růžový keřík", "růžové keříky", "růžový keřík", "růžovým keříkem")

Orb("Beauty", "Krásy")
N("Thornless Rose", GEN_F, "Růže bez trnů", "Růže bez trnů", "Růži bez trnů", "Růží bez trnů")

S("Don't be fooled by this red-haired girl, or you will be stabbed if you come too close!",
  "Nenech se oklamat touto zrzavou dívkou -- pokud se k ní příliš přiblížíš, bodne tě!")
S("Don't be fooled by this red-haired boy, or you will be stabbed if you come too close!",
  "Nenech se oklamat tímto zrzavým chlapcem -- pokud se k němu příliš přiblížíš, bodne tě!")
S("This false princess is immune to the alluring scent of roses.",
  "Tato falešná princezna je imunní vůči vábivé vůni růží.")
S("This false prince is immune to the alluring scent of roses.",
  "Tento falešný princ je imunní vůči vábivé vůni růží.")
S("She has flowers in her long fair hair. You could not bring yourself to attack such a beautiful woman.",
  "Tato krasavice má ve svých dlouhých, světlých vlasech květiny. Na tak krásnou ženu se nemůžeš přimět zaútočit.")
S("Tall, strong, and holding a flower in his hand. You could not bring yourself to attack such a handsome man.",
  "Vysoký, silný a s květinou v ruce. Na tak pohledného muže se nemůžeš přimět zaútočit.")

S(  "This Orb makes you stunningly beautiful. "
    "Monsters which come next to you will be stunned for one turn. "
    "Multi-tile monsters are not affected. Additionally, it makes you immune to "
    "beauty.",

    "Tato Sféra ti dává omračující krásu. "
    "Netvoři, kteří vstoupí na políčko vedle tebe, budou na jedno kolo omráčeni. "
    "Netvoří, kteří zabírají více než jedno políčko, nejsou ovlivněni. Kromě toho ti "
    "tato Sféra poskytuje imunitu proti kráse.")

S(  "A big, beautiful, magical flower.", "Velká, krásná, magická květina.")

S("You just cannot stand in place, those roses smell too nicely.",
  "Nemůžeš zůstat na místě, ty růže příliš krásně voní.")
S("Those roses smell too nicely. You have to come towards them.",
  "Ty růže příliš krásně voní. Musíš jít směrem k nim.")


S(
  "Each eight turns, each rosebush at distance at most 5 from you will "
  "release a wave of alluring scent. Creatures on the frontwave "
  "will move towards where the scent came from. Even if it causes them "
  "to attack their friends or beautiful creatures, or move into water, fire, chasm, or thorns of the rosebush. "
  "Ivies, Ghosts, Rock Snakes, Rose Ladies and Lords, and monsters restricted to a specific "
  "terrain are immune to scents.",

  "Každý růžový keřík ve vzdálenosti 5 nebo méně od tebe vypustí každých 8 kol "
  "vlnu vábivé vůně. Každý tvor zasažený touto vůní se musí pohybovat "
  "směrem k jejímu zdroji. A to i tehdy, pokud by díky tomu musel "
  "zaútočit na své kamarády nebo krásné tvory nebo musel vstoupit do vody, do ohně, "
  "do propasti nebo na trny růžového keříku. Břečťan, Kamenní hadi, Růžové dámy a pánové a netvoři omezení "
  "na určitý terén jsou vůči vůni imunní.")

S(
  "This land is filled with beautiful, but dangerous, creatures and plants.",

  "Tento kraj je plný krásných, ale nebezpečných tvorů a rostlin.")

S("%The1 is killed by thorns!", "%1 se nabod%l na trny!")

// Warped Sea/Coast
// ----------------

N("Warped Coast", GEN_N, "Pokřivené pobřeží", "Pokřivená pobřeží", "Pokřivené pobřeží", "na Pokřiveném pobřeží")
N("Warped Sea", GEN_F, "Pokřivené moře", "Pokřivená moře", "Pokřivené moře", "na Pokřiveném moři")
N("Ratling", GEN_M, "Krysák", "Krysáci", "Krysáka", "Krysákem")
N("Coral", GEN_O, "Korál", "Korály", "Korál", "Korálem")
Orb("the Warp", "Pokřivení")

S(
  "This land is warped. Ironically, the coast is completely straight...",
  "Tento kraj je pokřivený. Ironií je, že pobřeží je absolutně rovné...")

S(
  "This part of the world is warped, restricting the movement somewhat. "
  "\"Diagonal\" movement and attacking between triangular cells is not allowed. "
  "Flash, Storms, and Freedom spells ignore this, and Ghosts can move, attack, and "
  "be attacked diagonally.",
  "Tato část světa je pokřivená, což poněkud omezuje pohyb. "
  "\"Úhlopříčný\" pohyb a útok mezi trojúhelníkovými políčky není možný. "
  "Kouzla Záblesku, Bouře a Svobody toto pravidlo ignorují a Duchové se mohou pohybovat "
  "a útočit úhlopříčně nebo být sami úhlopříčně napadeni.")

S( "This Orb creates a warped zone of radius 5 around you, "
  "and also allows you to move diagonally in warped zones.",

  "Tato Sféra kolem tebe vytvoří pokřivenou zónu o poloměru 5, "
  "a kromě toho ti také umožňuje úhlopříčný pohyb v pokřivených zónách.")

S(  "Corals have a somewhat hyperbolic structure even in your home world, "
    "but natural corals from the Warped Sea have truly beautiful shapes. "
    "Ratlings know the value of corals, and thus keep them in boats for safety.",

    "Korály mají do jisté míry hyperbolickou strukturu i ve tvém domovském světě, "
    "ale tvary přírodních korálů z Pokřiveného moře jsou skutečně nádherné. "
    "Krysáci znají hodnotu korálů, a kvůli bezpečnosti je přechovávají ve člunech.")

S(
    "These warped humanoids are skilled warriors and sailors, and they "
    "feel at home at the Warped Coast. Their battle experience has taught them "
    "that enemies who wait without moving or attacking anything are the most deadly. "
    "If they see such an enemy, they become extremely suspicious, and they also wait.",

    "Tito pokřivení humanoidé jsou dobrými válečníky i námořníky a "
    "na Pokřiveném pobřeží jsou jako doma. Jejich bojové zkušenosti je naučily, "
    "že nepřátelé, kteří čekají -- nehýbou se a neútočí -- jsou ti nejvražednější. "
    "Když takového nepřítele uvidí, začnou být velmi podezíraví a budou čekat také.")


S("hexagonal #1", "šestiúhelníkový #1")
S("You cannot move between the triangular cells here!", "Tady se nemůžeš pohybovat mezi trojúhelníkovými políčky!")
S("display the triheptagonal grid", "zobraz triheptagonální mřížku")

N("no wall", GEN_F, "žádné stěna", "žádné stěny", "žádnou stěnu", "žádnou stěnou")
N("no monster", GEN_M, "žádný netvor", "žádní netvoři", "žádného netvora", "žádným netvorem")
N("no item", GEN_O, "žádný předmět", "žádné předměty", "žádný předmět", "žádným předmětem")

S("\n\nA Ghost never moves to a cell which is adjacent to another Ghost of the same kind.",
  "\n\nDuch nikdy nevstoupí na políčko sousedící s jiným Duchem stejného typu.")

S("You cannot attack diagonally!", "Nemůžeš útočit úhlopříčně!")

Orb("Energy", "Energie")

S(  "This Orb halves the power usage of orbs which cost some "
    "charges with each activation. It even affects the "
    "one-shot orbs such as Flash or Teleport. If such an activation "
    "normally costs x charges, it costs only x/2 (rounded up) "
    "if you have an Orb of Energy.",

    "Některé Sféry spotřebují každou aktivací nějaké množství nábojů. Sféra Energie "
    "snižuje toto množství na polovinu. Funguje dokonce i s jednorázovými "
    "Sférami, jako je Sféra Záblesku nebo Teleportace. Aktivace, která by normálně "
    "stála x nábojů bude se Sférou Energie stát pouze x/2 nábojů (zaokrouhleno nahoru).")

/*

		"NEW_ACHIEVEMENT_6_17_NAME"	"Došel jsi na Zborcené pobřeží"
		"NEW_ACHIEVEMENT_6_17_DESC"	"Získej Korál."
		"NEW_ACHIEVEMENT_6_18_NAME"	"Krysák"
		"NEW_ACHIEVEMENT_6_18_DESC"	"Získej 10 Korálů."
		"NEW_ACHIEVEMENT_6_19_NAME"	"Pokřivený Krysák"
		"NEW_ACHIEVEMENT_6_19_DESC"	"Získej 25 Korálů."
		"NEW_ACHIEVEMENT_6_20_NAME"	"Pán pokřivení"
		"NEW_ACHIEVEMENT_6_20_DESC"	"Získej 50 Korálů."
		"NEW_ACHIEVEMENT_6_21_NAME"	"Došel jsi do Růžové zahrady"
		"NEW_ACHIEVEMENT_6_21_DESC"	"Získej Růži bez trnů."
		"NEW_ACHIEVEMENT_6_22_NAME"	"Falešná princezna"
		"NEW_ACHIEVEMENT_6_22_DESC"	"Získej 10 Růží bez trnů."
		"NEW_ACHIEVEMENT_6_23_NAME"	"Růžová kráska"
		"NEW_ACHIEVEMENT_6_23_DESC"	"Získej 25 Růží bez trnů."
		"NEW_ACHIEVEMENT_6_24_NAME"	"Růžová dáma"
		"NEW_ACHIEVEMENT_6_24_DESC"	"Získej 50 Růží bez trnů."
		"NEW_ACHIEVEMENT_6_25_NAME"	"Cestovatel"
		"NEW_ACHIEVEMENT_6_25_DESC"	"Dojdi na Křižovatku IV."
		"NEW_ACHIEVEMENT_6_26_NAME"	"Pán chaosu"
		"NEW_ACHIEVEMENT_6_26_DESC"	"Získej 300 $$$ v chaotickém módu."

*/


N("warp gate", GEN_F, "křivá brána", "křivé brány", "křivou bránu", "křivou branou")
S("This gate separates the warped area from the normal land.", "Tato brána odděluje pokřivenou oblast od normálních krajů.")
S("You cannot move between the cells without dots here!",
  "Nie możesz tu przechodzić między polami bez kropek!")
S("You need to move to give space to %the1!",
  "Musíš se pohnout, abys uděla%l0 místo pro %a1!")

N("Ratling Avenger", GEN_M, "Krysák mstitel", "Krysáci mstitelé", "Krysáka mstitele", "Krysákem mstitelem")

S(
  "Those roses smell too nicely. You can only target cells closer to them!",
  "Ty růže příliš krásně voní. Můžeš zacílovat jenom políčka, která k nim jsou blíž!"
  )

S(
  "So, you have killed a Ratling on the unwarped sea? You will be punished for this! "
  "Luckily, if you run away from the Warped Sea quickly, the Ratling Avengers will lose track of you.",

  "Ty jsi zabi%l0 Krysáka na nepokřiveném moři? Za to zaplatíš! "
  "Krysáci mstitelé o tebe naštěstí ztratí zájem, pokud z Pokřiveného moře rychle utečeš."
  )

// VERSION 8.3
// ============

S("Kills required: %1 (%2).\n", "Nutno zabít nepřátel: %1 (%2).\n")

// Conformal/history mode
// ----------------------

S("conformal/history mode", "konformální mód / historie")
S("Preparing the line (%1/1000)...", "Připravuji přímku (%1/1000)...")
S("Could not create an image of that size.", "Obrázek této velikosti nebylo možné vytvořit.")
S("Saved the band image as: ", "Pásový obrázek uložen jako: ")
S("right", "doprava")
S("left", "doleva")
S("up", "nahoru")
S("down", "dolů")
S("counterclockwise", "proti směru hodinových ručiček")
S("clockwise", "po směru hodinových ručiček")
S("zoom out", "oddálení")
S("zoom in", "přiblížení")
S("left to right", "zleva doprava")
S("right to left", "zprava doleva")
S("spin down", "otoč dolů")
S("spin up", "otoč nahoru")
S("disk", "disk")
S("half-plane", "polorovina")
S("band", "pás")
S("polygonal", "polygonální")
S("polynomial", "polynomiální")
S("include history", "včetně historie")
S("model used", "použitý model")
S("rotation", "rotace")

S("coefficient", "koeficient")
S("which coefficient", "kterů koeficient")
S("polygon sides", "strany mnohoúhelníku")
S("star factor", "zhvězdění")
S("degree of the approximation", "stupeň opień przybliżenia")
S("prepare the line animation", "připrav přímkovou animaci")
S("animation speed", "rychlost animace")
S("render bands automatically", "renderovat pásy automaticky")
S("band width", "šířka pásu")
S("length of a segment", "délka segmentu")
S("spiral on rendering", "spirála při renderování")
S("render now (length: %1)", "renderovat hned (délka: %1)")
S("Enable cheat mode or GAME OVER to use this", "Dostupné pouze v cheat módu nebo po skončení hry")

// remember to note it's in English
S("see http://www.roguetemple.com/z/hyper/models.php", "viz http://www.roguetemple.com/z/hyper/models.php (anglicky)")

// Yendorian Forest
// ----------------

N("Yendorian Forest", GEN_O, "Yendorský les", "Yendorské lesy", "Yendorský les", "v Yendorském lese")

S(
  "This forest was planted by one of the wizards from the Ivory Tower "
  "to conduct experiments with gravity.",

  "Tento les vysadil jeden z čarodějů z Věže ze slonové kosti, "
  "aby tu prováděl pokusy s gravitací.")

N("Yendorian Researcher", GEN_M, "Yendorský badatel", "Yendorští badatelé", "Yendorského badatele", "Yendorským badatelem")

S(
    "These people study gravity and infinite trees. "
    "They have no special features, other than wearing a strange hat.",

  "Tito lidé studují gravitaci a nekonečné stromy. Kromě svých podivných "
  "čepic nemají žádné zvláštní schopnosti.")

N("Sparrowhawk", GEN_M, "Krahujec", "Krahujci", "Krahujce", "Krahujcem")

S("A bird who hunts in the treetops of the Yendorian Forest.",
  "Pták, který loví v korunách Yendorského lesa.")

N("Apple", GEN_N, "Jablko", "Jablka", "Jablko", "Jablkem")

S("A fruit from the Yendorian Forest.", "Ovoce z Yendorského lesa.")

N("trunk", GEN_O, "kmen", "kmeny", "kmen", "kmenem")

S("The skeleton of a tree.", "Skelet stromu.")

N("solid branch", GEN_F, "pevná větev", "pevné větve", "pevnou větev", "pevnou větví")
S("Branches here could bear your weight easily.",
  "Tyhle větve by snadno unesly tvou váhu.")

N("weak branch", GEN_F, "tenká větev", "tenké větve", "tenkou větev", "tenkou větví")
S("Branches here will bear you weight, but if you use them to move (not fall) to an unstable place, they will break.",
  "Tyto větve tvou váhu unesou, ale pokud se z nich pohneš (kromě pádu) na nestabilní políčko, zlomí se.")

N("canopy", GEN_F, "koruna", "koruny", "korunu", "korunou")
S(
  "Only thin twigs and leaves here. They may bear fruits, but for you, these cells count as unstable.",
  "Tady jsou jen tenké větvičky a listy. Mohou nést ovoce, ale pro tebe se počítají jako nestabilní.")


// Dragon Chasm
// ------------

N("Dragon Chasms", GEN_F, "Dračí propasti", "Dračí propasti", "Dračí propasti", "v Dračích propastech")

N("Dragon", GEN_M, "Drak", "Draci", "Draka", "Drakem")

N("Dragon Scale", GEN_F, "Dračí šupina", "Dračí šupiny", "Dračí šupinu", "Dračí šupinou")

S(
    "Dragon Scales are a prized material for armors. "
    "They are also prized by collectors, who would like to boast "
    "about how they have killed a Dragon.\n\n"
    "Dragon Scales disappear after 500 turns.",

    "Dračí šupiny jsou ceněným materiálem pro výrobu zbroje. "
    "Také si je cení sběratelé, kteří by se rádi "
    "chlubili tím, jak zabili Draka.\n\n"
    "Dračí šupiny zmizí po uplynutí 500 kol."
    )

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

    "Draci jsou mocní netvoři. Jsou pomalí, ale zlí, "
    "a moc rádi si dovolují na ty, kdo jsou ještě pomalejší než oni. "
    "Musíš je zastavit!\n\n"

    "Drak se pohybuje vždy jednou za dvě kola. Může zaútočit všemi svými "
    "segmenty, posunout celé své tělo dopředu nebo dozadu, nebo stáhnout "
    "dozadu přední část těla. Abys je zabil, musíš zasáhnout všechny jeho segmenty. "
    "Když se Drak nehýbe, jeho hlava automaticky zregeneruje, takže ji "
    "budeš obvykle musez zasáhnout posledním útokem; pokud je hlava zdravá, "
    "může drak chrlit oheň (na vzdálenost 3 polí), což jí vezme jeden život. "
    "Zabitím Draka v Dračích propastech získáš poklady."
   )

S("%The1 breathes fire at you!", "%1 na tebe chrlí oheň!")
S("%The1 breathes fire!", "%1 chrlí oheň!")

Orb("Domination", "Dominance")

S(
   "This Orb lets you ride Dragons and other worm-like creatures. "
   "Simply move onto such a creature to ride them; while riding, you are protected from dangerous terrains "
   "and partially from attacks (they cause you to lose half of your Domination power), "
   "but you cannot collect items.\n\n"
//   "When only one charge is left, "
//   "you have to dismount this turn -- be very careful to make this possible, "
//   "as your mount could attack you immediately!\n\n" 
   "While riding, "
   "click on a location to order your mount to move or attack there.",

   "Tato Sféra ti umožňuje jezdit na Dracích a jiných červovitých netvorech. "
   "Stačí se na ně pohnout, a můžeš nasednout; během jízdy jsi chráněn před nebezpečným terénem "
   "a částečně i před útoky (ty tě připraví o polovinu zbývající síly Dominance), "
   "nemůžeš ale sbírat předměty.\n\n"
//   "Ve chvíli, kdy ti zbývá pouze jeden náboj, "
//   "musíš v tomto kole sesednout -- dej si dobrý pozor, aby to bylo možné,  "
//   "protože tvůj oř na tebe může okamžitě zaútočit!\n\n "
   "Během jízdy můžeš kliknutím nařizovat svému oři, kam se "
   "má pohnout nebo zaútočit.")

S("You command %the1!", "%1 přijímá tvůj rozkaz.")
S("You need to dismount %the1!", "Musíš seskočit!")
S("%The1 tries to dismount you!", "%1 se tě snaží shodit!")

S("Enemies killed: %1 (%2 types)", "Počet zabitých nepřátel: %1 (%2 typů)")
S(" kills: %1/%2", " zabití: %1/%2")
S("Different kills required: %1.\n", "Požadovaný počet typů zabitých nepřátel: %1.\n")

// Galapagos
// ---------

N("Galápagos", GEN_F, "Galapágy", "Galapágy", "Galapágy", "na Galapágách")

N("Baby Tortoise", GEN_F, "Želvička", "Želvičky", "Želvičku", "Želvičkou")

Orb("the Shell", "Krunýře")

S(
    "This Orb protects you from physical attacks. "
    "It lasts for more turns than the Orb of Shielding, but "
    "10 charges are lost whenever you are attacked. "
    "It also does not protect you from fires, scents, and being eaten.",

    "Tato Sféra tě chrání před fyzickými útoky. "
    "Vydrží déle než Sféra Štítu, ale každý útok ti ubere "
    "10 nábojů. Nechrání proti ohni, pachu a sežrání.")

S(
  "Galápagos is the land of Tortoises. "
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

  "Galapágy jsou zemí Želv. Želvy jsou velice pomalé, takže si na ně mohou dovolovat "
  "Draci, kteří kradou a požírají jejich mláďata. Ty jim můžeš jejich Želvičky přinést zpátky, "
  "ale je tu jeden háček: existuje mnoho typů Želv podle toho, ve které části Galapág žijí -- "
  "je tu celkem 21 binárních faktorů prostředí, a tedy 2 097 152 variant. "
  "Musíš najít takovou Želvu, která přesně odpovídá tvé Želvičce!\n\n"
  "Želvy se pohybují vždy jednou za 3 kola a útoky je mohou pouze omráčit.\n\n"
  "Vrácení Želvičky se počítá za 5 $$$. Čím více faktorů se v dané části Galapág shoduje, "
  "tím jasněji tato část vypadá na obrazovce.")

S("%The1 scares %the2 a bit!", "%1 trochu vyleka%l1 %a2!")
S("%The1 attacks your shell!", "%1 zaútoči%l1 na tvůj krunýř!")

S("Hyperstone Quest: collect at least %3 points in %the2",
  "Hyperkamový úkol: získej nejméně %3 bodů %abl2")

S("animals killed: %1", "zabitých zvířat: %1")
S("\n\nTortoises are not monsters! They are just annoyed. They do not count for your total kills.",
  "\n\nŽelvy nejsou netvoři! Jsou jenom podrážděné. Proto se nepočítají do celkového počtu zabitých nepřátel.")

S("Aww, poor %1... where is your family?", "Á, chudáčku Želvičko... kdepak máš rodinu?")
S("You are now a tortoise hero!", "Nyní jsi želví hrdina!")
S("Find a %1 in %the2.", "Najdi %a1 %abl2.")

S("\"Be careful in the Rose Garden! It is beautiful, but very dangerous!\"",
  "\"V Růžové zahradě si dávej pozor! Je pěkná, ale také pěkně nebezpečná!\"")
S("\"Thank you very much for talking, and have a great rest of your day!\"",
  "\"Moc ti děkuju za rozhovor a hezký zbytek dne!\"")

/*
		"NEW_ACHIEVEMENT_6_27_NAME"	"Drakobijec"
		"NEW_ACHIEVEMENT_6_27_DESC"	"Získej Dračí šupinu."
		"NEW_ACHIEVEMENT_6_28_NAME"	"Dračí pán"
		"NEW_ACHIEVEMENT_6_28_DESC"	"Dískej 10 Dračích šupin."
		"NEW_ACHIEVEMENT_6_29_NAME"	"Drači princ"
		"NEW_ACHIEVEMENT_6_29_DESC"	"Získej 25 Dračích šupin."
		"NEW_ACHIEVEMENT_6_30_NAME"	"Dračí král"
		"NEW_ACHIEVEMENT_6_30_DESC"	"Získej 50 Dračích šupin."
		"NEW_ACHIEVEMENT_6_31_NAME"	"Želví hrdina"
		"NEW_ACHIEVEMENT_6_31_DESC"	"Zachraň dvě Želvičky."
		"NEW_ACHIEVEMENT_7_0_NAME"	"Želví šampión"
		"NEW_ACHIEVEMENT_7_0_DESC"	"Zachraň pět Želviček."
		"NEW_ACHIEVEMENT_7_1_NAME"	"Želví nindža"
		"NEW_ACHIEVEMENT_7_1_DESC"	"Zachraň deset Želviček."
		"NEW_ACHIEVEMENT_7_2_NAME"	"Došel jsi do Yendorského lesa"
		"NEW_ACHIEVEMENT_7_2_DESC"	"Najdi a získej Jablko."
		"NEW_ACHIEVEMENT_7_3_NAME"	"Yendorský badatel"
		"NEW_ACHIEVEMENT_7_3_DESC"	"Získej 10 Jablek."
		"NEW_ACHIEVEMENT_7_4_NAME"	"Krahujec"
		"NEW_ACHIEVEMENT_7_4_DESC"	"Získej 25 Jablek."
		"NEW_ACHIEVEMENT_7_5_NAME"	"Yendorský mistr"
		"NEW_ACHIEVEMENT_7_5_DESC"	"Získej 50 Jablek."
*/

S("(includes co-op)", "(zawiera co-op)")
S("Revived!", "Wskrzeszono!")

S("turn count = %1 last exploration = %2 ghost timer = %3",
  "počet kol = %1 poslední zkoumání = %2 časovač duchů = %3")

S("You hear a distant roar!", "Zaslechl jsi vzdálené zařvání!")

S("summon Bonfire", "vyvolej ohniště")
S("Hyperstone Quest", "hyperkamový úkol")
S("summon dead orbs", "vyvolej mrtvé sféry")
S("summon a Monster", "vyvolej netvora")
S("gain orb powers", "získej moc sféry")
S("summon a Golem", "vyvolej golema")
S("summon Thumpers", "vyvolej Tlouky")
S("summon Ivy", "vyvolej Břečťan")
S("lose all treasure", "zbav se všech pokladů")
S("gain kills", "získej zabité netvory")
S("Select the land ---", "Vyber si kraj ---")
S("summon Mimics", "vyvolej Mimiky")
S("summon orbs", "vyvolej sféry")
S("deplete orb powers", "zruš moc sféry")
S("Safety (quick save)", "Bezpečnost (rychlé uložení)")
S("summon treasure", "vyvolej poklad")
S("summon lots of treasure", "vyvolej spoustu pokladů")
S("--- and teleport there", "--- a teleportuj se tam")
S("summon Sand Worm", "vyvolej Písečného červa")
S("summon Orb of Yendor", "vyvolej Yendorskou sféru")
S("rotate the character", "otoč postavu")

S("gain Orb of Yendor", "získej Yendorskou sféru")
S("switch ghost timer", "přepni časové duchy")
S("switch web display", "přepni strukturu")
S("You damage %the1 with a mental blast!", "Zrani%l0 jsi %a1 mentálním útokem!")
S("You are now a tortoise heroine!", "Nyní jsi želví hrdinka!")
S("Note: currently scores are saved only in the normal mode on Android",
  "Poznámka: na Androidu se skóre v tuto chvíli ukládají pouze v normálním módu")
S("Note: you can play, but scores won't be saved on Android",
  "Poznámka: můžeš hrát, ale tvé skóre se neuloží")
S("settings set here won't be saved", "nastavení provedená zde nebudou uložena")
S("-- use the Android menu instead", "-- použij menu Android")

// just copy: "player 5 X"...
S("configure player 5", "konfigurace hráče 5")
S("configure player 6", "konfigurace hráče 6")
S("configure player 7", "konfigurace hráče 7")

S("player 5 X", "hráč 5 X")
S("player 5 Y", "hráč 5 Y")
S("player 5 go", "hráč 5 pohyb")
S("player 5 spin", "hráč 5 otáčení")
S("player 6 X", "hráč 6 X")
S("player 6 Y", "hráč 6 Y")
S("player 6 go", "hráč 6 pohyb")
S("player 6 spin", "hráč 6 otáčení")
S("player 7 X", "hráč 7 X")
S("player 7 Y", "hráč 7 Y")
S("player 7 go", "hráč 7 pohyb")
S("player 7 spin", "hráč 7 otáčení")

// VERSION 9.0
//=============

// extra descriptions for Orbs and some other things
// -------------------------------------------------

S("\n\nThis Orb is triggered on your first attack or illegal move.",
  "\n\nTato sféra se aktivuje prvním útokem nebo ilegálním tahem.")

S("\nYou can never target cells which are adjacent to the player character, "
     "or ones out of the sight range.",
     "\nPolíčka, která sousedí s vaší postavou nebo jsou mimo váš dohled, nelze nikdy zacílit.")

S("\nThis is a ranged Orb. ", "\nToto je sféra, kterou je třeba zacílit na určité políčko.")

S("\nRanged Orbs can be targeted by long touching the desired location.",
  "\nTyto sféry lze zacílovat dlouhým stiskem požadovaného políčka.")

S("\nRanged Orbs can be targeted by touching the desired location.",
  "\nTyto sféry můžete zacílovat tak, že se dotknete požadovaného políčka.")
  
S("\nRanged Orbs can be targeted by shift-clicking the desired location. ",
  "\nTyto sféry lze zacílovat kliknutím na požadované políčko se stisknutou klávesou Shift.")

S("\nRanged Orbs can be targeted by clicking the desired location. ",
  "\nTyto sféry lze zacílovat kliknutím na požadované políčko.")
  
S("\n\nOnce you collect 10 Bomberbird Eggs, "
  "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
  "Collecting even more Eggs will increase the radius. Additionally, collecting "
  "25 Bomberbird Eggs will reveal adjacent cells even in your future games.",
  
  "\n\nJakmile získáš 10 Vajec Bombarďáků, odhalí vstup na políčko bez sousedních min "
  "i všechna sousední políčka. Sebráním ještě většího počtu Vajec, se rozsah "
  "odhalování zvyšuje. Pokud sebereš 25 Vajec Bombarďáků, budou se navíc ve všech "
  "dalších hrách sousední políčka odhalovat automaticky.")

S("This might be very useful for devices with limited memory.",
  "To může být velmi užitečné při hře na zařízeních s omezenou pamětí.")
  
S("(in the MENU). You can reduce the sight range, this should make the animations smoother.",
  "(v MENU). Můžete zredukovat dohled, což by mělo udělat animace hladšími.")

S("Unavailable in the shmup mode.\n", "Nedostupné v módu střílečky.\n")
S("Unavailable in the multiplayer mode.\n", "Nedostupné v módu pro více hráčů.\n")
S("Unavailable in the Chaos mode.\n", "Nedostupné v chaotickém módu.\n")
S("Bonus land, available only in some special modes.\n",
  "Bonusový kraj, který je dostupný pouze v některých speciálních módech.\n")

S("\n\nThis Orb protects you from attacks, scents, and insulates you "
  "from electricity. It does not let you go through deadly terrain, but "
  "if you are attacked with fire, it lets you stay in place in it.",
  
  "\n\nTato sféra te chrání před útoky a pachy a izoluje tě před elektřinou. "
  "Neumožňuje procházet smrtícím terénem, ale pokud na tebe někdo zaútočí ohněm, "
  "můžeš v něm zůstat stát.")

S("Completing the quest in this land is not necessary for the Hyperstone Quest.",
  "Splnění mise v tomto kraji není nezbytné pro Hyperkamovou misi.")

// other options
// -------------

S("player", "hráč")
S("movement color", "barva pohybu")
S("model used (not conformal!)", "použitý model (nekonformální!)")
S("azimuthal equidistant", "azimutální délkojevný")
S("azimuthal equi-area", "azimutální plochojevný")
S("visit the website", "navštiv webovou stránku")
S("leaderboards/achievements", "leaderboardy/achievementy")
S("return to the game", "návrat do hry")

// Android texts
// -------------

S("connected to Google Games", "připojeno ke Google Games")
S("background music", "hudba")
S("view your achievements", "tvé achievementy")
S("next page", "další stránka")

// new multiplayer

S("Two players cannot move/attack the same location!", "Dva hráči nemohou zacílovat stejnou lokaci!")
S("Cannot move into the current location of another player!", "Nemůžeš vstoupit na políčko obsazené jiným hráčem!")
S("Players cannot get that far away!", "Hráči se nemohou vzdálit tak daleko!")
S("Cannot push into another player!", "Nemůžeš tlačit do jiného hráče!")
S("Cannot push into the same location!", "Nemůžeš tlačit na stejné místo!")

S("%The1 throws fire at %the2!", "%1 hází oheň na %a2!")
S("%The1 breathes fire at %the2!", "%1 chrlí oheň na %a2!")

S("configure keys", "konfigurace kláves")
S("use default keys", "standardní klávesy")

S("Left the game.", "Opuštění hry.")
S("shoot'em up mode", "střílečka")
S("turn-based mode", "hra na kola")
S("continue playing", "pokračuj ve hře")
S("start a new game", "nová hra")

S("five players", "pět hráčů")
S("six players", "šest hráčů")
S("seven players", "sedm hráčů")

S("You collect %the1. (%2)", "Získa%l0 jsi %a1. (%2)")


S(
"Shmup (shoot'em up) mode: You can play a hyperbolic shoot'em up game. The game is based "
"on the usual turn-based grid-based HyperRogue, but there are some changes. You fight by "
"throwing knives, and you have three extra lives. There are no allies, so all Orbs "
"related to allies give you extra lives instead (up to 5). Some other rules have been "
"adapted too.\n\n",

"Mód střílečky: zde můžeš hrát hyperbolickou střílečku. Hra je založena na běžném "
"HyperRogue (s koly a políčky), ale jsou tu určité změny. Bojuješ tak, že házíš nože, "
"a máš tři životy navíc. Nemůžeš mít žádné kamarády, takže všechny Sféry, které "
"s nimi souvisejí, ti místo toho přidávají životy navíc (maximum je 5). I některá "
"další pravidla jsou adaptována.\n\n")

S(
"Multiplayer: Play cooperatively (locally); treasures, kills, and deaths are calculated "
"for each player too, for more competitive play. Orbs and treasures are shared, orbs drain "
"faster, knives recharge slower, and player characters are not allowed to separate.\n\n",

"Více hráčů: Zde může hrát několik hráčů kooperativně (lokálně); poklady, "
"zabití netvoři a úmrtí se počítají pro každého hráče zvlášť, aby se dalo "
"hrát kompetitivněji. Sféry a poklady se sdílejí, sféry se vyčerpávají rychleji, "
"nože se dobíjejí pomaleji a postavy se nesmějí příliš vzdálit.\n\n")

S(
"Turn-based multiplayer: Turns are executed in parallel. A player can leave the game "
"by pressing a designated key (useful when about to get killed or lost). The following "
"Orbs work to bring such players back: ",
"Hra pro více hráčů na kola: Všichni hráči odehrají své kolo současně. Hráči mohou "
"opustit hru stisknutím specifické klávesy (hodí se, když jim hrozí, že se ztratí "
"nebo je něco zabije). Tyto hráče je možné vrátit zpátky použitím následujících Sfér:")

S("This menu can be also used to configure keys.\n\n",
  "Toto menu je také možné použít pro konfiguraci kláves.\n\n")

// other stuff
// -----------

S("Warnings are issued when you try to do something that appears dangerous, "
  "like stepping on a known mine, or getting your boat destroyed by "
  "a Kraken without having Orb of the Fish. In some cases the action "
  "might actually be safe -- so you can ignore the warning and do it anyway, "
  "simply by repeating the action.",
  
  "Varování se objeví, když se pokusíte udělat něco, co vypadá nebezpečně, jako "
  "je šlápnutí na políčko, kde víte, že je mina, nebo riskování, že vám Kraken "
  "zničí loďku, když nemáte Sféru Ryby. V některých případech může být taková "
  "věc ve skutečnosti bezpečná -- a proto je možné opakováním akce varování "
  "ignorovat a stejně to udělat.")


// kraken depths
// -------------

N("Kraken Depths", GEN_F, "Krakení hlubiny", "Krakení hlubiny", "Krakení hlubiny", "v Krakeních hlubinách")

S(
    "A long time ago, this was a trade route. But then, Krakens have risen out of the "
    "depths. Many trading ships sank here. Legend says that you can uncover the secret "
    "of a magical weapon spell somewhere in the depths...\n\n"
    
    "You can find Sunken Treasures here, but they won't appear until you have killed "
    "a Kraken. You will also need Orb of the Fish to get the treasures, luckily you can "
    "steal one from the Viking treasure hunters.",
    
    "Tohle byla kdysi dávno obchodní trasa. Jenomže potom se z hlubin vynořili "
    "Krakeni. Mnoho obchodních lodí se tu potopilo. Legenda praví, že kdesi "
    "v hlubinách můžete najít tajemství kouzla, které vytváří magickou zbraň...\n\n"
    "V Hlubinách lze najít Potopené poklady, ale ty se objeví teprve poté, co "
    "zabiješ alespoň jednoho Krakena. K získání pokladů budeš také potřebovat "
    "Sféru Ryby, ale ty se naštěstí dají ukrást vikingským lovcům pokladů.")

S(
  "There are Krakens in your homeland, too... huge sea monsters which "
  "could easily destroy ships. The geometry of this strange world "
  "prevents quick movement of huge objects, "
  "so there are no large ships, only small boats, and "
  "hyperbolic Krakens are relatively small too. Still, you suppose they might be "
  "the widest creatures which could still move at considerable speed...\n\n"
  
  "Kraken heads can move only on hexagons. You need to attack all the tentacles to "
  "kill the Kraken. A tentacle cannot attack if it has been attacked on the "
  "same turn. When a Kraken attacks you while you are in a boat, it "
  "destroys the boat, but does not kill you.",
  
  "I ve tvé domovině existují Krakeni... obrovské mořské obludy, které dokážou "
  "snadno zničit lodě. Geometrie tohoto podivného světa brání rychlému pohybu "
  "velkých objektů, a tak tu nejsou žádné velké lodě, jen malé loďky, a i "
  "hyperboličtí Krakeni jsou relativně malí. I tak jsou to možná nejširší "
  "tvorové, kteří se stále dokážou pohybovat nějakou slušnou rychlostí...\n\n"
  
  "Krakení hlava se může pohybovat pouze po šestiúhelníkových políčcích. Abys "
  "Krakena zabil, musíš zasáhnout všechna jeho chapadla. Chapadlo nemůže "
  "zaútočit, pokud jsi ve stejném kole zaútočil ty na něj. Pokud jsi "
  "na loďce, Krakenův útok ji zničí, ale ty zůstaneš naživu."
  )

Orb("the Sword", "Meče")

S(
    "This Orb gives you a weapon made of pure magical energy. You do not hold "
    "it, it simply floats in the air next to you. When you go, the energy sword moves "
    "with you, pointing at the same relative angle it pointed before -- you cannot "
    "move or rotate it otherwise. Most monsters can be killed by moving the sword into them, "
    "and won't move into the spot with the sword.",
    
    "Tato Sféra ti poskytne zbraň vyrobenou z čisté magické energie. Nedržíš ji -- "
    "prostě se jen vznáší ve vzduchu vedle tebe. Kamkoli půjdeš, energetický meč "
    "půjde s tebou a bude namířený ve stejném relativním úhlu -- jinak s ním "
    "nemůžeš pohybovat ani otáčet. Většinu netvorů lze zabít tím, že na jejich "
    "políčko posuneš meč, a samotní netvoři na políčko s mečem nevstoupí.")

N("Sunken Treasure", GEN_O, "Potopený poklad", "Potopené poklady", "Potopený poklad", "Potopeným pokladem")

S("Cargo of a ship which was once destroyed by a Kraken.", 
  "Náklad z lodi, kterou kdysi zničil Kraken.")

// burial grounds
// --------------

N("Burial Grounds", GEN_N, "Pohřebiště", "Pohřebiště", "Pohřebiště", "na Pohřebišti")

S(
  "Ancient Viking heroes were buried here. Their graves have barrows raised over "
  "them, and are guarded by Draugar, animated corpses who are immune to mundane weapons. "
  "You will need to use a magical weapon spell to defeat them, and to rob the "
  "ancient jewelry buried in the graves.",
  
  "Tady jsou pohřbeni dávní vikingští hrdinové. Nad jejich hroby jsou postaveny mohyly "
  "a střeží je Draugarové, oživlé mrtvoly imunní proti obyčejným zbraním. Pokud je "
  "chceš porazit a uloupit prastaré klenoty pohřbené v jejich hrobech, budeš si muset "
  "nějak vyčarovat magickou zbraň.")

N("Draugr", GEN_M, "Draugr", "Draugrové", "Draugra", "Draugrem")

S(
  "Animated corpses of ancient Viking warriors. They are immune to mundane weapons, "
  "but they can be destroyed by your Orb of the Sword.",
  "Oživlé mrtvoly dávných vikingských válečníků. Jsou imunní proti "
  "obyčejným zbraním, ale tvoje Sféra Meče je může zničit.")

Orb("the Sword II", "Meče II")

S(
  "An alternative version of Orb of the Sword. If you have both of them, "
  "you have two energy swords, facing in opposite directions.",
  "Alternativní verze Sféry Meče. Pokud máš obě zároveň, budeš mít dva "
  "energetické meče namířené do opačných směrů.."
  )

N("Ancient Jewelry", GEN_O, "Dávný šperk", "Dávné šperky", "Dávný šperk", "Dávným šperkem")

S(
  "Precious belongings of ancient Viking heroes. Your Orb of the Sword can be "
  "used to dig these treasures out of the barrows.",
  "Drahocenný majetel dávných vikingských hrdinů. Tvá Sféra Meče může tyto "
  "poklady vykopat z mohyl.")

S("%The1 destroys your boat!", "%1 zniči%l1 Tvou loďku!")
S("Your mundane weapon cannot hurt %the1!", "%1 se nedá zranit tvou obyčejnou zbraní!")
S("This move appears dangerous -- are you sure?", "Tento tah vypadá nebezpečně -- určitě ho chceš udělat?")
S("You slash %the1.", "Sek%l0 jsi %a1.")
S("You chop down %the1.", "Pokáce%l0 jsi %a1.")
S("Cannot slash another player!", "Nemůžeš seknout jiného hráče!")
S("%The1 suffocates!", "%1 leknu%l1!")

N("barrow wall", GEN_F, "stěna mohyly", "stěny mohyly", "stěně mohyly", "stěnou mohyly")

S("This wall is quite strong. You will need another way in.",
  "Tato stěna je velice pevná. Musíš najít jinou cestu dovnitř.")

N("barrow", GEN_F, "mohyla", "mohyly", "mohylu", "mohylou")

S("Your Orb of the Sword can be used to dig here.", 
  "Tvá Sféra Meče se dá použít k prokopání tohoto políčka.")

// Trollheim
// ---------

N("Trollheim", GEN_O, "Trollheim", "Trollheimy", "Trollheim", "v Trollheimu")

S(
    "Many clans of Trolls spend their lives in this kingdom. You can find many "
    "statues of Trolls here. You suppose that they are not actually statues, but simply "
    "elderly Trolls, who have petrified upon death. Or maybe you have killed "
    "these Trolls yourself?",
    
    "V tomto království tráví svůj život řada klanů Trollů. Také tu můžeš najít "
    "mnoho jejich soch. Říkáš si, že to asi nejsou sochy, ale prostě jen staří "
    "Trollové, kteří se po smrti proměnili v kámen. Nebo že by to byli "
    "Trollové, které jsi zabil ty?")


N("Golden Egg", GEN_N, "Zlaté vejce", "Zlatá vejce", "Zlaté vejce", "Zlatým vejcem")

S(
    "Trolls of Trollheim are descendants of a bridge Troll, who collected "
    "payments from people crossing the bridge. One of them paid with "
    "golden eggs. The bridge Troll found the eggs beautiful, but he quickly lost them. "
    "Golden eggs are still revered by Trolls, and you can find them in their "
    "caves.",
    
    "Trollové z Trollheimu jsou potomky trolla, který hlídal most a vybíral mýtné "
    "od těch, kdo po něm chtěli přejít. Jeden z nich zaplatil zlatými vejci. "
    "Trollovi tato vejce přišla moc krásná, ale rychle je ztratil. Trollové "
    "uctívají zlatá vejce dodnes a můžete je najít v jejich jeskyních.")

Orb("the Stone", "Kamene")

S(
    "Trolls turn into stone walls when they die. When you have this Orb, "
    "this happens to every monster you defeat. Statues created from this Orb "
    "have slightly different properties than Trolls who petrify naturally.",
    
    "Když Troll zemře, promění se v kámen. Tato Sféra to samé provede s každým "
    "netvorem, kterého porazíš. Sochy vytvořené pomocí Sféry Kamene mají trochu "
    "jiné vlastnosti než přirozeně zkamenělí Trollové.")

N("stone statue", GEN_F, "kamenná socha", "kamenné sochy", "kamennou sochu", "kamennou sochou")

S("A petrified creature.", "Jakýsi tvor proměněný v kámen.")

S("You leave %the1.", "Položi%l0 jsi %a1.") // Baby Tortoise

S("%The1 says, \"I die, but my clan in Trollheim will avenge me!\"",
  "1 říká: \"Já umírám, ale můj klan v Trollheimu mě pomstí!\"")

S("After killing %the1, you feel able to reach the Elemental Planes!",
  "Po zabití %a1 cítíš, že jsi schopen dosáhnout Elementálních rovin!")

S("You feel that a magical weapon is waiting for you...",
  "Cítíš, že na tebe čeká magická zbraň...")

S("reset per-player statistics", "Vyresetovat hráčské statistiky")
S("An alternate layout of the Crossroads, without walls.", "Alternativní verze Křižovatky bez zdí.")

S("Known mines may be marked by pressing 'm'. Your allies won't step on marked mines.", 

  "Políčka, která obsahují miny, je možné označit stiskem klávesy 'm'. Vaši spojenci na označené miny nevstoupí.")



S("Known mines may be marked by touching while in drag mode. Your allies won't step on marked mines.", 

  "Políčka, která obsahují miny, je možné označit dotekem v módu tažení. Vaši spojenci na označené miny nevstoupí.")

// VERSION 9.0n
// ============

// 9.0 patches
// ============

// extra help
// ----------

S("Coastal region -- connects inland and aquatic regions.\n",
  "Pobřežní kraj -- spojuje vnitrozemské a mořské kraje.\n")

S("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n",
  "Kraina morska -- dostępna jedynie zs krain przybrzeżnych i morskic.\n")

// barrow treasure protected from Orb of Space
// -------------------------------------------

S("%The1 is protected from this kind of magic!",
  "%1 má ochranu proti tomuto typu magie!")

// Orb is always available in the given land
// -----------------------------------------

S("always available", "vždy dostupná")

S("\n\nAfter the Trolls leave, you have 750 turns to collect %the1, or it gets stolen.",
  "\n\nKdyž Trollové odejdou, máš 750 kol na získání pokladu -- jinak ho něco ukradne.")

S(
    "There are many lands in HyperRogue. Collect 10 treasure "
    "in the given land type to complete it; this enables you to "
    "find the magical Orbs of this land, and in some cases "
    "get access to new lands. At 25 treasures "
    "this type of Orbs starts appearing in other lands as well. Press 'o' to "
    "get the details of all the Lands.\n\n",
    
    "Svět HyperRogue sestává z mnoha krajů. Když v nějakém kraji "
    "získáš 10 pokladů, dobyješ ho; pak v něm můžeš najít magické "
    "Sféry, a někdy také získáš přístup k jiným krajům. Po získání "
    "25 pokladů se tyto magické Sféry začnou objevovat i v jiných krajích. "
    "Detailní informace o všech krajích můžeš zobrazit stisknutím klávesy 'o'.\n\n"
    )

// welcome messages
// ----------------

S("Welcome to the Random Pattern mode!", "Vítej v módu náhodných vzorů!")
S("You are playing %the1 in the Pure Tactics mode.", "Právě hraješ %a1 v čistě taktickém módu.")
// %1 is the number
S("Welcome to the Yendor Challenge %1!", "Vítej v Yendorské misi %1!")
S("Welcome to HyperRogue!", "Vítej v HyperRogue!")
S("Press F1 or right-click things for help.", "Nápovědu můžeš zobrazit stiskem F1 nebo pravým kliknutím na různé věci.")
// for iOS
S("Press F1 or right-shift-click things for help.", "Nápovědu můžeš zobrazit stiskem F1 nebo kliknutím na věci s pravým shiftem.")
S("Welcome to the Euclidean mode!", "Vítej v eukleidovském módu!")
S("Good luck in the elliptic plane!", "Hodně štěstí v eliptické rovině!")
S("Welcome to Spherogue!", "Vítej ve Sférogue!")

// extra help for joystick configuration
// -------------------------------------

S("joystick configuration", "konfigurace joysticku")
S("first joystick position (movement)", "pozice prvního joysticku (pohyb)")
S("second joystick position (panning)", "pozice druhého joysticku (přesouvání)")
S("old style joystick configuration", "starý styl konfigurace joysticku")

S("move up-right", "pohyb doprava nahoru")
S("move up-left", "pohyb doleva nahoru")
S("move down-right", "pohyb doprava dolů")
S("move down-left", "pohyb doleva dolů")
S("stay in place (left + right)", "stát na místě (doleva + doprava)")
S("cancel move", "zruš pohyb")
S("leave the game", "vyskoč ze hry")
S("drop Dead Orb (up + down)", "polož Mrtvou sféru (nahoru + dolů)")

// extra help for configuration / projection and geometry
// ------------------------------------------------------

S("Roughly 42% cells are on the edge of your sight range. Reducing "
  "the sight range makes HyperRogue work faster, but also makes "
  "the game effectively harder.",
  "Zhruba 42% políček se nachází na poloměru tvého dohledu. Pokud snížíš "
  "dohled, bude HyperRogue běhat rychleji, ale také se tím ztíží.")

S("Watch the Minkowski hyperboloid or the hypersian rug mode with the "
  "red/cyan 3D glasses.",
  "Sleduj Minkowského hyperboloid nebo mód hyperského "
  "koberce s pomocí červenomodrých 3D brýlí.")

S("0 - return", "0 - návrat")
S("go back", "návrat")
S("F1 - help", "F1 - nápověda")

// for the conformal polynomial
// ----------------------------

S("coefficient (imaginary)", "koeficient (imaginární část)")
S("Scale the displayed model.", "Změň měřítko zobrazeného modelu.")
S("Reenter HyperRogue to apply this setting", "Toto nastavení se aplikuje po návratu do HyperRogue")

S(  "HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincaré models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
    "See also the conformal mode (in the special modes menu) "
    "for more models.",
    
    "HyperRogue interně používá model Minkowského hyperboloidu. "
    "Kleinův a Poincarého model lze získat pomocí perspektivy "
    "a Gansův model pomocí ortogonální projekce. Více modelů "
    "můžeš najít v konformálním módu (menu speciálních módů).")

S("stereographic projection", "stereografická projekce")
S("Poincaré model", "Poincarého model")
S("gnomonic projection", "gnómonická projekce")
S("Klein model", "Kleinův model")
S("towards orthographic", "směr k ortografické projekci")
S("towards Gans model", "směr ke Gansovu modelu")

S("Euclidean/elliptic mode", "Eukleidovský/eliptický mód")
S("geometry", "geometrie")
S("spherical", "sférická")
S("elliptic", "eliptická")
S("Euclidean", "eukleidovská")
S("projection", "projekce")

// VERSION 9.1
// ===========

N("Familiar", GEN_M, "Pomocník", "Pomocníci", "Pomocníka", "Pomocníkem")
S("Commanded %the1!", "%1 dosta%l1 rozkaz!")
S("F8 = SVG shot", "F8 = snímek SVG")
S("Saved the SVG shot to %1 (sightrange %2)", "Snímek SVG uložen do %1 (viditelnost %2)")

S("draw circle around the target", "nakresli kruh kolem cíle")
S("reverse pointer control", "obrácené ovládání ukazatele")
S("sight range", "viditelnost")
S("configure input", "konfigurace vstupu")

// quote of Euclid
S("\"There is no royal road to geometry.\"",
   "Není žádná královská cesta ke geometrii.\"")

// quote of Lobachevsky
S("\"There is no branch of mathematics, however abstract, ",
  "\"Není žádné matematické odvětví, jakkoli abstraktní, ")
  
S("which may not some day be applied to phenomena of the real world.\"",
  "které by jednoho dne nemohlo být možné aplikovat na jevy ve skutečném světě.\"")
  
// quote of Gauss
S("\"It is not possession but the act of getting there, ",
  "\"Ne vlastnictví, ale cesta k němu ")
  
S("which grants the greatest enjoyment.\"", "poskytuje největší potěšení.\"")

// quote of Escher
S("\"We live in a beautiful and orderly world, ",
  "\"Žijeme v krásném a uspořádaném světě, ")

S("and not in a chaos without norms.\"",
  "nikoli v chaosu, kde neplatí žádné normy.\"")

/*
		"NEW_ACHIEVEMENT_7_21_NAME"	"Smrtící smrt"
		"NEW_ACHIEVEMENT_7_21_DESC"	"Zabij Goblina energetickým mečem."
		"NEW_ACHIEVEMENT_7_22_NAME"	"Halloweenská čarodějnice"
		"NEW_ACHIEVEMENT_7_22_DESC"	"Získej 50 Pochoutek v Halloweenu."
		"NEW_ACHIEVEMENT_7_23_NAME"	"Halloweenský upír"
		"NEW_ACHIEVEMENT_7_23_DESC"	"Získej 100 Pochoutek v Halloweenu."
*/

S("You have enough treasure now to revive %the1!",
  "Nyní máš dost pokladů na to, abys mohl oživit %a1!")

S("Killed %1 can be revived with an Orb of Love, after you collect 20 more $$$.",
  "Zabit%ého1 %a1 je možné oživit pomocí Sféry Lásky, "
  "ale předtím musíš získat ještě 20 $$$."
  )
// Masculine -ého, Feminine -ou

S("%The1 will be revivable at %2 $$$", "%a1 bude možné oživit na %2 $$$")
// Genitive form identical to accusative here, but might not be for other words.

S("Halloween mini-game", "mini-gra Halloween")


N("Vampire Bat", GEN_M, "Vampýr", "Vampýři", "Vampýra", "Vampýrem")
N("Halloween", GEN_O, "Halloween", "Halloweeny", "Halloween", "v Halloweenu")
N("Treat", GEN_F, "Pochoutka", "Pochoutky", "Pochutku", "Pochoutkou")

S(
  "Halloween is a special land, that is available only in the spherical "
  "or elliptic geometry (press 'o' to switch). You play on the surface of "
  "a jack-o'-lantern, "
  "and have to collect as many Treats as possible. Each Treat you collect "
  "brings new monsters to fight, and new magical powers for you. You "
  "have to fight the monsters while effectively managing your limited "
  "resources.",
  
  "Halloween je zvláštní kraj, který je k dispozici pouze ve sférické nebo "
  "eliptické geometrii (pro přepnutí použijte klávesu 'o'). Hraje se v něm "
  "na povrchu dýňové lucerny, kde musíš posbírat co nejvíce Pochoutek. Každá "
  "Pochoutka, kterou sebereš, vyvolá nové netvory, se kterými musíš bojovat, "
  "a dá ti nové magické schopnosti. Musíš bojovat s netvory a přitom "
  "efektivně spravovat své omezené zdroje."
  )

S(
  "Vampire Bats don't attack normally, but they drain your magical powers if "
  "they are at distance at most 2 from you.",
  "Vampýři neútočí normální způsobem, ale pokud se dostanou do vzdálenosti "
  "2 a méně od tebe, vysávají ti tvé magické schopnosti.")

S("Ghosts can move through chasms!", "Duchové se mohou pohybovat přes propasti!")
S("Push Skeletons into the holes!", "Strč Kostlivce do děr!")
S("You'll need your magical sword against the Draugar!", "Proti Draugarům budeš potřebovat kouzelný meč!")
S("Demons are slow, but you'll need the experience against stronger ones...",
  "Čerti jsou pomalí, ale proti Ďáblům budeš potřebovat zkušenosti...")
S("You will need more experience to defeat the Greater Demon!",
  "Na poražení Ďábla budeš potřebovat víc zkušeností!")
S("Cultists throw fire at you from distance!",
  "Kultisté na tebe vrhají z dálky oheň!")
S("Defeat Flail Guards by moving away from them.",
  "Strážce s řemdihem porazíš tím, že se pohneš směrem od něj.")
S("Vampire Bats drain your magical powers!",
  "Vampýři ti vysávají magické schopnosti!")

S("Hint: use arrow keys to scroll.", "Nápověda: šipkami můžeš scrollovat.")
S("Hint: press 1 2 3 4 to change the projection.", "Nápověda: Klávesy 1 2 3 4 mění projekci.")
S("You gain a protective Shell!", "Získa%l0 jsi ochranný Krunýř!")
S("Hint: magical powers from Treats expire after a number of uses.",
  "Nápověda: magické schopnosti získané z Pochoutek zmizí po určitém počtu použití.")
S("A magical energy sword. Don't waste its power!",
  "Kouzelný energetický meč. Nepromarni jeho sílu!")
S("Another energy sword!", "Další energetický meč!")
S("You got Thorns! Stab monsters by moving around them.",
  "Získal jsi Trny! Nyní můžeš probodnout netvory tím, že kolem nich projdeš.")
S("Aethereal powers let you go through the holes.",
  "Síla Éteru ti umožňuje pohybovat se přes díry.")
S("The tasty treat increases your protection.",
  "Chutná Pochoutka zvyšuje tvou ochranu.")
S("You gain your protective Shell back!", "Získal jsi zpátky svůj ochranný Krunýř!")

S("rainbow landscape", "duhová krajina")
S("Cannot use %the1 here!", "Tady to nemùe pouít!")
S("draw the grid", "vykreslování møíky")
S("Escher/3D", "Escher/3D")
S("plain/3D", "obyèejné/3D")
S("3D", "3D")
S("Welcome to Halloween!", "Vítejte v Halloweenu!")

S("How long should the messages stay on the screen.",
  "Jak dlouho zùstanou zprávy na obrazovce")
S("select this color", "vyber tuto barvu")
S("sound effects volume", "hlasitost zvukových efektù")

S("\n\nSee sounds/credits.txt for credits for sound effects", 
  "\n\nPodìkování za zvukové efekty najde v souboru sounds/credits.txt")

S("scrolling speed", "rychlost scrollování")
S("movement animation speed", "rychlost pohybových animací")
S("+5 = move instantly", "+5 = okamitý pohyb")
S("extra graphical effects", "extra grafické efekty")

// VERSION 9.3
// ===========

S("SORT", "TŘÍDIT")
S("PICK", "VYBRAT")
S("PLAY", "HRÁT")

// 3D configuration
// ----------------

S("3D configuration", "3D konfigurace")
S("High detail range", "Rozsah vysokých detailů")
S("Mid detail range", "Rozsah středních detailů")
S("Camera level above the plane", "Výška kamery nad rovinou")
S("Ground level below the plane", "Úroveň země pod rovinou")
S("Projection at the ground level", "Projekce na úrovni země")
S("Height of walls", "Výška zdí")
S("Rock-III to wall ratio", "Poměr skály-III a zdí")
S("Human to wall ratio", "Poměr člověka a zdi")
S("Level of water surface", "Úroveň vodní hladiny")
S("Level of water bottom", "Úroveň dna")
S("set 3D monsters or walls in basic config first",
  "nejprve musíš nastavit 3D netvory nebo zdi v základní konfiguraci")
S("parameters set correctly", "správně nastavené parametry")

S("Objects at distance less than %1 absolute units "
  "from the center will be displayed with high "
  "detail, and at distance at least %2 with low detail.",
  
  "Objekty ve vzdálenosti méně než %1 od středu budou zobrazovány s vysokými detaily, a objekty ve vzdálenosti méně než %2 se středními detaily.")

S("Ground level is actually an equidistant surface, "
  "%1 absolute units below the plane P. "
  "Theoretically, this value affects the world -- "
  "for example, eagles could fly %2 times faster by "
  "flying above the ground level, on the plane P -- "
  "but the actual game mechanics are not affected. ",

  "Úroveň země je ve skutečnosti ekvidistantní plocha, která se nachází %1 absolutních "
  "jednotek pod rovinou P. Tato hodnota teoreticky ovlivňuje svět -- orli by například "
  "mohli létat %2-krát rychleji, kdyby létali nad úrovní země v rovině P -- ale herní "
  "mechaniky jako takové tím ovlivněny nejsou. ")
  
S(
  "(Distances reported by the vector graphics editor "
  "are not about points on the ground level, but "
  "about the matching points on the plane P -- "
  "divide them by the factor above to get actual "
  "distances.)",
  
  "(Vzdálenosti v editoru vektorové "
  "grafiky nejsou vzdálenosti bodů na úrovni země, ale odpovídajících bodů v rovině "
  "P -- skutečné vzdálenosti získáte jejich vydělením výše zmíněným faktorem.)")

S( "If we are viewing an equidistant g absolute units below a plane, "
   "from a point c absolute units above the plane, this corresponds "
   "to viewing a Minkowski hyperboloid from a point "
   "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
   "the Poincaré model for g=c, and Klein-Beltrami model for g=0.",
   
   "Díváme-li se na ekvidistantní plochu, která se nachází g absolutních jednotek "
   "pod rovinou, z bodu, který je c absolutních jednotek nad rovinou, je to "
   "ekvivalentní tomu, když se díváme na Minkowskiho hyperboloid z bodu, který je "
   "tanh(g)/tanh(c) jednotek pod jeho středem. To pak odpovídá Poincarého modelu, "
   "když je g=c, a Klein-Beltramiho modelu, když je g=0.")

S( "The height of walls, in absolute units. For the current values of g and c, "
   "wall height of %1 absolute units corresponds to projection value of %2.",
   
   "Výška zdí v absolutních jednotkách. Při současných hodnotách g a c odpovídá "
   "výška %1 absolutních jednotek projekční hodnotě %2.")

S( "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
   "Length of paths on the Rock III level is %3 of the corresponding length on the "
   "ground level.",
   
   "Poměr Skály III a zdí je %1, takže Skála III je vysoká %2 absolutních "
   "jednotek. Délka cest na úrovni Skály III je %3-násobkem odpovídající "
   "délky na úrovni země..")

S( "Humans are %1 "
   "absolute units high. Your head travels %2 times the distance travelled by your "
   "feet.",
   
   "Lidé jsou vysocí %1 absolutních jednotek. Tvá hlava urazí %2-násobek "
   "vzdálenosti, kterou urazí tvé nohy.")
  
// Euclidean regular patterns
// --------------------------

S("three colors", "tři barvy")
S("three colors rotated", "tři barvy s otočkou")
S("edit all three colors", "edituj všechny tři barvy")

S(" (local treasure)", " (místní poklad)")
S(" (increases treasure spawn)", " (zvyšuje výskyt pokladů)")
S(" (appears here)", " (objevuje se zde)")
S(" (appears here)", " (objevuje se zde)")
S("%The1 is destroyed!", "%1 by%l1 zniče%n1!") // generic message
// M/O-byl zničen, F-byla zničena, N-bylo zničeno

S("page", "stránka")
S("all", "vše")

S("Your Orb of Recall is blocked by something big!",
  "Tvoji Sféru Návratu blokuje něco velikého!")

S("You are recalled!", "Vracíš se!")
S("You vault over %the1!", "Přeskoči%l0 jsi %a1!")
S("You cannot attack %the1!", "Nemůžeš útočit na %a1!")

S("\n\nFast flying creatures may attack or go against gravity only in their first move.",
  "\n\nRychlí létající tvorové mohou útočit nebo se protivit gravitaci pouze při svém prvním pohybu.")

// Dungeon
// -------

N("Dungeon", GEN_O, "Žalář", "Žaláře", "Žalář", "v Žaláři")

S("The result of a collaboration of the Great Vizier and the Wizard of the Ivory Tower.",
  "Výsledek spolupráce Velkovezíra a Čaroděje z Věže ze slonové kosti.")

N("Slime Mold", GEN_F, "Hlenka","Hlenky", "Hlenku", "Hlenkou")

S("A very interesting species of slime mold.",
  "Velice zajímavý druh hlenky.")

N("Bat", GEN_M, "Netopýr", "Netopýři", "Netopýra", "Netopýrem")

S("Someone has told you that one can get battle experience safely by "
  "killing tons of essentially harmless creatures, such as Bats. But "
  "does this make any sense?...\n\n"
  
  "It does not. Bats cannot hurt you, but may block your movement, or "
  "toggle switches if they fall on them.",
  
  "Někdo ti řekl, že zabitím spousty v podstatě neškodných tvorů, jako jsou "
  "Netopýři, můžeš bezpečně získat bojové zkušenosti. Ale dává to nějaký "
  "smysl...?\n\n"
  
  "Ne, nedává. Netopýři ti nemohou ublížit, ale mohou blokovat tvůj pohyb nebo "
  "přepínat vypínače, když na ně spadnou."
  )

Orb("Recall", "Návratu")

S("When the charges on this Orb expire, "
  "you will be automatically returned to the place where you have found it. "
  "Extra Orbs of Recall delay this without changing the recall location. "
  "Picking up an Orb of Safety causes an immediate recall.",
  
  "Když vyprší náboje této Sféry, automaticky se vrátíš tam, kde jsi ji našel. "
  "Pokud sebereš více Sfér Návratu, místo návratu se nezmění -- návrat se pouze "
  "oddálí. Pokud sebereš Sféru Bezpečnosti, vrátíš se okamžitě."
  )
// Maybe needs new variety for "našel" -- the female form is "našla".

// 
//

// lost mountain
// =============

//N("Pyramid", GEN_F, "Pyramida", "Pyramidy", "Pyramida", "na Pyramidě")
N("Lost Mountain", GEN_F, "Ztracená hora", "Ztracené hory", "Ztracenou horu", "na Ztracené hoře")

/* S("An ancient civilization has created this pyramid in the Jungle by "
  "changing the gravity. The gravity is still changed, but "
  "the pyramid is overgrown with ivies and bushes. "
  "Will you dare to climb the ivies to get treasures waiting above?",
  
  ""
  ) */
  
S(
  "Gravitational anomalies in the Jungle create mountains "
  "overgrown with ivies and bushes. "
  "Will you dare to climb the ivies to get the amethysts hidden above?\n\n"
  
  "Cells adjacent to Ivies count as stable (but Ivies "
  "cannot climb themselves or other Ivies).",
  
  "Gravitační anomálie v Džungli vytvářejí hory zarostlé břečťanem a keři. "
  "Odvážíš se vylézt na břečťany, abys získal ametysty ukryté nahoře?\n\n"
  
  "Všechny políčka, která sousedí s Břečťanem, se berou jako stabilní (Břečťan ale nemůže šplhat po sobě ani po jiných Břečťanech)."
  )
  
N("big bush", GEN_O, "velký keř", "velké keře", "velký keř", "velkým keřem")

S("You can hold this bush to climb the Lost Mountain. "
  "Bushes block the movement of birds.",
  "Tohoto keře se můžeš chytit při výstupu na Ztracenou "
  "horu. Keře blokují pohyb ptáků."
  )

S("You can hold this bush to climb the Lost Mountain, "
  "but it is not very strong -- it will get destroyed "
  "if you climb from it into an unstable location. "
  "Bushes block the movement of birds.",
  
  "Tohoto keře se můžeš chytit při výstupu na Ztracenou horu, "
  "ale není příliš silný -- pokud z něj vylezeš na nestabilní "
  "lokaci, zničí se. Keře blokují pohyb ptáků.")

N("Amethyst", GEN_O, "Ametyst", "Ametysty", "Ametyst", "Ametystem")

S("A beatiful purple gem from the Lost Mountain.",
  "Krásný fialový kámen ze Ztracené hory.")

Orb("Nature", "Přírody")
N("Friendly Ivy", GEN_O, "Přátelský břečťan", "Přátelské břečťany", "Přátelský břečťan", "Přátelským břečťanem")

S("This Orb allows you to grow like an Ivy. "
  "The Ivy is always rooted in your current location; "
  "moving among the Ivy cells will move the root. "
  "Moving to a new location will cause the Ivy to grow "
  ", if an Ivy could make that movement "
  "(otherwise it breaks). "
  "You can also target one of the cells adjacent to your ivy "
  "(not to you) to grow or attack there.",
  
  "Tato Sféra ti umožňuje růst, jako bys byl Břečťan. Kořen Břečťanu je vždy na políčku, kde se právě nacházíš; pohybem mezi políčky Břečťanu se tento kořen přesune. Pokud se pohneš na nové místo, Břečťan tam vyroste, ale jen pokud by normální Břečťan mohl provést takový tah (jinak se přetrhne). Můžeš také zacílovat jedno z políček, které sousedí se tvým Břečťanem (ale ne s tebou), a rozrůst se tam nebo tam zaútočit."
  )

// reptiles
// ========

N("Reptiles", GEN_O, "Ještěrky", "Ještěrky", "Ještěrky", "na Ještěrkách")
N("Reptile", GEN_F, "Ještěrka", "Ještěrky", "Ještěrku", "Ještěrce")
N("Reptile floor", GEN_F, "Ještěrčí podlaha", "Ještěrščí podlahy", "Ještěrčí podlahu", "Ještěrčí podlahou")
N("Reptile bridge", GEN_O, "Ještěrčí most", "Ještěrčí mosty", "Ještěrčí most", "Ještěrčím mostem")

S("These reptiles are quite strange creatures. They "
  "spend most of their lives sleeping as floors "
  "that other creatures can walk on. "
  "Sometimes they wake up to hunt their prey, "
  "but they will happily go back to sleep if they "
  "happen to move into a hole on their way. "
  "Your attacks do not kill the Reptiles, but "
  "you can push and stun them.",
  
  "Tyto Ještěrky jsou opravdu podivní tvorové. Vštšinu života tráví "
  "tím, že spí ve formě podlahy, po které jiní tvorové chodí. Někdy "
  "se probudí, aby lovily kořist, ale pokud po cestě náhodou šlápnou "
  "do díry, ochotně zase usnou. Tvé útoky nemohou Ještěrky "
  "zabít, můžeš je ale strkat a omračovat."
  )

N("Dodecahedron", GEN_O, "Dvanáctistěn", "Dvanáctistěny", "Dvanáctistěn", "Dvanáctistěnem")
S("These dodecahedra made of a mysterious material are the Reptiles' favorite toy.",
  "Tyto dvanáctistěny z jakéhosi tajemného materiálu jsou oblíbenou hračkou Ještěrek.")

Orb("Vaulting", "Přeskoku")

S("This Orb allows you to jump over an adjacent monster, killing or stunning it. "
  "You can only vault in a roughly straight line. "
  "Target a cell on the other side to use it.",
  
  "Tato Sféra ti umožňuje přeskočit přes sousedního netvora, "
  "a tím ho zabít nebo omráčit. Vyběhnout můžeš pouze zhruba "
  "po přímce. Sféra se používá zacílováním políčka na druhé straně netvora."
  )

S("inventory/kill sorting", "řazení inventáře/netvorů")
S("first on top", "první nahoře")
S("first on bottom", "první dole")
S("last on top", "poslední nahoře")
S("last on bottom", "poslední dole")
S("by land", "podle kraje")
S("by number", "podle počtu")

N("small bush", GEN_O, "małý keřík", "małé keříky", "małý keřík", "małým keříkem")

/*
		"NEW_ACHIEVEMENT_7_24_NAME"	"Dostal ses na Ještěrky"
		"NEW_ACHIEVEMENT_7_24_DESC"	"Najdi a získej Dvanáctistěn."
		"NEW_ACHIEVEMENT_7_25_NAME"	"Ještěrka"
		"NEW_ACHIEVEMENT_7_25_DESC"	"Získej 10 Dvanáctistěnů."
		"NEW_ACHIEVEMENT_7_26_NAME"	"Aligátor"
		"NEW_ACHIEVEMENT_7_26_DESC"	"Získej 25 Dvanáctistěnů."
		"NEW_ACHIEVEMENT_7_27_NAME"	"Pán Ještěrek"
		"NEW_ACHIEVEMENT_7_27_DESC"	"Získej 50 Dvanáctistěnů."
		"NEW_ACHIEVEMENT_7_28_NAME"	"Dostal ses do Žaláře"
		"NEW_ACHIEVEMENT_7_28_DESC"	"Získej Hlenku."
		"NEW_ACHIEVEMENT_7_29_NAME"	"Netopýr"
		"NEW_ACHIEVEMENT_7_29_DESC"	"Získej 10 Hlenek."
		"NEW_ACHIEVEMENT_7_30_NAME"	"Kostlivec"
		"NEW_ACHIEVEMENT_7_30_DESC"	"Získej 25 Hlenek."
		"NEW_ACHIEVEMENT_7_31_NAME"	"Pán Žaláře"
		"NEW_ACHIEVEMENT_7_31_DESC"	"Získej 50 Hlenek."
		"NEW_ACHIEVEMENT_8_0_NAME"	"Horolezec"
		"NEW_ACHIEVEMENT_8_0_DESC"	"Najdi a seber Ametyst."
		"NEW_ACHIEVEMENT_8_1_NAME"	"Horský břečťan"
		"NEW_ACHIEVEMENT_8_1_DESC"	"Získej 10 Ametystů."
		"NEW_ACHIEVEMENT_8_2_NAME"	"Horský orel"
		"NEW_ACHIEVEMENT_8_2_DESC"	"Získej 50 Ametystů."
		"NEW_ACHIEVEMENT_8_3_NAME"	"Pán Hory"
		"NEW_ACHIEVEMENT_8_3_DESC"	"Získej 50 Ametystů."
*/

S("You feel something strange about gravity here...",
  "Cítíš, že na zdejší gravitaci je cosi podivného...")

S("The rug depicts a man in a deep dungeon, unable to leave.",
  "Na tomto koberci je vyobrazen muž v hlubokém žaláři, který nemůže ven.")

S("You feel attuned to gravity, ready to face mountains and dungeons.",
  "Cítíš se přizpůsoben%ý1 gravitaci, připraven%ý1 čelit horám a žalářům.")

// VERSION 9.4
// ===========

// not previously translated
S("shift+O to switch anti-aliasing", "Stiskem Shift+O se přepíná antialiasing")
S("For some reason... cannot attack!", "Z nějakého důvodu... nemůžeš útočit!")
S("Your energy swords get stronger!", "Tvé energetické meče sílí!")
S("You feel even more attuned to the magic of this land!",
  "Cítíš se ještě více přizpůsobený magii tohoto kraje!")
S("Quite tough, for your first fight.", "Na první bitku docela obtížný soupeř.")
S("(You can also use right Shift)\n\n", "(Také lze použít pravý Shift)\n\n")

// Crossroads V
// ------------

N("Crossroads V", GEN_O, "Křižovatka V", "Křižovatky V", "Křižovatku V", "na Křižovatce V")
S("Extremely narrow Crossroads layout.\n", "Velmi úzká verze Křižovatky.")

// Bull Dash/Prairie common
// ------------------------

N("Sleeping Bull", GEN_M, "Spící Býk", "Spící Býci", "Spícího Býka", "Spícím Býkem")

S("Sleeping bulls wake up when you get into distance of two cells from them.",
  "Spící býci se probudí, když se přiblížíš na dvě políčka od nich.")

N("Raging Bull", GEN_M, "Vzteklý Býk", "Vzteklí Býci", "Vzteklého Býka", "Vzteklým Býkem")

S("Raging Bulls charge in a straight line: on heptagons, when they can choose one of two possible directions, "
  "they choose one closer to your current location. In the case of a tie, the cell where more neighbors is "
  "closer to your current location is chosen; if still a tie, past locations are considered. "
  "They can attack you in any direction, and monsters on their way are attacked even if friendly. "
  "When they crash into something, the obstacle is usually destroyed, and they are stunned for three turns, after "
  "which they charge at you again (in any direction). "
  "Raging Bulls cannot be killed or stunned conventionally.",
  
  "Vzteklí Býci útočí po přímce: na sedmiúhelníku, kde si mohou vybrat jeden ze dvou směrů, si vždy zvolí ten, který "
  "je blíž k tobě. Pokud je to nastejno, vyberou si políčko s větším počtem sousedů bližších ke tvé pozici, "
  "a pokud je to stále nerozhodně, vezmou v úvahu minulé lokace. Býci mohou útočit ve všech směrech a na netvory, "
  "kteří jim stojí v cestě, útočí i tehdy, když jsou přátelští. Když Býk do něčeho narazí, obvykle tím překážku zničí a bude "
  "na tři kola omráčený; potom opět vyrazí proti tobě (v libovolném směru). Vzteklého Býka nelze zabít ani omráčit běžnými prostředky."
  )

N("Herd Bull", GEN_M, "Stádový Býk", "Stádoví Býci", "Stádového Býka", "Stádovým Býkem")

S("Herds of these Bulls are running long distances for some reason. They become Raging Bulls if something stops them.",
  "Sáda těchto Býků z nějakého důvodu běhají na dlouhé vzdálenosti. Pokud je něco zastaví, promění se ve Vzteklé Býky."
  )

N("Gadfly", GEN_M, "Ovád", "Ovádi", "Ováda", "Ovádem")
S("Annoying insects. They can awaken Sleeping Bulls.", "Otravný hmyz. Dokáže probudit Spící Býky.")
S("%The1 wakes up %the2.", "%1 probudil %a2.")

S("%The1 crashes into %the2!", "%1 narazil na %a2!")
S("Fire is extinguished!", "Oheň je uhašen!")
S("%The1 is filled!", "%1 je naplněn%1ý!")

// Prairie
// -------

N("Prairie", GEN_F, "Prérie", "Prérii", "Prérii", "na Prérii")
N("Green Grass", GEN_F, "Zelená Tráva", "Zelené Trávy", "Zelenou Trávu", "Zelenou Trávou")
S("You can find safety in some places in the Prairie, but if you want treasures, "
  "they can be found only on the other side of a giant herd of bulls.",
  "Na některých místech Prérie je bezpečno, ale pokud chceš poklady, musíš je hledat na druhé straně obrovského stáda býků."
  )

Orb("the Bull", "Býka")

S("You get the powers of Shield, Horns, and Thorns after you move two moves in a straight line "
  "with this Orb.",
  
  "Kdykoli se pohneš o dvě políčka rovně, získáš schopnosti Štítu, Rohů a Trnů.")

// Bull Dash
// ---------

N("Bull Dash", GEN_O, "Býčí kraj", "Býčí kraje", "Býčí kraj", "v Býčím kraji")
N("Butterfly", GEN_M, "Motýlek", "Motýlci", "Motýlka", "Motýlkem")
N("Spinel", GEN_O, "Spinel", "Spinely", "Spinel", "Spinelem")

S(
  "Butterflies don't pursue you -- unless you get next to them, they just spin around the obstacles. "
  "They cannot be killed conventionally, but you get treasure when a Raging Bull crashes into a Butterfly. ",

  "Motýlci tě nepronásledují -- pokud se nedostaneš těsně vedle nich, budou prostě jen poletovat kolem překážek. "
  "Motýlky nelze zabít běžným způsobem, ale když do nich vrazí Vzteklý býk, dostaneš poklad."
  )

Orb("Horns", "Rohů")

S(
  "After you move while having this Orb, you immediately attack the next cell in the straight line "
  "(or two cells, when moving on a heptagon). This attack is slightly stronger than your normal "
  "attack: it can stun some of the monsters which cannot be killed or stunned normally.",
  
  "Kdykoli se pohneš s touto Sférou, okamžitě zaútočíš na další políčko v řadě (nebo na dvě, "
  "pokud se pohneš na sedmiúhelník). Tento útok je o něco silnější než tvůj běžný útok: dokáže "
  "omráčit některé netvory, které nelze zabít ani omráčit běžným způsobem."
  )

S("You pierce %the1.", "Probodl jsi %a1.")

// new 3D options
// --------------

S("Y shift", "Y posuv")
S("Don't center on the player character.", "Necentrovat na hráče.")

S("camera rotation", "rotace kamery")
S("Rotate the camera. Can be used to obtain a first person perspective, "
  "or third person perspective when combined with Y shift.",
  "Rotace kamery. Lze ji použít k získání perspektivy první "
  "osoby, nebo (v kombinaci s Y posuvem) třetí osoby.")

S("ball model", "model koule")
S("hyperboloid model", "model hyperboloidu")

S("projection in ball model", "projekce v modelu koule")
S("This parameter affects the ball model the same way as the projection parameter affects the disk model.",
  "Tento parametr ovlivňuje model koule stejným způsobem jako projekce model disku.")
  

S("camera rotation in ball model", "rotace kamery v modelu koule")
S("Rotate the camera in ball/hyperboloid model.", "Rotace kamery v modelu koule/hyperboloidu.")

// extra help
// ----------

S("\nSpecial conduct (still valid)\n", "\nZvláštní výzva (stále platná):")
S("\nSpecial conduct failed:\n", "\nZvláštní výzva (neúspěšná):")
S("Avoid escaping from a discharge (\"That was close\").",
  "Nesmíš se vyhnout výboji (\"To bylo těsně\").")
S("Avoid chopping trees, using Orbs, and non-graveyard monsters in the Haunted Woods.",
  "Nesmíš ve Strašidelném lese kácet stromy a používat Sféry nebo netvory, kteří nepocházejí ze Hřbitova.")
S("\n\nEasy %1 might disappear when you collect more of its kind.",
  "\n\nSnadn%ý1 %1 může zmizet, když jich sebereš více.")
S(" You need to go deep to collect lots of them.", "Abys jich získa%l0 hodně, musíš jít hluboko do tohoto kraje.")

S("\nSpawn rate (as prize Orb): %1%/%2\n", "\nFrekvence (jako extra Sféra): %1%/%2\n")
S("\nSpawn rate (in Hubs): %1%/%2\n", "\nFrekvence (v centrálních krajích): %1%/%2\n")

S("Orbs of Safety always appear here, and may be used to escape.\n",
  "Sféry Bezpečí se tu dají vždy najít a můžeš je použít k úniku.")
  
S("You may be unable to leave %the1 if you are not careful!\n",
  "Pokud si nedáš pozor, možná už se odsud nedostaneš!")

N("Cellular Automaton", GEN_M, "Buněčný automat", "Buněčné automaty", "Buněčný automat", "v Buněčném automatu")

S("A land for people wanting to experiment with cellular automata in the HyperRogue grid. "
  "Rules can be given on the command line; the default rules are:\n"
  "-c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "(-c0 or -c1 can be given if the same rule is to be used for hexagonal "
  "and heptagonal cells).",
  "Kraj pro ty, kdo by chtěli experimentovat s buněčnými automaty na mřížce HyperRogue. "
  "Pravidla lze zadávat v příkazové řádce; standardně jsou to\n"
  "-c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "(také lze zadat -c0 nebo -c1, pokud se má použít stejné pravidlo pro šestiúhelníky a sedmiúhelníky).")

S("compass size", "velikost kompasu")

/*
                "NEW_ACHIEVEMENT_8_10_NAME"     "Na druhé straně"
                "NEW_ACHIEVEMENT_8_10_DESC"     "Najdi a získej Zelenou Trávu."
		"NEW_ACHIEVEMENT_8_11_NAME"	"Stádový Býk"
		"NEW_ACHIEVEMENT_8_11_DESC"	"Získej 10 Zelených Trav."
		"NEW_ACHIEVEMENT_8_12_NAME"	"Vzteklý Býk"
		"NEW_ACHIEVEMENT_8_12_DESC"	"Získej 25 Zelených Trav."
		"NEW_ACHIEVEMENT_8_13_NAME"	"Pán Prérie"
		"NEW_ACHIEVEMENT_8_13_DESC"	"Získej 50 Zelených Trav."
		"NEW_ACHIEVEMENT_8_14_NAME"	"Spcí Býk"
		"NEW_ACHIEVEMENT_8_14_DESC"	"Získej Spinel."
		"NEW_ACHIEVEMENT_8_15_NAME"	"Ovád"
		"NEW_ACHIEVEMENT_8_15_DESC"	"Získej 10 Spinelů."
		"NEW_ACHIEVEMENT_8_16_NAME"	"Motýlek"
		"NEW_ACHIEVEMENT_8_16_DESC"	"Získej 25 Spinelů."
		"NEW_ACHIEVEMENT_8_17_NAME"	"Pán Býčího kraje"
		"NEW_ACHIEVEMENT_8_17_DESC"	"Získej 25 Spinelů."
		"NEW_ACHIEVEMENT_8_18_NAME"	"Červ světa"
		"NEW_ACHIEVEMENT_8_18_DESC"	"Poraž Písečného červa v geometrii Zebra kvocientu."
*/

N("Tortoise", GEN_F, "Želva", "Želvy", "Želva", "Želvou")
S("line patterns", "vzory čar")
S("1 turn", "1 kolo")
S("%1 turns", "kola: %1")
S("inventory/kill mode", "mód předmětů/zabitých netvorů")
S("images", "obrázky")
S("letters", "písmena")
S("input", "vstup")
S("default", "default")
S("config", "konfigurace")
S("expansion", "rozšíření")

S("continue", "pokračuj")
S("main menu", "hlavní menu")
S("restart", "nová hra")
S("quit", "konec")
S("exit menu", "opusť menu")

S(
  "A tourist from another world. They mutter something about the 'tutorial', "
  "and claim that they are here just to learn, and to leave without any treasures. "
  "Do not kill them!",
  
  "Turista z jiného světa. Mumlá něco o 'tutoriálu' a tvrdí, že se jenom učí "
  "a odejde bez pokladů. Nezabíjet!")

S(
  "This monster has come from another world, presumably to steal our treasures. "
  "Not as fast as an Eagle, not as resilient as the guards from the Palace, "
  "and not as huge as the Mutant Ivy from the Clearing; however, "
  "they are very dangerous because of their intelligence, "
  "and access to magical powers.\n\n",

  "Tento netvor přišel z jiného světa, asi proto, aby nám ukradl poklady. "
  "Není tak rychlý jako Orel, není tak odolný jako stráže z Paláce, a není "
  "tak obrovský jako Mutantní břečtan z Mýtiny; díky své inteligenci a přístupu "
  "k magickým schopnostem je však velmi nebezpečný.\n\n"
  )

S(
  "Actually, their powers appear god-like...\n\n",
  "Vlastně se zdá, že má až božskou moc...\n\n"
  )

S(
  "Rogues will never make moves which result in their immediate death. "
  "Even when cornered, they are able to instantly teleport back to their "
  "home world at any moment, taking the treasures forever... but "
  "at least they will not steal anything further!\n\n",
  
  "Chytráci nikdy neudělají tah, který by vedl k jejich okamžité smrti. I když "
  "jsou zahnaní do kouta, dokážou se kdykoli okamžitě teleportovat do svého "
  "domovského světa a navěky s sebou odnést poklady... ale aspoň už nebudou "
  "krást nic dalšího!\n\n")

S(
  "Despite this intelligence, Rogues appear extremely surprised "
  "by the most basic facts about geometry. They must come from "
  "some really strange world.\n\n",
  
  "Navzdory jejich inteligenci se zdá, že Chytráky dokážou nesmírně překvapit "
  "i ta nejzákladnější geometrická fakta. Musejí pocházet z nějakého opravdu"
  "podivného světa.")

S("change the alpha parameter to show the lines",
  "čáry můžeš zviditelnit změnou parametru alfa")

S("triangle grid: not rings", "trojúhelníková mřížka: bez prstenců")
S("triangle grid: rings", "trojúhelníková mřížka: s prstenci")
S("heptagonal grid", "sedmiúhelníková mřížka")
S("rhombic tesselation", "kosočtvercové dělení")
S("triheptagonal tesselation", "troj-sedmiúhelníkové dělení")
S("normal tesselation", "normální dělení")
S("big triangular grid", "velká trojúhelníková mřížka")
S("underlying tree", "stromová základní struktura")
S("circle/horocycle tree", "kruhový/horocyklický strom")
S("zebra triangles", "zebra: trojúhelníky")
S("zebra lines", "zebra: čáry")
S("vineyard pattern", "vinicový vzor")
S("firewall lines", "čáry ohnivých stěn")
S("firewall lines: Palace", "čáry ohnivých stěn: Palác")
S("firewall lines: Power", "čáry ohnivých stěn: Moc")

S("(ESC) tour menu", "(ESC) menu prohlídky")
S("Try the Guided Tour to help with understanding the "
  "geometry of HyperRogue (menu -> special modes).\n\n",
  "Zkuste Tutoriál, chcete-li pomoci s chápáním geometrie "
  "HyperRogue (menu -> speciální módy.\n\n")

S("guided tour", "Tutoriál")
S("spherical geometry", "sférická geometrie")
S("Euclidean geometry", "eukleidovská geometrie")
S("more curved hyperbolic geometry", "zakřivenější hyperbolická geometrie")
S("teleport away", "teleportuj se pryč")
S("Shift-click a location to teleport there.", "Shift-klikni na políčko, kam se chceš teleportovat.")
S("Click a location to teleport there.", "Klikni na políčko, kam se chceš teleportovat.")

S("move through walls", "procházení zdmi")
S("flash", "záblesk")
S("static mode", "statický mód")
S("Static mode enabled.", "Statický mód zapnut.")
S("Static mode disabled.", "Statický mód vypnut.")
S("enable/disable texts", "zapnout/vypnout texty")
S("Help texts disabled.", "Texty nápovědy vypnuty.")
S("next slide", "další snímek")
S("previous slide", "předešlý snímek")

S("This guided tour is different than most other game tutorials -- "
  "you are not forced to do anything, and you can go wherever you want.\n\n"
  "However, %the1 is not what we are talking about now. "
  "We will not explain this land at the moment, and you could potentially "
  "get lost there.\n\n"
  "Remember that you can get to the next slide by pressing Enter.",
  
  "Tento tutoriál funguje jinak, než ve většině her -- k ničemu tě nenutíme "
  "a můžeš si jít, kam chceš.\n\n"
  "Ale %1 není to, o čem teď mluvíme. Tento kraj momentálně tutoriál nevysvětluje "
  "a je možné, že se v něm ztratíš.\n\n"
  "Nezapomeň, že stiskem klávesy Enter můžeš přejít na další snímek.")

S("Introduction", "Úvod")
S("Welcome to the HyperRogue Guided Tour!", "Vítej v tutoriálu HyperRogue!")
S(
  "This tour is mostly aimed to show what is "
  "special about the geometry used by HyperRogue. "
  "It also shows the basics of gameplay, and "
  "how is it affected by geometry.\n\n"
  "You decide when you want to stop playing with the "
  "current \"slide\" and go to the next one, by pressing Enter. You can also "
  "press ESC to see a "
  "menu with other options.",
  
  "Tento tutoriál je převážně zaměřen na to, aby ukázal, co je zvláštního "
  "na geometrii, kterou HyperRogue používá. Také ukazuje základy hry a to, "
  "jak ji geometrie ovlivňuje.\n\n"
  "Když si budeš chtít přestat hrát se současným \"snímkem\" a přejít na další, "
  "stiskni Enter. Stiskem ESC můžeš také zobrazit menu s dalšími možnostmi.")
// FIX: tutorial changed to tour

S("Basics of gameplay", "Základy hry")
S("gain Ice Diamonds", "získej Ledové diamanty")
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
  "Hra začíná v Ledovém kraji. Sbírej Ledové diamanty (pokud nevíš, jak "
  "se pohybovat, stiskni F1). Když jich nasbíráš mnoho, netvoři začnou "
  "být nebezpeční.\n"
  "Jak je zvykem ve hrách žánru roguelike a v jiných, které jsou založené "
  "na taktických dovednostech a nikoli na příběhu, pokud prohraješ, budeš "
  "muset začít hrát od začátku. V tomto tutoriálu se ale můžeš z ošklivé "
  "situace jednoduše teleportovat stisknutím klávesy '4'. Tutoriál je "
  "obecně dělaný tak, aby ti mohl snadno ukázat, co chce -- v tomto snímku "
  "můžeš například stisknout klávesu '5', abys rychle získal velké množství "
  "Ledových diamantů.")

S("Hypersian Rug model", "Model hyperského koberce")
S(
  "New players think that the action of HyperRogue takes place on a sphere. "
  "This is not true -- the next slide will show the surface HyperRogue "
  "actually takes place on.\n\n"
  "Use arrow keys to rotate the model, and Page Up/Down to zoom.\n\n"
  "If you do not see anything, press '5' to try a safer renderer.",
  

  "Noví hráči si často myslí, že se HyperRogue odehrává na povrchu koule. "
  "Není to pravda -- další snímek ti ukáže povrch, na jakém se "
  "HyperRogue opravdu odehrává.\n\n"
  "Model můžeš otáčet šipkami a zoomovat klávesami Page Up/Down.\n\n"
  "Pokud nic nevidíš, stiskni klávesu '5' pro aktivaci bezpečnějšího rendereru."
  )

S("Expansion", "Expanze")
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
  
  "Další snímek ukazuje počet políček ve vzdálenosti 1, 2, 3, ... od tebe. "
  "Roste exponenciálně: v kruhu o poloměru 1000 kolem tebe je více než 10^100 "
  "políček, a během hry dojdeš ještě dál!\n\n"
  "Tento fakt je pro design HyperRogue velmi důležitý. Hra obsahuje mnoho "
  "navigačních hádanek -- to, co by v eukleidovském světě bylo jednoduché, "
  "je v hyperbolické geometrii extrémně složité (chceš se dostat na určité "
  "místo 20 políček daleko -- kterým z tisíců možných směrů se máš vydat?); "
  "na druhou stranu, jiné věci, které jsou v eukleidovském světě prakticky "
  "nemožné, jsou v HyperRogue snadné. HyperRogue musel být speciálně navržen "
  "tak, aby se v jeho nekonečném světě nedalo grindovat. Nejsou v něm téměř "
  "žádné trvalé upgrady; sbírání pokladů ti přináší výhody, ale pokud se "
  "pokusíš získat příliš mnoho pokladů jednoho druhu, bude to nesmírně nebezpečné.")

S("Tiling and Tactics", "Hyperbolické dělení a taktika")
S(
  "The tactics of fighting simple monsters, such as the Yetis from the Icy Lands, "
  "might appear shallow, but hyperbolic geometry is essential even there. "
  "In the next slide, you are attacked by two monsters at once. "
  "You can make them line up simply by "
  "running away in a straight line. "
  "Press '2' to try the same in the Euclidean world -- it is impossible.",
  "Taktika boje proti jednoduchým netvorům, jako jsou například Yetiové "
  "z Ledového kraje, může vypadat jednoduše, ale i tam je hyperbolická "
  "geometrie zásadní. Na dalším snímku na tebe útočí dva netvoři najednou. "
  "Pokud budeš utíkat po přímce, přinutíš je, aby se seřadili. Stiskni "
  "klávesu '2', abys to samé zkusil v eukleidovském světě -- je to nemožné.")

S("Straight Lines", "Přímky")
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
  
  "Hyperbolickou geometrii objevili matematikové z 19. století, kteří "
  "přemýšleli o povaze rovnoběžnosti. Máš-li přímku p a bod A, může "
  "existovat svět, ve kterém bodem A prochází více než jedna přímka, "
  "která neprotíná p?\n\n"
  "Ledový kraj bude velmi nebezpečný, pokud máte hodně Ledových "
  "diamantů -- potom tě bude honit spousta Yetiů a Ledových vlků! "
  "Ale ostatní kraje, ve kterých ještě žádné poklady nemáš, budou "
  "stále bezpečné (relativně).\n\n"
  "Půjdeš-li dál, měl bys rychle narazit na Křižovatku -- Velké zdi "
  "jsou přímky a skutečně fungují jinak než v eukleidovské geometrii. "
  "Na druhé straně Velkých zdí uvidíš jiné kraje -- HyperRogue obsahuje "
  "zhruba 60 krajů založených na různých mechanikách a aspektech "
  "hyperbolické geometrie.")

S("Running Dogs", "Běžící psi")
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
  "Chceš-li se dozvědět víc o přímkách, běž dál a měl bys najít Zemi "
  "věčného pohybu. Pokus se tam běžet po přímce, zatímco vedle tebe běží "
  "Běžící pes. I když běží stejně rychle jako ty, bude se zdát, že je "
  "pomalejší -- to proto, že ty běžíš po přímce, zatímco Běžící pes musí "
  "běžet po křivce zvané 'ekvidistanta'.\n\n"
IF_MAC(
  "Nezapomeň, že pokud na cokoli klikneš s pravým shiftem, můžeš o tom "
  "získat víc informací."
,
  "Nezapomeň, že pokud na cokoli klikneš pravým tlačítkem, můžeš o tom "
  "získat víc informací."
)
  )

S("Equidistants", "Ekvidistanty")
S(
  "Equidistants are curves which are at some fixed distance from a "
  "straight line. Some lands in HyperRogue are based on equidistants; "
  "you should see them after wandering a bit more.\n\n"
  "This tutorial gives you freedom to go wherever you choose, "
  "but we do not recommend going deep into the Dungeon or the Ocean -- "
  "getting back might be difficult.",
  
  "Ekvidistanty jsou křivky, které mají určitou danou vzdálenost od nějaké "
  "přímky. V HyperRogue jsou kraje, které jsou na ekvidistantách založeny; "
  "když budeš putovat dál, měl bys na ně narazit.\n\n"
  "V tomto tutoriálu se můžeš vydat, kam se ti zlíbí, ale doporučujeme, abys "
  "nechodil příliš hluboko do Žaláře nebo do Oceánu -- mohl bys mít problém "
  "vrátit se zpátky.")

S("Circles", "Kružnice")
S(
  "Circles are strange in hyperbolic geometry too. "
  "Look for the Castle of Camelot in the Crossroads; "
  "the Round Table inside is a circle of radius 28. "
  "Finding its center is a difficult challenge.\n\n"
  "Press '5' to cheat by seeing the smaller circles too.\n\n"
  "Note: Camelot and some other lands are unlocked earlier in the Tutorial than in a real game.",

  "I kružnice jsou v hyperbolické geometrii podivné. Zkus na Křižovatce najít "
  "hrad Kamelot; Kulatý stůl uvnitř je kružnice o poloměru 28. Najít její střed "
  "je obtížný úkol.\n\n"
  "Stiskem klávesy '5' můžeš podvádět a vidět i menší kružnice.\n\n"
  "Poznámka: Camelot a některé další kraje se v Tutoriálu odemykají "
  "dříve než ve skutečné hře.")

S("Horocycles", "Horocykly")
S("Horocycles are similar to circles, but you cannot reach their center at all -- "
  "they can be understood as limit circles of infinite radius centered in some point "
  "in infinity (also called an ideal point).\n\n"
  "Go to R'Lyeh, and you should quickly find a Temple of Cthulhu there. "
  "Each circle of columns is actually a horocycle. Horocycles in a given "
  "temple are concentric, and there is an infinite number of them.",

  "Horocykly se podobají kružnicím, ale jejich středu vůbec nelze dosáhnout "
  "-- můžeš si je představit jako limitní kružnice s nekonečným poloměrem, "
  "jejichž střed je v nějakém bodě v nekonečnu (takzvaném ideálním bodě).\n\n"
  "Běž do R'Lyeh. Měl bys tam rychle najít Cthulhuův chrám. Každý kruh ze "
  "sloupů je ve skutečnosti horocyklus. Horocykly v každém chrámu jsou "
  "soustředné a je jich nekonečně mnoho.")

S("Half-plane model", "Polorovinový model")
S("The game is normally displayed in the so called Poincaré disk model, "
  "which is a kind of a map of the infinite hyperbolic world. "
  "There are many projections of Earth, but since Earth is curved, "
  "all of them have to distort distances or angles in some way -- "
  "the same is true in hyperbolic geometry. "
  "The next slide shows another model, called the Poincaré upper half-plane model. In this model, "
  "horocycles centered at one specific ideal point are drawn as straight lines.",
  
  "Hra se normálně zobrazuje v modelu nazývaném 'Poincarého disk', což je něco "
  "jako mapa nekonečného hyperbolického světa. Země má také mnoho projekcí, "
  "ale protože je zakřivená, musejí tyto projekce vždy nějakým způsobem "
  "zkreslovat vzdálenosti nebo úhly -- a v hyperbolické geometrii platí totéž. "
  "Další snímek ti ukáže jiný model, zvaný 'Poincarého model horní poloroviny'. "
  "V tomto modelu vypadají horocykly se středem v jednom specifickém ideálním "
  "bodě jako přímky.")

S("Curvature", "Zakřivení")
S("gain Orb of the Sword", "získej Sféru meče")
S(
  "Now, go to the Burial Grounds and find an Orb of the Sword. The Sword appears to "
  "always be facing in the same direction whatever you do, and it appears that "
  "you have to rotate the sword to excavate the treasures; "
  "yet, it is possible to excavate them! You might have already noticed "
  "that the world rotates after you move around a loop and return to an old "
  "place.\n\n"
  "This is related to the fact that the world of HyperRogue is curved, and "
  "the sum of angles in a triangle is not equal to 180 degrees.",

  "Teď se vydej na Pohřebiště a najdi Sféru meče. Ať děláš, co děláš, Meč bude "
  "vždy otočený stejným směrem, a přitom se zdá, že abys mohl vykopat poklady, "
  "musíš Meč natočit; přesto je však možné je vykopat! Možná už sis všiml, že "
  "když se pohybuješ po uzavřené křivce a vrátíš se tam, odkud jsi vyšel, "
  "bude svět pootočený.\n\n"
  "To souvisí s faktem, že svět HyperRogue je zakřivený a součet úhlů v trojúhelníku se nerovná 180 stupňům.")

S("Periodic patterns", "Periodické vzory")
S("Hyperbolic geometry yields much more interesting periodic patterns "
  "than Euclidean.",
  "V hyperbolické geometrii lze najít mnohem zajímavější "
  "periodické vzory než v eukleidovské.")

S("Periodic patterns: application", "Periodické vzory: aplikace")
S("Many lands in HyperRogue are based around periodic patterns. "
  "For example, both Zebra and Windy Plains are based on the pattern "
  "shown in the previous slide. "
  "Such lands often have tree-like nature.",
  "HyperRogue obsahuje řadu krajů, které jsou založené na periodických "
  "vzorech. Tak například Zebra a Větrné pláně jsou obě založené na vzoru "
  "na předešlém snímku. Tyto kraje mívají často stromovou povahu.")

S("Fractal landscapes", "Fraktální krajinky")
S("On the following slide, the colors change smoothly in the whole infinite world. "
  "Again, this works better than in Euclidean geometry.",
  "Na následujícím snímku se barvy v celém nekonečném světě hladce "
  "mění. I v tomto případě to v hyperbolické geometrii funguje lépe než "
  "v eukleidovské.")

S("Fractal landscapes: application", "Fraktální krajinky: aplikace")
S("create a baby tortoise", "vytvoř želvičku")
S(
  "This is applied in HyperRogue to create landscapes, such as the chasms in the "
  "land of Reptiles or the Dragon Chasms, which you should find quickly. "
  "Also in the Dragon Chasms, you can find a Baby Tortoise, and try to find "
  "a matching adult tortoise in the Galápagos. "
  "There are over two millions of species, but since there is so much space in "
  "hyperbolic geometry, finding a matching tortoise is possible. The brighter "
  "the color in Galápagos is, the more aspects of the tortoises in the given "
  "area are matching.",
  
  "HyperRogue tohoto využívá pro vytváření krajin, jako jsou propasti v zemi "
  "Ještěrek nebo v Dračích propastech, které bys měl rychle najít. V Dračích "
  "propastech můžeš také najít Želvičku, a potom se pokusit na Galapágách "
  "najít dospělou želvu, která jí odpovídá. Existuje více než dva milióny "
  "druhů želv, ale protože v hyperbolické geometrii je tolik místa, je možné "
  "najít tu, která patří ke tvé Želvičce. Čím jasnější barva, tím více aspektů "
  "želv v dané oblasti odpovídá tvé Želvičce.")

S("Poincaré Ball model", "Model Poincarého koule")
S(
    "The Poincaré disk model is a model of a hyperbolic *plane* -- you "
    "might wonder why are the walls rendered in 3D then.\n\n"
    "HyperRogue actually assumes that the floor level is an equidistant surface "
    "in a three-dimensional hyperbolic world, and the camera is placed above the "
    "plane that the surface is equidistant to (which boils down to showing "
    "the floor level in Poincaré disk model).\n\n"
    "This is shown on the next slide, in the Poincaré ball model, which is "
    "the 3D analog of the Poincaré disk model.",
    
  "Poincarého disk je model hyperbolické *roviny* -- možná si tedy říkáš, "
  "proč se zdi zobrazují ve 3D.\n\n"
  "HyperRogue ve skutečnosti předpokládá, že podlaha je ekvidistantní plocha "
  "ve trojrozměrném hyperbolickém světě, a že kamera se nachází nad rovinou, "
  "ke které je tato plocha ekvidistantní (což ve výsledku znamená, že se "
  "podlaha zobrazuje jako Poincarého disk).\n\n"
  "To můžeš vidět v následujícím snímku, který ukazuje Poincarého kouli "
  "-- 3D analog Poincarého disku.")

S("Hyperboloid model", "Hyperboloidový model")
S(
    "Let's see more models of the hyperbolic plane. "
    "This model uses a hyperboloid in the Minkowski geometry; "
    "it is used internally by HyperRogue.",
    
    "Podívejme se na další modely hyperbolické roviny. Tento model "
  "používá hyperboloid v Minkowského geometrii; právě tento model "
  "HyperRogue používá interně.")

S("Beltrami-Klein model", "Beltrami-Kleinův model")
S(
    "This model renders straight lines as straight, but it distorts angles.",
    "Tento model zobrazuje přímky jako přímky, ale zkresluje úhly.")

S("Gans model", "Gansův model")
S(
  "Yet another model, which corresponds to orthographic projection of the "
  "sphere. Poincaré disk model, Beltrami-Klein model, and the Gans "
  "model are all obtained by looking at either the hyperboloid model or an "
  "equidistant surface from various distances.",
  "A tady máme další model, který odpovídá ortografické projekci koule. Poincerého disk, Beltrami-Kleinův model i Gansův model se získají tak, že se z různých vzdáleností díváte na hyperboloidový model nebo na ekvidistantní plochu.")

S("Band model", "Pásový model")
S("render spiral", "renderování spirály")
S(
  "The band model is the hyperbolic analog of the Mercator projection of the sphere: "
  "a given straight line is rendered as a straight line, and the rest of the "
  "world is mapped conformally, that is, angles are not distorted. "
  "Here, we take the straight line connecting your starting point and your "
  "current position -- usually the path taken by the player is surprisingly "
  "close to a straight line. Press '8' to see this path.\n\n"
  "If you want, press '5' to see it rendered as a spiral, although it takes lots of time and "
  "memory.",
  "Pásový model je hyperbolickým analogem Mercatorovy projekce na kouli: "
  "jedna daná přímka se zobrazí jako přímka a zbytek světa je namapován "
  "konformálně, tedy bez zkreslení úhlů. V tomto případě použijeme přímku, "
  "která spojuje bod, kde jsi začal hru, a tvou současnou polohu -- dráha, "
  "po které hráč jde, je obvykle překvapivě blízká přímce. Tuto cestu můžeš "
  "vidět stisknutím klávesy '8'.\n\n"
  "Pokud chceš, můžeš ji také stisknutím klávesy '5' vyrenderovat jako spirálu; "
  "vyžaduje to však hodně času a paměti.")

S("Conformal square model", "Konformální čtvercový model")
S("The world can be mapped conformally to a square too.",
  "Svět je také možné konformálně namapovat na čtverec.")

S("Shoot'em up mode", "Mód střílečky")
S("In the shoot'em up mode, space and time is continuous. "
  "You attack by throwing knives. "
  "Very fun with two players!\n\n"
  "There are other special modes too which change the gameplay or "
  "focus on a particular challenge.",
  "V módu střílečky je prostor i čas spojitý. Útočíš "
  "vrháním nožů. Veliká legrace ve dvou hráčích!\n\n"
  "HyperRogue také obsahuje další speciální módy, které mění "
  "způsob hry nebo jsou zaměřené na určitý úkol.")

S("THE END", "KONEC")
S("leave the tour mode", "opusť Tutoriál")
S(
  "This tour shows just a small part of what you can see in the world of HyperRogue. "
  "For example, "
  "hyperbolic mazes are much nicer than their Euclidean counterparts. "
  "Have fun exploring!\n\n"
  "Press '5' to leave the tour mode.",

  "Tato prohlídka ukazuje jen malou část toho, co můžeš vidět ve světě "
  "HyperRogue. Tak například: hyperbolická bludiště jsou mnohem hezčí "
  "než jejich eukleidovské protějšky. Bav se při dalším zkoumání!\n\n."
  "Stisknutím klávesy '5' opustíš tutoriál."
  )

// Orb Strategy mode
// -----------------

S("Orb Strategy mode", "sférostrategický mód")

S(
  "You are playing in the Orb Strategy Mode. Collecting treasure "
  "gives you access to magical Orb powers. In this mode, "
  "unlocking requirements are generally higher, and "
  "several quests and lands "
  "give you extremely powerful Orbs of the Mirror.\n",

  "Právì hraješ ve sférostrategickém módu. Sbírání pokladù ti dá pøístup "
  "k magickým silám Sfér. V tomto módu jsou požadavky na odemykání nových "
  "vìcí obecnì vyšší a nìkolik úkolù a krajù ti dává nesmírnì silné Sféry "
  "Zrcadla.\n")

S("The treasure gives your magical powers!", "Poklady ti dávají magickou sílu!")
S("Press 'i' to access your magical powers.", "Stiskem klávesy 'i' pøejdeš na svou magickou sílu.")
S("inventory", "Tvé Sféry")
S("mirror what?", "co chceš zrcadlit?")
S("Which orb to use?", "Kterou Sféru chceš použít?")
S("Unlocked by: %1 in %2", "Odemèeno pøi: %1 %abl2")
S(" (next at %1)", " (další pøi %1)")
S(" (next at %1 to %2)", " (další pøi %1 až %2)")
S("Number of uses left: %1", "Poèet zbývajících použití: %1")
S("You mirror %the1.", "Zrcadlíš %a1.")
S("You need to stand next to a magic mirror or cloud to use %the1.", 
  "Abys mohl použít %a1, musíš stát vedle kouzelného zrcadla nebo oblaku.")
S("Each orb type can be mirrored only once.", "Každý typ Sféry lze zrcadlit pouze jednou.")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Yendor appear in Hell after "
  "you collect 25 Demon Daisies in Hell, in Crossroads/Ocean after you collect 50, "
  "and everywhere after you collect 100.",

  "\n\nVe sférostrategickém módu se Yendorské Sféry objevují v Pekle poté, "
  "co tam získáš 25 Èertových kvítek. Po získání 50 Èertových kvítek se "
  "zaènou objevovat na Køižovatce a v Oceánu, a po získání 100 Èertových "
  "kvítek všude."
  )  

S(
  "\n\nIn the Orb Strategy Mode, dead orbs are available once you collect "
  "10 Necromancer Totems in the Graveyard.",
  
  "\n\nMrtvé Sféry se ve sférostrategickém módu objevují poté, co získáš "
  "10 Nekromancerových totemù na Høbitovì.")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Safety can be gained by "
  "collecting Phoenix Feathers in the Land of Eternal Motion. "
  "You can also find unlimited Orbs of Safety in the Crossroads "
  "and the Ocean (after collecting 25 Phoenix Feathers) "
  "and in the Prairie.",
  
  "\n\nVe sférostrategickém módu mùžeš získat Sféry Bezpeènosti sbíráním "
  "Fénixových pírek v Kraji vìèného pohybu. Také jich mùžeš najít neomezenì "
  "mnoho na Køižovatce a v Oceánu (ale musíš nejprve získat 25 Fénixových "
  "pírek) a na Prérii."
  )

S(
  "\n\nCollect %the1 to gain an extra Orb of the Mirror. "
  "You can gain further Orbs of the Mirror by collecting 2, 4, 8...",

  "\n\nKdyž získáš %a1, dostaneš další Sféru Zrcadla. "
  "Další Sféry Zrcadla pak mùžeš získat za 2, 4, 8..."
  )

S(
  "\n\nIn the Orb Strategy Mode, the Orb of Luck also "
  "significantly increases the frequency of Great Walls, Crossroads IV, "
  "and sub-lands.",
  
  "\n\nVe sférostrategickém módu Sféra Štìstí také výraznì zvyšuje frekvenci Velkých zdí, Køižovatek IV a podkrajù.")

S("\n\nIn the Orb Strategy Mode, each 25 Necromancer's Totems "
  "you are given a random offensive Orb.",
  
  "\n\nVe sférostrategickém módu ti každých 25 Nekromancerových "
  "totemù pøidá jednu náhodnou útoènou Sféru.")
 
S(
  "Use Orb of the Mirror to gain copies of one of your Orbs; "
  "mirroring weaker Orbs usually yields more copies. "
  "It can only be used once per Orb type, "
  "and only when you are next to a mirror.",
  
  "Sféru Zrcadla mùžeš použít ke kopírování ostatních Sfér; ze slabších "
  "Sfér obvykle získáš více kopií. Toto lze s každým typem Sféry udìlat "
  "pouze jednou, a pouze tehdy, když stojíš vedle zrcadla.")
  
S("Uses to gain: %1", "Získáš použití: %1")
S("already mirrored", "již zrcadleno")
  
N("your orbs", GEN_F, "Tvé Sféry", "Tvé Sféry", "Tvé Sféry", "Tvé Sféry")
S("Click this to see your orbs.", "Kliknutím zde zobrazíš své Sféry.")                    

// peaceful mode
// -------------

S("configure keys/joysticks", "konfigurace kláves/joysticku")
S("peaceful mode", "klidný mód")

// config changes
S("Press F5 or 'o' to try again!", "Stiskem F5 nebo 'o' to mùžeš zkusit znovu!")
S("aura brightness", "jas aury")
S("aura smoothening factor", "vyhlazovací faktor aury")
S("graphics configuration", "konfigurace grafiky")
S("special display modes", "zvláštní zobrazovací módy")
S("openGL mode", "mód OpenGL")
S("anti-aliasing", "anti-aliasing")
S("line width", "šíøka èar")
S("configure panning and general keys", "konfigurace pøejíždìní a obecných kláves")

S("\n\nHint: use 'm' to toggle cells quickly",
  "\n\nTip: políèka lze rychle pøepínat klávesou 'm'")

// cell pattern names
S("football", "fotbal")
S("dark rainbow landscape", "temná duha")
S("field pattern", "vzor pole")
S("field pattern C", "vzor pole C")
S("field pattern D", "vzor pole D")
S("field pattern N", "vzor pole N")
S("field pattern S", "vzor pole S")
S("four triangles", "ètyøi trojúhelníky")
S("big triangles: rings", "velké trojúhelníky: prstence")

// missing for the Tutorial
S("guided tour", "tutoriál")
S("This Orb is not compatible with the Tutorial.", "Tato Sféra není kompatibilní s tutoriálem.")

// local scores
S("turns", "kol")
S("cells", "políèek")
S("sort", "tøídìní")
S("choose", "výbìr")
S("play", "hraj")

// draw editor
S("autochoose", "autovýbìr")
S("c = choose", "c = výbìr")
S("b = switch auto", "b = pøepni auto")

// mission screen hints
// --------------------

S(
  "If you collect too many treasures in a given land, it will become "
  "extremely dangerous. Try other lands once you have enough!",
  "Pokud získáš pøíliš mnoho pokladù v jednom kraji, zaène být velmi "
  "nebezpeèný. Až budeš mít dost, zkus jiné kraje!")

S(
  "Remember that you can right click almost anything for more information.",
  "Nezapomeò, že témìø na všechno mùžeš kliknout pravým tlaèítkem a získat "
  "o tom více informací.")

S("Want to understand the geometry in HyperRogue? Try the Guided Tour!",
  "Chceš porozumìt geometrii v HyperRogue? Zkus Tutoriál!")

S(
  "Collecting 25 treasures in a given land may be dangerous, "
  "but allows magical Orbs of this land to appear in other places!",
  "Získat 25 pokladù v jednom kraji mùže být nebezpeèné, ale zpùsobí "
  "to, že se magické Sféry z tohoto kraje zaènou objevovat i v jiných!")

S(
  "Press ESC to view this screen during the game.",
  "Tuto obrazovku mùžeš bìhem hry zobrazit stiskem ESC.")

S("The 'world overview' shows all the lands in HyperRogue.",
  "'Pøehled krajù' ti ukáže všechny kraje v HyperRogue."
  )

S("Press 'o' to see all the lands in HyperRogue.",
  "Stiskem 'o', zobrazíš všechny kraje v HyperRogue.")

S(
  "Want another type of game? Want more challenge?\n"
  "HyperRogue has many special modes and challenges that "
  "significantly change the gameplay. Try them!",
  
  "Chceš jiný typ hry? Chceš vìtší výzvu?\n"
  "HyperRogue obsahuje øadu zvláštních módù a výzev, které "
  "výraznì mìní styl hry. Zkus je!")

S(
  "Hyperbolic geometry can be shown in many ways.",
  "Hyperbolickou geometrii je možné ukázat mnoha zpùsoby...")

S(
  "You do not want to lose the game from a single mistake?\n"
  "Do you want to use the Orbs strategically?\n"
  "Try the Orb Strategy mode!",
  
  "Nechceš prohrát kvùli jedné chybì?\n"
  "Chceš používat Sféry strategicky\n"
  "Zkus Sférostrategický mód!")

S(
  "Do you think you are playing on a ball? "
  "This is far from the truth!\n",
  
  "Myslíš, že hraješ na kouli? Samá voda!\n")

S(
  "Did you know that the path you take during the game "
  "is usually very close to a straight line?\n",

  "Vìdìl jsi, že cesta, kterou bìhem hry urazíš, je "
  "obvykle velmi blízká pøímce?")

S("Show me!", "Ukaž!")

S(
  "You are %1 cells away from the starting point, or "
  "the place where you used an Orb of Safety last time. "
  "There are %2 such cells.\n",
  
  "Nacházíš se %1 políèek od výchozího bodu nebo od místa, "
  "kde jsi naposledy použil Sféru Bezpeènosti. Takových "
  "políèek je celkem %2.\n")

S("about ", "zhruba ")
S(" (%1 more digits)", " (%1 dalších èíslic)")

S("see how it ended", "podívej se, jak to skonèilo")

// other missing/new things
// ------------------------

S("Orb unlocked: %1", "Byla odemèena Sféra: %1")
S("Secondary orb: %1", "Sekundární Sféra: %1")
S(" to submerge", " do ponoøení")
S(" to surface", " do vynoøení")
S("%The1 says, \"not this place, it looks even worse...\"", 
  "%The1 øíká, \"tady ne, tohle vypadá ještì hùø...\"")
S("torus", "torus")
S(" (click to use)", " (použij kliknutím)")
N("Hall of Mirrors", GEN_F, "Zrcadlový sál", "Zrcadlové sály", "Zrcadlový sál", "v Zrcadlovém sále")
Orb("the Mirror", "Zrcadla")
N("Reflection", GEN_O, "Odraz", "Odrazy", "Odraz", "v Odraze")
N("mirror wall", GEN_F, "zrcadlová stìna", "zrcadlové stìny", "zrcadlovou stìnu", "zrcadlovou stìnou")

S("This would only move you deeper into the trap!",
  "Tím by ses jen dostal dál do pasti!")

S("You swing your sword at the mirror.", "Udeøil jsi meèem do zrcadla.")
N("Mimic", GEN_M, "Mimik", "Mimikové", "Mimika", "Mimikem")
N("Narcissist", GEN_M, "Narcis", "Narcisové", "Narcisa", "Narcisem")
N("Mirror Spirit", GEN_M, "Zrcadlový duch", "Zrcadloví duchové", "Zrcadlového ducha", "Zrcadlovým duchem")

S("This person loves to look at their own reflection in the mirror. "
  "He believes himself to be one of the most important creatures in this world, "
  "and hates those who do not admire him.",

  "Tenhle èlovìk se rád kochá svým odrazem v zrcadle. Myslí si, že "
  "je jedním z nejdùležitìjších tvorù na svìtì a nenávidí ty, "
  "kteøí ho neobdivují."
  )

S(
    "A long time ago a mighty warrior was guarding the mirrors from being broken. "
    "While this warrior is no longer alive, his reflections have gained life of "
    "their own, and will punish the intruders.\n\n"
    "If you attack a Mirror Spirit physically, it is delayed, but not destroyed -- "
    "more reflections will come out of the mirror. Use Mimics to destroy them.",
    
    "Tento mocný váleèník kdysi dávno støežil zrcadla, aby se nerozbila. "
    "Teï už nežije, ale jeho odrazy získaly svùj vlastní život a trestají vetøelce.\n\n"
    "Pokud na Zrcadlového ducha fyzicky zaútoèíš, zpomalíš ho tím, ale neznièíš "
    "-- ze zrcadla vyjdou další odrazy. Pokud ho chceš znièit nadobro, použij Mimiky.")

// S(" (25 in the Orb Strategy mode)", " (25 ve Sférostrategickém módu)")
// S(" (50 in the Orb Strategy mode)", " (50 ve Sférostrategickém módu)")
// S(" (not in the Orb Strategy mode)", " (ne ve Sférostrategickém módu)")
    
/*
                        "NEW_ACHIEVEMENT_8_19_NAME"     "Totální vítìzství"
                        "NEW_ACHIEVEMENT_8_19_DESC"     "Získej ve Sférostrategickém módu Yendorskou Sféru, Svatý grál, Princeznu a 50 Hyperkamù."
                        
added to old achievements:
"(nebo 25 ve Sférostrategickém módu)"
"(nebo 50 ve Sférostrategickém módu)"
"(ne ve Sférostrategickém módu)"

These apparently have not been yet translated:

			"NEW_ACHIEVEMENT_8_4_NAME"	"Yendorský adept"
			"NEW_ACHIEVEMENT_8_4_DESC"	"Dokonèi Yendorskou výzvu."
			"NEW_ACHIEVEMENT_8_5_NAME"	"Mistr klíèù"
			"NEW_ACHIEVEMENT_8_5_DESC"	"Dokonèi pìt Yendorských výzev."
			"NEW_ACHIEVEMENT_8_6_NAME"	"Velmistr klíèù"
			"NEW_ACHIEVEMENT_8_6_DESC"	"Dokonèi 15 Yendorských výzev."
			"NEW_ACHIEVEMENT_8_7_NAME"	"Adept taktiky"
			"NEW_ACHIEVEMENT_8_7_DESC"	"Získej 1000 bodù v Taktickém módu."
			"NEW_ACHIEVEMENT_8_8_NAME"	"Mistr taktiky"
			"NEW_ACHIEVEMENT_8_8_DESC"	"Získej 5000 bodù v Taktickém módu."
			"NEW_ACHIEVEMENT_8_9_NAME"	"Velmistr taktiky"
			"NEW_ACHIEVEMENT_8_9_DESC"	"Získej 15 000 bodù v Taktickém módu"
			"NEW_ACHIEVEMENT_8_10_NAME"	"Na druhé stranì"
			"NEW_ACHIEVEMENT_8_10_DESC"	"Najdi a získej Zelenou trávu."

*/

// peaceful texts
// --------------

S("memory game", "hra na paměť")
S("display hints", "zobrazovat tipy")
S("hyperbolic puzzles", "hyperbolické hádanky")

// missing descriptions

S(  "A strange land filled with mirrors. "
    "Break magic mirrors and enter clouds of mirage to "
    "gain treasures and helpful Mimics.",
    
    "Podivná země plná zrcadel. "
    "Rozbíjením kouzelných zrcadel a procházením oblaků přeludů "
    "získáš poklady a užitečné Mimiky.")

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
  
  "Dokonalá zrcadlová stěna. Je nerozbitná a neprostupná, "
  "a to i pro éterické bytosti. Všechno, co v ní vidíš, je "
  "pouhý obraz reálného světa; můžeš do toho bít mečem, "
  "ale v reálném světě se nic nestane. Zrcadlové stěny "
  "dokonale odrážejí Mimiky, blesky a střely."
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
"V klidném módu můžeš prostě jen zkoumat svět bez jakýchkoli bitev; "
 "je v něm také k dispozici několik navigačních hádanek. V paměťové "
 "hře musíš sesbírat co možná nejvíc Dvanáctistěnů a vrátit se do "
 "výchozího bodu -- to je v hyperbolické geometrii nesmírně obtížné! "
 "Další hyperbolické hádanky zahrnují Pohřebiště (vykopávání pokladů "
 "magickým mečem), Galapágy (hledání dospělé želvy, která odpovídá "
 "tvé želvičce), Camelot (nalezení středu velkého hyperbolického "
 "kruhu) a Palác (sledování myši). Jiná uvedená místa se dají "
 "zkoumat.")

S("puzzles and exploration", "hádanky a zkoumání")

S("Zebra quotient", "kvocient Zebra")
S("field quotient", "tělesový kvocient")
S("mark heptagons", "označit sedmiúhelníky")
S("cheats", "cheaty")
S("help for keyboard users", "pomoc pro uživatele klávesnice")

S("Zebra quotient", "kvocient Zebra")
S("field quotient", "tělesový kvocient")
S("mark heptagons", "označit sedmiúhelníky")
S("projection", "projekce")
S("compass size", "velikost kompasu")

S("Collect as many Dodecahedra as you can, then return here!",
  "Získej co možná nejvíc Dvanáctistěnů, a potom se vrať sem!")

S("reset all configuration", "vyresetovat konfiguraci")
S("Are you sure?", "Určitě?")
S("yes, and delete the config file", "ano, a vymaž i konfigurační soubor")
S("yes", "ano")
S("cancel", "zrušit")
S("reset the special game modes", "vyresetovat speciální herní módy")

// extra flavor messages for the OSM
// ---------------------------------

S("You feel the presence of free saves on the Crossroads.",
  "Cítíš, že na Křižovatce si můžeš zdarma uložit hru.")

S("You feel the Orbs of Yendor nearby...",
  "Cítíš blízkost Yendorských Sfér...")
  
S("You feel the Orbs of Yendor in the Crossroads...",
  "Cítíš Yendorské Sféry na Křižovatce...")

S("You feel the Orbs of Yendor everywhere...",
  "Cítíš Yendorské Sféry všude...")

S("You have gained an offensive power!",
  "Získal jsi útočnou schopnost!")

S("A small reward for braving the Hell.",
  "Malá odměna za to, že ses postavil Peklu.")

S(" (mirrored)", " (zrcadlená)")
S(" (used %1 times)", " (použitá %1krát)")
S("Extras:", "Extra:") // extra Orbs gained in OSM

// cheats
// ------

S("unlock Orbs of Yendor", "odemkni Yendorské Sféry")
S("Collected the keys!", "Získal jsi klíče!")
S("Saved the Princess!", "Zachránil jsi Princeznu!")
S("save a Princess", "zachraň Princeznu")

// other
// -----
  
S("Note for mobiles", "Poznámka pro mobilní telefony")
S(
    "This tour is designed for computers, "
    "and keys are given for all actions. It will "
    "work without a keyboard though, although less "
    "comfortably -- just ignore the keys "
    "given and select options from MENU.\n\n"
    "Select 'next slide' from MENU.",

    "Tento tutorial je navržený pro počítače a u všech akcí "
    "jsou uvedeny klávesy. Funguje i bez klávesnice, i když "
    "ne tak pohodlně -- prostě ignorujte udávané klávesy a "
    "vybírejte možnosti z MENU.\n\n"
    "Vyber z MENU 'další snímek'.")
// FIX: tutorial changed to tour

S("skip the start menu", "přeskoč menu 'start'")
S("quick mouse", "rychlá myš")
S("This combination is known to be buggy at the moment.", "Tato kombinace možností v současné době nepracuje správně.")

// extra Princess texts
// --------------------

S("\"I do not like butterflies. They are treacherous.\"", 
  "\"Nesnáším motýly. Jsou zrádní.\"")

S("\"I hate roses.\"", "\"Nenávidím růže.\"")

S("\"In this world there is plenty of space for everyone. We do not need wars.\"", 
  "\"V tomhle světě je dost místa pro všechny. Nepotřebujeme války.\"")

S("\"Only the stupid hyperbugs do not understand this.\"", 
  "\"Jenom ti hloupí hyperhmyzáci tomu nerozumějí.\"")

S("\"I have once talked to a Yendorian researcher... he was only interested in infinite trees.\"",
  "\"Jednou se mnou mluvil jeden Yendorský výzkumník... zajímal se jenom o nekonečné stromy.\"")

S("\"Infinite trees are boring. I prefer other graphs.\"", 
  "\"Nekonečné stromy jsou nudné. Mám raději jiné grafy.\"")

// new start menu
// --------------

S("skip the start menu", "přeskoč menu 'start'")

S("HyperRogue classic", "klasický HyperRogue")
S("explore the world, collect treasures", "zkoumej svět, sbírej poklady")
S("do not get checkmated", "vyhni se šachmatu")
S("use your Orbs in tough situations", "v obtížných situacích používej Sféry")
S("continuous spacetime", "spojitý časoprostor")
S("(most achievements are not available)", "(většina achievementů není k dispozici)")
S("learn about hyperbolic geometry!", "nauč se o hyperbolické geometrii!")
S("more options", "více možností")

S(
  "A strange land filled with mirrors. Break magic mirrors and mirage clouds to "
  "gain treasures and helpful Mimics.",
  "Podivný kraj plný zrcadel. Rozbíjej magická zrcadla a oblaky přeludů, abys získal poklady a užitečné Mimiky."
  )

// were missing

N("Kraken", GEN_M, "Kraken", "Krakeni", "Krakena", "Krakenem")
N("Kraken Tentacle", GEN_N, "Krakení chapadlo", "Krakení chapadla", "Krakení chapadlo", "Krakením chapadlem")
S(" (killing increases treasure spawn)", " (zabití zlepšuje výskyt pokladů)")

N("stepping stones", GEN_O, "Brod", "Brody", "Brod", "Brodem")

// 10.1

// mew message options
S("centered", "na střed")
S("left-aligned", "nalevo")
S("line-broken", "lámání řádků")
S("message style", "styl zpráv")
S("limit messages shown", "limit počtu zobrazených zpráv")
S("Maximum number of messages on screen.", "Maximální počet zpráv na obrazovce.")

S("message log", "log zpráv")
S("real time", "skutečný čas")
S("game time", "herní čas")
S("precise", "přesný čas")
S("no time", "bez času")
S("back", "zpátky")
S("clear", "vyčistit")

// 'u' now loads the current picture in the Map Editor
S("'u' to load current", "'u' nahraje současný")

// Happy Hunting Ground
//----------------------

N("Hunting Ground", GEN_N, "Věčná loviště", "Věčná loviště", "Věčných lovištích", "ve Věčných lovištích")

S("The Happy Hunting Ground is the place where hunting is easy; "
  "the spirits of hunters go here after their death, if their life was deemed worthy. "
  "You did not qualify though, so you will not ever be able to find your way to the places "
  "where the hunter spirits and game are... "
  "and their hunting dogs will hunt you instead!\n\n"
  "You hope to use this to learn some "
  "skills in fighting in open hyperbolic space, though.",
  
  "Věčná loviště jsou místem, kde je lov snadný; po smrti sem "
  "odcházejí duše lovců, pokud byl jejich život uznán hodným. "
  "Ty ses ovšem nekvalifikoval, a tak nikdy nenajdeš cestu do "
  "míst, kde jsou duchové lovců a zvěře... a jejich lovečtí psi "
  "budou místo toho lovit tebe!\n\n"
  "Doufáš však, že se tak něco naučíš o boji v otevřeném "
  "hyperbolickém prostoru."
  )
  
N("Hunting Dog", GEN_M, "Lovecký pes", "Lovečtí psi", "Loveckého psa", "Loveckým psem")

N("Hunting Dog (guarding)", GEN_M, "Lovecký pes (na stráži)", "Lovečtí psi (na stráži)", "Loveckého psa (na stráži)", "Loveckým psem (na stráži)")
N("Hunting Dog (regrouping)", GEN_M, "Lovecký pes (na ústupu)", "Lovečtí psi (na ústupu)", "Loveckého psa (na ústupu)", "Loveckým psem (na ústupu)")
N("Turquoise", GEN_O, "Tyrkys", "Tyrkysy", "Tyrkys", "Tyrkysem")
Orb("Ferocity", "Dravosti")

S("Hunters believe that wearing a Turquoise amulet will improve their accuracy. "
  "This one has been lost, but the hunting dogs are guarding it until the owner returns.",
  
  "Lovci věří, že amulet z Tyrkysu zlepší jejich mušku. Tento amulet se "
  "ztratil, ale lovečtí psi ho střeží, dokud se nevrátí jeho majitel."
  )

S("Whenever you attack with this Orb, you also hit the monsters adjacent to you and opposite to the monster you originally attacked.",
  "Když zaútočíš a máš tuto Sféru, zaútočíš zároveň na netvory vedle "
  "sebe a naproti netvorovi, na kterého jsi zaútočil původně.."
  )

S("When your plan has clearly failed, it is better to abandon it and go to a safe place, to have a chance of succeeding next time. This dog clearly knows this.",
  "Když plán evidentně selže, je lepší ho opustit a odejít na "
  "nějaké bezpečné místo, abys měl šanci na úspěch někdy příště. "
  "Tenhle pes to očividně ví.")

// Volcanic Wasteland
//--------------------

N("Volcanic Wasteland", GEN_F, "Sopačná pustina", "Sopečné pustiny", "Sopečnou pustinu", "v Sopečné pustině")
N("lava", GEN_F, "láva", "láva", "lávu", "lávou")

S("The volcanic activity in this land is extremely unstable. "
  "Lava is too hot to walk on. It cools quickly, but another "
  "flow will come soon...",
  
  "Sopečná činnost v tomto kraji je nesmírně nestabilní. "
  "Láva je příliš žhavá, než abys po ní mohl chodit. Rychle "
  "chladne, ale za chvíli vyteče další..."
  )

N("Salamander", GEN_M, "Salamandr", "Salamandři", "Salamandra", "Salamandrem")

S("Salamanders are tough lizard-like creatures. Their tough skin protects them "
  "from both physical attacks and heat. Salamanders "
  "are stunned for a longer time if you push them into lava, fire, or a solid obstacle.",
  
  "Salamandři jsou odolní tvorové podobní ještěrům. Jejich tuhá "
  "kůže je chrání před fyzickými útoky i před ohněm. Pokud Salamandra "
  "strčíte do lávy, do ohně, nebo do pevné překážky, bude omráčený "
  "na delší dobu."
  )

N("Lava Wolf", GEN_M, "Lávový vlk", "Lávoví vlci", "Lávového vlka", "Lávovým vlkem")

S("While Ice Wolves love heat, their instincts usually will not let them leave the Icy Lands -- they are afraid that they will be unable to get back home, and that they will lose track of their prey. However, they find the Volcanic Wasteland so hot and attractive that they abandon their natural instincts... and try to track their prey using their other senses and intelligence.",
 
  "Zatímco Ledoví vlci milují teplo, jejich instinkt jim obvykle nedovolí "
  "opustit Ledový kraj -- bojí se, že už nenajdou cestu domů a ztratí stopu "
  "kořisti. Ale Sopečná pustina je tak horká a přitažlivá, že nechají své "
  "instinkty plavat... a začnou se snažit stopovat kořist pomocí svých "
  "ostatních smyslů a inteligence. A tak vznikají Lávoví vlci.")
  
N("Lava Lily", GEN_F, "Lávová lilie", "Lávové lilie", "Lávovou lilii", "Lovávou lilií")

S("This plant, able to survive in the extreme conditions of the Volcanic Wasteland, "
  "is a valuable alchemical ingredient.",
  
  "Tato rostlina, která dokáže přežít v extrémních podmínkách Sopečné "
  "pustiny, je cennou alchymistickou přísadou.")

Orb("Lava", "Lávy")
S("This Orb summons a minor volcanic activity around you. "
  "All the heptagonal cells adjacent to enemies in distance at most 5 to you "
  "will be set on fire. Does not affect the cell you are on, and enemies resistant to fire.",
  
  "Tato Sféra kolem tebe vyvolá mírnou sopečnou činnost. Všechna "
  "sedmiúhelníková políčka vedle nepřátel ve vzdálenosti nejvýše 5 "
  "od tebe začnou hořet. Nepůsobí na políčko, na kterém se nacházíš "
  "ty, a na nepřátele odolné proti ohni."
  )

// Orbs would be destroyed in Volcanic Wasteland
S("would be destroyed in %the1", "by%l1by zničen%1 %abl1") 
S(" to go cold", " vychladnout")
S("%The1 is destroyed by lava!", "Láva zničila %a1!")
S("%The1 is killed by lava!", "Láva zabila %a1!")
S("Run away from the lava!", "Utíkej od lávy!")

// Terracotta Army
//-----------------

N("Terracotta Army", GEN_F, "Terakotová armáda", "Terakotové armády", "Terakotovou armádu", "v Terakotové armádě")

S("The Emperor's mausoleum. The army "
  "of Terracotta Warriors is supposed to protect him in his "
  "afterlife, while the traps are to protect the treasures "
  "in the mausoleum from thieves.",

  "Císařovo mauzoleum. Armáda Terakotových válečníků ho má "
  "chránit v jeho posmrtném životě a pasti mají chránit "
  "mauzoleum před zloději."
  )

S("Spending 3-5 turns next to the Terracotta Warriors will cause them to awaken. "
  "They have 7 HP, and get stunned for longer and longer time with each attack.",
  
  "Pokud zůstaneš 3-5 kol vedle Terakotového válečníka, probudí se. Má 7 "
  "životů a každý útok ho omráčí na delší dobu než ten předešlý."
  )

N("arrow trap", GEN_F, "šípová past", "šípové pasti", "šípovou past", "šípovou pastí")

S("The arrow traps will automatically launch a deadly arrow when stepped on. They are quite old though, so the arrow will "
  "actually fly at the end of the next turn.",
  
  "Když šlápneš na šípovou past, automaticky se vystřelí smrtící šíp. "
  "Pasti jsou ale velice staré, takže šíp vyletí teprve na konci "
  "příštího kola."
  )

N("mercury river", GEN_F, "rtuťová řeka", "rtuťové řeky", "rtuťovou řeku", "rtuťovou řekou")
S("A river of mercury.", "Řeka tvořená rtutí.")

N("Terracotta Warrior", GEN_M, "Terakotový válečník", "Terakotoví válečníci", "Terakotového válečníka", "Terakotovým válečníkem")

N("Jiangshi", GEN_M, "Ťiang-š'", "Ťiang-š'", "Ťiang-š'", "Ťiang-š'")
S("You think this was one of the people who have constructed the Terracotta Warriors and the arrow traps. "
  "They have been locked inside, so that they will never divulge the secrets of the mausoleum. They would like to return their homes and families, though.",
  
  "Říkáš si, že tohle byl asi jeden z lidí, kteří postavili Terakotové "
  "válečníky a šípové pasti. Zůstali tu zamčení, aby nikdy neprozradili "
  "tajemství mauzolea. Ale rádi by se vrátili domů ke svým rodinám." 
  )

Orb("Slashing", "Sekání")
S("Whenever you attack with this Orb, you also hit the monsters adjacent both to you and the monster you originally attacked.",
  "Kdykoli zaútočíš a máš tuto Sféru, zasáhneš také netvory, kteří sousedí s tebou a s netvorem, na kterého jsi zaútočil původně.")

N("Ancient Weapon", GEN_F, "Stará zbraň", "Staré zbraně", "Starou zbraň", "Starou zbraní")
S("This ancient weapon is beautifully encrusted with precious gems, but you prefer your own -- it is much lighter.",
  "Tato starobylá zbraň je krásně zdobená drahokamy, ale tobě je milejší "
  "tvoje vlastní -- je mnohem lehčí.")

// Blizzard
//----------

N("Blizzard", GEN_F, "Vánice", "Vánice", "Vánici", "ve Vánici")
N("Ice Golem", GEN_M, "Ledový golem", "Ledoví golemové", "Ledového golema", "Ledovým golemem")
N("Void Beast", GEN_F, "Prázdnostvůra", "Prázdnostvůry", "Prázdnostvůru", "Prázdnostvůrou")
N("Forgotten Relic", GEN_F, "Zapomenutá relikvie", "Zapomenuté relikvie", "Zapomenutou relikvii", "Zapomenutou relikvií")

S("The Ice Golems are powered by intense magical coldness. When destroyed in the Blizzard or another icy land, they become "
  "ice walls, and freeze the land around them.",
  
  "Ledové golemy pohání intenzivní magický chlad. Pokud jsou zničeni "
  "ve Vánici nebo v jiném ledovém kraji, promění se v ledové stěny "
  "a zmrazí půdu kolem sebe.")

S("Are creatures of Void actual monsters, or just monster-shaped holes in the air?\n\nVoid Beasts move simply by letting the air move into their current location -- "
  "the hole will move to the place where the air came from! Void Beasts are great at moving against the wind, but they have problems "
  "moving with the wind.",
  
  "Jsou bytosti z Prázdnoty skuteční netvoři nebo jenom díry "
  "ve vzduchu ve tvaru netvorů?\n\n"
  "Prázdnostvůry se pohybují prostě tím, že nechávají vzduch "
  "proniknout tam, kde právě jsou -- díra se pak přesune tam, "
  "odkud přišel vzduch! Prázdnostvůry se dokážou skvěle "
  "pohybovat proti větru, ale mají problémy s pohybem po větru."
  )

S("Once in the past there lived a tribe whose prophets warned about the great blizzard that was to come and freeze the world. "
  "They thought it was the wrath of the gods, so they put some altars, so that the gods could hear their pleas. "
  "The mighty golems guarded those altars, making sure nobody steals the jewels, because a robbed god is a angry god. "
  "Unfortunately winter has come (maybe because only the monks knew how to stop the golems, and they were said to had escaped earlier with some of the jewels). "
  "The tribe has frozen to death, altars got covered with snow and ice. Only ice golems still guard them waiting for the spring.",
  
  "Kdysi dávno byl jeden kmen, jehož prorokové varovali před velkou vánicí, "
  "která měla přijít a zmrazit svět. Mysleli si, že je to hněv bohů, a tak "
  "postavili oltáře, aby bohové vyslyšeli jejich prosby. Oltáře chránili "
  "mocní golemové, aby nikdo neukradl klenoty, protože okradený bůh je "
  "rozhněvaný bůh. Zima však bohužel stejně přišla (možná proto, že "
  "jenom mnichové věděli, jak golemy zastavit, a ti prý utekli i "
  "s nějakými klenoty). Kmen zahynul zimou a oltáře pokryl sníh a "
  "led. Jen ledoví golemové je stále střeží a čekají na jaro."
  )

// Winter becomes native in Blizzard, while Change replaces Winter in Cocytus
Orb("Change", "Změny")
S("This ranged Orb will transform the target monster into one without any special powers. It also stuns them for one turn. "
  "Does not affect multi-tile monsters.",
  "Tato sféra promění cílového netvora v jiného, který nemá žádné "
  "zvláštní schopnosti. Kromě toho ho také na jedno kolo omráčí. "
  "Nepůsobí na netvory, kteří zabírají více políček."
  )

// yet unused orb
Orb("the Triangle", "Trujúhelníku")
S("Whenever you attack with this Orb, you also hit the monsters adjacent to you which are neither adjacent nor opposite to the monster "
  "you originally attacked.",
  "Když zaútočíš a máš tuto Sféru, zasáhneš také všechny netvory, kteří s tebou sousedí, ale nesousedí s netvorem, na kterého jsi zaútočil původně, a nejsou ani naproti němu.")

S("Using %the1 in %the2 sounds dangerous...", "Používat %1 %abl2, to zní nebezpečně...")
S("%The1 is mostly useless in %the2...", "%1 není %abl2 příliš užitečná...")
S("%The1 is forbidden in %the2 (disables some achievements)", "%1 je %abl2 zakázané (její použití znemožní některé achievementy)")

S("You are ambushed!", "Přepadení!")
S("teleport", "teleport")
S("ambush:", "přepadení:")

S("The Hunting Dogs give up.", "Lovečtí psi vzdali pronásledování.")

/*
		"NEW_ACHIEVEMENT_8_20_NAME"	"Kořist"
		"NEW_ACHIEVEMENT_8_20_DESC"	"Získej Tyrkys."
		"NEW_ACHIEVEMENT_9_4_NAME"	"Lovecký pes"
		"NEW_ACHIEVEMENT_9_4_DESC"	"Získej 10 Tyrkysů (nebo 25 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_8_22_NAME"	"Mistr úniků"
		"NEW_ACHIEVEMENT_8_22_DESC"	"Získej 25 Tyrkysů (nebo 50 ve Sférostrategickém módu)."
                "NEW_ACHIEVEMENT_8_23_NAME"     "Pán dravosti"
		"NEW_ACHIEVEMENT_8_23_DESC"	"Získej 50 Tyrkysů (nelze získat ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_8_24_NAME"	"Došel jsi do Terakotové armády"
		"NEW_ACHIEVEMENT_8_24_DESC"	"Získej Staoru zbraň."
		"NEW_ACHIEVEMENT_8_25_NAME"	"Ťiang-š'"
		"NEW_ACHIEVEMENT_8_25_DESC"	"Získej 10 Starých zbraní (nebo 25 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_8_26_NAME"	"Terakotový válečník"
		"NEW_ACHIEVEMENT_8_26_DESC"	"Získej 25 Starých zbraní (nebo 50 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_8_27_NAME"	"Císař"
		"NEW_ACHIEVEMENT_8_27_DESC"	"Získej 50 Starých zbraní (nelze získat ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_8_28_NAME"	"Došel jsi do Vánice"
		"NEW_ACHIEVEMENT_8_28_DESC"	"Získej Zapomenutou relikvii."
		"NEW_ACHIEVEMENT_8_29_NAME"	"Ledový golem"
		"NEW_ACHIEVEMENT_8_29_DESC"	"Získej 10 Zapomenutých relikvií (nebo 25 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_8_30_NAME"	"Prázdnostvůra"
		"NEW_ACHIEVEMENT_8_30_DESC"	"Získej 25 Zapomenutých relikvií (nebo 50 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_8_31_NAME"	"Pán Vánice"
		"NEW_ACHIEVEMENT_8_31_DESC"	"Získej 50 Zapomenutých relikvií (nelze získat ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_0_NAME"	"Došel jsi do Sopečné pustiny"
		"NEW_ACHIEVEMENT_9_0_DESC"	"Získej Lávovou lilii."
		"NEW_ACHIEVEMENT_9_1_NAME"	"Lávový vlk"
		"NEW_ACHIEVEMENT_9_1_DESC"	"Získej 10 Lávových lilií (nebo 25 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_2_NAME"	"Salamandr"
		"NEW_ACHIEVEMENT_9_2_DESC"	"Získej 25 Lávových lilií (nebo 50 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_3_NAME"	"Pán sopky"
		"NEW_ACHIEVEMENT_9_3_DESC"	"Získej 50 Lávových lilií (nelze získat ve Sférostrategickém módu)."
*/

// missing from previous versions:

S("\n\nThis orb also allows you to collect items encased in ice.",
  "\n\nTato Sféra ti také umožňuje sbírat předměty zarostlé v ledu.")

// vampire attack
S("%The1 drains your powers!", "%The1 vysává tvou sílu!")

// additions
S(" This tour will not advance on its own -- you have to press Enter (not while reading help text).", 
  " Tento tutoriál nepostupuje sám od sebe -- musíš stisknout Enter (ale ne při čtení nápovědy)."
  )

S("Hint: hold Alt to highlight enemies and other important features.",
  "Nápověda: když budeš držet Alt, zvýrazníš tím nepřátele a jiné důležité věci.")

// Crystal World
N("Crystal World", GEN_O, "Krystalický svět", "Krystalické světy", "Krystalický svět", "v krystalickém světě")
N("Glowing Crystal", GEN_O, "Zářící krystal", "Zářící krystaly", "Zářící krystal", "Zářícím krystalem")

S("Yes, this is definitely a crystal. A very regular crystalline structure.\n\n"
  "This land has been designed as a tool to play with various geometries, and it does not appear during a normal gameplay.",
  
  "Ano, tohle je rozhodně krystal. A navíc má velice pravidelnou krystalickou strukturu.\n\n"
  "Tento kraj byl navržený jako nástroj pro hraní s různými geometriemi a v normální hře se neobjevuje.")

S("You cannot move there!", "Tam se nemůžeš pohnout!")

// geometry stuff

S("experiment with geometry", "experimentování s geometrií")

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
  
  "Pokud chceš vědět, jak geometrie HyperRogue ovlivňuje hru, tento mód je právě "
  "pro tebe!\n\n"
  "Můžeš tu vyzkoušet celou řadu různých geometrií. Začneme tím, že slepíme "
  "n-úhelníky tak, aby se jich v každém vrcholu stýkalo k. V závislosti na "
  "n a k se výsledek složí do koule, rozloží do roviny, a nebo bude vyžadovat "
  "hyperbolický prostor. Výsledek je pak možné 'seříznout' tím, že se každý "
  "vrchol nahradí 2k-úhelníkem. Navíc si můžete hrát s kvocientovými "
  "geometriemi. Tak například eliptickou geometrii získáte ze sférické "
  "tím, že ztotožníš každé dva protilehlé body, takže po příchodu tam "
  "skončíš na stejném místě (ovšem zrcadlově převrácen%ý0). Bav se při "
  "experimentování! Achievementy a žebříčky v geometrických experimentech "
  "nefungují, s výjimkou některých zvláštních případů.\n\n"
  "Ve standardní geometrii (seříznuté či neseříznuté) můžeš hrát plnou hru, "
  "ale v jiných geometriích si musíš vybrat, kde chceš hrát. Kraje se "
  "odemknou, když je navštívíš během hry nebo trvale po sebrání 25 "
  "pokladů. Zkus Křižovatku v eukleidovském módu nebo chaotický mód "
  "v nestandardní, nekvocientovém hyperbolickém světě, pokud chceš "
  "navštívit mnoho krajů.\n"
  "Zajímavé body:\n"
  "* Křišťálový svět a Pokřivené pobřeží lze brát jako další geometrie navíc.\n"
  "* Halloween je kraj specificky navržený pro sférickou geometrii.\n"
  "* Zkus Věčná loviště v eukleidovském módu, abys viděl ten rozdíl -- jsou nemožná."  
  )

// new geometries available:
S("standard", "standardní") // renamed from 'hyperbolic'

S("octagons", "osmiúhelníky")
S("four pentagons", "čtyři pětiúhelníky")
S("four hexagons", "čtyři šestiúhelníky")
S("four heptagons", "čtyři sedmiúhelníky")
S("cube", "krychle")
S("tetrahedron (buggy)", "čtyřstěn (problémový)")
S("tetrahedron", "čtyřstěn") // in case if it gets better

S("land", "kraj")
S("bitruncated", "seříznutí")
S("does not matter", "nehraje roli")
S("sides per face", "počet stran")
S("faces per vertex", "stupeň vrcholů")
S("none", "žádný")
S("zebra", "zebra")
S("field", "těleso")
S("quotient space", "kvocientový prostor")
S("size of the world", "velikost světa")
S("flat", "placka")

// bitruncated or not bitrunc
S(" (b)", " (s)")
S(" (n)", " (n)")

S("(locked)", "(uzamčeno)")

S("stereographic/orthogonal", "stereografická/ortogonální projekce")
S("Poincaré/Klein", "Poincaré/Klein")
  
// Paper Model Creator
S("Useless in Euclidean geometry.", "V eukleidovské geometrii k ničemu.")
S("Not implemented for spherical geometry. Please tell me if you really want this.",
  "Není implementováno ve sférické geometrii. Pokud byste to opravdu chtěli, dejte mi vědět.")

S("score: %1", "skóre: %1")
S("kills: %1", "zabitých netvorů: %1")

// === 10.3 ===
// ============

// === 10.3 ===
// ============

// Missing/minor updated texts
//-----------------------------

// file selection dialog
S("Enter = choose", "Enter = výbìr")
S("Esc = cancel", "Esc = zrušit")

S("hyperbolic", "hyperbolická")
S("reset special modes", "resetovat speciální módy")
S("orthogonal projection", "ortogonální projekce")
S("small Poincaré model", "malý Poincarého model")
S("big Poincaré model", "velký Poincarého model")
S("Klein-Beltrami model", "Klein-Beltramiho model")
S("bestiary of %the1", "bestiáø: %1")

// for the map editor
N("Dragon Head", GEN_F, "Draèí hlava", "Draèí hlavy", "Draèí hlavu", "Draèí hlavou")

S("score: %1", "skóre: %1")
S("kills: %1", "zabití: %1")

// advanced config of quotient geometry
//--------------------------------------

S(
  "This geometry is obtained by applying the same 'generators' which "
  "lead to creating the given basic hyperbolic geometry, "
  "but using a fixed finite field instead of the field of reals. "
  "It can be also interpreted as a quotient of the given basic geometry. "
  "Warning: field patterns based on large primes might generate for a long time.",
  
  "Tato geometrie vzniká použítím stejných 'generátorù', které vedou k vytvoøení "
  "dané základní hyperbolické geometrie, ale místo tìlesa reálných èísel používá "
  "pevnì dané koneèné tìleso. Lze ho také interpretovat jako kvocient dané "
  "základní geometrie. Varování: vzory založené na velkých prvoèíslech se mohou "
  "generovat velmi dlouho!")

S("find the next prime", "najdi další prvoèíslo")
S("activate", "aktivovat")
S("order %1%2 (non-bitruncated cells: %3)", "øád %1%2 (nezkosených políèek: %3)")

// Torus/Klein bottle, Euclidean Squares, and their advanced configuration
//-------------------------------------------------------------------------

S("square grid", "ètvercová sí ")

S("torus/Klein bottle", "torus/Kleinova láhev")
S("mode", "mód")

S("single row (hex)", "jedna øada (hexy)")
S("single row (squares)", "jedna øada (ètverce)")
S("parallelogram (hex)", "rovnobìžník (hexy)")
S("rectangle (squares)", "obdélník (ètverce)")
S("rectangle (hex)", "obdélník (hexy)")
S("Klein bottle (squares)", "Kleinova láhev (ètverce)")
S("Klein bottle (hex)", "Kleinova láhev (hexy)")

S("this mode has bad patterns", "v tomto módu jsou špatné vzory")
S("incompatible with bitruncating", "nekompatibilní se zkosením")
S("best if %1 is divisible by %2", "nejlépe to funguje, když je %1 dìlitelné %2")

S("number of cells (n)", "poèet políèek (n)")
S("cell bottom-right from 0 (d)", "políško vpravo dole od 0 (d)")
S("cell below 0 (d)", "políèko pod 0 (d)")
S("width (x)", "šíøka (x)")
S("height (y)", "výška (y)")

// upgraded Hypersian Rug mode
//-----------------------------

S("In the perspective projection, you can use arrows to rotate the camera, Page Up/Down to go forward/backward, Shift+arrows to strafe, and Ctrl+arrows to rotate the model.",
  "V perspektivní projekci je možné otáèet kameru šipkami, Page Up/Down pro pohyb "
  "vpøed/vzad, Shift+šipky pro pohyb do strany a Ctrl+šipky pro otáèení modelu.")

S("Note: this mode is based on what you see on the screen -- but re-rendered in another way. "
  "If not everything is shown on the screen (e.g., too zoomed in), the results will be incorrect "
  "(though possibly interesting). "
  "Use a different projection to fix this.",
  
  "Poznámka: tento mód je založený na tom, co vidíte na obrazovce -- je to pouze "
  "re-renderované jiným zpùsobem. Pokud není na obrazovce všechno vidìt (napøíklad "
  "proto, že pohled je pøíliš pøiblížený), budou výsledky nesprávné (i když "
  "možná zajímavé). Mùžete to opravit tím, že použijete jinou projekci.")

S("vertex limit", "limit vrcholù")
S("The more vertices, the more accurate the Hypersian Rug model is. "
  "However, a number too high might make the model slow to compute and render.",
  "Èím více vrcholù, tím budfe model Hyperského koberce pøesnìjší. Pøíliš "
  "vysoké èíslo však mùže zpùsobit, že se model bude pomalu poèítat a renderovat.")

S("model distance", "vzdálenost modelu")
S("In the perspective projection, this sets the distance from the camera to the center of the model. "
  "In the orthogonal projection this just controls the scale.",
  
  "V perspektivní projekci tento parametr nastavuje vzdálenost kamery od støedu "
  "modelu. V ortogonální projekci pouze upravuje mìøítko.")

S("maximum error", "maximální chyba")
S("New points are added when the current error in the model is smaller than this value.",
  "Pokud je souèasná chyba modelu menší než tato hodnota, pøidají se nové body.")

S("field of view", "zorné pole")
S("Horizontal field of view, in the perspective projection. "
  "In the orthogonal projection this just controls the scale.",
  "Horizontální zorné pole, v perspektivní projekci. V ortogonální "
  "projekci tento parametr pouze upravuje mìøítko.")

S("native geometry", "rodná geometrie")
S("scale model", "mìøítko modelu")
S("This is relevant when the native geometry is not Euclidean. "
  "For example, if the native geometry is spherical, and scale < 1, a 2d sphere will be rendered as a subsphere; "
  "if the native geometry is hyperbolic, and scale > 1, a hyperbolic plane will be rendered as an equidistant surface. ",
  
  "Toto je relevantní v situacích, kdy 'rodná geometrie' není eukleidovská. Pokud je "
  "napøíklad sférická a mìøítko je < 1, bude 2d koule vyrenderovaná jako subsféra; "
  "pokud je rodná geometrie hyperbolická a mìøítko je > 1, bude hyperbolická rovina "
  "vyrenderovaná jako ekvidistantní plocha.")

S("This just shows the 'z' coordinate of the selected point.",
  "Tohle jenom ukazuje souøadnici 'z' vybraného bodu.")

S("radar", "radar")
S("model iterations", "iterace modelu")
S("keep shape", "udržovat tvar")

// texture mode
//--------------

S("texture mode", "texturový mód")

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
  
  "Tento mód vám umožòuje snadno mìnit dìlení podlahy -- vyberte 'namalovat "
  "novou texturu' a kreslete jako v Malování. Získaný vzor se pak dá snadno "
  "zmìnit na jinou geometrii nebo uložit.\n\n"
  "Místo kreslení také mùžete použít jako texturu libovolný obrázek. "
  "Nejlépe to funguje se sférickými/eukleidovskými/hyperbolickými dìleními "
  "(napøíklad s fotografií fotbalového míèe nebo s jedním z dìlení M. C. "
  "Eschera), ale mùžete také použít libovolné obrázky, abyste je udìlali "
  "periodickými (nejlépe to funguje s nastavením 'velký obrázek' ve výbìru "
  "geometrie). Geometrii dìlení je opìt možné mìnit.\n\n")

S("texture mode (off)", "texturový mód (vypnutý)")
S("select geometry/pattern", "vyber geometrii/vzor")
S("reactivate the texture", "reaktivuj texturu")
S("open PNG as texture", "otwórz PNG jako teksturê")
S("load texture config", "nahraj konfiguraci textury")
S("warning: unable to find the center", "varování: nemohu najít støed")
S("texture size", "rozmìr textury")
S("paint a new texture", "namaluj novou texturu")
S("precision", "pøesnost")

S("texture mode (overlay)", "texturový mód (pøekryv)")
S("select the texture's pattern", "vyber vzor textury")
S("enable the texture", "zapni texturu")
S("cancel the texture", "zruš texturu")
S("move the model", "pøesuò model")
S("move the texture", "pøesuò texturu")
S("zoom/scale the texture", "zmìò pøiblížení/mìøítko textury")
S("zoom/scale the model", "zmìò pøiblížení/mìøítko modelu")
S("projection", "projekce")
S("affine transformations", "afinní transformace")
S("magic", "magie")

S("grid color (master)", "barva møížky (hlavní)")
S("grid color (copy)", "barva møížky (kopie)")
S("save the raw texture", "ulož surovou texturu")
S("Saved the raw texture to %1", "Surová textura uložena do %1")

S("texture auto-adjustment", "automatická úprava textury")
S("drag from the model to the texture", "pøetáhni z modelu na texturu")
S("affect model scale", "mìøítko modelu")
S("affect model projection", "projekce")
S("affect model rotation", "rotace modelu")
S("affect model position", "poloha modelu")
S("affect texture slanting", "zkosení textury")
S("affect texture stretching", "natažení textury")
S("delete markers", "vymaž markery")
S("perform auto-adjustment", "proveï automatickou úpravu")

S("texture mode (active)", "texturový mód (aktivní)")
S("deactivate the texture", "deaktivuj texturu")
S("back to overlay mode", "zpátky do módu pøekryvu")
S("change the geometry", "zmìò geometrii")
S("grid color", "barva møížky")
S("mesh color", "barva sí ky")
S("color alpha", "alfa barva")
S("The higher the value, the less important the color of underlying terrain is.",
  "Èím vyšší hodnota, tím ménì dùležitá je barva podloží.")
S("edit the texture", "edituj texturu")
S("save the full texture image", "ulož plný texturový obraz")
S("Saving full texture to %1...", "Ukládám plnou texturu do %1...")
S("save texture config", "ulož konfiguraci textury")

// pattern groups

S("football", "fotbalový míè")
S("three colors", "tøi barvy")
S("chessboard", "šachovnice")
S("single type", "jeden typ")
S("large picture", "velký obrázek")
S("periodic patterns", "periodické vzory")
S("more tuning", "doladìní")
S("BAD", "ŠPATNÉ")

// extra patterns

S("Docks pattern", "vzor Dokù")
S("broken Emerald pattern", "polámaný smaragdový vzor")
S("coloring", "barvení")


// Docks
//-------

N("Docks", GEN_O, "Dok", "Doky", "Dok", "v Doku")
N("dock", GEN_O, "dok", "doky", "dok", "dokem")
S("A dock.", "Dok.")
N("burning dock", GEN_O, "hoøící dok", "hoøící doky", "hoøící dok", "hoøícím dokem")
S("A burning dock.", "Hoøící dok.")

N("Sea Glass", GEN_N, "Moøské sklo", "Moøská skla", "Moøské sklo", "Moøským sklem")

// Snake Nest

N("Snake Nest", GEN_N, "Hadí hnízdo", "Hadí hnízda", "Hadí hnízdo", "v Hadím hnízdì")

N("Snake Oil", GEN_O, "Hadí olej", "Hadí oleje", "Hadí olej", "Hadím olejem")

S("Made of actual snakes!", "Vyrobený z pravých hadù!")

// Ruined City

N("Ruined City", GEN_N, "Znièené mìsto", "Znièená mìsta", "Znièené mìsto", "ve Znièeném mìstì")

S("Once a beautiful city... but now overrun by the mighty Raiders of unknown origin.\n\n"
 "Raiders cannot be harmed with mundane weapons, but each color has its movement restrictions.",
 
 "Kdysi to bylo krásné mìsto... ale teï ho ovládli mocní Nájezdníci neznámého pùvodu.\n\n"
 "Nájezdníkùm nemohou ublížit obyèejné zbranì, ale každá barva má svá omezení pohybu.")

N("ruin wall", GEN_F, "znièená zeï", "znièené zdi", "znièenou zeï", "znièenóu zdí")

N("Chrysoberyl", GEN_O, "Chryzoberyl", "Chryzoberyly", "Chryzoberyl", "Chryzoberylem")
S("Fragment of the past glory.", "Fragment minulé slávy.")

N("Red Raider", GEN_M, "Èervený nájezdník", "Èervení nájezdníci", "Èerveného nájezdníka", "Èerveným nájezdníkem")
S("Red Raiders travel in pairs. They have promised to always watch each other's backs. They are able to destroy walls on their way.",
  "Èervení nájezdníci cestují ve dvojicích. Slíbili si, že si vždycky budou navzájem "
  "hlídat záda. Po cestì mohou nièit zdi.")

N("Gray Raider", GEN_M, "Šedý nájezdník", "Šedí nájezdníci", "Šedého nájezdníka", "Šedým nájezdníkem")
S("Gray Raiders never step on gray cells.",
  "Šedí nájezdníci nikdy nevstupují na šedá políèka.")

N("Green Raider", GEN_M, "Zelený nájezdník", "Zelení nájezdníci", "Zeleného nájezdníka", "Zeleným nájezdníkem")
S("Green Raiders never step from one green cell to another.",
  "Zelení nájezdníci nikdy nepøecházejí z jednoho zeleného políèka na druhé.")

N("Brown Raider", GEN_M, "Hnìdý nájezdník", "Hnìdí nájezdníci", "Hnìdého nájezdníka", "Hnìdým nájezdníkem")
S("Brown Raiders never move adjacent to an item.",
  "Hnìdí nájezdníci nikdy nevstupují na políèka, která sousedí s pøedmìtem.")


N("Blue Raider", GEN_M, "Modrý nájezdník", "Modøí nájezdníci", "Modrého nájezdníka", "Modrým nájezdníkem")
S("Blue Raiders have a powerful attack which takes two turns to complete, and also makes the Blue Raider stunned "
  "for a long time. This attack can destroy other Raiders if it hits them.",
  "Modøí nájezdníci mají mocný útok, který trvá dvì kola a Modrý nájezdník "
  "po nìm zùstane dlouho omráèený. Tento útok dokáže znièit jiné Nájezdníky, pokud je zasáhne."
  )

S("You cannot attack Raiders directly!", "Na Nájezdníky nemùžeš útoèit pøímo!")

Orb("Slaying", "Záhuby")

S("This Orb lets you defeat Raiders and other tough single-cell monsters in melee.",
  "Tato Sgéra ti umožòuje zabít v boji nablízko Nájezdníky a jiné odolné jednopolíèkové netvory.")

// Jelly Kingdom

N("Jelly Kingdom", GEN_N, "Království rosolu", "Království rosolu", "Království rosolu", "v Království rosolu")

S("Some of the Slime Beasts have decided to revolt against the color rules in the "
  "Alchemist Lab. They have changed their shape and consistency, declared independence, and established their own Kingdom.\n\n"
  "Jellies switch between being a wall and being a monster after every treasure you collect.",
  
  "Nìkteøí Slizoví netvoøi se rozhodli vzbouøit proti barevným pravidlùm v Alchymistické laboratoøi. Zmìnili svùj tvar a konzistencim vyhlásili nezávislost a založili si své vlastní Království.\n\n"
  "Kdykoli sebereš nìjaký poklad, rosoly se zmìní ze zdí na netvory a opaènì."
  )

N("Red Jelly", GEN_O, "Èervený rosol", "Èervené rosoly", "Èervený rosol", "Èerveným rosolem")
N("Blue Jelly", GEN_O, "Modrý rosol", "Modré rosoly", "Modrý rosol", "Modrým rosolem")

N("Tasty Jelly", GEN_O, "Chutný rosol", "Chutné rosoly", "Chutný rosol", "Chutným rosolem")
S("A tasty byproduct of the Jelly Revolution.",
  "Chutný vedlejší produkt Rosolové revoluce.")

Orb("Phasing", "Fázování")
S("This orb lets you pass through walls (one cell wide), and also through monsters, as long as they will not attack you in transit.",
  "Tato Sféra ti umožòuje procházet zdmi (o tlouš ce jednoho políèka), a také netvory, pokud na tebe pøitom nemohou zaútoèit.")

S("You cannot attack Jellies in their wall form!", "Na Rosoly nemùžeš útoèit, když jsou v podobì zdi!")

// keyboard help
//---------------

S("advanced keyboard shortcuts", "pokroèilé klávesové zkratky")

/* not yet translated to Polish
  "1 = orthogonal/Gans model",
  "2 = small Poincare model/stereographic projection",
  "3 = big Poincare model/stereographic projection",
  "4 = Klein model/gnomonic projection",
  "5 = change wall display mode",
  "6 = change grid",
  "7 = change heptagon marking",
  "8 = change background color",
  "9 = hyperboloid model",
  "qweasdzxc, hjklyubn, numpad = move/skip turn",
  "arrows = panning",
  "o = world overview",
  "v = menu",
  "F1 = help",
  "F5 = restart game",
  "F10 = quit game",
  "Esc = quest status",
  "Alt+Enter = full screen",
  "Alt = highlight interesting stuff",
  "t = use a ranged Orb (target center of the screen)",
  "g = drop a Dead Orb",
  "click left mouse button = move/skip",
  "shift+click left mouse button = use ranged Orb",
  "click right mouse button = context help",
  "mousewheel up = panning",
  "hold middle mouse button = panning",
  "mousewheel down = move/skip",
  "shift + mousewheel = change projection",
  "ctrl + mousewheel = change zoom",
  "ctrl + shift + mousewheel = change both projection and zoom",
  "ctrl + hold middle button = move the screen",
  "shift + middle button = reset position"
*/

/* not yet translated to Polish
  "1 = ortogonální/Gansùv model",
  "2 = malý Poincarého model/stereografická projekce",
  "3 = velký Poincarého model/stereografická projekce",
  "4 = Kleinùv model/gnómonická projekce",
  "5 = zmìna módu zobrazování zdí",
  "6 = zmìna møížky",
  "7 = zmìna oznaèování sedmiúhelníkù",
  "8 = zmìna barvy pozadí",
  "9 = model hyperboloidu",
  "qweasdzxc, hjklyubn, numpad = pohyb/èekání na místì",
  "šipky = scrollování",
  "o = pøehled svìta",
  "v = menu",
  "F1 = nápovìda",
  "F5 = restart",
  "F10 = ukonèení hry",
  "Esc = status úkolu",
  "Alt+Enter = full screen",
  "Alt = zvýraznìní zajímavých vìcí",
  "t = použití dálkové Sféry (zacíluje støed obrazovky)",
  "g = polož Mrtvou sféru",
  "levé kliknutí = pohyb/èekání na místì",
  "shift+levé kliknutí = použití dálkové Sféry",
  "pravé kliknutí = kontextová nápovìda",
  "koleèko myši nahoru = scrollování",
  "držení prostøedního tlaèítka myši = scrollování",
  "koleèko myši dolù = pohyb/èekání na místì",
  "shift + koleèko myši = zmìna projekce",
  "ctrl + koleèko myši = zmìna pøiblížení",
  "ctrl + shift + koleèko myši = zmìna projekce a pøiblížení najednou",
  "ctrl + držet prostøední tlaèítko myši = pohyb obrazovky",
  "shift + prostøední tlaèítko myši = reset polohy"
*/

/* achievements 

		"NEW_ACHIEVEMENT_9_5_NAME"	"Došel jsi do znièeného mìsta"
		"NEW_ACHIEVEMENT_9_5_DESC"	"Najdi a získej Chryzoberyl."
		"NEW_ACHIEVEMENT_9_6_NAME"	"Hnìdý nájezdník"
		"NEW_ACHIEVEMENT_9_6_DESC"	"Získej 10 Chryzoberylù (nebo 25 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_7_NAME"	"Modrý nájezdník"
		"NEW_ACHIEVEMENT_9_7_DESC"	"Získej 25 Chryzoberylù (nebo 50 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_8_NAME"	"Pán nájezdníkù"
		"NEW_ACHIEVEMENT_9_8_DESC"	"Získej 50 Chryzoberylù (nikoli ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_9_NAME"	"Došel jsi do Království rosolu"
		"NEW_ACHIEVEMENT_9_9_DESC"	"Najdi a získej Chutný rosol."
		"NEW_ACHIEVEMENT_9_10_NAME"	"Chutný rosol"
		"NEW_ACHIEVEMENT_9_10_DESC"	"Získej 10 Chutných rosolù (nebo 25 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_11_NAME"	"Rosol"
		"NEW_ACHIEVEMENT_9_11_DESC"	"Získej 25 Chutných rosolù (nebo 50 ve Sférostrategickém módu)."
		"NEW_ACHIEVEMENT_9_12_NAME"	"Král rosolu"
		"NEW_ACHIEVEMENT_9_12_DESC"	"Získej 50 Chutných rosolù (nikoli ve Sférostrategickém módu)."

*/

S("pics to save:", "obrázky k nahrání:")
S("level to save:", "mapa k nahrání:")
S("pics to load:", "obrázky k uložení:")
S("level to load:", "mapa k uložení:")

// === 10.3j extra texts ===
// =========================

// new option
S("forget faraway cells", "zapomínání vzdálených políček")
N("Lost Memory", GEN_F, "Ztracená paměť", "Ztracené paměti", "Ztracenou paměť", "ve Ztracené paměti")
S(  "Because of the properties of hyperbolic geometry, it is extremely unlikely to randomly "
    "get back to a faraway place you have been to. However, you have managed to get there "
    "somehow. In the meantime, its memory has been cleared, since the 'forget faraway cells'"
    " option was on.",
    
    "Díky vlastnostem hyperbolické geometrie je nesmírně nepravděpodobné, "
    "že se ti podaří náhodně se vrátit na nějaké vzdálené místo z minulosti. "
    "Ale tobě se to nějak povedlo. Vzpomínka na ně se ale mezitím vyčistila, "
    "protože byla zapnutá možnost 'zapomínání vzdálených políček'"
    )

// shmup 
N("Blueball", GEN_F, "Modrá koule", "Modré koule", "Modrou kouli", "Modrou koulí")
S("A powerful missile from a Blue Raider.", "Mocná střela Modrého nájezdníka.")

// missing
S(" fps: ", " fps: ")
S("default value", "defaultní hodnota")
S("You can scroll with arrow keys -- Ctrl to fine-tune", "Scrolluj šipkami -- Ctrl pro jemné ladění")
S("font scale", "velikost fontu")
S("background particle effects", "částicové efekty na pozadí")

// not yet translated for Hypersian Rug mode
S("orthogonal", "ortogonální")
S("perspective", "perspektiva")
S("model scale factor", "faktor škálování modelu")

// stereo vision config
S("stereo vision config", "konfigurace stereovidění")

S("This allows you to view the world of HyperRogue in three dimensions. "
  "Best used with the Hypersian Rug mode. When used in the disk model, "
  "this lets you look at the Minkowski hyperboloid (which means the "
  "depth of terrain features is actually reversed). It also works with non-disk models, "
  "from the conformal menu.",
  "Tento mód ti umožňuje dívat se na svět HyperRogue trojrozměrně. Nejlepší "
  "je kombinovat ho s módem Hyperského koberce. Při použití s diskovým modelem "
  "se můžeš dívat na Minkowskiho hyperboloid (což znamená, že hloubka "
  "terénních prvků bude převrácená). Funguje také s ne-diskovými modely "
  "z konformního menu."
  )

S("Currently, red-cyan anaglyph glasses and mobile VR googles are supported.",
  "V tuto chvíli jsou podporovány červeno-modré anaglyfové brýle a mobilní VR brýle.")

S("stereo mode", "stereomód")
S("anaglyph", "anaglyf")
S("side-by-side", "vedle sebe")

S("pupillary distance", "vzdálenost zorniček")
S("The distance between your eyes in the represented 3D object. This is given in absolute units.",
  "Vzdálenost mezi vašima očima v reprezentovaném 3D objektu. Udávaná "
  "v absolutních jednotkách.")

S("distance between images", "vzdálenost mezi obrazy")
S("The distance between your eyes. 1 is the width of the screen.",
  "Vzdálenost vašich očí. 1 je šířka obrazovky.")

// field of view
S("Horizontal field of view, in angles. "
  "This affects the Hypersian Rug mode (even when stereo is OFF) "
  "and non-disk models.",
  "Horizontální zorné pole, ve stupních. Toto nastavení ovlivňuje mód "
  "Hyperského koberce (i při vypnutém stereomódu) a ne-diskové modely.")

// more magic configuration
S("affect model central point", "ovlivňování centrálního bodu modelu")
S("affect texture position X", "ovlivňování pozice textury X")
S("affect texture position Y", "ovlivňování pozice textury Y")

S("select master cells", "výběr hlavních buněk")
S("select master triangles", "výběr hlavních trojúhelníků")
S("fine tune vertices", "doladění vrcholů")

// === 10.3n extra texts ===
// =========================

// main menu
// an option to keep the current settings
S("use current/saved settings", "použít současné/uložené nastavení")
// randomly chosen mode: hypersian rug
S("see the true form", "skutečná podoba")
// randomly chosen mode: heptagonal mode
S("more curvature", "vyšší zakřivení")

// anti-crossing in the Hypersian Rug
S("anti-crossing", "bez křížení")
S("The anti-crossing algorithm prevents the model from crossing itself, "
  "by preventing points which should not be close from being close. "
  "The bigger number, the more sensitive it is, but the embedding is slower. Set 0 to disable.",
  
  "Tento algoritmus brání tomu, aby se model křížil, a to tím, že brání "
  "přibližování bodů, které by neměly být blízko u sebe. Čím vyšší číslo, "
  "tím citlivější algoritmus je, ale tím pomalejší bude konstrukce. "
  "Hodnota 0 tuto možnost vypne."
  )

// new 3D models

// various menu names

S("constant curvature surfaces", "plochy s konstantní křivostí")
S("smooth surfaces", "hladké plochy")
S("models of hyperbolic geometry", "modely hyperbolické geometrie")
S("history mode", "mód historie") // old 'conformal/history mode' has been split into two
S("other 3D models", "další 3D modely")

// various models

S("Minkowski hyperboloid", "Minkowskiho hyperboloid")
S("plane", "rovina")
S("sphere", "koule")
S("hemisphere", "polokoule")
S("tractricoid", "traktrikoida")
S("concave barrel", "konkávní sud")
S("Dini's surface", "Diniho plocha")
S("Kuen surface", "Kuenova plocha")
S("inversion", "inverze")

S(
  "In this menu you can choose from several known smooth surfaces of constant negative curvature. "
  "Since the curvature of hyperbolic plane is also constant negative, these surfaces "
  "are great to draw hyperbolic tesselations on. While they look great, only a small part "
  "of the hyperbolic plane can be visibly represented in this way, so they are not "
  "good for playing HyperRogue; however, the coverage extends far away in specific directions, "
  "so first increasing sight range in graphics config and texture size in hypersian rug settings may improve the effect. "
  "For convenience, you can also choose other 3D models from this menu.",
  
  "V tomto menu si můžeš vybrat z několika známých hladkých ploch s konstantní "
  "zápornou křivostí. Vzhledem k tomu, že i hyperbolická rovina má konstantní "
  "zápornou křivost, jsou tyto povrchy skvělé pro kreslení hyperbolických "
  "dělení. I když vypadají skvěle, tímto způsobem je možné viditelně reprezentovat "
  "pouze malou část hyperbolické roviny, takže na hraní HyperRogue se nehodí; "
  "v některých směrech se však jejich pokrytí táhne velmi daleko, a proto se "
  "dá efekt někdy vylepšit tím, že v konfiguraci grafiky zvýšíš dohled a "
  "v nastavení hyperského koberce velikost textury. Kvůli pohodlí je z tohoto "
  "menu možné vybrat i jiné 3D modely."
  )

// coverage
S("display coverage", "zobraz pokrytí")
S("on surface", "na ploše")
S("on H² (static)", "na H² (statické)")
S("on H² (dynamic)", "na H² (dynamické)")

S("Computing these models involves integrals and differential equations, which are currently solved numerically. This controls the precision.",
  "Výpočet těchto modelů zahrnuje integrály a diferenciální rovnice, které "
  "se v současné době řeší numericky. Tento parametr řídí jejich přesnost.")

// too bored for declension of %1 (either a surface name or a a part of a surface)
S("solving the geodesics on: %1, %2/%3", "řešení geodetik na ploše: %1, %2/%3")

S("the upper component", "horní komponenta")
S("the lower center", "dole uprostřed")
S("the lower left", "vlevo dole")
S("the lower right", "vpravo dole")

S("camera rotation in 3D models", "rotace kamery ve 3D modelech")

S("Rotate the camera in 3D models (ball model, hyperboloid, and hemisphere). "
  "Note that hyperboloid and hemisphere models are also available in the "
  "Hypersian Rug surfaces menu, but they are rendered differently there -- "
  "by making a flat picture first, then mapping it to a surface. "
  "This makes the output better in some ways, but 3D effects are lost. "
  "Hypersian Rug model also allows more camera freedom.",
  
  "Umožňuje rotaci kamery ve 3D modelech (koule, hyperboloid a polokoule). "
  "Poznámka: modely hyperboloidu a polokoule jsou k dispozici také v menu "
  "ploch hyperského koberce, ale tam se renderují jiným způsobem -- tím, "
  "že se nejprve vytvoří plochý obrázek, a ten se pak namapuje na plochu. "
  "V některých ohledech pak výstup vypadá lépe, ale ztratí se tím 3D "
  "efekty. Model hyperského koberce také umožňuje větší volnost kamery.")

// parameter for the given model
S("parameter", "parametr")
S( "Stereographic projection to a sphere. Choose the radius of the sphere.",
  "Stereografická projekce na koule. Vyber si poloměr koule.")
// parameter for Dini
S("The larger the number, the more twisted it is.",
  "Čím vyšší je hodnota parametru, tím víc bude tato plocha zkroucená.")
// parameter for concave barrel
S("Controls the inner radius.", "Ovládá vnitřní poloměr.")

// improved rotation
S(
  "This controls the automatic rotation of the world. "
  "It affects the line animation in the history mode, and "
  "lands which have a special direction. Note that if finding this special direction is a part of the puzzle, "
  "it works only in the cheat mode.",
  
  "Toto nastavení ovládá automatickou rotaci světa. "
  "Ovlivňuje animace dráhy v módu historie a "
  "země se speciálním směrem. Pozor -- pokud "
  "je nalezení speciálního směru součástí hádanky, " 
  "funguje toto nastavení pouze v cheat-módu."
  )

S("NEVER", "NIKDY")

// Goldberg polyhedra

S("cube/elliptic", "krychle/el")
S("chamfered", "zkosené")
S("2x chamfered", "2x zkosené")
S("select", "výběr")

S("Goldberg polyhedra are obtained by adding extra hexagons to a dodecahedron. "
  "GP(x,y) means that, to get to a nearest non-hex from any non-hex, you should move x "
  "cells in any direction, turn right 60 degrees, and move y cells. "
  "HyperRogue generalizes this to any tesselation with 3 faces per vertex. "
  "By default HyperRogue uses bitruncation, which corresponds to GP(1,1).",
  
  "Goldbergovy mnohostěny se získají tím, že k dvanáctistěnu přidáte nějaké šestiúhelníky navíc. "
  "GP(x,y) znamená, že abyste se z ne-šestiúhelníku dostali do jiného nejbližšího "
  "ne-šestiúhelníku, musíte jít x políček libovolným směrem, potom se otočit o 60 "
  "stupňů doprava, a jít dalších y políček. HyperRogue toto zobecňuje na libovolné "
  "dělení se třemi políčky kolem každého vrcholu. HyperRogue standardně používá "
  "operaci dvojseříznutí, která odpovídá GP(1,1).")

// land compatibility guides

S("Cannot kill Hedgehog Warriors in this geometry.", "V této geometrii není možné zabít Ježčího válečníka.")
S("No random pattern version.", "Tento kraj nemá žádnou verzi s náhodným vzorem.")
S("Great Walls not implemented.", "V této geometrii nejsou implementovány Velké zdi.")
S("Pattern incompatible.", "Nekompatibilní vzor.")
S("Pattern not implemented -- using random.", "Vzor není implementován -- používám náhodný vzor.")
S("Pattern not implemented.", "Vzor není implementován.")
S("Not enough space.", "Není dost místa.")
S("Does not work in this geometry.", "V této geometrii nefunguje.")
S("This land is designed for bounded worlds.", "Tento kraj je navržen pro omezené světy.")
S("This land is designed for infinite worlds.", "Tento kraj je navržen pro nekonečné světy.")
S("This land is designed for infinite worlds or big spheres.", "Tento kraj je navržen pro nekonečné světy nebo velké koule.")
S("Out of theme for the full game.", "Nezapadá do plné hry.")
S("No game here.", "Zde se nedá hrát.")
S("Does not work in chaos mode.", "Nefunguje v chaotickém módu.")
S("Not in the full game.", "Není v plné hře.")
S("Special construction in the Chaos mode.", "Speciální konstrukce v chaotickém módu.")
S("Special construction in the Euclidean mode.", "Speciální konstrukce v eukleidovském módu.")
S("Special construction in this geometry.", "Speciální konstrukce v této geometrii.")
S("Not implemented.", "Neimplementováno.")
S("Partially implemented.", "Částečně implementováno.")
S("No comments.", "Bez komentáře.")
S("Does not work in pure tactics mode.", "Nefunguje v taktickém módu.")
S("Technical.", "Technický kraj.")
S("Full game.", "Plná hra.")
S("Somewhat inaccurate.", "Poněkud nepřesné.")
S("Mercury rivers not implemented (or could not work) in this geometry.", "Rtuťové řeky nejsou v této geometrii implementovány (nebo by v ní nefungovaly).")
S("Patterns compatible.", "Vzory jsou kompatibilní.")
S("This land is specially designed for this geometry.", "Tento kraj je speciálně navržen pro tuto geometrii.")
S("Three-colorability required.", "Tento kraj vyžaduje obarvitelnost třemi barvami.")
S("Land not implemented.", "Tento kraj není implementován.")
S("Special interest.", "Zvlášť zajímavá kombinace.")
S("Better version exists.", "Existuje lepší verze.")
S("Grid does not work in this geometry.", "Síť v této geometrii nefunguje.")
S("Graphics not implemented in this geometry.", "Grafika není v této geometrii implementována.")
S("This land does not work in the current settings. Reason not available.",
  "Tento kraj při současném nastavení nefunguje. Důvod není znám.")
S("This land does not work well in the current settings. Reason not available.",
  "Tento kraj při současném nastavení nefunguje dobře. Důvod není znám.")
S("This combination is known to be buggy at the moment.", "O této kombinaci se v současné době ví, že je chybová.")
S("Somewhat sloppy pattern.", "Poněkud nedbalý vzor.")
S("Fractal landscapes not implemented in this geometry.", "V této geometrii nejsou implementovány fraktální krajiny.")
S("Only simplified walls implemented.", "Implementovány jsou pouze jednoduché zdi.")

// new models of hyperbolic/spherical geometry, and related texts

S("Mercator", "Mercatorova projekce")
S("band equidistant", "pás, délkojevný")
S("band equi-area", "pás, plochojevný")
S("sinusoidal", "sinusoida")
S("two-point equidistant", "dvoubodová okvidistantní projekce")
S("orthogonal projection", "ortogonální projekce")
S("stereographic projection", "stereografická projekce")
S("zoomed stereographic projection", "přiblížená stereografická projekce")
S("zoomed out", "oddálit")
S("zoomed in", "přiblížit")
S("gnomonic projection", "gnómonická projekce")
S("Goldberg", "Goldberg")
S("fisheye", "rybí oko")
S("extend the ends", "rozšířit konce") // for the line animation

S(
  "This model maps the world so that the distances from two points "
  "are kept. This parameter gives the distance from the two points to "
  "the center.",
  
  "Tento model mapuje svět tak, aby zachovával vzdálenost od dvou daných "
  "bodů. Tento parametr udává vzdálenost z těchto dvou bodů do středu.")

// missing texts from the Tutorial

S("enable the Camelot cheat", "zapnout Camelotový cheat")
S("disable the Camelot cheat", "vypnout Camelotový cheat")
S("gain Orb of the Sword", "získat Sféru meče")
S("Static mode enabled.", "Statický mód zapnut.")
S("Static mode disabled.", "Statický mód vypnut.")

S("Returned to your game.", "Návrat do tvé hry.")
S("Spherical version of %the1. ", "%1 ve sférické verzi. ")
S("Euclidean version of %the1. ", "%1 v eukleidovské verzi. ")
S("Click again to go back to your game.", "Dalším kliknutím se vrátíš do své hry.")
S("Press %1 again to go back to your game.", "Dalším stisknutím %1 se vrátíš do své hry.")
S("return to your game", "návrat do hry")
S("list of slides", "seznam snímků")

// main menu

S("paint pictures", "maluj obrázky")
S("back to the start menu", "zpět do startovního menu")

// special texts for 'g' key

S("You juggle the Dead Orbs.", "Žongluješ Mrtvými Sférami.")
S("You give %the1 a grim look.", "Vrhl jsi pochmurný pohled na %a1.")

// Strange Challenge

S("Strange Challenge", "Podivná mise")
S("compete with other players on random lands in random geometries", "soupeř s jinými hráči v náhodných krajích a náhodných geometriích")

S("Strange Challenge #%1", "Podivná mise #%1")
S("next challenge in %1", "do další mise: %1")
S("synchronizing scores...", "synchronizuji skóre...")
S("already played", "už jsi hrá%l0")
S("play!", "hraj!")
S("See past challenges", "předešlé mise")
S("Past Strange Challenges. You can see the setting and high scores, but you can no longer play them.",
  "Předešlé Divné mise. Můžeš se podívat na jejich nastavení a nejvyšší skóre, ale už je nemůžeš hrát.")

S("Forbidden cells are marked with a different color.", "Zakázaná políčka jsou označena odlišnou barvou.")

S("This orb lets you attack adjacent cells %1 steps from the primary target.", "Tato Sféra ti umožňuje útočit na políčka %1 kroků od primárního cíle.")



S("Note: the rules above correspond to the standard geometry; actual rules in other geometries may be different. ",

  "Poznámka: tato pravidla platí pro standardní geometrii; v jiných geometriích se pravidla mohou lišit. ")



S("Note: the rules refer to colors which are not visible in other lands. ", "Poznámka: tato pravidla odkazují na barvy, které v jiných krajích nejsou vidět. ")



S("When the 'mark heptagons' option (hotkey '7') is on, forbidden moves are marked.", 

  "Pokud je zapnutá možnost označování sedmiúhelníků (klávesa '7'), jsou vyznačené zakázané tahy.")



S("When the 'mark heptagons' option (hotkey '7') is on, moves between unmarked cells are forbidden.",

  "Pokud je zapnutá možnost označování sedmiúhelníků (klávesa '7'), je zakázán pohyb mezi neoznačenými políčky.")

// 10.4p

// in the sight range dialog
S("enable the cheat mode for additional options", "pro další možnosti zapni cheat mód")

// steal a baby tortoise with Orb of Space
S("No, that would be heartless!", "Ne, to by bylo bezcitné!")

S("show quotient spaces", "ukaž kvocientové prostory")
S("minimal quotient", "minimální kvocient")
S("Klein Quartic", "Kleinova kvartika")
S("Bolza Surface", "Bolzův povrch")
S("Bolza Surface x2", "Bolzův povrch x2")

// extra texts for Goldberg 

S("variations", "varianty")
S("auto-adjust in Goldberg grids", "automatická úprava v Goldbergových mřížkách")
S("2x bitruncated", "2x dvojseříznutá")
S("expanded", "rozšířená")
S("rectified", "rektifikovaná")

// irregular grids

S("irregular", "nepravidelná")
S("bad cells: %1", "špatná políčka: %1")
S("too many edges: %1", "příliš mnoho hran: %1")
S("median edge: %1 minimum: %2", "medián hrany: %1 minimum: %2")
S("cells badly paired: %1", "špatně spárovaná políčka: %1")
S("cells not covered: %1", "nepokrytá políčka: %1")
S("adjacent cells from nonadjacent heptagons: %1", "sousední políčka nesousedních sedmiúhelníků: %1")
S("bad format or bad map geometry", "špatný formát nebo špatná geometrie mapy")
S("irregular grid", "nepúravidelná mřížka")
S("density", "hustota")
S("min edge to median", "min hrana / medián")
S("quality", "kvalita")
S("save", "ulož")
S("load", "nahraj")
S("irregular to save:", "nepravidelná mapa k uložení:")
S("irregular to load:", "nepravidelná mapa k nahrání:")
S("Map saved to %1", "Mapa uložena do %1")
S("Failed to save map to %1", "Uložení mapy do %1 se nezdařilo")
S("Map loaded from %1", "Mapa nahrána z %1")
S("Failed to load map from %1", "Nahrání mapy z %1 se nezdařilo")
S("reset", "reset")
S("OK", "OK")
S("ready", "hotovo")

S("This option creates irregular grids to play the game on. "
  "Currently rather slow algorithms are used, "
  "so not recommended with too high density or "
  "with too large periodic base geometry. "
  "For technical reasons, the density cannot be too small.",

  "Tato možnost vytváří nepravidelné mřížky, na kterých můžeš "
  "hrát. V současné době využívá poměrně pomalé algoritmy, a "
  "proto se ji nedoporučuje používat s příliš vysokou hustotou "
  "nebo s příliš velkým periodickým základem geometrie. Z "
  "technických důvodů nemůže být hustota příliš malá."
  )

// vertical stretch

S("vertical stretch", "svislé roztažení")
S("Vertical stretch factor.", "Faktor svislého roztažení.")
S("Gall-Peters", "Gall-Peters")
S("The current value makes the map conformal at the latitude of %1 (%2°).", "Se současnou hodnotou je mapa konformní v zeměpisné šířce %1 (%2°).")
S("The current value makes the map conformal %1 units from the main line.", "Se současnou hodnotou je mapa konformní %1 jednotek od hlavní linie.")

// line quality

S("line quality", "kvalita čar")

S("Higher numbers make the curved lines smoother, but reduce the performance.",
  "Na vyšších hodnotách budou zakřivené čáry hladší, ale zhorší se tím výkon.")

// new options in mobile

S("FIRE", "PAL")

S("scrolling by device rotation", "scrollování rotací zařízení")

S(  "This lets you scroll the map by rotating your device. It can be e.g. used to "
    "play the spherical mode of HyperRogue in mobile VR goggles -- the \"spherical VR\" "
    "button configures this; this VR mode can be disabled by touching the screen for 1 second.",

  "Tato možnost umožňuje scrollovat mapu otáčením zařízení. Dá se "
    "například použít k tomu, abyste mohli hrát sférický mód HyperRogue "
    "s mobilními VR brýlemi -- tlačítko \"sférická VR\" toto konfiguruje; "
    "tento VR mód lze vypnout tím, že se na 1 sekundu dotknete obrazovky.")

S("sensitivity", "citlivost")
S("1 means that rotating the device by 1 radian corresponds to scrolling by 1 unit. In spherical geometry, 1 unit = 1 radian.", 
  "1 znamená, že otočení zařízení o 1 radián odpovídá scrollování "
    "o 1 jednotku. Ve sférické geometrii se 1 jednotka rovná 1 radiánu.")

S("spherical VR", "sférická VR")

S("aura from texture", "aura z textury")

// extra for irregular
S("bitruncation count", "Počet dvojseříznutí")

S("Bitruncation introduces some regularity, allowing more sophisticated floor tilings and textures.", "Dvojseříznutí zavede určitou pravidelnost, což umožňuje sofistikovanější dláždění a textury.")

S(
      "The smallest allowed ratio of edge length to median edge length. "
      "Tilings with low values are easier to generate, but tend to be more ugly.", "Nejmenší povolený poměr délky strany k mediánu délky strany. Dláždění s nízkými hodnotami se snáze generují, ale bývají ošklivější.")

// 10.4q
//-------

// missing

S("The Orb of Love gives no bonus score in the Strange Challenge.", "V Podivné misi nedává Sféra Lásky žádný bonus na skóre.")
S("This does not work in non-orientable geometries", "Toto nefunguje v neorientovatelných geometriích")
S("This pattern needs x-y divisible by 3", "Tento vzor vyžaduje, aby bylo x-y dělitelné třemi")
S("This pattern needs x-y divisible by 2", "Tento vzor vyžaduje, aby bylo x-y dělitelné dvěma")

// missing from the texture editor
S("p = color", "p = barva")
S("b = brush size: %1", "b = velikost štětce: %1")
S("d = draw", "d = kresli")
S("l = line", "l = čára")
S("c = circle", "c = kruh")

// new binary tiling

S("binary tiling", "binární dělení")

// Archimedean tilings

S("Archimedean tilings", "archimédovská dělení")
S("Archimedean", "Archimédovská")

S("This lets you generate tilings where each cell is a regular polygon, and each vertex is identical. "
  "You can choose from a big list of known Archimedean tilings, or create your own. "
  "The format of specifying Archimedean tilings is based on one used by the Hyperbolic Applet by Don Hatch.\n\n"
  "You can enhance the gameplay/land generation by adding special codes after faces: "
  "H ('heptagon' for Ruined City, Graveyard, etc.), L ('line' for Zebra, Power, Vineyard, etc.).\n"
  "Crystal World and Warped Coast work on (k,l,m) tilings by removing/enlarging the first tile listed ('k').",

  "Tato volba ti umožňuje vytvářet dělení, jejichž políčka jsou pravidelné mnohoúhelníky "
  "a jejichž vrcholy jsou identické. Můžeš si vybrat z velkého seznamu známých "
  "archimédovských dělení nebo si vytvořit své vlastní. Formát pro specifikaci "
  "archimédovského dělení je založený na formátu, který používá Hyperbolic "
  "Applet od Dona Hatche.\n\n"
  "Hru/generování terénu můžeš zlepšit tím, že za stranu přidáš speciální kód: "
  "H ('sedmiúhelník' pro Zničené město, Hřbitov, atd.), L ('linie' pro "
  "Zebru, Kraj moci, Vinici, atd.).\n"
  "Křišťálový svět a Pokřivené pobřeží fungují na děleních (k,l,m) tak, "
  "že odstraní/zvětší první uvedenou hodnotu ('k').")

S("color by symmetries", "barvení pomocí symetrií")
S("color by sides", "barvení pomocí stran")

S("edit", "editace")
S("full angle", "plný úhel")
S("not enough faces", "příliš málo stěn")
S("not enough edges", "příliš málo hran")
S("polygons match incorrectly", "mnohoúhelníky nesedí")
S("currently no more than %1 edges", "momentálně maximálně %1 hran")
S("currently no more than %1 faces in vertex", "momentálně maximálně %1 stěn v jednom vrcholu")

N("Canvas", GEN_N, "Plátno", "Plátna", "Plátno", "na Plátně")
S("find the player", "k hráči")
S("press SPACE to find the player", "Mezerník = k hráči")

S("edge length", "délka hrany")
S(" (arbitrary)", " (libovolné)") // arbitrary edge length
S("previous page", "předchozí stránka")
S("Defined pattern.", "Vzor definován.")

// graphics editor
S(" (r = complex tesselations)", " (r = komplexní dláždění)")
S("area: %1", "plocha: %1")

// short codes for variations
S(" (d)", " (d)") // dual
S(" (g)", " (g)") // Goldberg
S(" (i)", " (i)") // irregular

// pattern types
S("cell types", "typy políček")
S("siblings", "sourozenci") // a pattern on the sphere
S("rotate the color groups", "rotace barevných skupin")
S("full symmetry", "úplná symetrie")
S("alternate coloring", "alternativní zbarvení")
S("display full floors", "zobrazuj úplné podlahy")
S("remove complete classification", "odstraň úplnou klasifikaci")

// 10.4y

// animations
S("animations/history", "animace/historie")
S("animations", "animace")
S("period", "perioda")
S("game animation period", "perioda herní animace")
S("no movement animation", "bez animace pohybu")
S("translation", "posunutí")
S("parabolic", "parabolická izometrie")
S("circle", "pohyb po kruhu")
S("animate parameter change", "animace změny parametrů")
S("choose parameters to animate", "vyber si parametry pro animaci")
S("frames to record", "snímky pro nahrání")
S("record to a file", "nahraj do souboru")

// improved worldsize demo dialog
S("which distance", "která vzdálenost")
S("start", "start")
S("land", "kraj")

S("number codes", "číselné kódy")
S("distance", "vzdálenost")
S("type", "typ")
S("debug", "debug")

S("color codes", "barevné kódy")
S("display distances from", "zobrazuj vzdálenosti od")
S("disable", "vyřaď")
S("move the player", "pohyb hráčem")
S("show number of cells by distance", "ukaž počet buněk podle vzdálenosti")
S("show number of descendants by distance", "ukaž počet potomků podle vzdálenosti")

S("triangle grid: tree edges", "trojúhelníková mřížka: stromové hrany")
S("triangle grid: other edges", "trojúhelníková mřížka: jiné hrany")
S("dual grid", "duální síť")
S("horocycles", "horocykly")
S("configure", "konfigurace")
S("scroll", "posouvání")

// improved sight range dialog
S("minimum visible cell in pixels", "nejmenší viditelné políčko v pixelech")
S("draw range based on distance", "rozsah vykreslování založený na vzdálenosti")
S("draw based on size in the projection (no generation)",
  "vykreslování na základě velikosti v projekci (bez generování)")
S("draw based on size in the projection (generation)",
  "vykreslování na základě velikosti v projekci (s generováním)")
S("cells drawn", "počet vykreslených políček")
S("limit on cells drawn", "hranice vykreslených políček")
S("This limit exists to protect the engine from freezing when too many cells would be drawn according to the current options.",
  "Tato hranice existuje proto, aby hra nezamrzla, když by aktuální možnosti požadovaly vykreslení příliš velkého počtu políček.")
S("generation range bonus", "bonus generačního rozsahu")
S("game range bonus", "bonus herního rozsahu")

// new options for models

S("Joukowsky transform", "Žukovského transformace")
S("Joukowsky+inversion", "Žukovský + inverze")

S("maximum z coordinate to show", "maximální zobrazená souřadnice z")

S("model transition", "přechod mezi modely")
S("You can change this parameter for a transition from another model to this one.",
  "Tento parametr je možné změnit pro přechod z jiného modelu do tohoto.")

S("model orientation", "orientace modelu")

// 10.4z

S("left-handed", "levoruk%ý0")

// formula features

S(
  "This lets you specify the projection as a formula f. "
  "The formula has access to the value 'z', which is a complex number corresponding to the (x,y) coordinates in the currently selected model; "
  "the point z is mapped to f(z). You can also use the underlying coordinates ux, uy, uz.",
  
  "Toto umožòuje specifikovat projekci ve formì vzorce f. "
  "Vzorec má pøístup k hodnotì 'z', což je komplexní èíslo odpovídající souøadnicím [x,y] v právì vybraném modelu; "
  "bod z je zobrazen na f(z). Lze také použít jeho základní souøadnice ux, uy, uz.")

S("Functions available: %1", "Dostupné funkce: %1")

S("animate parameters", "animace parametrù")

S(
  "Most parameters can be animated simply by using '..' in their editing dialog. "
  "For example, the value of a parameter set to 0..1 will grow linearly from 0 to 1. "
  "You can also use functions (e.g. cos(0..2*pi)) and refer to other parameters.",
  
  "Vìtšinu parametrù lze animovat jen tím, že v jejich editaèním dialogu napíšeš '..'. "
  "Tak napøíklad hodnota parametru nastaveného na 0..1 poroste lineárnì od 0 do 1. "
  "Lze také použít funkce (napø. cos(0..2*pi) a odkazovat na jiné parametry.")

// color edit dialog

S("background", "pozadí")
S("foreground", "popøedí")
S("projection boundary", "hranice projekce")
S("projection background", "pozadí projekce")
S("projection period", "perioda projekce")
S("dialogs", "dialogy")
S("canvas colors", "barvy plátna")
S("minefield colors", "barvy minového pole")
S("distance colors", "barvy vzdálenosti")

// extra conformal models

S("rotated hyperboles", "otoèené hyperboly")
S("spiral", "spirála")

S("spiral angle", "úhel spirály")
S("spiral period: x", "perioda spirály: x")
S("spiral period: y", "perioda spirály: y")
S("match the period of the torus", "sladìná s periodou torusu")

// extra patterns

S("circles", "kruhy")
S("radii", "polomìry")
S("meridians", "poledníky")
S("parallels", "rovnobìžky")
S("seven-coloring", "sedmibarva")

S("half-plane scale", "polorovinové měřítko")
S("use GPU to compute projections", "použít GPU k výpočtu projekcí")
S("N/A", "nedostupné")
S(" (2D only)", " (pouze 2D)")

// 10.5d

// this line was missing...
S("geometry experiments", "experimenty s geometrií")

S("cylinder (squares)", "válec (čtverce)")
S("cylinder (hex)", "válec (hexy)")
S("Möbius band (squares)", "Möbiův pásek (čtverce)")
S("Möbius band (hex)", "Möbiův pásek (hexy)")

S("Macbeath Surface", "Macbeathova plocha")
S("Bring's Surface", "Bringova plocha")
S("Schmutz's M(3)", "Schmutzův M(3)")
S("Schmutz's M(4)", "Schmutzův M(4)")

S("dimensional crystal", "dimenzionální krystal")

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
    
    "Tato geometrie ti v podstatě umožňuje hrát na d-rozměrné mřížce. Vyber si tři rozměry "
    "a '3D zobrazení', abyste viděli, jak to funguje -- v podstatě hrajeme na periodickém "
    "povrchu ve třech rozměrech tvořeném šestiúhelníky; každý šestiúhelník je spojený se "
    "šesti dalšími šestiúhelníky v každém z šesti možných směrů. Hra to normálně vizualizuje "
    "z pohledu tvora žijícího uvnitř tohoto povrchu (poněkud regularizovaného a vyhlazeného) "
    "za předpokladu, že i světelné paprsky se mohou šířit pouze po tomto povrchu -- bude to "
    "vypadat přesně jako dláždění {2d,4}, až na to, že světelné paprsky mohou tím pádem někdy "
    "udělat okruh a ty v některých směrech můžeš vidět sám sebe (jinými slovy, d-rozměrná "
    "mřížka je kvocientem dláždění hyperbolické roviny). Stejná konstrukce funguje i "
    "v jiných rozměrech. "
    "Poločíselné rozměry se interpretují následovně: přebytečný rozměr má pouze dvě "
    "'úrovně' -- například 2.5D má horní rovinu a dolní rovinu.\n\n"
    "Tuto geometrii je také možné dvojseříznout -- bude pak lépe fungovat v rámci pravidel "
    "HyperRogue, ale bude o něco obtížnější jí porozumět.")

S("4D double bitruncated", "4D dvojitě dvojseříznutý")
S("view coordinates in the cheat mode", "zobraz souřadnice v cheat módu")
S("compass probability", "pravděpodobnost kompasu")
S("3D display", "3D zobrazení")

S("\"Each piece of the Round Table is exactly %1 steps away from the Holy Grail.\"",
  "\"Každý kousek Kulatého stolu je přesně %1 kroků od Svatého grálu.\"")

S("\"According to Merlin, the Round Table is a perfect Euclidean sphere in %1 dimensions.\"",
  "\"Merlin říká, že Kulatý stůl je dokonalá eukleidovská sféra v %1 rozměrech.")

S(
  "Lands in this geometry are usually built on North-South or West-East axis. "
  "Compasses always point North, and all the cardinal directions to the right from compass North are East (this is not "
  "true in general, but it is true for the cells where compasses are generated). "
  "North is the first coordinate, while East is the sum of other coordinates.",
  
  "Země v této geometrii se obvykle budují na ose sever-jih nebo západ-východ. Kompasy "
  "vždy ukazují na sever a všechny kardinální směry napravo od kompasového severu jsou "
  "východ (toto neplatí obecně, ale platí to pro políčka, na kterých se generují kompasy). "
  "Sever je první souřadnice, zatímco východ je součet zbývajících souřadnic.")

S("crystal coordinate colors", "barvy souřadných krystalů")

S("3D monsters/walls on the surface", "3D netvoři/stěny na povrchu")
S("no 3D effects available in this projection", "v této projekci nejsou k dispozici 3D efekty")

// new screenshot menu

S("screenshots", "snímky obrazovky")
S("format", "formát")
S("pixels (X)", "pixely (X)")
S("pixels (Y)", "pixely (Y)")
S("supersampling", "supersampling")
S("transparent", "průsvitnost")
S("gamma", "gamma")
S("higher value = darker", "vyšší hodnota = tmavší")
S("brightness", "jas")
S("higher value = lighter", "vyšší hodnota = světlejší")
S("show the HUD", "zobrazuj HUD")
S("SVG screenshots do not work in this 3D mode", "SVG snímky obrazovky v tomto 3D módu nefungují")
S("SVG screenshots do not work with textures", "SVG snímky obrazovky nefungují s texturami")
S("take screenshot", "pořiď snímek obrazovky")
S("screenshot", "snímek obrazovky")
S("shot settings", "nastavení snímků obrazovky")

// new warnings

S("WARNING", "VAROVÁNÍ")
S("YES", "ANO")
S("NO", "NE")

S("Are you sure you want to hit %the1?", "Víš jistě, že chceš udeřit %a1?")
S("This will exit HyperRogue without saving your current game. Are you sure?",
  "Tímto ukončíš HyperRogue bez uložení aktuální hry. Víš jistě, že to chceš udělat?")
S("This will end your current game and start a new one. Are you sure?",
  "Tímto ukončíš současnou hru a začneš novou. Víš to jistě?")
S("This will enable the cheat mode, making this game ineligible for scoring. Are you sure?",
  "Tímto aktivuješ cheat mód a tuto hru nebude možné skórovat. Víš to jistě?")

S("octahedron", "osmistěn")

// 11.0
S("debug values", "debugové hodnoty")
S("internal details", "vnitřní detaily")
S("show debug data", "ukaž debugovací data")
S("click a cell to edit it", "klikni na políčko pro editaci")
S("click a cell to view its data", "klikni na políčko pro zobrazení jeho dat")

S("(these lands are not in game)", "(tyto země ve hře nejsou)")

// new lands

N("Brown Island", GEN_F, "Hnědý ostrov", "Hnědé ostrovy", "Hnědý ostrov", "na Hnědém ostrově")

S("The Brown Islands have been generated by ancient underground creatures, who moved randomly and raised lands in their path... "
  "adults spawned larvae in each move, which also moved randomly and also raised lands in their path, and eventually became adults and spawned their own larvae. "
  "The creatures spawned exponentially, but the space they lived in still grew faster, so there was enough space for the whole spawn...\n\n"
  "That was a long time ago. Now, the mountainous Brown Islands are inhabited by strange monsters. Good luck adventuring!",
  
  "Hnědé ostrovy vytvořily prastaré podzemní bytosti, které se náhodně pohybovaly a "
  "vyzdvihovaly půdu, která jim stála v cestě... Dospělci při každém pohybu plodili "
  "larvy, které se také pohybovaly náhodně a vyzdvihovaly půdu, která jim stála v "
  "cestě, až nakonec dospěly a začaly plodit své vlastní larvy. Tito tvorové se "
  "množili exponenciální rychlostí, ale prostor, ve kterém žili, rostli ještě "
  "rychleji, takže měly dost místa pro veškeré své potomstvo...\n\n"
  "Tohle už bylo dávno. Dnes obývají hornaté Hnědé ostrovy podivní "
  "netvoři. Hodně štěstí s dobrodružstvím!")
  

N("Bronze Beast", GEN_F, "Bronzová bestie", "Bronzové bestie", "Bronzovou bestii", "Bronzovou bestií")

S("A large bug native to the Brown Islands. Cannot be killed easily due to their tough armor; still, they can be killed by pushing them into water or from great heights. "
"Bronze Beasts are very bad at descending slopes -- it takes them extra time to get down, and they are stunned for a longer time if you push them down.",

  "Velký brouk z Hnědých ostrovů. Díky jeho tvrdému pancíři se nedá "
  "snadno zabít -- můžete to ale udělat tak, že ho strčíte do vody "
  "nebo ho necháte spadnout z velké výšky.")

N("Acid Gull", GEN_M, "Kyselinoracek", "Kyselinorackové", "Kyselinoracka", "Kyselinorackem")

S("Where did this strange bird come from?...\n\nAcid Gulls dissolve the land on which they fall when they die. ",

  "Kde se tu vzal tenhle podivný pták...?\n\nKdyž Kyselinoracek zemře, rozpustí terén, na který dopadne.")

N("Tiger's Eye", GEN_N, "Tygří oko", "Tygří oka", "Tygří oko", "Tygřím okem")
S("A brown gem.", "Hnědý drahokam.")

Orb("Choice", "Volby")

S("Did you know that it is possible to break a magical sphere into finitely many parts, and then, out of the parts obtained, to make two magical spheres, just like the original? "
  "Hyperbolic geometry makes this even easier!\n\n"
  "When you pick up another Orb, it will not disappear from the ground -- Orb of Choice will be drained instead.",
  
  "Vědě%l0 jsi, že je možné rozbít magickou sféru na konečný "
  "počet částí a složit z nich dvě magické sféry, stejné jako "
  "originál? V hyperbolické geometrii je to ještě jednodušší!\n\n"
  "Když sebereš nějakou jinou Sféru, nezmizí ze země -- "
  "místo toho přijdeš o Sféru volby.")
    
N("Free Fall", GEN_O, "Volný pád", "Volné pády", "Volný pád", "ve Volném pádu")

S("What on one side looks to be a normal (well, infinite) horizontal wall, on the other side turns out to be the vertical wall"
  " of an infinitely high tower. Jump from the window, and let the magical gravity carry you...",

  "To, co na jedné straně vypadá jako normální (no, nekonečná) vodorovná "
  "stěna, se na druhé straně ukazuje být svislou zdí nekonečně vysoké věže. "
  "Vyskoč z okna a nech magickou gravitaci, aby tě unášela...")

N("Falling Dog", GEN_M, "Padající pes", "Padající psi", "Padajícího psa", "Padajícím psem")

S("Distant relatives of the Running Dogs.", "Vzdálení příbuzní Běžících psů.")

N("Western Hawk", GEN_M, "Západní jestřáb", "Západní jestřábi", "Západního jestřába", "Západním jestřábem")

N("Meteorite", GEN_O, "Meteorit", "Meteority", "Meteorit", "Meteoritem")

S("These rocks falling from the sky have been captured to fall forever in the artificial gravity. Meteorite iron is believed to be a valuable material for magical weapons.",
  "Tyto z nebe spadlé kameny byly zachyceny, aby navěky padaly "
  "v umělé gravitaci. Meteorické železo se pokládá za cenný "
  "materiál pro magické zbraně.")

Orb("Gravity", "Gravitace")

S("This Orb lets you magically control gravity around you. In lands with unusual gravity, the options are: usual gravity (no change, except that it may disrupt birds), "
  "anti-gravity (causes the direction of gravity to be reversed), levitation (you can move in directions which are neither up nor down, "
  "or between cells if one of them has a wall in a 'neutral' direction). In lands with standard gravity, levitation lets creatures to avoid traps, chasms, "
  "and water, while anti-gravity makes it possible to move only when next to a wall (movement between cells without adjacent walls is not allowed). "
  "For each move, the gravity is based on what you do, and all enemies in the effective range are affected (for example, if you move next to a wall in a non-gravity land, "
  "anti-gravity kicks in and monsters will not be able to move in open space).",
  
  "Tato Sféra ti umožňuje magicky ovládat gravitaci kolem tebe. V zemích s neobvyklou "
  "gravitací máš následující možnosti: obvyklá gravitace (beze změny, ale může rušit "
  "ptáky), antigravitace (směr gravitace je obrácen), levitace (můžeš se pohybovat ve "
  "směrech, které nevedou nahoru ani dolů, nebo mezi políčky, pokud má jedno z nich zeď "
  "v 'neutrálním' směru. V zemích se standardní gravitací umožňuje levitace tvorům "
  "vyhnout se pastím, propastem a vodě, zatímco antigravitace způsobí, že se mohou "
  "pohybovat pouze tehdy, když jsou vedle zdi (pohyb mezi políčky bez sousedících "
  "zdí není dovolen). Při každém pohybu je gravitace založená na tom, co uděláte, "
  "a ovlivňuje všechny nepřátele v efektivním dosahu (pokud se například v "
  "negravitační zemi pohneš vedle zdi, spustí se antigravitace a netvoři nebudou "
  "schopní pohybovat se na otevřeném prostranství).")

N("Irradiated Field", GEN_N, "Ozářené pole", "Ozářená pole", "Ozářené pole", "na Ozářeném poli")

S("These fields are ravaged with many kinds of magical radiation, which not only make the ground glow nicely in various colors, "
  "but also cause the inhabitants to protect the treasures of their land in various ways. In some areas of the Irradiated Fields, "
  "you will meet powerful enemies; in other areas, you will find arrow traps, fire traps, or pools, which can kill you if you are "
  "careless, or help you if you know how to use them to your advantage. Will you walk through the Irradiated Fields randomly, or "
  "try to find areas where treasures are common but nasty monsters are not, and keep to them? It is your choice!\n\n",
  
  "Tato pole jsou zpustošená mnoha druhy magického záření, což nejenom způsobuje, že "
  "zem pěkně různobarevně svítí, ale také to její obyvatele vede k tomu, že poklady "
  "svého kraje chrání různými způsoby. V některých oblastech Ozářených polí se setkáš "
  "s mocnými nepřáteli; v jiných najdeš šípové pasti, ohnivé pasti nebo jezírka, která "
  "tě mohou zabít, pokud si nedáš pozor, nebo ti pomoci, pokud víš, jak je využít ve "
  "tvůj prospěch. Budeš Ozářenými poli procházet náhodně, nebo se pokusíš najít "
  "oblasti, kde je mnoho pokladů, ale málo ošklivých netvorů a držet se jich? Je to na tobě.")

N("Mutant", GEN_M, "Mutant", "Mutanti", "Mutanta", "Mutantem")

S("These guys look a bit strange, but they have no special properties.",
  "Tihle chlapíci vypadají trošku podivně, ale nemají žádné speciální vlastnosti.")

N("Torbernite", GEN_O, "Torbernit", "Torbernity", "Torbernit", "Torbernitem")

S("Crystals emitting magical radiation.", "Drahokamy vyzařující prospěšnou magickou radiaci.")

N("fire trap", GEN_F, "ohnivá past", "ohnivé pasti", "ohnivou past", "ohnivou pastí")

S("This trap will explode when stepped on, setting all the adjacent cells on fire. However, this happens on the next turn, "
  "so you can safely escape if you are fast enough.",
  "Když na tuto past někdo šlápne, vybuchne a zapálí všechna okolní "
  "políčka. To se však stane až v příštím kole, takže pokud jsi dostatečně "
  "rychlý, můžeš bezpečně uniknout.")

N("Explosive Barrel", GEN_O, "Sud s výbušninami", "Sudy s výbušninami", "Sud s výbušninami", "Sudem s výbušninami")
S("These barrels can be pushed, and they will explode if next to a fire, or under some attacks.",
  "Tyto sudy je možné tlačit; vybuchnou, pokud se ocitnou vedle ohně nebo je zasáhnou určité útoky.")

Orb("Intensity", "Intenzity")

S("When you have this, initial and maximal charge amounts of all Orbs are increased by 20%.",
  "Pokud máš tuto sféru, počáteční i maximální náboj všech Sfér se zvyšuje o 20%.")

// new mode: Racing

S("Racing available only in unbounded worlds.", "Závody jsou k dispozici pouze v neohraničených světech.")
S("select the track and start!", "vyber si dráhu a začni!")
S("configure the projection", "konfigurace projekce")
S("racing projections", "závodní projekce")
S("Poincaré disk model", "model Poincarého disku")
S("third-person perspective", "perspektiva třetí osoby")
S("point of view", "bod pohledu")
S("track", "dráha")
S("race angle", "úhel závodu")
S("show more in front", "ukazuj víc vpředu")
S("guiding line", "vedoucí linie")
S("track seed", "seed závodu")
S("play the official seed", "hraj oficiální seed")
S("play a random seed", "hraj náhodný seed")
S("best scores to show as ghosts", "ukaž nejlepší skóre jako duchy")
S("best scores to save", "nejlepší skóre k uložení")
S("disable the racing mode", "zruš závodní mód")
S("racing menu", "závodní menu")
S("Race did not generate correctly for some reason -- not ranked",
  "Závod se z nějakého důvodu nevygeneroval správně -- není hodnocen")

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
    
  "Hnědé ostrovy vytvořily prastaré podzemní bytosti, které se náhodně pohybovaly a "
  "vyzdvihovaly půdu, která jim stála v cestě... Dospělci při každém pohybu plodili "
  "larvy, které se také pohybovaly náhodně a vyzdvihovaly půdu, která jim stála v "
  "cestě, až nakonec dospěly a začaly plodit své vlastní larvy. Tito tvorové se "
  "množili exponenciální rychlostí, ale prostor, ve kterém žili, rostli ještě "
  "rychleji, takže měly dost místa pro veškeré své potomstvo...\n\n"
  "Tohle už bylo dávno. Dnes obývají hornaté Hnědé ostrovy podivní "
  "netvoři. Hodně štěstí s dobrodružstvím!\n\n"
  "Různé části Hnědých ostrovů mají různou výšku, od 0 do 3. Není možné "
  "při jednom pohybu získat dvě či více úrovní výšky nebo ztratit tři "
  "úrovně (útoky jsou však možné s libovolným výškovým rozdílem). Zabité "
  "Bronzové bestie zvyšují úroveň půdy o 1."
  )

S(
    "Some readers misinterpreted the early maps of Free Fall, thinking that it is a land to the west from some wall. "
    "The name Western Hawks remained.",
    
    "Někteří čtenáři si rané mapy Volného pádu vyložili chybně a mysleli si, že je "
    "to země na západ od nějaké zdi. Jméno 'Západní jestřáb' zůstalo.")

N("Torbernite", GEN_O, "Torbernit", "Torbernity", "Torbernit", "Torbernitem")

S("Crystals emitting magical radiation.", "Krystaly vyzařující magickou radiaci.")

// other things:

S("pure", "základní") // non-bitruncated

S(" (level %1)", " (úroveň %1)")

S("fixed facing", "fixní orientace")
S("configure TPP automatically", "automatická konfigurace TPP")

S("Finished the race! Time: %1, place: %2 out of %3", "Dorazi%l0 jsi do cíle! Čas: %1, umístění: %2 z %3")
S("Finished the race in time %1!", "Dotar%łeś0 do mety w czasie %1!")

S("Welcome to HyperRogue! (cheat mode on)", "Vítej v HyperRogue! (cheat mód zapnutý)")
S("Welcome to the Heptagonal Mode!", "Vítej v Sedmiúhelníkovém módu!")

S("adult Tortoise flags", "flagy dospělých Želv")
S("baby Tortoise flags", "flagy Želviček")

S("Map settings", "Nastavení mapy")

S("disable wandering monsters","vypnout bludné netvory")
S("disable ghost timer", "vypnout časové duchy")
S("simple pattern generation", "jednoduché generování vzorů")
S("(e.g. pure Reptile pattern)", "np. základní vzor Ještěrů")
S("safety generation", "bezpečné generování")
S("(no treasure, no dangers)", "(žádné poklady a nebezpečí)")
S("god mode", "božský mód")

S("(unlock all, allow cheats, normal character display, cannot be turned off!)",
  "(vše odemknout, povolit cheaty, normální zobrazení postavy -- nelze vypnout!)")

S("change the pattern/color of new Canvas cells", "změň vzor/barvu nových políček Plátna")

S("racing mode", "závodní mód")
S("Racing", "Závody")

S("octahedron", "osmistěn")

S("\"Did you know that the Cultists are relatives of the Desert Men?\"", 
  "\"Vědě%l0 jsi, že Kultisté jsou příbuzní s Pouštními muži?\"")                      

S("do not use special centering for racing", "nepoužívat speciální centrování pro závody")

S("F8 = settings", "F8 = nastavení")

// 11.0p

// standarization of geometry names

S("{7,3} (standard HyperRogue map)", "{7,3} (standardní mapa HyperRogue)")
S("{6,3} (Euclidean grid)", "{6,3} (Eukleidovská šestiúhelníková mřížka)")
S("{5,3} (dodecahedron)", "{5,3} (dvanáctistěn)")
S("elliptic geometry in {5,3}", "eliptická geometrie v {5,3}")
S("torus/Klein bottle/...", "torus/Kleinova láhev/...")
S("{8,3} (like standard but with octagons)", "{8,3} (jako standard, ale s osmiúhelníky)")
S("{5,4} (four pentagons)", "{5,4} (čtyři pětiúhelníky)")
S("{6,4} (four hexagons)", "{6,4} (čtyři šestiúhelníky)")
S("{7,4} (four heptagons)", "{7,4} (čtyři sedmiúhelníky)")
S("{4,3} (cube)", "{4,3} (krychle)")
S("{3,3} (tetrahedron)", "{3,3} (čtyřstěn)")
S("{4,4} (Euclidean square grid)", "{4,4} (Eukleidovská čtvercová mřížka)")
S("elliptic geometry in {4,3}", "eliptická geometrie ve {4,3}")
S("{3,4} (octahedron)", "{3,4} (osmistěn)")

// new 3D geometries
                     
S("variant of the binary tiling", "varianta binárního dělení")
S("{4,3,4} cube tiling", "{4,3,4} krychlové dělení")
S("rhombic dodecahedral honeycomb", "rhombické dvanáctistěnové dělení")
S("bitruncated cubic honeycomb", "dvojseříznuté krychlové dělení")

S("{4,4} on horospheres", "{4,4} na horosférách")
S("{3,6} on horospheres", "{3,6} na horosférách")
S("{6,3} on horospheres", "{6,3} na horosférách")
S("rectangles on horospheres", "obdélníky na horosférách")

// quotient space abbreviations
S("minimal", "minimální")
S("Klein", "Klein")
S("Bolza", "Bolza")
S("Bolza2", "Bolza2")
S("Macbeath", "Macbeath")
S("Bring", "Bring")
S("M3", "M3")
S("M4", "M4")
S("Crystal", "Krystal")

#define Cell(x) \
  S(x "-cell", x "-nadstěn") \
  S(x "-cell (elliptic space)", x "-nadstěn (eliptický prostor)")

Cell("{3,3,3} 5") Cell("{4,3,3} 8") Cell("{3,3,4} 16") Cell("{3,4,3} 24") Cell("{5,3,3} 120") Cell("{3,3,5} 600")
#undef Cell

#define Honeycomb(x) \
  S(x " hyperbolic honeycomb", x " hyperbolické dělení") \
  S(x " field quotient space", x " prostor s tělesovým kvocientem")

Honeycomb("{5,3,4}") Honeycomb("{4,3,5}") Honeycomb("{3,3,6}") Honeycomb("{3,4,4}") Honeycomb("{5,3,5}") Honeycomb("{5,3,6}") Honeycomb("{4,3,6}") Honeycomb("{5,3,6}")
#undef Honeycomb

// new or previously untranslated options
S("dimension", "rozměr")
S("pattern", "vzor")
S("Euler characteristics", "Eulerova charakteristika")
S("demigenus", "neorientovatelný genus")
S("number of mines", "počet min") // for Minefield in bounded spaces
S("advanced parameters", "pokročilé parametry")

// models & projections

S("disk/Gans", "disk/Gansův model")
S("spiral/ring", "spirála/prstenec")
S("native perspective", "domorodá perspektiva")
S("azimuthal equi-volume", "azimutální, objemojevný")
S("ball model/Gans", "kulový/Gansův model")
S("perspective in 4D", "4D perspektiva")
S("half-space", "poloprostor")

// projection configuration

S("projection distance", "projekční vzdálenost")
S("rotation (x/z plane)", "rotace (rovina x/z)")
S("model orientation (y/z plane)", "orientace modelu (rovina y/z)")

S("near clipping plane", "blízká klipovací rovina")
S("far clipping plane", "vzdálená klipovací rovina")
S(
  "Your view of the 3D model is naturally bounded from four directions by your window. "
  "Here, you can also set up similar bounds in the Z direction. Radius of the ball/band "
  "models, and the distance from the center to the plane in the halfspace model, are 1.\n\n",
  
  "Tvůj pohled na 3D model je ve čtyřech směrech přirozeně omezený vaším oknem. Zde "
  "můžeš nastavit podobné hranice i ve směru osy Z. Poloměr kulových/pásových modelů "
  "a vzdálenost od středu k rovině v poloprostorovém modelu je 1.\n\n")

S("Objects with Z coordinate "
  "bigger than this parameter are not shown. This is useful with the models which "
  "extend infinitely in the Z direction, or if you want things close to your character "
  "to be not obscured by things closer to the camera.",
  
  "Objekty se souřadnicí Z větší než tento parametr nejsou zobrazovány. Toto je "
  "užitečné pro modely s nekonečnými rozměry ve směru Z, nebo pokud chceš, aby "
  "objekty poblíž tvé postavy nebyly zakrývány objekty, které jsou blíže ke kameře.")

S("Objects with Z coordinate "
  "smaller than this parameter are not shown; it also affects the fog effect"
  " (near clipping plane = 0% fog, far clipping plane = 100% fog).",
  
  "Objekty se souřadnicí Z menší, než je tento parametr, se nezobrazují; "
  "také to ovlivňuje efekt mlhy (blízká klipovací rovina = 0% mlhy, "
  "vzdálená klipovací rovina = 100% mlhy).")

S("set to 90° for the ring projection", "Pro prstencovou projekci nastav na 90°")

S("spiral multiplier", "spirálový multiplikátor")
S(
  "This parameter has a bit different scale depending on the settings:\n"
  "(1) in spherical geometry (with spiral angle=90°, 1 produces a stereographic projection)\n"
  "(2) in hyperbolic geometry, with spiral angle being +90° or -90°\n"
  "(3) in hyperbolic geometry, with other spiral angles (1 makes the bands fit exactly)",
  
  "Měřítko tohoto parametru se poněkud liší v závislosti na nastavení:\n"
  "(1) ve sférické geometrii (stereografická projekce vznikne se spirálovým úhlem 90°)\n"
  "(2) v hyperbolické geometrii, se spirálovým úhlem +90° nebo -90°\n"
  "(3) v hyperbolické geometrii, s jinými spirálovými úhly (hodnota 1 způsobí, "
  "že pásy budou přesně pasovat)\n"
  )

// line animation

S("animate from start to current player position", "animace od začátku do současné polohy hráče")
S("animate from last recenter to current view", "animace od posledního vycentrování k současnému pohledu")

// 3D settings for 3D mode

S("3D sight range", "viditelnost ve 3D")
S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "Viditelnost ve 3D geometriích je specifikovaná v absolutních jednotkách. "
  "Tato hodnota také ovlivňuje efekt mlhy.\n\n"
  "Ve sférických geometriích vám viditelnost 2π umožní vidět věci za vámi, "
  "jako by byly před vámi, a viditelnost π (nebo víc) vám umožní vidět věci "
  "ve vašem protipólu, jako kdyby byly blízko vás.\n\n"
  "V hyperbolických geometriích počet renderovaných buněk exponenciálně "
  "závisí na viditelnosti. Více vykreslovaných buněk zhoršuje výkon.\n\n"
  "Viditelnost ovlivňuje hru; netvoři jednají právě tehdy, jsou-li viditelní. "
  "Generování netvorů to bere v úvahu.")

S("limit generation", "omezení generování")
S("Cells over this distance will not be generated, but they will be drawn if they are already generated and in the sight range.",
  "Buňky ve větší vzdálenosti nebudou generovány, ale pokud už jsou vygenerovány a "
  "viditelné, budou se vykreslovat.")

S("inverted Poincaré model", "inverzní Poincarého model")
S("mouse aiming sensitivity", "citlivost zaměřování myší")
S("set to 0 to disable", "pro vypnutí nastav na 0")
S("radar size", "velikost radaru")
S("Height to width", "výška : šířka")
S("Creature scale", "měřítko bytostí")

// not only in 3D mode
S("desaturate colors", "desaturované barvy")
S("Make the game colors less saturated. This is useful in the anaglyph mode.",
  "Snižuje sytost barev. Užitečné pro anaglyfový mód.")

S("brightness behind the sphere", "jas za koulí")
S("In the orthogonal projection, objects on the other side of the sphere are drawn darker.",
  "V ortogonální projekci se objekty na druhé straně sféry vykreslují tmavěji.")

// restructured menus

S("colors & aura", "barvy a aura")
S("colors of some game objects can be edited by clicking them.", "barvy některých herních objektů lze editovat kliknutím.")
S("standard grid color", "standardní barva mřížky")
S("mouse & touchscreen", "myš a doteková obrazovka")
S("settings", "nastavení")
S("interface", "rozhraní")
S("general graphics", "obecná grafika")
S("quick options", "rychlé možnosti")
S("models & projections", "modely a projekce")
S("keyboard & joysticks", "klávesnice a joysticky")
S("other settings", "další nastavení")
S("special modes", "zvláštní módy")
S("creative mode", "kreativní mód")
S("multiplayer", "multiplayer")

// extra / previously untranslated options

S("smooth scrolling", "hladké scrollování")
S("change slideshow", "změnit slideshow")
S("move by clicking on compass", "pohyb klikáním na kompas")
S("disable the HUD", "vypnout HUD")
S("hide the player", "skrýt hráče")

// redefinable key in 3D shmup mode
S("scroll forward", "scrollování dopředu")
S("scroll backward", "scrollování dozadu")

S("note: configured input is designed for", "pozunámka: konfigurovaný vstup je určen pro")
S("multiplayer and shmup mode; some features", "multiplayer a mód střílečky; některé prvky")
S("work worse if you use it.", "budou fungovat hůř, pokud ho budete používat.")

// quick options in 3D
S("first person perspective", "perspektiva první osoby")
S("fixed point of view", "pevný bod pohledu")
S("third person perspective", "perspektiva třetí osoby")

S("Hint: these keys usually work during the game", "Nápověda: tyto klávesy obvykle fungují během hry")

// 3D graphics editor

S("l = color group: %1", "l = skupina barev: %1")
S("z = z-level", "z = z-hladina")
S("Changing the z-level will make this layer affected by the parallax effect.", "změna z-hladiny způsobí, že tato vrstva bude ovlivněná efektem paralaxy.")
S("The distance from the camera to added points.", "Vzdálenost mezi kamerou a přidanými body.")

S("This land has been disabled with compilation flags.", "Tento kraj byl vypnut kompilačními flagy.")
S("This land does not make much sense in 3D.", "Tento kraj nedává ve 3D velký smysl.")
S("This land does not make much sense in binary tiling.", "Tento kraj nedává v binárním dělení velký smysl.")
S("Special pattern implemented for this geometry.", "Speciální vzor implementovaný pro tuto geometrii.")
S("This land works only in the shmup mode.", "Tento kraj funguje pouze v módu střílečky.")

// new land

N("Space Rocks", GEN_F, "Vesmírné skály", "Vesmírné skály", "Vesmírnými skalami", "ve Vesmírných skálách")
N("Space Rock", GEN_F, "Vesmírná skála", "Vesmírné skály", "Vesmírnou skálu", "Vesmírnou skálou")
N("Fuel", GEN_N, "Palivo", "Palivo", "Palivo", "Palivem")
S("Shoot the Space Rocks for score. Large Rocks will split into two smaller rocks.",
  "Střílej do Vesmírných skal a získávej za to body. Velké skály se rozpadnou na dvě menší.")

N("editable statue", GEN_F, "editovatelná socha", "editovatelné sochy", "editovatelnou sochu", "editovatelnou sochou")
S("These statues are designed to have their graphics edited in the Vector Graphics Editor. Each number has its own, separate graphics.",
  "Tyto sochy jsou určeny k tomu, aby se ve vektorovém grafickém editoru měnila jejich grafika. Každé číslo má svou vlastní, odlišnou grafiku.")

S("The Orb of Yendor is locked in with powerful magic.", "Yendorská Sféra je uzamčena mocnou magií.")

S("general perspective", "obecná perspektiva")
S("formula", "vzorec")

S("limit generated cells per frame", "limit počtu generovaných políček na snímek")

S("In the 3D mode, lowering this value may help if the game lags while exploring new areas.",
  "Snížení této hodnoty může ve 3D pomoci, pokud se hra zadrhává při zkoumání nových oblastí zadrhává.")
  
S("sloppy range checking", "nedbalá kontrola rozsahu")

S("fog effect", "efekt mlhy")
S("memory configuration", "konfigurace paměti")

S("wall quality", "kvalita stěn")

S(
  "Controls the number of triangles used for wall surfaces. "
  "Higher numbers reduce the performance. "
  "This has a strong effect when the walls are curved indeed "
  "(floors in 2D geometries, honeycombs based on horospheres, and projections other than native perspective), "
  "but otherwise, usually it can be set to 1 without significant adverse effects other "
  "than slightly incorrect texturing.",
  
  "Ovládá počet trojúhelníků použitých na povrchy stěn. "
  "Vyšší hodnoty snižují výkon. "
  "Toto má silný efekt, když jsou stěny opravdu zakřivené "
  "(dělení založená na horosférách a projekce jiné než domorodá), "
  "ale jinak to obvykle lze nastavit na 1 a nebude to mít žádné významné negativní následky "
  "s výjimkou mírně nesprávného texturování.")

S("use the full 3D models", "používej úplné 3D modely")

S("Z shift", "Z-posuv")
S("fixed Y/Z rotation", "stálá Y/Z rotace")

S("configure FPP automatically", "automatická konfigurace FPP")

S("Look from behind.", "Pohled zezadu.")

S("reduce if walls on the way", "zmenši, pokud jsou v dráze stěny")

S("The camera is currently exactly on the plane P. "
  "The horizon is seen as a straight line.",
  
  "V tuto chvíli je kamera přesně na rovině P. Obzor je vidět jako přímka.")

S(
  "The camera is currently %1 units above the plane P. "
  "This makes you see the floor level as in general perspective projection "
  "with parameter %2.", 
  
  "V tuto chvíli je kamera %1 jednotek nad rovinou P. "
  "To znamená, že úroveň podlahy je vidět stejně jako v obecné perspektivní projekci s parametrem %2.")

S("In the FPP mode, the camera will be set at this altitude (before applying shifts).", 
  "V módu FPP bude kamera nastavena na tuto výšku "
  "(před aplikací posuvů).")

S("auto-adjust to eyes on the player model", "automaticky upravit podle očí modelu hráče")

S("eye color", "barva očí")

// rotation (line animation etc.)

S("XY plane", "rovina XY")
S("XZ plane", "rovina XZ")
S("XY plane #2", "rovina XY #2")

// cube honeycomb

S("twisting implemented only for diagonal matrices", "kroucení implementováno pouze pro diagonální matice")
S("nothing to twist", "není co kroutit")
S("change the bottom left corner", "změna levého dolního rohu")

S("flip X coordinate", "převrátit souřadnici X")
S("flipping X impossible", "X nelze převrátit")
S("flip Y coordinate", "převrátit souřadnici Y")
S("flipping Y impossible", "Y nelze převrátit")
S("flip X coordinate", "převrátit souřadnici X")
S("swap X and Y", "záměna X a Y")
S("swapping impossible", "záměnu nelze provést")

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

  "Tato matice umožňuje hrát v kvocientových prostorech trojrozměrného eukleidovského "
  "prostoru. Každý sloupec specifikuje translační vektor, který vás zavede zpět "
  "do výchozího bodu. Pokud například zadáte na hlavní diagonále 2, 6 a 0, "
  "vrátíte se do výchozího bodu po dvou krocích ve směru X nebo šesti "
  "krocích ve směru Y (ve směru Z je kvocientový prostor nekonečný).\n\n"
  "U diagonálních matic můžete také vytvořit zkroucení: po určitém počtu "
  "kroků ve směru Z bude prostor zrcadlený nebo otočený. (Obecnější 'zkroucené' "
  "prostory nejsou v současné době implementovány.)")

// map editor 

S("pick something", "něco vyber") // to edit

S("z = camera", "z = kamera")
S("Z = spheres", "z = sféry")
S("z = equi", "z = ekvi")

S("The distance from the camera to added points.", "Vzdálenost od kamery k přidaným bodům.")
S("place points at fixed radius", "umísťování bodů v pevně daném poloměru")
S("place points on equidistant surfaces", "umísťování bodů na ekvidistantních površích")
S("mousewheel step", "krok kolečka myši")
S("hint: shift for finer steps", "nápověda: stiskni Shift pro jemnější kroky")

S("random walk", "náhodná procházka")

// dual like two at once, not in mathematical sense
S("dual geometry mode", "mód dvojité geometrie")

S("Impossible.", "Nemožné.")

S("subgame affected", "ovlivněná podhra")
S("both", "obě")

// changing the rug model distance
S("adjust points", "upravení bodů")
S("center on camera", "vycentrování na kameře")
S("adjust edges", "úprava hran")
S("adjust distance", "úprava vzdálenosti")

S("memory", "paměť")

S(
  "HyperRogue takes place in a world that is larger than anything Euclidean. "
  "Unfortunately, in some cases running it on an Euclidean computer might be "
  "a problem -- the computer could simply run out of memory. Some lands (such as the Ocean or the Brown Islands) "
  "may use up memory very fast!\n\n",
  
  "HyperRogue se odehrává ve světě, který je větší než cokoli eukleidovského. "
  "Bohužel, v některých případech může při hře na eukleidovském počítači nastat "
  "problém -- počítači může prostě dojít paměť. Některé kraje (například Oceán "
  "nebo Hnědé ostrovy) mohou paměť spotřebovat velice rychle!\n\n")
  
S("You are playing a 32-bit HyperRogue executable, which can only use 4GB of memory.\n\n",
  "Hraješ HyperRogue z 32-bitového spustitelného souboru, který může využívat pouze 4 GB paměti.\n\n")

S(
  "Although you are extremely unlikely to return to a place you have already been to, "
  "the game never forgets these areas, unless you start a new game, use an Orb of "
  "Safety (found in Land of Eternal Motion, the Prairie, and the Ocean), or activate the memory "
  "saving mode, which tries to intelligently predict which cells you will never find "
  "again and can be safely forgotten.\n\n",
  
  "Přestože je nesmírně nepravděpodobné, že by ses vrátil na místo, kde už jsi byl, "
  "hra tyto oblasti nikdy nezapomíná, dokud nezačneš novou hru, nepoužiješ Sféru "
  "Bezpečnosti (která se nachází v Kraji věčného pohybu, na Prérii a v Oceánu) "
  "nebo neaktivuješ mód šetření pamětí, který se snaží inteligentně předvídat, "
  "která políčka už nikdy znovu nenajdeš a mohou tedy být bezpečně zapomenuta.")

S("cells in memory", "políčka v paměti")

S("memory saving mode", "mód šetření pamětí")
S("show memory warnings", "zobrazovat varování o paměti")

S("just let me find Orb of Safety or finish the game", "prostě mě nech najít Sféru Bezpečnosti nebo dokončit hru")
S("memory reserve", "paměťová rezerva")
S("When to show a memory warning.", "Kdy zobrazovat varování o paměti.")
S("clear caches", "vyčistit mezipaměť")

// in animation menu
S("shmup action", "akce v módu střílečky")

// 11.2

// extra projections
S("central inversion", "středová inverze")
S("two-point azimuthal", "dvoubodová azimutální")
S("two-point hybrid", "dvoubodová hybridní")
S("Mollweide", "Mollweideova")
S("central cylindrical", "centrální válcová")
S("Collignon", "Collignonova")

// extra settings

S("draw crosshair", "kresli zaměřovací kříž")

S("Display a targetting reticle in the center of the screen. Might be useful when exploring 3D modes, "
  "as it precisely shows the direction we are going. However, the option is available in all modes.",
  
  "Zobrazí uprostřed obrazovky zaměřovací kříž. Může se hodit při zkoumání 3D módů, "
  "protože přesně ukazuje směr, kterým směřuješ. K dispozici je nicméně ve všech módech.")

S("radar range", "dosah radaru")

S("boundary width multiplier", "hranice s multiplikátorem")
S("grid width multiplier", "mřížka s multiplikátorem")
S("highlight the cell forward", "zvýraznit buňku vepředu")
S("edit widths individually", "editovat šířky jednotlivě")

// extra tilings

S("kite-and-dart", "Penroseovo dláždění")
S("kite-and-dart on horospheres", "Penroseovo dláždění na horosférách")
S("standard binary tiling", "standardní binární dláždění")

S("show quotient spaces", "ukaž kvocientové prostory")
S("three-dimensional", "trojrozměrné")

// non-isotropic geometries:

S("Solv geometry", "geometrie Solv")
S("Sol", "Sol")

S(
  "Solv (aka Sol) is a 3D space where directions work in different ways. It is described by the following metric:\n"
  "ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  "You are currently displaying Solv in the perspective projection based on native geodesics. You can control how "
  "the fog effects depends on the geodesic distance, and how far object in X/Y/Z coordinates are rendered.", 
  
  "Solv (též Sol) je 3D prostor, ve kterém směry fungují odlišně. je popsán následující metrikou: "
  "ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  
  "V tuto chvíli se Solv zobrazuje v perspektivní projekci založené na geodetikách. Můžeš ovládat, "
  "jak má efekt mlhy záviset na geodetické vzdálenosti a jak vzdálené objekty ve směrech "
  "X/Y/Z se mají vykreslovat.")

S("max difference in X/Y coordinates", "maximální rozdíl v souřadnicích X/Y")
S("max difference in Z coordinate", "maximální rozdíl v souřadnici Z")

S("geodesic movement in Sol/Nil", "geodetický pohyb v Sol/Nil")

// ---

S("Nil geometry", "geometrie Nil")
S("Nil", "Nil")

// ---

S("product space", "součinový prostor")
S("current geometry x E", "současná geometrie x E")
S("Only works with 2D non-Euclidean geometries", "Funguje pouze s 2D neeukleidovskými geometriemi")
S("%1 x E", "%1 x E")
S("Z-level height factor", "faktor výšky úrovně Z")

S("precision of S2xE rings", "přesnost prstenců v S2xE")

S(
  "In S2xE, objects at spherical distances which are multiples of π will look like "
  "rings, and objects close to these will look like crescents. "
  "This setting controls the quality of rendering these rings and crescents.",
  
  "V S2xE, se objekty ve sférické vzdálenosti, která je násobkem π, "
  "budou jevit jako prstence a objekty v blízkosti této vzdálenosti jako srpky. "
  "Toto nastavení řídí kvalitu vykreslování těchto prstenců a srpků.")

// ---

S("rotation space", "prostor rotací")
S("steps", "kroky")

// 11.3
//======

// new geometries

S("space of rotations in current geometry", "prostor rotací v současné geometrii")
S("~SL(2,R)~", "~SL(2,R)~")
S("Only works with (semi-)regular tilings", "Funguje pouze s (polo)pravidelnými dlážděními")
S("That would have %1/%2 levels", "To by mělo %1/%2 úrovní")
S("number of levels", "počet úrovní")
S("Hint: this is more playable with pure {7,3} or pure {5,4}", "Nápověda: toto je hratelnější v čistém {7,3} nebo čistém {5,4}")

S("view the underlying geometry", "zobraz základní geometrii")

S("The space you are currently in is the space of rotations of the underlying hyperbolic or spherical geometry. ",
  "Prostor, ve kterém se právě nacházíš, je prostorem rotací základní hyperbolické nebo sférické geometrie. ")

S("This option lets you see the underlying space. Lands and some walls (e.g. in the Graveyard) are based on "
  "the respective features in the underlying world, but details such as monsters or items are ignored.",
  
  "Tato možnost ti umožňuje vidět tento základní prostor. Kraje a některé stěny (např. na Hřbitově) "
  "jsou založeny na prvcích základního světa, ale detaily jako netvoři nebo předměty se ignorují.")

// map editor in 3D nonisotropic

S("place points on surfaces of const Z", "umísťuj body na povrchy s konstantním Z")
S("place points on surfaces of const X", "umísťuj body na povrchy s konstantním X")
S("place points on surfaces of const Y", "umísťuj body na povrchy s konstantním Y")

S("parallels to draw", "rovnoběžky")
S("meridians to draw", "poledníky")
S("range of grid to draw", "rozsah mřížky")

S("current filter", "aktuální filtr")
S("Solv quotient spaces", "kvocientové prostory v Solv")
S("%1 period", "perioda %1")
S("Note: the value 0 functions effectively as the size of int (2^32).", "Poznámka: hodnota 0 funguje jako velikost int (2^32).")

S("standard ternary tiling", "standardní ternární dláždění")
S("stretched hyperbolic", "natažené hyperbolické")
S("stretched Solv", "natažený Solv")

S("{3,∞} (infinite triangles)", "{3,∞} (nekonečné trojúhelníky)")
S("{4,∞} (infinite triangles)", "{4,∞} (nekonečné čtverce)")
S("{3/4,∞} (infinite triangles and squares)", "nekonečné trojúhelníky a čtverce")

S("4D crystal in H3", "4D krystal v H3")
S("6D crystal in H3", "6D krystal v H3")

S("Arnold's cat mapping torus", "torus v Arnoldově kočičím mapování")
S("{5,3,5} quotient", "kvocient v {5,3,5}")
S("Seifert-Weber space", "Seifert-Weberův prostor")
S("Poincaré homology sphere", "Sféra Poincarého homologie")

S("load from file", "nahraj ze souboru")

S("Crystal torus", "Krystalický torus")
S("Z_k^d instead of Z^d. Only works with k even.", "Z_k^d místo Z^d. Funguje pouze pro sudá k.")

S("Euclidean quotient spaces", "Eukleidovské kvocientové prostory")
S("columns specify periods", "sloupce specifikují periody")
S("(vectors you need to take to get back to start)", "(vektory, kterými je třeba se dát, aby ses vrátil na začátek)")
S("third-turn space", "prostor třetinové otáčky")
S("make it even", "musejí to být sudá čísla")
S("or for columns : (A,B,C), (B,C,A), (D,D,D) where A+B+C=0", "nebo pro sloupce : (A,B,C), (B,C,A), (D,D,D), kde A+B+C=0")
S("quarter-turn space", "prostor čtvrtotáčky")

S("change the second column for Möbius bands and Klein bottles", "pro Möbiovy pásky a Kleinovy lahve změň druhý sloupec")
S("second period is chiral -- cannot be mirrored", "druhá perioda je chirální -- nelze ji zrcadlit")
S("periods must be orthogonal for mirroring", "pro zrcadlení musejí být periody kolmé")
S("mirror flip in the second period", "zrcadlové překlopení ve druhé periodě")
S("single-cell torus", "jednopolíčkový torus")
S("Klein bottle", "Kleinova lahev")
S("large regular torus", "velký pravidelný torus")
S("cylinder", "válec")
S("Möbius band", "Möbiův pásek")
S("seven-colorable torus", "torus obarvitelný sedmi barvami")
S("HyperRogue classical torus", "klasický HyperRogue torus")
S("no quotient", "bez kvocientu")
S("standard rotation", "standardní rotace")

S("field quotient", "tělesový kvocient")
S("order %1%2 (%3 %4s)", "řád %1%2 (%3 %4ů)")
S("order %1%2 (%3 %4-gons)", "řád %1%2 (%3 %4-úhelníků)")

S("non-isotropic", "neizotropní geometrie")
S("regular 2D tesselations", "pravidelná 2D dláždění")
S("regular 3D honeycombs", "pravidelná 3D dláždění")
S("interesting quotient spaces", "zajímavé kvocientové prostory")
S("geometries", "geometrie")
S("geometry", "geometrie")

S("start discovery", "začni s objevováním")
S("resume discovery", "pokračuj v objevování")
S("suspend discovery", "přeruš objevování")

S("#%1, cells: %2", "#%1, buněk: %2")

S("hyperbolic (3:2)", "hyperbolická (3:2)")
S("Sol (3:2)", "Solv (3:2)")

S("quotient spaces in ", "kvocientové prostory v: ")
S("No quotient spaces available in the current tiling.",
  "V současném dláždění nejsou k dispozici žádné kvocientové prostory.")

S("You are currently in a product space.", "Momentálně se nacházíš v produktovém prostoru.")

S("Nil quotient spaces", "Kvocientové prostory v Nil")
S("honeycomb", "mřížka")
S("Y period must be divisible by Z period", "perioda Y musí být dělitelná periodou Z")
S("quotient product spaces", "kvocientové produktové prostory")
S("Set to 0 to make it non-periodic.", "Pro neperiodičnost nastav na 0.")
S("Works if the underlying space is symmetric.", "Funguje, je-li základní prostor symetrický.")
S("reflect", "zrcadlení")
S("the current rotation is invalid", "současná rotace je neplatná")

S("Note: adjacency rule affects environmental effects, but not movement.",
  "Poznámka: pravidlo sousedění má vliv na efekty prostředí, ale nikoli na pohyb.")

S("\n\nOption -mineadj 1 can be added for Moore neighborhoods.", "\n\npro Mooreova okolí můžeš přidat možnost -mineadj 1.")

// WFC generation

S("configure WFC", "konfigurace WFC")

S("Wave Function Collapse", "Wave Function Collapse")
S("import the current map", "importuj současnou mapu")
S("generate a map using WFC", "vygeneruj mapu s použitím WFC")

// racing in Thurston geometries

S("Racing", "Závodění")
S("how fast can you reach the end?", "jak rychle se dokážeš dostat na konec?")
S("Racing in Thurston geometries", "Závodění v Thurstonových geometriích")
S("race through a maze in exotic 3D geometry!", "závoď v bludišti v exotické 3D geometrii!")

S("periodic Solv", "periodický Solv")
S("hyperbolic crystal", "hyperbolický krystal")
S("hyperbolic regular", "hyperbolická pravidelná mřížka")
S("S2xE regular", "pravidelná v S2xE")
S("H2xE regular", "pravidelná v H2xE")
S("periodic Nil", "periodický Nil")
S("alternate versions", "alternativní verze")

// extra option

S("floors in 2D geometries", "podlahy v 2D geometriích")

S("slides", "snímky")

// raycasting

S("configure raycasting", "konfiguruj raycasting")
S("raycasting configuration", "konfigurace raycastingu")
S("available in current geometry", "dostupné v současné geometrii")
S("use raycasting?", "používat raycasting?")
S("comparison mode", "srovnávací mód")
S("exponential range", "exponenciální dosah")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))", "vzorec jasnosti: max(1-d/sightrange, s*exp(-d/r))")
S("exponential start", "exponenciální start")
S("hard limit", "tvrdý limit")
S("no limit", "bez limitu")
S("reflective walls", "odrazné stěny")
S("max step", "maximální krok")
S("affects the precision of solving the geodesic equation in Solv", "ovlivňuje přesnost řešení geodetické rovnice v Solv")
S("min step", "minimální krok")
S("how precisely should we find out when do cross the cell boundary", "jak přesně máme zjišťovat, kdy překračujeme hranici políčka")
S("iterations", "iterace")
S("in H3/H2xE/E3 this is the number of cell boundaries; in nonisotropic, the number of simulation steps",
  "v H3/H2xE/E3 je toto počet hranic buněk; v neizotropních geometriích počet kroků simulace")
S("max cells", "maximální počet buněk")
S("generate", "generuj")
S("out-of-range color", "barva mimo dosah")

// new graphical styles

S("standard graphics", "standardní grafika")
S("neon mode", "neonový mód")
S("no boundary mode", "mód bez hranice")
S("neon mode II", "neonový mód II")
S("illustration mode", "mód ilustrace")
S("hint: Press Alt while testing modes", "nápověda: používej během testování módů klávesu Alt")
S("disable shadows", "vypni stíny")
S("bright mode", "jasný mód")
S("colorblind simulation", "simulace daltonismu")
S("ASCII/3D", "ASCII/3D")

// new configuration options

S("particles on attack", "částice při útoku")
S("floating bubbles: special", "bubliny: speciální")
S("floating bubbles: treasure  thresholds", "bubliny: hranice pokladů")
S("floating bubbles: all treasures", "bubliny: všechny poklady")

S("variable width", "proměnná šířka")
S("lines longer than this value will be split into shorter lines, with width computed separately for each of them.",
  "čáry delší než tato hodnota budou rozděleny na kratší, jejichž šířka bude vypočtena zvlášť.")

S("smoothen", "vyhlazení")
S("large values help if the joystick is imprecise", "vyšší hodnoty pomohou v případě nepřesného joysticku")

S("level lines", "horizontály")
S("This feature superimposes level lines on the rendered screen. These lines depend on the Z coordinate. In 3D hyperbolic the Z coordinate is taken from the Klein model. "
  "Level lines can be used to observe the curvature: circles correspond to positive curvature, while hyperbolas correspond to negative. See e.g. the Hypersian Rug mode.",
  
  "Tato funkce přeloží přes vyrenderovanou obrazovku horizontály. Tyto čáry závisejí na souřadnici Z. Ve 3D hyperbolické geometrii je souřadnice Z převzata z Kleinova modelu. "
  "Horizontály lze použít k pozorování zakřivení: kruhy odpovídají kladnému zakřivení, hyperboly zápornému. Viz např. model hyperského koberce.")

S("disable textures", "vypni textury")
S("disable level lines", "vypni horizontály")

S("Size of the fish eye.", "Rozměr rybího oka.")
S("product Z stretch", "Z-natažení produktu")
S("horocyclic coordinates", "horocyklické souřadnice")

// dual geometry puzzles

S("You won!", "Vyhrál jsi!")
S("dual geometry puzzles", "hádanky duální geometrie")
S("move both characters to marked squares at once!", "přesuň obě postavy zároveň na označené čtverce!")
S("enter seed", "zadej seed")
S("Euclidean size", "eukleidovský rozměr")
S("hyperbolic size", "hyperbolický rozměr")

// another new mode

S("play this Strange Challenge unofficially", "zahraj si tuto Podivnou výzvu neoficiálně")

// new gameplay lines

S("\n\nIn the Orb Strategy Mode, the effect is increased to +100%.",
  "\n\nVe Sférostrategickém módu se tento efekt zvyšuje na +100%.")

S("\n\nAdditionally, your allies are protected from your indirect attacks.", 
  "\n\nMimo to jsou tví spojenci chráněni před tvými nepřímými útoky.")

S("\n\nLeaves cut directly: %1", "\n\nPřímo useknuté listy: %1")
S("\n\nLeaves cut onscreen: %1", "\n\nListy useknuté na obrazovce: %1")
S("\n\nLeaves cut offscreen (approximately): %1", "\n\nListy useknuté mimo obraz (přibližně): %1")
S("leaves cut: %1", "useknuté listy: %1")

// explanations for the start screen

S("(random option)", "(náhodná možnost)")
S("(seasonal option)", "(sezónní možnost)")

S("spinning in the band model", "rotace v pásovém modelu")
S("spinning around", "rotace kolem dokola")
S("row of ghosts", "řada duchů")
S("army of ghosts", "armáda duchů")
S("ghost spiral", "spirála duchů")
S("Fibonacci ghosts", "Fibonnaciho duchové")

// new welcome messages

S("Welcome to Spherindrogue!", "Vítej ve Spherindrogue!")
S("Welcome to Hyper-X-R-Rogue!", "Vítej v Hyper-X-R-Rogue!")
S("Good luck in the elliptic space!", "Hodně štěstí v eliptickém prostoru!")
S("Welcome to SolvRogue!", "Vítej v SolvRogue!")
S("Welcome to NilRogue!", "Vítej v NilRogue!")
S("Welcome to PSL(2,R)-ogue!", "Vítej v PSL(2,R)-ogue!")

// new patterns and line patterns

S("empty", "prázdná")
S("rainbow by distance", "duha podle vzdálenosti")
S("Cage", "Klec")
S("Hyperplanes", "Hyperroviny")
S("Honeycomb", "Mřížka")
S("Diagonal", "Diagonála")
S("Penrose staircase", "Penroseovo schodiště")
S("nearer end", "bližší konec")
S("furthest from start", "nejdále od startu")
S("types", "typy")
S("display the inner walls", "zobraz vnitřní stěny")
S("floor type", "typ podlahy")
S("(no wall)", "(bez stěny)")
S("Goldberg tree", "Goldbergův strom")
S("irregular master", "hlavní nepravidelné")
S("sub-lines", "sub-linie")
S("fat edges", "tlusté hrany")
S("You chop down the shrub.", "Sekáš keřík.")

// new lands

N("Wetland", GEN_O, "Mokřad", "Mokřadu", "Mokřady", "v Mokřadu")

S("Some people have definitely drowned in this treacherous area. Better be careful!",
  "V této zrádné oblasti se už určitě někdo utopil. Dej si pozor!")

N("Water Lily", GEN_O, "Leknín", "Lekníny", "Leknín", "Leknín")
S("A beautiful plant from the Wetland.", "Krásná rostlina z Mokřadu.")

N("shallow water", GEN_F, "mělká voda", "mělké vody", "mělkou vodu", "mělkou vodou")
S("Shallow water is passable both for land creatures and for water creatures.",
  "Mělkou vodou mohou procházet jak suchozemští, tak vodní tvorové.")

N("shallow water", GEN_F, "hluboká voda", "hluboké vody", "hlubokou vodu", "hlubokou vodou")

S("This body of water is calm and beautiful, but too deep to pass.",
  "Tato voda je klidná a krásná, ale příliš hluboká, než abys skrz ni mohl projít.")

S("You remember anglers from your hometown showing the impressive pikes they have caught. This one is much larger.",
  "Vzpomínáš si, jak rybáři ve tvém rodném městě ukazovali impresivní štiky, které chytili. Tahle je mnohem větší.")

S("World Turtle", "Světoželva")
S("Have you reached the edge of the Poincaré disk? No, it must just be the edge of this world. "
  "Which happens to be floating on the backs of some turtles. Wondering how such not very big turtles could "
  "support such a large world? Well, there are LOTS of them! "
  "This one got so annoyed by the fact that you discovered this that it left its post. However, the world still "
  "stands, because there are so many turtles.",
  
  "Že bys dorazil na okraj Poincarého disku? Ne, to musí být okraj tohoto světa. "
  "Který, jak se ukazuje, pluje na zádech želv. Zajímá tě, jak takovéhle ne zrovna velké želvy dokážou "
  "udržet tak velký svět? Inu, je jich SPOUSTA! "
  "Tahle je tak otrávená skutečností, že jsi ji objevil, že opustila své místo. Ale svět stále "
  "stojí, protože želv je tolik.")

// gender-switched
N("Rusałka", GEN_F, "Rusalka", "Rusalky", "Rusalku", "Rusalkou")
S("A malicious water being. When you kill her, she changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "Zlovolná vodní bytost. Když ji zabiješ, změní políčko, na kterém stojíš, ze země na mělkou vodu nebo z mělké vody na hlubokou.")
  
N("Topielec", GEN_M, "Vodník", "Vodníci", "Vodníka", "Vodníkem")
S("A malicious water being. When you kill him, he changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "Zlovolná vodní bytost. Když ho zabiješ, změní políčko, na kterém stojíš, ze země na mělkou vodu nebo z mělké vody na hlubokou.")

// przet

Orb("Plague", "Moru")
S(
  "When you kill an enemy, adjacent enemies are also attacked, as long as they are further away from you than from the originally attacked enemy. "
  "These enemies may in turn infect further enemies, up to distance of at most 4 from you.",
  "Kdykoli zabiješ nepřítele, útok zasáhne i nepřátele, kteří s ním sousedí, pokud jsou od tebe dál než nepřítel, na kterého jsi původně zaútočil. "
  "Tito nepřátelé mohou nakazit další nepřátele, až do vzdálenosti maximálně 4 políčka od tebe.")

//---

N("Frog Park", GEN_O, "Žabí Park", "Žabí Parky", "Žabí Park", "v Žabím parku")

S("This park is a perfect place for frogs to hide from predators. "
  "So perfect that the frogs have grown in size, gained magical abilities, "
  "and become very dangerous themselves.",
  "Tento park je dokonalé místo, kde se mohou žáby ukrývat před predátory. "
  "Tak dokonalé, že žáby vyrostly, získaly magické schopnosti "
  "a samy se staly velice nebezpečnými."
  )

N("Gold Ball", GEN_O, "Zlatý míček", "Zlaté míčky", "Zlatý míček", "Zlatým míčkem")
S("A toy lost in the Frog Park by a spoiled child.", "Hračka, kterou nějaké rozmazlené dítě ztratilo v Žabím parku.")

Orb("Impact", "Úderu")
S("Whenever you use a ranged Orb to achieve an effect, all the single-tile monsters adjacent to the target "
  "location are stunned.",
  "Kdykoli použiješ efekt nějaké dálkové Sféry, všichni jednopolíčkoví netvoři sousedící s cílovou "
  "lokací budou omráčeni.")

N("shrub", GEN_O, "krzak", "krzaki", "krzaka", "krzakiem")
S("A strange small tree that cannot be attacked with mundane weapons nor phased though. "
  "It can be vaulted over, though.",
  "Podivná malá dřevina, na kterou nelze zaútočit běžnými zbraněmi, ani skrz ni profázovat. "
  "Dá se však přeskočit.")

N("Giant Frog", GEN_F, "Velká žába", "Velké žáby", "Velkou žábu", "Velkou žábou")

S(
  "At first, you are shocked by the agility of this frog. Such a large creature "
  "should not be able to jump that quickly!\n\n"
  "Then, you notice the green glow around its legs. This frog must be magical... "
  "it has sacrificed its swimming abilities for superfrog jumping powers.",
  
  "Hbitost této žáby tě zpočátku ohromí. Tak velký tvor "
  "by neměl být schopný tak rychle skákat!\n\n"
  "Potom si všimneš zelené záře kolem jejích nohou Tahle žába musí být kouzelná... "
  "obětovala svoji schopnost plavat, aby získala superžabí skokanské schopnosti.")

N("Yellow Frog", GEN_F, "Žlutá žába", "Žluté žáby", "Žlutou žábu", "Žlutou žábou")

S("A slightly transparent yellow frog. It has mastered the magical power of phasing through solid "
  "obstacles such as rock.",
  "Mírně průsvitná žlutá žába. Ovládla magickou schopnost fázování skrz pevné "
  "překážky, jako jsou skály.")

N("Blue Frog", GEN_F, "Modrá žába", "Modré žáby", "Modrou žábu", "Modrou žábou")

S(
  "This frog is able to vault over the shrubs in the Frog Park, destroying the shrub in the process. "
  "While it can also vault over monsters, it will never hurt the other frogs!",

  "Tato žába dokáže přeskočit keříky v Žabím parku, a tím je zničit. "
  "Může přeskakovat i netvory, ale nikdy neublíží jiným žábám!")

S("\n\nThis Frog uses the power of %the1. You get 5 charges yourself for killing it.", "\n\nTato žába využívá sílu '%1'. Když ji zabiješ, získáš 5 nábojů této Sféry.")

//---

N("Eclectic City", GEN_N, "Eklektické město", "Eklektická města", "Eklektické město", "v Eklektickém městě")

S("Many kinds of creatures are living here, each with its own architectural style.",
  "Žije zde mnoho druhů tvorů, každý se svým vlastním architektonickým stylem.")

N("Lazurite Figurine", GEN_F, "Lazuritová figurka", "Lazuritové figurky", "Lazuritovou figurku", "Lazuritovou figurkou")

S("A beautiful blue figurine. Every figurine is different.",
  "Krásná modrá figurka. Každá je jiná.")
  
Orb("Chaos", "Chaosu")
S(
  "When you move, the contents of the two cells adjacent to your both locations are swapped. "
  "If they contain creatures, these creatures will be stunned for some time, before they understand what happened.\n\n"
  "Does not work if one of the cells contains a "
  "multi-tile creature or a barrier-like wall.",
  
  "Kdykoli se pohneš, obsah dvou políček, které sousedí jak s počátečním, tak s cílovým políčkem, se vymění. "
  "Pokud tam jsou nějací tvorové, budou na chvíli omráčeni, než si uvědomí, co se stalo.\n\n"
  "Nefunguje, pokud je na jednom z políček vícepolíčkový tvor nebo bariéra jako zeď.")

S("digon", "dvojúhelník")
S("triangle", "trojúhelník")
S("square", "čtverec")
S("pentagon", "pětiúhelník")
S("hexagon", "šestiúhelník")
S("heptagon", "sedmiúhelník")
S("octagon", "osmiúhelník")

// 11.5
//======

// missing Android buttons

S("WAIT", "ČEKEJ")
S("RUG", "KOBEREC")
S("CAM", "KAM")

// missing UI texts

S("error: ", "chyba: ")
S("rules: %1", "pravidla: %1") // WFC

// missing game texts

S("You polymorph %the1 into %the2!", "Měníš %a1 v %a2!")
S("All players are in the game!", "Všichni hráči jsou ve hře!")
S("\"Are there Temples of Cthulhu in your world? Why not?\"", 
  "\"Jsou ve tvém světě Cthulhuovy chrámy? Proč ne?\"")
S("saves", "uložené hry")
S("players", "hráči")
S("You swing your sword at %the1.", "Útočíš mečem na %a1.")
S("You swing your sword.", "Mácháš mečem.")
S("There is no room for %the1!", "Na %a1 tu není místo!")
S("Don't play with slime and electricity next time, okay?", "Příště si nehraj se slizem a elektřinou, ano?")
S("The Orb of Safety from the Land of Eternal Motion might save you.", 
  "Sféra Bezpečnosti z Kraje věčného pohybu by tě mohla zachránit.")
S("%the1 protects your boat!", "%1 chrání tvůj člun!")
S("Are you sure you want to step there?", "Víš jistě, že tam chceš vstoupit?")
S("Hint: press 1 to change the projection.", "Nápověda: pro změnu projekce stiskni 1.")
S("%The1 turned out to be an illusion!", "%1 se ukáza%l1 být iluze!")
S("You feel that these directions are %1 turns old.", "Cítíš, že tyto instrukce jsou %1 kol staré.")
S("%The1 raises his weapon...", "%1 zvedá zbraň...")
S("%The1 takes %his1 revenge on %the2!", "%1 se mstí na %a2!")

// parameters

S("You can enter formulas in this dialog.", "V tomto dialogu můžeš zadávat vzorce.")
S("Functions available:", "Dostupné funkce:")
S("Constants and variables available:", "Dostupné konstanty a proměnné:")
S("Animations:", "Animace:")
S("a..b -- animate linearly from a to b", "a..b -- animovat lineárně od a do b")
S("a..b..|c..d -- animate from a to b, then from c to d", "a..b..|c..d -- animovat od a do b, potom od c do d")
S("a../x..b../y -- change smoothly, x and y are derivatives", "a../x..b../y -- hladká změna; x a y jsou derivace")
S("Parameter names, e.g. '%1'", "Názvy parametrů, např. '%1'")
S("Parameter names", "Názvy parametrů")
S("These can be combined, e.g. %1", "Je možné je kombinovat, např. %1")

// manifolds

S("alternate manifolds", "alternativní variety")
S("generators (%1,%2), size %3", "generátory (%1,%2), velikost %3")
S("This option finds alternate solutions. For example, there are three {7,3} manifolds with 156 heptagons each (\"first Hurwitz triplet\").", 
  "Tato možnost hledá alternativní řešení. Tak například, existují tři variety {7,3} se 156 sedmiúhelníky (\"první Hurwitzův triplet\").")

// unrecognized nonliteral: f->name in geom-exp.cpp:274
S("Only works with 2D geometries", "Funguje pouze ve 2D geometriích")
S("stereographic Panini", "stereografická Paniniho projekce")
S("orthographic projection", "ortografická projekce")
S("#%1, cells: %2, p=%3", "#%1, políček: %2, p=%3")
S("affine", "afinní")
// unrecognized nonliteral: qstring in geom-exp.cpp:625
S("geometry/topology/tiling", "geometrie/topologie/dláždění")
S("binary tiling width", "šířka binárního dláždění")
S("Nil width", "šířka Nilu")
S("stretched geometry", "natažená geometrie")
S(
  "Stretch the metric along the fibers. This can currently be done in rotation spaces and in 8-cell, 24-cell and 120-cell. "
  "Value of 0 means not stretched, -1 means S2xE or H2xE (works only in the limit). (Must be > -1)",
  
  "Natáhne metriku podél vláken. V současné době je to možné provést v rotačních prostorech a v 8-prostoru, 24-prostoru a 120-prostoru. "
  "Hodnota 0 znamená žádné natažení, -1 znamená S2xE nebo H2xE (funguje pouze v limitě). (Musí být > -1.)")
  
S("basic tiling", "základní dláždění")
S("fake curvature", "falešné zakřivení")
S("tessellation sliders", "šoupátka dláždění")
S("truncate ultra-vertices with mirrors", "seříznout ultra-vrcholy pomocí zrcadel")
S("adjacency rule", "pravidlo sousednosti")
S("vertex", "vrchol")
S("face", "stěna")
S("edge", "hrana")
S("info about: %1", "info o: %1")
S("genus", "genus")

S("VR demos", "demo VR")
S("warning: these will restart your game!", "varování: toto zrestartuje hru!")
S("standard HyperRogue but in VR", "standardní HyperRogue, ale ve VR")
S("HyperRogue FPP", "HyperRogue FPP")
S("Hypersian Rug", "hyperský koberec")
S("sphere from the inside", "koule zevnitř")
S("sphere from the outside", "koule zvenčí")
S("Thurston racing", "thurstonovské závody")
S("raytracing in H3", "raytracing v H3")

S("VR settings", "nastavení VR")
S("VR enabled", "VR povolena")
S("VR initialized correctly", "VR správně inicializována")
S("(this setting is for 3D geometries only, use 'model viewing' instead)", 
  "(toto nastavení je pouze pro 3D geometrie, místo toho použij 'pohled na model')")
  
S("absolute unit in meters", "absolutní jednotka v metrech")

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
  
  "Velikost absolutní jednotky neeukleidovské geometrie v metrech. "
  "To má vliv na pohyb headsetu a binokulární vidění.\n\n"
  "Ve sférické geometrii je absolutní jednotkou poloměr koule. "
  "Čím menší je absolutní jednotka, tím silnější jsou neeukleidovské efekty. "
  "Elementy ve světě HyperRogue mají pevně danou velikost v absolutních jednotkách, "
  "takže zmenšení absolutní jednotky zmenší i je. "
  "Pokud hraješ v eukleidovském módu, toto nastavení pouze změní měřítko všeho "
  "(např. v krychlovém dláždění je 'absolutní jednotka' prostě jen hrana krychle). "
  "Ovlivněny jsou pouze perspektivní projekce, ostatní modely používají nastavení 'měřítko VR' "
  "v menu Projekce.")
        
S("reset the reference point", "vyresetuj vztažný bod")
S("pointer length", "délka ukazatele")
S(
  "If the pointer length is 0.5m, the object pointed to is 0.5 meter from the controller. "
  "This is used in situations where the controller is used as a 3D mouse, e.g., "
  "the drawing tool in three-dimensional geometries. When pointing at two-dimensional surfaces, "
  "this is not relevant (the pointer is as long as needed to hit the surface).",
  
  "Pokud je délka ukazatele 0,5 m, je objekt, na který ukazuje, 0,5 metru od ovladače. "
  "To se používá v situacích, kdy se ovladač používá jako 3D myš, např. "
  "jako kreslicí nástroj ve trojrozměrných geometriích. Při ukazování na dvojrozměrné povrchy "
  "to není relevantní (ukazatel je tak dlouhý, jak je třeba, aby se mohl dotknout povrchu.")
  
S("UI size", "velikost UI")
S("How big is a pixel of the user interface (HUD and menus). The user interface is as big as the window on the desktop.",
  "Jak je velký jeden pixel uživatelského rozhraní (HUD a menu). Uživatelské rozhraní je stejně velké jako okno na ploše.")

S("UI depth", "hloubka UI")
S("How far to show the user interface (HUD and menus).", "Jak daleko se má ukazovat uživatelské rozhraní (HUD a menu).")

//

S("Not available in the daily challenge!", "V denní misi nedostupné!")
S("Welcome to CoverRogue!", "Vítej v CoverRogue!")
S("Welcome to SL(2,R)-ogue!", "Vítej v SL(2,R)-ogue!")
S("Emergency save at ", "Nouzové uložení v ")

// tour

S("You cannot change geometry in this slide.", "V tomto snímku nemůžeš měnit geometrii.")
S("This does not work in bounded geometries.", "Toto v omezených geometriích nefunguje.")
S("This does not work in Euclidean geometry.", "Toto v eukleidovské geometrii nefunguje.")
S("This works only in hyperbolic geometry.", "Toto funguje pouze v hyperbolické geometrii.")
S("This land has no Euclidean version.", "Tento kraj nemá eukleidovskou verzi.")
S("This land has no spherical version.", "Tento kraj nemá sférickou verzi.")
S("Changed the projection.", "Projekce změněna.")
S("HyperRogue Guided Tour", "Prohlídka HyperRogue")
S("slideshows", "prezentace")
S("go up", "nahoru")

S(" (edges too short: %1)", " (příliš krátké hrany: %1)")
S("wait...", "čekej...")
S("bitruncation const", "bitrunkační konstanta")
S("too small period -- irregular tiling generation fails", "příliš malá perioda -- vytváření nepravidelného dláždění selhalo")
S("To create Euclidean irregular tesselations, first enable a torus", 
  "Pro vytvoření eukleidovských nepravidelných dláždění nejprve zapni torus")

S("Error: period mismatch", "chyba: neshoda v periodách")
S("Hantzsche-Wendt space", "Hantzsche-Wendtův prostor")
S("HyperRogue classic torus", "klasický HyperRogue torus")

// archimedean & arbitrile

S("invalid dihedron", "neplatný dvojstěn")
S("face mismatch", "neshoda ve stěnách")
S("face mismatch II ", "neshoda ve stěnách II ")
S("face mismatch II", "neshoda ve stěnách II")

S("Polygon number %1 did not close correctly (%2 %3). Here is the picture to help you understand the issue.\n\n", 
  "Mnohoúhelník číslo %1 se správně neuzavřel (%2 %3). Zde je obrázek pro lepší pochopení problému.\n\n")
S("connection debugger", "debugger spojení")
S("open a tiling", "otevřít dláždění")

// goldberg

S("inverse rectify", "inverzní rektifikace")
S("inverse truncate", "inverzní seříznutí")
S("warped version", "pokřivená verze")
S("swap x and y", "vyměnit x a y")
S("dual of current", "dláždění duální k současnému")
S("dual", "duální dláždění")
S("unrectified", "nerektifikované")
S("warped", "pokřivené")
S("untruncated", "neseříznuté")

// rug/embeddings
S("save the current embedding", "uložit současné vnoření")
S("load a saved embedding", "nahrát uložené vnoření")
S("save embedding to:", "uložit vnoření do:")
S("load embedding from:", "nahrát vnoření z:")
S("Failed to save embedding to %1", "Uložení vnoření do %1 se nezdařilo")
S("Failed to load embedding from %1", "Nahrání vnoření z %1 se nezdařilo")

S("Failed to enable", "Aktivace se nezdařila")
S("protractor", "úhloměr")
S("move on touch", "pohyb při doteku")
S("finger range", "rozsah prstu")
S("Press 1 to enable the finger mode.", "Stiskni 1 pro aktivaci prstového módu.")
S("finger force", "síla prstu")
S("Press 1 to enable the finger force.", "Stiskni 1 pro aktivaci síly prstu.")

// patterns

S("probability of a wall (%)", "pravděpodobnost zdi (%)")
S("single color", "jedna barva")
S("distance from origin", "vzdálenost od počátku")
S("nice coloring", "pěkné zbarvení")
S("Crystal coordinates", "souřadnice Krystalu")
S("sides", "strany")
S("display only chessboard white", "zobrazit pouze bílá políčka šachovnice")
S("display only chessboard black", "zobrazit pouze černá políčka šachovnice")

S(
          "This lets you specify the color pattern as a function of the cell. "
          "Available parameters:\n\n"
          "x, y, z (hyperboloid/sphere/plane coordinates in non-crystal geometries)\n"
          "ex, ey, ez (in Euclidean geometries)\n"
          "x0, x1, x2... (crystal geometry only)\n"
          "0 is black, 1 is white, rgb(1,0,0) is red, ifp(p-2,1,0) is blue (p=1 for red, 2 for green, 3 for blue).",

          "Zde můžeš specifikovat barevný vzor jako funkci políčka. "
          "Dostupné parametry:\n\n"
          "x, y, z (souřadnice hyperboloidu/koule/roviny v nekrystalových geometriích)\n"
          "ex, ey, ez (v eukleidovských geometriích)\n"
          "x0, x1, x2... (pouze v krystalové geometrii)\n"
          "0 je černá, 1 bílá, rbg(1,0,0) červená, ifp(p-2,1,0) modrá (p=1 pro červenou, 2 pro zelenou, 3 pro modrou)."
          )

S(
          "w (fourth coordinate)\n"
          "wallif(condition, color)\n",

          "w (čtvrtá souřadnice)\n"
          "wallif(podmínka, barva)\n"
          )

S("see compute_map_function in pattern2.cpp for more\n", "více informací ve funkci compute_map_function v pattern2.cpp")

S("broken Emerald Pattern", "rozbitý Smaragdový vzor")
S("single cells", "jednotlivá políčka")
S("edit both bitrunc colors", "editovat obě barvy bitrunkace")
S("extra symmetries", "extra symetrie")
S("odd/even", "sudá/lichá")
S("cheat", "cheat")

S("cannot adjust depth", "nastavení hloubky nelze změnit")
S("cannot adjust camera", "nastavení kamery nelze změnit")
S("does not work with perfect Klein", "toto v dokonalém Kleinově modelu nefunguje")

S("randomize", "randomizovat")
S("seed", "seed")
S("select a puzzle", "vyber hádanku")

// texture mode

S("Failed to load %1", "Nahrání %1 se nezdařilo")
S("scale/rotate the texture", "změna měřítka/rotace textury")
S("scale/rotate the model", "změna měřítka/rotace modelu")
S("Unexpected missing cell #%1/%1", "Neočekávané chybějící políčko #%1/%1")
S("racing in Thurston geometries", "závody v thurstonovských geometriích")
S("S2xE", "S2xE")
S("H2xE", "H2xE")
S("PSL(2,R)", "PSL(2,R)")
S("torus x E", "torus x E")

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
    
    "Tento prostor v podstatě umožňuje hrát v d-rozměrné mřížce. Vyberte tři "
    "rozměry a '3D zobrazení', abyste viděli, jak to funguje -- v podstatě hrajeme na periodickém povrchu ve "
    "třech rozměrech tvořeném šestiúhelníky; každý šestiúhelník je spojený se šesti dalšími šestiúhelníky, v každém ze 6 "
    "možných směrů. Hra toto normálně vizualizuje z pohledu tvora žijícího uvnitř "
    "povrchu (poněkud regularizovaného a vyhlazeného), za předpokladu, že i světelné paprsky jsou omezeny na tento povrch -- "
    "vypadá to přesně jako dláždění {2d,4}, až na to, že světelné paprsky mohou "
    "někdy vytvořit smyčku a vy v určitých směrech uvidíte svůj vlastní obraz (jinými slovy, "
    "d-rozměrná mřížka je kvocientem hyperbolické roviny.) Stejná konstrukce funguje i v jiných rozměrech. "
    "Poloviční rozměry se interpretují následovně: přebytečná dimenze má pouze dvě 'úrovně', například 2.5D "
    "má horní rovinu a dolní rovinu.\n\n"
    "U tohoto dláždění můžete také provést bitrunkaci -- bude pak lépe fungovat v rámci pravidel HyperRogue, ale bude o něco méně srozumitelné."    
    )
S(
    "This is a representation of four-dimensional geometry. Can you find the Holy Grail in the center of the Round Table?\n\n"
    "In 'Knight of the 16-Cell Table', each cell has 8 adjacent cells, "
    "which correspond to 8 adjacent points in the four-dimensional grid. The Round Table has the shape of a 16-cell.\n\n"
    "In 'Knight of the 3-Spherical Table', it is the same map, but double bitruncated. The Round Table has the shape of a hypersphere.\n\n",
    
    
    "Toto je reprezentace čtyřrozměrné geometrie. Dokážete najít Svatý grál ve středu Kulatého stolu?\n\n"
    "¨V 'Rytířích 16-prostorového stolu' má každé políčko 8 sousedů.\n\n"
    "V 'Rytířích 3-sférického stolu' je mapa stejná, ale po dvou bitrunkacích. Kulatý stůl má tvar hypersféry.\n\n"
    )
S("let me understand how the coordinates work", "ukaž mi, jak fungují souřadnice")
S("thanks, I need no hints (achievement)", "díky, nepotřebuju nápovědu (achievement)")
S("more about this geometry...", "více o této geometrii...")
S("render a cut", "zobrazit řez")
S("cut level", "úroveň řezu")

S("Do not use compases.\n\n", "Nepoužívat kompasy.\n\n")
S("Crystal Camelot is an octahedron in 'pure' 3D crystal geometry (and a similar polytope in other pure crystals), "
      "and an Euclidean ball in bitruncated/Goldberg crystals.", 
  "Krystalový Kamelot je osmistěn v 'čisté' 3D krystalové geometrii (a podobný polytop v jiných čistých krystalech) "
  "a eukleidovská koule v bitrunkatovaných/Goldbergových krystalech. ")
S("puzzle/exploration mode", "mód hádanek/objevování")

// puzzle mode

S("exploration", "objevování")
S("puzzles", "hádanky")
S("other puzzles", "další hádanky")
S("hyperbolic Minesweeper", "hyperbolický Minesweeper")
S("dual geometry puzzle", "hádanky v duální geometrii")
S("Uncover all cells which do not contain mines", "Odhal všechna políčka, která neobsahují miny")
S("Cells to uncover: %1", "Políček k odkrytí: %1")
S("You won in %1", "Vyhrá%l jsi v %1")
S("Dropped floors: %1/%2", "Propadlé podlahy: %1/%2")

// shmup control
S("camera forward", "kamera vpřed")
S("camera rotate X", "rotace kamery X")
S("camera rotate Y", "rotace kamery Y")
S("Configure dead zones", "Konfigurovat mrtvé zóny") // for joysticks

// keyboard help
S("arrows", "šipky")
S("WASD keys", "klávesy WASD")
S("VI keys", "klávesy VI")

// sight range settings

S("sight range bonus", "bonus k dohledu")
S("3D sight range for the fog effect", "3D dohled pro efekt mlhy")

S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "Dohled v 3D geometriích je specifikován v absolutních jednotkách. Tato hodnota také ovlivňuje efekt mlhy.\n\n"
  "Ve sférických geometriích vám dohled 2π umožní vidět věci za vámi, jako by byly před vámi, "
  "a dohled π (nebo víc) vám umožní vidět věci v protilehlém bodě, jako kdyby byly blízko vás.\n\n"
  "V hyperbolických geometriích počet políček, která se renderují, exponenciálně závisí na dohledu. Větší počet vykreslovaných políček "
  "snižuje výkon.\n\n"
  "Dohled ovlivňuje hratelnost -- netvoři jednají právě tehdy, když jsou viditelní. Generování netvorů to bere v úvahu.")
  
S("max difference in Z coordinates", "maximální rozdíl v souřadnici Z")
S("area-based range", "dosah založený na ploše")
S("draw range based on", "dosah vykreslování založený na")
S("size (no gen)", "velikosti (bez generování)")
S("size", "velikosti")
S("sight range settings", "nastavení dohledu")

// other settings

S("play music when out of focus", "hrát hudbu, když je hra neaktivní")
S("floating bubbles: treasure thresholds", "létající bubliny: prahy pokladů")
S("vector settings", "vektorová nastavení")
S("Line width setting is only taken into account in OpenGL.", "Nastavení šířky čar se bere v úvahu pouze v OpenGL.")
S("shots only", "pouze střely")
S("hint: press Alt while testing modes", "nápověda: stiskni při testování módů Alt")
S("no fill in neon mode", "bez výplně v neonovém módu")
S("(vsync disabled in VR)", "(vsync je ve VR vypnutý)")
S("apply changes", "provést změny")
S("width", "šířka")
S("camera movement speed", "rychlost pohybu kamery")
S("camera rotation speed", "rychlost rotace kamery")
S("send scores to Steam leaderboards", "posílat skóre na Steam leaderboardy")
S("crosshair size", "velikost zaměřovacího kříže")
S("crosshair color", "barva zaměřovacího kříže")

S("Must be less than %1°. Panini projection can be used to get higher values.", 
  "Musí být méně než %1°. Pro vyšší hodnoty je možné použít Paniniho projekci.")

S("Panini projection", "Paniniho projekce")
S(
  "The Panini projection is an alternative perspective projection "
  "which allows very wide field-of-view values.\n\n",
  
  "Paniniho projekce je projekce s alternativní perspektivou, "
  "která umožňuje velmi vysoké hodnoty šířky zorného pole.\n\n")

S("spherical perspective projection", "sférická perspektivní projekce")
S(
  "Set to 1 to get stereographic projection, "
  "which allows very wide field-of-view values.\n\n",
  
  "Nastavením na 1 získáš stereografickou projekci, "
  "která umožňuje velmi vysoké hodnoty šířky zorného pole.\n\n"  
  )

S(
  "HyperRogue uses "
  "a quick implementation, so parameter values too close to 1 may "
  "be buggy (outside of raycasting); try e.g. 0.9 instead.", 
  
  
  "HyperRogue používá "
  "rychlou implementaci, takže hodnoty parametrů příliš blízké 1 mohou "
  "vést k chybám (mimo raycasting); zkus místo toho např. 0.9.")

S("Berger sphere limit", "limita Bergerovy sféry")

S("Primitive-based rendering of Berger sphere is currently very slow and low quality. "
  "Here you can choose how many images to draw.", 
  "Rendering Bergerovy sféry na bázi primitivů je v současné době velmi pomalý a nekvalitní. "
  "Zde si můžeš vybrat, kolik obrazů se má vykreslovat.")
  
S(
  "Camera is placed %1 absolute units above a plane P in a three-dimensional "
  "world. Ground level is actually an equidistant surface, %2 absolute units "
  "below the plane P. The plane P (as well as the ground level or any "
  "other equidistant surface below it) is viewed at an angle of %3 "
  "(the tangent of the angle between the point in "
  "the center of your vision and a faraway location is 1/cosh(c) = %4).", 
  
  "Kamera je umístěna %1 absolutních jednotek nad rovinou P ve trojrozměrném "
  "světě. Základní úroveň je ve skutečnosti ekvidistantní plocha %2 absolutních jednotek "
  "pod rovinou P. Rovina P (a také základní úroveň nebo jakákoli jiná ekvidistantní plocha pod ní) se vidí pod úhlem %3 "
  "(tečna úhlu mezi bodem "
  "uprostřed tvého zorného pole a nějakou vzdálenou lokací je 1/cosh(c) = %4).")
  
S("cool fog effect", "prima efekt mlhy")
S("borders", "hranice")
S("pattern colors", "barvy vzoru")
S("Galápagos shading", "galapážské stínování")
S("targetting ranged Orbs long-click only", "cílování dálkových sfér pouze dlouhým kliknutím")

S("0 to disable", "0 pro vypnutí")
S("recently changed settings", "nedávno změněná nastavení")
S("find a setting", "najdi nastavení")

// nonisotropic

S("geodesic table missing", "chybí geodetická tabulka")
S(
  "If the 2D underlying manifold is bounded, the period should be a divisor of the 'rotation space' "
  "value (PSL(2,R)) times the Euler characteristics of the underlying manifold. "
  "For unbounded underlying manifold, any value should work (theoretically, "
  "the current implementation in HyperRogue is not perfect).\n\n"
  "We won't stop you from trying illegal numbers, but they won't work correctly.", 
        
  "V případě, že je základní 2D varieta ohraničená, by měla být perioda dělitelem hodnoty 'rotačního prostoru' "
  "(PSL(2,R)) krát Eulerova charakteristika základní variety. "
  "Pokud je základní varieta neohraničená, měla by fungovat jakákoli hodnota (teoreticky, "
  "současná interpretace v HyperRogue není dokonalá).\n\n"
  "Nebudeme ti bránit, abys zkoušel nelegální čísla, ale nebudou fungovat správně."
  )
S("SL(2,R)", "SL(2,R)")
S("universal cover", "univerzální pokrytí")
S("maximum", "maximum")
S("works correctly so far", "zatím funguje správně")

S("non-periodic", "neperiodické")
S("patched Chaos", "záplatovaný Chaos")
S("excessive walls", "přemíra zdí")
S("total chaos", "totální chaos")
S("random-walk chaos", "chaos náhodné procházky")
S("single land: ", "jeden kraj: ")
S("single land", "jeden kraj")
S("wall-less", "bez zdí")

S("highlight stuff", "zvýraznit věci")
S("press Alt", "stiskni Alt")
S("highlight", "zvýraznění")
S("super-highlight", "super-zvýraznění")

S("shape editor", "editor tvarů")
S("drawing tool", "kreslicí nástroj")
S("land structure", "struktura krajů")
S("highlights & achievements", "význačné body & achievementy")
S("classic game except hyperbolic", "klasická hra, jen hyperbolická")
S("can you find the Heptagonal Grail?", "dokážeš najít Sedmiúhelníkový grál?")
S("General Euclid", "Generál Eukleidés")
S("Worm of the World", "Červ světa")
S("Lovász Conjecture", "Lovászova hypotéza")
S("Knight of the 16-Cell Table", "Rytíř 16-prostorového stolu")
S("Knight of the 3-Spherical Table", "Rytíř 3-sférického stolu")
S("Emerald Mine in {5,3,4}", "Smaragdový důl v {5,3,4}")

// help

S("credits", "titulky")

S("\n\nAdditionally, the probabilities of generating terrain features are subtly changed in the following lands:", 
  "\n\nKromě toho se v následujících krajích mírně mění pravděpodobnosti generování terénních prvků:")
S(
    "\n\nOnce you collect a Bomberbird Egg, "
    "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
    "Collecting even more Eggs will increase the radius.",
    
    "\n\nPoté, co sebereš Vejce Bombarďáka, "
    "vstup na políčko, které nesousedí s žádnými minami, odhalí i sousední políčka. "
    "Nasbíráním ještě více Vajec zvýšíš poloměr.")
    
S(" (shadow in %1)", " (stín v %1)")
S(", %1 the Knight", ", Rytíř %1")
S("Hypersian Rug menu", "menu Hyperského koberce")
S("HyperRogue help", "nápověda HyperRogue")

// history mode

S("s = save to .png", "s = uložit do .png")
S("rendered band ($ID=segment, $DATE=date)", "vyrenderovaný pás ($ID=segment, $DATE=data)")
S("animate from last recenter to precise current view", "animovat od posledního vycentrování k přesnému současnému pohledu.")
S("save the animation starting point", "uložit počáteční bod animace")
S("animate from the starting point", "animovat od počátečního bodu")
S("reset animation", "resetovat animaci")
S("include history when auto-rendering", "zahrnout při auto-renderování historii")
S("more options in hyperbolic geometry", "více možností v hyperbolické geometrii")
S("more options in band projections", "více možností v pásových projekcích")
S("filename format to use ($ID=segment, $DATE=date)", "formát názvu souboru ($ID=segment, $DATE=data)")
S("band segment", "segment pásu")

// screenshot/animations

S("PNG", "PNG")
S("SVG", "SVG")
S("WRL", "WRL")
S("centering", "centrování")
S("You can pick the angle. Note: the direction the PC is facing matters.", 
  "Můžeš si vybrat úhel. Poznámka: hraje roli, kterým směrem je postava otočena.")
S("rotate PC", "otočit postavu")
S(
  "The value of 1 means that the period of this animation equals the period set in the animation menu. "
  "Larger values correspond to faster animations.", 
  "Hodnota 1 znamená, že perioda této animace se rovná periodě nastavené v menu animace. ")
S("shift", "posun")
S("movement angle", "úhel pohybu")
S("This is the period of the whole animation, though in some settings the animation can have a different period or be aperiodic. "
      "Changing the value will make the whole animation slower or faster.",
  "Toto je perioda celé animace, i když v některých nastaveních může mít animace jinou periodu nebo být aperiodická. "
  "Změna této hodnoty zpomalí nebo zrychlí celou animaci.")
S("Least common multiple of the animation periods of all the game objects on screen, such as rotating items.", 
  "Nejmenší společný násobek animačních period všech herních objektů na obrazovce, například rotujících předmětů.")
S("Möbius transformations", "Möbiovy transformace")
S("circle spins", "otočky kruhu")
S("circle radius", "poloměr kruhu")
S("double spin", "dvojitá rotace")
S("triple spin", "trojitá rotace")
S("draw the circle", "nakreslit kruh")
S("cycle length", "délka cyklu")
S("full circle", "celý kruh")
S("Zebra period", "perioda Zebry")
S("Bolza period", "Bolzova perioda")
S("angle to screen normal", "úhel k normále obrazovky")
S("distance from rotation center", "vzdálenost od středu rotace")
S("monster turns", "kola netvorů")
S("Number of turns to pass. Useful when simulating butterflies or cellular automata.", 
  "Kolik kol má uběhnout. Užitečné při simulaci motýlů nebo buněčných automatů.")
S("screen-relative rotation", "rotace vzhledem k obrazovce")
S("angle", "úhel")
S("model-relative rotation", "rotace vzhledem k modelu")
S("automatic move speed", "rychlost automatického pohybu")
S("Move automatically without pressing any keys.", "Automatický pohyb bez mačkání kláves")
S("synchronize", "synchronizace")
S("model rotation", "rotace modelu")
S("3D rotation", "3D rotace")
S("some parameters will only change in recorded animation", "některé parametry se změní pouze v nahrané animaci")
S("record to sequence of image files", "nahrát jako sekvenci obrazových souborů")
S("record to video file", "nahrát jako video soubor")
S("explore this animation", "zkoumat tuto animaci")

// raycaster

S("volumetric raycasting", "volumetrický raycasting")
S("active", "aktivní")
S("intensity of random coloring", "intenzita náhodného obarvení")
S("color randomly", "obarvit náhodně")
S("SMART", "CHYTRÉ")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))\n", "vzorec jasu: max(1-d/sightrange, s*exp(-d/r))\n")
S("unfortunately this honeycomb is too complex for the current implementation (%1>%2)", 
  "toto 3D dláždění je bohužel pro současnou implementaci příliš složité (%1>%2)")

// extra projections

S("geodesic", "geodetické")
S("quadrant coordinates", "kvadrantové souřadnice")
S("axial coordinates", "osové souřadnice")
S("anti-axial coordinates", "anti-osové souřadnice")
S("Werner projection", "Wernerova projekce")
S("Aitoff projection", "Aitoffova projekce")
S("Hammer projection", "Hammerova projekce")
S("loximuthal projection", "loximutální projekce")
S("Miller projection", "Millerova projekce")
S("Gall stereographic", "stereografická Gallova projekce")
S("Winkel tripel", "Winkelova tripelová projekce")
S("Poor man's square", "chudákův čtverec")
S("Craig retroazimuthal", "Craigova retroazimutální projekce")
S("Littrow retroazimuthal", "Littrowova retroazimutální projekce")
S("Hammer retroazimuthal", "Hammerova retroazimutální projekce")
S("three-point equidistant", "tříbodová ekvidistantní projekce")
S("stereographic projection [VR]", "stereografická projekce [VR]")

// projection settings

S("simple model: projection", "prostý model: projekce")
S("simple model: perspective", "prostý model: perspektiva")
S("projection type", "typ projekce")
S("edit VR or non-VR settings", "upravit nastavení VR nebo ne-VR")
S("rotational or Heisenberg", "rotační nebo Heisenbergův model")
S("1 = Heisenberg, 0 = rotational", "1 = Heisenbergův, 0 = rotační")
S("use atan to make it finite", "použij atan, aby byl konečný")
S("VR: rotate the 3D model", "VR: rotace 3D modelu")
S("VR: shift the 3D model", "VR: posun 3D modelu")
S("VR: scale the 3D model", "VR: změna měřítka 3D modelu")
S("match the period", "srovnat periodu")
S("(zoomed out)", "(oddálené)")
S("zoom 2x", "měřítko 2x")
S("zoom 1x", "měřítko 1x")
S("zoom 0.5x", "měřítko 0,5x")
S("cylinder/donut view", "válcový/toroidální pohled")
S("first-person perspective", "perspektiva první osoby (FPP)")
S("toggle full 3D graphics", "přepnout úplnou 3D grafiku")
S("advanced projections", "pokročilé projekce")

// map editor
S("F5 = clear", "F5 = vyčistit")
S("Are you sure you want to clear the map?", "Opravdu chceš vyčistit mapu?")
S("select area to copy", "vyber oblast pro zkopírování")
S("statue", "socha")
S("c = reuse", "c = znovu použít")
S("d = delete", "d = vymazat")
S("p = grid color", "p = barva mřížky")
S("f = fill", "f = vyplnit")
S("T = text", "t = text")
S("e = erase", "e = kasuj")
S("z = spheres", "z = koule")
S("z-level", "z-hladina")

S("Failed to load pictures from %1", "Nahrání obrázků z %1 se nezdařilo")
S("Pictures loaded from %1", "Obrázky nahrány z %1")
S("Failed to save pictures to %1", "Uložení obrázků do %1 se nezdařilo")
S("Pictures saved to %1", "Obrázky uloženy do %1")
S("Hint: use F7 to edit floor under the player", "Nápověda: podlahu pod hráčem můžeš editovat pomocí klávesy F7")
S("Are you sure you want to restart? This will let you draw on a blank screen.", "Opravdu chceš restartovat? To ti umožní kreslit na prázdnou obrazovku.")
S("brush size", "velikost štětce")

// checking configurables
S("Aitoff parameter", "Aitoffův parametr")
S("The Aitoff projection is obtained by multiplying the longitude by 1/2, using azimuthal equidistant projection, and then dividing X by 1/2. Hammer projection is similar but equi-area projection is used instead. Here you can change this parameter.", 
  "Aitoffova projekce se získá tím, že vynásobíte zeměpisnou délku 1/2 pomocí azimutální délkojevné projekce, a potom X vydělíte 1/2. Hammerova projekce je podobná, jen místo délkojevné projekce používá plochojevnou. Zde můžeš změnit tento parametr.")
S("any spiral multiplier", "libovolný spirálový násobek")
S("Collignon parameter", "Collingtonův parametr")
S("Collignon reflect", "Collingtonův odraz")
S("ETS parameter", "ETS parametr")
S("eye level", "úroveň očí")
S("off", "vypnout")
S("spam", "spam")
S("normal monsters", "normální netvoři")
S("high-threat monsters only", "pouze velmi nebezpeční netvoři")
S("faraway highlight color", "barva zvýraznění vzdálených netvorů")
S("0 = monster color, 100 = red-light oscillation", "0 = barva netvora, 100 = oscilace červeného světla")
S("font size", "velikost fontu")
S("use specific fullscreen resolution", "použít specifické rozlišení v celoobrazovkovém módu")
S("fullscreen resolution to use (X)", "rozlišení v celoobrazovkovém módu (X)")
S("fullscreen resolution to use (Y)", "rozlišení v celoobrazovkovém módu (Y)")
S("hyperbolic|spherical|Euclidean", "hyperbolická|sférická|eukleidovská")
S("show flat", "plochý obraz")
S("loximuthal parameter", "loximutální parametr")
S("Loximuthal is similar to azimuthal equidistant, but based on loxodromes (lines of constant geographic direction) rather than geodesics. The loximuthal projection maps (the shortest) loxodromes to straight lines of the same length, going through the starting point. This setting changes the latitude of the starting point.\n\nIn retroazimuthal projections, a point is drawn at such a point that the azimuth *from* that point to the chosen central point is correct. For example, if you should move east, the point is drawn to the right. This parameter is the latitude of the central point.\n\n(In hyperbolic geometry directions are assigned according to the Lobachevsky coordinates.)", 
  "Loximutální projekce je podobná azimutální délkojevné projekci, ale je založená na loxodromách (čárách s konstantním geografickým směrem), nikoli na geodetikách. Loximutální projekce mapuje (nejkratší) loxodromy procházející výchozím bodem na úsečky stejné délky. Toto nastavení mění zeměpisnou šířku tohoto výchozího bodu.\n\nV retroazimutálních projekcích se bod vykresluje tak, že azimut *z* tohoto bodu do zvoleného centrálního bodu je správný. Pokud byste se například měli pohybovat na východ, bude bod napreslen vpravo. Tento parametr je zeměpisná šířka centrálního bodu.\n\n(V hyperbolické geometrii se směry přiřazují podle Lobačevského souřadnic.)")
 
S("onscreen keyboard", "klávesnice na obrazovce")
S("projection distance|Gans Klein Poincare orthographic stereographic", "vzdálenost projekce|Gans Klein Poincaré ortografická stereografická")
S("set relative font size", "nastavit relativní velikost fontu")
S("resizable window", "možnost měnit rozměr okna")
S("right spiral multiplier", "správný spirálový násobek")
S("fisheye parameter", "parametr rybího oka")
S("two-point parameter", "parametr dvoubodových modelů")
S("In two-point-based models, this parameter gives the distance from each of the two points to the center.", 
  "V dvoubodových modelech tento parametr udává vzdálenost od každého ze dvou bodů do středu.")
S("Winkel Tripel mixing", "Winkelovo tripelové mísení")
S("The Winkel Tripel projection is the average of Aitoff projection and equirectangular projection. Here you can change the proportion.", 
  "Winkelova tripelová projekce je průměr z Aitoffovy projekce a obdélníkojevné projekce. Zde můžeš změnit proporce.")
S("sphere spiral multiplier", "sférický spirálový násobek")
S("spiral cone", "spirálový kužel")
S("variation|dual|bitruncated", "variace|duální|bitrunkace")
S("screen size percentage to use (X)", "procento obrazovky (X)")
S("screen size percentage to use (Y)", "obrazovky (Y)")
S("specify relative window size", "specifikovat relativní velikost okna")
S("window resolution to use (X)", "rozlišení okna (X)")
S("window resolution to use (Y)", "rozlišení okna (Y)")

// 11.5 continued
//================

// fixed text

S("select the starting land", "vyber počáteční kraj")

// missing text in animation

S("cells to go", "zbývá políček")

// missing translations for Hardcore, and the new Casual mode

S("casual mode", "neformální mód")
S("Switching casual allowed only before saving the game. Do you want to restart?", 
  "Přepnutí neformálního módu je možné pouze předtím, než uložíš hru. Chceš restartovat?")
S("You are in the casual mode! Achievements are disabled.", 
  "Jsi v neformálním módu! Achievementy jsou vypnuté.")
S("Collect an Orb of Safety to save a checkpoint.", 
  "Sebráním Sféry Bezpečnosti uložíš checkpoint.")
S("load (%1 turns passed)", "nahrát (uplynulých kol: %1)")

// missing texts in searching

S("press letters to search", "pro vyhledávání piš písmena")
S("matching items", "odpovídající položky: ")

// YASC codes

S("YASC codes", "YASC kódy")
S(" YASC code: ", " YASC kód: ")
S("YASC codes: Sides-Entity-Restrict-Threat-Wall", "YASC kódy: Strany-Entita-Omezení-Hrozba-Zeď")

// Dice Reserve

N("Dice Reserve", GEN_F, "Rezervace kostek", "Rezervace kostek", "Rezervaci kostek", "v Rezervaci kostek")
S("This land is full of sentient dice. Do not disturb them!",
  "Tento kraj je plný inteligentních kostek. Neruš je!")

N("Crystal Die", GEN_F, "Křišťálová kostka", "Křišťálové kostky", "Křišťálovou kostku", "Křišťálovou kostkou")
S("A nice souvenir from the Dice Reserve. Make sure to collect the whole set!",
  "Pěkný suvenýr z Rezervace Kostek. Rozhodně nasbírej celou sadu!")

N("Unhappy Die", GEN_F, "Smutná kostka", "Smutné kostky", "Smutnou kostku", "Smutnou kostkou")
S("Sentient dice like to be in a position such that their highest number is on top. "
  "Unfortunately, someone has rolled this one into a wrong position, and did not fix this. "
  "It will reward you if you roll it so that the highest number is on top again!",

  "Inteligentní kostky jsou rády v poloze s nejvyšším číslem nahoře. "
  "Tuto kostku bohužel někdo překlopil do špatné pozice a už to nenapravil. "
  "Pokud ji překlopíš tak, aby bylo opět nahoře nejvyšší číslo, odmění tě!")

N("Happy Die", GEN_F, "Šťastná kostka", "Šťastné kostky", "Šťastnou kostku", "Šťastnou kostkou")

S(
  "A happy sentient die. Dice are happy when they are in their correct position "
  "(the highest number on the top); happy dice with one roll from their best position are sometimes found too."
  "Other positions are much less convenient for them and expose their "
  "weak spots. You can roll Happy Dice, but it may become angry!",

  "Šťastná inteligentní kostka. Kostky jsou šťastné když jsou ve správné poloze "
  "(s nejvyšším číslem nahoře); někdy se objevují i šťastné kostky, které jsou jedno překlopení od nejlepší polohy. "
  "Jiné polohy jsou pro ně mnohem méně příhodné a odhalují jejich "
  "slabiny. Šťastnou kostku můžeš překlápět, ale je možné, že ji to rozzlobí!")

N("Animated Die", GEN_F, "Oživlá kostka", "Oživlé kostky", "Oživlou kostku", "Oživlou kostkou")

S("When sentient dice are too long in an incorrect position, they start to move on their own, "
  "and attack everyone. You can still convince Animated Dice of your good intentions by "
  "rolling them into a position such that the highest number is on top. "
  "If you do, the die will stop moving and (if it happens in the Dice Reserve) you will be rewarded. "
  "Other rolls and attacks are not allowed.",
  
  "Když jsou inteligentní kostky příliš dlouho v nesprávné poloze, začnou se pohybovat samy od sebe "
  "a na všechny útočit. Oživlé kostky můžete stále přesvědčit o vašich dobrých úmyslech tím, "
  "že je překlopíš do polohy, v níž je nahoře její nejvyšší číslo. "
  "V tom případě se kostka přestane pohybovat a (pokud se to stane v Rezervaci kostek) dá ti odměnu. "
  "Jiná překlopení a útoky nejsou dovoleny."
  )

N("Angry Die", GEN_F, "Naštvaná kostka", "Naštvané kostky", "Naštvanou kostku", "Naštvanou kostkou")

S("You have made a die unhappy. Taste the revenge! This one won't forgive you, no matter what you do.",
  "Rozzlobil jsi kostku. Okus její pomstu! Ať uděláš, co uděláš, tato kostka ti už neodpustí.")

S("The die is now happy, and you are rewarded!", "Kostka je nyní šťastná, a ty získáváš odměnu!")
S("You have made a Happy Die angry!", "Rozzlobil jsi Šťastnou kostku!")
S("You can only push this die if the highest number would be on the top!", 
  "Do této kostky můžeš strčit jen tehdy, když bys tím dostal nahoru její nejvyšší číslo!")
S("This die is really angry at you!", "Tato kostka se na tebe OPRAVDU zlobí!")
S("The die is now happy, but won't reward you outside of the Dice Reserve!", "Tato kostka je nyní šťastná, ale mimo Rezervaci kostek ti žádnou odměnu nedá!")

// Cursed Canyon
//===============

N("Cursed Canyon", GEN_O, "Prokletý kaňon", "Prokleté kaňony", "Prokletý kaňon", "v Prokletém kaňonu")
S("This canyon is full of curses! Curses are like Orbs, but grant negative effects.",
  "Tento kaňon je plný kleteb! Kletby jsou jako Sféry, ale dávají negativní efekty.")
  
N("Canyon Hag", GEN_F, "Ježibaba z kaňonu", "Ježibaby z Kaňonu", "Ježibabu z Kaňonu", "Ježibabou z Kaňonu")
S("A Canyon Hag can curse you from afar, if standing on top of a Curse and close enough.",
  "Ježibaba z Kaňonu tě dokáže zdálky proklít, pokud stojí na nějaké Kletbě a je dostatečně blízko.")
S("%The1 curses you with %the2!", "%1 tě proklíná %a2!")

N("Capon Stone", GEN_O, "Kapouní kámen", "Kapouní kameny", "Kapouní kámen", "Kapouním kamenem")
S("A lapis alectorius, alectoria or capon stone is a non-precious stone found in the gizzard of capons (young, castrated roosters). In magic it is believed to be an effective amulet, granting the wearer a heightened sense of courage and boldness. These properties are unproven. [Wikipedia]",
  "Lapis alectorius, alectoria, neboli 'kapouní kámen' je kámen, který se nachází ve voleti kapounů (mladých, kastrovaných kohoutů). V oboru magie je pokládán za účinný amulet, který dává svému nositeli zvýšený pocit odvahy a smělost. Tyto vlastnosti nejsou prokázány. [Wikipedie]")

#define Curse(E, P) N("Curse of " E, GEN_F, "Kletba " P, "Kletby " P, "Kletbu " P, "Kletbou " P)
Curse("Weakness", "Slabosti")
S("This Curse weakens your normal attack, which will only stun monsters. For monsters which are normally only stunned anyway, they don't push the victim away. You are also unable to cut trees or fight Ivy.",
  "Tato kletba oslabuje tvůj normální útok, který bude nyní netvory pouze omračovat. U netvorů, které tvůj útok omračuje normálně, je nyní nebude odstrkovat. Kromě toho nemůžeš kácet stromy ani bojovat s Břečťanem.")

Curse("Draining", "Vysávání")
S("Drains the powers of all your orbs (and curses) by 2 charges per turn.",
  "Vysává sílu všech tvých Sfér (a Kleteb) rychlostí 2 nábojů za kolo.")

Curse("Water", "Vody")
S("Makes you leave a trail of shallow water. You also become afraid of water. You are unable to step into water, including shallow water and boats on water. "
  "On the good side, you are protected from fire.",
  
  "Způsobí, že za sebou zanecháváš stopu z mělké vody. Kromě toho se také začneš vody bát. Nemůžeš vstoupit do žádné vody, ani do mělké nebo do člunu na vodě. "
  "Na druhou stranu, jsi chráněný před ohněm.")
  
Curse("Fatigue", "Únavy")
S("With this Curse you cannot move too quickly. Every move increases your fatigue by 1; fatigue cannot go over 10. "
  "Resting reduces your fatigue by 5.",
  "Tato Kletba ti znemožňuje příliš rychlý pohyb. Pokaždé, když se pohneš, tvá únava se zvýší o 1; únava nemůže překročit 10. "
  "Odpočinkem se únava sníží o 5.")
N("fatigue", GEN_N, "únava", "únavy", "únavu", "únavou")
S("See the Curse of Fatigue.", "Viz Kletba Únavy.")
S("You are too fatigued!", "Jsi příliš unavený!")

Curse("Repulsion", "Odpuzování")
S("If you have this Curse, all the items on adjacent cells will try to escape from you.",
  "Když máš tuto Kletbu, všechny předměty na sousedních políčkách se před tebou budou snažit utéct.")

Curse("Gluttony", "Obžerství")
S("The first item you pick up is consumed without any effect.",
  "První předmět, který sebereš, bude bez jakéhokoli efektu pozřen.")
S("%The1 is consumed!", "%1 je pozřen!")

Orb("Purity", "Čistoty")
S("Reverses all the curses. When found far away from the Cursed Canyon, you gain low amounts of various reversed curse powers.",
  "Obrací všechny kletby. Pokud tuto Sféru najdeš daleko od Prokletého kaňonu, získáš malé množství různých obrácených schopností kleteb.")
#undef Curse
#undef Orb

// chapter names of the Guided Tour
S("Hyperbolic shapes", "Hyperbolické tvary")
S("Projections of hyperbolic space", "Projekce hyperbolického prostoru")
S("Procedural generation", "Procedurální generace")

S("Find an Orb of Safety to save your game",
  "Pro uložení hry najdi Sféru Bezpečnosti")

S("how to find an Orb of Safety?", "jak najít Sféru Bezpečnosti?")

S(
  "To save the game you need an Orb of Safety.\n\n"
  "Orbs of Safety appear:\n\n"
  "* in the Crossroads and the Land of Eternal Motion, after you collect %1 Phoenix Feathers in the Land of Eternal Motion\n\n"
  "* in the Ocean after you unlock it (%2 treasures)\n\n"
  "* in the Prairie after you unlock it (%3 treasures)\n\n",

  "Pro uložení hry potřebuješ Sféru Bezpečnosti.\n\n"
  "Sféry Bezpečnosti se objevují:\n\n"
  "* na Křižovatce a v Kraji Věčného pohybu poté, co získáš %1 Fénixových pírek v Kraji Věčného pohybu\n\n"
  "* v Oceánu poté, co ho odemkneš (%2 pokladů)\n\n"
  "* na Prérii poté, co ji odemkneš (%3 pokladů)\n\n"
  )

