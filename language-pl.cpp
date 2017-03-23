// HyperRogue Polish translation, by Zeno and Tehora Rogue
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

N("Yeti", GEN_M, "Yeti", "Yeti", "Yeti", "Yeti")
N("Icewolf", GEN_M, "Lodowy Wilk", "Lodowe Wilki", "Lodowego Wilka", "Lodowym Wilkiem")
N("Ranger",  GEN_M, "Strażnik", "Strażnicy", "Strażnika", "Strażnikiem")
N("Rock Troll", GEN_M, "Skalny Troll", "Skalne Trolle", "Skalnego Trolla", "Skalnym Trollem")
N("Goblin", GEN_M, "Goblin", "Gobliny", "Goblina", "Goblinem")
N("Sand Worm", GEN_M, "Pustynny Czerw", "Pustynne Czerwie", "Pustynnego Czerwia", "Pustynnym Czerwiem")
N("Sand Worm Tail", GEN_M, "Ogon Czerwia", "Ogony Czerwia", "Ogon Czerwia", "Ogonem Czerwia")
N("Sand Worm W", GEN_M, "Pustynny Czerw W", "Pustynne Czerwie W", "Pustynnego Czerwia W", "Pustynnym Czerwiem W")
N("Desert Man", GEN_M, "Człowiek Pustyni", "Ludzie Pustyni", "Człowieka Pustyni", "Człowiekiem Pustyni")
N("Ivy Root", GEN_M, "Korzeń Bluszczu", "Korzenie Bluszczu", "Korzeń Bluszczu", "Korzeniem Bluszczu")
N("Active Ivy", GEN_M, "Aktywny Bluszcz", "Aktywne Bluszcze", "Aktywny Bluszcz", "Aktywnym Bluszczem")
N("Ivy Branch", GEN_F, "Gałąź Bluszczu", "Gałęzie Bluszczu", "Gałąź Bluszczu", "Gałęzią Bluszczu")
N("Dormant Ivy", GEN_M, "Uśpiony Bluszcz", "Uśpione Bluszcze", "Uśpiony Bluszcz", "Uśpionym Bluszczem")
N("Ivy N", GEN_M, "Bluszcz N", "Bluszcze N", "Bluszcz N", "Bluszczem N")
N("Ivy D", GEN_M, "Bluszcz D", "Bluszcze D", "Bluszcz D", "Bluszczem D")
N("Giant Ape", GEN_F, "Wielka Małpa", "Wielkie Małpy", "Wielką Małpę", "Wielką Małpą")
N("Slime Beast", GEN_M, "Mazisty Stwór", "Maziste Stwory", "Mazistego Stwora", "Mazistym Stworem")
N("Mirror Image", GEN_N, "Odbicie Lustrzane", "Odbicia Lustrzane", "Odbicie Lustrzane", "Odbiciem Lustrzanym")
N("Mirage", GEN_M, "Miraż", "Miraże", "Miraż", "Mirażem")
N("Golem", GEN_M, "Golem", "Golemy", "Golema", "Golemem")
N("Eagle", GEN_M, "Orzeł", "Orły", "Orła", "Orłem")
N("Seep", GEN_M, "Sączak", "Sączaki", "Sączaka", "Sączakiem")
N("Zombie", GEN_M, "Zombie", "Zombies", "Zombie", "Zombie")
N("Ghost", GEN_M, "Duch", "Duchy", "Ducha", "Duchem")
N("Necromancer", GEN_M, "Nekromanta", "Nekromanci", "Nekromantę", "Nekromantą")
N("Shadow", GEN_M, "Cień", "Cienie", "Cień", "Cieniem")
N("Tentacle", GEN_F, "Macka", "Macki", "Mackę", "Macką")
N("Tentacle Tail", GEN_F, "Ogon Macki", "Ogony Macki", "Ogon Macki", "Ogonem Macki")
N("Tentacle W", GEN_F, "Macka W", "Macki W", "Mackę W", "Macką W")
N("Tentacle (withdrawing)", GEN_F, "Macka (uciekająca)", "Macki (uciekające)", "Mackę (uciekającą)", "Macką (uciekającą)")
N("Cultist", GEN_M, "Kultysta", "Kultyści", "Kultystę", "Kultystą")
N("Fire Cultist", GEN_M, "Ognisty Kultysta", "Ogniści Kultyści", "Ognistego Kultystę", "Ognistym Kultystą")
N("Greater Demon", GEN_M, "Wielki Demon", "Wielkie Demony", "Wielkiego Demona", "Wielkim Demonem")
N("Lesser Demon", GEN_M, "Mniejszy Demon", "Mniejsze Demony", "Mniejszego Demona", "Mniejszym Demonem")
N("Ice Shark", GEN_M, "Lodowy Rekin", "Lodowe Rekiny", "Lodowego Rekina", "Lodowym Rekinem")
N("Running Dog", GEN_M, "Biegnący Pies", "Biegnące Psy", "Biegnącego Psa", "Biegnącym Psem")
N("Demon Shark", GEN_M, "Rekin-Demon", "Rekiny-Demony", "Rekina-Demona", "Rekinem-Demonem")
N("Fire Fairy", GEN_F, "Ognista Wiła", "Ogniste Wiły", "Ognistą Wiłę", "Ognistą Wiłą")
N("Crystal Sage", GEN_M, "Kryształowy Mędrzec", "Kryształowi Mędrcy", "Kryształowego Mędrca", "Kryształowym Mędrcem")
N("Hedgehog Warrior", GEN_M, "Wojownik-Jeż", "Wojownicy-Jeże", "Wojownika-Jeża", "Wojownikiem-Jeżem")

// ITEMS
// =====

N("Ice Diamond", GEN_O, "Lodowy Diament", "Lodowe Diamenty", "Lodowy Diament", "Lodowym Diamentem")
N("Gold", GEN_N, "Złoto", "Złota", "Złoto", "Złotem")
N("Spice", GEN_F, "Przyprawa", "Przyprawy", "Przyprawę", "Przyprawą")
N("Ruby", GEN_O, "Rubin", "Rubiny", "Rubin", "Rubinem")
N("Elixir of Life", GEN_O, "Eliksir Życia", "Eliksiry Życia", "Eliksir Życia", "Eliksirem Życia")
N("Shard", GEN_O, "Odłamek", "Odłamki", "Odłamek", "Odłamkiem")
N("Necromancer's Totem", GEN_O, "Totem Nekromanty", "Totemy Nekromanty", "Totem Nekromanty", "Totemem Nekromanty")
N("Demon Daisy", GEN_F, "Czarcie Ziele", "Czarcie Ziela", "Czarcie Ziele", "Czarcim Zielem")
N("Statue of Cthulhu", GEN_F, "Statuetka", "Statuetki", "Statuetkę", "Statuetką")
N("Phoenix Feather", GEN_N, "Pióro Feniksa", "Pióra Feniksa", "Pióro Feniksa", "Piórem Feniksa")
N("Ice Sapphire", GEN_O, "Lodowy Szafir", "Lodowe Szafiry", "Lodowy Szafir", "Lodowym Szafirem")
N("Hyperstone", GEN_O, "Hiperkamień", "Hiperkamienie", "Hiperkamień", "Hiperkamieniem")
N("Key", GEN_O, "Klucz", "Klucze", "Klucz", "Kluczem")
N("Dead Orb", GEN_F, "Martwa Sfera", "Martwe Sfery", "Martwą Sferę", "Martwą Sferą")
N("Fern Flower", GEN_O, "Kwiat Paproci", "Kwiaty Paproci", "Kwiat Paproci", "Kwiatem Paproci")

// ORBS: we are using a macro here
// ===============================

#define Orb(E, P) N("Orb of " E, GEN_F, "Sfera " P, "Sfery " P, "Sferę " P, "Sferą " P)
Orb("Yendor", "Yendoru")
Orb("Storms", "Burzy")
Orb("Flash", "Błysku")
Orb("Winter", "Zimy")
Orb("Speed", "Szybkości")
Orb("Life", "Życia")
Orb("Shielding", "Tarczy")
Orb("Teleport", "Teleportacji")
Orb("Safety", "Bezpieczeństwa")
Orb("Thorns", "Cierni")

// TERRAIN FEATURES
// ================

N("none", GEN_O, "nic", "nic", "niczego", "niczym")
N("ice wall", GEN_F, "lodowa ściana", "lodowe ściany", "lodową ścianę", "lodową ścianą")
N("great wall", GEN_F, "wielka ściana", "wielkie ściany", "wielką ścianę", "wielką ścianą")
N("red slime", GEN_F, "czerwona maź", "czerwone mazie", "czerwoną maź", "czerwoną mazią")
N("blue slime", GEN_F, "niebieska maź", "niebieskie mazie", "niebieską maź", "niebieską mazią")
N("living wall", GEN_F, "żywa ściana", "żywe ściany", "żywą ścianę", "żywą ścianą")
N("living floor", GEN_F, "żywa ziemia", "żywe ziemie", "żywą ziemię", "żywą ziemią")
N("dead troll", GEN_M, "martwy troll" ,"martwe trolle", "martwego trolla", "martwym trollem")
N("sand dune", GEN_F, "wydma", "wydmy", "wydmę", "wydmą")
N("Magic Mirror", GEN_N, "Magiczne Lustro", "Magiczne Lustra", "Magiczne Lustro", "Magicznym Lustrem")
N("Cloud of Mirage", GEN_F, "Chmura Mirażowa", "Chmury Mirażowe", "Chmurę Mirażową", "Chmurą Mirażową")
N("Thumper", GEN_O, "Dudnik", "Dudniki", "Dudnik", "Dudnikiem")
N("Bonfire", GEN_N, "Ognisko", "Ogniska", "Ognisko", "Ogniskiem")
N("ancient grave", GEN_O, "stary grób", "stare groby", "stary grób", "starym grobem")
N("fresh grave", GEN_O, "świeży grób", "świeże groby", "świeży grób", "świeżym grobem")
N("column", GEN_F, "kolumna", "kolumny", "kolumnę", "kolumną")
N("lake of sulphur", GEN_N, "jezioro siarki", "jeziora siarki", "jezioro siarki", "jeziorem siarki")
N("lake", GEN_N, "jezioro", "jeziora", "jezioro", "jeziorem")
N("frozen lake", GEN_N, "zamarznięte jezioro", "zamarznięte jeziora", "zamarznięte jezioro", "zamarzniętym jeziorem")
N("chasm", GEN_N, "przepaść", "przepaści", "przepaść", "przepaścią")
N("big tree", GEN_N, "duże drzewo", "duże drzewa", "duże drzewo", "dużym drzewem")
N("tree", GEN_N, "drzewo", "drzewo", "drzewo", "drzewem")

// LANDS
// =====

N("Great Wall", GEN_N, "Wielka Ściana", "Wielka Ściana", "Wielką Ścianę", "na Wielkiej Ścianie")
N("Crossroads", GEN_N, "Skrzyżowanie", "Skrzyżowanie", "Skrzyżowanie", "na Skrzyżowaniu")
N("Desert", GEN_F, "Pustynia", "Pustynia", "Pustynię", "na Pustyni")
N("Icy Land", GEN_F, "Lodowa Kraina", "Lodowa Kraina", "Lodową Krainę", "w Lodowej Krainie")
N("Living Cave", GEN_F, "Żywa Jaskinia", "Żywa Jaskinia", "Żywą Jaskinię", "w Żywej Jaskini")
N("Jungle", GEN_F, "Dżungla", "Dżungla", "Dżunglę", "w Dżungli")
N("Alchemist Lab", GEN_N, "Laboratorium", "Laboratorium", "Laboratorium", "w Laboratorium")
N("Mirror Land", GEN_F, "Kraina Luster", "Kraina Luster", "Krainę Luster", "w Krainie Luster")
N("Graveyard", GEN_O, "Cmentarz", "Cmentarz", "Cmentarz", "na Cmentarzu")
N("R'Lyeh", GEN_N, "R'Lyeh", "R'Lyeh", "R'Lyeh", "w R'Lyeh")
N("Hell", GEN_N, "Piekło", "Piekło", "Piekło", "w Piekle")
N("Cocytus", GEN_M, "Kocyt", "Kocyt", "Kocytu", "w Kocycie")
N("Land of Eternal Motion", GEN_N, "Kraina Wiecznego Ruchu", "Kraina Wiecznego Ruchu", "Krainę Wiecznego Ruchu", "w Krainie Wiecznego Ruchu")
N("Dry Forest", GEN_M, "Puszcza", "Puszcza", "Puszczę", "w Puszczy")
N("Game Board", GEN_F, "Plansza", "Plansza", "Planszę", "na Planszy")

// GAME MESSAGES
// =============

// fighting messages
// -----------------

// For each English form, provide a Polish form. Player is referred to via %...0,
// and objects are referred to via %...1 and %...2. For example, in Polish:

// %a1 refers to the accusative form of the first object (as explained above in 'Nouns')
// likewise, %a2 refers to the accusative form of the second object
// %ł1 is replaced by "ł", "ło" or "ła", depending on the gender of %1
// %łeś0 adds "łeś" or "łaś" depending on the gender of the player

// Use whatever %xxx# codes you need for your language. Of course,
// tell me how your codes should be expanded.

S("You kill %the1.", "Zabi%łeś0 %a1.")
S("You would be killed by %the1!", "Zabi%ł1by Cię %1!")
S("%The1 would get you!", "Dopad%ł1by Cię %1!")
S("%The1 destroys %the2!", "%1 zniszczy%ł1 %a2!")
S("%The1 eats %the2!", "%1 zjad%ł1 %a2!")
S("The ivy destroys %the1!", "Bluszcz zniszczył %a1!")
S("%The1 claws %the2!", "%1 zaatakowa%ł1 %a2!")
S("%The1 scares %the2!", "%1 wystraszy%ł1 %a2!")
S("%The1 melts away!", "%1 się stopi%ł1!")
S("%The1 stabs %the2.", "%1 dźgn%ął1 %a2.")
S("You stab %the1.", "Dźgn%ąłeś0 %a1.")

S("You cannot attack %the1 directly!", "Nie możesz atakować %a1 wprost!")
S("Stab them by walking around them.", "Przejdź wokół, by dźgnąć.")

S("You feel more experienced in demon fighting!", "Jesteś bardziej doświadczony w walce z demonami!")
S("Cthulhu withdraws his tentacle!", "Cthulhu chowa swoją mackę!")
S("The sandworm explodes in a cloud of Spice!", "Czerw eksplodował! Chmura Przyprawy!")
S("%The1 is confused!", "%1 jest zmieszan%ya1.")
S("%The1 raises some undead!", "%1 ożywia nieumarłych!")
S("%The1 throws fire at you!", "%1 rzuca w Ciebie ogniem!")
S("%The1 shows you two fingers.", "%1 pokaza%ł1 Ci dwa palce.")
S("You wonder what does it mean?", "Co to może znaczyć?")
S("%The1 shows you a finger.", "%1 pokaza%ł1 Ci palec.")
S("You think about possible meanings.", "Zastanawiasz się nad możliwymi znaczeniami.")
S("%The1 moves his finger downwards.", "%1 przesun%ął1 swój palec w dół.")
S("Your brain is steaming.", "Twój mózg paruje.")
S("%The1 destroys %the2!", "%1 zniszczy%ł1 %a2!")
S("You join %the1.", "Połączy%łeś0 się z %abl1.")
S("Two of your images crash and disappear!", "Dwa Twoje obrazy wpadły na siebie i znikły!")
S("%The1 breaks the mirror!", "%1 rozbi%ł1 lustro!")
S("%The1 disperses the cloud!", "%1 rozprys%ł1 chmurę!")
S("You activate the Flash spell!", "Aktywowa%łeś0 czar Błysk!")
S("You activate the Lightning spell!", "Aktywowa%łeś0 czar Burza!")
S("Ice below you is melting! RUN!", "Lód pod Tobą się topi! UCIEKAJ!")
S("This spot will be burning soon! RUN!", "To się zaraz zapali! UCIEKAJ!")
S("The floor has collapsed! RUN!", "Ziemia się zapadła! UCIEKAJ!")
S("You need to find the right Key to unlock this Orb of Yendor!",
  "Potrzebujesz prawidłowego Klucza, by otworzyć Sferę Yendoru!")
S("You fall into a wormhole!", "Wpad%łeś0 w tunel przestrzenny!")
S("You activate %the1.", "Aktywowa%łeś0 %a1.")
S("No room to push %the1.", "Brak miejsca, by popchnąć %a1.")
S("You push %the1.", "Popchn%ąłeś0 %a1.")
S("You start chopping down the tree.", "Zaczynasz ciąć drzewo.")
S("You chop down the tree.", "Ści%ąłeś0 drzewo.")
S("You cannot attack Sandworms directly!", "Nie możesz atakować Czerwi wprost!")
S("You cannot attack Tentacles directly!", "Nie możesz atakować Macek wprost!")
S("You cannot defeat the Shadow!", "Nie możesz pokonać Cienia!")
S("You cannot defeat the Greater Demon yet!", "Nie możesz jeszcze pokonać Wielkiego Demona!")
S("That was easy, but groups could be dangerous.", "To było proste, z grupami może być trudniej.")
S("Good to know that your fighting skills serve you well in this strange world.", "Dobrze wiedzieć, że Twoje zdolności walki przydają się w tym świecie.")
S("You wonder where all these monsters go, after their death...", "Gdzie te wszystkie potwory idą po śmierci?...")
S("You feel that the souls of slain enemies pull you to the Graveyard...", "Czujesz, jak dusze pokonanych wrogów wzywają Cię na Cmentarz...")
S("Wrong color!", "Zły kolor!")
S("You cannot move through %the1!", "Nie możesz przejść przez %a1!")
S("%The1 would kill you there!", "Zabi%ł1by Ciebie %1!")
S("Wow! %1! This trip should be worth it!", "%1! Ta wyprawa powinna się opłacić!")
S("For now, collect as much treasure as possible...", "Szukaj skarbów, znajdź, ile tylko możesz...")
S("Prove yourself here, then find new lands, with new quests...", "Pokaż, co potrafisz tutaj, potem znajdź nowe krainy i misje...")
S("You collect your first %1!", "Znalaz%łeś0 pierwsz%yą1 %a1!")
S("You have found the Key! Now unlock this Orb of Yendor!", "Znalaz%łeś0 Klucz! Otwórz Sferę Yendoru!")
S("This orb is dead...", "Ta sfera jest martwa...")
S("Another Dead Orb.", "Jeszcze jedna Martwa Sfera.")
S("You have found %the1!", "Znalaz%łeś0 %a1!")
S("You feel that %the2 become%s2 more dangerous.", "Masz wrażenie, że %P2 staje się bardziej niebezpieczn%ya2.")
S("With each %1 you collect...", "Z każd%ymą1 %abl1, które znajdujesz...")
S("Are there any magical orbs in %the1?...", "Czy %abl1 są jakieś magiczne sfery?")
S("You feel that %the1 slowly become%s1 dangerous...", "Masz wrażenie, że %P1 powoli staje się niebezpieczn%ya1...")
S("Better find some other place.", "Lepiej znajdź inne miejsce.")
S("You have a vision of the future, fighting demons in Hell...", "Masz wizję przyszłości, w której walczysz z demonami w Piekle...")
S("With this Elixir, your life should be long and prosperous...", "Ten Eliksir pozwoli Ci na długie i szczęśliwe życie...")
S("The Necromancer's Totem contains hellish incantations...", "Totem Nekromanty zawiera piekielne inkantacje...")
S("The inscriptions on the Statue of Cthulhu point you toward your destiny...", "Inskrypcje na Statui Cthulhu wskazują Ci Twoje Przeznaczenie...")
S("Still, even greater treasures lie ahead...", "Jeszcze większe skarby przed Tobą...")
S("You collect %the1.", "Znalaz%łeś0 %a1.")
S("CONGRATULATIONS!", "GRATULACJE!")
S("Collect treasure to access more different lands...", "Zbieraj skarby, by znaleźć nowe krainy...")
S("You feel that you have enough treasure to access new lands!", "Masz wystarczająco wiele skarbów, by znaleźć nowe krainy!")
S("Collect more treasures, there are still more lands waiting...", "Zbieraj skarby, nowe krainy czekają...")
S("You feel that the stars are right, and you can access R'Lyeh!", "Gwiazdy są na miejscu, R'Lyeh czeka!")
S("Kill monsters and collect treasures, and you may get access to Hell...", "Zabijaj potwory, zdobywaj skarby, może trafisz do Piekła...")
S("To access Hell, collect 10 treasures each of 9 kinds...", "By dostać się do Piekła, znajdź po 10 skarbów każdego z 9 rodzajów...")
S("Abandon all hope, the gates of Hell are opened!", "Porzuć wszelką nadzieję, bramy Piekła są otwarte!")
S("And the Orbs of Yendor await!", "I sfery Yendoru czekają!")
S("You switch places with %the1.", "Zamieniasz się miejscami z %abl1.")
S("You rejoin %the1.", "Połączy%łeś0 się z %abl1.")
S("The mirror shatters!", "Lustro pęka!")
S("The cloud turns into a bunch of images!", "Chmura zamienia się w obrazy!")
S("The slime reacts with %the1!", "Maź reaguje z %abl1!")
S("You drop %the1.", "Porzuci%łeś0 %a1.")
S("You feel great, like a true treasure hunter.", "Czujesz się świetnie, jak prawdziwy łowca skarbów.")
S("Your eyes shine like gems.", "Twoje oczy błyszczą jak klejnoty.")
S("Your eyes shine as you glance at your precious treasures.", "Twoje oczy błyszczą, gdy patrzysz na swoje skarby.")
S("You glance at your great treasures.", "Patrzysz na swoje wspaniałe skarby.")
S("You glance at your precious treasures.", "Patrzysz na swoje drogocenne skarby.")
S("You glance at your precious treasure.", "Patrzysz na swój drogocenny skarb.")
S("Your inventory is empty.", "Nie masz żadnych skarbów.")
S("You teleport to a new location!", "Teleportujesz się do nowego miejsca!")
S("Could not open the score file: ", "Nie udało się otworzyć pliku: ")
S("Game statistics saved to %1", "Statystyki gry zapisane do %1")
S("Game loaded.", "Gra odczytana.")
S("You summon some Mimics!", "Przywoła%łeś0 Miraże!")
S("You summon a golem!", "Przywoła%łeś0 Golema!")
S("You will now start your games in %1", "Będziesz zaczynał grę %abl1")
S("Activated the Hyperstone Quest!", "Aktywowałeś Misję Hiperkamień!")
S("Orb power depleted!", "Zabrano sfery mocy!")
S("Orbs summoned!", "Sfery mocy przywołane!")
S("Orb power gained!", "Zdobyta moc!")
S("Dead orbs gained!", "Zdobyte Martwe Sfery!")
S("Orb of Yendor gained!", "Zdobyta Sfera Yendoru!")
S("Treasure gained!", "Skarb zdobyty!")
S("Lots of treasure gained!", "Mnóstwo skarbów zdobyte!")
S("You summon a sandworm!", "Przywoła%łeś0 Czerwia!")
S("You summon an Ivy!", "Przywoła%łeś0 Bluszcz!")
S("You summon a monster!", "Przywoła%łeś0 potwora!")
S("You summon some Thumpers!", "Przywoła%łeś0 dudniki!")
S("You summon a bonfire!", "Przywoła%łeś0 ognisko!")
S("Treasure lost!", "Skarb utracony!")
S("Kills gained!", "Zdobyto trupy!")
S("Activated Orb of Safety!", "Aktywowano Sferę Bezpieczeństwa!")
S("Teleported to %1!", "Przeniesiono w %abl1")

S("Welcome to HyperRogue", "Witaj w HyperRogue")
S(" for Android", " na Android")
S(" for iOS", " na iOS")
S("! (version %1)\n\n", "! (wersja %1)\n\n")
S(" (press ESC for some hints about it).", " (naciśnij ESC by dostać wskazówki).")
S("Press 'c' for credits.", "Naciśnij 'c' by obejrzeć autorów")
S("game design, programming, texts and graphics by Zeno Rogue <zeno@attnam.com>\n\n",
  "projekt, programowanie, teksty i grafika: Zeno Rogue <zeno@attnam.com>\n\n")
S("add credits for your translation here", "Teksty i tłumaczenie: Zeno i Tehora Rogue\n\n")
S(" (touch to activate)", " (dotknij, by aktywować)")
S(" (expired)", " (wyczerpany)")
S(" [%1 turns]", " [kolejek: %1]")
S(", you", ", Ty")
S("0 = Klein model, 1 = Poincaré model", "0 = model Kleina, 1 = model Poincaré")
S("you are looking through it!", "patrzysz przez hiperboloidę!")
S("simply resize the window to change resolution", "po prostu zmień rozmiar okna")
S("[+] keep the window size, [-] use the screen resolution", "[+] zachowaj rozmiar okna, [-] użyj rozdzielczości ekranu")
S("+5 = center instantly, -5 = do not center the map", "+5 = centruj natychmiast, -5 = nie centruj")
S("press Space or Home to center on the PC", "naciśnij Space lub Home, by wycentrować na postaci")
S("You need special glasses to view the game in 3D", "Potrzebujesz specjalnych okularów, by oglądać grę w 3D")
S("You can choose one of the several modes", "Możesz wybrać jeden z kilku trybów")
S("ASCII", "ASCII")
S("black", "czarny")
S("plain", "prosty")
S("Escher", "Escher")
S("items only", "tylko przedmioty")
S("items and monsters", "przedmioty i potwory")
S("no axes", "bez osi")
S("auto", "auto")
S("light", "lekki")
S("heavy", "gruby")
S("The axes help with keyboard movement", "Osie pomagają, gdy gramy na klawiaturze")
S("Config file: %1", "Plik konfiguracyjny: %1")
S("joystick mode: automatic (release the joystick to move)", "tryb dżojstika: automatyczny (puść by ruszyć)")
S("joystick mode: manual (press a button to move)", "tryb dżojstika: ręczny (naciśnij przycisk)")
S("Reduce the framerate limit to conserve CPU energy", "Ogranicz fps, by oszczędzać energię")
S("Press F1 or right click for help", "Wciśnij F1 lub kliknij prawym, by uzyskać pomoc")
S("No info about this...", "Brak informacji o tym...")
S("Press Enter or F10 to save", "Wciśnij Enter lub F10, by zapisać i wyjść")
S("Press Enter or F10 to quit", "Wciśnij Enter lub F10, by wyjść z gry")
S("or 'r' or F5 to restart", "lub 'r' lub F5, by zacząć od początku")
S("or 't' to see the top scores", "lub 't' by zobaczyć najlepsze wyniki")
S("or another key to continue", "lub inny klawisz, by kontynuować")
S("It is a shame to cheat!", "Wstyd oszukiwać!")
S("Showoff mode", "Tryb pokazowy")
S("Quest status", "Stan misji")
S("GAME OVER", "KONIEC GRY")
S("Your score: %1", "Twój wynik: %1")
S("Enemies killed: %1", "Potwory pokonane: %1")
S("Orbs of Yendor found: %1", "Znalezione Sfery Yendoru: %1")
S("Collect 30 $$$ to access more worlds", "Znajdź 30 $$$, by iść do nowych krain")
S("Collect at least 10 treasures in each of 9 types to access Hell", "Znajdź po 10 skarbów w 9 typach, by się dostać do Piekła")
S("Collect at least 10 Demon Daisies to find the Orbs of Yendor", "Znajdź 10 Czarciego Ziela, by znaleźć Sfery Yendoru")
S("Hyperstone Quest: collect at least 10 %1 in %the2", "Misja alternatywna: znajdź co najmniej 10 skarbów %abl2")
S("Hyperstone Quest completed!", "Misja alternatywna zakończona!")
S("Look for the Orbs of Yendor in Hell or in the Crossroads!", "Szukaj Sfer Yendoru w Piekle albo na Skrzyżowaniu!")
S("Unlock the Orb of Yendor!", "Otwórz Sferę Yendoru!")
S("Defeat 100 enemies to access the Graveyard", "Pokonaj 100 przeciwników, by trafić na Cmentarz")
S("(press ESC during the game to review your quest)", "(naciśnij ESC w trakcie gry, by zobaczyć stan swojej misji)")
S("you have cheated %1 times", "liczba oszustw: %1")
S("%1 turns (%2)", "kolejek: %1 (%2)")
S("last messages:", "ostatnie wiadomości: ")
S("time elapsed", "czas")
S("date", "data")
S("treasure collected", "zdobyte skarby")
S("total kills", "łączne zabicia")
S("turn count", "liczba kolejek")
S("cells generated", "wygenerowane pola")
S("t/left/right - change display, up/down - scroll, s - sort by", "t/lewo/prawo - zmiana, góra/dół - przewijanie, s - sortowanie")
S("kills", "zab")
S("time", "czas")
S("ver", "wer")
S("SORT", "SORT")
S("PLAY", "GRA")
S("Your total wealth", "Twoje łączne bogactwo")
S("treasure collected: %1", "zdobyte skarby: %1")
S("objects found: %1", "znalezione przedmioty: %1")
S("orb power: %1", "moc: %1")
S(" (click to drop)", " (naciśnij by porzucić)")
S("You can also scroll to the desired location and then press 't'.", "Możesz też przewinąć do miejsca docelowego i wcisnąć 't'.")
S("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n",
  "Jest więc to użyteczne w bardzo długich grach, które inaczej by zjadły całą pamięć.")
S("You can touch the Dead Orb in your inventory to drop it.", "Dotknij Martwej Sfery na liście, by ją porzucić.")
S("This might be useful for Android devices with limited memory.", "To może się przydać, gdy Twoje urządzenie ma mało pamięci.")
S("You can press 'g' or click them in the list to drop a Dead Orb.", "Wciśnij 'g' lub kliknij Martwe Sfery na liście, by je porzucić.")
S("frames per second", "klatki na sekundę")
S("monsters killed: %1", "pokonane potwory: %1")
S("Drawing %1 (layer %2), F1 for help", "Rysujemy %1 (warstwa %2), F1 - pomoc")
S("hepta floor", "podłoga hepta")
S("hexa floor", "podłoga hex")
S("character", "postać")
S("ESC for menu/quest", "ESC - menu/misja")
S("vector graphics editor", "edytor grafiki wektorowej")
S("cheat mode", "tryb oszusta")
S("heptagonal game board", "plansza do gry z siedmiokątów")
S("triangular game board", "plansza do gry z trójkątów")
S("HyperRogue game board", "plansza do gry HyperRogue")
S("first page [Space]", "pierwsza strona [Space]")
S("exit configuration", "wyjdź z konfiguracji")
S("Configuration:", "Konfiguracja:")
S("video resolution", "rozdzielczość obrazu")
S("fullscreen mode", "tryb pełnoekranowy")
S("animation speed", "prędkość animacji")
S("dist from hyperboloid ctr", "odległość od centrum hiperboloidy")
S("scale factor", "współczynnik skali")
S("wall display mode", "tryb pokazywania ścian")
S("monster display mode", "tryb pokazywania potworów")
S("cross display mode", "tryb pokazywania osi")
S("background music volume", "głośność muzyki tła")
S("OFF", "WYŁ")
S("ON", "WŁ")
S("distance between eyes", "odległość między oczami")
S("framerate limit", "ograniczenie na fps")
S("joystick mode", "tryb dżojstika")
S("automatic", "automatyczny")
S("manual", "ręczny")
S("language", "język")
S("EN", "PL")
S("player character", "postać gracza")
S("male", "mężczyzna")
S("female", "kobieta")
S("use Shift to decrease and Ctrl to fine tune ", "użyj Shift by zmniejszyć, Ctrl by zwiększyć dokładność")
S("(e.g. Shift+Ctrl+Z)", " (na przykład Shift+Ctrl+Z)")
S("the second page [Space]", "druga strona [Space]")
S("special features [Space]", "opcje specjalne [Space]")
S("exit configuration", "wyjdź z konfiguracji")
S("see the help screen", "obejrzyj ekran pomocy")
S("save the current config", "zapisz obecną konfigurację")
S("(v) config", "(v) ust.")
S("Screenshot saved to %1", "Zrzut ekranu zapisany do %1")
S("You need an Orb of Teleport to teleport.", "Potrzebujesz Sfery Teleportacji, by to zrobić.")
S("Use arrow keys to choose the teleport location.", "Użyj strzałek, by wybrać cel teleportacji.")
S("openGL mode enabled", "włączono tryb OpenGL")
S("openGL mode disabled", "wyłączono tryb OpenGL")
S("openGL & antialiasing mode", "openGL i antialiasing")
S("anti-aliasing enabled", "włączono anti-aliasing")
S("anti-aliasing disabled", "wyłączono anti-aliasing")
S("You activate your demonic powers!", "Aktywowałeś moce demoniczne!")

