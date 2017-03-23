// HyperRogue Czech translation, by Marek Čtrnáct <marek14@seznam.cz>
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

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
S("To access Hell, collect 10 treasures each of 9 kinds...", "Aby ses dostal do Pekla, sesbírej 10 pokladů od každého z 9 různých typů...")
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
S("Collect 30 $$$ to access more worlds", "Další kraje zpřístupníš sesbíráním 30 $$$")
// S("Collect 60 $$$ to access R'Lyeh and Dry Forest", "R'Lyeh a Suchý hvozd zpřístupníš sesbíráním 60 $$$")
S("Collect at least 10 treasures in each of 9 types to access Hell", "Peklo zpřístupníš sesbíráním 10 pokladů od každého z 9 typů")
S("Collect at least 10 Demon Daisies to find the Orbs of Yendor", "Yendorské sféry zpřístupníš sesbíráním nejméně 10 Čertových kvítek")
S("Hyperstone Quest: collect at least 10 %1 in %the2", "Hyperkamový úkol: sesbírej nejméně 10 pokladů %abl2")
S("Hyperstone Quest completed!", "Hyperkamový úkol splněn!")
S("Look for the Orbs of Yendor in Hell or in the Crossroads!", "Hledej Yendorské sféry v Pekle nebo na Křižovatce!")
S("Unlock the Orb of Yendor!", "Odemkni Yendorskou sféru!")
S("Defeat 100 enemies to access the Graveyard", "Hřbitov zpřístupníš poražením 100 nepřátel")
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
S("exit configuration", "opustit nastavení")
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
S("exit configuration", "opustit nastavení")
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

S("Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Home/Space. "
    "To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n",
 "Pohybuješ se pomocí myši, numerické klávesnice, kláves qweadzxc nebo kláves hjklyubn. Klávesy 's' nebo '.' ti umožňují čekat na místě. "
 "Šipky, klávesy PgUp/PgDn a Home nebo mezerník ti umožňují otáčet svět. Nastavení můžeš otevřít klávesou 'v', úkoly a menu klávesou ESC.\n\n")

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

S("A nasty creature native to the Living Caves. They don't like you "
 "for some reason.",
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

S("A creepy monster who follows you everywhere in the Graveyard.",
  "Děsivý netvor, který tě sleduje po celém Hřbitově.")

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
 "Crystal Sages melt at -30 °C, and they can rise the temperature around you from afar.",
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
S("Scores and achievements are not", "V Eukleidovském módu se");
S("saved in the Euclidean mode!", "neukládají skóre a achievementy!");

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

S("Collect 60 $$$ to access even more lands", "Sesbíráním 60 $$$ získáš přístup do dalších krajů")

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

S("You have to escape first!", "Nejprve musíš utéct!");
S("There is not enough space!", "Není tady dost místa!");

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
S("draw the heptagons darker", "tmavší sedmiúhelníky");
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

S("%The1 drowns!", "%1 se utopi%l1!");
S("%The1 falls!", "%1 spad%l1!");

// these were missing from the translation for some reason

S("Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.",
  "Tato jezera se nacházejí po celém Pekle... Mají tvar zlých hvězd a jsou naplněna hořící sírou.")

// Hardcore Mode
// -------------

S("hardcore mode", "hardcore mód");

S("One wrong move and it is game over!", "Jediný špatný tah a hra končí!");
S("Not so hardcore?", "Že bys nebyl zas až tak hardcore?");

// Shoot'em up Mode
// ----------------

S("shoot'em up mode", "mód střílečky");
S("Welcome to the Shoot'em Up mode!", "Vítej v módu střílečky!");
S("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move", "F/;/Space/Enter/KP5 = střelba, WASD/IJKL/Numpad = pohyb");

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

S("Cannot drop %the1 here!", "Tady nelze položit Mrtvou Sféru!");

// Euclidean scores
// ----------------

S(" (E:%1)", " (E:%1)");

S("You cannot attack Rock Snakes directly!", "Na Kamenné hady nemůžeš útočit přímo!");

S("\"I am lost...\"", "\"I am lost...\"");

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
S("%The1 sinks!", "%1 zmize%l1 pod vodou!");

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
S("%The1 squeaks gratefully!", "%1 vděčně piští!");
S("%The1 squeaks hopelessly.", "%1 zdrceně piští.");
S("%The1 squeaks in despair.", "%1 zoufale piští.");
S("%The1 squeaks sadly.", "%1 smutně piští.");
S("%The1 squeaks with hope!", "%1 nadějně piští!");
S("%The1 squeaks happily!", "%1 šťastně piští!");
S("%The1 squeaks excitedly!", "%1 vzrušeně piští!");

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
S("Hardness frozen at %1.", "Obtížnost nastavená na: %1.");
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
S("Finished lands required: %1 (collect 10 treasure)\n",
  "Potřebuješ dokončit %1 krajů (získat v nich 10 pokladů)\n")

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
S(" Hell: %1/9", " Peklo: %1/9")

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
  "tend do disappear after you have collected many of them.",
  "Nádherná figurka ze slonové kosti. Figurky poblíž základů Věže mají tendenci "
  "mizet, pokud jich získáš hodně.")