// Steam achievement messages
// --------------------------

S("New Achievement:", "Nowe osiągnięcie:")
S("Your total treasure has been recorded in the Steam Leaderboards.", "Twój wynik został zapisany w rankingach Steam.")
S("Congratulations!", "Gratulacje!")
S("You have improved your total high score and %1 specific high scores!", "Poprawi%łeś0 swój ogólny najlepszy wynik oraz %1 z wyników szczegółowych!")
S("You have improved your total and '%1' high score!", "Poprawi%łeś0 swój wynik ogólny oraz swój wynik w kategorii '%P1!'")
S("You have improved your total high score on Steam. Congratulations!", "Poprawi%łeś0 swój ogólny najlepszy wynik na Steam. Gratulacje!")
S("You have improved %1 of your specific high scores!", "Poprawi%łeś0 %1 z wyników szczegółowych!")
S("You have improved your '%1' high score on Steam!", "Poprawi%łeś0 swój wynik w kategorii '%P1' na Steam!")
S("You have collected 10 treasures of each type.", "Znalaz%łeś0 10 skarbów każdego typu.")
S("This is your first victory!", "To Twoje pierwsze zwycięstwo!")
S("This has been recorded in the Steam Leaderboards.", "To zostało zapisane w Rankingach Steam.")
S("The faster you get here, the better you are!", "Im szybciej to osiągniesz, tym lepiej!")
S("You have improved both your real time and turn count. Congratulations!", "Poprawi%łeś0 swój najlepszy czas rzeczywisty i liczbę ruchów. Gratulacje!")
S("You have used less real time than ever before. Congratulations!", "Zajęło to mniej czasu rzeczywistego niż wcześniej. Gratulacje!")
S("You have used less turns than ever before. Congratulations!", "Zajęło to mniej kolejek niż kiedykolwiek wcześniej. Gratulacje!")

// help texts
// ----------

// These are separated into multiple lines just for convenience,
// you don't have to follow.

S(
 "You have been trapped in a strange, non-Euclidean world. Collect as much treasure as possible "
 "before being caught by monsters. The more treasure you collect, the more "
 "monsters come to hunt you, as long as you are in the same land type. The "
 "Orbs of Yendor are the ultimate treasure; get at least one of them to win the game!",

 "Trafi%łeś0 do dziwnej, nieeuklidesowej krainy. Zdobądź jak najwięcej skarbów, "
 "zanim potwory Cię dopadną. Im więcej zdobędziesz skarbów, tym więcej potworów "
 "w danej krainie na Ciebie poluje. Sfery Yendoru są skarbem ostatecznym: zdobądź "
 "Sferę Yendoru, by wygrać grę!"
 )

S(
 "You can fight most monsters by moving into their location. "
 "The monster could also kill you by moving into your location, but the game "
 "automatically cancels all moves which result in that.\n\n",

 "Większość przeciwników zabijasz poprzez wejście na zajmowane przez nie pole. "
 "Przeciwnik może Cię zabić w ten sam sposób, ale gra automatycznie zabrania "
 "ruchów, które by do tego prowadziły.\n\n")

S(
 "Usually, you move by touching somewhere on the map; you can also touch one "
 "of the four buttons on the map corners to change this (to scroll the map "
 "or get information about map objects). You can also touch the "
 "numbers displayed to get their meanings.\n",
 "Zazwyczaj poruszasz się przez dotknięcie pola na mapie; możesz również "
 "dotknąć jednego z przycisków w rogach, by to zmienić (by przewijać mapę albo "
 "zdobyć informacje). Możesz też dotknąć liczb na ekranie, by poznać ich "
 "znaczenie.\n")

S("Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Home/Space. "
 "To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n",
 "Ruszasz się myszą, klawiaturą numeryczną, qweadzxc, lub hjklyubn. Czekasz naciskając 's' lub '.'. "
 "Obracasz świat strzałkami, PgUp/Dn, lub Home/Space. Naciśnij 'v' by przejść do menu (konfiguracja, tryby specjalne itd.), ESC "
 "by zobaczyć stan misji.\n\n")

S("See more on the website: ", "Więcej na stronie: ")

S("special thanks to the following people for their bug reports, feature requests, porting, and other help:\n\n%1\n\n",
  "Szczególne podziękowania dla poniższych osób za zgłoszone przez nich błędy, pomysły, porty i inną pomoc:\n\n%1\n\n")

S(
 "The total value of the treasure you have collected.\n\n"
 "Every world type contains a specific type of treasure, worth 1 $$$; "
 "your goal is to collect as much treasure as possible, but every treasure you find "
 "causes more enemies to hunt you in its native land.\n\n"
 "Orbs of Yendor are worth 50 $$$ each.\n\n",

  "Łączna wartość Twoich skarbów.\n\n"
  "Każda kraina zawiera specyficzny dla niej skarb, wart 1 $$$; "
  "Twoim celem jest zebranie jak najwięcej skarbów, ale każdy "
  "znaleziony skarb powoduje, że w jego krainie więcej potworów na Ciebie poluje.\n\n"
  "Sfery Yendoru są warte po 50 $$$.\n\n")

S(
 "The higher the number, the smoother the animations in the game. "
 "If you find that animations are not smooth enough, you can try "
 "to change the options ",
 "Im większa liczba, tym płynniejsze są animacje. Jeśli animacja nie "
 "jest dostatecznie płynna, spróbuj zmienić opcje")

S("(Menu button) and select the ASCII mode, which runs much faster. "
 "Depending on your device, turning the OpenGL mode on or off might "
 "make it faster, slower, or cause glitches.",
 "(Menu), tryb ASCII działa dużo szybciej. Tryb OpenGL również wpływa "
 "na jakość grafiki."
 )

S("(in the MENU). You can reduce the sight range, this should make "
 "the animations smoother.",
 "(w MENU). Możesz zmniejszyć zasięg widzenia, dzięki temu animacje "
 "będą płynniejsze.")

S("(press v) and change the wall/monster mode to ASCII, or change "
 "the resolution.",
 "(naciśnij 'v') i zmień tryb ścian/potworów na ASCII albo zmień "
 "rozdzielczość.")

S(
 "In this mode you can draw your own player character and floors. "
 "Mostly for the development purposes, but you can have fun too.\n\n"
 "f - floor, p - player (repeat 'p' for layers)\n\n"
 "n - new shape, u - copy the 'player body' shape\n\n"
 "1-9 - rotational symmetries, 0 - toggle axial symmetry\n\n"
 "point with mouse and: a - add point, m - move nearest point, d - delete nearest point, c - nearest point again, b - add after nearest\n\n"
 "s - save in C++ format (but cannot be loaded yet without editing source)\n\n"
 "z - zoom, o - Poincaré model\n",

 "W tym trybie możesz narysować swoją własną postać albo podłogę. "
 "Głównie w celu rozwijania gry, ale Ty też możesz się pobawić.\n\n"
 "f - podłoga, p - gracz (powtarzaj, by zmienić warstwę)\n\n"
 "n - nowy kształt, u - skopiuj standardową postać gracza\n\n"
 "1-9 - symetrie obrotowe, 0 - symetria osiowa\n\n"
 "wskaż myszą i: a - dodaj punkt, m - przesuń najbliższy punkt, d - skasuj najbliższy punkt, c - powtórz najbliższy punkt, b - po najbliższym\n\n"
 "s - zapisz w formacie C++ (niestety obecnie nie można wczytać bez edytowania źródeł gry)\n\n"
 "z - powiększenie, o - model Poincaré\n")

S(
 "These huge monsters normally live below the sand, but your movements have "
 "disturbed them. They are too big to be slain with your "
 "weapons, but you can defeat them by making them unable to move. "
 "This also produces some Spice. They move two times slower than you.",

 "Te wielkie stworzenia normalnie żyją pod pustynnym piaskiem, ale Twoja obecność "
 "na pustyni spowodowała ich wyjście na powierzchnię. Są za duże, byś m%ógł0 je "
 "zaatakować swoją bronią, ale zginą, jeśli nie będą mogły się ruszyć -- to "
 "produkuje także trochę Przyprawy. Czerwie ruszają się 2 razy wolniej od Ciebie.")

S("The tentacles of Cthulhu are like sandworms, but longer. "
 "They also withdraw one cell at a time, instead of exploding instantly.",

 "Macki Cthulhu są podobne do Pustynnych Czerwi, ale dłuższe. "
 "W przeciwieństwie do nich nie eksplodują po zablokowaniu, tylko cofają się "
 "pod ziemię, po 1 polu na kolejkę.")

S(
 "A huge plant growing in the Jungle. Each Ivy has many branches, "
 "and one branch grows per each of your moves. Branches grow in a clockwise "
 "order. The root itself is vulnerable.",
 "Wielka roślina z dżungli. Bluszcz zazwyczaj ma wiele gałęzi, po każdym Twoim "
 "ruchu jedna z nich rośnie. Gałęzie rosną w kolejnosci zegarowej. Sam korzeń "
 "nie atakuje, zatem goły korzeń łatwo ściąć.\n")

S("The Alchemists produce magical potions from pools of blue and red slime. You "
 "can go through these pools, but you cannot move from a blue pool to a red "
 "pool, or vice versa. Pools containing items count as colorless, and "
 "they change color to the PC's previous color when the item is picked up. "
 "Slime beasts also have to keep to their own color, "
 "but when they are killed, they explode, destroying items and changing "
 "the color of the slime and slime beasts around them.",
 "Alchemicy produkują magiczne napoje z niebieskiej i czerwonej mazi. Możesz "
 "poruszać się poprzez maź, ale nie możesz przesunąć się z pola niebieskiego "
 "na czerwone, ani z powrotem. Pola zawierające przedmioty są bezbarwne, "
 "po zebraniu przedmiotu zmieniają kolor na kolor pola, na którym "
 "gracz był wcześniej. Maziste Stwory również ograniczone są do swojego koloru. "
 "Zabijane eksplodują, niszcząc przedmioty i zmieniając kolor mazi wokół nich.\n")

S(
 "These creatures are slow, but very powerful... more powerful than you. "
 "You need some more experience in demon fighting before you will be able to defeat them. "
 "Even then, you will be able to slay this one, but more powerful demons will come...\n\n"
 "Each 10 lesser demons you kill, you become powerful enough to kill all the greater "
 "demons on the screen, effectively turning them into lesser demons.",

 "Te demony są powolne, ale bardzo silne... silniejsze od Ciebie. "
 "Potrzebujesz zdobyć trochę doświadczenia, zanim będziesz w stanie je pokonać. "
 "Nawet wtedy przyjdą jeszcze silniejsze demony...\n\n"
 "Za każdym razem, gdy pokonasz 10 mniejszych demonów, stajesz się dostatecznie silny, "
 "by pokonać wszystkie Wielkie Demony na ekranie. Stają się one w tym momencie "
 "Mniejszymi Demonami.")

S("These creatures are slow, but they often appear in large numbers.",
  "Te demony są powolne, ale często pojawiają się w dużych grupach.")

S("A big monster from the Living Caves. A dead Troll will be reunited "
 "with the rocks, causing some walls to grow around its body.",
 "Duży stwór z Żyjących Jaskiń. Martwy Troll połączy się ze skałą, "
 "powodując rozrost skał wokół jego ciała.")

S("Huge, impassable walls which separate various lands.",
  "Wielkie ściany, które oddzielają od siebie poszczególne krainy.")

S(
 "This cave contains walls which are somehow living. After each turn, each cell "
 "counts the number of living wall and living floor cells around it, and if it is "
 "currently of a different type than the majority of cells around it, it switches. "
 "Items count as three floor cells, and dead Trolls count as five wall cells. "
 "Some foreign monsters also count as floor or wall cells.\n",
 "Ściany tej jaskini sa żywe. W każdej kolejce każde pole liczy, ile wokół niego "
 "jest pól ściany i pól ziemi i jeśli jest innego typu niż większość pól wokół "
 "niego, to zmienia typ na przeciwny. Przedmioty liczą się jako 3 pola ziemi, "
 "martwe Trolle jako 5 pól ściany. Niektóre potwory z innych krain również liczą się "
 "jako ziemia lub ściana.\n")

S(
 "This forest is quite dry. Beware the bushfires!\n"
 "Trees catch fire on the next turn. The temperature of the grass cells "
 "rises once per turn for each fire nearby, and becomes fire itself "
 "when its temperature has risen 10 times.\n"
 "You can also chop down the trees. Big trees take two turns to chop down.",
 "Ta puszcza jest wyschnięta. Uważaj na pożary!\n"
 "Sąsiednie drzewa zajmują się ogniem w następnej kolejce. Temperatura "
 "pola bez drzewa rośnie o 1 na kolejkę z każdym ogniem w sąsiedztwie i gdy "
 "wzrośnie do 10, to pole również staje się ogniem.\n"
 "Możesz też ścinać drzewa. Ścięcie dużego drzewa zajmuje dwie kolejki."
 )

S("A big and quite intelligent monster living in the Icy Land.",
  "Duża i całkiem inteligentna bestia z Krainy Lodu.")

S(
 "A nasty predator from the Icy Land. Contrary to other monsters, "
 "it tracks its prey by their heat.",
 "Niebezpieczny drapieżnik z Krainy Lodu. W przeciwieństwie do innych "
 "stworów, śledzi ofiarę po jej cieple.")

S("Rangers take care of the magic mirrors in the Land of Mirrors. "
 "They know that rogues like to break these mirrors... so "
 "they will attack you!",
 "Strażnicy chronią lustra w Krainie Luster. Wiedzą, że złodzieje lubią "
 "rozbijać lustra... także spodziewaj się ataku!")

S("A nasty creature native to the Living Caves. They don't like you "
 "for some reason.",
 "Brzydki stwór z Żywych Jaskiń. Jakoś Cię nie lubi.")

S("A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them.",
  "Plemię ludzi żyjących na Pustyni. Oswoili oni Pustynne Czerwie, dzięki czemu żyją razem pokojowo.")

S("This giant ape thinks that you are an enemy.", "Ta małpa uważa, że jesteś jej przeciwnikiem.")

S("A magical being which copies your movements.", "Magiczna istota kopiująca Twoje ruchy.")

S("A magical being which copies your movements. "
 "You feel that it would be much more useful in an Euclidean space.",
 "Magiczna istota kopiująca Twoje ruchy. W przestrzeni euklidesowej byłaby dużo bardziej przydatna.")

S("You can summon these friendly constructs with a magical process.",
  "Przyjazna konstrukcja, tworzona poprzez magiczny proces.")

S("A majestic bird, who is able to fly very fast.",
  "Ten majestatyczny ptak bardzo szybko lata.")

S("A monster who is able to live inside the living cave wall.",
  "Sączaki żyją w ściane Żywej Jaskini.")

S("A typical Graveyard monster.", "Typowy cmentarny stwór.")

S("A typical monster from the Graveyard, who moves through walls.\n\n"
  "There are also wandering Ghosts. They will appear "
  "if you do not explore any new places for a long time (about 100 turns). "
  "They can appear anywhere in the game.",
  "Typowy cmentarny stwór. Może przechodzić przez ściany.\n\n"
  "Są też błądzące Duchy, które pojawiają się, gdy nie zwiedzasz "
  "nowych miejsc przez długi czas (około 100 kolejek). Duchy te "
  "mogą pojawić się w dowolnym miejscu w grze."
  )

S("Necromancers can raise ghosts and zombies from fresh graves.",
  "Nekromanci wzbudzają duchy i zombie ze świeżych grobów.")

S("A creepy monster who follows you everywhere in the Graveyard.",
  "Ten odrażający potwór chodzi za Tobą po cmentarzu!")

S("People worshipping Cthulhu. They are very dangerous.",
  "Wyznawcy Cthulhu, bardzo niebiezpieczni.")

S("People worshipping Cthulhu. This one is especially dangerous, "
 "as he is armed with a weapon which launches fire from afar.",
 "Wyznawcy Cthulhu. Ten jest szczególnie niebezpieczny, bo "
 "może z odległości rozpalić ogień w Twojej okolicy.")

S("This dangerous predator has killed many people, and has been sent to Cocytus.",
  "Ten niebezpieczny drapieżnik zabił wielu ludzi i został zesłany do Kocytu.")

S("This white dog is able to run all the time. It is the only creature "
 "able to survive and breed in the Land of Eternal Motion.",
 "Ten biały piesek jest w stanie biegać przez całe swoje życie. Jest to jedyne "
 "stworzenie, które jest w stanie przeżyć w Krainie Wiecznego Ruchu.")

S("Demons of Hell do not drown when they fall into the lake in Cocytus. "
 "They turn into demonic sharks, enveloped in a cloud of steam.",
 "Demony nie topią się, gdy wpadną w jezioro Kocyt. Zamieniają się "
 "w demoniczne rekiny, otoczone chmurą pary.")

S("These fairies would rather burn the forest, than let you get some Fern Flowers. "
 "The forest is infinite, after all...\n\n"
 "Fire Fairies transform into fires when they die.",
 "Wiły wolą spalić las, niż pozwolić Ci zdobyć Kwiaty Paproci. W końcu "
 "las jest nieskończony...\n\n"
 "Wiły zamieniają się w ogień, gdy giną.")

S("These warriors of the Forest wield exotic weapons called hedgehog blades. "
 "These blades protect them from a frontal attack, but they still can be 'stabbed' "
 "easily by moving from one place next to them to another.",
 "Ci wojownicy z Puszczy walczą egzotyczną bronią, Jeżowym Ostrzem. "
 "Ostrza te bronią przed atakiem z frontu, ale za to możesz ich łatwo 'dźgnąć' "
 "poprzez przejście z jednego pola sąsiadującego z nimi na inne.")

S("This being radiates an aura of wisdom. "
 "It is made of a beautiful crystal, you would love to take it home. "
 "But how is it going to defend itself? Better not to think of it, "
 "thinking causes your brain to go hot...\n\n"
 "Crystal Sages melt at -30 °C, and they can rise the temperature around you from afar.",
 "Ta istota promieniuje mądrością. Jest zrobiona z pięknego kryształu, na pewno "
 "bardzo cennego. Ciekawe, jak zamierza się przed Tobą bronić? Lepiej o tym nie myśleć, "
 "myślenie za bardzo Cię rozgrzewa...\n\n"
 "Kryształowi Mędrcy topią się w -30 °C i powodują wzrost temperatury wokół Ciebie.")

S("Cold white gems, found in the Icy Land.", "Zimne białe kamienie z Krainy Lodu.")

S("An expensive metal from the Living Caves. For some reason "
 "gold prevents the living walls from growing close to it.",
 "Drogocenny metal z Żyjących Jaskiń. Złoto utrudnia wzrost ścian wokół niego.")

S("A rare and expensive substance found in the Desert. "
 "It is believed to extend life and raise special psychic powers.",
 "Rzadka i droga substancja z Pustyni. Podobno wydłuża życie i daje moce psychiczne.")

S("A beautiful gem from the Jungle.", "Piękny klejnot z Dżungli.")

S(
 "A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
 "healthy after drinking it, but you still feel that one hit of a monster is enough to kill you.",
 "Cudowny napój, powstały z mieszania czerwonej i niebieskiej mazi. Po jego wypiciu czujesz "
 "się zdecydowanie lepiej, ale wciąż jedno uderzenie potwora może Cię powalić.")

S("A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
 "in the Land of Mirrors leave these.",
 "Odłamek szkła z magicznego lustra albo fragment mirażowej chmury. Może zostać użyty do magii. "
 "Tylko lustra i chmury z Krainy Luster zostawiają odłamki.")

S("These sinister totems contain valuable gems.",
  "Te złowieszcze totemy zawierają cenne klejnoty.")

S("These star-shaped flowers native to Hell are a valuable alchemical component.",
  "Te piekielne rośliny w kształcie gwiazdy są cennym składnikiem alchemicznym.")

S("This statue is made of materials which cannot be found in your world.",
  "Ta statua jest zrobiona z materiałów, których nie ma w Twoim świecie.")

S("One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable.",
  "Pióro Feniksa jest tak lekie, że podłoga z Krainy Wiecznego Ruchu pod nim się nie zapada. "
  "Takie pióra muszą być bardzo cenne.")

S("Cold blue gems, found in the Cocytus.", "Zimne niebieskie kamienie, znajdowane na zamarzniętym Kocycie.")

S("These bright yellow gems can be found only by those who have mastered the Crossroads.",
  "Te jasne, żółte klejnoty mogą znaleźć tylko mistrze Skrzyżowań.")

S("That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
 "Each key unlocks only the Orb of Yendor which led you to it.",
 "To jest to, czego potrzebujesz, by otworzyć Sferę Yendoru! O ile umiesz do niej trafić...\n"
 "Każdy klucz otwiera tylko jedną Sferę, która Cię do niego doprowadziła.")

S("These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
 "power is long gone. No way to use them, you could as well simply drop them...\n\n",
 "Na cmentarzu jest mnóstwo tych sfer. Chyba były to kiedyś potężne magiczne kule, ale ich "
 "moc dawno przeminęła. Nie ma jak ich użyć, także może lepiej je po prostu zostawić...\n\n")

S(
 "This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
 "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
 "where the key is after you touch the Orb.",
 "Ta cudowna sfera może być zdobyta tylko przez prawdziwych mistrzów tego świata. "
 "By ją otworzyć, potrzebujesz klucza. Na szczęście Twoje moce psychiczne powiedzą Ci, "
 "gdzie jest klucz, gdy dotkniesz Sfery.")

S(
  "This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions.",
  "Ta sfera pozwala rzucić czar Błyskawica, który powoduje wystrzelenie błyskawicy w każdym kierunku.")

S("This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2.",
  "Ta sfera pozwala rzucić czar Błysk, który niszczy wszystko w promieniu 2.")

S("This orb can be used to invoke a wall of ice. It also protects you from fires.",
  "Ta sfera zostawia za Tobą ścianę lodu, a także chroni Cię przed ogniem.")

S("This orb can be used to move faster for some time.",
  "Ta sfera powoduje, że przez pewien czas poruszasz się szybciej.")

S("This orb can be used to summon friendly golems. It is used instantly when you pick it up.",
  "Ta sfera przywołuje przyjazne golemy. Jest natychmiast używana w momencie podniesienia.")

S("This orb can protect you from damage.", "Ta sfera chroni Cię przed obrażeniami od potworów.")

S("This orb lets you instantly move to another location on the map. Just click a location which "
 "is not next to you to teleport there. ",
   "Ta sfera pozwala Ci natychmiast przenieść się do innego miejsca na mapie. Wystarczy "
   "kliknąć pole, które nie jest obok Ciebie.")

S("This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
 "that you will ever find the way back...\n\n"
 "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
 "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). ",
 "Ta sfera pozwala Ci natychmiast przenieść się do bezpiecznego miejsca. Znając naturę tego "
 "świata, zapewne nigdy nie trafisz z powrotem...\n\n"
 "Jeśli wyjdziesz z gry podczas gdy Sfera Bezpieczeństwa wciąż ma moc, gra zostanie zapisana.\n\n"
 "Uwaga techniczna: nawet jeśli nie zapiszesz gry, ta sfera czyści całą pamięć o świecie gry. ")

S("This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n",
  "Ta sfera pozwala bezpośrednio atakować Wojowników-Jeże, a także dźgać pozostałych przeciwników.\n")

S("This flower brings fortune to the person who finds it.\n",
  "Te rośliny dają szczęście temu, kto je znajdzie.\n")

S("Ice Walls melt after some time has passed.", "Lodowe ściany topią się po pewnym czasie.")

S("A natural terrain feature of the Desert.", "Naturalny teren z Pustyni.")

S("You can go inside the Magic Mirror, and produce some mirror images to help you.",
  "Możesz wejść w Magiczne Lustro, by Twoje odbicia z niego wyszły i Ci pomogły.")

S(
 "Tiny droplets of magical water. You see images of yourself inside them. "
 "Go inside the cloud, to make these images help you.",
 "Malutkie kropelki magicznej wody, w których widać Twój obraz. Wejdź do środka chmury, "
 "by te obrazy Ci pomogły.")

S("A device that attracts sandworms and other enemies. You need to activate it.",
  "To urządzenie przyciąga czerwie i innych przeciwników. Musisz je aktywować.")

S("A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it.",
  "Stos drewna. Wszystko gotowe, wystarczy podpalić.")

S("An ancient grave.", "Stary grób.")

S("A fresh grave. Necromancers like those.", "Świeży grób. Nekromanci to lubią.")

S("A piece of architecture typical to R'Lyeh.", "Architektura typowa dla R'Lyeh.")

S("An impassable lake in Cocytus.", "Nie możesz przejść przez Jezioro Kocyt.")

S("You can walk on it... but beware.", "Możesz tu przejść, ale uważaj!")

S("It was a floor... until something walked on it.", "Tu kiedyś była podłoga, ale coś po niej przeszło.")

S(
 "This land is a quick gateway to other lands. It is very easy to find other lands "
 "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
 "As long as you have found enough treasure in their native lands, you can "
 "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
 "and other land types bring magical orbs.\n\n"
 "A special treasure, Hyperstone, can be found on the Crossroads, but only "
 "after you have found 10 of every other treasure.",

 "Ta kraina jest szybkim przejściem do pozostałych krain.\n\n"
 "Bardzo łatwo stąd dostać się do większości miejsc, ale można też tu spotkać "
 "potwory z różnych stron!\n\n"
 "Możesz znaleźć magiczne przedmioty na Skrzyżowaniu, jeśli zdoby%łeś0 wystarczająco "
 "wiele skarbów w ich rodzinnych krainach. Są to sfery, magiczne lustra i chmury.\n\n"
 "Specyficzne skarby Skrzyżowań, Hiperkamienie, można znaleźć na Skrzyżowaniu, ale tylko "
 "po znalezieniu 10 jednostek każdego innego skarbu.")

S("A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms.",
  "Gorąca ziemia, pełna wydm, tajemniczej Przyprawy i wielkich Pustynnych Czerwi.")

S(
 "A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
 "melt, even if you don't want it.",
 "Bardzo zimna kraina, pełna lodowych ścian. Wystarczy Twoja obecność, by te ściany "
 "się roztopiły. Nawet, jak tego nie chcesz.")

S("A land filled with huge ivy plants and dangerous animals.",
  "Kraina pełna wielkiego pnącego bluszczu i niebezpiecznych bestii.")

S("A strange land which contains mirrors and mirages, protected by Mirror Rangers.",
  "Dziwna kraina, gdzie można znaleźć magiczne lustra i chmury, bronione przez "
  "Strażników Luster.")

S("All the monsters you kill are carried to this strange land, and buried. "
 "Careless Rogues are also carried here...",
 "Pochowane są tu wszystkie potwory, które zabijasz. A także poszukiwacze skarbów, którzy "
 "nie uważali...")

S("An ancient sunken city which can be reached only when the stars are right.\n\n"
  "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "Prastare zatopione miasto, do którego można dostać się tylko, gdy gwiazdy są "
  "na swoich miejscach.\n\n"
  "Po znalezieniu 5 Statuetek Cthulhu możesz w R'Lyeh trafić na Świątynie Cthulhu.")

S("A land filled with demons and molten sulphur. Abandon all hope ye who enter here!",
  "Kraina pełna demonów i stopionej siarki. Ty, który wchodzisz, żegnaj się z nadzieją!")

S("This frozen lake is a hellish version of the Icy Land. Now, your body heat melts the floor, not the walls.",
  "Zamarznięte jezioro. Piekielna wersja Krainy Lodu, w której ciepło Twojego ciała topi ziemię pod Tobą, a nie "
  "ściany.")

S("A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
 "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
 "the floor.\n",
 "Kraina, w której nie możesz przestać się ruszać, bo grunt jest wszędzie bardzo niestabilny. "
 "Jedynie stworzenia będące w stanie wiecznie się ruszać mogą tu przeżyć i jedynie pióra feniksa "
 "są na tyle lekkie, by nie zaburzyć podłogi.")

S("Affects looks and grammar", "Wpływa na wygląd i gramatykę")

S("first joystick: movement threshold", "dżojstik 1: próg ruchu")
S("first joystick: execute movement threshold", "dżojstik 1: próg wykonania ruchu")
S("second joystick: pan threshold", "dżojstik 2: próg przewijania")
S("second joystick: panning speed", "dżojstik 2: prędkość przewijania")
S("%The1 is frozen!", "%1 zamarzł%oa1!")
S("%The1 burns!", "%1 się spalił%oa1!")
S("message flash time", "czas pokazywania wiadomości")

S("skin color", "kolor skóry")
S("weapon color", "kolor broni")
S("hair color", "kolor włosów")
S("dress color", "kolor sukienki")
S("Shift=random, Ctrl=mix", "Shift=losowo, Ctrl=miks")

S("Euclidean mode", "tryb euklidesowy")
S("Return to the hyperbolic world", "powrót na hiperboloidę")
S("Choose from the lands visited this game.", "Wybierz spośród odwiedzonych krain.")
S("Scores and achievements are not", "Wyniki i osiągnięcia nie są");
S("saved in the Euclidean mode!", "zapamiętywane w tym trybie!");

// Android buttons (some are not translated because there are no good short words in Polish)
S("MOVE", "RUCH")
S("BACK", "OK")
S("DRAG", "DRAG")
S("INFO", "INFO")
S("MENU", "MENU")
S("QUEST", "MISJA")
S("HELP", "POMOC")
S("NEW", "NOWA")
S("PLAY", "GRAJ")
S("SHARE", "SHARE")
S("HyperRogue for Android", "HyperRogue dla Android")
S("Date: %1 time: %2 s ", "Data: %1 czas: %2 s ")
S("distance: %1\n", "odległość: %1\n")
S("Cheats: ", "Oszustwa: ")
S("Score: ", "Wynik: ")
S("Kills: ", "Zabicia: ")
S("Retrieving scores from Google Leaderboards...", "Ściągam wyniki z rankingów Google...")
S("Scores retrieved.", "Wyniki ściągnięte.")

S("Your total treasure has been recorded in the Google Leaderboards.", "Twój wynik został zapisany w rankingach Google.")
S("You have improved your total high score on Google. Congratulations!", "Poprawi%łeś0 swój ogólny najlepszy wynik na Google. Gratulacje!")
S("You have improved your '%1' high score on Google!", "Poprawi%łeś0 swój wynik w kategorii '%P1' na Google!")
S("This has been recorded in the Google Leaderboards.", "To zostało zapisane w Rankingach Google.")

// this text changed a bit:

S("Ever wondered how some boardgame would look on the hyperbolic plane? "
 "I wondered about Go, so I have created this feature. Now you can try yourself!\n"
 "Enter = pick up an item (and score), space = clear an item\n"
 "Other keys place orbs and terrain features of various kinds\n"
 "In the periodic editor, press 0-4 to switch walls in different ways\n",
 "Zastanawia%łeś0 się może, jak jakaś gra planszowa wyglądałaby na płaszczyźnie "
 "hiperbolicznej? Ja się zastanawiałem nad Go i postanowiłem to sprawdzić. Teraz "
 "Ty też możesz sprawdzić!\n"
 "Enter = podnieś przedmiot, space = zabierz przedmiot\n"
 "Pozostałe klawisze kładą różnego rodzaju przedmioty\n"
 "W edytorze okresowym, klawisze 0-4 zmieniają ściany na różne sposoby")

S("Periodic Editor", "Edytor okresowy")
// also translate this line:
// "In the periodic editor, press 0-4 to switch walls in different ways\n",

S("Collect 60 $$$ to access even more lands", "Znajdź 60 $$$ by iść do kolejnych krain")

// Emerald Mine
// ------------

N("Emerald Mine", GEN_F, "Kopalnia Szmaragdów", "Kopalnia Szmaragdów", "Kopalnię Szmaragdów", "w Kopalni Szmaragdów")
N("Pikeman", GEN_M, "Pikinier", "Pikinierzy", "Pikiniera", "Pikinierem")
N("Flail Guard", GEN_M, "Strażnik z cepem", "Strażnicy z cepami", "Strażnika z cepem", "Strażnikiem z cepem")
N("Miner", GEN_M, "Górnik", "Górnicy", "Górnika", "Górnikiem")
N("Emerald", GEN_O, "Szmaragd", "Szmaragdy", "Szmaragd", "Szmaragdem")

Orb("the Mind", "Psychiki")

S(
     "Evil people are mining for emeralds in this living cave. "
     "It does not grow naturally, but it is dug out in a regular "
     "pattern, which is optimal according to the evil engineers.",
     "Źli ludzie szukają szmaragdów w tej żywej jaskini. "
     "Nie rośnie ona naturalnie -- ludzie wykopują regularne "
     "wzory, które, zgodnie z oceną złych inżynierów, są 'optymalne'. ")

S(
    "Miners have special tools for dealing with the Living Cave. "
    "When they die, these tools activate, destroying the living cave "
    "around them.",
    "Górnicy mają specjalne narzędzia do kopania w Żywych Jaskiniach. "
    "Kiedy giną, te narzędzia się uruchamiają, powodując, że żywa "
    "jaskinia wokół nich obumiera.")

S(
    "When Pikemen move, they attack all cells which are now adjacent to them. "
    "Luckily, they can be killed in the same way.\n\n"
    "They never move if this would attack their friends.",
    "Kiedy Pikinier się rusza, atakuje wszystkie komórki, z którymi sąsiaduje "
    "po wykonaniu ruchu. Na szczęście, jego samego można pokonać taką samą "
    "metodą.\n\n"
    "Pikinier się nie rusza, jeśli by w ten sposób zaatakował przyjaciela.")

S(
    "This guard of the Emerald Mine is wielding a huge flail. "
    "You cannot attack him directly, as the flail would still hit you then. "
    "Luckily, you have learned a trick: if you step away from him, "
    "he will hit himself with the flail!",

    "Ten strażnik Kopalni Szmaragdów jest uzbrojony w wielki cep bojowy. "
    "Nie możesz go zaatakować bezpośrednio, bo cep by Cię uderzył. "
    "Na szczęście, znasz sztuczkę: jak się od niego odsuniesz, "
    "strażnik uderzy cepem samego siebie!")

S("A precious green gem from the Emerald Mines.", "Cenny, zielony klejnot z Kopalni Szmaragdów.")

S("%The1 pierces %the2!", "%1 przebi%ł1 %a2!")

S("Make him hit himself by walking away from him.",
  "Odejdź od niego, by sam się zaatakował.")

S("This orb allows you to instantly kill a non-adjacent enemy by clicking it. "
    "Each use drains 30 charges.",
  "Ta sfera pozwala natychmiastowo zabić niesąsiadującego przeciwnika (klikając go). "
  "Każde użycie zmniejsza poziom naładowania o 30 jednostek.")

S("%The1 is immune to mental blasts!", "%1 jest odporn%ya1 na atak psychiczny!")
S("You kill %the1 with a mental blast!", "Zabi%łeś0 %a1 mocą psychiczną!")

// Vineyard
// --------

N("Vineyard", GEN_F, "Winnica", "Winnica", "Winnicę", "w Winnicy")
N("Vine Beast", GEN_M, "Winny Stwór", "Winne Stwory", "Winnego Stwora", "Winnym Stworem")
N("Vine Spirit", GEN_M, "Winny Duszek", "Winne Duszki", "Winnego Duszka", "Winnym Duszkiem")
N("vine", GEN_F, "winorośl", "winorośle", "winorośl", "winoroślą")
N("partial fire", GEN_M, "częściowy ogień", "częściowe ognie", "częściowy ogień", "częściowym ogniem")
N("Wine", GEN_N, "Wino", "Wina", "Wino", "Winem")

S(  "This cell is partially on fire.", "To pole częściowo się pali.")

S(
  "The Vineyard is filled with vines. A very dense pattern of straight lines here...\n\n"
  "Vine Beasts and Vine Spirits change vine cells to grass, and vice versa.",
  "Winnica jest wypełniona winoroślami. Bardzo gęsta siatka linii prostych...\n\n"
  "Winne Bestie i Duszki zmieniają winorośle w trawę i z powrotem.")

S("A vine is growing here... but only on a half of the cell. How is that even possible?!"
  "Most monsters cannot move from this cell to the cell containing the other half. "
  "Vine spirits can move only to the adjacent cells which are also adjacent to the "
  "other half.",
  "Tu rośnie winorośl... ale tylko na połowie pola. Jak to w ogóle jest możliwe?!"
  "Większość stworzeń nie może ruszyć się z tego pola na pole zawierające drugą połowę "
  "winorośli. Winne duszki mogą się ruszyć tylko na te sąsiednie pola, które sąsiadują "
  "także z drugą połową.")

S(
    "A beast made of vines!\n\n"
    "Vine Beasts turn into vines when they die.",
    "Stworzenie zrobione z winorośli!\n\n"
    "Winne Stwory zmieniają się w winorośle, gdy giną.")

S(  "A spirit living in the vines!\n\n"
    "Vine Spirits destroy the vines when they die.",
    "Duch żyjący w winoroślach!\n\n"
    "Winne Duszki niszczą winorośle, gdy giną.")

S("Wine grown under hyperbolic sun would be extremely prized in your home location.",
  "Wino uprawiane pod hiperbolicznym słońcem, byłoby bardzo cenne w Twoim "
  "świecie.")

S("This orb allows one to pass through all kinds of walls and chasms.",
  "Ta sfera pozwala przechodzić przez wszelkie ściany i przepaście.")

S("You cannot attack through the Vine!",
  "Nie możesz atakować poprzez winorośl!")

Orb("Aether", "Eteru")

// Dead Caves
// ----------

N("Dead Cave", GEN_F, "Martwa Jaskinia", "Martwa Jaskinia", "Martwą Jaskinię", "w Martwej Jaskini")
N("Dark Troll", GEN_M, "Mroczny Troll", "Mroczne Trolle", "Mrocznego Trolla", "Mrocznym Trollem")
N("Earth Elemental", GEN_M, "Żywiołak", "Żywiołaki", "Żywiołaka", "Żywiołakiem")
N("dead wall", GEN_F, "martwa ściana", "martwe ściany", "martwą ścianę", "martwą ścianą")
N("dead floor", GEN_F, "martwa ziemia", "martwe ziemie", "martwą ziemię", "martwą ziemią")
N("rubble", GEN_O, "gruz", "gruzy", "gruz", "gruzem")
N("Silver", GEN_N, "Srebro", "Srebra", "Srebro", "Srebrem")

S("A precious metal from the Dead Caves.", "Drogocenny metal z Martwej Jaskini.")

S(  "A Troll without the power of Life.", "Troll bez mocy Życia.")

S(
    "A rare unliving construct from the Dead Caves. "
    "It instantly destroys cave walls next to its path, and also leaves "
    "an impassable wall behind it. You suppose that this impassable wall helps it to "
    "escape from some threats. You hope you won't meet these threats...",

    "Rzadka konstrukcja z Martwych Jaskiń. "
    "Natychmiast niszczy skalne ściany przy jego trasie i tworzy "
    "za sobą ścianę skały. Przypuszczasz, że ta ściana pomaga mu "
    "uciekać przed jakimiś zagrożeniami. Masz nadzieję, że nie spotkasz tych "
    "zagrożeń...")

S("%The1 punches %the2!", "%1 uderzy%ł1 %a2!")
S("%The1 is destroyed by the forces of Life!", "Siły Życia zniszczyły %a1!")
S(  "Somehow, this cave has not received the spark of Life yet.",
    "Dziwnym trafem, ta jaskinia jeszcze nie zdobyła iskry Życia.")

// Hive
// ----

N("Hive", GEN_N, "Mrowisko", "Mrowisko", "Mrowisko", "w Mrowisku")
N("Red Hyperbug", GEN_M, "Czerwony Hiperinsekt", "Czerwone Hiperinsekty", "Czerwonego Hiperinsekta", "Czerwonym Hiperinsektem")
N("Green Hyperbug", GEN_M, "Zielony Hiperinsekt", "Zielone Hiperinsekty", "Zielonego Hiperinsekta", "Zielonym Hiperinsektem")
N("Blue Hyperbug", GEN_M, "Niebieski Hiperinsekt", "Niebieskie Hiperinsekty", "Niebieskiego Hiperinsekta", "Niebieskim Hiperinsektem")
N("Royal Jelly", GEN_N, "Królewskie Mleczko", "Królewskie Mleczka", "Królewskie Mleczko", "Królewskim Mleczkiem")
N("weird rock", GEN_N, "dziwna skała", "dziwne skały", "dziwną skałę", "dziwną skałą")

Orb("Invisibility", "Niewidzialności")

S("A weirdly colored rock. Hyperentomologists claim that the "
  "Hyperbug armies use these rocks to navigate back home after a victorious battle.",
  "Ta skała ma dziwny kolor. Hiperentomolodzy twierdzą, że roje Hiperinsektów "
  "używają tych skał do nawigacji, by wrócić do domu po wygranej bitwie.")

S("%The1 fights with %the2!", "%1 zaatakował %a2!")

S("The Hive is filled with Hyperbugs. They are huge insects which look a bit like "
  "ants, a bit like bees, and a bit like roaches. "
  "They live in circular nests, and an army of Hyperbugs will attack any intruder, "
  "including you and enemy Hyperbugs. Will you manage to get to the "
  "heart of such a nest, and get the precious Royal Jelly?",
  "Mrowisko jest wypełnione Hiperinsektami. Te wielkie owady wyglądają trochę jak "
  "mrówki, trochę jak pszczoły, trochę jak karaluchy. Żyją w okrągłych gniazdach, "
  "i atakują każdego najeźdźcę, łącznie z Tobą i wrogimi hiperinsektami. "
  "Czy uda Ci się dotrzeć do serca roju i zdobyć cenne królewskie mleczko?")

S("This is what Hyperbug Queens eat. Very tasty and healthy.",
  "Pożywienie Królowej Hiperinsektów, bardzo smaczne i zdrowe.")

S("When you have this Orb, most monsters won't see you, unless "
    "you are standing still, attacking, or picking up items.",
  "Kiedy masz Sferę Niewidzialności, większość potworów Cię nie widzi, "
  "chyba że stoisz w miejscu, atakujesz, albo podnosisz przedmioty.")

Orb("Earth", "Ziemi")

S("This orb lets you go through living walls. It also has powers in some of the other lands.",
  "Ta sfera pozwala Ci przechodzić przez żywą ścianę. Ma też moce w niektórych z pozostałych krain.")

// Land of Power
// -------------

N("Land of Power", GEN_F, "Kraina Mocy", "Kraina Mocy", "Krainą Mocy", "w Krainie Mocy")
N("Witch Apprentice", GEN_F, "Uczennica", "Uczennice", "Uczennicę", "Uczennicą")
N("Speed Witch", GEN_F, "Czarownica Szybka", "Czarownice Szybkie", "Czarownicę Szybką", "Czarownicą Szybką")
N("Flash Witch", GEN_F, "Czarownica Błysku", "Czarownice Błysku", "Czarownicę Błysku", "Czarownicą Błysku")
N("Fire Witch", GEN_F, "Czarownica Ognia", "Czarownice Ognia", "Czarownicę Ognia", "Czarownicą Ognia")
N("Aether Witch", GEN_F, "Czarownica Eteru", "Czarownice Eteru", "Czarownicę Eteru", "Czarownicą Eteru")
N("Winter Witch", GEN_F, "Czarownica Zimy", "Czarownice Zimy", "Czarownicę Zimy", "Czarownicą Zimy")
N("Evil Golem", GEN_M, "Zły Golem", "Złe Golemy", "Złego Golema", "Złym Golemem")
N("Powerstone", GEN_M, "Kamień Mocy", "Kamienie Mocy", "Kamień Mocy", "Kamieniem Mocy")
N("crystal cabinet", GEN_F, "kryształowa gablota", "kryształowe gabloty", "kryształową gablotę", "kryształową gablotą")

S("When you have this Orb, you will leave a trail of fire behind you.",
  "Kiedy masz Sferę Ognia, zostawiasz za sobą ognistą ścieżkę.")

S(
    "A Witch without any special powers. But watch out! She will "
    "pick up any basic Orbs on her path, and use their powers.",
    "Czarownica bez specjalnych mocy. Uważaj! Może ona podnieść "
    "dowolną podstawową Sferę na swojej drodze i użyć jej mocy.")

S(  "A Witch with a Speed spell. She moves twice as fast as you. Unless you "
    "have an Orb of Speed too, of course!",
    "Czarownica z czarem Szybkości. Rusza się dwa razy szybciej od Ciebie. "
    "Chyba, że Ty też masz Sferę Szybkości!")

S(  "A Witch with a Fire spell. She will leave a trail of fire behind her.",
    "Czarownica z czarem Ognia. Zostawia za sobą płonącą ścieżkę.")

S(  "A Witch with a Winter spell. She is able to move through fire.",
    "Czarownica z czarem Zimy. Może przechodzić przez ogień.")

S(  "A Witch with an Aether spell. She is able to move through fire and walls.",
    "Czarownica z czarem Eteru. Może przechodzić przez ogień i ściany.")

S(  "Somebody has summoned these evil constructs with a magical process.",
    "Wroga konstrukcja, tworzona poprzez magiczny proces.")

S(  "The Land of Power is filled with everburning fire, magical Orbs, and guarded by "
    "witches and golems. There are basic orbs lying everywhere, and more prized ones "
    "are kept in crystal cabinets.\n\n"
    "Witches are allowed to use all the powers of the "
    "basic orbs against intruders. These powers never expire, but a Witch "
    "can use only one power at a time (not counting Orbs of Life).\n\n"
    "Witches and Golems don't pursue you into other Lands. Also, most Orb powers "
    "are drained when you leave the Land of Power.",
    "Kraina Mocy jest wypełniona wiecznie płonącym ogniem, magicznymi Sferami, "
    "i strzeżona przez Czarownice i golemy. Podstawowe Sfery walają się wszędzie, a "
    "te bardziej cenne są trzymane w kryształowych gablotach.\n\n"
    "Czarownice używają wszystkich mocy podstawowych Sfer przeciwko intruzom. "
    "Ich moce nigdy nie przemijają, ale Czarownica może używać naraz tylko jednej "
    "mocy (nie licząc Sfer Życia).\n\n"
    "Czarownice i Golemy nie gonią Cię do innych krain. Większość Sfer "
    "traci swoją moc, gdy opuszczasz Krainę Mocy.")

S(
    "Witches use these crystal cabinets to protect Powerstones, as well as the more "
    "expensive Orbs. They are partially protected from thieves: they are too strong "
    "to be smashed by conventional attacks, and if you try to steal the item "
    "using an Orb of Aether, your Aether power will be completely drained.",
  "Czarownice używają tych kryształowych gablot, by chronić Kamienie Mocy oraz bardziej "
  "cenne Sfery. Gabloty są częściowo chronione przed kradzieżą: są zbyt mocne, "
  "by je rozbić konwencjonalnymi atakami, a jeśli spróbujesz ukraść przedmiot "
  "używając Sfery Eteru, Twoja moc Eteru zostanie całkowicie wyczerpana.")

S(
    "A Stone from the Land of Power. You are not sure what it is exactly, but "
    "as the Powerstones are kept in crystal cabinets, they are surely valuable.",
    "Kamień z Krainy Mocy. W sumie to nie wiesz, co to jest, ale trzymają te "
    "kamienie w kryształowych gablotach, także muszą być bardzo cenne.")

Orb("Fire", "Ognia")

// achievements

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

S("Your Aether power has expired! RUN!", "Twoja moc Eteru się wyczerpała! UCIEKAJ!")

S("%The1 tricks %the2.", "%1 pokona%ł1 %a2.")
S("You trick %the1.", "Pokona%łeś0 %a1.")

S(  "A Witch with a Flash spell. Very dangerous!\n\nLuckily, she never uses the spell if it "
    "would kill her friends. She could destroy an Evil Golem, though.",
    "Czarownica z czarem Błysku. Bardzo niebezpieczna!\n\nNa szczęście, nie użyje swojego "
    "czaru, jeśli by to zabiło jej przyjaciółki.")

S("Your Aether powers are drained by %the1!", "Twoja moc Eteru została wyssana przez %a1!")
S("As you leave, your powers are drained!", "Opuszczasz Krainę Mocy... Twoja moc znikła!")
S("%The1 picks up %the2!", "%1 podniosła %a2!") // currently only Witches, so only female form

S("You can right click any element to get more information about it.\n\n",
  "Kliknij obiekt prawym przyciskiem myszy, by zobaczyć więcej informacji na jego temat.\n\n")

S("TRANSLATIONWARNING", "")
S("TRANSLATIONWARNING2", "")

S("The Hyperbugs are fighting!", "Hiperinsekty ze sobą walczą!")

S("","") N("",GEN_N,"","","","")

S("Cheat-changed the display.", "Zmieniam tryb wyświetlania.")
S("Dead floor, with some rubble.", "Martwa ziemia, jest tu trochę gruzu.")

// Camelot
// -------

N("Camelot", GEN_O, "Camelot", "Camelot", "Camelot", "w Camelot")
N("wall of Camelot", GEN_F, "ściana Camelot", "ściany Camelot", "ścianę Camelot", "ścianą Camelot")
N("moat of Camelot", GEN_F, "fosa Camelot", "fosy Camelot", "fosę Camelot", "fosą Camelot")
N("Round Table", GEN_O, "Okrągły Stół", "Okrągłe Stoły", "Okrągły Stół", "Okrągłym Stołem")
N("Knight", GEN_M, "Rycerz", "Rycerze", "Rycerza", "Rycerzem")
N("Holy Grail", GEN_M, "Święty Graal", "Święte Graale", "Świętego Graala", "Świętym Graalem")

S(
  "The Knights of the Round Table are the greatest warriors of these lands. "
  "They are not very inventive with names though, as they call each of their "
  "castles Camelot. "
  "You are probably worthy of joining them, but they will surely give you "
  "some quest to prove yourself...\n\n"
  "Each castle contains a single treasure, the Holy Grail, in the center. "
  "The radius of the Round Table is usually 28, but after you find a Holy Grail "
  "successfully, each new castle (and each Round Table) you find will be bigger.",

  "Rycerze Okrągłego Stołu to najwięksi wojownicy tych krain. Niestety, nie są "
  "zbyt pomysłowi w nazywaniu i każdy swój zamek nazywają Camelot. "
  "Prawdopodobnie nadajesz się, by stać się jednym z nich, ale na pewno "
  "dadzą Ci jakąś misję, byś pokaza%ł0 swoją wartość...\n\n"
  "Każdy zamek ma jeden skarb (Święty Graal) w samym środku. Promień Okrągłego "
  "Stołu to normalnie 28, ale z każdym znalezionym Śwętym Graalem każdy nowy "
  "zamek (i jego Okrągły Stół) będzie większy.")

S("It would be impolite to land on the table!",
  "Byłoby niegrzecznie wylądować na stole!")
S("You jump over the table!", "Skaczesz przez stół!")
S("That was not a challenge. Find a larger castle!",
  "To nie było wyzwanie. Znajdź większy zamek!")
S("The Knights congratulate you on your success!",
  "Rycerze gratulują Ci sukcesu!")
S("The Knights laugh at your failure!",
  "Rycerze śmieją się z Twojej porażki!")
S("The Knights stare at you!", "Rycerze gapią się na Ciebie ze zdziwieniem!")
S("Come on, this is too easy... find a bigger castle!",
  "Eee, to będzie za łatwe... znajdź większy zamek!")
S("The Knights wish you luck!", "Rycerze życzą powodzenia!")

S("Congratulations! You have found the Holy Grail!",
  "Gratulacje! Znalaz%łeś0 Świętego Graala!")

S("\"I would like to congratulate you again!\"", "\"Pragnę jeszcze raz Ci pogratulować!\"")
S("\"Find the Holy Grail to become one of us!\"", "\"Znajdź Świętego Graala, by stać się jednym z nas!\"")
S("\"The Holy Grail is in the center of the Round Table.\"", "\"Święty Graal jest pośrodku Okrągłego Stołu.\"")
S("\"I enjoy watching the hyperbug battles.\"", "\"Lubię oglądać bitwy hiperinsektów.\"")
S("\"Have you visited a temple in R'Lyeh?\"", "\"By%łeś0 w świątyni w R'Lyeh?\"")
S("\"Nice castle, eh?\"", "\"Ładny zamek, co?\"")

// Temple
// ------

N("Temple of Cthulhu", GEN_F, "Świątynia Cthulhu", "Świątynia Cthulhu", "Świątynię Cthulhu", "w Świątyni Cthulhu")
N("big statue of Cthulhu", GEN_O, "pomnik Cthulhu", "pomniki Cthulhu", "pomnik Cthulhu", "pomnikiem Cthulhu")
N("Grimoire", GEN_F, "Księga", "Księgi", "Księgę", "Księgą")
N("Cult Leader", GEN_M, "Przywódca Kultu", "Przywódcy Kultu", "Przywódcę Kultu", "Przywódcą Kultu")

S("The temple of Cthulhu consists of many concentric circles of columns. "
  "You will surely encounter many Cultists there, who believe that a pilgrimage "
  "to the inner circles will bring them closer to Cthulhu himself, and Grimoires "
  "which surely contain many interesting secrets.\n\n"
  "The circles in the temple of Cthulhu are actually horocycles. They are "
  "infinite, and there is an infinite number of them.",

  "Świątynia Cthulhu składa się z wielu koncentrycznych kręgów kolumn. "
  "Spotkasz tu wielu Kultystów, dla których pielgrzymka do wewnętrznych kręgów "
  "to szansa na bycie bliżej Cthulhu, a także Księgi zawierające wiele interesujących "
  "sekretów.\n\n"
  "Kręgi w Świątyni Cthulhu są właściwie horocyklami. Są nieskończone i "
  "jest ich nieskończenie wiele."
  )

S("The Grimoires contain many secrets of the Great Old Ones. "
  "Each new inner circle in the Temple of Cthulhu contains new Grimoires, with new secrets. "
  "You hope to read them when you return home, and to learn many things. "
  "The knowledge is valuable to you, but it is rather pointless to try to get "
  "several copies of the same Grimoire...",
  "Te Księgi zawierają tajemnice Wielkich Przedwiecznych. "
  "Każdy krąg wewnętrzny w Świątyni Cthulhu zawiera nowe Księgi, z nowymi sekretami. "
  "Zamierzasz je przeczytać po powrocie do domu. Wiedza jest dla Ciebie wartościowa, "
  "ale nie ma sensu, by brać kilka kopii tej samej Księgi..."
  )

S("These statues of Cthulhu are too large to carry, and they don't look too "
  "valuable anyway. Most monsters will never go through them... they probably have "
  "their reasons. But you can go! When you move into the cell containing "
  "a statue, you push the statue to the cell you left.\n",

  "Te pomniki Cthulhu są za duże, by je nosić, zresztą i tak nie wyglądają "
  "na zbyt cenne. Potwory nigdy przez nie nie przechodzą... pewnie mają swoje "
  "powody. Ale Ty możesz przejść! Kiedy wchodzisz na pole z pomnikiem, "
  "automatycznie przesuwasz pomnik na pole, które opuści%łeś0."
  )

S("These Cultists can push the statues, just like you.",
  "Ci kultyści mogą przesuwać pomniki tak samo jak Ty.")

S("You already have this Grimoire! Seek new tomes in the inner circles.",
  "Już masz taką Księgę! Szukaj nowych tomów w wewnętrznych kręgach.")

S("You push %the1 behind you!", "Popchn%ąłeś0 %a1 za siebie.")

// More texts
// ----------

S("Your total kills", "Łączne zabicia")

S(
  "In most lands, more treasures are generated with each enemy native to this land you kill. "
  "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
  "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
  "but are not counted in the total kill count.",

  "W większości krain, zabijanie przeciwników pochodzących z danej krainy "
  "powoduje, że generowane jest więcej skarbów. Ponadto, 100 zabić jest konieczne, "
  "by móc znaleźć Cmentarz i Mrowisko.\n\n"
  "Przyjazne stworzenia i części potworów (jak np. Bluszcz) występują na liście, "
  "ale nie są liczone do łącznej liczby zabić.")

S("There must be some temples of Cthulhu in R'Lyeh...",
  "Muszą być jakieś świątynie Cthulhu w R'Lyeh...")

S("You overheard Hedgehog Warriors talking about emeralds...",
  "Podsłucha%łeś0 Wojowników-Jeży rozmawiających o szmaragdach...")

S("You overhear miners talking about a castle...",
  "Podsłucha%łeś0 górników rozmawiających o zamku...")

S("A castle in the Crossroads...", "O zamku na Skrzyżowaniu...")

/*
S("You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "Po znalezieniu 5 Statuetek Cthulhu możesz w R'Lyeh trafić na Świątynie Cthulhu.")
*/

S("You have to escape first!", "Musisz najpierw uciec!");
S("There is not enough space!", "Nie ma miejsca!");

S("Customize character", "pokoloruj postać")
S("gender", "płeć")

//

Orb("the Dragon", "Smoka")

S("This Orb allows you to throw fire, just like the Fire Cultists.\n\n"
  "Each fire drains 5 charges. You are not allowed to throw fire into adjacent cells.",
  "Ta sfera pozwala Ci rzucać ogniem tak jak Ogniści Kultyści. \n\n"
  "Każdy rzucony ogień wymaga 5 ładunków. Nie możesz rzucać ogniem do sąsiednich "
  "komórek."
  )

S("You throw fire!", "Rzucasz ogniem!")

Orb("Trickery", "Sztuczek")

N("Illusion", GEN_F, "Iluzja", "Iluzje", "Iluzję", "Iluzją")

S("This Orb allows you to create illusions of yourself. Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.\n\n"
  "Each illusion takes 5 charges to create, and one extra charge "
  "per turn. You can also click your illusion to take it away, restoring 4 charges.\n\n"
  "If you have both Orb of Teleport and Orb of Trickery, Illusion is cast "
  "first -- you can then teleport on your Illusion to switch places with it.",
  "Ta sfera pozwala Ci tworzyć iluzje samego siebie. Większość potworów celuje w "
  "iluzje, podobnie jak w Ciebie, Dudniki i Twoich przyjaciół.\n\n"
  "Każda iluzja wymaga 5 ładunków do stworzenia i jednego dodatkowego ładunku na "
  "kolejkę. Możesz też kliknąć swoją iluzję, by ją zabrać, co przywraca 4 ładunki.\n\n"
  "Jeśli masz zarówno Sferę Teleportacji jak i Sztuczek, Iluzja rzucana jest "
  "jako pierwsza -- możesz następnie teleportować się na swoją iluzję, by się z nią "
  "zamienić miejscami."
  )

S("Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.",
  "Większość potworów celuje w "
  "iluzje, podobnie jak w Ciebie, Dudniki i Twoich przyjaciół.")

S("You create an Illusion!", "Tworzysz iluzję!")
S("You take the Illusion away.", "Zabierasz iluzję!")

S("You cannot target that far away!", "Nie możesz celować tak daleko!")
S("You cannot target that close!", "Nie możesz celować tak blisko!")
S("Cannot teleport on a monster!", "Nie możesz teleportować się na potwora!")
S("Cannot teleport on an item!", "Nie możesz teleportować się na przedmiot!")
S("Cannot teleport here!", "Nie możesz teleportować się tutaj!")
S("Cannot cast illusion on an item!", "Nie możesz rzucić iluzji na przedmiot!")
S("Cannot cast illusion here!", "Nie możesz rzucić iluzji tutaj!")
S("Cannot cast illusion on a monster!", "Nie możesz rzucić iluzji na potwora!")
S("Cannot throw fire there!", "Nie możesz tego podpalić!")

// teleport -> target
// You cannot target that close!

S("or ESC to see how it ended", "lub ESC by zobaczyć, jak się skończyło")
S("high contrast", "wysoki kontrast")
S("draw the heptagons darker", "siedmiokąty ciemniejsze");
S("targetting ranged Orbs Shift+click only",
  "celowanie sfer na odgległość wymaga Shift+klik")
S("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc.",
  "Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, itd.")
S("also hold Alt during the game to toggle high contrast",
  "Alt podczas gry przełącza wysoki kontrast")

// Crossroads II
// -------------

N("Crossroads II", GEN_N, "Skrzyżowanie II", "Skrzyżowania II", "Skrzyżowanie II", "na Skrzyżowaniu II")

S(
  "An alternate layout of the Crossroads. It is more dense and more regular, "
  "although you won't find the castle of Camelot here.",

  "Alternatywny układ Skrzyżowania. Jest gęstszy i bardziej regularny, "
  "choć nie znajdziesz tutaj zamku Camelot.")

// Caribbean
// ---------

// note: in Polish,
// 'Karaiby' is plural in Polish, should probably fix the grammar for this

N("Caribbean", GEN_N, "Karaiby", "Morze Karaibskie", "Karaibów", "na Karaibach")

N("Pirate", GEN_M, "Pirat", "Piraci", "Pirata", "Piratem")
N("Shark", GEN_M, "Rekin", "Rekiny", "Rekina", "Rekinem")
N("Parrot", GEN_F, "Papuga", "Papugi", "Papugę", "Papugą")
N("Pirate Treasure", GEN_M, "Skarb Piratów", "Skarby Piratów", "Skarb Piratów", "Skarbem Piratów")
N("Compass", GEN_M, "Kompas", "Kompasy", "Kompas", "Kompasem")

N("sea", GEN_F, "morze", "morza", "morze", "morzem")
N("boat", GEN_F, "łódka", "łódki", "łódkę", "łódką")
N("island", GEN_F, "wyspa", "wyspy", "wyspę", "wyspą")
N("tree", GEN_N, "drzewo", "drzewa", "drzewo", "drzewem")

Orb("Time", "Czasu")

S("This dangerous sea contains nasty sharks and pirates. ",
  "Niebezpieczne morze, pełne rekinów i piratów.")

S("The islands of Caribbean are infinite and circular, and "
  "the pirates probably have hidden their treasures somewhere far from the coast.",
  "Wyspy na Karaibach są okrągłe i nieskończone... Skarby piratów są pewnie "
  "schowane gdzieś daleko od brzegu.")

S(
    "Hyperbolic pirates do not need huge ships, since so many lands to conquest "
    "are so close. These small boats are enough for them.\n\n"
    "Boats allow you to go through water. If you are in a boat, you can move into "
    "a water cell (and the boat will come with you).",

    "Hiperboliczni piraci nie potrzebują dużych statków, skoro tyle lądów do "
    "zdobycia jest tak blisko. Wystarczą im małe łódki.\n\n"
    "Łódki pozwalają poruszać się przez wodę. Jeśli jesteś na łódce, możesz "
    "przejść na pole z wodą (a łódka przesunie się razem z Tobą).")

S(
    "The forests of Caribbean are too dense to be traversed by humans, "
    "and they are hard to burn. Many colorful parrots can be found there.",
    "Lasy na Karaibach są zbyt gęste, by mógł przez nie przejść człowiek "
    "i trudno jest je spalić. Mieszka w nich dużo kolorowych papug.")

S("Ye wonder where did th' Pirates find all these riches...",
    "Zastanawiasz się, gdzie Piraci znaleźli te bogactwa...")

S(
  "The hyperbolic pirates have no use for treasure maps. However, they have found "
  "out that a compass points to the center of the island. So they just go as "
  "far towards the center as they can, and hide their treasure there.",

  "Mapy są bezużyteczne dla hiperbolicznych piratów, ale zauważyli oni, że "
  "kompas zawsze wskazuje w kierunku środka wyspy. Zatem piraci po prostu "
  "idą tak daleko w stronę środka, jak mogą i tam chowają swoje skarby.")

S("Just a nasty shark.", "Zwykły groźny rekin.")

S("Parrots feel safe in the forests of Caribbean, so they "
    "never leave them. But they will help the Pirates by attacking the intruders.",

  "Papugi czują się bezpiecznie w lasach Karaibów i ich nie opuszczają, ale "
  "chętnie pomogą Piratom walczyć z najeźdźcami.")

S("Just a typical hyperbolic pirate.", "Typowy hiperboliczny pirat.")

S(
    "Normally, the power of most Orbs slowly fades away, even when "
    "you are not actively using them. This Orb prevents this.\n\n"

    "When you have the Orb of Time, Orbs which are not active won't lose their power. "
    "Orbs are considered active if they have a continuous power which has actually "
    "affected something in the last turn.\n\n"

    "Orbs of Shielding remain active after being activated (even if you are no longer "
    "attacked), and Orbs of Time have a bigger cap inside their native Caribbean than "
    "outside.",

    "Zazwyczaj moc Sfer powoli wygasa, nawet gdy ich nie używasz aktywnie. Ta Sfera "
    "temu zapobiega.\n\n"

    "Jeśli masz Sferę Czasu, nieaktywne sfery nie będą traciły mocy. Sfery są "
    "uważane za aktywne, jeśli mają ciągły efekt, który rzeczywiście na coś "
    "wpłynął w ostatniej turze.\n\n"

    "Sfery Tarczy pozostają aktywne po aktywacji (nawet jeżeli nikt więcej nie "
    "atakuje), a Sfery Czasu mają większy limit na Karaibach niż poza nimi.")