N("Platform", GEN_F, "Plošinka", "Plošinky", "Plošinku", "Plošinkou")
N("platform", GEN_F, "plošinka", "plošinky", "plošinku", "plošinkou")
S("You can stand here.", "Na této plošince můžeš stát.")

N("ladder", GEN_O, "žebřík", "žebříky", "žebřík", "žebříkem")
S("You can use this ladder to climb the Tower.",
  "Po tomto žebříku můžeš lézt na Věž.")

Orb("Matter", "Hmoty")

S("This Orb allows to temporarily create impassable matter, either to block paths or "
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
S("back to HyperRogue", "zpět do HyperRogue")
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

S("Wow! That was close.", "Uf! To bylo o chlup!");

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
  "outcoming wind. However, you can move two cells with the wind in a single turn, "
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

S(  "This Orb lets your allies to share your Orb powers.\n\n"
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

S(
  "In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
  "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n"
  "This requires some OpenGL extensions and may crash or not work correctly -- enabling "
  "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
  "will make the rendering faster, but the surface will be rendered only once, so "
  "you won't be able to play a game on it.\n\n"
  "Use arrow keys to rotate, Page Up/Down to zoom.",

  "V tomto módu můžete hrát HyperRogue na 3D modelu části hyperbolické roviny "
  "podobném tomu, co vytváří generátor papírových modelů nebo hyperbolické háčkování.\n\n"
  "Je k tomu zapotřebí OpenGL a je možné, že v tomto módu hra spadne nebo nepoběží "
  "správně -- v takovém případě může pomoci zapnout možnost 'renderuj textury bez OpenGL'. "
  "Kromě toho je možné renderování urychlit volbou 'renderuj jednou', ale v takovém případě "
  "bude povrch vygenerován pouze jednou, takže na něm není možné hrát.\n\n"
  "Model se otáčí šipkami a lze ho zoomovat klávesami Page Up/Down.")

S("what's this?", "co to je?")
S("take me back", "návrat do menu")
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

S("Don't be fooled by beauty, or you will be stabbed if you come too close!",
  "Nenech se oklamat krásou -- když se moc přiblížíš, můžeš dostat ránu nožem!")

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

S("%The1 is killed by thorns!", "%1 se nabod%l na trny!");

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
S("exit this menu", "ven z menu")
S("Enable cheat mode or GAME OVER to use this", "Dostupné pouze v cheat módu nebo po skončení hry")

// remember to note it's in English
S("see http://www.roguetemple.com/z/hyper/conformal.php", "viz http://www.roguetemple.com/z/hyper/conformal.php (anglicky)")

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
   "but you cannot collect items. When only one charge is left, "
   "you have to dismount this turn -- be very careful to make this possible, "
   "as your mount could attack you immediately!\n\n" "While riding, "
   "click on a location to order your mount to move or attack there.",

   "Tato Sféra ti umožňuje jezdit na Dracích a jiných červovitých netvorech. "
   "Stačí se na ně pohnout, a můžeš nasednout; během jízdy jsi chráněn před nebezpečným terénem "
   "a částečně i před útoky (ty tě připraví o polovinu zbývající síly Dominance), "
   "nemůžeš ale sbírat předměty. Ve chvíli, kdy ti zbývá pouze jeden náboj, "
   "musíš v tomto kole sesednout -- dej si dobrý pozor, aby to bylo možné,  "
   "protože tvůj oř na tebe může okamžitě zaútočit!\n\n "
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

S("Hyperstone Quest: collect at least 10 points in %the2",
  "Hyperkamový úkol: získej nejméně 10 bodů %abl2")

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

S("player", "hráč")
S("movement color", "barva pohybu")
S("model used (not conformal!)", "použitý model (nekonformální!)")
S("azimuthal equidistant", "azimutální délkojevný")
S("azimuthal equi-area", "azimutální plochojevný")
S("visit the website", "navštiv webovou stránku")
S("leaderboards/achievements", "leaderboardy/achievementy")
S("return to the game", "návrat do hry")

// Android texts

S("connected to Google Games", "připojeno ke Google Games")
S("background music", "hudba")
S("view your achievements", "tvé achievementy")
S("next page", "další stránka")

// new multiplayer

S("shoot'em up and multiplayer", "střílečka / více hráčů")

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
  "There are Krakens in your homeland too... huge sea monsters which "
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

// trollheim

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
//==============

// 9.0 patches
//=============

// extra help
S("Coastal region -- connects inland and aquatic regions.\n",
  "Pobřežní kraj -- spojuje vnitrozemské a mořské kraje.\n")

S("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n",
  "Kraina morska -- dostępna jedynie zs krain przybrzeżnych i morskic.\n")

// barrow treasure protected from Orb of Space
S("%The1 is protected from this kind of magic!",
  "%1 má ochranu proti tomuto typu magie!")

// Orb is always available in the given land
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
S("return", "návrat")
S("F1 - help", "F1 - nápověda")

// for the conformal polynomial
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
//=============

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

S("Killed %1 can be revived with Orb of the Love, after you collect 20 more $$$.",
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
S("You gain a protective Shell!", "Získa%l0 jsi ochranný Krunýř!");
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
//=============

S("SORT", "TŘÍDIT")
S("PICK", "VYBRAT")
S("PLAY", "HRÁT")

// 3D configuration
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
S("exit 3D configuration", "opustit 3D konfiguraci")

S("Objects at distance less than %1 absolute units "
  "from the center will be displayed with high "
  "detail, and at distance at least %2 with low detail.",
  
  "Objekty ve vzdálenosti méně než %1 od středu budou zobrazovány s vysokými detaily, a objekty ve vzdálenosti méně než %2 se středními detaily.")

S("Ground level is actually an equidistant surface, "
  "%1 absolute units below the plane P. "
  "Theoretically, this value affects the world -- "
  "for example, eagles could fly %2 times faster by "
  "flying above the ground level, on the plane P -- "
  "but the actual game mechanics are not affected. "
  "(Distances reported by the vector graphics editor "
  "are not about points on the ground level, but "
  "about the matching points on the plane P -- "
  "divide them by the factor above to get actual "
  "distances.",
  
  "Úroveň země je ve skutečnosti ekvidistantní plocha, která se nachází %1 absolutních "
  "jednotek pod rovinou P. Tato hodnota teoreticky ovlivňuje svět -- orli by například "
  "mohli létat %2-krát rychleji, kdyby létali nad úrovní země v rovině P -- ale herní "
  "mechaniky jako takové tím ovlivněny nejsou. (Vzdálenosti v editoru vektorové "
  "grafiky nejsou vzdálenosti bodů na úrovni země, ale odpovídajících bodů v rovině "
  "P -- skutečné vzdálenosti získáte jejich vydělením výše zmíněným faktorem.")

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
//---------

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
  "Pick up an Orb of Safety causes an immediate recall.",
  
  "Když vyprší náboje této Sféry, automaticky se vrátíš tam, kde jsi ji našel. "
  "Pokud sebereš více Sfér Návratu, místo návratu se nezmění -- návrat se pouze "
  "oddálí. Pokud sebereš Sféru Bezpečnosti, vrátíš se okamžitě."
  )
// Maybe needs new variety for "našel" -- the female form is "našla".

// 
//

// lost mountain
//===============

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
//==========

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
//=============

// not previously translated
S("shift+O to switch anti-aliasing", "Stiskem Shift+O se přepíná antialiasing")
S("For some reason... cannot attack!", "Z nějakého důvodu... nemůžeš útočit!")
S("Your energy swords get stronger!", "Tvé energetické meče sílí!")
S("You feel even more attuned to the magic of this land!",
  "Cítíš se ještě více přizpůsobený magii tohoto kraje!")
S("Quite tough, for your first fight.", "Na první bitku docela obtížný soupeř.")
S("(You can also use right Shift)\n\n", "(Také lze použít pravý Shift)\n\n")

// Crossroads V
//--------------

N("Crossroads V", GEN_O, "Křižovatka V", "Křižovatky V", "Křižovatku V", "na Křižovatce V")
S("Extremely narrow Crossroads layout.\n", "Velmi úzká verze Křižovatky.")

// Bull Dash/Prairie common

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
//---------

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
//-----------

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
//----------------

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
//------------

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

#undef Orb