// Achievements

// Parrot : Find and collect Pirate Treasure.
// Caribbean Shark : Collect 10 Pirate Treasures.
// Pirate : Collect 25 Pirate Treasures.
// Master of Caribbean : Collect 50 Pirate Treasures.

// Red Rock Valley
// ---------------

N("Red Rock Valley", GEN_F, "Dolina Czerwonej Skały", "Dolina Czerwonej Skały", "Dolinę Czerwonej Skały", "w Dolinie Czerwonej Skały")

N("Rock Snake", GEN_M, "Skalny Wąż", "Skalne Węże", "Skalnego Węża", "Skalnym Wężem")
N("Rock Snake Tail", GEN_O, "Ogon Węża", "Ogony Węża", "Ogon Węża", "Ogonem Węża")
N("Red Troll", GEN_M, "Czerwony Troll", "Czerwone Trolle", "Czerwonego Trolla", "Czerwonym Trollem")
N("Red Gem", GEN_O, "Czerwony Kamień", "Czerwone Kamienie", "Czerwony Kamień", "Czerwonym Kamieniem")

N("rock I", GEN_F, "skała I", "skały I", "skałę I", "skałą I")
N("rock II", GEN_F, "skała II", "skały II", "skałę II", "skałą II")
N("rock III", GEN_F, "skała III", "skały III", "skałę III", "skałą III")

Orb("Space", "Przestrzeni")

S("Not enough power for telekinesis!", "Za mało mocy do telekinezy!")

S("This land contains high rock formations. Most of the valley is at level 0, "
  "while items are found at level 3. It is impossible to gain two or more levels, "
  "or to lose three levels, in a single move, (attacks are possible at any "
  "difference, though). Kill Red Trolls and Rock Snakes to make a cell higher.",

  "W tej krainie są wysokie formacje skalne. Większość doliny jest na poziomie 0, "
  "a przedmioty można znaleźć na poziomie 3. Niemożliwe jest wzniesienie się na "
  "dwa lub więcej poziomów lub obniżenie się o trzy poziomy w jednym ruchu "
  "(ale atakować można przy każdej różnicy poziomów). Zabij Czerwonego Trolla "
  "albo Skalnego Węża, by podnieść poziom pola.")

S("A kind of Troll native to the Red Rock Valley.",
  "Ten rodzaj Trolla mieszka w Dolinie Czerwonej Skały.")

S(
  "Rock snakes are similar to Sandworms, but they move each turn. However, they "
  "only move on the hexagonal cells. They are also somewhat longer than the Sandworms.",

  "Kamienne węże są podobne do Pustynnych Czerwi, ale wykonują ruch w każdej kolejce. "
  "Mogą jednak poruszać się tylko po sześciokątach. Są trochę dłuższe niż Pustynne "
  "Czerwie.")

S("A gem from the Red Rock Valley.", "Kamień szlachetny z Doliny Czerwonej Skały.")

S(
    "This Orb is able to bring faraway items to your location, even if there are "
    "monsters or barriers on the way. The cost of "
    "bringing an item (in charges) equals the square of its distance to you. Contrary "
    "to some other Orbs, usage is not allowed if you have not enough power left.",

  "Ta Sfera może ściągać do Ciebie odległe przedmioty, nawet, jak po drodze są "
  "potwory lub inne przeszkody. Koszt ściągnięcia przedmiotu (w ładunkach) równy jest "
  "kwadratowi odległości. W przeciwieństwie do niektórych innych Sfer, nie możesz "
  "użyć Sfery Przestrzeni, jeśli nie masz wystarczająco dużo mocy.")

// Climber : Find and collect a Red Gem.
// Red Troll : Collect 10 Red Gems.
// Rock Snake : Collect 25 Red Gems.
// Master of Red Rock : Collect 50 Red Gems.

// missing sentences

S("%The1 drowns!", "%1 się utopi%ł1!");
S("%The1 falls!", "%1 spad%ł1!");

// these were missing from the translation for some reason

S("Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.",
  "Piekło jest pełne tych jezior pełnych płonącej siarki, w kształcie złych gwiazd...")

// Hardcore Mode
// -------------

S("hardcore mode", "tryb hardcore");

S("One wrong move and it is game over!", "Jeden fałszywy ruch i koniec gry!");
S("Not so hardcore?", "Hardkor to nie to?");

// Shoot'em up Mode
// ----------------

S("shoot'em up mode", "tryb strzelanki");
S("Welcome to the Shoot'em Up mode!", "Witaj w trybie strzelanki!");
S("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move", "F/;/Space/Enter/KP5 = strzał, WASD/IJKL/Numpad = ruch");

N("Rogue", GEN_M, "Cwaniak", "Cwaniaki", "Cwaniaka", "Cwaniakiem")
N("Knife", GEN_O, "Nóż", "Noże", "Nóż", "Nożem")
N("Flail", GEN_O, "Cep", "Cepy", "Cep", "Cepem")
N("Fireball", GEN_F, "Ognista Kula", "Ogniste Kule", "Ognistą Kulę", "Ognistą Kulą")
N("Tongue", GEN_O, "Język", "Języki", "Język", "Językiem")

S("In the Shoot'em Up mode, you are armed with thrown Knives.",
  "W trybie strzelanki jesteś uzbrojony w noże do rzucania.")
S("A simple, but effective, missile, used by rogues.",
  "Prosta choć efektywna broń rzucana, używana przez poszukiwaczy skarbów.")
S("This attack is likely to hit the attacker.",
  "Ten atak pewnie trafi w atakującego.")
S("This magical missile burns whatever it hits.", "Ten magiczny pocisk pali to, w co trafi.")
S("Some monsters have long tongues, which allow them to attack enemies in nearby cells.",
  "Niektóre potwory mają długie języki, dzięki którym mogą atakować sąsiednie pola.")

// modes for the local highscores
// ------------------------------

S(", m - mode: normal", ", m - tryb normalny")
S(", m - mode: hardcore only", ", m - tryb hardcore")
S(", m - mode: shoot'em up", "m - tryb strzelanki")

// update description for Steam

// (Shoot'em Up mode and two more lands than in the free version, and more are planned!)

// missing lines

S("You would get hurt!", "To by bolało!")
S("PARTIAL", "CZĘŚCIOWO")

S("Cannot drop %the1 here!", "Nie możesz tu położyć %a1!");

// Euclidean scores
// ----------------

S(" (E:%1)", " (E:%1)");

S("You cannot attack Rock Snakes directly!", "Nie możesz atakować Skalnych Węży bezpośrednio!");

S("\"I am lost...\"", "\"Zgubiłem się...\"");

S("You are killed by %the1!", "Zabi%ł1 Cię %1!")

// new menu for Version 7.1
// ------------------------

S("(v) menu", "(v) menu")
S("return to the game", "wróć do gry")
S("or 'v' to see the main menu", "lub 'v' by zobaczyć menu")

S("HyperRogue %1", "HyperRogue %1")
S("basic configuration", "podstawowa konfiguracja")
S("advanced configuration", "zaawansowana konfiguracja")
S("local highscores", "lokalne wyniki")
S("help", "help")
S("restart game", "gra od nowa")
S("special game modes", "specjalne tryby gry")
S("save the game", "zapisz i zakończ")
S("quit the game", "koniec gry")
S("review your quest", "przegląd misji")
S("review the scene", "przegląd sceny")
S("game over screen", "ekran końca gry")
S("continue game", "kontynuuj grę")
S("play the game!", "graj!")

// fixed descriptions for Shmup achievements
// -----------------------------------------

S("You have used less knives than ever before. Congratulations!",
  "Zużyłeś mniej noży niż kiedykolwiek wcześniej. Gratulacje!")

S("You have improved both your real time and knife count. Congratulations!",
  "Poprawi%łeś0 swój najlepszy czas rzeczywisty i liczbę noży. Gratulacje!")

S("%1 knives (%2)", "noży: %1 (%2)")

// temporary
S("No description yet." ,"Brak opisu.")

// special case when no Spice is visible
S("The sandworm explodes!", "Czerw eksplodował!")

// Ocean
// -----

// achievements
// Reached Ocean: Find and collect an Amber.
// Albatross: Collect 10 Ambers.
// Amber Albatross: Collect 25 Ambers.
// Master of the Tides: Collect 50 Ambers.

N("Ocean", GEN_O, "Ocean", "Oceany", "Ocean", "na Oceanie")

S(
    "You can collect some valuable Ambers on the coast, but beware the tides!\n\n"
    "You can also take one of the boats and venture into the Ocean, "
    "to find other coasts, dangerous whirlpools, and islands inhabited by Pirates.",

  "Możesz zbierać cenne bursztyny na plaży, ale uważaj na odpływy!\n\n"
  "Możesz też wziąć łódkę i popłynąć nią przez Ocean, by znaleźć inne brzegi, "
  "niebezpieczne wiry oraz wyspy zamieszkane przez Piratów.")

N("Albatross", GEN_O, "Albatros", "Albatrosy", "Albatrosa", "Albatrosem")

S(
  "Those large seabirds attack you for some reason. At least they are not "
  "as fast as Eagles...",
  "Te wielkie morskie ptaki atakują Cię z jakiegoś powodu. Przynajmniej nie są "
  "tak szybkie, jak Orły...")

N("stranded boat", GEN_F, "łódka na mieliźnie", "łódki na mieliźnie", "łódkę na mieliźnie", "w łódce na mieliźnie")

S(
  "This boat cannot go through the sand. But if you sit inside and "
  "wait for the tide, you will be able to use it to travel through the Ocean.",

  "Ta łódka nie może pływać przez piasek. Ale jeśli w niej usiądziesz "
  "i poczekasz na przypływ, będziesz móc płynąć przez Ocean."
  )

N("Amber", GEN_O, "Bursztyn", "Bursztyny", "Bursztyn", "Bursztynem")

S(
  "When the tide is away, beautiful ambers can be found on the hyperbolic beaches. "
  "Perhaps there used to be a forest here?" ,
  "W czasie odpływu na hiperbolicznej plaży można znaleźć piękne bursztyny.")

Orb("Air", "Powietrza")

S("This Orb allows you to blow your enemies away.\n\n"
  "Click a monster to blow it one cell away. It cannot be used against mimics, ghosts, sharks and other monsters restricted to a specific terrain, and multi-tile monsters.",
  "Ta Sfera pozwala Ci zdmuchnąć Twoich przeciwników.\n\n"
  "Kliknij potwora, by go zdmuchnąć o jedno pole. Nie można używać przeciwko mimikom, duchom i potworom zajmującym wiele pól."
  )


N("Sea Border", GEN_O, "Granica morska", "Granice morskie", "Granicy morskiej", "na Granicy morskiej")

S("Border between seas.", "Granica między morzami.")

// Whirlpool
// ---------

// achievements
// Escaped Whirlpool: Collect a Pearl, and escape the Whirlpool.
// Vortex Shark: Collect 10 Pearls.
// Vortex Pirate: Collect 25 Pearls.
// Master of the Whirlpool: Collect 50 Pearls.

N("Whirlpool", GEN_O, "Wir", "Wiry", "Wir", "w Wirze")

S(
  "Many lost boats are spinning in this whirlpool. Some of them contain treasures "
  "and Pirates.\n\n"
  "Unmanned boats will go one cell clockwise in each turn. Sharks and manned boats "
  "are only allowed to go with the current, that is, towards the center, or clockwise.",

  "Dużo statków zagubiło się w tym wirze. Na niektórych z nich są skarby i Piraci.\n\n"
  "Statki bez załogi poruszają się o 1 pole w każdej kolejce, zgodnie z ruchem wskazówek "
  "zegara. Rekiny i statki z załogą mogą poruszać się tylko z prądem, czyli w kierunku "
  "środka wiru albo zgodnie z ruchem wskazówek zegara."
  )

N("Pearl", GEN_F, "Perła", "Perły", "Perłę", "Perłą")

S(
  "You do not know exactly why, but there are valuable pearls on many boats "
  "in the whirlpool.",
  "Właściwie nie wiesz, dlaczego, ale na wielu statkach w Wirze można znaleźć "
  "drogocenne perły."
  )

Orb("Water", "Wody")

S( "This Orb allows your boat to go against the current, "
   "and also to go into the land, creating water on the way.",
   "Ta Sfera pozwala Twojemu statkowi płynąć pod prąd, "
   "a także płynąć przez ląd, tworząc wodę po drodze.")

S("You cannot go against the current!", "Nie możesz płynąć pod prąd!")

// Minefield
// ---------

// achievements
// Reached Minefield: Collect a Bomberbird Egg.
// Minesweeper: Collect 10 Bomberbird Eggs.
// Bomberbird: Collect 25 Bomberbird Eggs.
// Master of the Minefield: Collect 50 Bomberbird Eggs.

N("Minefield", GEN_N, "Pole Minowe", "Pola Minowe", "Pole Minowe", "na Polu Minowym")
N("minefield", GEN_N, "pole minowe", "pola minowe", "pole minowe", "polem minowym")
N("cell without mine", GEN_N, "pole bez miny", "pola bez min", "pole bez min", "polem bez min")

S(
  "This minefield has been created by the Bomberbirds to protect their valuable eggs. "
  "Mines explode when they are stepped on, creating fire all around them "
  "(this fire never expires in the Minefield). If you don't step on a mine, you "
  "know the number of mines in cells around you. These numbers are marked with "
  "color codes on the cells you have visited.",
  "To pole minowe zostało stworzone przez ptaki, Bombardiery, w celu ochrony ich jaj. "
  "Miny eksplodują, gdy ktoś na nich stanie, tworząc ogień wokół (ten ogień nigdy "
  "nie wygasa na Polu Minowym). Jeśli uda Ci się nie stanąć na minę, to poznajesz liczbę "
  "min na polach wokół siebie. Te liczby są oznaczone kolorowymi kropkami na "
  "odwiedzonych polach."
  )

N("Bomberbird", GEN_O, "Bombardier", "Bombardiery", "Bombardiera", "Bombardierem")

S(
    "Dark red birds who have created the minefield.\n\n"
    "They create a mine on the spot where they are killed, provided "
    "that the terrain is suitable. Also note that mines are triggered "
    "by dead birds falling on them.",

    "Ciemnoczerwone ptaki, które stworzyły pole minowe.\n\n"
    "Na polu, na które spadły, powstaje mina, o ile teren jest odpowiedni. "
    "Pamiętaj, że mina wybucha, gdy spadnie na nią martwy ptak.")

N("Tame Bomberbird", GEN_O, "Oswojony Bombardier", "Oswojone Bombardiery", "Oswojonego Bombardiera", "Oswojonym Bombardierem")

S(
  "This bomberbird will follow you at some distance, and attack your enemies. "
  "You can kill it to place a mine.",

  "Ten bombardier będzie latał za Tobą i atakował Twoich wrogów. Możesz też "
  "go zabić, by podłożyć minę.")

N("Bomberbird Egg", GEN_N, "Jajo Bombardiera", "Jaja Bombardiera", "Jajo Bombardiera", "Jajem Bombardiera")

S(
  "Bomberbird eggs are big and tasty, and thus valuable. "
  "They can hatch when left alone for some time (but this will never happen "
  "if you are watching).",

  "Jaja Bombardierów są wartościowe, bo są duże i smaczne. "
  "Mogą się wykluć, gdy dasz im czas (ale nie stanie się to, jeżeli patrzysz)."
  )


Orb("Friendship", "Przyjaźni")

S("This Orb summons a friendly Bomberbird.",
  "Ta sfera przywołuje przyjaznego Bombardiera.")

S("Still confused? Read the FAQ on the HyperRogue website!\n\n",
  "Coś jest niejasne? Przeczytaj FAQ na stronie HyperRogue! (po angielsku)\n\n")

S("Nowhere to blow %the1!", "Nie ma gdzie zdmuchnąć %a1!")
S("%The1 is immune to wind!", "%1 jest odporn%ya1 na wiatr!")
S("You blow %the1 away!", "Zdmuchn%ąłeś0 %a1!")

S("WARNING: you are entering a minefield!", "UWAGA: wchodzisz na pole minowe!")
S("No mines next to you.", "Nie ma min obok Ciebie.")
S("A mine is next to you!", "Jedna mina koło Ciebie!")
S("Two mines next to you!", "Dwie miny koło Ciebie!")
S("Three mines next to you!", "Trzy miny koło Ciebie!")
S("Four mines next to you!", "Cztery miny koło Ciebie!")
S("Five mines next to you!", "Pięć min koło Ciebie!")
S("Six mines next to you!", "Sześć min koło Ciebie!")
S("Seven mines next to you!", "Siedem min koło Ciebie!")

S("You have to run away from the water!", "Musisz uciec z wody!")

// VERSION 7.2
// ===========

// Palace
// ------

N("Palace", GEN_O, "Pałac", "Pałace", "Pałac", "w Pałacu")
N("palace wall", GEN_F, "mur Pałacu", "mury Pałacu", "mur Pałacu", "murem Pałacu")

S("Apparently a princess is kept locked somewhere, but you won't ever find her in this hyperbolic palace. ",
  "Gdzieś tu jest uwięziona księżniczka, ale w tym hiperbolicznym pałacu nigdy jej nie znajdziesz. ")

S("Apparently a prince is kept locked somewhere, but you won't ever find him in this hyperbolic palace. ",
  "Gdzieś tu jest uwięziony książę, ale w tym hiperbolicznym pałacu nigdy go nie znajdziesz. ")

S("So better just to concentrate on collecting treasure. "
  "Beware the traps, and the guards, who are hard to kill!",

  "Także lepiej się skoncentrować na szukaniu skarbów. "
  "Uwaga na pułapki i trudnych do pokonania strażników!"
  )

N("trapdoor", GEN_N, "zapadnia", "zapadnie", "zapadnię", "zapadnią")
S("This floor will fall after someone goes there. Go quickly!",
  "Ta podłoga się zapadnie, jak ktoś będzie przechodził. Prechodź szybko!")

N("closed gate", GEN_F, "zamknięta brama", "zamknięte bramy", "zamkniętą bramę", "zamkniętą bramą")
N("open gate", GEN_F, "otwarta brama", "otwarte bramy", "otwartą bramę", "otwartą bramą")
N("closing plate", GEN_F, "zamykająca płyta", "zamykające płyty", "zamykającą płytę", "zamykającą płytą")
N("opening plate", GEN_F, "otwierająca płyta", "otwierające płyty", "otwierającą płytę", "otwierającą płytą")

S("Green pressure plates open all gates in radius 3 around them. "
  "Similarly, red pressure plates close all gates in this radius.",

  "Zielone płyty otwierają wszystkie bramy w promieniu 3 wokół nich. "
  "Podobnie, czerwone płyty zamykają wszystkie bramy w tym promieniu.")

N("Hypersian Rug", GEN_O, "Hiperski Dywan", "Hiperskie Dywany", "Hiperski Dywan", "Hiperskim Dywanem")
S("Nice, a hyperbolic rug! It will not fit very well on your flat Euclidean floor, but who cares?",
  "Jaki ładny hiperboliczny dywan! Nie będzie dobrze przylegał do Twojej płaskiej, euklidesowej podłogi, ale co z tego?")

Orb("the Frog", "Żaby")
S("This Orb lets you jump to a place which is two cell away from you, in a single turn. "
  "You can jump over water, chasms and fire, but not over walls.",
  "Ta Sfera pozwala na skok do miejsca odległego od Ciebie o dwa pola, w jednej kolejce. "
  "Możesz przeskoczyć wodę, przepaść i ogień, ale nie ścianę.")
S("Cannot jump through %the1!", "Nie możesz skakać przez %a1!")
S("You jump!", "Skaczesz!")

S("\n\nNote: when pushing %the1 off a heptagonal cell, you can control the pushing direction "
  "by clicking left or right half of the heptagon.",
  "\n\nUwaga: kiedy spychasz %a1 z siedmiokąta, możesz sterować kierunkiem pchnięcia "
  "klikając lewą lub prawą połowę siedmiokąta."
  )

S(" With the keyboard, you can rotate the view for a similar effect (Page Up/Down).",
  " Na klawiaturze możesz obrócić widok, by uzyskać podobny efekt (Page Up/Down).")

N("Palace Guard", GEN_M, "Strażnik Pałacu", "Strażnicy Pałacu", "Strażnika Pałacu", "Strażnikiem Pałacu")

S("You have to hit Palace Guards several times to kill them. After each hit, they "
  "are pushed away and stunned for some time.",
  "Musisz uderzyć Strażnika kilka razy, by go zabić. Po każdym trafieniu strażnik "
  "jest odpychany i ogłuszony przez pewien czas.")

N("Fat Guard", GEN_M, "Gruby Strażnik", "Grubi Strażnicy", "Grubego Strażnika", "Grubym Strażnikiem")

S("Fat guards are too heavy to be pushed away by your hits.",
  "Grubi Strażnicy są zbyt ciężcy, by dało się ich odepchnąć atakiem.")

N("Skeleton", GEN_M, "Kościotrup", "Kościotrupy", "Kościotrupa", "Kościotrupem")
S("Skeletons work similar to Palace Guards, but they won't die no matter how many "
  "times you hit them. Well, you have to be more creative...\n\n"
  "Skeletons attacked outside of their native land, Palace, are stunned for a longer time.",
  "Kościotrupy działają podobnie do Strażników Pałacu, ale nie zabijesz ich, "
  "ilekolwiek byś ich atakowa%ł0. Wymyśl coś bardziej twórczego...\n\n"
  "Kościotrup zaatakowany poza Pałacem będzie ogłuszony przez dłuższy czas."
  )

N("Vizier", GEN_M, "Wezyr", "Wezyrowie", "Wezyra", "Wezyrem")
S("Viziers are neither pushed away nor stunned. However, you attack them automatically when "
  "escaping from them.",
  "Wezyrowie ani nie są ogłuszani, ani odpychani, ale automatycznie ich atakujesz, gdy "
  "od nich uciekasz.")

S("You stun %the1.", "Ogłuszy%łeś0 %a1.")
S("The gate is closing right on you! RUN!", "Brama leci na Ciebie! UCIEKAJ!")
S("%The1 is crushed!", "%1 zosta%ł1 przygniecion%ya1!")
S("You hit %the1.", "Trafi%łeś0 %a1.")
S("Hit him by walking away from him.", "Odejdź od niego, by go trafić.")
S("Hmm, he has been training in the Emerald Mine. Interesting...", "Ciekawe... trenował w Kopalni Szmaragdów.")

// Reached the Palace: Find and collect a Hypersian Rug.
// Palace Guard: Collect 10 Hypersian Rugs.
// Vizier: Colect 25 Hypersian Rugs.
// Prince: Colect 50 Hypersian Rugs.

// Living Fjord
// ------------

N("Living Fjord", GEN_O, "Żywy Fiord", "Żywe Fiordy", "Żywy Fiord", "w Żywym Fiordzie")

S("A coastal area, from where you can get both to the inland worlds and to the Ocean. "
  "Each turn, each cell will become water or earth, based on the majority of cells around it. "
  "Contrary to the Living Caves, this process is not affected by most items and monsters, "
  "but elementals, dead Trolls, and cells outside of the Living Fjord have "
  "a very powerful effect.",
  "Nadbrzeżny rejon, z którego możesz dostać się zarówno do krain wewnątrzlądowych "
  "jak i na Ocean. W każdej kolejce, każde pole zmienia się w wodę lub ziemię "
  "w zależności od większości pól wokół. W przeciwieństwie do Żywej Jaskini na ten proces "
  "nie wpływa większość przedmiotów i stworzeń, ale żywiołaki, martwe Trolle i pola "
  "poza granicą Żywego Fiordu mają bardzo silny efekt."
  )

N("Fjord Troll", GEN_M, "Fjordtroll", "Fjordtrolle", "Fjordtrolla", "Fjordtrollem")
S("Fjord Trolls leave a wall when they die, causing the living fjord to rise around it. "
  "Contrary to Rock Trolls, items around are not destroyed.",
  "Fjordtrolle zostawiają ścianę po zabiciu, co powoduje, że żywy fiord w okolicy się podnosi. "
  "W przeciwieństwie do Skalnych Trolli Fjordtrolle nie niszczą przedmiotów wokół."
  )

N("Viking", GEN_M, "Wiking", "Wikingowie", "Wikinga", "Wikingiem")
S("Mighty warriors from the Fjord, who can use boats.",
  "Potężni wojownicy z Fiordu, używający łódek.")

N("Water Elemental", GEN_M, "Wodny Żywiołak", "Wodne Żywiołaki", "Wodnego Żywiołaka", "Wodnym Żywiołakiem")
S("Wherever this powerful being goes, the living fjord "
  "sinks below water, non-magical boats are destroyed, and fires are extinguished.\n\n"
  "As a special case, you can attack the Water Elemental from the water, without drowning immediately.",
  "Gdziekolwiek przejdzie ta potężna istota, żywy fiord tonie, niemagiczne łódki są niszczone, a "
  "ogień gaśnie.\n\n"
  "Jako szczególny przypadek, możesz atakować Wodnego Żywiołaka z wody, nie topiąc się.")


N("Garnet", GEN_O, "Granat", "Granaty", "Granat", "Granatem")
S("Vikings believe that garnets improve their strength.",
  "Wikingowie wierzą, że granaty dają im siłę.")

Orb("the Fish", "Ryby")
S("This Orb lets you dive into the water. While diving, you are able to see and collect underwater treasures, "
  "while being invisible to monsters yourself. You are afraid of jumping into the water straight from a boat, so usually you won't do it.",
  "Ta Sfera pozwala Ci nurkować. Podczas nurkowania możesz widzieć i zbierać podwodne "
  "skarby, a potwory Cię nie widzą. Boisz się wyskakiwać do wody z łódki, także raczej tego nie zrobisz.")

S("%The1 is washed away!", "%2 zmy%ł2 %a1!")
// %1 zosta%ł1 zmyt%ya1!")
S("%The1 is extinguished!", "%2 ugasi%ł2 %a1!")

// Reached the Living Fjord: Find and collect a Garnet.
// Viking: Collect 10 Garnets.
// Water Elemental: Colect 25 Garnets.
// Master of the Fjord: Colect 50 Garnets.

Orb("Discord", "Niezgody")

S("Causes most monsters to attack other monsters, not only you and your friends.",
  "Powoduje, że większość potworów będzie też atakowała inne potwory, nie tylko Ciebie "
  "i Twoich przyjaciół.")

// Shmup Configuration
// -------------------

S("forward", "do przodu")
S("backward", "do tyłu")
S("turn left", "skręć w lewo")
S("turn right", "skręć w prawo")
S("move up", "idź do góry")
S("move right", "idź w prawo")
S("move down", "idź do dołu")
S("move left", "idź w lewo")
S("throw a knife", "rzut nożem")
S("face the pointer", "celuj we wskaźnik")
S("throw at the pointer", "rzut we wskaźnik")
S("drop Dead Orb", "połóż Martwą Sferę")
S("center the map on me", "centruj mapę na mnie")
S("activate Orb power", "aktywuj moc Sfery")

S("pan up", "przewiń w górę")
S("pan right", "przewiń w prawo")
S("pan down", "przewiń w dół")
S("pan left", "przewiń w lewo")
S("rotate left", "obróć w lewo")
S("rotate right", "obróć w prawo")
S("home", "centruj na graczy")

S("do nothing", "nic")
S("rotate view", "obróć widok")
S("panning X", "przewiń X")
S("panning Y", "przewiń Y")
S("player 1 X", "gracz 1 X")
S("player 1 Y", "gracz 1 Y")
S("player 1 go", "gracz 1 idź")
S("player 1 spin", "gracz 1 obrót")
S("player 2 X", "gracz 2 X")
S("player 2 Y", "gracz 2 Y")
S("player 2 go", "gracz 2 idź")
S("player 2 spin", "gracz 2 obrót")
S("player 3 X", "gracz 3 X")
S("player 3 Y", "gracz 3 Y")
S("player 3 go", "gracz 3 idź")
S("player 3 spin", "gracz 3 obrót")
S("player 4 X", "gracz 4 X")
S("player 4 Y", "gracz 4 Y")
S("player 4 go", "gracz 4 idź")
S("player 4 spin", "gracz 4 obrót")

S("Joystick %1, axis %2", "Joystick %1, oś %2")
S("one player", "jeden gracz")
S("two players", "dwóch graczy")
S("configure player 1", "skonfiguruj gracza 1")
S("configure player 2", "skonfiguruj gracza 2")
S("configure player 3", "skonfiguruj gracza 3")
S("configure player 4", "skonfiguruj gracza 4")
S("configure panning", "skonfiguruj przewijanie")
S("configure joystick axes", "skonfiguruj osie joysticka")
S("continue playing", "kontynuuj grę")
S("start playing in the shmup mode", "zacznij grę w trybie strzelanki")
S("return to the turn-based mode", "powrót do trybu turowego")
S("save the configuration", "zapis konfiguracji")
S("press a key to unassign", "naciśnij klawisz do odwiązania")
S("press a key for '%1'", "naciśnij klawisz dla '%1'")
S("unassign a key", "odwiąż klawisz")

// extra pattern modes
// -------------------

S("explore the Emerald Pattern", "Eksploracja Szmaragdowego Wzoru")

// extra flavor/Knight lines
// -------------------------

S("Kill a Vizier in the Palace to access Emerald Mine", "Zabij Wezyra w Pałacu, by iść do Kopalni Szmaragdów")
S("Collect 5 Emeralds to access Camelot", "Zbierz 5 Szmaragdów, by dostać się do Camelot")
S("Some lands unlock at specific treasures or kills", "Niektóre krainy otwierają się dzięki konkretnym skarbom lub pokonanym potworom")

S("You will be fighting red rock snakes, too...", "Będziesz też walczyć z czerwonymi skalnymi wężami...")

S("\"The Red Rock Valley is dangerous, but beautiful.\"",
  "\"Dolina Czerwonej Skały jest piękna, choć niebezpieczna.\"")
S("\"Train in the Desert first!\"", "\"Potrenuj najpierw na Pustyni!\"")
S("\"I like collecting ambers at the beach.\"", "\"Lubię zbierać bursztyny na plaży.\"")
S("\"Our Table seats %1 Knights!\"", "\"Przy naszym stole mieści się %1 Rycerzy!\"")
S("\"There are %1 floor tiles inside our Table!\"", "\"Nasz stół otacza %1 pól podłogi!\"")
S("\"By now, you should have your own formula, you know?\"", "\"Wiesz co, m%ógł0byś już mieć swój własny wzór!\"")
S("\"Have you tried to take a boat and go into the Ocean? Try it!\"", "\"Próbowa%łeś0 wziąć łódkę i płynąć na Ocean?\"")

// VERSION 7.3
// ===========

// new name for 'Ivy D'

N("Dead Ivy", GEN_M, "Martwy Bluszcz", "Martwe Bluszcze", "Martwy Bluszcz", "Martwym Bluszczem")

// new messages for living/nonliving monsters

S("%The1 bites %the2!", "%1 ugryz%ł1 %a2!")
S("%The1 attacks %the2!", "%1 atakuje %a2!")
S("You destroy %the1 with a mental blast!", "Zniszczy%łeś0 %a1 mocą psychiczną!")
S("The ivy kills %the1!", "Bluszcz zabił %a1!")
S("You destroy %the1.", "Zniszczy%łeś0 %a1.")
S("%The1 kills %the2!", "%1 zabi%ł1 %a2!")
S("%The1 sinks!", "%1 uton%ął1!");

S("Cannot jump on %the1!", "Nie możesz skoczyć na %1!")

// renamed Fifty to Palace
S("explore the Palace Pattern (4 colors)", "Eksploracja Wzoru Pałac (4 kolory)")
S("explore the Palace Pattern (8 colors)", "Eksploracja Wzoru Pałac (8 kolorów)")

// Map Editor
// ----------

S("map editor", "edytor map")
S("You activate your terraforming powers!", "Uaktywni%łeś0 swoje moce edycji!")
S("Press F5 to clear the board.", "Wciśnij F5, by wyczyścić planszę.")
S("Press F5 to restart the game normally.", "Wciśnij F5, by zacząć normalną grę.")
S("High quality shots not available on this platform", "Zrzuty HQ niedostępne na tej platformie")
S("Saved the high quality shot to %1", "Zapisano zrzut HQ do %1")
S("Map saved to %1", "Mapa zapisana do %1")
S("Map loaded from %1", "Mapa wczytana z %1")
S("copying", "kopiuję")
S("boundary", "brzeg")
S("clear monster", "zabierz potwora")
S("use at your own risk!", "używasz na własne ryzyko!")

// Note: in English, these have been arranged from longest to shortest.
// This gives a somewhat nice effect on the top left corner of the screen.
// You may want to aim for the same effect when translating

S("0-9 = radius (%1)", "0-9 = promień (%1)")
S("b = boundary", "i = przedmioty")
S("m = monsters", "m = potwory")
S("w = walls",    "w = ściany")
S("i = items",    "l = krainy")
S("l = lands",    "c = kopia")
S("c = copy",     "b = brzeg")

// And this is from shortest to longest (bottom left corner)
S("F1 = help",                "F1 = pomoc")
S("F2 = save",                "F2 = zapisz")
S("F3 = load",                "F3 = wczytaj")
S("F5 = restart",             "F5 = restart")
S("F6 = HQ shot",             "F6 = zrzut HQ")
S("F7 = player on/off",       "F7 = gracz wł/wył")
S("ESC = return to the game", "ESC = powrót do gry")

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


  "Ten tryb pozwala edytować mapę.\n\n"
  "UWAGA: używasz na własne ryzyko! Kombinacje, które nigdy nie pojawiają się "
  "w prawdziwej grze, mogą działać w sposób nieokreślony (nie działać, wyglądać "
  "dziwnie, produkować dziwne teksty albo wywalać grę).\n\n"
  "By wynieść najwięcej z tego edytora, przydatna jest pewna wiedza o działaniu "
  "HyperRogue. Każde pole ma cztery atrybuty: typ lądu, typ ściany, typ potwora, "
  "typ przedmiotu. Ten sam typ ściany (szczególnie \"nic\", \"morze\" albo \"ognisko\")"
  " może wyglądać albo działać różnie w zależności od lądu, na którym się "
  "znajduje. Czasami obiekt może występować dwa razy na liście ze względu na "
  "subtelne różnice (na przykład, Demony mogą się ruszać w następnej kolejce albo nie).\n\n"
  "Wciśnij w, i, l, m by wybrać, który atrybut zmienić, następnie klikaj pola "
  "i będą się zmieniać. Naciśnij 'c', gdy myszka jest nad polem, by kopiować to "
  "pole -- to kopiuje całą informację o tym polu. Kiedy kopiujesz albo stawiasz "
  "potwory wielopolowe, może mieć znaczenie punkt pola, w którym klikasz.\n\n"
  "Możesz też nacisnąć 0-9 by zastosować zmianę w większym promieniu. "
  "To wpływa również na kopiowanie/wklejanie, pozwalając na skopiowanie "
  "większego obszaru.\n\n"
  "Wciśnij F2, by zapisać obecną mapę (i F3, by naładować). Jak spróbujesz "
  "to zrobić po długiej grze w HyperRogue (nie używającej Sfer Bezpieczeństwa), "
  "rozmiar pliku będzie bardzo duży! Uwaga: duże struktury, jak Wielkie Ściany "
  "i horocykle, są przez to niszczone.\n\n"
  "Naciśnij 'b' by zaznaczyć pole jako brzeg. Takie pola i pola za nimi "
  "nie są kopiowane funkcją copy/paste, ani też nie są zapisywane funkcją save.\n\n"
  )

// Princess Quest
// --------------

// Saved the Princess - "Wybawca" - "Uratuj Księżniczkę."
// Princess Challenge - "Misja Księżniczka" - "Zwycięstwo w misji Księżniczka."
// Martial Artist - "Sztuki walki" - "Pozwól nieuzbrojonej Księżniczce wygrać walkę"

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
  
  "Myszka piszczy na Ciebie. Wydaje się, że chce Cię gdzieś "
  "zaprowadzić. Ciekawe, co tam może być?\n\n"
  "W części Pałacu, którą zwiedzasz podczas tej misji, "
  "rozkład potworów, płyt i przedmiotów został trochę zmieniony, "
  "by misja była bardziej odpowiednia dla mniej doświadczonych "
  "graczy. W szczególności, liczba potworów nie zależy od liczby zebranych "
  "Hiperskich Dywanów i jest więcej płyt otwierających.\n\n"
  "Jednak nawet przy normalnym rozkładzie wydaje się, że "
  "zawsze powinno być możliwe dotarcie do celu. Jeśli chcesz "
  "takiego wyzwania, wybierz je z menu specjalnych trybów gry.")

S("\"When I visited the Palace, a mouse wanted me to go somewhere.\"",
  "\"Kiedy byłem w Pałacu, myszka chciała, bym gdzieś poszedł.\"")

S("\"I wonder what was there...\"", "\"Zastanawiam się, co tam było...\"")

N("Mouse", GEN_F, "Myszka", "Myszki", "Myszkę", "Myszką")

S("You hear a distant squeak!", "Słyszysz odległy pisk!")
S("%The1 squeaks in a confused way.", "Zmieszan%ya1 %1 pisn%ął1.")
S("%The1 squeaks gratefully!", "%1 pisn%ął1 w podziękowaniu!");
S("%The1 squeaks hopelessly.", "%1 pisn%ął1 bez nadziei.");
S("%The1 squeaks in despair.", "%1 pisn%ął1 rozpaczliwie.");
S("%The1 squeaks sadly.", "%1 pisn%ął1 smutno.");
S("%The1 squeaks with hope!", "%1 pisn%ął1 z nadzieją!");
S("%The1 squeaks happily!", "%1 pisn%ął1 szczęśliwie!");
S("%The1 squeaks excitedly!", "%1 się ekscytuje!");

N("giant rug", GEN_O, "wielki dywan", "wielkie dywany", "wielki dywan", "wielkim dywanem")

S("This is the biggest Hypersian Rug you have ever seen! "
  "Unfortunately, it is too large to take it as a trophy.",
  "To jest największy hiperski dywan, jaki kiedykolwiek widzia%łeś0! "
  "Niestety, jest za duży, by wziąć go jako zdobycz.")

N("Prince", GEN_M, "Książę", "Książęta", "Księcia", "Księciem")
N("Princess", GEN_F, "Księżniczka", "Księżniczki", "Księżniczkę", "Księżniczką")

S("%1 takes %his1 revenge on %the2!", "%1 się mści!")
S("\"That should teach you to take me seriously!\"",
  "\"Traktuj mnie poważnie! Niech to będzie dla Ciebie nauczką!\"")

S("%The1 kisses you, and begs you to bring %him1 away from here.",
  "%1 Cię pocałowa%ł1 i poprosi%ł1, byś %go1 stąd zabra%ł0.")

S("\"I want my revenge. Stun a guard and leave him for me!\"",
  "\"Chcę zemsty. Ogłusz strażnika i mi go zostaw!\"")

S("\"That felt great. Thanks!\"", "\"Wspaniałe uczucie. Dzięki!\"")

S("\"Bring me out of here please!\"", "\"Proszę, zabierz mnie stąd!\"")

S("%The1 kisses you, and thanks you for saving %him1.",
  "%1 Cię pocałowa%ł1 i podziękowa%ł1 za uratowanie %go1.")

S("\"I have been trained to fight with a Hypersian scimitar, you know?\"",
  "\"Trenowa%łem1 walkę hiperskim szamszirem, wiesz?\"")

S("\"I would love to come to your world with you!\"",
  "\"Chcia%ł1bym wrócić z Tobą do Twojego świata!\"")

S("\"Straight lines stay close to each other forever, this is so romantic!\"",
  "\"Linie proste, co mogą zawsze być blisko siebie... Jakie to romantyczne!\"")

S("\"Maps... Just like the world, but smaller... how is that even possible?!\"",
  "\"Mapy... jak świat, tylko mniejsze... jak to w ogóle możliwe?!\"")

Orb("Love", "Miłości")
S("Love takes time, but it heals all wounds, and transcends time and space.\n\n"
  "The Orb of Love is worth 30$$$, if you end the game with it.\n",

  "Miłość potrzebuje czasu, ale leczy wszelkie rany; czas i przestrzeń nie są "
  "dla niej przeszkodami.\n\n"
  "Sfera Miłości jest warta 30$$$, jeśli skończysz z nią grę.\n")

// Princess Challenge
// ------------------

S("%1 Challenge", "Misja %1")
S("Save %the1 first to unlock this challenge!", "Uratuj %a1, by mieć dostęp do tej misji!")
S("Welcome to %the1 Challenge!", "Uratuj %a1!")
S("The more Hypersian Rugs you collect, the harder it is.", "Im więcej zbierzesz Hiperskich Dywanów, tym misja trudniejsza.")
S("Follow the Mouse and escape with %the1!", "Idź za Myszką i ucieknij z %abl1!")
S("Hardness frozen at %1.", "Trudność zamrożona: %1.");
S("Congratulations! Your score is %1.", "Gratulacje! Twój wynik to %1.")

S("u = undo",     "u = cofnij")
S("f = flip %1",     "f = odbicie %1")

S("Failed to save map to %1", "Nie udało się zapisać do %1")
S("Failed to load map from %1", "Nie udało się wczytać %1")
S("save whom", "kogo ratujemy")
S("other", "inne") // other keys in the main menu

// VERSION 7.4
// ===========

// missing texts, refactored things, and rule changes
// --------------------------------------------------

S("%The1 activates her Flash spell!", "%1 rzuci%ł1 swój czar Błysk!")

N("Fire", GEN_O, "Ogień", "Ognie", "Ogień", "Ogniem")
S("This cell is on fire. Most beings and items cannot survive.",
  "To pole się pali. Większość stworzeń i przedmiotów tu nie przetrwa.")

N("Eternal Fire", GEN_O, "Wieczny Ogień", "Wieczne Ognie", "Wieczny Ogień", "Wiecznym Ogniem")
S("This fire never burns out.", "Ten ogień pali się wiecznie.")

S("Some rubble.", "Gruzy.")

S("The Orb loses its power as it leaves the Land of Power!",
  "Sfera traci swą moc, gdy opuszcza Krainę Mocy!")

S("%The1 fills the hole!", "%1 zatka%ł1 dziurę!")

N("Tentacle+Ghost", GEN_F, "Macka+Duch", "Macki+Duchy", "Mackę+Ducha", "Macką+Duchem")

// Land Overview
// -------------

S("world overview", "przegląd krain")
S("or 'o' to see the world overview", "lub 'o', by obejrzeć przegląd krain")

S("forbidden to find in %the1", "zakazana %abl1")
S("too dangerous to use in %the1", "zbyt niebezpieczna %abl1")
S("useless in %the1", "bezużyteczna %abl1")
S("only native Orbs allowed in %the1", "tylko rodzime sfery %abl1")
S("this Orb is never unlocked globally (only hubs)", "ta Sfera występuje tylko w centrach")
S("collect 25 %2 to unlock it in %the1", "znajdź 25x %2 by udostępnić %abl1")
S("collect 3 %2 to unlock it in %the1", "znajdź 3x %2 by udostępnić %abl1")
S("native to %the1 (collect 10 %2)", "rodzima %abl1 (10x %2)")
S("native to %the1 (collect 1 %2)", "rodzima %abl1 (1x %2)")
S("secondary in %the1 (collect 10 %3, or 25 %2)", "podrzędna %abl1 (10x %3, lub 25x %2)")
S("the native Orb of %the1", "rodzima sfera %abl1")
S("this Orb appears on floors and is used by witches", "ta Sfera pojawia się na ziemi i jest używana przez czarownice")
S("a prized Orb, it appears only in cabinets", "cenna Sfera, tylko w gablotach")
S("this Orb never appears in %the1", "ta Sfera nie pojawia się %abl1")
S("Hub Land: orbs appear here if unlocked in their native land", "Centrum: sfery pojawiają się tu przy 10 skarbach")
S("kill a monster, or collect 25 %2", "zabij potwora lub znajdź 25x %2")

S("Always available.\n", "Kraina zawsze dostępna.\n")
S("Treasure required: %1 $$$.\n", "Wymagane skarby: %1 $$$.\n")
S("Accessible only from %the1.\n", "Kraina dostępna tylko poprzez %a1.\n")
S("Accessible only from %the1 (until finished).\n", "Kraina dostępna tylko poprzez %a1 (przed ukończeniem).\n")

S("Accessible only from %the1 or %the2.\n", "Kraina dostępna tylko poprzez %a1 i %a2.\n")

S("Kills required: %1.\n", "Wymagani pokonani przeciwnicy: %1.\n")
S("Finished lands required: %1 (collect 10 treasure)\n",
  "Wymagane ukończone krainy: %1 (zdobądź 10 skarbów)\n")

S("Treasure required: %1 x %2.\n", "Wymagane skarby: %1 x %2.\n")

S("Alternatively: kill a %1.\n", "Alternatywnie: zabij %a1.\n")

S("Kills required: any Elemental (Living Fjord/Dead Caves).\n",
  "Wymagane zabicia: dowolny Żywiołak (Żywy Fiord/Martwa Jaskinia).")

S("Variants of %the1 are always available in the Random Pattern Mode.",
  "Warianty krainy %1 są zawsze dostępne w trybie losowych wzorków.")

S("Variants of %the1 are available in the Random Pattern Mode after "
  "getting a highscore of at least 10 %2.",
  "Warianty krainy %1 są dostępne w trybie losowych wzorków po zdobyciu "
  "10x %2.")

S(" kills: %1", " zabicia: %1")
S(" $$$: %1", " $$$: %1")
S(" Hyperstone: %1/%2", " Hiperkamienie: %1/%2")
S(" Hell: %1/9", " Piekło: %1/9")

// improved editor

S("vector graphics editor -- press F1 for help", "edytor grafiki -- wciśnij F1, by zobaczyć pomoc")
S("pics to save/load:", "obrazki do zapisu/wczytania:")
S("level to save/load:", "mapa do zapisu/wczytania:")

S("F4 = extension", "F4 = rozszerzenie")
S("Enter = back", "Enter = powrót")
S("SPACE = map/graphics", "SPACE = mapa/grafika")

S("Emerald Pattern", "Szmaragdowy Wzór")
S("Palace Pattern", "Pałacowy Wzór")
S("Zebra Pattern", "Wzór Zebra")
S("rotational symmetry", "symetria obrotowa")
S("symmetry 0-1", "symetria 0-1")
S("symmetry 0-2", "symetria 0-2")
S("symmetry 0-3", "symetria 0-3")

S("display pattern codes (full)", "pokaż kody wzorku (pełne)")
S("display pattern codes (simplified)", "pokaż kody wzorku (uproszczone)")
S("display only hexagons", "pokaż tylko sześciokąty")
S("display only heptagons", "pokaż tylko siedmiokąty")
S("predesigned patterns", "zaprojektowane wzorki")

S("Gameboard", "Plansza")
S("random colors", "losowe kolory")
S("emerald pattern", "szmaragdowy wzór")
S("four elements", "cztery żywioły")
S("eight domains", "osiem domen")
S("zebra pattern", "wzór zebry")
S("three stripes", "trzy paski")
S("random black-and-white", "losowy czarno-biały wzorek")
S("p = paint", "p = kolory")
S("r = regular", "r = wzorki")

S(
   "In this mode you can draw your own player characters, "
   "floors, monsters, and items. Press 'e' while hovering over "
   "an object to edit it. Start drawing shapes with 'n', and "
   "add extra vertices with 'a'. Press 0-9 to draw symmetric "
   "pictures easily. More complex pictures can "
   "be created by using several layers ('l'). See the edges of "
   "the screen for more keys.",

   "W tym trybie możesz narysować swoje postacie, podłogi, potwory "
   "i przedmioty. Naciśnij 'e', gdy przesuwasz mysz ponad obiektem, "
   "by go edytować. Zaczynaj nowe kształty od 'n', dodawaj wierzchołki 'a'. "
   "Naciśnij 0-9 by łatwo rysować symetryczne obrazy. Bardziej "
   "złożone obrazy mogą być tworzone przy użyciu warstw ('l'). "
   "Pozostałe klawisze są wymienione na krawędziach ekranu.")

S(
   "Press 'r' to choose a regular pattern. When a pattern is on, "
   "editing a cell automatically edits all cells which are "
   "equivalent according to this pattern. You can choose from "
   "several patterns, and choose which symmetries matter "
   "for equivalence. Also, you can press Space to switch between "
   "the map and graphics editor quickly -- note that editing floors "
   "with the graphics editor also adheres to the pattern.",

   "Wciśnij 'r', by wybrać regularny wzorek. Gdy wzorek jest włączony, "
   "edytowanie pola automatycznie edytuje także wszystkie pola, "
   "które są mu równoważne ze względu na ten wzorek. Możesz wybrać jeden "
   "z kilku wzorków i wybrać symetrie uwzględniane przy obliczaniu równoważności. "
   "Możesz też nacisnąć spację, by szybko się przenosić pomiędzy edytorem map "
   "i edytorem grafiki -- edytowanie podłóg w edytorze grafiki również "
   "następuje zgodnie ze wzorkiem.")

S("monster" ,"potwór")
S("item", "przedmiot")
S("floor", "podłoga")
S("hexagonal", "sześciokąt")
S("heptagonal", "siedmiokąt")
S("floor/pattern", "podłoga/wzorek")

S("l = layers: %1", "l = warstwy: %1")
S("1-9 = rotations: %1", "1-9 = obroty: %1")
S("0 = symmetry", "0 = symetria")
S("0 = asymmetry", "0 = asymetria")
S("%1 vertices", "wierzchołki: %1")
S("a = add v", "a = dodaj v")
S("m = move v", "m = przenieś v")
S("d = delete v", "d = skasuj v")
S("c = readd v", "c = ponownie v")
S("b = reorder", "b = kolejność")
S("t = shift", "t = przesuń")
S("y = spin", "y = obróć")

S("'n' to start", "n = start")
S("z = zoom in", "z = zbliżenie")
S("o = zoom out", "o = oddalenie")
S("g = grid", "g = siatka")
S("e = edit this", "e = edytuj to")

S("x: %1", "x: %1")
S("y: %1", "y: %1")
S("z: %1", "z: %1")
S("r: %1", "r: %1")
S("ϕ: %1°", "ϕ: %1°")

S("A fake Land with colored floors.",
  "Fałszywa Kraina z kolorowymi podłogami.")

S("random pattern mode", "tryb losowych wzorów")

// Ivory Tower
// -----------

N("Ivory Tower", GEN_F, "Wieża z Kości Słoniowej", "Wieże z Kości Słoniowej", "Wieżą z Kości Słoniowej", "na Wieży z Kości Słoniowej")

S(
  "Powerful wizards claimed this part of the world, to perform their magical "
  "experiments in peace and solitude. They have changed the direction of gravity, "
  "to make it even harder for intruders to reach them.\n\n"
  "Gravity works as follows: cells are unstable if they are empty, and there is "
  "no cell immediately below them which contains a wall. It is impossible to move "
  "from one unstable cell to another, except if moving down.",

  "Część świata zajęta przez potężnych czarodziejów, w celu prowadzenia magicznych "
  "eksperymentów w spokoju i samotności. Zmienili oni kierunek grawitacji, "
  "by intruzom było jeszcze trudniej się do nich dostać.\n\n"
  "Grawitacja działa w sposób następujący: pole jest niestabilne, jeśli jest puste i "
  "żadne pole bezpośrednio pod nim nie zawiera ściany. Nie można przejść z jednego "
  "niestabilnego pola na inne, chyba że w dół."
  )

N("Servant", GEN_M, "Sługa", "Słudzy", "Sługę", "Sługą")
S("A simple servant of the master of the Ivory Tower.",
  "Prosty sługa mistrza Wieży z Kości Słoniowej.")

N("Gargoyle", GEN_M, "Rzygacz", "Rzygacze", "Rzygacza", "Rzygaczem")
N("stone gargoyle", GEN_M, "kamienny rzygacz", "kamienne rzygacze", "kamiennego rzygacza", "kamiennym rzygaczem")
N("stone gargoyle floor", GEN_F, "podłoga z rzygacza", "podłogi z rzygacza", "podłogę z rzygacza", "podłogą z rzygacza")
N("stone gargoyle bridge", GEN_O, "most z rzygacza", "mosty z rzygacza", "most z rzygacza", "mostem z rzygacza")

S(
  "A being made of stone, who likes high buildings. It becomes normal stone when "
  "killed, but only if next to something stable -- otherwise it falls.",
  "Stworzenie wykonane z kamienia, lubi wysokie budynki. Zamienia się w normalny "
  "kamień, gdy go zabijesz, ale tylko, gdy stanie się to obok czegoś stabilnego -- "
  "w przeciwnym razie spada.")

N("Ivory Figurine", GEN_F, "Figurka", "Figurki", "Figurkę", "Figurką")

S("A beautiful figurine, made of ivory. Figurines close to the base of the Tower "
  "tend do disappear after you have collected many of them.",
  "Piękna figurka z kości słoniowej. Figurki blisko podstawy Wieży znikają, gdy "
  "zbierzesz dużo figurek.")

N("Platform", GEN_F, "Platforma", "Platformy", "Platformę", "Platformą")
N("platform", GEN_F, "platforma", "platformy", "platformę", "platformą")

S("You can stand here.", "Możesz na niej stanąć.")

N("ladder", GEN_F, "drabina", "drabiny", "drabinę", "drabiną")
S("You can use this ladder to climb the Tower.",
  "Użyj tej drabiny, by wspiąć się na Wieżę.")

Orb("Matter", "Materii")

S("This Orb allows to temporarily create impassable matter, either to block paths or "
  "to build bridges across chasms and waters.",
  "Ta sfera pozwala tymczasowo stworzyć nieprzekraczalną barierę. Można to wykorzystać "
  "do blokowania tras lub do budowy mostów nad przepaściami i wodami.")

N("temporary wall", GEN_F, "tymczasowa ściana", "tymczasowe ściany", "tymczasową ścianę", "tymczasową ścianą")
N("temporary floor", GEN_F, "tymczasowa podłoga", "tymczasowe podłogi", "tymczasową podłogę", "tymczasową podłogą")
N("temporary bridge", GEN_O, "tymczasowy most", "tymczasowe mosty", "tymczasowy most", "tymczasowym mostem")

S("This structure will disappear after some time.", "Ta konstrukcja zniknie za jakiś czas.")

S("Nothing to stand on here!", "Nie ma tu na czym stać!")
S("Gravity does not allow this!", "Grawitacja nie pozwala!")

// Elemental Planes
// ----------------

N("Elemental Planes", GEN_F, "Strefa Żywiołów", "Strefy Żywiołów", "Strefę Żywiołów", "w Strefie Żywiołów")
N("Plane of Fire", GEN_F, "Strefa Ognia", "Strefy Ognia", "Strefę Ognia", "w Strefie Ognia")
N("Plane of Earth", GEN_F, "Strefa Ziemi", "Strefy Ziemi", "Strefę Ziemi", "w Strefie Ziemi")
N("Plane of Air", GEN_F, "Strefa Powietrza", "Strefy Powietrza", "Strefę Powietrza", "w Strefie Powietrza")
N("Plane of Water", GEN_F, "Strefa Wody", "Strefy Wody", "Strefę Wody", "w Strefie Wody")

S("The Elemental Planes are divided into four subzones: Planes of Fire, Water, Air, and Earth. "
  "You need to collect a Shard from each Plane to construct an Elemental Gem. "
  "It is dangerous to collect too many Shards of the same type without constructing a Gem.",

  "Strefa Żywiołów dzieli się na cztery podstrefy: Strefa Ognia, Wody, Powietrza i Ziemi. "
  "Musisz znaleźć Okruch każdego żywiołu, by skonstruować Kamień Żywiołów. "
  "Jest niebezpiecznie zbierać wiele Okruchów tego samego żywiołu bez konstruowania Kamienia."
  )

N("Fire Elemental", GEN_M, "Ognisty Żywiołak", "Ogniste Żywiołaki", "Ognistego Żywiołaka", "Ognistym Żywiołakiem")

S("This monster leaves a trail of fire behind.",
  "Ten żywiołak zostawia za sobą ścianę ognia.")

N("Air Elemental", GEN_M, "Powietrzny Żywiołak", "Powietrzne Żywiołaki", "Powietrznego Żywiołaka", "Powietrznym Żywiołakiem")

S("An Air Elemental looks like a live tornado. Once you are three (or less) cells from it, "
  "it is impossible to move closer, due to strong winds. You can stand and wait, though. "
  "This also affects most monsters.",

  "Powietrzny Żywiołak wygląda jak żywa trąba powietrzna. Gdy znajdziesz się trzy lub "
  "mniej pól od niego, nie możesz się zbliżyć bardziej, ze względu na silny powiew. "
  "Możesz jednak stać w miejscu i czekać. Powietrzny Żywiołak wpływa także na potwory.")

N("Fire Shard", GEN_O, "Okruch Ognia", "Okruchy Ognia", "Okruch Ognia", "Okruchem Ognia")
N("Air Shard", GEN_O, "Okruch Powietrza", "Okruchy Powietrza", "Okruch Powietrza", "Okruchem Powietrza")
N("Water Shard", GEN_O, "Okruch Wody", "Okruchy Wody", "Okruch Wody", "Okruchem Wody")
N("Earth Shard", GEN_O, "Okruch Ziemi", "Okruchy Ziemi", "Okruch Ziemi", "Okruchem Ziemi")
N("Elemental Gem", GEN_O, "Kamień Żywiołów", "Kamienie Żywiołów", "Kamień Żywiołów", "Kamieniem Żywiołów")

Orb("Summoning", "Przywołania")

S(
    "This orb allows you to summon monsters. Usually, they are either Elementals or "
    "native creatures. While the monsters do not like being summoned, and will "
    "attack you once they recover from summoning sickness, such summoning "
    "often has its uses.",
  "Ta sfera pozwala Ci przywoływać potwory. Zwykle są to Żywiołaki albo stworzenia "
  "pochodzące z danej krainy. Stwory nie lubią być przywoływane i będą atakować "
  "po wyjściu ze zmieszania wywołanego przywołaniem. Ale jednak takie przywoływanie "
  "ma pewne zastosowania."
  )

N("limestone wall", GEN_F, "wapienna ściana", "wapienne ściany", "wapienną ścianę", "wapienną ścianą")

S("Simply a wall. Mostly.", "Po prostu ściana. W zasadzie.")

// Zebra
// -----

N("Zebra", GEN_F, "Zebra", "Zebry", "Zebrą", "w Zebrze")
S("Everything in this Land has black and white stripes.",
  "Wszystko w tej Krainie ma czarne i białe paski.")

N("Striped Dog", GEN_M, "Pasiasty Pies", "Pasiate Psy", "Pasiastego Psa", "Pasiastym Psem")
S("A predator native to the Zebra.", "Drapieżnik żyjący w krainie Zebry.")

N("Onyx", GEN_O, "Onyks", "Onyksy", "Onyks", "Onyksem")
S("A black gem with white stripes. It is beautiful.", "Piękny, drogocenny czarny kamień z białymi paskami.")

// Crossroads III
// --------------

N("Crossroads III", GEN_N, "Skrzyżowanie III", "Skrzyżowania III", "Skrzyżowanie III", "na Skrzyżowaniu III")

S("An alternate layout of the Crossroads. Great Walls cross here at right angles.",
  "Alternatywny układ Skrzyżowania. Wielkie Ściany przecinają się tu pod kątami prostymi.")

S("Cannot create temporary matter on a monster!", "Nie można tworzyć tymczasowej materii na potworze!");
S("Cannot create temporary matter on an item!", "Nie można tworzyć tymczasowej materii na przedmiocie!");
S("Cannot create temporary matter here!", "Nie można tu stworzyć tymczasowej materii!");
S("Cannot summon on a monster!", "Nie można przywołać na potworze!");
S("No summoning possible here!", "Przywołanie niemożliwe!");
S("You summon %the1!", "Przywoła%łeś0 %a1!")

S("F4 = file", "F4 = plik")

// VERSION 8.0
//=============

S("The Air Elemental blows you away!", "Powietrzny Żywiołak Cię zdmuchnie!")

// appended to cave troll description
S(" Additionally, all items around the killed Troll will be destroyed.",
  " Dodatkowo, wszystkie przedmioty wokół zabitego Trolla ulegają zniszczeniu.")

// 'dead cave troll' and 'dead troll' are separated now

N("dead rock troll", GEN_M, "martwy skalny troll" ,"martwe skalne trolle", "martwego skalnego trolla", "martwym skalnym trollem")

S("There are several species of trolls living in the hyperbolic world. "
  "Some of them leave this wall behind them when they die.",

  "W hiperbolicznym świecie żyje kilka gatunków trolla. Niektóre z nich "
  "zostawiają taką ścianę, gdy się je zabije.")

// paper model creator
// --------------------

S("paper model creator", "kreator modeli papierowych")
S("synchronize net and map", "synchronizuj siatkę z mapą")
S("display the scope", "pokaż zasięg")
S("create the model", "twórz model")
S("back to HyperRogue", "powrót do HyperRogue")
S("design the net", "projektuj siatkę")

S("The paper model created as papermodel-*.bmp", "Model stworzony jako papermodel-*.bmp")
S("Failed to load the file 'papermodeldata.txt'", "Nie udało się wczytać papermodeldata.txt")
S("Could not save the paper model data", "Nie udało się zapisać danych do modelu")

// pure tactics mode
// ------------------

S("pure tactics mode", "tryb taktyczny")
S("Not available in the pure tactics mode!", "Niedostępne w trybie taktycznym!")
S("\"The Knights of the Horocyclic Table salute you!\"", "\"Rycerze Horocyklicznego Stołu Cię pozdrawiają!\"")
S("press 0 to leave this mode", "Wciśnij 0, by opuścić ten tryb")
S("Collect %1x %2 to unlock", "Zbierz %1x %2 by odblokować")

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

        "W trybie taktycznym skupiasz się na jednej krainie. Twoim celem "
        "jest zdobycie jak najwyższego wyniku, bez używania elementów "
        "pozostałych krain. Możesz potem porównać wyniki ze znajomymi!\n\n"

        "Musisz sobie dobrze radzić w normalnej grze, by odblokować daną "
        "krainę tutaj (zbierz 20 skarbów w danym świecie, albo 2 w przypadku "
        "Camelot).\n\n"

        "Zdobywanie wysokich wyników w niektórych krainach jest zależne od "
        "szczęścia, zatem grasz każdą krainę N razy, a Twój wynik jest oparty "
        "na N kolejnych grach. Wartość N zależy od tego, jak 'szybko' gra się w krainie "
        "i jak bardzo jest losowa.\n\n"

        "Na Karaibach masz dostęp do Sfery Cierni, Eteru i Przestrzeni, jeżeli "
        "kiedylkolwiek zebra%łeś0 25 skarbów w ich natywnych krainach.\n\n"

        "Szybkość generowania skarbów jest stała w tym trybie, nie powiększa "
        "się wraz z zabijaniem potworów.\n\n"

        "Powodzenia i miłej zabawy!")

// Yendor Challenge
// -----------------

S("Yendor Challenge", "Misja Yendor")
S("Collect 10 treasures in various lands to unlock the challenges there",
  "Zbieraj po 10 skarbów w różnych krainach, by odblokować tutejsze misje")

S("easy", "łatwe")
S("challenge", "wyzwanie")

S("Challenges do not get harder", "Misje nie stają się trudniejsze")
S("Each challenge gets harder after each victory", "Każda misja staje się trudniejsza z każdym zwycięstwem")

S(" (won!)", " (wygrane!)")
S(" (won at level %1!)", " (wygrane na poziomie %1!)")
S("(locked)", "(zablokowane)")

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

    "Jest dużo możliwych sposobów, by zdobyć Sferę Yendoru. W tej misji "
    "spróbujesz wielu z nich!\n\n"
    "Każda misja dzieje się w konkretnej krainie i musisz użyć tego, "
    "co jest dostepne.\n\n"
    "Musisz zdobyć Sferę Yendoru w normalnej grze, by odblokować Misję Yendor "
    "i (kiedykolwiek) zdobyć 10 skarbów w jednym czy dwu światach, by uaktywnić "
    "konkretną misję.\n\n"

    "Po ukończeniu każdej misji, możesz spróbować jescze raz, na trudniejszym poziomie.\n\n"

    "Wszystkie rozwiązania użyte w Misji Yendor działają w normalnej grze. Przejścia "
    "do innych krain i (czasami) niektóre inne elementy są wyłączone podczas Misji "
    "Yendor, byś musia%ł0 użyć oczekiwanej metody. Reguły generowania krain są "
    "lekko zmienione, gdy szukasz Sfer Yendoru w Pałacu i na Polu Minowym, by "
    "poprawić równowagę (te zmiany są też aktywne przy poszukiwaniu Sfery Yendoru "
    "podczas normalnej gry).\n\n"
    )

S("Unlock this challenge by getting the Orb of Yendor!",
  "Odblokuj Misję Yendor, zdobywając Sferę Yendoru!")

S("Collect 10 treasures in various lands to unlock the challenges there",
  "Zdobywaj 10 skarbów w różnych krainach, by odblokowywać misje")

// Wild West
// ----------

N("Wild West", GEN_O, "Dziki Zachód", "Dzikie Zachody", "Dziki Zachód", "na Dzikim Zachodzie")
N("Outlaw", GEN_M, "Bandyta" ,"Bandyci", "Bandytę", "Bandytą")
N("Bounty", GEN_F, "Nagroda", "Nagrody", "Nagrodę", "Nagrodą")
N("saloon wall", GEN_F, "ściana salonu", "ściany salonu", "ścianę salonu", "ścianą salonu")
N("Revolver", GEN_O, "Rewolwer", "Rewolwery", "Rewolwer", "Rewolwerem")

S("%The1 shoots you!", "%1 do Ciebie strzeli%ł!")
S("You shoot %the1!", "Zastrzeli%łeś0 %a1!")

S(
  "Take a revolver, kill outlaws, collect bounties.\n\n"
  "Note: since this land is anachronistic, it is not available in normal game. "
  "It is only available in special modes.",

  "Weź rewolwer, zabijaj bandytów i zgarniaj nagrody!\n\n"
  "Uwaga: ta kraina jest anachronistyczna, także nie jest dostępna w normalnej "
  "grze. Jest za to dostępna w specjalnych trybach.")

// Land of Storms
// ---------------

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

  "Za każdym razem, gdy po Twoim ruchu jest połączenie między naładowanym a "
  "uziemionym polem, następuje zwarcie. Wszystkie pola na dowolnej ścieżce "
  "łączące pole naładowane z uziemionym (bez powtórzonych pól i bez "
  "dwóch sąsiednich naładowanych/uziemionych komórek) zostają uszkodzone.\n\n"

  "Piaskowe ściany i większość potworów przewodzi prąd. Wielkie ściany "
  "są izolatorami, ale krainy za nimi liczą się jako uziemione.\n\n"

  "Fulguryt to skarb, który powstaje, gdy zwarcie zniszczy piaskową ścianę "
  "albo Bogatego Stwora.\n\n"

  "Zabite trolle pozostawiają przewodzące ściany. Metalowe i Bogate Stwory można "
  "zabić tylko elektrycznością -- Twoje ataki je ogłuszają albo odpychają, "
  "jeśli są już ogłuszone.")

N("Land of Storms", GEN_F, "Kraina Burzy", "Krainy Burzy", "Krainę Burzy", "w Krainie Burzy")
N("charged wall", GEN_F, "naładowana ściana", "naładoawne ściany", "naładowaną scianę", "naładowaną ścianą")
N("grounded wall", GEN_F, "uziemiona ściana", "uziemione ściany", "uziemioną scianę", "uziemioną ścianą")
N("metal wall", GEN_F, "metalowa ściana", "metalowe ściany", "metalową scianę", "metalową ścianą")
N("sandstone wall", GEN_F, "piaskowa ściana", "piaskowe ściany", "piaskową scianę", "piaskową ścianą")
N("Storm Troll", GEN_M, "Burzowy Troll", "Burzowe Trolle", "Burzowego Trolla", "Burzowym Trollem")
N("Metal Beast", GEN_O, "Metalowy Stwór", "Metalowe Stwory", "Metalowego Stwora", "Metalowym Stworem")
N("Rich Metal Beast", GEN_O, "Bogaty Stwór", "Bogate Stwory", "Bogatego Stwora", "Bogatym Stworem")
N("electric discharge", GEN_N, "wyładowanie", "wyładowania", "wyładowanie", "wyładowaniem")

S("There is a flash of thunder!", "Błyskawica!")

Orb("Stunning", "Ogłuszania")

S("This Orb allows you to target monsters to stun them. "
  "10 charges are used to stun for 5 turns. Does not "
  "work against multi-tile monsters.",

  "Ta sfera pozwala ogłuszać potwory na odległość. "
  "10 ładunków = ogłuszenie na 5 tur. Nie działa "
  "przeciwko potworom zajmującym wiele pól."
  )

S("You stun %the1!", "Ogłuszy%łeś0 %a1!")

// Overgrown Woods
// ----------------

Orb("Luck", "Szczęścia")

S("This Orb allows you to find new lands more easily. "
  "Lands where you have already collected less treasure, "
  "and especially the Crossroads, are more likely to "
  "spawn while you have this. Additionally, Orbs of Safety "
  "are more likely to spawn in the Whirlpool.",

  "Ta sfera pozwala łatwo znajdować nowe krainy. "
  "Gdy ją masz, łatwiej znaleźć jest krainy, w których "
  "zebrane zostało mniej skarbów, w szczególności Skrzyżowania. "
  "Dodatkowo, Sfery Bezpieczeństwa częściej "
  "trafiają się w Wirze."
  )

N("Overgrown Woods", GEN_O, "Przerośnięty Las", "Przerośnięte Lasy", "Przerośnięty Las", "w Przerośniętym Lesie")
N("Mutant Ivy", GEN_O, "Zmutowany Bluszcz", "Zmutowane Bluszcze", "Zmutowany Bluszcz", "Zmutowanym Bluszczem")
N("Mutant Sapling", GEN_F, "Zmutowana Sadzonka", "Zmutowane Sadzonki", "Zmutowaną Sadzonkę", "Zmutowaną Sadzonką")
N("Forest Troll", GEN_M, "Leśny Troll", "Leśne Trolle", "Leśnego Trolla", "Leśnym Trollem")

S("Forest Trolls create an impassable wall when they die.",
  "Zabite Leśne Trolle pozostawiają ścianę.")

S(
  "The Overgrown Woods are filled with mutant ivies! These plants "
  "grow very fast. Each leaf, after being grown, can grow itself "
  "on the next turn. However, each part is only able to grow "
  "once in 16 turns. Outside of the Overgrown Woods, the Mutant Ivy "
  "may grow only on hexagonal cells.\n\n"
  "Maybe such fast growing plants could help you solve the problem "
  "of hunger in your world? Kill the Mutant Ivies to collect Mutant Saplings.",

  "Przerośnięty Las jest pełen zmutowanego bluszczu! Te rośliny rosną "
  "bardzo szybko. Każdy liść może sam rosnąć w następnej turze po "
  "wyrośnięciu, ale każda część rośliny może rosnąć tylko raz na 16 tur. "
  "Poza Przerośniętym Lasem Zmutowany Bluszcz może rosnąć tylko na sześciokątach.\n\n"
  "Może tak szybko rosnące rośliny pozwoliłyby Ci rozwiązać problem "
  "głodu w Twoim świecie? Zabijaj Zmutowane Bluszcze, by zbierać sadzonki."
  )

S("Trees in this forest can be chopped down. Big trees take two turns to chop down.",
  "Drzewa w tym lesie można ścinać. Ścięcie dużego drzewa zajmuje dwie kolejki."
  )

S("Your power is drained by %the1!", "Twoja moc jest wyssana przez %a1!")

S("Wow! That was close.", "Uff! Było blisko.");

S("Collect four different Elemental Shards!", "Znajdź Okruch każdego żywiołu!")
S("Unbalanced shards in your inventory are dangerous.",
  "Niebezpiecznie jest nosić niezrównoważone Okruchy.")
S("You construct some Elemental Gems!", "Skonstruowa%łeś0 Kamień Zywiołów!")

N("Fulgurite", GEN_O, "Fulguryt", "Fulguryty", "Fulguryt", "Fulgurytem")

S("Alternatively: kill a %1 in %the2.\n", "Alternatywnie: zabij %a1 %abl2.\n")

// VERSION 8.1
// ============

// extra config
S("Prince", "Książę")
S("Princess", "Księżniczka")
S("cat" ,"kot")
S("dog", "pies")
S("dress color II", "kolor sukienki II")
S("character", "postać")

//missing texts
S("%The1 hits %the2.", "%1 uderzy%ł1 %a2.")
S("%The1 is destroyed by the Flash.", "Błysk zniszczył %a1.")

// improved kill descriptions
S("parts destroyed: %1", "części zniszczone: %1")
S("friends killed: %1", "przyjaciele zabici: %1")
S("monsters destroyed: %1", "potwory zniszczone: %1")
S("friends destroyed: %1", "przyjaciele zniszczeni: %1")
S("\n\nThis is a part of a monster. It does not count for your total kills.",
  "\n\nTo jest część potwora. Nie liczy się do łącznych zabić.")
S("\n\nThis is a friendly being. It does not count for your total kills.",
  "\n\nTo jest przyjazne stworzenie. Nie liczy się do łącznych zabić.")

// Overgrown Clearing
// -------------------

S("A clearing in the Overgrown Woods. Obviously, this gives "
  "the Mutant Ivies an infinite space to grow...\n\n"
  "Mutant Fruits rot if they are not adjacent to a Mutant Ivy.",

  "Polana w Przerośniętym Lesie. Oczywiście, pozwala "
  "Zmutowanym Bluszczom rosnąć w nieskończoność...\n\n"
  "Zmutowane Owoce gniją, gdy nie sąsiadują ze Zmutowanym Bluszczem.")

N("Clearing", GEN_F, "Polana", "Polany", "Polanę", "na Polanie")

N("Mutant Fruit", GEN_O, "Zmutowany Owoc", "Zmutowane Owoce", "Zmutowany Owoc", "Zmutowanym Owocem")

N("Giant Fox", GEN_O, "Wielki Lis", "Wielkie Lisy", "Wielkiego Lisa", "Wielkim Lisem")

S(
    "What is freedom for you? A situation when you can walk wherever you want? "
    "Or a situation when you do not have to work, since you have as much tasty food "
    "as you want?\n\n"
    "Well, this creature has chosen the second option. It won't be happy "
    "if you destroy its prison.\n",

    "Czym jest dla Ciebie wolność? Sytuacją, gdy możesz chodzić, gdzie chcesz? "
    "Czy sytuacją, gdy nie musisz pracować, bo masz tyle jedzenia, ile zapragniesz?\n\n"
    "To stworzenie wybrało drugą opcję. Nie będzie szczęśliwe, jak je uwolnisz "
    "z więzienia."
    )

Orb("Freedom", "Wolności")

S(
    "This orb is activated if you are unable to escape (radius 4) "
    "without making illegal moves or "
    "going through cells which are currently adjacent to enemy monsters. "
    "Most game over situations are covered by this, but generally, "
    "this orb is oversensitive...\n\n"
    "When activated, it creates a Flash effect of radius 5.",

    "Ta sfera aktywuje się, gdy nie możesz uciec (promień 4) bez "
    "wykonywania nielegalnych ruchów lub przechodzenia przez komórki "
    "nie sąsiadujące z przeciwnikami. Pokrywa większość sytuacji 'koniec gry', "
    "ale zazwyczaj ta sfera jest nadwrażliwa...\n\n"
    "Aktywacja powoduje efekt czaru Błysk w promieniu 5."
  )

S("Your %1 activates!", "Uruchomi%ł1 się Twoja %1!")

// Haunted Woods
// --------------

S("You become a bit nervous...", "Czujesz się zagrożon%y0...")
S("Better not to let your greed make you stray from your path.",
  "Nie pozwól, by zgubiła Cię chciwość!")

N("Haunted Woods", GEN_O, "Nawiedzony Las", "Nawiedzone Lasy", "Nawiedzony Las", "w Nawiedzonym Lesie")

S(
  "A dark forest filled with ghosts and graves. But there is also treasure hidden "
  "deep within... But don't let greed make you stray from your path, as "
  "you can get lost!\n\n"
  "The Haunted Woods are bounded by a single equidistant curve. It is not a circle or horocycle.\n\n",

  "Mroczny las, pełen duchów i grobów. Głęboko w środku są również skarby... "
  "Ale nie pozwól, by zgubiła Cię chciwość!\n\n"
  "Nawiedzony Las jest ograniczony jedną ekwidystantą. Nie jest kołem ani horocyklem.\n\n")


N("Black Lotus", GEN_O, "Czarny Lotos", "Czarne Lotosy", "Czarny Lotos", "Czarnym Lotosem")

S(
    "This beautiful flower is greatly prized by wizards, as it allows them to cast powerful magical spells "
    "without preparation.\n",

    "Ten piękny kwiat ma wysoką cenę wśród czarodziejów, bo pozwala im rzucać "
    "silne czary bez przygotowania.\n"
    )

Orb("Undeath", "Nieumarłych")

S(
    "Monsters slain by you in melee are turned into friendly ghosts. "
    "Does not affect plants and friends.",

    "Potwory zabite przez Ciebie wręcz są zamieniane w przyjazne duchy. "
    "Nie wpływa na rośliny i przyjaciół."
    )

N("Friendly Ghost", GEN_O, "Przyjazny Duch", "Przyjazne Duchy", "Przyjaznego Ducha", "Przyjaznym Duchem")

S("Friendly ghosts are friendly beings who can go through any obstacles. However, "
  "unlike most friends, they tend to fly away from you.",

  "Przyjazne duchy mogą przechodzić przez wszelkie przeszkody. Jednak, w przeciwieństwie "
  "do większości pozostałych przyjaciół, mają zwyczaj odlatywać od Ciebie."
   )

// Windy Plains
// -------------

N("Windy Plains", GEN_F, "Wietrzna Równina", "Wietrzne Równiny", "Wietrzną Równinę", "na Wietrznej Równinie")

N("fan", GEN_O, "Wentylator", "Wentylatory", "Wentylator", "Wentylatorem")

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

  "Ktoś porozstawiał wentylatory na tych równinach, powodując wszędzie silne wiatry. "
  "Celem jest zapewne uzyskanie magicznych mocy Powietrznych Żywiołaków, ale nie "
  "masz pewności.\n\n"

  "Wszystkie komórki poza wentylatorami są podzielone na trzy kolory, zgodnie z "
  "regularnym wzorem. Wiatr wieje przeciwnie do ruchu wskazówek zegara wokół "
  "każdej grupy pól jednego koloru. Pola zablokowane przez ściany lub "
  "w odległości co najwyżej 2 od Powietrznego Żywiołaka, nie liczą się tutaj.\n\n"
  "Nie można ruszać się w kierunku który jest bliżej przychodzącego niż odchodzącego "
  "wiatru. Możesz jednak się ruszyć o dwa pola z wiatrem w jednej kolejce. "
  "Ptaki również tak mogą zrobić."
  )

N("White Dove Feather", GEN_N, "Gołębie Pióro", "Gołębie Pióra", "Gołębie Pióro", "Gołębim Piórem")
S("This feather is truly beautiful and strong.",
  "Pióro białego gołębia. Piękne i silne.")

N("Wind Crow", GEN_F, "Wietrzna Wrona", "Wietrzne Wrony", "Wietrzną Wronę", "Wietrzną Wroną")
S("A large bird who likes strong winds. Just as you, it can fly quickly in the wind.",
  "Wielki ptak, lubiący silne wiatry. Tak jak Ty może szybko latać z wiatrem."
  )

// Orb of Empathy replaces Orb of Air on the Beach
Orb("Empathy", "Empatii")
S(" %1", " %1")
S(", %1", ", %1")

S(  "This Orb lets your allies to share your Orb powers.\n\n"
    "The following Orbs are affected:",

    "Ta sfera pozwala Ci dzielić się mocami Sfer z przyjaciółmi.\n\n"
    "Następujące Sfery są dzielone:")

N("strong wind", GEN_O, "silny wiatr", "silne wiatry", "silny wiatr", "silnym wiatrem")

S(
    "In the Windy Plains, you can let the wind carry you, "
    "causing you to move two cells with the wind in a single turn. "
    "This cannot be done if you are standing at distance at most 2 "
    "from the Air Elemental, or if any of the three cells on the way "
    "has two wind directions.\n\n"
    "Press 't' or click the destination to activate.",

    "Na Wietrznej Równinie możesz pozwolić dać się nieść wiatrowi, "
    "dzięki czemu ruszasz się 2 pola z wiatrem w jednej kolejce. "
    "Jest to możliwe tylko, jeśli nie stoisz w odległości co najwyżej "
    "2 od Powietrznego Żywiołaka i żadne z trzech pól po drodze "
    "nie ma wiatru w dwóch kierunkach.\n\n"
    "Naciśnij 't' albo kliknij pole docelowe, by wykonać ten ruch."
  )

S("Return to the normal game", "Powrót do normalnej gry")
S("three players", "trzech graczy")
S("four players", "czterech graczy")
S("key Orb power", "moc sfery jednym klawiszem")

S("Orb power (target: mouse)", "moc sfery (cel: mysz)")
S("Orb power (target: facing)", "moc sfery (cel: na wprost)")
S("You cannot go against the wind!", "Nie możesz iść pod wiatr!")

// VERSION 8.2
// ============

S("The ivy attacks %the1!", "Bluszcz zaatakował %a1!")

// heptagonal mode
// ----------------

S("heptagonal mode", "tryb siedmiokątów")

S("\n\n(For the heptagonal mode, the radius has been reduced to 2 for closing plates.)",
  "\n\n(W trybie siedmiokątów promień działania dla płyt zamykających jest zmniejszony do 2.)")

// Hypersian Rug mode
// -------------------

S("hypersian rug mode", "tryb hiperskiego dywanu")

S(
  "In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
  "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n"
  "This requires some OpenGL extensions and may crash or not work correctly -- enabling "
  "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
  "will make the rendering faster, but the surface will be rendered only once, so "
  "you won't be able to play a game on it.\n\n"
  "Use arrow keys to rotate, Page Up/Down to zoom.",

  "W tym trybie można grać w HyperRogue na trójwymiarowym modelu części płaszczyzny "
  "hiperbolicznej, podobnym do tego, który można otrzymać w kreatorze modeli "
  "papierowych albo poprzez hiperboliczne szydełkowanie.\n\n"
  "Działanie wymaga pewnych rozszerzeń OpenGL i może działać nieprawidłowo lub "
  "powodować zatrzymanie gry -- w takim przypadku może być przydatne włączenie opcji "
  "'renderuj bez OpenGL'. Opcja 'renderuj raz' spowoduje, że animacja będzie szybsza, "
  "ale tekstura powierzchni zostanie narysowana tylko raz, zatem nie będzie możliwe "
  "granie na niej na żywo.\n\nMożesz obracać model strzałkami, a przybliżać i oddalać go "
  "klawiszami Page Up/Down.")

S("what's this?", "co to jest?")
S("take me back", "powrót do menu")
S("enable the Hypersian Rug mode", "włącz tryb hiperskiego dywanu")
S("render the texture only once", "renderuj teksturę tylko raz")
S("render texture without OpenGL", "renderuj teksturę bez OpenGL")
S("texture size", "rozmiar tekstury")

// Crossroads IV & Chaos Mode
// ---------------------------

N("Crossroads IV", GEN_O, "Skrzyżowanie IV", "Skrzyżowania IV", "Skrzyżowanie IV", "na Skrzyżowaniu IV")

S("Chaos mode", "tryb Chaosu")

S("In the Chaos mode, lands change very often, and there are no walls between them. "
  "Some lands are incompatible with this."
  "\n\nYou need to reach Crossroads IV to unlock the Chaos mode."
  ,

  "W trybie Chaosu krainy zmieniają się bardzo często i nie ma między nimi "
  "ścian. Niektóre krainy są niekompatybilne z tym trybem."
  "\n\nMusisz dotrzeć do Skrzyżowania IV, by odblokować tryb Chaosu."
)

// Rose Garden
// ------------

N("Rose Garden", GEN_O, "Różany Ogród", "Różane Ogrody", "Różany Ogród", "w Różanym Ogrodzie")
N("False Princess", GEN_F, "Fałszywa Księżniczka", "Fałszywe Księżniczki", "Fałszywą Księżniczkę", "Fałszywą Księżniczką")
N("False Prince", GEN_M, "Fałszywy Książę", "Fałszywi Książęta", "Fałszywego Księcia", "Fałszywym Księciem")
N("Rose Lady", GEN_F, "Różana Dama", "Różane Damy", "Różaną Damę", "Różaną Damą")
N("Rose Lord", GEN_M, "Różany Pan", "Różani Panowie", "Różanego Pana", "Różanym Panem")
N("Rose Beauty", GEN_F, "Różana Piękność", "Różane Piękności", "Różaną Piękność", "Różaną Piękność")
N("Handsome Gardener", GEN_M, "Przystojny Ogrodnik", "Przystojni Ogrodnicy", "Przystojnego Ogrodnika", "Przystojnym Ogrodnikiem")
N("rosebush", GEN_O, "krzak róży", "krzaki róży", "krzak róży", "krzakiem róży")

Orb("Beauty", "Piękna")
N("Thornless Rose", GEN_F, "Róża bez Kolców", "Róże bez Kolców", "Różę bez Kolców", "Różą bez Kolców")

S("Don't be fooled by this red-haired girl, or you will be stabbed if you come too close!",
  "Nie daj się zwieść tej rudowłosej, bo jak się za bardzo zbliżysz, możesz zostać ugodzon%ya1 nożem!")
S("Don't be fooled by this red-haired boy, or you will be stabbed if you come too close!",
  "Nie daj się zwieść temu rudowłosemu, bo jak się za bardzo zbliżysz, możesz zostać ugodzon%ya1 nożem!")
S("This false princess is immune to the alluring scent of roses.",
  "Ta fałszywa księżniczka jest niewrażliwa na zapach róż.")
S("This false prince is immune to the alluring scent of roses.",
  "Ten fałszywy książę jest niewrażliwy na zapach róż.")
S("She has flowers in her long fair hair. You could not bring yourself to attack such a beautiful woman.",
  "Ta ślicznotka ma kwiaty w swych jasnych, długich lokach. Nie jesteś w stanie zaatakować tak pięknej kobiety.")
S("Tall, strong, and holding a flower in his hand. You could not bring yourself to attack such a handsome man.",
  "Wysoki, silny, z kwiatem w dłoni. Nie jesteś w stanie zaatakować tak przystojnego mężczyzny.")

S(  "This Orb makes you stunningly beautiful. "
    "Monsters which come next to you will be stunned for one turn. "
    "Multi-tile monsters are not affected. Additionally, it makes you immune to "
    "beauty.",

    "Dzięki tej Sferze stajesz się oszałamiająco piękn%y0. "
    "Stworzenia, które wejdą na pole obok Ciebie, zostaną oszołomione na 1 kolejkę. "
    "Nie działa to na stworzenia zajmujące wiele pól. Dodatkowo, stajesz się "
    "niewrażliw%y0 na piękno.")

S(  "A big, beautiful, magical flower.", "Duży, piękny, magiczny kwiat.")

S("Don't be fooled by beauty, or you will be stabbed if you come too close!",
  "Nie daj się zwieść urodzie, bo jak się za bardzo zbliżysz, możesz zostać ugodzon%y0 nożem!")


S(
  "Each eight turns, each rosebush at distance at most 5 from you will "
  "release a wave of alluring scent. Creatures on the frontwave "
  "will move towards where the scent came from. Even if it causes them "
  "to attack their friends or beautiful creatures, or move into water, fire, chasm, or thorns of the rosebush. "
  "Ivies, Ghosts, Rock Snakes, Rose Ladies and Lords, and monsters restricted to a specific "
  "terrain are immune to scents.",

  "Co 8 kolejek każdy krzak róży w promieniu 5 od Ciebie wysyła falę "
  "uwodzicielskiego zapachu. Stworzenia na czole fali wykonają ruch "
  "w kierunku źródła zapachu, nawet jeżeli by to spowodowało, że "
  "zaatakują swoich przyjaciół lub piękne istoty, wejdą w wodę, ogień, przepaść, czy "
  "wpadną na kolce róży. Bluszcz, duchy, skalne węże, Różane Damy i Panowie, a także potwory "
  "żyjące w konkretnym terenie są odporne na zapachy.")

S(
  "This land is filled with beautiful, but dangerous, creatures and plants.",

  "Ta kraina pełna jest pięknych, lecz niebezpiecznych, stworzeń i roślin.")

S("%The1 is killed by thorns!", "%1 wpad%ł1 na kolce i się zabi%ł1!");

S("You just cannot stand in place, those roses smell too nicely.", "Nie możesz stać w miejscu, róże zbyt ładnie pachną.")
S("Those roses smell too nicely. You have to come towards them.", "Te róże tak ładnie pachną. Musisz iść w ich kierunku.")

// Warped Sea/Coast
// -----------------

N("Warped Coast", GEN_O, "Zakrzywiony Brzeg", "Zakrzywiony Brzegi", "Zakrzywiony Brzeg", "na Zakrzywionym Brzegu")
N("Warped Sea", GEN_F, "Zakrzywione Morze", "Zakrzywione Morza", "Zakrzywione Morze", "na Zakrzywionym Morzu")
N("Ratling", GEN_M, "Szczurak", "Szczuraki", "Szczuraka", "Szczurakiem")
N("Coral", GEN_O, "Koral", "Korale", "Koral", "Koralem")
Orb("the Warp", "Zakrzywienia")

S(
  "This land is warped. Ironically, the coast is completely straight...",
  "Ta kraina jest dziwnie zakrzywiona. Co ciekawe, brzeg jest prosty jak drut...")

S(
  "This part of the world is warped, restricting the movement somewhat. "
  "\"Diagonal\" movement and attacking between triangular cells is not allowed. "
  "Flash, Storms, and Freedom spells ignore this, and Ghosts can move, attack, and "
  "be attacked diagonally.",
  "Ta część świata jest zakrzywiona, co powoduje zmniejszenie możliwości ruchu. "
  "Ruch i atakowanie \"po przekątnej\", czyli między trójkątami, są zabronione. "
  "Czary Błysk, Burza i Wolność ignorują to. Duchy mogą się ruszać, atakować "
  "i być atakowane po przekątnej.")

S( "This Orb creates a warped zone of radius 5 around you, "
  "and also allows you to move diagonally in warped zones.",

  "Ta sfera tworzy wokół Ciebie w promieniu zakrzywioną przestrzeń, "
  "a także pozwala Ci się poruszać po przekątnej w zakrzywionej przestrzeni.")

S(  "Corals have a somewhat hyperbolic structure even in your home world, "
    "but natural corals from the Warped Sea have truly beautiful shapes. "
    "Ratlings know the value of corals, and thus keep them in boats for safety.",

    "Korale mają hiperboliczną strukturę nawet w świecie, z którego pochodzisz, "
    "ale naturalne korale z Zakrzywionego Morza mają prawdziwie piękne kształty. "
    "Szczuraki znają wartość korali i dla bezpieczeństwa trzymają je na łódkach.")

S(
    "These warped humanoids are skilled warriors and sailors, and they "
    "feel at home at the Warped Coast. Their battle experience has taught them "
    "that enemies who wait without moving or attacking anything are the most deadly. "
    "If they see such an enemy, they become extremely suspicious, and they also wait.",

    "Te człekokształtne stworzenia są zręczne na morzu i w walce, a na Zakrzywionym "
    "Brzegu czują się jak w domu. Ich doświadczenie bitewne nauczyło je, że ci przeciwnicy, "
    "którzy czekają bez ruchu i bez ataku, są najbardziej niebezpieczni. Kiedy "
    "widzą takiego przeciwnika, stają się bardzo podejrzliwe i również czekają.")


S("hexagonal #1", "sześciokąt #1")
S("You cannot move between the triangular cells here!", "Nie możesz przechodzić między trójkątami!")
S("display the triheptagonal grid", "pokaż siatkę triheptagonalną")

N("no wall", GEN_O, "brak ściany", "braki ściany", "braku ściany", "brakiem ściany")
N("no monster", GEN_O, "brak potwora", "braki potwora", "braku potwora", "brakiem potwora")
N("no item", GEN_O, "brak przedmiotu", "braki przedmiotu", "braku przedmiotu", "brakiem przedmiotu")

S("\n\nA Ghost never moves to a cell which is adjacent to another Ghost of the same kind.",
  "\n\nDuch nigdy nie wchodzi na pole sąsiadujące z innym Duchem tego samego typu.")

S("You cannot attack diagonally!", "Nie możesz atakować po przekątnej!")

Orb("Energy", "Energii")

S(  "This Orb halves the power usage of orbs which cost some "
    "charges with each activation. It even affects the "
    "one-shot orbs such as Flash or Teleport. If such an activation "
    "normally costs x charges, it costs only x/2 (rounded up) "
    "if you have an Orb of Energy.",

    "Ta Sfera dwukrotnie zmniejsza zużycie mocy przez te Sfery, których "
    "każda aktywacja kosztuje pewną liczbę ładunków. Działa nawet na "
    "Sfery jednokrotnego użytku jak Błysk czy Teleport. Jeśli normalnie "
    "taka aktywacja kosztuje x ładunków, a masz Sferę Energii, to "
    "będzie ona kosztowała x/2 ładunków (zaokrąglone w górę).")

// missing....

N("warp gate", GEN_F, "wrota zakrzywienia", "wrota zakrzywienia", "wrota zakrzywienia", "wrotami zakrzywienia")
S("This gate separates the warped area from the normal land.",
  "Ta brama oddziela strefę zakrzywioną od normalnych krain.")
S("You cannot move between the cells without dots here!",
  "Nie możesz poruszać się między polami bez kropek!")
S("Those roses smell too nicely. You can only target cells closer to them!",
  "Te róże tak ładnie pachną. Możesz celować tylko w pola bliżej nich!")
S("You need to move to give space to %the1!",
  "Musisz się ruszyć, by zrobić miejsce dla %a1!")
// this should be genetive, but in Polish it's the same as %a1 for both Golem and Tame Bomberbird

N("Ratling Avenger", GEN_M, "Szczurak-Mściciel", "Szczuraki-Mściciele", "Szczuraka-Mściciela", "Szczurakiem-Mścicielem")

S(
  "So, you have killed a Ratling on the unwarped sea? You will be punished for this! "
  "Luckily, if you run away from the Warped Sea quickly, the Ratling Avengers will lose track of you.",

  "Zabi%łeś0 Szczuraka na niezakrzywionym morzu? Mściciele Cię dosięgną! "
  "Na szczęście, jeśli szybko uciekniesz z Zakrzywionego Morza, Szczuraki-Mściciele zgubią Twój ślad.")

// VERSION 8.3
// ============

S("Kills required: %1 (%2).\n", "Wymagani pokonani przeciwnicy: %1 (%2).\n")

S("\"Be careful in the Rose Garden! It is beautiful, but very dangerous!\"",
  "\"Uważaj w Różanym Ogrodzie! Jest tam pięknie, ale bardzo niebezpiecznie!\"")

S("\"Thank you very much for talking, and have a great rest of your day!\"",
  "\"Dziękuję bardzo za rozmowę, życzę wspaniałej reszty dnia!\"")

// Conformal/history mode
// ----------------------

S("conformal/history mode", "tryb konforemny/historii")
S("Preparing the line (%1/1000)...", "Przygotowanie linii (%1/1000)...")
S("Could not create an image of that size.", "Nie da się stworzyć obrazu tej wielkości.")
S("Saved the band image as: ", "Zapisano obraz jako: ")
S("right", "prawo")
S("left", "lewo")
S("up", "góra")
S("down", "dół")
S("counterclockwise", "antyzegarowo")
S("clockwise", "zegarowo")
S("zoom out", "oddalenie")
S("zoom in", "zbliżenie")
S("left to right", "w prawo")
S("right to left", "w lewo")
S("spin down", "obróć w dół")
S("spin up", "obróć w górę")
S("disk", "koło")
S("half-plane", "półpłaszczyzna")
S("band", "wstęga")
S("polygonal", "wielokąt")
S("polynomial", "wielomian")
S("include history", "uwzględnij historię")
S("model used", "użyty model")
S("rotation", "obrót")

S("coefficient", "współczynnik")
S("which coefficient", "który współczynnik")
S("polygon sides", "boki wielokąta")
S("star factor", "współczynnik gwiazdki")
S("degree of the approximation", "stopień przybliżenia")
S("prepare the line animation", "przygotuj animację linii")
S("animation speed", "prędkość animacji")
S("render bands automatically", "automatycznie twórz wstęgi")
S("band width", "szerokość wstęgi")
S("length of a segment", "długość segmentu")
S("spiral on rendering", "spirala przy renderowaniu")
S("render now (length: %1)", "rysuj teraz (długość: %1)")
S("exit this menu", "wyjdź")
S("Enable cheat mode or GAME OVER to use this", "Dostępne tylko po końcu gry albo w trybie oszusta")

// remember to note it's in English
S("see http://www.roguetemple.com/z/hyper/conformal.php", "patrz http://www.roguetemple.com/z/hyper/conformal.php (po angielsku)")

// Yendorian Forest
// ----------------

N("Yendorian Forest", GEN_O, "Las Yendoriański", "Lasy Yendoriańskie", "Yendoriańskiego Lasu", "w Yendoriańskim Lesie")

S(
  "This forest was planted by one of the wizards from the Ivory Tower "
  "to conduct experiments with gravity.",

  "Ten las został zasadzony przez jednego z czarodziejów z Wieży z Kości Słoniowej, "
  "by przeprowadzać eksperymenty z grawitacją.")

N("Yendorian Researcher", GEN_M, "Yendoriański Badacz", "Yendoriańscy Badacze", "Yendoriańskiego Badacza", "Yendoriańskim Badaczem")

S(
    "These people study gravity and infinite trees. "
    "They have no special features, other than wearing a strange hat.",

  "Ci ludzie badają grawitację i drzewa nieskończone. Nie mają "
  "szczególnych własności poza swoim dziwnym nakryciem głowy.")

N("Sparrowhawk", GEN_M, "Krogulec", "Krogulce", "Krogulca", "Krogulcem")

S("A bird who hunts in the treetops of the Yendorian Forest.",
  "Ptak polujący w koronach drzew w lesie yendoriańskim.")

N("Apple", GEN_N, "Jabłko", "Jabłka", "Jabłko", "Jabłkiem")

S("A fruit from the Yendorian Forest.", "Owoc z Yendoriańskiego Lasu.")

N("trunk", GEN_O, "pień", "pnie", "pień", "pniem")

S("The skeleton of a tree.", "Szkielet drzewa.")

N("solid branch", GEN_F, "solidna gałąź", "solidne gałęzie", "solidną gałąź", "solidną gałęzią")

S("Branches here could bear your weight easily.",
  "Te gałęzie z łatwością udźwigną Twój ciężar.")

N("weak branch", GEN_F, "słaba gałąź", "słabe gałęzie", "słabą gałąź", "słabą gałęzią")

S("Branches here will bear you weight, but if you use them to move (not fall) to an unstable place, they will break.",
  "Te gąłęzie udźwigną Twój ciężar, ale jeśli użyjesz ich, by się przemieścić (nie spaść) na niestabilną pozycję, złamią się.")

N("canopy", GEN_N, "listowie", "listowia", "listowie", "listowiem")

S(
  "Only thin twigs and leaves here. They may bear fruits, but for you, these cells count as unstable.",
  "Tu są jedynie cieniutkie gałązki i liście. Mogą tu rosnąć owoce, ale dla Ciebie te pola liczą się jako niestabilne.")

// Dragon Chasm
// ------------

N("Dragon Chasms", GEN_F, "Smocze Otchłanie", "Smocze Otchłanie", "Smocze Otchłanie", "w Smoczych Otchłaniach")

N("Dragon", GEN_M, "Smok", "Smoki", "Smoka", "Smokiem")

N("Dragon Scale", GEN_F, "Smocza Łuska", "Smocze Łuski", "Smoczą Łuskę", "Smoczą Łuską")

S(
    "Dragon Scales are a prized material for armors. "
    "They are also prized by collectors, who would like to boast "
    "about how they have killed a Dragon.\n\n"
    "Dragon Scales disappear after 500 turns.",

    "Smocze łuski są cenionym materiałem na zbroje. "
    "Są one również poszukiwane przez zbieraczy, którzy "
    "lubią się przechwalać tym, ile smoków zabili.\n\n"
    "Smocze łuski znikają po 500 kolejkach."
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

    "Smoki to potężne bestie. Są powolne, ale bardzo złe, uwielbiają "
    "znęcać się nad stworzeniami, które są od nich jeszcze wolniejsze. "
    "Musisz je powstrzymać!\n\n"

    "Smok rusza się co dwie kolejki. Może atakować każdym segmentem, "
    "ruszyć całe swoje ciało do przodu lub do tyłu albo ruszyć początkowy "
    "fragment do tyłu. By pokonać Smoka, musisz uderzyć każdy z jego segmentów. "
    "Głowa się regeneruje w kolejkach, podczas których Smok się nie rusza, "
    "także zwykle musi być atakowana ostatnia; jeśli głowa jest zdrowa, "
    "może zionąć ogniem (zasięg 3), tracąc punkt życia. Za pokonanie Smoka "
    "(będąc wciąż w Smoczych Otchłaniach) dostajesz skarby."
   )

S("%The1 breathes fire at you!", "%1 zionął ogniem na Ciebie!")
S("%The1 breathes fire!", "%1 zionął ogniem!")

Orb("Domination", "Dominacji")

S(
    "This Orb lets you ride Dragons and other worm-like creatures. "
   "Simply move onto such a creature to ride them; while riding, you are protected from dangerous terrains "
   "and partially from attacks (they cause you to lose half of your Domination power), "
   "but you cannot collect items. When only one charge is left, "
   "you have to dismount this turn -- be very careful to make this possible, "
   "as your mount could attack you immediately!\n\n" "While riding, "
   "click on a location to order your mount to move or attack there.",

   "Ta Sfera pozwala Ci jeździć Smokami i innymi wężopodobnymi stworami. "
   "Po prostu wykonaj ruch na pole z takim stworem, by go dosiąść. "
   "Podczas jazdy jesteś chronion%y0 przed "
   "niebezpiecznym terenem i częściowo przed atakami (które powodują utratę połowy "
   "Twojej mocy Dominacji), ale nie możesz zbierać przedmiotów. Kiedy zostanie "
   "tylko 1 ładunek, musisz natychmiast zsiąść -- zadbaj o to, by to było "
   "możliwe bez zostania zaatakowanym przez Twojego dawnego wierzchowca!\n\n"
   "Podczas jazdy kliknij dowolne pole, by nakazać Twojemu wierzchowcowi "
   "tam się ruszyć lub zaatakować."
   )

S("You command %the1!", "%1 przyjmuje Twój rozkaz.")
S("You need to dismount %the1!", "Musisz opuścić %a1!")
S("%The1 tries to dismount you!", "%1 próbuje Cię zrzucić!")

S("Enemies killed: %1 (%2 types)", "Zabici przeciwnicy: %1 (%2 typów)")
S(" kills: %1/%2", " zabicia: %1/%2")
S("Different kills required: %1.\n", "Różnych zabitych przeciwników wymaganych: %1.\n")

// Galapagos
// ---------

N("Galápagos", GEN_M, "Galápagos", "Galápagos", "Galápagos", "na Galápagos")

N("Baby Tortoise", GEN_M, "Żółwik", "Żółwiki", "Żółwika", "Żółwikiem")

Orb("the Shell", "Skorupy")

S(
    "This Orb protects you from physical attacks. "
    "It lasts for more turns than the Orb of Shielding, but "
    "10 charges are lost whenever you are attacked. "
    "It also does not protect you from fires, scents, and being eaten.",
    "Ta Sfera chroni Cię przed fizycznymi atakami. "
    "Działa dłużej niż Sfera Tarczy, ale każdy atak zabiera "
    "10 ładunków. Nie chroni przed ogniem, zapachami i byciem zjedzonym.")

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

  "Galápagos to kraina Żółwi. Żółwie są bardzo powolne, co pozwala Smokom znęcac się nad nimi -- "
  "kradnąc i zjadając ich młode. Zwróć Żółwiki ich rodzinom, ale "
  "jest jeden haczyk: w różnych częściach Galápagos żyje wiele odmian Żółwi -- "
  "jest 21 binarnych czynników środowiskowych, co prowadzi do 2097152 odmian. "
  "Musisz znaleźć Żółwia dokładnie pasującego do Żółwika!\n\n"
  "Żówie ruszają się co trzy kolejki. Ataki powodują jedynie ogłuszenie.\n\n"
  "Zwrócenie Żółwika liczy się jako 5 $$$. Im więcej czynników zgadza się "
  "w danym miejscu, tym jaśniej jest ono narysowane na Twoim ekranie.")

S("%The1 scares %the2 a bit!", "%1 troszkę przestraszy%ł1 %a2!")
S("%The1 attacks your shell!", "%1 zaatakowa%ł1 Twoją skorupę!")

S("Hyperstone Quest: collect at least 10 points in %the2",
  "Misja Hiperkamień: zdobądź co najmniej 10 punktów %abl2")

S("animals killed: %1", "zabitych zwierząt: %1")
S("\n\nTortoises are not monsters! They are just annoyed. They do not count for your total kills.",
  "\n\nŻółwie to nie potwory! Są tylko rozwścieczone. Nie liczą się do Twoich zabić.")

S("Aww, poor %1... where is your family?", "Auu, biedny %1... gdzie Twoja rodzina?")
S("You are now a tortoise hero!", "Jesteś żółwim bohaterem!")
S("Find a %1 in %the2.", "Znajdź %a1 %abl2.")

/*
		"NEW_ACHIEVEMENT_6_27_NAME"	"Dragon Slayer"
		"NEW_ACHIEVEMENT_6_27_DESC"	"Collect a Dragon Scale."
		"NEW_ACHIEVEMENT_6_28_NAME"	"Dragon Lord"
		"NEW_ACHIEVEMENT_6_28_DESC"	"Collect 10 Dragon Slayers."
		"NEW_ACHIEVEMENT_6_29_NAME"	"Dragon Prince"
		"NEW_ACHIEVEMENT_6_29_DESC"	"Collect 25 Dragon Scales."
		"NEW_ACHIEVEMENT_6_30_NAME"	"Dragon King"
		"NEW_ACHIEVEMENT_6_30_DESC"	"Collect 50 Dragon Scales."
		"NEW_ACHIEVEMENT_6_31_NAME"	"Tortoise Hero"
		"NEW_ACHIEVEMENT_6_31_DESC"	"Save two Baby Tortoises."
		"NEW_ACHIEVEMENT_7_0_NAME"	"Tortoise Champion"
		"NEW_ACHIEVEMENT_7_0_DESC"	"Save five Baby Tortoises."
		"NEW_ACHIEVEMENT_7_1_NAME"	"Tortoise Ninja"
		"NEW_ACHIEVEMENT_7_1_DESC"	"Save ten Baby Tortoises."
		"NEW_ACHIEVEMENT_7_2_NAME"	"Reached the Yendorian Forest"
		"NEW_ACHIEVEMENT_7_2_DESC"	"Find and collect an Apple."
		"NEW_ACHIEVEMENT_7_3_NAME"	"Yendorian Researcher"
		"NEW_ACHIEVEMENT_7_3_DESC"	"Collect 10 Apples."
		"NEW_ACHIEVEMENT_7_4_NAME"	"Sparrowhawk"
		"NEW_ACHIEVEMENT_7_4_DESC"	"Collect 25 Apples."
		"NEW_ACHIEVEMENT_7_5_NAME"	"Yendorian Master"
		"NEW_ACHIEVEMENT_7_5_DESC"	"Collect 50 Apples."
*/

S("(includes co-op)", "(zawiera co-op)")
S("Revived!", "Wskrzeszono!")

S("You hear a distant roar!", "Słyszysz odległy ryk!")

S("turn count = %1 last exploration = %2 ghost timer = %3",
  "kolejek = %1 eksploracja = %2 czaso-duchy = %3")

S("You damage %the1 with a mental blast!", "Zrani%łeś0 %a1 mocą psychiczną!")

S("You are now a tortoise heroine!", "Jesteś żółwią bohaterką!")

S("Note: currently scores are saved only in the normal mode on Android",
  "Uwaga: wyniki na Androidzie są zapisywane tylko w normalnym trybie")
S("Note: you can play, but scores won't be saved on Android",
  "Uwaga: możesz grać, ale wyniki nie będą zapisywane")
S("settings set here won't be saved", "ustawienia ustawione tutaj się nie")
S("-- use the Android menu instead", "zapiszą -- użyj menu Androida")

S("summon Bonfire", "przywołaj ognisko")
S("Hyperstone Quest", "Misja Hiperkamień")
S("summon dead orbs", "przywołaj martwe sfery")
S("summon a Monster", "przywołaj potwora")
S("gain orb powers", "Moce sfer")
S("summon a Golem", "przywołaj Golema")
S("summon Thumpers", "przywołaj dudniki")
S("summon Ivy", "przywołaj Bluszcz")
S("lose all treasure", "utrata skarbów")
S("gain kills", "zdobądź zabicia")
S("Select the land ---", "Wybierz krainę ---")
S("summon Mimics", "przywołaj Mimiki")
S("summon orbs", "przwołaj sfery")
S("deplete orb powers", "utrata mocy")
S("Safety (quick save)", "Bezpieczeństwo (szybki save)")
S("summon treasure", "przywołaj skarby")
S("summon lots of treasure", "przywołaj dużo skarbów")
S("--- and teleport there", "--- i się teleportuj")
S("summon Sand Worm", "przywołaj Czerwia")
S("summon Orb of Yendor", "przywołaj sferę Yendoru")
S("rotate the character", "obróć postać")

S("gain Orb of Yendor", "zdobądź sferę Yendoru")
S("switch ghost timer", "przestaw czaso-duchy")
S("switch web display", "przestaw strukturę")

// just copy: "player 5 X"...
S("configure player 5", "konfiguruj gracza 5")
S("configure player 6", "konfiguruj gracza 6")
S("configure player 7", "konfiguruj gracza 7")

S("player 5 X", "gracz 5 X")
S("player 5 Y", "gracz 5 Y")
S("player 5 go", "gracz 5 idź")
S("player 5 spin", "gracz 5 obrót")
S("player 6 X", "gracz 6 X")
S("player 6 Y", "gracz 6 Y")
S("player 6 go", "gracz 6 idź")
S("player 6 spin", "gracz 6 obrót")
S("player 7 X", "gracz 7 X")
S("player 7 Y", "gracz 7 Y")
S("player 7 go", "gracz 7 idź")
S("player 7 spin", "gracz 7 obrót")

// VERSION 9.0
//=============

// extra descriptions for Orbs and some other things

S("\n\nThis Orb is triggered on your first attack or illegal move.",
  "\n\nTa Sfera się aktywuje automatycznie, gdy wykonasz atak lub nielegalny ruch.")

S("\nYou can never target cells which are adjacent to the player character, "
     "or ones out of the sight range.",
     "\nNie możesz celować w pola obok postaci, ani poza zasięgiem wzroku.")

S("\nThis is a ranged Orb. ", "\nDziałanie tej Sfery celujesz w konkretne miejsce.")

S("\nRanged Orbs can be targeted by long touching the desired location.",
  "\nRobi się to przez długie dotknięcie celu.")

S("\nRanged Orbs can be targeted by touching the desired location.",
  "\nRobi się to przez dotknięcie celu.")
  
S("\nRanged Orbs can be targeted by shift-clicking the desired location. ",
  "\nRobi się to przez kliknięcie na celu z Shiftem.")

S("\nRanged Orbs can be targeted by clicking the desired location. ",
  "\nRobi się to przez kliknięcie na celu.")
  
S("\n\nOnce you collect 10 Bomberbird Eggs, "
  "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
  "Collecting even more Eggs will increase the radius. Additionally, collecting "
  "25 Bomberbird Eggs will reveal adjacent cells even in your future games.",
  
  "\n\nGdy zdobędziesz 10 Jaj Bombardiera, ruch na pole bez sąsiadujących min "
  "pokazuje także liczby na sąsiednich polach; jeszcze więcej jaj to większy promień. "
  "Dodatkowo, zebranie 25 Jaj Bombardiera powoduje pokazywanie sąsiednich pól "
  "nawet w kolejnych grach.")

S("This might be very useful for devices with limited memory.",
  "To może być bardzo przydatne na urządzeniach z ograniczoną pamięcią.")
  
S("(in the MENU). You can reduce the sight range, this should make the animations smoother.",
  "(w MENU). Możesz zmniejszyć zasięg wzroku, co powinno poprawić płynność animacji.")

S("Unavailable in the shmup mode.\n", "Niedostępne w trybie strzelanki.\n")
S("Unavailable in the multiplayer mode.\n", "Niedostępne w trybie wielu graczy.\n")
S("Unavailable in the Chaos mode.\n", "Niedostępne w trybie Chaosu.\n")
S("Bonus land, available only in some special modes.\n",
  "Kraina dodatkowa, dostępna tylko w niektórych trybach.\n")

S("\n\nThis Orb protects you from attacks, scents, and insulates you "
  "from electricity. It does not let you go through deadly terrain, but "
  "if you are attacked with fire, it lets you stay in place in it.",
  
  "\n\nTa Sfera chroni Cię przed atakami, zapachami, i izoluje od "
  "elektryczności. Nie pozwala iść przez zabójczy teren, ale "
  "jeśli zostaniesz zaatakowan%y0 ogniem, możesz stać w miejscu.")

S("Completing the quest in this land is not necessary for the Hyperstone Quest.",
  "Wykonanie tego zadania nie jest konieczne do zdobycia Hiperkamieni.")

// other options

S("player", "gracz")
S("movement color", "kolor ruchu")
S("model used (not conformal!)", "użyty model (nie konforemny!)")
S("azimuthal equidistant", "azymutalny, równe d")
S("azimuthal equi-area", "azymutalny, równe pola")
S("visit the website", "idź na stronę HyperRogue")
S("leaderboards/achievements", "osiągnięcia i rankingi")
S("return to the game", "powrót do gry")

// Android texts

S("connected to Google Games", "połączenie z Google Games")
S("background music", "muzyka w tle")
S("view your achievements", "pokaż osiągnięcia")
S("next page", "kolejna strona")

// new multiplayer

S("shoot'em up and multiplayer", "strzelanka / wielu graczy")

S("Two players cannot move/attack the same location!", "Dwóch graczy nie może celować w to samo miejsce!")
S("Cannot move into the current location of another player!", "Nie możesz wejść na miejsce innego gracza!")
S("Players cannot get that far away!", "Gracze nie mogą się tak oddalać!")
S("Cannot push into another player!", "Nie można pchać na innego gracza!")
S("Cannot push into the same location!", "Nie można pchać w to samo miejsce!")

S("%The1 throws fire at %the2!", "%1 rzuca ogniem w %a2!")
S("%The1 breathes fire at %the2!", "%1 zieje ogniem w %a2!")

S("configure keys", "klawisze skonfigurowane")
S("use default keys", "klawisze domyślne")

S("Left the game.", "Opuszczono grę.")
S("shoot'em up mode", "strzelanka")
S("turn-based mode", "gra turowa")
S("continue playing", "kontynuuj grę")
S("start a new game", "nowa gra")

S("five players", "pięciu graczy")
S("six players", "sześciu graczy")
S("seven players", "siedmiu graczy")

S("You collect %the1. (%2)", "Znalaz%łeś0 %a1. (%2)")


S(
"Shmup (shoot'em up) mode: You can play a hyperbolic shoot'em up game. The game is based "
"on the usual turn-based grid-based HyperRogue, but there are some changes. You fight by "
"throwing knives, and you have three extra lives. There are no allies, so all Orbs "
"related to allies give you extra lives instead (up to 5). Some other rules have been "
"adapted too.\n\n",

"Strzelanka: grasz w hiperboliczną strzelankę, bazowaną na turowych zasadach HyperRogue, "
"ale z pewnymi zmianami. Twoja broń to rzucane noże, masz 3 dodatkowe życia. Nie ma "
"za to przyjaciół, zatem wszystkie Sfery ich dotyczące dają Ci dodatkowe życia (max 5). "
"Niektóre z pozostałych reguł również zostały dostosowane.\n\n")

S(
"Multiplayer: Play cooperatively (locally); treasures, kills, and deaths are calculated "
"for each player too, for more competitive play. Orbs and treasures are shared, orbs drain "
"faster, knives recharge slower, and player characters are not allowed to separate.\n\n",

"Wielu graczy: współpracujecie (lokalnie); można też współzawodniczyć o skarby i zabicia, "
"które są liczone dla każdego gracza. Sfery i skarby są dzielone, sfery wyczerpują się "
"szybciej, noże wracają wolniej, i gracze nie mogą się rozdzielać.\n\n")

S(
"Turn-based multiplayer: Turns are executed in parallel. A player can leave the game "
"by pressing a designated key (useful when about to get killed or lost). The following "
"Orbs work to bring such players back: ",
"Turowo na wielu graczy: tury wykonujecie równolegle. Gracz może opuścić grę "
"naciskając wyznaczony klawisz (użyteczne, gdy się zgubi lub by zaraz został zabity). "
"Następujących Sfer można użyć, by taki gracz wrócił:")

S("This menu can be also used to configure keys.\n\n",
  "To menu może być też użyte do konfiguracji klawiszy.")

// other stuff

S("Warnings are issued when you try to do something that appears dangerous, "
  "like stepping on a known mine, or getting your boat destroyed by "
  "a Kraken without having Orb of the Fish. In some cases the action "
  "might actually be safe -- so you can ignore the warning and do it anyway, "
  "simply by repeating the action.",
  
  "Ostrzeżenia pokazują się, gdy próbujesz zrobić coś niebezpiecznego: "
  "stanąć na minie, spowodować zniszczenie łódki przez "
  "Krakena bez Sfery Ryby, itp. W niektórych przypadkach może to być bezpieczne -- "
  "także możesz zignorować ostrzeżenie, po prostu próbując wykonać akcję "
  "jeszcze raz.")


// kraken depths

N("Kraken Depths", GEN_F, "Głębiny Krakenów", "Głębiny Krakenów", "Głębiny Krakenów", "w Głębinach Krakenów")

S(
    "A long time ago, this was a trade route. But then, Krakens have risen out of the "
    "depths. Many trading ships sank here. Legend says that you can uncover the secret "
    "of a magical weapon spell somewhere in the depths...\n\n"
    
    "You can find Sunken Treasures here, but they won't appear until you have killed "
    "a Kraken. You will also need Orb of the Fish to get the treasures, luckily you can "
    "steal one from the Viking treasure hunters.",
    
    "Dawno, dawno temu, to był szlak handlowy, ale Krakeny wypłynęły z głębin. Wiele "
    "statków handlowych zatonęła. Według legendy, gdzieś w głębinach ukryty jest sekret "
    "magicznej broni...\n\n"
    "Możesz tu znaleźć Zatopione Skarby, ale nie będą się one pojawiać, aż pokonasz "
    "Krakena. By wyłowić skarby, potrzebna jest Sfera Ryby. Na szczęście można ukraść "
    "ją od wikińskich łowców skarbów.")

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
  
  "Tam, skąd pochodzisz, również są Krakeny... wielkie morskie potwory, które "
  "z łatwością mogą zatapiać statki. Geometria tego dziwnego świata powoduje, "
  "że wielkie obiekty nie mogą się szybko poruszać, nie ma tu zatem dużych statków, "
  "tylko małe łódki, i hiperboliczne Krakeny też są względnie małe. Wciąż one są "
  "chyba największymi stworzeniami, które wciąż mogą się poruszać z rozsądną "
  "prędkością...\n\n"
  
  "Głowa krakena porusza się tylko po sześciokątach. By pokonać Krakena, musisz "
  "zaatakować jego wszystkie macki. Macka nie atakuje, jeśli została zaatakowana "
  "w tej turze. Jeśli Kraken zaatakuje Cię, gdy jesteś na statku, zniszczy "
  "statek, ale Cię nie zabije."
  )

Orb("the Sword", "Ostrza")

S(
    "This Orb gives you a weapon made of pure magical energy. You do not hold "
    "it, it simply floats in the air next to you. When you go, the energy sword moves "
    "with you, pointing at the same relative angle it pointed before -- you cannot "
    "move or rotate it otherwise. Most monsters can be killed by moving the sword into them, "
    "and won't move into the spot with the sword.",
    
    "Ta Sfera daje Ci ostrze zbudowaną z czystej magicznej energii. Nie trzymasz "
    "go w ręce, po prostu unosi się ono w powietrzu obok Ciebie. Kiedy się poruszasz, "
    "ostrze energii porusza się razem z Tobą, pozostając skierowane pod tym samym "
    "kątem względem Twojego ruchu, co wcześniej -- nie możesz nim obracać "
    "ani poruszać w inny sposób. Większość potworów ginie, gdy uderzysz w nie ostrzem, "
    "nie wejdą one też na pole, na którym ostrze jest.")

N("Sunken Treasure", GEN_O, "Zatopiony Skarb", "Zatopione Skarby", "Zatopiony Skarb", "Zatopionym Skarbem")

S("Cargo of a ship which was once destroyed by a Kraken.", 
  "Ładunek statku zatopionego dawno temu przez Krakena.")

// burial grounds

N("Burial Grounds", GEN_O, "Kurhany", "Kurhany", "Kurhany", "w Kurhanach")

S(
  "Ancient Viking heroes were buried here. Their graves have barrows raised over "
  "them, and are guarded by Draugar, animated corpses who are immune to mundane weapons. "
  "You will need to use a magical weapon spell to defeat them, and to rob the "
  "ancient jewelry buried in the graves.",
  
  "Tu pochowano starożytnych wikińskich bohaterów. Nad ich grobami wzniesiono kurhany. "
  "Kurhany są strzeżone przez Draugry, ożywione zwłoki odporne na zwyczajne bronie. "
  "Potrzebujesz magicznej broni, by je pokonać i zrabować starożytną biżuterię zakopaną "
  "w kurhanach.")

N("Draugr", GEN_M, "Draugr", "Draugry", "Draugra", "Draugrem")

S(
  "Animated corpses of ancient Viking warriors. They are immune to mundane weapons, "
  "but they can be destroyed by your Orb of the Sword.",
  "Ożywione zwłoki starożytnych wikińskich wojowników. Draugry są odporne na zwyczajne bronie, "
  "ale mogą zostać zniszczone przy użyciu Sfery Ostrza.")

Orb("the Sword II", "Ostrza II")

S(
  "An alternative version of Orb of the Sword. If you have both of them, "
  "you have two energy swords, facing in opposite directions.",
  "Alternatywna wersja Sfery Ostrza. Jeśli masz obie, masz dwa ostrza energii, "
  "skierowane w przeciwnych kierunkach."
  )

N("Ancient Jewelry", GEN_F, "Starożytna Biżuteria", "Starożytna Biżuteria", "Starożytną Biżuterię", "Starożytną Biżuterią")

S(
  "Precious belongings of ancient Viking heroes. Your Orb of the Sword can be "
  "used to dig these treasures out of the barrows.",
  "Drogocenne przedmioty należące do starożytnych wikińskich bohaterów. Użyj "
  "Sfery Ostrza, by wykopać te skarby z kurhanów.")

S("%The1 destroys your boat!", "%1 zniszczy%ł1 Twój statek!")
S("Your mundane weapon cannot hurt %the1!", "Twoja zwykła broń nie działa na %a1!")
S("This move appears dangerous -- are you sure?", "Ten ruch wygląda na niebezpieczny -- na pewno?")
S("You slash %the1.", "Poci%ąłeś0 %a1.")
S("You chop down %the1.", "Ści%ąłeś0 %a1.")
S("Cannot slash another player!", "Nie możesz ciąć innego gracza!")
S("%The1 suffocates!", "%1 się udusi%ł1!")

N("barrow wall", GEN_F, "ściana kurhanu", "ściany kurhanu", "ścianę kurhanu", "ścianą kurhanu")

S("This wall is quite strong. You will need another way in.",
  "Ściana kurhanu jest dosyć mocna, musisz poszukać innego wejścia.")

N("barrow", GEN_O, "kurhan", "kurhany", "kurhan", "kurhanem")

S("Your Orb of the Sword can be used to dig here.", 
  "Możesz tu kopać przy użyciu Sfery Ostrza.")

// trollheim

N("Trollheim", GEN_O, "Trollheim", "Trollheim", "Trollheim", "w Trollheim")

S(
    "Many clans of Trolls spend their lives in this kingdom. You can find many "
    "statues of Trolls here. You suppose that they are not actually statues, but simply "
    "elderly Trolls, who have petrified upon death. Or maybe you have killed "
    "these Trolls yourself?",
    
    "Wiele klanów Trolli żyje w tym królestwie. Można tu znaleźć mnóstwo pomników "
    "trolli, albo raczej nie pomników, tylko po prostu stare Trolle, które "
    "umierając zamieniły się kamień. A może to Trolle zabite przez Ciebie?")


N("Golden Egg", GEN_N, "Złote Jajo", "Złote Jaja", "Złote Jajo", "Złotym Jajem")

S(
    "Trolls of Trollheim are descendants of a bridge Troll, who collected "
    "payments from people crossing the bridge. One of them paid with "
    "golden eggs. The bridge Troll found the eggs beautiful, but he quickly lost them. "
    "Golden eggs are still revered by Trolls, and you can find them in their "
    "caves.",
    
    "Trolle z Trollheim są potomkami Trolla, który pobierał myto od osób "
    "przechodzących mostem. Jeden z podróżnych zapłacił złotymi jajami. Jaja "
    "bardzo spodobały się naszemu Trollowi, ale szybko je zgubił. Trolle z Trollheim "
    "wciąż darzą złote jaja wielkim poważaniem i możesz znaleźć je w ich legowiskach.")

Orb("the Stone", "Kamienia")

S(
    "Trolls turn into stone walls when they die. When you have this Orb, "
    "this happens to every monster you defeat. Statues created from this Orb "
    "have slightly different properties than Trolls who petrify naturally.",
    
    "Gdy Trolle umierają, zamieniają się w kamień. Ta Sfera powoduje, że "
    "dzieje się to ze wszystkimi pokonanymi stworzeniami. Pomniki stworzone "
    "przez Sferę Kamienia mają inne własności niż Trolle spetryfikowane naturalnie.")

N("stone statue", GEN_O, "kamienny pomnik", "kamienne pomniki", "kamienny pomnik", "kamiennym pomnikiem")

S("A petrified creature.", "Stworzenie zamienione w kamień.")

S("mark heptagons", "oznaczenia na siedmiokątach")
S("help for keyboard users", "pomoc dla użytkowników klawiatury")

// missing

S("You leave %the1.", "Zostawiasz %a1.") // Baby Tortoise

S("%The1 says, \"I die, but my clan in Trollheim will avenge me!\"",
  "%1 mówi: \"Ja umieram, ale mój klan w Trollheim mnie pomści!\"")

S("After killing %the1, you feel able to reach the Elemental Planes!",
  "Po pokonaniu %a1 czujesz, że masz dostęp do Strefy Żywiołów!")

S("You feel that a magical weapon is waiting for you...",
  "Czujesz, że magiczna broń na Ciebie czeka...")

// missing

S("reset per-player statistics", "zresetuj statystyki graczy")
S("An alternate layout of the Crossroads, without walls.", "Alternatywny układ Skrzyżowań, bez ścian.")
S("cheats", "oszustwa")

S("Known mines may be marked by pressing 'm'. Your allies won't step on marked mines.", 
  "Znane miny można oznaczać naciskając 'm'. Twoi przyjaciele nie będą stawać na oznaczonych minach.")

S("Known mines may be marked by touching while in drag mode. Your allies won't step on marked mines.", 
  "Znane miny można oznaczać dotykając ich w trybie przeciągania. Twoi przyjaciele nie będą stawać na oznaczonych minach.")

// 9.0 patches
//=============

// extra help
S("Coastal region -- connects inland and aquatic regions.\n",
  "Kraina przybrzeżna -- łączy krainy lądowe i morskie.\n")

S("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n",
  "Kraina morska -- dostępna jedynie zs krain przybrzeżnych i morskic.\n")

// barrow treasure protected from Orb of Space
S("%The1 is protected from this kind of magic!",
  "Tego typu magia nie działa na %1!")

// Orb is always available in the given land
S("always available", "zawsze dostępna")

S("\n\nAfter the Trolls leave, you have 750 turns to collect %the1, or it gets stolen.",
  "\n\nGdy Trolle wyruszą, masz 750 kolejek na zebranie skarbów -- jeśli tego nie zrobisz, %P1 zostaną ukradzione.")

S(
    "There are many lands in HyperRogue. Collect 10 treasure "
    "in the given land type to complete it; this enables you to "
    "find the magical Orbs of this land, and in some cases "
    "get access to new lands. At 25 treasures "
    "this type of Orbs starts appearing in other lands as well. Press 'o' to "
    "get the details of all the Lands.\n\n",
    
    "W świecie HyperRogue jest wiele krain. Kraina jest zdobyta, "
    "gdy zbierzesz w niej 10 skarbów; gdy zdobędziesz krainę, "
    "możesz znaleźć magiczną Sferę tej krainy, i w niektórych przypadkach "
    "uzsykać dostęp do nowych krain. Gdy zbierzesz 25 skarbów, "
    "ten typ Sfer będzie się też pojawiał w innych krainach. "
    "Naciśnij '0', by obejrzeć szczegóły na temat wszystkich Krain.\n\n"
    )

// welcome messages
S("Welcome to the Random Pattern mode!", "Witaj w trybie losowych wzorów!")
S("You are playing %the1 in the Pure Tactics mode.", "Grasz %a1 w trybie taktycznym.")
// %1 is the number
S("Welcome to the Yendor Challenge %1!", "Witaj w Misji Yendor %1!")
S("Welcome to HyperRogue!", "Witaj w HyperRogue!")
S("Press F1 or right-click things for help.", "F1 lub klikaj prawym na elementach gry, by uzyskać pomoc.")
// for iOS
S("Press F1 or right-shift-click things for help.", "F1 lub klikaj z prawym shiftem na elementach gry, by uzyskać pomoc.")
S("Welcome to the Euclidean mode!", "Witaj w trybie euklidesowym!")
S("Good luck in the elliptic plane!", "Powodzenia na płasczyźnie eliptycznej!")
S("Welcome to Spherogue!", "Witaj w Sferogue!")

// extra help for joystick configuration

S("joystick configuration", "konfiguracja joysticka")
S("first joystick position (movement)", "pozycja pierwszego joysticka (ruch)")
S("second joystick position (panning)", "pozycja drugiego joysticka (przesuwanie)")
S("old style joystick configuration", "konfiguracja joysticka w starym stylu")

S("move up-right", "ruch w górę-prawo")
S("move up-left", "ruch w górę-lewo")
S("move down-right", "ruch w dół-prawo")
S("move down-left", "ruch w dół-lewo")
S("stay in place (left + right)", "stój w miejscu (lewo+prawo)")
S("cancel move", "anuluj ruch")
S("leave the game", "opuść grę")
S("drop Dead Orb (up + down)", "połóż Martwą Sferę (góra+dół)")

// extra help for configuration / projection and geometry

S("Roughly 42% cells are on the edge of your sight range. Reducing "
  "the sight range makes HyperRogue work faster, but also makes "
  "the game effectively harder.",
  "Około 42% pól jest na brzegu Twojego pola widzenia. Zmniejszenie "
  "zasięgu sprawia, że HyperRogue działa szybciej, ale rozgrywka jest "
  "trudniejsza.")

S("Watch the Minkowski hyperboloid or the hypersian rug mode with the "
  "red/cyan 3D glasses.",
  "Oglądaj hiperboloidę Minkowskiego albo tryb hiperskiego dywanu przez czerwono-turkusowe okulary 3D.")

S("0 - return", "0 - powrót")
S("return", "powót")
S("F1 - help", "F1 - pomoc")

// for the conformal polynomial
S("coefficient (imaginary)", "współczynnik (część urojona)")
S("Scale the displayed model.", "Skaluj model na ekranie.")
S("Reenter HyperRogue to apply this setting", "Wyjdź i wróć do HyperRogue, by zastosować")

S(  "HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincaré models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
    "See also the conformal mode (in the special modes menu) "
    "for more models.",
    
    "HyperRogue wewnętrznie używa modelu hiperboloidy Minkowskiego. "
    "Modele Kleina i Poincaré są uzyskiwane przez perspektywę, "
    "a model Gansa przez rzut ortogonalny. "
    "Więcej modeli w trybie konforemnym (w menu trybów specjalnych).")

S("stereographic projection", "rzut stereograficzny")
S("Poincaré model", "model Poincaré")
S("gnomonic projection", "rzut gnomoniczny")
S("Klein model", "model Kleina")
S("towards orthographic", "w stronę rzutu ortogonalnego")
S("towards Gans model", "w stronę modelu Gansa")

S("Euclidean/elliptic mode", "tryb euklidesowy/eliptyczny")
S("geometry", "geometria")
S("spherical", "sferyczna")
S("elliptic", "eliptyczna")
S("Euclidean", "euklidesowa")
S("projection", "rzut")

// VERSION 9.1
//=============

N("Familiar", GEN_M, "Chowaniec", "Chowańce", "Chowańca", "Chowańcem")
S("Commanded %the1!", "%1 dosta%ł1 rozkaz!")
S("F8 = SVG shot", "F8 = zrzut SVG")
S("Saved the SVG shot to %1 (sightrange %2)", "Zapisano zrzut SVG do %1 (zasięg %2)")

S("draw circle around the target", "rysuj kółko wokół celu")
S("reverse pointer control", "odwrotne sterowanie")
S("sight range", "zasięg wzroku")
S("configure input", "konfiguracja sterowania")

// quote of Euclid
S("\"There is no royal road to geometry.\"",
   "W geometrii nie ma specjalnych dróg dla królów.\"")

// quote of Lobachevsky
S("\"There is no branch of mathematics, however abstract, ",
  "\"Nie ma na tyle abstrakcyjnej gałęzi matematyki, by się nie dało jej ")
  
S("which may not some day be applied to phenomena of the real world.\"",
  "zastosować do zjawisk w świecie rzeczywistym.\"")
  
// quote of Gauss
S("\"It is not possession but the act of getting there, ",
  "\"Nie posiadanie, ale proces docieranie na miejsce, ")
  
S("which grants the greatest enjoyment.\"", "sprawia największą przyjemność.\"")

// quote of Escher
S("\"We live in a beautiful and orderly world, ",
  "\"Żyjemy w pięknym i uporządkowanym świecie, ")

S("and not in a chaos without norms.\"",
  "a nie w chaosie bez reguł.\"")

/*
		"NEW_ACHIEVEMENT_7_21_NAME"	"Deadly Death"
		"NEW_ACHIEVEMENT_7_21_DESC"	"Defeat Goblin with an energy sword."
		"NEW_ACHIEVEMENT_7_22_NAME"	"Halloween Witch"
		"NEW_ACHIEVEMENT_7_22_DESC"	"Collect 50 Treats in Halloween."
		"NEW_ACHIEVEMENT_7_23_NAME"	"Halloween Vampire"
		"NEW_ACHIEVEMENT_7_23_DESC"	"Colelct 100 Treats in Halloween."
*/

S("You have enough treasure now to revive %the1!",
  "Masz dość skarbów, by przywrócić %a1 do życia!")

S("Killed %1 can be revived with Orb of the Love, after you collect 20 more $$$.",
  "Zabit%ya1 %1 może zostać przywrócona do życia Sferą Miłości, "
  "po zdobyciu 20 dodatkowych $$$."
  )

S("%The1 will be revivable at %2 $$$", "%1 będzie mogła ożyć przy %2 $$$")

S("Halloween mini-game", "mini-gra Halloween")


N("Vampire Bat", GEN_M, "Wampir", "Wampiry", "Wampira", "Wampirem")
N("Halloween", GEN_O, "Halloween", "Halloween", "Halloween", "w Halloween")
N("Treat", GEN_O, "Cukierek", "Cukierki", "Cukierka", "Cukierkiem")

S(
  "Halloween is a special land, that is available only in the spherical "
  "or elliptic geometry (press 'o' to switch). You play on the surface of "
  "a jack-o'-lantern, "
  "and have to collect as many Treats as possible. Each Treat you collect "
  "brings new monsters to fight, and new magical powers for you. You "
  "have to fight the monsters while effectively managing your limited "
  "resources.",
  
  "Halloween to specjalna kraina, dostępna jedynie w geometrii sferycznej "
  "lub eliptycznej ('o' by zmienić). Grasz na powierzchni wydrążonej dyni, "
  "i musisz zdobyć jak najwięcej Cukierków. Każdy Cukierek przynosi "
  "nowe potwory do walki, i nowe magiczne moce. Walcz z potworami, "
  "efektywnie zarządzając swoimi ograniczonymi zasobami."
  )

S(
  "Vampire Bats don't attack normally, but they drain your magical powers if "
  "they are at distance at most 2 from you.",
  "Nietoperze Wampiry nie atakują jak inne potwory. Zamiast tego, jeśli "
  "są w odległości co najwyżej 2, wysysają Twoje magiczne moce.")

S("Ghosts can move through chasms!", "Duchy przelatują nad dziurami!")
S("Push Skeletons into the holes!", "Wpychaj Szkielety do dziur!")
S("You'll need your magical sword against the Draugar!", "Potrzebujesz magicznego miecza przeciwko Draugrom!")
S("Demons are slow, but you'll need the experience against stronger ones...",
  "Mniejsze Demony są powolne, ale musisz zdobyć doświadczenie przeciwko Większym...")
S("You will need more experience to defeat the Greater Demon!",
  "Musisz zdobyć doświadczenie, by pokonać Większego Demona!")
S("Cultists throw fire at you from distance!",
  "Kultyści rzucają ogniem z odległości!")
S("Defeat Flail Guards by moving away from them.",
  "Wojownika z Cepem pokonujesz odchodząc od niego.")
S("Vampire Bats drain your magical powers!",
  "Wampiry wysysają Twoje magiczne moce!")

S("Hint: use arrow keys to scroll.", "Wskazówka: przewijasz strzałkami.")
S("Hint: press 1 2 3 4 to change the projection.", "Wskazówka: 1 2 3 4 zmienia rzut.")
S("You gain a protective Shell!", "Zdobył%eś0 ochronną Skorupę!");
S("Hint: magical powers from Treats expire after a number of uses.",
  "Wskazówka: magiczne moce Cukierków wyczerpują się po kilku użyciach.")
S("A magical energy sword. Don't waste its power!",
  "Miecz energii. Nie zmarnuj jego mocy!")
S("Another energy sword!", "Jeszcze jeden miecz energii!")
S("You got Thorns! Stab monsters by moving around them.",
  "Ciernie! Dźgaj potwory chodząc obok nich.")
S("Aethereal powers let you go through the holes.",
  "Moc Eteru pozwala Ci przechodzić przez dziury.")
S("The tasty treat increases your protection.",
  "Pyszny Cukierek poprawia Twoją ochronę.")
S("You gain your protective Shell back!", "Masz Skorupę z powrotem!")

S("rainbow landscape", "tęczowy krajobraz")
S("Cannot use %the1 here!", "Nie możesz tutaj tego użyć!")
S("draw the grid", "rysowanie siatki")
S("Escher/3D", "Escher/3D")
S("plain/3D", "prosty/3D")
S("3D", "3D")
S("Welcome to Halloween!", "Witaj w Halloween!")

S("How long should the messages stay on the screen.",
  "Jak długo wiadomości zostają na ekranie")
S("select this color", "wybierz ten kolor")
S("sound effects volume", "głośność efektów dźwiękowych")

S("\n\nSee sounds/credits.txt for credits for sound effects", 
  "\n\nPodziękowania za efekty dźwiękowe w sounds/credits.txt")

S("scrolling speed", "prędkość przewijania")
S("movement animation speed", "prędkość animacji ruchu")
S("+5 = move instantly", "+5 = ruch natychmiastowy")
S("extra graphical effects", "dodatkowe efekty graficzne")

// VERSION 9.3
//=============

S("SORT", "SORT")
S("PICK", "TEN")
S("PLAY", "GRAJ")

// 3D configuration
S("3D configuration", "konfiguracja 3D")
S("High detail range", "Zasięg wysokiego poziomu szczegółów")
S("Mid detail range", "Zasięg średniego poziomu szczegółów")
S("Camera level above the plane", "Wysokość kamery nad płaszczyzną")
S("Ground level below the plane", "Poziom podłoża pod płaszczyzną")
S("Projection at the ground level", "Współczynnik rzutu na poziome podłoża")
S("Height of walls", "Wysokość ścian")
S("Rock-III to wall ratio", "Stosunek skały-III do ścian")
S("Human to wall ratio", "Stosunek człowieka do ścian")
S("Level of water surface", "Poziom powierzchni wody")
S("Level of water bottom", "Poziom dna wody")
S("set 3D monsters or walls in basic config first",
  "najpierw ustaw 3D potwory/ściany w konfiguracji podstawowej")
S("parameters set correctly", "parametry prawidłowe")
S("exit 3D configuration", "wyjdź z konfiguracji 3D")

S("Objects at distance less than %1 absolute units "
  "from the center will be displayed with high "
  "detail, and at distance at least %2 with low detail.",
  
  "Obiekty w odległości bliższej niż %1 od środka będą rysowane "
  "z wieloma szczegółami, a dalszej niż %2 z niewieloma.")

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
  
  "Poziom podłoża jest w rzeczywistości ekwidystantną powierzchnią "
  "%1 jednostek pod płaszczyzną P. Teoretycznie, ta wartość "
  "wpływa na świat -- przykładowo, orły mogłyby latać %2 razy "
  "szybciej latając powyżej poziomu podłoża, na wysokości płaszczyzny "
  "P -- ale nie wpływa to na mechanikę gry w żaden sposób. "
  "(Odległości wyświetlane przez edytor grafiki "
  "dotyczą odległości między odpowiednimi punktami na płaszczyźnie P.")

S( "If we are viewing an equidistant g absolute units below a plane, "
   "from a point c absolute units above the plane, this corresponds "
   "to viewing a Minkowski hyperboloid from a point "
   "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
   "the Poincaré model for g=c, and Klein-Beltrami model for g=0.",
   
   "Jeśli ekwidystantną powierzchnię g jednostek pod płaszczyzną oglądamy "
   "z punktu na wysokości c, odpowiada to oglądaniu hiperboloidy Monkowskiego "
   "z punktu tanh(g)/tanh(c) poniżej jej środka. To z kolei odpowiada "
   "modelowi Poincaré dla g=c, i Kleina-Beltramiego dla g=0.")

S( "The height of walls, in absolute units. For the current values of g and c, "
   "wall height of %1 absolute units corresponds to projection value of %2.",
   
   "Wysokość ścian. Przy obecnych wartościach g i c, góra ściany na wysokości "
   "%1 odpowiada współczynnikowi projekcji %2.")

S( "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
   "Length of paths on the Rock III level is %3 of the corresponding length on the "
   "ground level.",
   
   "Iloraz Skały III do ściany to %1, czyli Skała III ma wysokość %2. "
   "Drogi na poziomie Skały III są %3 odpowiednich odległości na poziomie podłoża.")

S( "Humans are %1 "
   "absolute units high. Your head travels %2 times the distance travelled by your "
   "feet.",
   
   "Ludzie mają wysokość %1. Twoja głowa porusza się %2 razy szybciej niż Twoje "
   "nogi.")
  
// Euclidean regular patterns
S("three colors", "trzy kolory")
S("three colors rotated", "trzy kolory obrócone")
S("edit all three colors", "edytuj wszystkie kolory")

S(" (local treasure)", " (lokalny skarb)")
S(" (increases treasure spawn)", " (więcej skarbów)")
S(" (appears here)", " (występuje tu)")
S(" (appears here)", " (występuje tu)")
S("%The1 is destroyed!", "%1 zosta%ł1 zniszczon%ya1!") // generic message

S("page", "strona")
S("all", "wszystko")

S("Your Orb of Recall is blocked by something big!",
  "Coś dużego blokuje Twoją Sferę Powrotu!")

S("You are recalled!", "Wracasz!")
S("You vault over %the1!", "Szarżujesz!")
S("You cannot attack %the1!", "Nie możesz zaatakować %a1!")

S("\n\nFast flying creatures may attack or go against gravity only in their first move.",
  "\n\nSzybko latające stworzenia mogą atakować i poruszać się wbrew grawitacji "
  "tylko w swoim pierwszym ruchu.")

// Dungeon
//---------

N("Dungeon", GEN_O, "Loch", "Lochy", "Loch", "w Lochu")

S("The result of a collaboration of the Great Vizier and the Wizard of the Ivory Tower.",
  "Efekt współpracy Wielkiego Wezyra i Czarodzieja z Wieży z Kości Słoniowej.")

N("Slime Mold", GEN_O, "Śluzowiec","Śluzowce", "Śluzowiec", "Śluzowcem")

S("A very interesting species of slime mold.",
  "Bardzo ciekawy gatunek śluzowca.")

N("Bat", GEN_O, "Nietoperz", "Nietoperze", "Nietoperza", "Nietoperzem")

S("Someone has told you that one can get battle experience safely by "
  "killing tons of essentially harmless creatures, such as Bats. But "
  "does this make any sense?...\n\n"
  
  "It does not. Bats cannot hurt you, but may block your movement, or "
  "toggle switches if they fall on them.",
  
  "Ktoś powiedział, że można bezpiecznie zdobyć doświadczenie w walce "
  "poprzez zabicie dużej liczby właściwie niegroźnych stworzeń, jak na "
  "przykład Nietoperzy. Ale czy to ma jakiś sens?...\n\n"
  
  "Nie ma. Nietoperze nie mogą Ci nic zrobić, ale blokują Twój ruch, "
  "i mogą przestawiać przełączniki, gdy na nie spadną."
  )

Orb("Recall", "Powrotu")

S("When the charges on this Orb expire, "
  "you will be automatically returned to the place where you have found it. "
  "Extra Orbs of Recall delay this without changing the recall location. "
  "Pick up an Orb of Safety causes an immediate recall.",
  
  "Kiedy skończą się ładunki tej Sfery, automatycznie wracasz do "
  "miejsca, w którym ją znalazłeś. Dodatkowe Sfery Powrotu "
  "opóźniają powrót bez zmiany miejsca powrotu, a Sfera Bezpieczeństwa "
  "powoduje natychmiastowy powrót."
  )

// 
//

// lost mountain
//===============

//N("Pyramid", GEN_F, "Piramida", "Piramidy", "Piramidą", "na Piramidzie")
N("Lost Mountain", GEN_F, "Zagubiona Góra", "Zagubione Góry", "Zagubioną Górą", "na Zagubionej Górze")

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
  
  "Anomalia grawitacyjna w Dżungli stworzyła górę porośniętą bluszczem "
  "i krzakami. Czy odważysz się wspiąć po bluszczu, by zdobyć ukryte "
  "na niej ametysty?\n\n"
  
  "Pola sąsiadujące z Bluszczem liczą się jako stabilne (ale Bluszcz "
  "nie może się wspinać po samym sobie ani po innych bluszczach)."
  )
  
N("big bush", GEN_O, "duży krzak", "duże krzaki", "duży krzak", "dużym krzakiem")

S("You can hold this bush to climb the Lost Mountain. "
  "Bushes block the movement of birds.",
  "Możesz złapać ten krzak, by się wspiąć na Zagubioną Górę. "
  "Krzaki blokują drogę ptakom."
  )

S("You can hold this bush to climb the Lost Mountain, "
  "but it is not very strong -- it will get destroyed "
  "if you climb from it into an unstable location. "
  "Bushes block the movement of birds.",
  
  "Możesz złapać ten krzak, by się wspiąć na Zagubioną Górę, "
  "ale nie jest on zbyt wytrzymały -- zniszczy się, "
  "gdy wespniesz się z niego do niestabilnej pozycji. "
  "Krzaki blokują drogę ptakom.")

N("Amethyst", GEN_O, "Ametyst", "Ametysty", "Ametyst", "Ametystem")

S("A beatiful purple gem from the Lost Mountain.",
  "Piękny fioletowy kamień z Zagubionej Góry.")

Orb("Nature", "Natury")
N("Friendly Ivy", GEN_O, "Przyjazny Bluszcz", "Przyjazne Bluszcze", "Przyjazny Bluszcz", "Przyjaznym Bluszczem")

S("This Orb allows you to grow like an Ivy. "
  "The Ivy is always rooted in your current location; "
  "moving among the Ivy cells will move the root. "
  "Moving to a new location will cause the Ivy to grow "
  ", if an Ivy could make that movement "
  "(otherwise it breaks). "
  "You can also target one of the cells adjacent to your ivy "
  "(not to you) to grow or attack there.",
  
  "Ta Sfera pozwala Ci rosnąć jak bluszcz. "
  "Bluszcz jest zawsze ukorzeniony na Twojej pozycji; "
  "ruch w obrębie komórek bluszcza powoduje przemieszczenie "
  "korzenia. Bluszcz rośnie gdy ruszasz się na nową komórkę "
  "(o ile bluszcz mógłby tam urosnąć, w przeciwnym przypadku "
  "bluszcz się łamie). Możesz również wycelować w jedną z "
  "komórek sąsiadujących z Twoim bluszczem (nie z Tobą), "
  "by tam urosnąć lub zaatakować."
  )

// reptiles
//==========

N("Reptiles", GEN_O, "Jaszczurki", "Jaszczurki", "Jaszczurki", "na Jaszczurkach")
N("Reptile", GEN_F, "Jaszczurka", "Jaszczurki", "Jaszczurkę", "Jaszczurką")
N("Reptile floor", GEN_F, "Jaszczurza podłoga", "Jaszczurze podłogi", "Jaszczurzą podłogę", "Jaszczurzą podłogą")
N("Reptile bridge", GEN_O, "Jaszczurzy most", "Jaszczurze mosty", "Jaszczurzy most", "Jaszczurzym mostem")

S("These reptiles are quite strange creatures. They "
  "spend most of their lives sleeping as floors "
  "that other creatures can walk on. "
  "Sometimes they wake up to hunt their prey, "
  "but they will happily go back to sleep if they "
  "happen to move into a hole on their way. "
  "Your attacks do not kill the Reptiles, but "
  "you can push and stun them.",
  
  "Te jaszczurki to dosyć dziwne stwory. "
  "Większość życia spędzają śpiąc, tworząc "
  "podłogi, po których można chodzić. "
  "Czasami się budzą by zapolować, "
  "ale bardzo chętnie położą się znów do snu, jeśli "
  "natrafią na swojej drodze na dziurę. "
  "Twoje ataki nie zabijają jaszczurek, ale "
  "możesz je pchać i ogłuszać."
  )

N("Dodecahedron", GEN_O, "Dwunastościan", "Dwunastościany", "Dwunastościan", "Dwunastościanem")
S("These dodecahedra made of a mysterious material are the Reptiles' favorite toy.",
  "Te dwunastościany zrobione z tajemniczego materiału to ulubiona zabawka Jaszczurek.")

Orb("Vaulting", "Przeskoku")

S("This Orb allows you to jump over an adjacent monster, killing or stunning it. "
  "You can only vault in a roughly straight line. "
  "Target a cell on the other side to use it.",
  
  "Ta Sfera pozwala przeskoczyć sąsiadującego potwora, zabijając lub ogłuszając go. "
  "Możesz skakać tylko w linii prostej. Wyceluj w pole po drugiej "
  "stronie potwora, by użyć tej Sfery."
  )

S("inventory/kill sorting", "sortowanie skarbów/potworów")
S("first on top", "pierwsze na górze")
S("first on bottom", "pierwsze na dole")
S("last on top", "ostatnie na górze")
S("last on bottom", "ostatnie na dole")
S("by land", "kraina")
S("by number", "liczba")

N("small bush", GEN_O, "krzaczek", "krzaczki", "krzaczek", "krzaczkiem")

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
  "Czujesz coś dziwnego z grawitacją tutaj...")
S(
  "The rug depicts a man in a deep dungeon, unable to leave.",
  "Dywan przedstawia człowieka w głębokich lochach, niezdolnego do ucieczki.")
S(
  "You feel attuned to gravity, ready to face mountains and dungeons.",
  "Jesteś dostrojon%ya1 do grawitacji, gotow%ya1 na góry i lochy.")

// VERSION 9.4
//=============

// not previously translated
S("shift+O to switch anti-aliasing", "shift+O by przełączyć antialiasing")
S("For some reason... cannot attack!", "Z jakiegoś powodu... nie możesz atakować!")
S("Your energy swords get stronger!", "Twoje ostrza energii stały się silniejsze!")
S("You feel even more attuned to the magic of this land!",
  "Jesteś lepiej dostrojony do magii tego świata!")
S("Quite tough, for your first fight.", "Jak na pierwszą walkę -- całkiem trudny przeciwnik.")
S("(You can also use right Shift)\n\n", "(Możesz też użyć prawego Shifta)\n\n")

// Crossroads V
//--------------

N("Crossroads V", GEN_O, "Skrzyżowanie V", "Skrzyżowania V", "Skrzyżowanie V", "na Skrzyżowaniu V")
S("Extremely narrow Crossroads layout.\n", "Skrajnie wąski układ Skrzyżowań.")

// Bull Dash/Prairie common

N("Sleeping Bull", GEN_M, "Śpiący Byk", "Śpiące Byki", "Śpiącego Byka", "Śpiącym Bykiem")

S("Sleeping bulls wake up when you get into distance of two cells from them.",
  "Byki się budzą, gdy się znajdziesz w promieniu dwóch pól od nich.")

N("Raging Bull", GEN_M, "Wściekły Byk", "Wściekłe Byki", "Wściekłego Byka", "Wściekłym Bykiem")

S("Raging Bulls charge in a straight line: on heptagons, when they can choose one of two possible directions, "
  "they choose one closer to your current location. In the case of a tie, the cell where more neighbors is "
  "closer to your current location is chosen; if still a tie, past locations are considered. "
  "They can attack you in any direction, and monsters on their way are attacked even if friendly. "
  "When they crash into something, the obstacle is usually destroyed, and they are stunned for three turns, after "
  "which they charge at you again (in any direction). "
  "Raging Bulls cannot be killed or stunned conventionally.",
  
  "Wściekłe Byki szarżują w linii prostej: na siedmiokątach, gdy mogą wybrać jeden z dwóch kierunków, "
  "wybierają pole bliższe Twojej obecnej pozycji. W przypadku remisu pole z większą liczbą sąsiadów bliżej "
  "Ciebie jest wybierane. W przypadku dalszego remisu, rozważane są Twoje wcześniejsze pozycje. "
  "Byki mogą atakować w dowolnym kierunku; potwory na ich drodze są atakowane nawet jak są przyjazne. "
  "Gdy Byk się z czymś zderzy, przeszkoda jest niszczona, i Byk jest ogłuszony na 3 kolejki, "
  "po czym szarżuje ponownie (w dowolnym kierunku). Wściekłego Byka nie można zabić ani ogłuszyć "
  "zwykłą bronią."
  )

N("Herd Bull", GEN_M, "Stadny Byk", "Stadne Byki", "Stadnego Byka", "Stadnym Bykiem")

S("Herds of these Bulls are running long distances for some reason. They become Raging Bulls if something stops them.",
  "Stada tych Byków biegną długie dystansy z jakiegoś powodu. Gdy coś je zatrzyma, stają się "
  "Wściekłymi Bykami."
  )

N("Gadfly", GEN_M, "Giez", "Gzy", "Gza", "Gzem")
S("Annoying insects. They can awaken Sleeping Bulls.", "Irytujące owady. Mogą budzić Śpiące Byki.")
S("%The1 wakes up %the2.", "%1 budzi %a2.")

S("%The1 crashes into %the2!", "%1 walnął w %a2!")
S("Fire is extinguished!", "Ogień zgasł!")
S("%The1 is filled!", "%1 zosta%ł1 zalan%1y!")

// Prairie
//---------

N("Prairie", GEN_F, "Preria", "Prerie", "Prerię", "na Prerii")
N("Green Grass", GEN_F, "Zielona Trawa", "Zielone Trawy", "Zieloną Trawę", "Zieloną Trawą")
S("You can find safety in some places in the Prairie, but if you want treasures, "
  "they can be found only on the other side of a giant herd of bulls.",
  "Są na Prerii miejsca bezpieczne, ale jeśli szukasz skarbów, "
  "możesz je znaleźć tylko po drugiej stronie wielkiego stada biegnących byków."
  )

Orb("the Bull", "Byka")

S("You get the powers of Shield, Horns, and Thorns after you move two moves in a straight line "
  "with this Orb.",
  
  "Gdy ruszysz się 2 pola w linii prostej z tą Sferą, dostajesz moce Tarczy, Rogów i Cierni.")

// Bull Dash
//-----------

N("Bull Dash", GEN_F, "Kraina Byków", "Krainy Byków", "Krainę Byków", "w Krainie Byków")
N("Butterfly", GEN_M, "Motylek", "Motylki", "Motylka", "Motylkiem")
N("Spinel", GEN_O, "Spinel", "Spinele", "Spinel", "Spinelem")

S(
  "Butterflies don't pursue you -- unless you get next to them, they just spin around the obstacles. "
  "They cannot be killed conventionally, but you get treasure when a Raging Bull crashes into a Butterfly. ",

  "Motylki nie gonią Cię -- kręcą się wokół przeszkód, chyba że jesteś na sąsiednim polu. "
  "Nie można ich zabić zwykłą bronią, ale zdobywasz skarb, gdy Wściekły Byk zderzy się z Motylkiem."
  )

Orb("Horns", "Rogów")

S(
  "After you move while having this Orb, you immediately attack the next cell in the straight line "
  "(or two cells, when moving on a heptagon). This attack is slightly stronger than your normal "
  "attack: it can stun some of the monsters which cannot be killed or stunned normally.",
  
  "Po każdym ruchu z tą Sferą, natychmiast atakujesz kolejne pole w linii prostej "
  "(albo dwa pola, gdy jesteś na siedmiokącie). Ten atak jest trochę silniejszy od Twojego "
  "zwykłego ataku: niektóre potwory niewrażliwe na zwykły atak można w ten sposób ogłuszyć."
  )

S("You pierce %the1.", "Bodziesz %a1.")

// new 3D options
//----------------

S("Y shift", "przesunięcie Y")
S("Don't center on the player character.", "Nie centruj na graczu.")

S("camera rotation", "obróć kamerę")
S("Rotate the camera. Can be used to obtain a first person perspective, "
  "or third person perspective when combined with Y shift.",
  "Obróć kamerę. Można użyć do uzyskania perspektywy z pierwszej osoby, "
  "lub z trzeciej osoby po połączeniu z przesunięciem Y.")

S("ball model", "model kuli")
S("hyperboloid model", "model hiperboloidy")

S("projection in ball model", "projekcja w modelu kuli")
S("This parameter affects the ball model the same way as the projection parameter affects the disk model.",
  "Ten parametr wpływa na model kuli w ten sam sposób, co projekcja wpływa na model dysku.")
  

S("camera rotation in ball model", "obróć kamerę w modelu kuli")
S("Rotate the camera in ball/hyperboloid model.", "Obróć kamerę w modelu kuli/hiperboloidy.")

// extra help
//------------

S("\nSpecial conduct (still valid)\n", "\nDodatkowe wyzwanie (wciąż spełnione):")
S("\nSpecial conduct failed:\n", "\nDodatkowe wyzwanie (przegrane):")
S("Avoid escaping from a discharge (\"That was close\").",
  "Unikaj uciekania przed wyładowaniem (\"było blisko\").")
S("Avoid chopping trees, using Orbs, and non-graveyard monsters in the Haunted Woods.",
  "Unikaj ścinania drzew, używania Sfer, i potworów spoza Cmentarza.")
S("\n\nEasy %1 might disappear when you collect more of its kind.",
  "\n\nŁatw%ya1 %1 może zniknąć podczas gdy zbierasz je w innych miejscach.")
S(" You need to go deep to collect lots of them.", "By zebrać ich dużo, musisz głęboko wejść w krainę.")

S("\nSpawn rate (as prize Orb): %1%/%2\n", "\nCzęstość występowania (jako nagroda): %1%/%2\n")
S("\nSpawn rate (in Hubs): %1%/%2\n", "\nCzęstość występowania (w centrach): %1%/%2\n")

S("Orbs of Safety always appear here, and may be used to escape.\n",
  "Sfery Bezpieczeństwa zawsze można tu znaleźć, by stąd uciec.")
  
S("You may be unable to leave %the1 if you are not careful!\n",
  "Jeśli nie uważasz, możesz nie być w stanie stąd uciec!")

N("Cellular Automaton", GEN_M, "Automat Komórkowy", "Automaty Komórkowe", "Automat Komórkowy", "w Automacie Komórkowym")

S("A land for people wanting to experiment with cellular automata in the HyperRogue grid. "
  "Rules can be given on the command line; the default rules are:\n"
  "-c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "(-c0 or -c1 can be given if the same rule is to be used for hexagonal "
  "and heptagonal cells).",
  "Kraina dla tych, co chcą eksperymentować z automatami komórkowymi w siatce HyperRogue. "
  "Reguły można podawać w linii poleceń; domyślne to\n"
  "-c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "(można podać -c0 lub -c1 by dać te same reguły dla sześciokątów i "
  "siedmiokątów).")

S("compass size", "rozmiar kompasu")

S("Zebra quotient", "przestrzeń ilorazowa zebry")
S("field quotient", "przestrzeń ilorazowa ciała")

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

#undef Orb


