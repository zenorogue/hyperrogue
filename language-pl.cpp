// HyperRogue Polish translation, by Zeno and Tehora Rogue
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
S("To access Hell, collect %1 treasures each of %2 kinds...", "By dostać się do Piekła, znajdź po %1 skarbów każdego z %2 rodzajów...")
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
S("Collect %1 $$$ to access more worlds", "Znajdź %1 $$$, by iść do nowych krain")
S("Collect at least %1 treasures in each of %2 types to access Hell", "Znajdź po %1 skarbów w %2 typach, by się dostać do Piekła")
S("Collect at least %1 Demon Daisies to find the Orbs of Yendor", "Znajdź %1 Czarciego Ziela, by znaleźć Sfery Yendoru")
S("Hyperstone Quest: collect at least %3 %1 in %the2", "Misja alternatywna: znajdź co najmniej %3 skarbów %abl2")
S("Hyperstone Quest completed!", "Misja alternatywna zakończona!")
S("Look for the Orbs of Yendor in Hell or in the Crossroads!", "Szukaj Sfer Yendoru w Piekle albo na Skrzyżowaniu!")
S("Unlock the Orb of Yendor!", "Otwórz Sferę Yendoru!")
S("Defeat %1 enemies to access the Graveyard", "Pokonaj %1 przeciwników, by trafić na Cmentarz")
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

S("Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Space. "
 "To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n",
 "Ruszasz się myszą, klawiaturą numeryczną, qweadzxc, lub hjklyubn. Czekasz naciskając 's' lub '.'. "
 "Obracasz świat strzałkami, PgUp/Dn, lub Space. Naciśnij 'v' by przejść do menu (konfiguracja, tryby specjalne itd.), ESC "
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
 "na czerwone ani z powrotem. Pola zawierające przedmioty są bezbarwne, "
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

// TODO update translation
S("A nasty creature that lives in caves. They don't like you for some reason.",
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

S("A creepy monster who follows you everywhere in the Graveyard and the Cursed Canyon.",
  "Ten odrażający potwór chodzi za Tobą po cmentarzu!") //TODO UPDATE

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
 "Crystal Sages melt at -30 °C, and they can raise the temperature around you from afar.",
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
S("Scores and achievements are not", "Wyniki i osiągnięcia nie są")
S("saved in the Euclidean mode!", "zapamiętywane w tym trybie!")

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

S("Collect %1 $$$ to access even more lands", "Znajdź %1 $$$ by iść do kolejnych krain")

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

S("You have to escape first!", "Musisz najpierw uciec!")
S("There is not enough space!", "Nie ma miejsca!")

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
S("draw the heptagons darker", "siedmiokąty ciemniejsze")
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
N("Pirate Treasure", GEN_O, "Skarb Piratów", "Skarby Piratów", "Skarb Piratów", "Skarbem Piratów")
N("Compass", GEN_O, "Kompas", "Kompasy", "Kompas", "Kompasem")

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

S("%The1 drowns!", "%1 się utopi%ł1!")
S("%The1 falls!", "%1 spad%ł1!")

// these were missing from the translation for some reason

S("Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.",
  "Piekło jest pełne tych jezior pełnych płonącej siarki, w kształcie złych gwiazd...")

// Hardcore Mode
// -------------

S("hardcore mode", "tryb hardcore")

S("One wrong move and it is game over!", "Jeden fałszywy ruch i koniec gry!")
S("Not so hardcore?", "Hardkor to nie to?")

// Shoot'em up Mode
// ----------------

S("shoot'em up mode", "tryb strzelanki")
S("Welcome to the Shoot'em Up mode!", "Witaj w trybie strzelanki!")
S("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move", "F/;/Space/Enter/KP5 = strzał, WASD/IJKL/Numpad = ruch")

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

S("Cannot drop %the1 here!", "Nie możesz tu położyć %a1!")

// Euclidean scores
// ----------------

S(" (E:%1)", " (E:%1)")

S("You cannot attack Rock Snakes directly!", "Nie możesz atakować Skalnych Węży bezpośrednio!")

S("\"I am lost...\"", "\"Zgubiłem się...\"")

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
S("%The1 sinks!", "%1 uton%ął1!")

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
S("%The1 squeaks gratefully!", "%1 pisn%ął1 w podziękowaniu!")
S("%The1 squeaks hopelessly.", "%1 pisn%ął1 bez nadziei.")
S("%The1 squeaks in despair.", "%1 pisn%ął1 rozpaczliwie.")
S("%The1 squeaks sadly.", "%1 pisn%ął1 smutno.")
S("%The1 squeaks with hope!", "%1 pisn%ął1 z nadzieją!")
S("%The1 squeaks happily!", "%1 pisn%ął1 szczęśliwie!")
S("%The1 squeaks excitedly!", "%1 się ekscytuje!")

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
S("Hardness frozen at %1.", "Trudność zamrożona: %1.")
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
S("Finished lands required: %1 (collect %2 treasure)\n",
  "Wymagane ukończone krainy: %1 (zdobądź %2 skarbów)\n")

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
S(" Hell: %1/%2", " Piekło: %1/%2")

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
  "tend to disappear after you have collected many of them.",
  "Piękna figurka z kości słoniowej. Figurki blisko podstawy Wieży znikają, gdy "
  "zbierzesz dużo figurek.")

N("Platform", GEN_F, "Platforma", "Platformy", "Platformę", "Platformą")
N("platform", GEN_F, "platforma", "platformy", "platformę", "platformą")

S("You can stand here.", "Możesz na niej stanąć.")

N("ladder", GEN_F, "drabina", "drabiny", "drabinę", "drabiną")
S("You can use this ladder to climb the Tower.",
  "Użyj tej drabiny, by wspiąć się na Wieżę.")

Orb("Matter", "Materii")

S("This Orb allows one to temporarily create impassable matter, either to block paths or "
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

S("Cannot create temporary matter on a monster!", "Nie można tworzyć tymczasowej materii na potworze!")
S("Cannot create temporary matter on an item!", "Nie można tworzyć tymczasowej materii na przedmiocie!")
S("Cannot create temporary matter here!", "Nie można tu stworzyć tymczasowej materii!")
S("Cannot summon on a monster!", "Nie można przywołać na potworze!")
S("No summoning possible here!", "Przywołanie niemożliwe!")
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

S("Wow! That was close.", "Uff! Było blisko.")

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
  "outgoing wind. However, you can move two cells with the wind in a single turn, "
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

S(  "This Orb lets your allies share your Orb powers.\n\n"
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
  "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n",
  "W tym trybie można grać w HyperRogue na trójwymiarowym modelu części płaszczyzny "
  "hiperbolicznej, podobnym do tego, który można otrzymać w kreatorze modeli "
  "papierowych albo poprzez hiperboliczne szydełkowanie.\n\n")

S("This requires some OpenGL extensions and may crash or not work correctly -- enabling "
  "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
  "will make the rendering faster, but the surface will be rendered only once, so "
  "you won't be able to play a game on it.\n\n",

  "Działanie wymaga pewnych rozszerzeń OpenGL i może działać nieprawidłowo lub "
  "powodować zatrzymanie gry -- w takim przypadku może być przydatne włączenie opcji "
  "'renderuj bez OpenGL'. Opcja 'renderuj raz' spowoduje, że animacja będzie szybsza, "
  "ale tekstura powierzchni zostanie narysowana tylko raz, zatem nie będzie możliwe "
  "granie na niej na żywo.\n\n")

S("Use arrow keys to rotate, Page Up/Down to zoom.", "Możesz obracać model strzałkami, a przybliżać i oddalać go "
  "klawiszami Page Up/Down.")

S("what's this?", "co to jest?")
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
  "Nie daj się zwieść tej rudowłosej, bo jak się za bardzo zbliżysz, możesz zostać ugodzon%ya0 nożem!")
S("Don't be fooled by this red-haired boy, or you will be stabbed if you come too close!",
  "Nie daj się zwieść temu rudowłosemu, bo jak się za bardzo zbliżysz, możesz zostać ugodzon%ya0 nożem!")
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

S("%The1 is killed by thorns!", "%1 wpad%ł1 na kolce i się zabi%ł1!")

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
S("Enable cheat mode or GAME OVER to use this", "Dostępne tylko po końcu gry albo w trybie oszusta")

// remember to note it's in English
S("see http://www.roguetemple.com/z/hyper/models.php", "patrz http://www.roguetemple.com/z/hyper/models.php (po angielsku)")

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
   "but you cannot collect items.\n\n"
   /* "When only one charge is left, "
   "you have to dismount this turn -- be very careful to make this possible, "
   "as your mount could attack you immediately!\n\n" */ "While riding, "
   "click on a location to order your mount to move or attack there.",

   "Ta Sfera pozwala Ci jeździć Smokami i innymi wężopodobnymi stworami. "
   "Po prostu wykonaj ruch na pole z takim stworem, by go dosiąść. "
   "Podczas jazdy jesteś chronion%y0 przed "
   "niebezpiecznym terenem i częściowo przed atakami (które powodują utratę połowy "
   "Twojej mocy Dominacji), ale nie możesz zbierać przedmiotów.\n\n"
//   "Kiedy zostanie "
//   "tylko 1 ładunek, musisz natychmiast zsiąść -- zadbaj o to, by to było "
//   "możliwe bez zostania zaatakowanym przez Twojego dawnego wierzchowca!\n\n"
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

S("Hyperstone Quest: collect at least %3 points in %the2",
  "Misja Hiperkamień: zdobądź co najmniej %3 punktów %abl2")

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
// -------------------------------------------------

S("\n\nThis Orb is triggered on your first attack or illegal move.",
  "\n\nTa Sfera się aktywuje automatycznie, gdy wykonasz atak lub nielegalny ruch.")

S("\nYou can never target cells which are adjacent to the player character, "
     "or ones out of the sight range.",
     "\nNie możesz celować w pola obok postaci ani poza zasięgiem wzroku.")

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
  
  "\n\nTa Sfera chroni Cię przed atakami, zapachami i izoluje od "
  "elektryczności. Nie pozwala iść przez zabójczy teren, ale "
  "jeśli zostaniesz zaatakowan%y0 ogniem, możesz stać w miejscu.")

S("Completing the quest in this land is not necessary for the Hyperstone Quest.",
  "Wykonanie tego zadania nie jest konieczne do zdobycia Hiperkamieni.")

// other options
// -------------

S("player", "gracz")
S("movement color", "kolor ruchu")
S("model used (not conformal!)", "użyty model (nie konforemny!)")
S("azimuthal equidistant", "azymutalny, równe d")
S("azimuthal equi-area", "azymutalny, równe pola")
S("visit the website", "idź na stronę HyperRogue")
S("leaderboards/achievements", "osiągnięcia i rankingi")
S("return to the game", "powrót do gry")

// Android texts
// -------------

S("connected to Google Games", "połączenie z Google Games")
S("background music", "muzyka w tle")
S("view your achievements", "pokaż osiągnięcia")
S("next page", "kolejna strona")

// new multiplayer
// ---------------

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
"szybciej, noże wracają wolniej, a gracze nie mogą się rozdzielać.\n\n")

S(
"Turn-based multiplayer: Turns are executed in parallel. A player can leave the game "
"by pressing a designated key (useful when about to get killed or lost). The following "
"Orbs work to bring such players back: ",
"Turowo na wielu graczy: tury wykonujecie równolegle. Gracz może opuścić grę, "
"naciskając wyznaczony klawisz (użyteczne, gdy się zgubi lub gdyby zaraz został zabity). "
"Aby taki gracz wrócił, można użyć następujących Sfer:")

S("This menu can be also used to configure keys.\n\n",
  "To menu może być też użyte do konfiguracji klawiszy.")

// other stuff
// -----------

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


// Kraken Depths
// -------------

N("Kraken Depths", GEN_F, "Głębiny Krakenów", "Głębiny Krakenów", "Głębiny Krakenów", "w Głębinach Krakenów")

S(
    "A long time ago, this was a trade route. But then, Krakens have risen out of the "
    "depths. Many trading ships sank here. Legend says that you can uncover the secret "
    "of a magical weapon spell somewhere in the depths...\n\n"
    
    "You can find Sunken Treasures here, but they won't appear until you have killed "
    "a Kraken. You will also need Orb of the Fish to get the treasures, luckily you can "
    "steal one from the Viking treasure hunters.",
    
    "Dawno, dawno temu, był to szlak handlowy, ale Krakeny wypłynęły z głębin. Wiele "
    "statków handlowych zatonęło. Według legendy, gdzieś w głębinach ukryty jest sekret "
    "magicznej broni...\n\n"
    "Możesz tu znaleźć Zatopione Skarby, ale nie będą się one pojawiać, póki nie pokonasz "
    "Krakena. By wyłowić skarby, potrzebna jest Sfera Ryby. Na szczęście można ukraść "
    "ją wikińskim łowcom skarbów.")

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
  
  "Tam, skąd pochodzisz, również są Krakeny... wielkie morskie potwory, które "
  "z łatwością mogą zatapiać statki. Geometria tego dziwnego świata powoduje, "
  "że wielkie obiekty nie mogą się szybko poruszać, nie ma tu zatem dużych statków, "
  "tylko małe łódki, a hiperboliczne Krakeny też są względnie małe. Wciąż one są "
  "chyba największymi stworzeniami, mogącymi się poruszać z rozsądną "
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
    
    "Ta Sfera daje Ci ostrze zbudowane z czystej magicznej energii. Nie trzymasz "
    "go w ręce, po prostu unosi się ono w powietrzu obok Ciebie. Kiedy się poruszasz, "
    "ostrze energii porusza się razem z Tobą, pozostając skierowane pod tym samym "
    "kątem względem Twojego ruchu co wcześniej -- nie możesz nim obracać, "
    "ani poruszać w inny sposób. Większość potworów ginie, gdy uderzysz w nie ostrzem, "
    "nie wejdą one też na pole, na którym ostrze jest.")

N("Sunken Treasure", GEN_O, "Zatopiony Skarb", "Zatopione Skarby", "Zatopiony Skarb", "Zatopionym Skarbem")

S("Cargo of a ship which was once destroyed by a Kraken.", 
  "Ładunek statku zatopionego dawno temu przez Krakena.")

// Burial Grounds
// --------------

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

// Trollheim
// ---------

N("Trollheim", GEN_O, "Trollheim", "Trollheim", "Trollheim", "w Trollheim")

S(
    "Many clans of Trolls spend their lives in this kingdom. You can find many "
    "statues of Trolls here. You suppose that they are not actually statues, but simply "
    "elderly Trolls, who have petrified upon death. Or maybe you have killed "
    "these Trolls yourself?",
    
    "Wiele klanów Trolli żyje w tym królestwie. Można tu znaleźć mnóstwo pomników "
    "trolli albo raczej nie pomników, tylko po prostu stare Trolle, które "
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
    "przez Sferę Kamienia mają nieco inne własności niż Trolle spetryfikowane naturalnie.")

N("stone statue", GEN_O, "kamienny pomnik", "kamienne pomniki", "kamienny pomnik", "kamiennym pomnikiem")

S("A petrified creature.", "Stworzenie zamienione w kamień.")

S("mark heptagons", "oznaczenia na siedmiokątach")
S("help for keyboard users", "pomoc dla użytkowników klawiatury")

// missing
// -------

S("You leave %the1.", "Zostawiasz %a1.") // Baby Tortoise

S("%The1 says, \"I die, but my clan in Trollheim will avenge me!\"",
  "%1 mówi: \"Ja umieram, ale mój klan w Trollheim mnie pomści!\"")

S("After killing %the1, you feel able to reach the Elemental Planes!",
  "Po pokonaniu %a1 czujesz, że masz dostęp do Strefy Żywiołów!")

S("You feel that a magical weapon is waiting for you...",
  "Czujesz, że magiczna broń na Ciebie czeka...")

// missing
// -------

S("reset per-player statistics", "zresetuj statystyki graczy")
S("An alternate layout of the Crossroads, without walls.", "Alternatywny układ Skrzyżowań, bez ścian.")
S("cheats", "oszustwa")

S("Known mines may be marked by pressing 'm'. Your allies won't step on marked mines.", 
  "Znane miny można oznaczać, naciskając 'm'. Twoi przyjaciele nie będą stawać na oznaczonych minach.")

S("Known mines may be marked by touching while in drag mode. Your allies won't step on marked mines.", 
  "Znane miny można oznaczać, dotykając ich w trybie przeciągania. Twoi przyjaciele nie będą stawać na oznaczonych minach.")

// 9.0 patches
// ===========

// extra help
// ----------

S("Coastal region -- connects inland and aquatic regions.\n",
  "Kraina przybrzeżna -- łączy krainy lądowe i morskie.\n")

S("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n",
  "Kraina morska -- dostępna jedynie z krain przybrzeżnych i morskich.\n")

// barrow treasure protected from Orb of Space
// -------------------------------------------

S("%The1 is protected from this kind of magic!",
  "Tego typu magia nie działa na %1!")

// Orb is always available in the given land
// -----------------------------------------

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
    "możesz znaleźć magiczną Sferę tej krainy i w niektórych przypadkach "
    "uzyskać dostęp do nowych krain. Gdy zbierzesz 25 skarbów, "
    "ten typ Sfer będzie się też pojawiał w innych krainach. "
    "Naciśnij '0', by obejrzeć szczegóły na temat wszystkich Krain.\n\n"
    )

// welcome messages
// ----------------

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
// -------------------------------------

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
// ------------------------------------------------------

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
S("go back", "powrót")
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
// ===========

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
  "\"Nie posiadanie, ale proces docierania na miejsce ")
  
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

S("Killed %1 can be revived with an Orb of Love, after you collect 20 more $$$.",
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
  "lub eliptycznej ('o' by zmienić). Grasz na powierzchni wydrążonej dyni "
  "i musisz zdobyć jak najwięcej Cukierków. Każdy Cukierek przynosi "
  "nowe potwory do walki i nowe magiczne moce. Walcz z potworami, "
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
S("You gain a protective Shell!", "Zdoby%łeś0 ochronną Skorupę!")
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
// ===========

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
  "but the actual game mechanics are not affected. ",

  "Poziom podłoża jest w rzeczywistości ekwidystantną powierzchnią "
  "%1 jednostek pod płaszczyzną P. Teoretycznie, ta wartość "
  "wpływa na świat -- przykładowo, orły mogłyby latać %2 razy "
  "szybciej, latając powyżej poziomu podłoża na wysokości płaszczyzny "
  "P -- ale nie wpływa to na mechanikę gry w żaden sposób. ")
  
  
S("(Distances reported by the vector graphics editor "
  "are not about points on the ground level, but "
  "about the matching points on the plane P -- "
  "divide them by the factor above to get actual "
  "distances.)",
  
  "(Odległości wyświetlane przez edytor grafiki "
  "dotyczą odległości między odpowiednimi punktami na płaszczyźnie P.)")

S( "If we are viewing an equidistant g absolute units below a plane, "
   "from a point c absolute units above the plane, this corresponds "
   "to viewing a Minkowski hyperboloid from a point "
   "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
   "the Poincaré model for g=c, and Klein-Beltrami model for g=0.",
   
   "Jeśli ekwidystantną powierzchnię g jednostek pod płaszczyzną oglądamy "
   "z punktu na wysokości c, odpowiada to oglądaniu hiperboloidy Minkowskiego "
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
// --------------------------

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
// -------

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
  
  "Nie ma. Nietoperze nie mogą Ci nic zrobić, ale blokują Twój ruch "
  "i mogą przestawiać przełączniki, gdy na nie spadną."
  )

Orb("Recall", "Powrotu")

S("When the charges on this Orb expire, "
  "you will be automatically returned to the place where you have found it. "
  "Extra Orbs of Recall delay this without changing the recall location. "
  "Picking up an Orb of Safety causes an immediate recall.",
  
  "Kiedy skończą się ładunki tej Sfery, automatycznie wracasz do "
  "miejsca, w którym ją znalazłeś. Dodatkowe Sfery Powrotu "
  "opóźniają powrót bez zmiany miejsca powrotu, a Sfera Bezpieczeństwa "
  "powoduje natychmiastowy powrót."
  )

// 
//

// Lost Mountain
// -------------

//N("Pyramid", GEN_F, "Piramida", "Piramidy", "Piramidą", "na Piramidzie")
N("Lost Mountain", GEN_F, "Zagubiona Góra", "Zagubione Góry", "Zagubioną Górą", "na Zagubionej Górze")

/*
S("An ancient civilization has created this pyramid in the Jungle by "
  "changing the gravity. The gravity is still changed, but "
  "the pyramid is overgrown with ivies and bushes. "
  "Will you dare to climb the ivies to get treasures waiting above?",
  
  "Starożytna cywilizacja stworzyła tę piramidę w Dżungli, zmieniając "
  "grawitację. Grawitacja nadal jest zmieniona, ale piramida zarosła "
  "bluszczem i krzakami. Odważysz się wspiąć po bluszczu, aby zdobyć "
  "skarby czekające wyżej?"
  )
*/
  
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
  "korzenia. Bluszcz rośnie, gdy ruszasz się na nową komórkę "
  "(o ile bluszcz mógłby tam urosnąć, w przeciwnym przypadku "
  "bluszcz się łamie). Możesz również wycelować w jedną z "
  "komórek sąsiadujących z Twoim bluszczem (nie z Tobą), "
  "by tam urosnąć lub zaatakować."
  )

// Reptiles
// --------

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
  "Większość życia spędzają śpiąc -- tworzą wtedy "
  "podłogi, po których można chodzić. "
  "Czasami się budzą, by zapolować, "
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
  "Czujesz się przystosowan%ya1 do grawitacji, gotow%ya1 na góry i lochy.")

// VERSION 9.4
// ===========

// not previously translated
S("shift+O to switch anti-aliasing", "shift+O by przełączyć antialiasing")
S("For some reason... cannot attack!", "Z jakiegoś powodu... nie możesz atakować!")
S("Your energy swords get stronger!", "Twoje ostrza energii stały się silniejsze!")
S("You feel even more attuned to the magic of this land!",
  "Jesteś lepiej dostrojony do magii tego świata!")
S("Quite tough, for your first fight.", "Jak na pierwszą walkę -- całkiem trudny przeciwnik.")
S("(You can also use right Shift)\n\n", "(Możesz też użyć prawego Shifta)\n\n")

// Crossroads V
// ------------

N("Crossroads V", GEN_O, "Skrzyżowanie V", "Skrzyżowania V", "Skrzyżowanie V", "na Skrzyżowaniu V")
S("Extremely narrow Crossroads layout.\n", "Skrajnie wąski układ Skrzyżowań.")

// Bull Dash/Prairie common
// ------------------------

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
  "wybierają pole bliższe Twojej obecnej pozycji. W przypadku remisu wybierane jest pole z większą liczbą sąsiadów bliżej "
  "Ciebie. W przypadku dalszego remisu, rozważane są Twoje wcześniejsze pozycje. "
  "Byki mogą atakować w dowolnym kierunku; potwory na ich drodze są atakowane nawet, jak są przyjazne. "
  "Gdy Byk się z czymś zderzy, przeszkoda jest niszczona, a Byk jest ogłuszony na 3 kolejki, "
  "po czym szarżuje ponownie (w dowolnym kierunku). Wściekłego Byka nie można zabić, ani ogłuszyć "
  "zwykłą bronią."
  )

N("Herd Bull", GEN_M, "Stadny Byk", "Stadne Byki", "Stadnego Byka", "Stadnym Bykiem")

S("Herds of these Bulls are running long distances for some reason. They become Raging Bulls if something stops them.",
  "Stada tych Byków z jakiegoś powodu biegną długie dystansy. Gdy coś je zatrzyma, stają się "
  "Wściekłymi Bykami."
  )

N("Gadfly", GEN_M, "Giez", "Gzy", "Gza", "Gzem")
S("Annoying insects. They can awaken Sleeping Bulls.", "Irytujące owady. Mogą budzić Śpiące Byki.")
S("%The1 wakes up %the2.", "%1 budzi %a2.")

S("%The1 crashes into %the2!", "%1 walnął w %a2!")
S("Fire is extinguished!", "Ogień zgasł!")
S("%The1 is filled!", "%1 zosta%ł1 zalan%1y!")

// Prairie
// -------

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
// ---------

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
// --------------

S("Y shift", "przesunięcie Y")
S("Don't center on the player character.", "Nie centruj na graczu.")

S("camera rotation", "obróć kamerę")
S("Rotate the camera. Can be used to obtain a first person perspective, "
  "or third person perspective when combined with Y shift.",
  "Obróć kamerę. Można użyć do uzyskania perspektywy z pierwszej osoby "
  "lub z trzeciej osoby po połączeniu z przesunięciem Y.")

S("ball model", "model kuli")
S("hyperboloid model", "model hiperboloidy")

S("projection in ball model", "projekcja w modelu kuli")
S("This parameter affects the ball model the same way as the projection parameter affects the disk model.",
  "Ten parametr wpływa na model kuli w ten sam sposób, co projekcja wpływa na model dysku.")
  

S("camera rotation in ball model", "obróć kamerę w modelu kuli")
S("Rotate the camera in ball/hyperboloid model.", "Obróć kamerę w modelu kuli/hiperboloidy.")

// extra help
// ----------

S("\nSpecial conduct (still valid)\n", "\nDodatkowe wyzwanie (wciąż aktualne):")
S("\nSpecial conduct failed:\n", "\nDodatkowe wyzwanie (przegrane):")
S("Avoid escaping from a discharge (\"That was close\").",
  "Unikaj uciekania przed wyładowaniem (\"było blisko\").")
S("Avoid chopping trees, using Orbs, and non-graveyard monsters in the Haunted Woods.",
  "Unikaj ścinania drzew, używania Sfer i potworów spoza Cmentarza.")
S("\n\nEasy %1 might disappear when you collect more of its kind.",
  "\n\nŁatw%ya1 %1 może zniknąć, podczas gdy zbierasz je w innych miejscach.")
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
  "(można podać -c0 lub -c1, by dać te same reguły dla sześciokątów i "
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

// additional texts for 9.4k
// -------------------------

N("Tortoise", GEN_M, "Żółw", "Żółwie", "Żółwia", "Żółwiem")
S("line patterns", "wzory linii")
S("1 turn", "1 kolejka")
S("%1 turns", "kolejek: %1")
S("inventory/kill mode", "tryb rzeczy/zabić")
S("images", "obrazki")
S("letters", "literki")
S("input", "sterowanie")
S("default", "domyślne")
S("config", "ustawione")
S("expansion", "rozszerzanie")

S("continue", "kontynuuj")
S("main menu", "menu główne")
S("restart", "nowa gra")
S("quit", "koniec")
S("exit menu", "wyjdź z menu")

S(
  "A tourist from another world. They mutter something about the 'tutorial', "
  "and claim that they are here just to learn, and to leave without any treasures. "
  "Do not kill them!",
  
  "Turyst%aka0 z innego świata. Coś mamrocze o 'podręczniku', że "
  "się tylko uczy i nie jest tu po skarby. Nie zabijać!")

S(
  "This monster has come from another world, presumably to steal our treasures. "
  "Not as fast as an Eagle, not as resilient as the guards from the Palace, "
  "and not as huge as the Mutant Ivy from the Clearing; however, "
  "they are very dangerous because of their intelligence, "
  "and access to magical powers.\n\n",

  "Ten potwór przybył z innego świata, prawdopodobnie po nasze skarby. "
  "Nie taki szybki jak Orzeł, nie taki odporny jak strażnicy Pałacu, "
  "i nie taki wielki jak Zmutowany Bluszcz z Polany; jest jednak "
  "bardzo niebezpieczny ze względu na inteligencję i "
  "dostęp do mocy magicznych.\n\n"
  )

S(
  "Actually, their powers appear god-like...\n\n",
  "Jego moce wyglądają właściwie na boskie...\n\n"
  )

S(
  "Rogues will never make moves which result in their immediate death. "
  "Even when cornered, they are able to instantly teleport back to their "
  "home world at any moment, taking the treasures forever... but "
  "at least they will not steal anything further!\n\n",
  
  "Cwaniacy nigdy nie robią ruchów, po których zostaliby od razu zabici. "
  "Nawet gdy się ich otoczy, są zdolni natychmiast teleportować się "
  "i wrócić do świata, skąd pochodzą, zabierając skarby na zawsze... "
  "ale przynajmniej nie ukradną już nic więcej!\n\n")

S(
  "Despite this intelligence, Rogues appear extremely surprised "
  "by the most basic facts about geometry. They must come from "
  "some really strange world.\n\n",
  
  "Mimo ich inteligencji Cwaniacy wyglądają na szalenie zdziwionych "
  "nawet najprostszymi faktami geometrycznymi. Muszą pochodzić z jakiegoś "
  "bardzo dziwnego świata.")  

S("change the alpha parameter to show the lines",
  "zmień parametr alfa by linie były widoczne")

S("triangle grid: not rings", "trójkątna siatka: nie pierścienie")
S("triangle grid: rings", "trójkątna siatka: pierścienie")
S("heptagonal grid", "siatka siedmiokątna")
S("rhombic tesselation", "posadzka rombowa")
S("triheptagonal tesselation", "posadzka trójkątno-siedmiokątna")
S("normal tesselation", "zwykła posadzka")
S("big triangular grid", "duża siatka trójkątna")
S("underlying tree", "drzewo bazowe")
S("circle/horocycle tree", "drzewo bazowe dla kół/horocykli")
S("zebra triangles", "zebra: trójkąty")
S("zebra lines", "zebra: linie")
S("vineyard pattern", "wzór winnicy")
S("firewall lines", "linie ścian ognia")
S("firewall lines: Palace", "linie ścian ognia: Pałac")
S("firewall lines: Power", "linie ścian ognia: Moc")

S("(ESC) tour menu", "(ESC) menu ucznia")
S("Try the Guided Tour to help with understanding the "
  "geometry of HyperRogue (menu -> special modes).\n\n",
  "Uruchom Wycieczkę, by zrozumieć geometrię HyperRogue (menu -> tryby specjalne).\n\n")

S("guided tour", "Wycieczka")
S("spherical geometry", "geometria sferyczna")
S("Euclidean geometry", "geometria euklidesowa")
S("more curved hyperbolic geometry", "większa krzywizna")
S("teleport away", "uciekaj stąd (teleport)")
S("Shift-click a location to teleport there.", "Shift-kliknij pole, by tam się teleportować.")
S("Click a location to teleport there.", "Kliknij pole, by tam się teleportować.")

S("move through walls", "przechodź przez ściany")
S("flash", "błysk")
S("static mode", "tryb statyczny")
S("Static mode enabled.", "Tryb statyczny włączony.")
S("Static mode disabled.", "Tryb statyczny wyłączony.")
S("enable/disable texts", "włącz/wyłącz teksty")
S("Help texts disabled.", "Teksty pomocy wyłączone.")
S("next slide", "kolejny slajd")
S("previous slide", "poprzedni slajd")

S("This guided tour is different than most other game tutorials -- "
  "you are not forced to do anything, and you can go wherever you want.\n\n"
  "However, %the1 is not what we are talking about now. "
  "We will not explain this land at the moment, and you could potentially "
  "get lost there.\n\n"
  "Remember that you can get to the next slide by pressing Enter.",
  
  "Ta wycieczka różni się od większości podręczników w grach -- "
  "nie masz obowiązku robić tego, co tutorial sugeruje, możesz iść, dokąd chcesz.\n\n"
  "Ale %1 nie jest miejscem, o którym teraz mówimy. Nie wyjaśnimy go "
  "i potencjalnie możesz się zgubić.\n\n"
  "Naciśnij Enter, by przejść do kolejnego slajdu.")

S("Introduction", "Wstęp")
S("Welcome to the HyperRogue Guided Tour!", "Witaj w Wycieczce HyperRogue!")
S(
  "This tour is mostly aimed to show what is "
  "special about the geometry used by HyperRogue. "
  "It also shows the basics of gameplay, and "
  "how is it affected by geometry.\n\n"
  "You decide when you want to stop playing with the "
  "current \"slide\" and go to the next one, by pressing Enter. You can also "
  "press ESC to see a "
  "menu with other options.",
  
  "Zadaniem tej wycieczki jest pokazanie szczególnych własności "
  "geometrii używanej przez HyperRogue; są też pokazane podstawy "
  "rozgrywki i jak geometria na nie wpływa.\n\n"
  "Ty decydujesz, kiedy chcesz skończyć bawić się obecnym \"slajdem\" "
  "i przejść do kolejnego, naciskając Enter. Naciśnij ESC, by zobaczyć "
  "pozostałe opcje.")

S("Basics of gameplay", "Podstawy rozgrywki")
S("gain Ice Diamonds", "daj Lodowe Diamenty")
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
  "Gra rozpoczyna się w Lodowej Krainie. Zbieraj Lodowe Diamenty "
  "(naciśnij F1, jeśli nie wiesz, jak się ruszać). "
  "Po zebraniu wielu Diamentów ataki potworów staną się "
  "wyzwaniem. Zgodnie z konwencjami gier roguelike, "
  "jeśli przegrasz, zaczynasz od początku; "
  "w tym podręczniku jednak wystarczy nacisnąć '4', "
  "by uciec ze złej sytuacji. Podręcznik jest "
  "podkręcony tak, by pokazać to, co powinien -- "
  "na przykład tutaj możesz nacisnąć '5', by "
  "zdobyć szybko dużo diamentów.")

S("Hypersian Rug model", "Model Hiperskiego Dywanu")
S(
  "New players think that the action of HyperRogue takes place on a sphere. "
  "This is not true -- the next slide will show the surface HyperRogue "
  "actually takes place on.\n\n"
  "Use arrow keys to rotate the model, and Page Up/Down to zoom.\n\n"
  "If you do not see anything, press '5' to try a safer renderer.",
  

  "Nowi gracze myślą, że akcja HyperRogue toczy się na sferze. "
  "To nieprawda -- kolejny slajd pokazuje powierzchnię, "
  "na której toczy się gra. "
  "Obracaj modelem strzałkami, Page Up/Down by skalować.\n\n"
  "Jeśli nic nie widzisz, naciśnij '5' dla bezpieczniejszego renderera."
  )

S("Expansion", "Ekspansja")
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
  
  "Kolejny slajd pokazuje liczbę pól w odległości 1, 2, 3, ... od Ciebie. "
  "Rośnie ona wykładniczo: jest ponad 10^100 pól w promieniu 1000 pól od Ciebie, "
  "a w trakcie gry będziesz odchodzić jeszcze dalej!\n\n"
  "Jest to bardzo ważne dla designu HyperRogue. Są tu zagadki nawigacyjne -- "
  "coś, co by było proste w świecie euklidesowym, wymaga przemyślenia "
  "w geometrii hiperbolicznej (chcesz dostać się do miejsca 20 pól od Ciebie, "
  "który z tysiąca możliwych kierunków wybrać?); z kolei inne rzeczy stają się "
  "tu łatwe. Gra HyperRogue jest zaprojektowana tak, by nie dało się "
  "'grindować' nieskończonego świata -- prawie nie ma permanentnych ulepszeń, "
  "zbieranie skarbów przynosi pożytki, ale zdobycie wielu jednego rodzaju "
  "jest skrajnie niebezpieczne.")

S("Tiling and Tactics", "Posadzka i taktyka")
S(
  "The tactics of fighting simple monsters, such as the Yetis from the Icy Lands, "
  "might appear shallow, but hyperbolic geometry is essential even there. "
  "In the next slide, you are attacked by two monsters at once. "
  "You can make them line up simply by "
  "running away in a straight line. "
  "Press '2' to try the same in the Euclidean world -- it is impossible.",
  "Taktyka walki z prostymi potworami, jak Yeti z Krainy Lodu, może się "
  "wydawać płytka, ale geometria hiperboliczna jest istotna nawet tu. "
  "W następnym slajdzie atakują Cię 2 potwory naraz. Możesz uciec, po prostu "
  "odchodząc od nich w linii prostej. Naciśnij '2', by spróbować tego "
  "w świecie euklidesowym -- jest to niemożliwe.")

S("Straight Lines", "Linie proste")
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
  
  "Geometria hiperboliczna została odkryta przez dziewiętnastowiecznych matematyków "
  "zainteresowanych naturą równoległości. Weź linię prostą L i punkt A. Czy może "
  "istnieć świat, w którym jest więcej niż jedna linia prosta przechodząca przez A, "
  "ale nie przecinająca L?\n\n"
  "Kraina Lodu jest bardzo niebezpieczna, gdy masz mnóstwo Diamentów. "
  "Ale inne krainy, w których jeszcze nie masz skarbów, są w miarę bezpieczne.\n\n"
  "Zwiedzaj dalej, a szybko znajdziesz Skrzyżowanie -- jego ściany są "
  "liniami prostymi i działają inaczej niż w geometrii euklidesowej. "
  "Po drugiej stronie Wielkich Ścian widzisz inne krainy -- jest ich około 60, "
  "opartych na róznych aspektach geometrii hiperbolicznej.")

S("Running Dogs", "Biegnące Psy")
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
  "By dowiedzieć się więcej o liniach prostych, "
  "podróżuj dalej, aż znajdziesz Krainę Wiecznego Ruchu. "
  "Biegnij w linii prostej z Psem biegnącym obok. "
  "Mimo że Pies biegnie z tą samą prędkością, nie będzie "
  "w stanie Cię dogonić -- ponieważ Ty ruszasz się w linii prostej, "
  "a Pies biegnie po krzywej zwanej ekwidystantą.\n\n"
IF_MAC(
 "Pamiętaj, że możesz klikać z prawym Shiftem na różnych elementach gry, by dowiedzieć się o nich więcej."
,
 "Pamiętaj, że możesz klikać prawym przyciskiem na różnych elementach gry, by dowiedzieć się o nich więcej."
)
  )

S("Equidistants", "Ekwidystanty")
S(
  "Equidistants are curves which are at some fixed distance from a "
  "straight line. Some lands in HyperRogue are based on equidistants; "
  "you should see them after wandering a bit more.\n\n"
  "This tutorial gives you freedom to go wherever you choose, "
  "but we do not recommend going deep into the Dungeon or the Ocean -- "
  "getting back might be difficult.",
  
  "Ekwidystanty to krzywe w stałej odległości od linii prostej. "
  "Niektóre krainy w grze są oparte na ekwidystantach; "
  "po krótkiej podróży je zobaczysz.\n\n"
  "Ten podręcznik pozwala Ci się ruszać dokąd chcesz, "
  "ale nie polecamy zagłębiać się w Lochy ani w Ocean -- "
  "powrót może być trudny.")

S("Circles", "Koła")
S(
  "Circles are strange in hyperbolic geometry too. "
  "Look for the Castle of Camelot in the Crossroads; "
  "the Round Table inside is a circle of radius 28. "
  "Finding its center is a difficult challenge.\n\n"
  "Press '5' to cheat by seeing the smaller circles too.\n\n"
  "Note: Camelot and some other lands are unlocked earlier in the Tutorial than in a real game.",

  "Koła również są dziwne w geometrii hiperbolicznej. "
  "Szukaj zamku Camelot na Skrzyżowaniu; Okrągły Stół "
  "jest kołem o promieniu 28. Znalezienie jego środka "
  "to trudne wyzwanie.\n\n"
  "Naciśnij '5', by oszukać -- zobaczyć mniejsze okręgi.\n\n"
  "Uwaga: Camelot i niektóre inne krainy w tym podręczniku są pokazywane wcześniej "
  "niż w normalnej grze.")

S("Horocycles", "Horocykle")
S("Horocycles are similar to circles, but you cannot reach their center at all -- "
  "they can be understood as limit circles of infinite radius centered in some point "
  "in infinity (also called an ideal point).\n\n"
  "Go to R'Lyeh, and you should quickly find a Temple of Cthulhu there. "
  "Each circle of columns is actually a horocycle. Horocycles in a given "
  "temple are concentric, and there is an infinite number of them.",

  "Horocykle są podobne do okręgów, ale nie możesz dotrzeć do ich środka -- "
  "są to graniczne okręgi o nieskończonym promieniu, których środek jest w nieskończoności "
  "(takie punkty nazywamy punktami idealnymi).\n\n"
  "Idź do R'Lyeh, a szybko znajdziesz Świątynię Cthulhu.  Każdy krąg kolumn "
  "jest horocyklem. Horocykle w danej świątyni są współśrodkowe i jest ich "
  "nieskończenie wiele.")

S("Half-plane model", "Model półpłaszczyzny")
S("The game is normally displayed in the so called Poincaré disk model, "
  "which is a kind of a map of the infinite hyperbolic world. "
  "There are many projections of Earth, but since Earth is curved, "
  "all of them have to distort distances or angles in some way -- "
  "the same is true in hyperbolic geometry. "
  "The next slide shows another model, called the Poincaré upper half-plane model. In this model, "
  "horocycles centered at one specific ideal point are drawn as straight lines.",
  
  "Gra jest normalnie w tak zwanym modelu Poincaré, rodzaju mapy nieskończonego "
  "hiperbolicznego świata. Istnieje wiele rodzajów rzutu Ziemi, ale "
  "ze względu na jej krzywiznę, każdy z nich przekłamuje odległości czy kąty "
  "w jakiś sposób -- to samo dzieje się w geometrii hiperbolicznej. "
  "Następny slajd pokazuje inny model -- model półpłaszczyzny. "
  "W tym modelu horocykle o środku w danym punkcie idealnym wyglądają jak linie "
  "poziome.")

S("Curvature", "Krzywizna")
S("gain Orb of the Sword", "zdobądź Sferę Ostrza")
S(
  "Now, go to the Burial Grounds and find an Orb of the Sword. The Sword appears to "
  "always be facing in the same direction whatever you do, and it appears that "
  "you have to rotate the sword to excavate the treasures; "
  "yet, it is possible to excavate them! You might have already noticed "
  "that the world rotates after you move around a loop and return to an old "
  "place.\n\n"
  "This is related to the fact that the world of HyperRogue is curved, and "
  "the sum of angles in a triangle is not equal to 180 degrees.",

  "Teraz idź do Kurhanów i znajdź Sferę Ostrza. Ostrze wygląda, jakby zawsze było "
  "skierowane w tę samą stronę, a wydaje się, że do wykopania skarbów trzeba "
  "je obrócić. Jednak wykopanie skarbów jest możliwe! Być może już zauważy%łeś0, "
  "że świat się obraca, gdy zrobisz pętlę.\n\n"
  "Jest to związane z zakrzywieniem świata HyperRogue i tym, że suma kątów "
  "trójkąta nie jest równa 180 stopni.")

S("Periodic patterns", "Wzory okresowe")
S("Hyperbolic geometry yields much more interesting periodic patterns "
  "than Euclidean.",
  "W geometrii hiperbolicznej są dużo bardziej ciekawe regularne wzory "
  "niż w euklidesowej.")

S("Periodic patterns: application", "Zastosowanie wzorów okresowych")
S("Many lands in HyperRogue are based around periodic patterns. "
  "For example, both Zebra and Windy Plains are based on the pattern "
  "shown in the previous slide. "
  "Such lands often have tree-like nature.",
  "Wiele krain jest opartych na okresowych wzorach. Na przykład Zebra "
  "i Wietrzna Równina są obie oparte na wzorze z poprzedniego slajdu. "
  "Krainy takie mają zwykle drzewiastą strukturę.")

S("Fractal landscapes", "Fraktalne krajobrazy")
S("On the following slide, the colors change smoothly in the whole infinite world. "
  "Again, this works better than in Euclidean geometry.",
  "Na kolejnym slajdzie kolory się płynnie zmieniają w nieskończonym świecie. "
  "To również działa lepiej niż w geometrii euklidesowej.")

S("Fractal landscapes: application", "Zastosowanie fraktalnych krajobrazów")
S("create a baby tortoise", "stwórz żółwika")
S(
  "This is applied in HyperRogue to create landscapes, such as the chasms in the "
  "land of Reptiles or the Dragon Chasms, which you should find quickly. "
  "Also in the Dragon Chasms, you can find a Baby Tortoise, and try to find "
  "a matching adult tortoise in the Galápagos. "
  "There are over two millions of species, but since there is so much space in "
  "hyperbolic geometry, finding a matching tortoise is possible. The brighter "
  "the color in Galápagos is, the more aspects of the tortoises in the given "
  "area are matching.",
  
  "Jest to używane w grze do stworzenia krajobrazów, takich jak przepaści w "
  "Jaszczurkach lub Smoczych Otchłaniach, które niedługo znajdziesz. "
  "W Smoczych Otchłaniach możesz znaleźć Żółwika i poszukać "
  "pasującego dorosłego żółwia w Galápagos. Jest ponad 2 miliony gatunków, "
  "ale ze względu na ilość miejsca w geometrii hiperbolicznej, znalezienie "
  "pasującego żółwia jest możliwe. Im jaśniejszy kolor w Galápagos, tym "
  "więcej aspektów żółwika pasuje do żółwii z danego terenu.")

S("Poincaré Ball model", "Model kuli")
S(
    "The Poincaré disk model is a model of a hyperbolic *plane* -- you "
    "might wonder why are the walls rendered in 3D then.\n\n"
    "HyperRogue actually assumes that the floor level is an equidistant surface "
    "in a three-dimensional hyperbolic world, and the camera is placed above the "
    "plane that the surface is equidistant to (which boils down to showing "
    "the floor level in Poincaré disk model).\n\n"
    "This is shown on the next slide, in the Poincaré ball model, which is "
    "the 3D analog of the Poincaré disk model.",
    
  "Model dysku Poincaré to model *płaszczyny* hiperbolicznej -- "
  "być może się zastanawiasz, jak w takim razie rozumieć ściany w 3D.\n\n"
  "HyperRogue zakłada, że powierzchnia podłogi to powierzchnia ekwidystantna "
  "w trójwymiarowym hiperbolicznym świecie, a kamera znajduje się nad powierzchnią, "
  "do której podłoga jest ekwidystantna -- co powoduje, że podłoga wygląda "
  "dokładnie jak w modelu Poincaré.\n\n"
  "Pokazuje to kolejny slajd, w modelu kuli, trójwymiarowym odpowiedniku modelu dysku.")

S("Hyperboloid model", "Model hiperboloidy")
S(
    "Let's see more models of the hyperbolic plane. "
    "This model uses a hyperboloid in the Minkowski geometry; "
    "it is used internally by HyperRogue.",
    
    "Pokażemy teraz więcej modeli. To jest hiperboloida w geometrii Minkowskiego; "
    "HyperRogue wewnętrznie używa tego modelu.")

S("Beltrami-Klein model", "Model Beltramiego-Kleina")
S(
    "This model renders straight lines as straight, but it distorts angles.",
    "Ten model pokazuje linie proste jako proste, ale przekłamuje kąty.")

S("Gans model", "Model Gansa")
S(
  "Yet another model, which corresponds to orthographic projection of the "
  "sphere. Poincaré disk model, Beltrami-Klein model, and the Gans "
  "model are all obtained by looking at either the hyperboloid model or an "
  "equidistant surface from various distances.",
  "Kolejny model, odpowiadający ortograficznemu rzutowi sfery. Model dysku, "
  "Kleina i Gansa powstają przez patrzenie na model hiperboloidy lub "
  "powierzchnię ekwidystantną z różnych odległości.")

S("Band model", "Model wstęgi")
S("render spiral", "rysuj spiralę")
S(
  "The band model is the hyperbolic analog of the Mercator projection of the sphere: "
  "a given straight line is rendered as a straight line, and the rest of the "
  "world is mapped conformally, that is, angles are not distorted. "
  "Here, we take the straight line connecting your starting point and your "
  "current position -- usually the path taken by the player is surprisingly "
  "close to a straight line. Press '8' to see this path.\n\n"
  "If you want, press '5' to see it rendered as a spiral, although it takes lots of time and "
  "memory.",
  "Model wstęgi to hiperboliczny analog rzutu Mercatora: "
  "pewna linia prosta jest rysowana jako linia prosta, a reszta świata jest "
  "odwzorowana konforemnie, tzn. kąty nie są przekłamywane. "
  "Tutaj jako centralną linię prostą bierzemy linię łączącą punkt początkowy i "
  "Twoją obecną pozycję -- zazwyczaj droga wybrana przez gracza jest "
  "niespodzianie zbliżona do linii prostej. Naciśnij '8', by zobaczyć ścieżkę.\n\n"
  "Możesz też nacisnąć '5', by zobaczyć to w postaci spirali, ale wymaga to "
  "bardzo dużo czasu i pamięci.")

S("Conformal square model", "Konforemny kwadrat")
S("The world can be mapped conformally to a square too.",
  "Świat można też konforemnie odwzorować na kwadrat.")

S("Shoot'em up mode", "Tryb strzelanki")
S("In the shoot'em up mode, space and time is continuous. "
  "You attack by throwing knives. "
  "Very fun with two players!\n\n"
  "There are other special modes too which change the gameplay or "
  "focus on a particular challenge.",
  "W trybie strzelanki czas i przestrzeń są ciągłe. Atakujesz, "
  "rzucając nożami. Bardzo fajne dla dwóch graczy!\n\n"
  "Jest wiele trybów specjalnych, zmieniających rozgrywkę lub "
  "koncentrujących się na konkretnym wyzwaniu.")

S("THE END", "KONIEC")
S("leave the tour mode", "opuść wycieczkę")
S(
  "This tour shows just a small part of what you can see in the world of HyperRogue. "
  "For example, "
  "hyperbolic mazes are much nicer than their Euclidean counterparts. "
  "Have fun exploring!\n\n"
  "Press '5' to leave the tour mode.",

  "Ten podręcznik pokazuje tylko małą część świata HyperRogue. Na przykład, "
  "hiperboliczne labirynty są ciekawsze niż euklidesowe. Miłej zabawy!"
  "Naciśnij '5', by opuścić podręcznik."
  )

/*
// for 10.0
*/

// Orb Strategy mode
// -----------------

S("Orb Strategy mode", "tryb strategii sfer")

S(
  "You are playing in the Orb Strategy Mode. Collecting treasure "
  "gives you access to magical Orb powers. In this mode, "
  "unlocking requirements are generally higher, and "
  "several quests and lands "
  "give you extremely powerful Orbs of the Mirror.\n",

  "Grasz w trybie strategii sfer. Zebrane skarby dają Ci "
  "dostęp do magicznych mocy. W tym trybie wymagania "
  "są generalnie wyższe, a  niektóre krainy i misje "
  "dają Ci bardzo potężne Sfery Lustra.\n")

S("The treasure gives your magical powers!", "Skarby dają Ci moce magiczne!")
S("Press 'i' to access your magical powers.", "Naciśnij 'i', by użyć mocy.")
S("inventory", "Twoje sfery")
S("mirror what?", "Co odbić?")
S("Which orb to use?", "Której Sfery użyć?")
S("Unlocked by: %1 in %2", "Odblokowane przez: %1 %abl2")
S(" (next at %1)", " (kolejny przy %1)")
S(" (next at %1 to %2)", " (kolejny przy %1 do %2)")
S("Number of uses left: %1", "Pozostało użyć: %1")
S("You mirror %the1.", "Odbijasz %a1.")
S("You need to stand next to a magic mirror or cloud to use %the1.", 
  "Musisz stać przy magicznym lustrze, by odbić %a1.")
S("Each orb type can be mirrored only once.", "Każdy typ sfery może być odbity tylko raz.")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Yendor appear in Hell after "
  "you collect 25 Demon Daisies in Hell, in Crossroads/Ocean after you collect 50, "
  "and everywhere after you collect 100.",

  "\n\nW trybie strategii sfer Sfery Yendoru pojawiają się w Piekle "
  "po zebraniu 25 Czarcich Ziel, na Skrzyżowaniu/Oceanie po zebraniu 50, "
  "wszędzie po zebraniu 100."
  )  

S(
  "\n\nIn the Orb Strategy Mode, dead orbs are available once you collect "
  "10 Necromancer Totems in the Graveyard.",
  
  "\n\nW trybie strategii sfer martwe sfery są dostępne po zdobyciu "
  "10 Totemów Nekromanty na Cmentarzu.")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Safety can be gained by "
  "collecting Phoenix Feathers in the Land of Eternal Motion. "
  "You can also find unlimited Orbs of Safety in the Crossroads "
  "and the Ocean (after collecting 25 Phoenix Feathers) "
  "and in the Prairie.",
  
  "\n\nW trybie strategii sfer Sfery Bezpieczeństwa mogą być zdobyte "
  "przez zbieranie Piór Feniksa w Krainie Wiecznego Ruchu. "
  "Można też znaleźć nieograniczone Sfery Bezpieczeństwa na "
  "Skrzyżowaniach (po zdobyciu 25 Piór Feniksa) i na Prerii."
  )

S(
  "\n\nCollect %the1 to gain an extra Orb of the Mirror. "
  "You can gain further Orbs of the Mirror by collecting 2, 4, 8...",

  "\n\nZdobądź %a1 by dostać dodatkową Sferę Lustra. "
  "Więcej Sfer Lustra dostaniesz przy 2, 4, 8..."
  )

S(
  "\n\nIn the Orb Strategy Mode, the Orb of Luck also "
  "significantly increases the frequency of Great Walls, Crossroads IV, "
  "and sub-lands.",
  
  "\n\nW trybie strategii sfer Sfera Szczęścia dodatkowo "
  "znacznie zwiększa częstotliwość wielkich ścian, Skrzyżowań IV "
  "i podkrain.")

S("\n\nIn the Orb Strategy Mode, each 25 Necromancer's Totems "
  "you are given a random offensive Orb.",
  
  "\n\nW trybie strategii sfer każde 25 Totemów Nekromanty "
  "daje dodatkową ofensywną sferę.")
 
S(
  "Use Orb of the Mirror to gain copies of one of your Orbs; "
  "mirroring weaker Orbs usually yields more copies. "
  "It can only be used once per Orb type, "
  "and only when you are next to a mirror.",
  
  "Użyj Sfery Lustra, by skopiować jedną z Twoich sfer; "
  "odbijanie słabszych Sfer zwykle daje więcej kopii. "
  "Możesz użyć tylko raz na każdy typ Sfery "
  "i tylko stojąc obok lustra.")
  
S("Uses to gain: %1", "Dostaniesz użyć: %1")
S("already mirrored", "już było odbijane")
  
N("your orbs", GEN_F, "Twoje Sfery", "Twoje Sfery", "Twoje Sfery", "Twoje Sfery")
S("Click this to see your orbs.", "Kliknij, by zobaczyć Twoje sfery.")                                                   

// peaceful mode
// -------------

S("configure keys/joysticks", "konfiguracja klawiszy/joysticka")
S("peaceful mode", "tryb spokojny")

// config changes
S("Press F5 or 'o' to try again!", "Naciśnij F5 lub 'o', by spróbować jeszcze raz!")  
S("aura brightness", "jasność aury")
S("aura smoothening factor", "wygładzanie aury")
S("graphics configuration", "konfiguracja grafiki")
S("special display modes", "specjalne tryby ekranu")
S("openGL mode", "tryb OpenGL")
S("anti-aliasing", "anti-aliasing")
S("line width", "szerokość linii")
S("configure panning and general keys", "skonfiguruj klawisze ogólne")

S("\n\nHint: use 'm' to toggle cells quickly",
  "\n\nWsk: użyj 'm' by szybko przestawiać pola")

// cell pattern names
S("football", "piłka nożna")
S("dark rainbow landscape", "ciemna tęcza")
S("field pattern", "wzór pola")
S("field pattern C", "wzór pola C")
S("field pattern D", "wzór pola D")
S("field pattern N", "wzór pola N")
S("field pattern S", "wzór pola S")
S("four triangles", "cztery trójkąty")
S("big triangles: rings", "duże trójkąty: pierścenie")

// missing for the Tutorial
S("This Orb is not compatible with the Tutorial.", "Ta Sfera nie jest kompatybilna z podręcznikiem.")

// local scores
S("turns", "kol")
S("cells", "pola")
S("sort", "sortuj")
S("choose", "wybór")
S("play", "graj")

// draw editor
S("autochoose", "autowybór")
S("c = choose", "c = wybór")
S("b = switch auto", "b = ustaw auto")

// mission screen hints

S(
  "If you collect too many treasures in a given land, it will become "
  "extremely dangerous. Try other lands once you have enough!",
  "Jeśli zbierzesz za dużo skarbów w jednej krainie, stanie się ona "
  "bardzo niebezpieczna. Spróbuj pójść do innych krain, gdy masz dość!")

S(
  "Remember that you can right click almost anything for more information.",
  "Pamiętaj, że prawie wszystko możesz kliknąć prawym przyciskiem, "
  "by dowiedzieć się czegoś na dany temat.")

S("Want to understand the geometry in HyperRogue? Try the Guided Tour!",
  "Chcesz zrozumieć geometrię w HyperRogue? Idź na wycieczkę!")

S(
  "Collecting 25 treasures in a given land may be dangerous, "
  "but allows magical Orbs of this land to appear in other places!",
  "Zebranie 25 skarbów w jednej krainie może być niebezpieczne, "
  "ale powoduje, że magiczne Sfery z tej krainy "
  "pojawiają się w pozostałych krainach!")

S(
  "Press ESC to view this screen during the game.",
  "Naciśnij ESC w czasie gry, by obejrzeć ten ekran.")

S("The 'world overview' shows all the lands in HyperRogue.",
  "'Przegląd krain' pokazuje wszystkie krainy w HyperRogue."
  )

S("Press 'o' to see all the lands in HyperRogue.",
  "Naciśnij 'o', by zobaczyć wszystkie krainy w HyperRogue.")

S(
  "Want another type of game? Want more challenge?\n"
  "HyperRogue has many special modes and challenges that "
  "significantly change the gameplay. Try them!",
  
  "Chcesz spróbować innego typu gry? Dodatkowych wyzwań?\n"
  "HyperRogue ma dużo specjalnych trybów istotnie "
  "zmieniających styl gry. Wypróbuj je!")

S(
  "Hyperbolic geometry can be shown in many ways.",
  "Geometria hiperboliczna może być pokazana na wiele sposobów...")

S(
  "You do not want to lose the game from a single mistake?\n"
  "Do you want to use the Orbs strategically?\n"
  "Try the Orb Strategy mode!",
  
  "Nie chcesz ginąć od jednego błędu? Chcesz używać Sfer strategicznie? "
  "Wypróbuj tryb strategii sfer!")

S(
  "Do you think you are playing on a ball? "
  "This is far from the truth!\n",
  
  "Myślisz, że grasz na sferze? Jest to dalekie od prawdy!\n")

S(
  "Did you know that the path you take during the game "
  "is usually very close to a straight line?\n",

  "Czy wiesz, że droga, którą przebywasz w czasie gry, "
  "jest zwykle bardzo zbliżona do linii prostej?")

S("Show me!", "Pokaż!")

S(
  "You are %1 cells away from the starting point, or "
  "the place where you used an Orb of Safety last time. "
  "There are %2 such cells.\n",
  
  "Jesteś %1 kroków od punktu startu lub miejsca "
  "ostatniego użycia Sfery Bezpieczeństwa. "
  "Takich pól jest %2.\n")

S("about ", "około ")
S(" (%1 more digits)", " (jeszcze cyfr: %1)")

S("see how it ended", "jak się skończyło")

// other missing/new things
S("Orb unlocked: %1", "Odblokowana Sfera: %1")
S("Secondary orb: %1", "Dodatkowa Sfera: %1")
S(" to submerge", " do zanurzenia")
S(" to surface", " do wynurzenia")
S("%The1 says, \"not this place, it looks even worse...\"", 
  "%The1 mówi, \"nie tu, tu jest jeszcze gorzej...\"")
S("torus", "torus")
S(" (click to use)", " (klik by użyć)")
N("Hall of Mirrors", GEN_F, "Lustrzana Sala", "Lustrzane Sale", "Lustzaną Salę", "w Lustrzanej Sali")
Orb("the Mirror", "Lustra")
N("Reflection", GEN_N, "Odbicie", "Odbicia", "Odbicie", "w Odbiciu")
N("mirror wall", GEN_F, "lustrzana ściana", "lustrzane ściany", "lustrzaną ścianę", "lustrzaną ścianą")

S("This would only move you deeper into the trap!",
  "To tylko przeniesie Cię w głąb pułapki!")

S("You swing your sword at the mirror.", "Wywijasz mieczem w kierunku lustra.")
N("Mimic", GEN_M, "Mimik", "Mimiki", "Mimika", "Mimikiem")
N("Narcissist", GEN_M, "Narcyz", "Narcyzy", "Narcyza", "Narcyzem")
N("Mirror Spirit", GEN_M, "Duch Lustra", "Duchy Lustra", "Ducha Lustra", "Duchem Lustra")

S("This person loves to look at their own reflection in the mirror. "
  "He believes himself to be one of the most important creatures in this world, "
  "and hates those who do not admire him.",

  "Ten osobnik kocha patrzeć na własne lustrzane odbicie. "
  "Uważa się za jedną z najważniejszych istot na świecie "
  "i nienawidzi tych, którzy go nie podziwiają."
  )

S(
    "A long time ago a mighty warrior was guarding the mirrors from being broken. "
    "While this warrior is no longer alive, his reflections have gained life of "
    "their own, and will punish the intruders.\n\n"
    "If you attack a Mirror Spirit physically, it is delayed, but not destroyed -- "
    "more reflections will come out of the mirror. Use Mimics to destroy them.",
    
    "Dawno, dawno temu potężny wojownik bronił luster przed tymi, co chcieli "
    "jej rozbić. Wojownik ten już dawno nie żyje, ale jego odbicia zaczęły "
    "żyć własnym życiem i karać najeźdźców.\n\n"
    "Jeśli zaatakujesz Ducha Lustra fizycznie, jest spowolniony, ale nie zniszczony -- "
    "więcej odbić wyjdzie z lustra. Użyj Mimików, by go zniszczyć.")

// peaceful texts

S("memory game", "gra pamięciowa")
S("display hints", "pokaż wskazówki")
S("hyperbolic puzzles", "hiperboliczne zagadki")

// missing descriptions

S(  "A strange land filled with mirrors. "
    "Break magic mirrors and enter clouds of mirage to "
    "gain treasures and helpful Mimics.",
    
    "Dziwna kraina wypełniona lustrami. "
    "Rozbijaj magiczne lustra i wchodź w chmury mirażowe, "
    "by zdobywać skarby i przyjazne Mimiki.")

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
  
  "Doskonała lustrzana ściana. Nie da się jej zniszczyć  "
  "i nawet istoty eteryczne nie są w stanie "
  "przejść na drugą stronę. Wszystko, co widzisz "
  "jest tylko obrazem świata rzeczywistego; "
  "możesz machnąć mieczem w kierunku tego obrazu, "
  "ale to nie wpłynie na rzeczywistość. "
  "Lustrzane ściany perfekcyjnie odbijają Mimiki, "
  "błyskawice i pociski."
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

"W trybie spokojnym zwiedzasz świat bez żadnej walki; "
  "jest też tu kilka hiperbolicznych zagadek nawigacyjnych. "
  "W grze pamięciowej musisz zebrać jak najwięcej Dwunastościanów "
  "i wrócić do punktu wyjścia -- geometria hiperboliczna powoduje, "
  "że jest to bardzo trudne! Pozostałe zagadki to Kurhany "
  "(wykopuj skarby swoim magicznym mieczem), Galapágos "
  "(znajdź dużego żółwia odpowiadającego żółwikowi), "
  "Camelot (znajdź środek wielkiego hiperbolicznego koła), "
  "i Pałac (idź za myszą). Można też zwiedzić kilka "
  "innych miejsc.")

S("puzzles and exploration", "zagadki i zwiedzanie")

S("Collect as many Dodecahedra as you can, then return here!",
  "Zbierz jak najwięcej Dwunastościanów i tu wróć!")

S("reset all configuration", "reset konfiguracji")
S("Are you sure?", "Czy na pewno?")
S("yes, and delete the config file", "tak, i skasuj plik konfiguracyjny")
S("yes", "tak")
S("cancel", "anuluj")
S("reset the special game modes", "zresetuj specjalne tryby gry")

// extra flavor messages for the OSM
// ---------------------------------

S("You feel the presence of free saves on the Crossroads.",
  "Wyczuwasz bezpieczeństwo na Skrzyżowaniach.")

S("You feel the Orbs of Yendor nearby...",
  "Czujesz w pobliżu Sfery Yendoru...")
  
S("You feel the Orbs of Yendor in the Crossroads...",
  "Wyczuwasz Sfery Yendoru na Skrzyżowaniach...")

S("You feel the Orbs of Yendor everywhere...",
  "Wyczuwasz wszędzie Sfery Yendoru ...")

S("You have gained an offensive power!",
  "Zdobywasz moc ofensywną!")

S("A small reward for braving the Hell.",
  "Drobna nagroda za zmierzenie się z Piekłem.")

S(" (mirrored)", " (odbite)")
S(" (used %1 times)", " (użyte %1 razy)")
S("Extras:", "Dodatkowe:") // extra Orbs gained in OSM

// cheats
// ------

S("unlock Orbs of Yendor", "otwórz Sfery Yendoru")
S("Collected the keys!", "Zebrano klucze!")
S("Saved the Princess!", "Uratowano Księżniczkę!")
S("save a Princess", "uratuj Księżniczkę")

// other
// -----
  
S("Note for mobiles", "Notka dla urządzeń mobilnych")
S(
    "This tour is designed for computers, "
    "and keys are given for all actions. It will "
    "work without a keyboard though, although less "
    "comfortably -- just ignore the keys "
    "given and select options from MENU.\n\n"
    "Select 'next slide' from MENU.",

    "Ta wycieczka jest przeznaczony głównie "
    "dla komputerów i klawisze są podane "
    "dla wszystkich akcji. Działa jednak bez "
    "klawiatury, choć mniej wygodnie -- "
    "ignoruj klawisze i wybieraj opcje z MENU.\n\n"
    "given and select options from MENU.\n\n"
    "Wybierz z MENU 'następny slajd'.")

S("quick mouse", "szybka mysz")
S("This combination is known to be buggy at the moment.", "Ta kombinacja opcji obecnie działa błędnie.")

// extra Princess texts
// --------------------
 
S("\"I do not like butterflies. They are treacherous.\"", 
  "\"Nie lubię motyli. Są zdradzieckie.\"")

S("\"I hate roses.\"", "\"Nienawidzę róż.\"")

S("\"In this world there is plenty of space for everyone. We do not need wars.\"", 
  "\"W tym świecie jest dość miejsca dla każdego. Nie potrzebujemy wojen.\"")

S("\"Only the stupid hyperbugs do not understand this.\"", 
  "\"Tylko głupie hiperinsekty tego nie rozumieją.\"")

S("\"I have once talked to a Yendorian researcher... he was only interested in infinite trees.\"",
  "\"Odwiedził mnie raz badacz yendoriański... interesowały go jedynie nieskończone drzewa.\"")

S("\"Infinite trees are boring. I prefer other graphs.\"", 
  "\"Nieskończone drzewa są nudne. Wolę inne grafy.\"")

// new start menu
// --------------

S("skip the start menu", "pomiń menu startowe")

S("HyperRogue classic", "tryb klasyczny HyperRogue")
S("explore the world, collect treasures", "zwiedzaj świat i zbieraj skarby")
S("do not get checkmated", "nie daj się zaszachować")
S("use your Orbs in tough situations", "używaj Sfer w trudnych sytuacjach")
S("continuous spacetime", "ciągła czasoprzestrzeń")
S("(most achievements are not available)", "(większość osiągnięć niedostępna)")
S("learn about hyperbolic geometry!", "dowiedz się więcej o geometrii hiperbolicznej!")
S("more options", "więcej opcji")

S(
  "A strange land filled with mirrors. Break magic mirrors and mirage clouds to "
  "gain treasures and helpful Mimics.",
  "Dziwna kraina pełna luster. Zbijaj magiczne lustra i mirażowe chmury, "
  "by zdobywać skarby i przyjazne Mimiki."
  )
  
// were missing

N("Kraken", GEN_M, "Kraken", "Krakeny", "Krakena", "Krakenem")
N("Kraken Tentacle", GEN_F, "Macka Krakena", "Macki Krakena", "Mackę Krakena", "Macką Krakena")
S(" (killing increases treasure spawn)", " (pokonywanie zwiększa częstość skarbów)")

N("stepping stones", GEN_O, "Bród", "Brody", "Bród", "Brodem")

// 10.1

// mew message options
S("centered", "po środku")
S("left-aligned", "na lewo")
S("line-broken", "łamane linie")
S("message style", "styl wiadomości")
S("limit messages shown", "ograniczenie na wiadomości")
S("Maximum number of messages on screen.", "Największa możliwa liczba wiadomości na ekranie.")

S("message log", "log wiadomości")
S("real time", "czas rzeczywisty")
S("game time", "czas gry")
S("precise", "dokładny czas")
S("no time", "bez czasu")
S("back", "wróć")
S("clear", "czyść")

// 'u' now loads the current picture in the Map Editor
S("'u' to load current", "'u' by załadować obecne")

// Happy Hunting Ground
//----------------------

N("Hunting Ground", GEN_F, "Kraina Wiecznych Łowów", "Kraina Wiecznych Łowów", "Krainę Wiecznych Łowów", "w Krainie Wiecznych Łowów")

S("The Happy Hunting Ground is the place where hunting is easy; "
  "the spirits of hunters go here after their death, if their life was deemed worthy. "
  "You did not qualify though, so you will not ever be able to find your way to the places "
  "where the hunter spirits and game are... "
  "and their hunting dogs will hunt you instead!\n\n"
  "You hope to use this to learn some "
  "skills in fighting in open hyperbolic space, though.",
  
  "W Krainie Wiecznych Łowów polowanie jest łatwe; dusze myśliwych idą tu po ich śmierci, "
  "jeżeli ich życie zostało uznane za godne. Ty się nie kwalifikujesz, także nigdy "
  "nie trafisz do miejsc, gdzie przebywają dusze myśliwych i zwierzyna... "
  "a ich psy myśliwskie będą polować na Ciebie!\n\n"
  "Masz jednak nadzieję, że uda Ci się wykorzystać to miejsce do wytrenowania "
  "umiejętności walki w otwartej hiperbolicznej przestrzeni."
  )
  
N("Hunting Dog", GEN_M, "Pies Myśliwski", "Psy Myśliwskie", "Psa Myśliwskiego", "Psem Myśliwskim")
N("Hunting Dog (guarding)", GEN_M, "Pies Myśliwski (waruje)", "Psy Myśliwskie (warują)", "Psa Myśliwskiego (warującego)", "Psem Myśliwskim (warującym)")
N("Turquoise", GEN_O, "Turkus", "Turkusy", "Turkus", "Turkusem")
Orb("Ferocity", "Zaciekłości")

S("Hunters believe that wearing a Turquoise amulet will improve their accuracy. "
  "This one has been lost, but the hunting dogs are guarding it until the owner returns.",
  
  "Myśliwi wierzą, że turkusowy amulet poprawi ich celność. "
  "Ten amulet został zgubiony, ale psy myśliwskie go pilnują do czasu powrotu właściciela."
  )

S("Whenever you attack with this Orb, you also hit the monsters adjacent to you and opposite to the monster you originally attacked.",
  "Gdy atakujesz z tą Sferą, atakujesz także potwory obok Ciebie po przeciwnej stronie od oryginalnie zaatakowanego potwora."
  )

// Volcanic Wasteland
//--------------------

N("Volcanic Wasteland", GEN_N, "Wulkaniczne Pustkowie", "Wulkaniczne Pustkowia", "Wulkaniczne Pustkoiwe", "w Wulkanicznym Pustkowiu")
N("lava", GEN_F, "lawa", "lawy", "lawę", "lawą")

S("The volcanic activity in this land is extremely unstable. "
  "Lava is too hot to walk on. It cools quickly, but another "
  "flow will come soon...",
  
  "W tej krainie aktywność wulkaniczna jest bardzo niestabilna. "
  "Lawa jest zbyt gorąca, by po niej chodzić. Stygnie szybko, "
  "ale wkrótce przyjdzie kolejny przypływ..."
  )

N("Salamander", GEN_F, "Salamandra", "Salamandry", "Salamandrę", "Salamandrą")

S("Salamanders are tough lizard-like creatures. Their tough skin protects them "
  "from both physical attacks and heat. Salamanders "
  "are stunned for a longer time if you push them into lava, fire, or a solid obstacle.",
  
  "Salamandry to potężne stworzenia podobne do jaszczurek. Ich skóra chroni je przed "
  "atakami fizycznymi i gorącem. Salamandry stają się nieaktywne przez dłuższy czas, "
  "jeśli wepchniesz je do lawy, ognia, lub w solidną przeszkodę."
  )

N("Lava Wolf", GEN_M, "Wilk Lawowy", "Wilki Lawowe", "Wilka Lawowego", "Wilkiem Lawowym")

S("While Ice Wolves love heat, their instincts usually will not let them leave the Icy Lands -- "
  "they are afraid that they will be unable to get back home, and that they will lose track of their prey. "
  "However, they find the Volcanic Wasteland so hot and attractive that they abandon their natural instincts... "
  "and try to track their prey using their other senses and intelligence.",
  
  "Lodowe Wilki uwielbiają ciepło. Ich instynkt jednak zabrania im wyjść poza Lodową Krainę -- "
  "obawiają się, że już nie wrócą do domu, i że nie będą w stanie dalej wyczuwać swoje ofiary. "
  "Jednak Wulkaniczne Pustkowie jest dla nich tak gorące i atrakcyjne, że postanawiają "
  "porzucić swoje naturalne instynkty... i polować przy użyciu swoich pozostałych zmysłów i inteligencji.")
  
N("Lava Lily", GEN_F, "Lilia Lawowa", "Lilie Lawowe", "Lilię Lawową", "Lilią Lawową")

S("This plant, able to survive in the extreme conditions of the Volcanic Wasteland, "
  "is a valuable alchemical ingredient.",
  
  "Ta roślina, zdolna to przeżycia w ekstremalnych warunkach Wulkanicznego Pustkowia, "
  "jest wartościowym składnikiem alchemicznym.")

Orb("Lava", "Lawy")
S("This Orb summons a minor volcanic activity around you. "
  "All the heptagonal cells adjacent to enemies in distance at most 5 to you "
  "will be set on fire. Does not affect the cell you are on, and enemies resistant to fire.",
  
  "Ta Sfera powoduje aktywność wulkaniczną w Twoich okolicach. "
  "Wszystkie siedmiokątne pola obok przeciwników w odległości co najwyżej 5 od Ciebie "
  "będą się palić. Nie wpływa to na pole, na którym stoisz, ani na przeciwników odpornych na ogień."
  )

// Orbs would be destroyed in Volcanic Wasteland
S("would be destroyed in %the1", "by%ł1by zniszczon%y1 %abl1") 
S(" to go cold", " by ostygnąć")
S("%The1 is destroyed by lava!", "%The1 jest zniszczon%y1 przez lawę!")
S("%The1 is killed by lava!", "%The1 jest zabit%y1 przez lawę!")
S("Run away from the lava!", "Uciekaj od lawy!")

// Terracotta Army
//-----------------

N("Terracotta Army", GEN_F, "Armia Terakotowa", "Armie Terakotowe", "Armię Terakotową", "w Armii Terakotowej")

S("The Emperor's mausoleum. The army "
  "of Terracotta Warriors is supposed to protect him in his "
  "afterlife, while the traps are to protect the treasures "
  "in the mausoleum from thieves.",

  "Mauzuleum Cesarza. "
  "Terakotowi wojownicy mają go chronić w życiu pozagrobowym, "
  "a pułapki chronić znajdujące się w mauzoleum skarby "
  "przed złodziejami."
  )

S("Spending 3-5 turns next to the Terracotta Warriors will cause them to awaken. "
  "They have 7 HP, and get stunned for longer and longer time with each attack.",
  
  "Terakotowi Wojownicy budzą się, gdy spędzisz 3 do 5 kolejek obok nich. "
  "Mają 7 HP, i z każdym atakiem są ogłuszani na coraz dłuższy czas."
  )

N("arrow trap", GEN_F, "strzelająca pułapka", "strzelające pułapki", "strzelającą pułapkę", "strzelającą pułapką")

S("The arrow traps will automatically launch a deadly arrow when stepped on. They are quite old though, so the arrow will "
  "actually fly at the end of the next turn.",
  
  "Te pułapki strzelają zabójczymi strzałami, gdy ktoś na nich stanie. Konstrukcja jest dosyć stara, także strzały "
  "wylatują po zakończeniu następnej kolejki."
  )

N("mercury river", GEN_F, "rzeka rtęci", "rzeki rtęci", "rzekę rtęci", "rzeką rtęci")
S("A river of mercury.", "Rzeka rtęci.")

N("Terracotta Warrior", GEN_M, "Terakotowy Wojownik", "Terakotowi Wojownicy", "Terakotowego Wojownika", "Terakotowym Wojownikiem")

N("Jiangshi", GEN_M, "Jiangshi", "Jiangshi", "Jiangshi", "Jiangshi")
S("You think this was one of the people who have constructed the Terracotta Warriors and the arrow traps. "
  "They have been locked inside, so that they will never divulge the secrets of the mausoleum. They would like to return their homes and families, though.",
  
  "Podejrzewasz, że te istoty kiedyś były rzemieślnikami, którzy skonstruowali Terakotowych Wojowników i "
  "pułapki. Zostali tutaj zamknięci, by nigdy nie wydali sekretów mauzoleum. Chcieliby jednak wrócić do swoich domów i rodzin." 
  )

Orb("Slashing", "Cięcia")
S("Whenever you attack with this Orb, you also hit the monsters adjacent both to you and the monster you originally attacked.",
  "Gdy atakujesz z tą Sferą, atakujesz także potwory obok Ciebie i obok oryginalnie zaatakowanego potwora.")

N("Ancient Weapon", GEN_F, "Starożytna Broń", "Starożytne Bronie", "Starożytną Broń", "Starożytną Bronią")
S("This ancient weapon is beautifully encrusted with precious gems, but you prefer your own -- it is much lighter.",
  "Ta starożytna broń jest pięknie zdobiona drogocennymi kamieniami, ale wolisz swoją własną -- jest dużo lżejsza.")

// Blizzard
//----------

N("Blizzard", GEN_F, "Zamieć", "Zamieci", "Zamieci", "w Zamieci")
N("Ice Golem", GEN_M, "Lodowy Golem", "Lodowe Golemy", "Lodowego Golema", "Lodowym Golemem")
N("Void Beast", GEN_F, "Bestia Nicości", "Bestie Nicości", "Bestię Nicości", "Bestią Nicości")
N("Forgotten Relic", GEN_F, "Zgubiona Relikwia", "Zgubione Relikwie", "Zgubioną Relikwię", "Zgubioną Relikwią")

S("The Ice Golems are powered by intense magical coldness. When destroyed in the Blizzard or another icy land, they become "
  "ice walls, and freeze the land around them.",
  
  "Lodowe Golemy napędzane są intensywnym, magicznym zimnem. Gdy golem jest zniszczony w Zamieci albo innej "
  "lodowej krainie, zamienia się w lodową ścianę, i zamraża ziemię w okolicy.")

S("Are creatures of Void actual monsters, or just monster-shaped holes in the air?\n\nVoid Beasts move simply by letting the air move into their current location -- "
  "the hole will move to the place where the air came from! Void Beasts are great at moving against the wind, but they have problems "
  "moving with the wind.",
  
  "Czy stworzenia Nicości to rzeczywiście potwory, czy tylko dziury w powietrzu o kształcie potworów?\n\n"
  "Bestie Nicości poruszają się poprzez pozwolenie powietrzu na wlecenie w zajmowane przez nie obecnie miejsce -- "
  "dziura przesunie się w miejsce, skąd powietrze przyszło! Bestie Nicości świetnie poruszają się pod wiatr, ale "
  "nie są w stanie ruszać się z wiatrem."
  )

S("Once in the past there lived a tribe whose prophets warned about the great blizzard that was to come and freeze the world. "
  "They thought it was the wrath of the gods, so they put some altars, so that the gods could hear their pleas. "
  "The mighty golems guarded those altars, making sure nobody steals the jewels, because a robbed god is a angry god. "
  "Unfortunately winter has come (maybe because only the monks knew how to stop the golems, and they were said to had escaped earlier with some of the jewels). "
  "The tribe has frozen to death, altars got covered with snow and ice. Only ice golems still guard them waiting for the spring.",
  
  "Dawno dawno temu żyło tu plemię, którego prorocy ostrzegali przed wielką zamiecią, która miała zamrozić świat. "
  "Według nich był to gniew bogów, także postawili ołtarze, chronione przez golemy, by nikt nie ukradł należących do bogów "
  "klejnotów -- okradziony bóg, to rozzłoszczony bóg. Niestety, zima w końcu przyszła (być może dlatego że tylko mnisi wiedzieli, jak sobie radzić z golemami, a ci uciekli z "
  "klejnotami). Plemię zamarzło, ołtarze pokryły się lodem. Golemy zostały, czekając na odwilż."
  )

// Winter becomes native in Blizzard, while Change replaces Winter in Cocytus
Orb("Change", "Zmiany")
S("This ranged Orb will transform the target monster into one without any special powers. It also stuns them for one turn. "
  "Does not affect multi-tile monsters.",
  "Ta sfera zamienia wybranego potwora w takiego, który nie ma żadnych specjalnych mocy, a także ogłusza go na jedną kolejkę. "
  "Nie działa na potwory zajmujące wiele pól."
  )

// yet unused orb
Orb("the Triangle", "Trójkąta")
S("Whenever you attack with this Orb, you also hit the monsters adjacent to you which are neither adjacent nor opposite to the monster "
  "you originally attacked.",
  "Gdy atakujesz z tą Sferą, atakujesz także potwory obok Ciebie, które ani nie są obok, ani na przeciwko oryginalnie zaatakowanego potwora.")
                                                                    
S("Using %the1 in %the2 sounds dangerous...", "Używanie %1 %abl2 brzmi niebezpiecznie...")
S("%The1 is mostly useless in %the2...", "%1 jest raczej bezużyteczna %abl2...")
S("%The1 is forbidden in %the2 (disables some achievements)", "%1 zabroniona %abl2 (użycie wyłącza niektóre osiągnięcia")

S("You are ambushed!", "Zasadzka!")
S("teleport", "teleport")
S("ambush:", "zasadzka:")

N("Hunting Dog (regrouping)", GEN_M, "Pies Myśliwski (ucieka)", "Psy Myśliwskie (uciekają)", "Psa Myśliwskiego (uciekającego)", "Psem Myśliwskim (uciekającym)")
S("When your plan has clearly failed, it is better to abandon it and go to a safe place, to have a chance of succeeding next time. This dog clearly knows this.",
  "Kiedy jest jasne, że Twój plan się nie powiódł, lepiej jest go porzucić i odejść w bezpieczne miejsce, by mieć szansę na sukces przy kolejnej próbie. "
  "Ten pies zdecydowanie to wie.")

S("The Hunting Dogs give up.", "Myśliwskie psy się poddały.")

// missing from previous versions:

S("\n\nThis orb also allows you to collect items encased in ice.",
  "\n\nTa sfera pozwala Ci również zbierać rzeczy zamrożone w lodowej ścianie.")

// vampire attack
S("%The1 drains your powers!", "%The1 wysysa Twoje moce!")

// additions
S(" This tour will not advance on its own -- you have to press Enter (not while reading help text).", 
  " Ta wycieczka nie idzie do przodu sama z siebie -- musisz nacisnąć Enter (nie podczas czytania tekstu pomocy)."
  )

S("Hint: hold Alt to highlights enemies and other important features.",
  "Wskazówka: trzymaj Alt by podświetlać przeciwników i inne istotne elementy.")

// Crystal World
N("Crystal World", GEN_O, "Kryształowy Świat", "Kryształowe Światy", "Kryształowy Świat", "w Kryształowym Świecie")
N("Glowing Crystal", GEN_O, "Świecący Kryształ", "Świecące Kryształy", "Świecący Kryształ", "Świecącym Kryształem")

S("Yes, this is definitely a crystal. A very regular crystalline structure.\n\n"
  "This land has been designed as a tool to play with various geometries, and it does not appear during a normal gameplay.",
  
  "Tak, to na pewno kryształ. Bardzo regularna struktura krystaliczna.\n\n"
  "Ta kraina została zaprojektowana do zabawy różnymi geometriami, i nie pojawia się podczas normalnej gry.")

S("You cannot move there!", "Nie możesz tam iść!")

// geometry stuff

S("experiment with geometry", "eksperymenty z geometrią")

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
  
  "Chcesz wiedzieć, jak geometria wpływa na grę HyperRogue? Ten tryb jest dla Ciebie!\n\n"
  "Możesz tu wypróbować wiele różnych geometrii. Zaczynamy od sklejania n-kątów tak, "
  "by k z nich spotykało się w 1 wierzchołku. Zależnie od n i k, to się zwija w kulkę, "
  "rozwija w płaszczyznę, albo wymaga przestrzeni hiperbolicznej. Wynik może być "
  "następnie 'przycięty' poprzez zastąpienie każdego wierzchołka 2k-kątem. Można "
  "też eksperymentować z przestrzeniami ilorazowymi. Na przykład, geometria eliptyczna "
  "jest otrzymana w ten sposób, że antypody traktujemy jako ten sam punkt, także "
  "po przejściu na antypody wracasz w to samo miejsce (ale odbit%y0 lustrzanie). "
  "Miłej zabawy! Osiągnięcia nie działają w eksperymentach geometrycznych, oprócz "
  "niektórych specjalnych.\n\n"
  "W standardowej geometrii (przyciętej lub nie) grasz całą grę, w innych geometriach "
  "wybierasz krainę. Krainy są odblokowane poprzez odwiedzenie ich w tej sesji, lub "
  "permanentnie przez zebranie 25 skarbów. By odwiedzić wiele krain, wypróbuj "
  "Skrzyżowanie w geometrii euklidesowej, lub Chaos w niestandardowej nieilorazowej "
  "geometrii.\n"
  "Godne uwagi:\n"
  "* Kryształowy Świat i Zakrzywiony Brzeg to w pewnym sensie dodatkowe geometrie.\n"
  "* Halloween to kraina zaprojektowana dla geometrii sferycznej.\n"
  "* Wypróbuj Krainę Wiecznych Łowów w geometrii euklidesowej -- staje się "
  "niemożliwa do przejścia."  
  )

// new geometries available:
S("standard", "standard") // renamed from 'hyperbolic'

S("octagons", "ośmiokąty")
S("four pentagons", "cztery pięciokąty")
S("four hexagons", "cztery sześciokąty")
S("four heptagons", "cztery siedmiokąty")
S("cube", "sześcian")
S("tetrahedron (buggy)", "czworościan (bugi)")
S("tetrahedron", "czworościan") // in case if it gets better

S("land", "kraina")
S("bitruncated", "przycięte")
S("does not matter", "bez znaczenia")
S("sides per face", "boki ścian")
S("faces per vertex", "wierzchołki")
S("none", "brak")
S("zebra", "zebra")
S("field", "ciało")
S("quotient space", "przestrzeń ilorazowa")
S("size of the world", "wielkość świata")
S("flat", "płaska")

// bitruncated or not bitrunc
S(" (b)", " (b)")
S(" (n)", " (n)")

S("(locked)", "(blok)")

S("stereographic/orthogonal", "rzut stereo/orto")
S("Poincaré/Klein", "Poincaré/Klein")
  
// Paper Model Creator
S("Useless in Euclidean geometry.", "Bezużyteczne w geometrii euklidesowej.")
S("Not implemented for spherical geometry. Please tell me if you really want this.",
  "Nie zaimplementowane dla geometrii sferycznej. Jeśli bardzo tego chcesz, powiedz.")

// === 10.3 ===
// ============

// Missing/minor updated texts
//-----------------------------

// file selection dialog
S("Enter = choose", "Enter = wybierz")
S("Esc = cancel", "Esc = anuluj")

S("hyperbolic", "hiperboliczna")
S("reset special modes", "resetuj tryby specjalne")
S("orthogonal projection", "rzut prostokątny")
S("small Poincaré model", "mały model Poincaré")
S("big Poincaré model", "duży model Poincaré")
S("Klein-Beltrami model", "model Kleina-Beltramiego")
S("bestiary of %the1", "bestiariusz: %1")

// for the map editor
N("Dragon Head", GEN_F, "Głowa Smoka", "Głowy Smoka", "Głowę Smoka", "Głową Smoka")

S("score: %1", "wynik: %1")
S("kills: %1", "zabicia: %1")

// advanced config of quotient geometry
//--------------------------------------

S(
  "This geometry is obtained by applying the same 'generators' which "
  "lead to creating the given basic hyperbolic geometry, "
  "but using a fixed finite field instead of the field of reals. "
  "It can be also interpreted as a quotient of the given basic geometry. "
  "Warning: field patterns based on large primes might generate for a long time.",
  
  "Ta geometria powstaje przez użycie tych samych 'generatorów', "
  "których używamy do uzyskania zadanej podstawowej geometrii hiperbolicznej, "
  "ale w pewnym ciele skończonym zamiast ciała liczb rzczywistych. "
  "Można ją zrozumieć jako przestrzeń ilorazową zadanej geometrii. "
  "Uwaga: generowanie tego typu geometrii opartej na dużej liczbie pierwszej może "
  "zająć dużo czasu!")

S("find the next prime", "znajdź następną liczbę pierwszą")
S("activate", "uruchom")
S("order %1%2 (non-bitruncated cells: %3)", "rząd %1%2 (nieprzyciętych pól: %3)")

// Torus/Klein bottle, Euclidean Squares, and their advanced configuration
//-------------------------------------------------------------------------

S("square grid", "siatka kwadratowa")

S("torus/Klein bottle", "torus/butelka Kleina")
S("mode", "tryb")

S("single row (hex)", "jeden rząd (heksy)")
S("single row (squares)", "jeden rząd (kwadraty)")
S("parallelogram (hex)", "równoległobok (heksy)")
S("rectangle (squares)", "prostokąt (kwadraty)")
S("rectangle (hex)", "prostokąt (heksy)")
S("Klein bottle (squares)", "butelka Kleina (kwadraty)")
S("Klein bottle (hex)", "butelka Kleina (heksy)")

S("this mode has bad patterns", "nieprawidłowe wzorki dla tego trybu")
S("incompatible with bitruncating", "niekompatybilne z przycinaniem")
S("best if %1 is divisible by %2", "najlepiej gdy %1 jest podzielne przez %2")

S("number of cells (n)", "liczba pól (n)")
S("cell bottom-right from 0 (d)", "pole w dół i na prawo od 0 (d)")
S("cell below 0 (d)", "pole pod 0 (d)")
S("width (x)", "szerokość (x)")
S("height (y)", "wysokość (y)")

// upgraded Hypersian Rug mode
//-----------------------------

S("In the perspective projection, you can use arrows to rotate the camera, Page Up/Down to go forward/backward, Shift+arrows to strafe, and Ctrl+arrows to rotate the model.",
  "W trybie perspektywy możesz używać strzałek by obracać kamerą, Page Up/Down by iść do przodu/w tył, Shift+strzałek by chodzić na boki, Ctrl+strzałek by obracać model.")

S("Note: this mode is based on what you see on the screen -- but re-rendered in another way. "
  "If not everything is shown on the screen (e.g., too zoomed in), the results will be incorrect "
  "(though possibly interesting). "
  "Use a different projection to fix this.",
  
  "Uwaga: ten tryb jest oparty na tym, co widzisz na ekranie -- tylko inaczej przedstawionym. "
  "Jeśli nie wszystko jest na ekranie (np. za mocno zbliżenie), wyniki będą nieprawidłowe, "
  "choć być może interesujące. Użyj innego rzutu, by to naprawić.")

S("vertex limit", "ograniczenie na liczbę wierzchołków")
S("The more vertices, the more accurate the Hypersian Rug model is. "
  "However, a number too high might make the model slow to compute and render.",
  "Im więcej wierzchołków, tym dokładniejszy model, "
  "ale za duża liczba może spowodować, że model się będzie wolno liczył i rysował.")

S("model distance", "odległość od modelu")
S("In the perspective projection, this sets the distance from the camera to the center of the model. "
  "In the orthogonal projection this just controls the scale.",
  
  "W perspektywie ten parametr ustawia odległość od kamery do centrum modelu. "
  "W rzucie ortogonalnym zmienia skalę.")

S("maximum error", "maksymalny błąd")
S("New points are added when the current error in the model is smaller than this value.",
  "Nowe punkty są dodawane, gdy obecny błąd jest mniejszy niż ta wartość.")

S("field of view", "pole widzenia")
S("Horizontal field of view, in the perspective projection. "
  "In the orthogonal projection this just controls the scale.",
  "Poziome pole widzenia, w rzucie perspektywicznym. "
  "W rzucie ortogonalnym zmienia skalę.")

S("native geometry", "rodzima geometria")
S("scale model", "skala modelu")
S("This is relevant when the native geometry is not Euclidean. "
  "For example, if the native geometry is spherical, and scale < 1, a 2d sphere will be rendered as a subsphere; "
  "if the native geometry is hyperbolic, and scale > 1, a hyperbolic plane will be rendered as an equidistant surface. ",
  
  "To ma znaczenie gdy rodzima geometria nie jest euklidesowa. "
  "Przykładowo, dla rodzimej geometrii sferycznej i skali < 1 2-sfera będzie modelowana jako podsfera; "
  "w rodzimej geometrii hiperbolicznej dla skali > 1 płaszczyzna hiperboliczna będzie modelowana jako powietrzchnia ekwidystantna.")

S("This just shows the 'z' coordinate of the selected point.",
  "To jedynie pokazuje współrzędną 'z' wybranego punktu.")

S("radar", "radar")
S("model iterations", "iteracje modelu")
S("keep shape", "trzymaj kształt")

// texture mode
//--------------

S("texture mode", "tryb tekstury")

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
  
  "Ten tryb pozwala Ci łatwo zmienić teselację powierzchni -- "
  "wybierz 'namaluj nową teksturę' i rysuj jak w programie typu Paint. "
  "Otrzymany wzorek może być następnie przekształcony do innej geometrii "
  "lub zapisany.\n\n"
  "Zamiast rysowania jest też możliwe użycie dowolnego obrazka. "
  "Działa najlepiej z parkietażami (np. zdjęcie piłki nożnej, albo "
  "parkietaże M. C. Eschera), ale można też tego użyć na dowolnych zdjęciach, "
  "by uzyskać ich okresową wersję (zazwyczaj najlepiej działają 'duże obrazki' "
  "w wyborze geometrii). Tu również możemy przekształcić parkietaż do "
  "innej geometrii.\n\n")

S("texture mode (off)", "tryb tekstury (wyłączony)")
S("select geometry/pattern", "wybierz geometrię/wzór")
S("reactivate the texture", "reaktywuj teksturę")
S("open PNG as texture", "otwórz PNG jako teksturę")
S("load texture config", "ładuj konfigurację tekstury")
S("warning: unable to find the center", "ostrzeżenie: nie udało się znaleźć centrum")
S("texture size", "rozmiar tekstury")
S("paint a new texture", "namaluj nową teksturę")
S("precision", "precyzja")

S("texture mode (overlay)", "tryb tekstury (nałożenie)")
S("select the texture's pattern", "wybierz wzorek tekstury")
S("enable the texture", "uruchom teksturę")
S("cancel the texture", "anuluj teksturę")
S("move the model", "przesuń model")
S("move the texture", "przesuń teksturę")
S("zoom/scale the texture", "obróć/zmień rozmiar tekstury")
S("zoom/scale the model", "obróć/zmień rozmiar modelu")
S("projection", "rzut")
S("affine transformations", "przekszt. afiniczne")
S("magic", "magia")

S("grid color (master)", "kolor siatki (główny)")
S("grid color (copy)", "kolor siatki (kopia)")
S("save the raw texture", "zapisz surową teksturę")
S("Saved the raw texture to %1", "Zapisano surową teksturę do %1")

S("texture auto-adjustment", "auto-ustawianie tekstury")
S("drag from the model to the texture", "przeciągaj z modelu na teksturę")
S("affect model scale", "skala modelu")
S("affect model projection", "projekcja")
S("affect model rotation", "obrót modelu")
S("affect model position", "pozycja modelu")
S("affect texture slanting", "przechylenie tekstury")
S("affect texture stretching", "rozciągnięcie tekstury")
S("delete markers", "usuń markery")
S("perform auto-adjustment", "wykonaj auto-ustawianie")

S("texture mode (active)", "tryb tekstury (aktywny)")
S("deactivate the texture", "wyłącz teksturę")
S("back to overlay mode", "wróć do trybu nałożenia")
S("change the geometry", "zmień geometrię")
S("grid color", "kolor siatki")
S("mesh color", "kolor siateczki")
S("color alpha", "współczynnik koloru")
S("The higher the value, the less important the color of underlying terrain is.",
  "Im wyższa wartość, tym mniej istotny kolor podłoża.")
S("edit the texture", "edytuj teksturę")
S("save the full texture image", "zapisz pełną teksturę")
S("Saving full texture to %1...", "Zapis pełnej tekstury do %1...")
S("save texture config", "zapisz konfigurację tekstury")

// pattern groups

S("football", "piłka nożna")
S("three colors", "trzy kolory")
S("chessboard", "szachownica")
S("single type", "pojedynczy typ")
S("large picture", "duży obrazek")
S("periodic patterns", "okresowe wzorki")
S("more tuning", "dostrajanie")
S("BAD", "ŹLE")

// extra patterns

S("Docks pattern", "wzorek Doków")
S("broken Emerald pattern", "złamany szmaragdowy wzór")
S("coloring", "kolorowanie")


// Docks
//-------

N("Docks", GEN_O, "Dok", "Doki", "Dok", "w Doku")
N("dock", GEN_O, "dok", "doki", "dok", "dokiem")
S("A dock.", "Dok.")
N("burning dock", GEN_O, "płonący dok", "płonące doki", "płonący dok", "płonącym dokiem")
S("A burning dock.", "Płonący dok.")

N("Sea Glass", GEN_N, "Morskie Szkło", "Morskie Szkła", "Morskie Szkło", "Morskim Szkłem")

// Snake Nest

N("Snake Nest", GEN_N, "Gniazdo Węży", "Gniazda Węży", "Gniazdo Węży", "w Gnieździe Węży")

N("Snake Oil", GEN_O, "Wężowy Olej", "Wężowe Oleje", "Wężowy Olej", "Wężowym Olejem")

S("Made of actual snakes!", "Zrobiony z prawdziwych węży!")

// Ruined City

N("Ruined City", GEN_N, "Zrujnowane Miasto", "Zrujnowane Miasta", "Zrujnowane Miasto", "w Zrujnowanym Mieście")

S("Once a beautiful city... but now overrun by the mighty Raiders of unknown origin.\n\n"
 "Raiders cannot be harmed with mundane weapons, but each color has its movement restrictions.",
 
 "Niegdyś piękne miasto... ale obecnie zrujnowane przez potężnych Najeźdźców nieznanego pochodzenia.\n\n"
 "Najeźdźców nie można zranić zwykłą bronią, ale każdy kolor ma pewne ograniczenie ruchu.")

N("ruin wall", GEN_F, "zrujnowana ściana", "zrujnowane ściany", "zrujnowaną ścianę", "zrujnowaną ścianą")

N("Chrysoberyl", GEN_O, "Chryzoberyl", "Chryzoberyle", "Chryzoberyl", "Chryzoberylem")
S("Fragment of the past glory.", "Fragment dawnej chwały.")

N("Red Raider", GEN_M, "Czerwony Najeźdźca", "Czerwoni Najeźdźcy", "Czerwonego Najeźdźcę", "Czerwonym Najeźdźcą")
S("Red Raiders travel in pairs. They have promised to always watch each other's backs. They are able to destroy walls on their way.",
  "Czerwoni Najeźdźcy podróżują parami. Obiecali sobie nawzajem, że zawsze będą chronić swoje tyły. Mogą niszczyć ściany na swojej drodze.")

N("Gray Raider", GEN_M, "Szary Najeźdźca", "Szarzy Najeźdźcy", "Szarego Najeźdźcę", "Szarym Najeźdźcą")
S("Gray Raiders never step on gray cells.",
  "Szarzy Najeźdźcy nie stają na szarych polach.")

N("Green Raider", GEN_M, "Zielony Najeźdźca", "Zieloni Najeźdźcy", "Zielonego Najeźdźcę", "Zielonym Najeźdźcą")
S("Green Raiders never step from one green cell to another.",
  "Zieloni Najeźdźcy nie przechodzą z jednego zielonego pola na drugie.")

N("Brown Raider", GEN_M, "Brązowy Najeźdźca", "Brązowi Najeźdźcy", "Brązowego Najeźdźcę", "Brązowym Najeźdźcą")
S("Brown Raiders never move adjacent to an item.",
  "Brązowi Najeźdźcy nie przechodzą koło przedmiotów.")


N("Blue Raider", GEN_M, "Niebieski Najeźdźca", "Niebiescy Najeźdźcy", "Niebieskiego Najeźdźcę", "Niebieskim Najeźdźcą")
S("Blue Raiders have a powerful attack which takes two turns to complete, and also makes the Blue Raider stunned "
  "for a long time. This attack can destroy other Raiders if it hits them.",
  "Niebiescy Najeźdźcy mają potężny atak, który trwa dwie kolejki, i powoduje, że Niebieski Najeźdźca jest "
  "ogłoszony przez dłuższy czas. Ten atak jest w stanie zniszczyć innych Najeźdźców, gdy ich trafi."
  )

S("You cannot attack Raiders directly!", "Nie możesz atakować Najeźdźców bezpośrednio!")

Orb("Slaying", "Pogromu")

S("This Orb lets you defeat Raiders and other tough single-cell monsters in melee.",
  "Ta Sfera pozwala Ci pokonywać Najeźdźców i inne potężne jednopolowe potwory w walce wręcz.")

// Jelly Kingdom

N("Jelly Kingdom", GEN_N, "Królestwo Galarety", "Królestwa Galarety", "Królestwo Galarety", "w Królestwie Galarety")

S("Some of the Slime Beasts have decided to revolt against the color rules in the "
  "Alchemist Lab. They have changed their shape and consistency, declared independence, and established their own Kingdom.\n\n"
  "Jellies switch between being a wall and being a monster after every treasure you collect.",
  
  "Część Mazistych Stworów z Laboratorium postanowiło zbuntować się przeciwko regułom koloru w "
  "Laboratorium. Zmieniły kształt i konsystencję, ogłosiły niepodległość i stworzyły własne Królestwo.\n\n"
  "Galareta zmienia swój stan z potwora na ścianę i odwrotnie z każdym zebranym skarbem."
  )

N("Red Jelly", GEN_F, "Czerwona Galareta", "Czerwone Galarety", "Czerwoną Galaretę", "Czerwoną Galaretą")
N("Blue Jelly", GEN_F, "Niebieska Galareta", "Niebieskie Galarety", "Niebieską Galaretę", "Niebieską Galaretą")

N("Tasty Jelly", GEN_F, "Pyszna Galaretka", "Pyszne Galaretki", "Pyszną Galaretkę", "Pyszną Galaretą")
S("A tasty byproduct of the Jelly Revolution.",
  "Pyszny produkt uboczny Galaretowej Rewolucji.")

Orb("Phasing", "Fazy")
S("This orb lets you pass through walls (one cell wide), and also through monsters, as long as they will not attack you in transit.",
  "Ta Sfera pozwala Ci przechodzić przez ściany (o szerokości jednego pola), a także przez potwory, o ile "
  "nie zaatakują one w trakcie ruchu.")

S("You cannot attack Jellies in their wall form!", "Nie możesz atakować Galarety w formie ściany!")

// keyboard help
//---------------

S("advanced keyboard shortcuts", "zaawansowane skróty klawiaturowe")

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

S("pics to save:", "obrazki do zapisu:")
S("level to save:", "mapa do zapisu:")
S("pics to load:", "obrazki do wczytania:")
S("level to load:", "mapa do wczytania:")

// new option
S("forget faraway cells", "zapominaj odległe pola")
N("Lost Memory", GEN_F, "Utracona Pamięć", "Utracone Pamięci", "Utraconą Pamięć", "w Utraconej Pamięci")
S(  "Because of the properties of hyperbolic geometry, it is extremely unlikely to randomly "
    "get back to a faraway place you have been to. However, you have managed to get there "
    "somehow. In the meantime, its memory has been cleared, since the 'forget faraway cells'"
    " option was on.",
    
    "Ze względu na właściwości geometrii hiperbolicznej praktycznie niemożliwe jest, by "
    "przypadkowo wrócić do odległego miejsca, w którym się było wcześniej. Ale jakoś Ci się to udało... "
    "W międzyczasie pamięć została wyczyczona, ze względu na włączoną opcję 'zapominaj odległe pola'."
    )

// shmup 
N("Blueball", GEN_F, "Niebieska kula", "Niebieske kule", "Niebieską kulę", "Niebieską kulą")
S("A powerful missile from a Blue Raider.", "Potężny pocisk Niebieskiego Najeźdźcy.")

// missing
S(" fps: ", " fps: ")
S("default value", "domyślna wartość")
S("You can scroll with arrow keys -- Ctrl to fine-tune", "Przewijaj strzałkami -- Ctrl by dostroić")
S("font scale", "rozmiar czcionki")
S("background particle effects", "cząsteczki w tle")

// not yet translated for Hypersian Rug mode
S("orthogonal", "ortogonalny")
S("perspective", "perspektywa")
S("model scale factor", "współczynnik skali modelu")

// stereo vision config
S("stereo vision config", "konfiguracja trybu stereo")

S("This allows you to view the world of HyperRogue in three dimensions. "
  "Best used with the Hypersian Rug mode. When used in the disk model, "
  "this lets you look at the Minkowski hyperboloid (which means the "
  "depth of terrain features is actually reversed). It also works with non-disk models, "
  "from the conformal menu.",
  "Ten tryb pozwala oglądać świat HyperRogue w trzech wymiarach. "
  "Najlepiej używać w trybie Hiperskiego Dywanu. Używane w modelu dysku "
  "pozwala patrzeć na hiperboloidę Minkowskiego (co oznacza, że głębokość "
  "terenu jest odwrócona). Działa też w innych modelach niż dysk, z trybu "
  "konforemnego."
  )

S("Currently, red-cyan anaglyph glasses and mobile VR googles are supported.",
  "Obecnie działa z czerwono-turkusowymi okularami i goglami VR do telefonów.")

S("stereo mode", "tryb stereo")
S("anaglyph", "anaglif")
S("side-by-side", "stereopara")

S("pupillary distance", "odległość źrenic")
S("The distance between your eyes in the represented 3D object. This is given in absolute units.",
  "Odległość między oczami w reprezentowanym obiekcie 3D. Podana w jednostkach absolutnych.")

S("distance between images", "odległość między obrazami")
S("The distance between your eyes. 1 is the width of the screen.",
  "Odległość między oczami. 1 to szerokość ekranu.")

// field of view
S("Horizontal field of view, in angles. "
  "This affects the Hypersian Rug mode (even when stereo is OFF) "
  "and non-disk models.",
  "Szerokość pola widzenia, w stopniach. "
  "Ma wpływ na tryb Hiperskiego Dywanu (nawet gdy stereo jest wyłączone) "
  "i modele inne niż dysk.")

// more magic configuration
S("affect model central point", "punkt centralny")
S("affect texture position X", "położenie tekstury X")
S("affect texture position Y", "położenie tekstury Y")

S("select master cells", "wybierz główne pola")
S("select master triangles", "wybierz główne trójkąty")
S("fine tune vertices", "dostrajanie wierzchołków")

// main menu
// an option to keep the current settings
S("use current/saved settings", "obecne/zapisane ustawienia")
// randomly chosen mode: hypersian rug
S("see the true form", "prawdziwy kształt")
// randomly chosen mode: heptagonal mode
S("more curvature", "większa krzywizna")

// anti-crossing in the Hypersian Rug
S("anti-crossing", "anty-krzyżowanie")
S("The anti-crossing algorithm prevents the model from crossing itself, "
  "by preventing points which should not be close from being close. "
  "The bigger number, the more sensitive it is, but the embedding is slower. Set 0 to disable.",
  
  "Algorytm zabrania modelowi krzyżowania się samemu ze sobą "
  "przez zabranianie odległym punktom znaleźć się blisko siebie. "
  "Im większa liczba, tym większa czułość, ale konstrukcja modelu "
  "jest wolniejsza. Ustaw 0, by wyłączyć."
  )

// new 3D models

// various menu names

S("constant curvature surfaces", "powierzchnie o stałej krzywiźnie")
S("smooth surfaces", "gładkie powierzchnie")
S("models of hyperbolic geometry", "modele geometrii hiperbolicznej")
S("history mode", "tryb historii") // old 'conformal/history mode' has been split into two
S("other 3D models", "inne modele 3D")

// various models

S("Minkowski hyperboloid", "hiperboloida")
S("plane", "płaszczyzna")
S("sphere", "sfera")
S("hemisphere", "półsfera")
S("tractricoid", "traktrysoida")
S("concave barrel", "wklęsła beczka")
S("Dini's surface", "powierzchnia Diniego")
S("Kuen surface", "powierzchnia Kuena")
S("inversion", "inwersja")

S(
  "In this menu you can choose from several known smooth surfaces of constant negative curvature. "
  "Since the curvature of hyperbolic plane is also constant negative, these surfaces "
  "are great to draw hyperbolic tesselations on. While they look great, only a small part "
  "of the hyperbolic plane can be visibly represented in this way, so they are not "
  "good for playing HyperRogue; however, the coverage extends far away in specific directions, "
  "so first increasing sight range in graphics config and texture size in hypersian rug settings may improve the effect. "
  "For convenience, you can also choose other 3D models from this menu.",
  
  "W tym trybie możesz wybrać spośród kilku znanych powierzchni o stałej ujemnej krzywiźnie. "
  "Krzywizna płaszczyzny hiperbolicznej jest również stała ujemna, także powierzchnie te "
  "świetnie się nadają do rysowania hiperbolicznych teselacji. Tylko mała część "
  "płaszczyzny hiperbolicznej może być pokazana w ten sposób, także nie nadają się "
  "one zbyt dobrze do grania w HyperRogue; pokrycie zato wybiega daleko w określonych kierunkach, "
  "także powiększenie pola widzenia w konfiguracji grafiki oraz rozmiaru tekstury w ustawieniach "
  "hiperskiego dywanu może poprawić efekt. Dla wygody możesz również wybrać inne tryby 3D "
  "z tego menu."
  )

// coverage
S("display coverage", "pokaż pokrycie")
S("on surface", "na powierzchni")
S("on H² (static)", "na H² (statycznie)")
S("on H² (dynamic)", "na H² (dynamicznie)")

S("Computing these models involves integrals and differential equations, which are currently solved numerically. This controls the precision.",
  "Obliczanie tych modeli wymaga całek i równań różniczkowych, które są obecnie rozwiązywane numerycznie. Ten parametr określa dokładność.")

// too bored for declension of %1 (either a surface name or a a part of a surface)
S("solving the geodesics on: %1, %2/%3", "obliczanie geodezyjnych na: %1, %2/%3")

S("the upper component", "górna składowa")
S("the lower center", "dół środek")
S("the lower left", "dół lewo")
S("the lower right", "dół prawo")

S("camera rotation in 3D models", "obrót kamery w trybach 3D")

S("Rotate the camera in 3D models (ball model, hyperboloid, and hemisphere). "
  "Note that hyperboloid and hemisphere models are also available in the "
  "Hypersian Rug surfaces menu, but they are rendered differently there -- "
  "by making a flat picture first, then mapping it to a surface. "
  "This makes the output better in some ways, but 3D effects are lost. "
  "Hypersian Rug model also allows more camera freedom.",
  
  "Obróć kamerę w modelach 3D (kula, hiperboloida, półsfera). "
  "Uwaga: model hiperboloidy i półsfery są też dostępne w menu "
  "powierzchni hiperskiego dywanu, ale są one tam renderowane w inny "
  "sposób -- poprzez zrobienie najpierw płaskiego rysunku, a potem "
  "odwzorowanie go na powierzchnię. To daje ładniejszy wynik pod "
  "pewnymi względami, ale gubi efekty 3D. Tryb hiperskiego dywanu "
  "pozwala za to na większą dowolność ustawienia kamery.")

// parameter for the given model
S("parameter", "parametr")
S( "Stereographic projection to a sphere. Choose the radius of the sphere.",
  "Rzut stereograficzny na sferę. Wybierz promień sfery.")
// parameter for Dini
S("The larger the number, the more twisted it is.",
  "Im większa liczba, tym bardziej poskręcana jest ta powierzchnia.")
// parameter for concave barrel
S("Controls the inner radius.", "Określa promień wewnętrzny.")

// improved rotation
S(
  "This controls the automatic rotation of the world. "
  "It affects the line animation in the history mode, and "
  "lands which have a special direction. Note that if finding this special direction is a part of the puzzle, "
  "it works only in the cheat mode.",
  
  "To ustawienie steruje automatycznym obracaniem świata. "
  "Ma wpływ na animację linii (tryb historii) i "
  "krainy ze szczególnym kierunkiem. Jeśli znalezienie "
  "tego szczególnego kierunku jest częścią zadania gracza, "
  "opcja ta działa tylko w trybie oszusta."
  )
S("NEVER", "NIGDY")

S("Mercator", "odwzorowanie Merkatora")
S("band equidistant", "wstęga ekwidystantna")
S("band equi-area", "wstęga ekwiwalentna")
S("sinusoidal", "sinusoida")
S("two-point equidistant", "rzut dwupunktowy ekwidystantny")

S(
  "This model maps the world so that the distances from two points "
  "are kept. This parameter gives the distance from the two points to "
  "the center.",
  
  "Ten model przekształca świat tak, że odległości od dwóch punktów "
  "są zachowane. Ten parametr zadaje odległość tych dwóch punktów "
  "od środka.")

S("enable the Camelot cheat", "włącz oszustwo w Camelot")
S("disable the Camelot cheat", "wyłącz oszustwo w Camelot")
S("gain Orb of the Sword", "zdobądź Sferę Ostrza")
S("Static mode enabled.", "Tryb statyczny włączony.")
S("Static mode disabled.", "Tryb statyczny wyłączony.")

S("Returned to your game.", "Wracasz do swojej gry.")
S("Spherical version of %the1. ", "%1 w wersji sferycznej. ")
S("Euclidean version of %the1. ", "%1 w wersji euklidesowej. ")
S("Click again to go back to your game.", "Kliknij ponownie by wrócić do swojej gry.")
S("Press %1 again to go back to your game.", "Wciśnij %1 ponownie by wrócić do swojej gry.")
S("return to your game", "Wróć do gry")

S("orthogonal projection", "rzut ortogonalny")
S("stereographic projection", "rzut stereograficzny")
S("zoomed stereographic projection", "zbliżony rzut stereograficzny")
S("zoomed out", "oddalenie")
S("zoomed in", "zbliżenie")
S("gnomonic projection", "rzut gnomoniczny")
S("Goldberg", "Goldberg")
S("fisheye", "rybie oko")
S("paint pictures", "maluj obrazki")
S("list of slides", "lista slajdów")
S("back to the start menu", "powrót do menu startowego")
S("You juggle the Dead Orbs.", "Żonglujesz Martwymi Sferami.")
S("You give %the1 a grim look.", "Spoglądasz ponuro na %a1.")

S("Strange Challenge", "Dziwna Misja")
S("compete with other players on random lands in random geometries", "konkuruj z innymi na losowych krainach w losowych geometriach")

S("Strange Challenge #%1", "Dziwna Misja #%1")
S("next challenge in %1", "do następna misji: %1")
S("synchronizing scores...", "synchronizacja trwa...")
S("already played", "już gra%łeś0")
S("play!", "graj!")
S("See past challenges", "minione misje")
S("Past Strange Challenges. You can see the setting and high scores, but you can no longer play them.",
  "Minione Dziwne Misje. Możesz oglądać ustawienia i wyniki, ale nie możesz już w nie grać.")

// land compatibility guides

S("Cannot kill Hedgehog Warriors in this geometry.", "W tej geometrii nie da się zabić Wojownika-Jeża.")
S("No random pattern version.", "Tej krainy nie ma w trybie losowych wzorów.")
S("Great Walls not implemented.", "Wielkie ściany nie zaimplementowane w tej geometrii.")
S("Pattern incompatible.", "Niekompatybilność wzoru.")
S("Pattern not implemented -- using random.", "Wzór nie zaimplementowany -- generujemy losowo.")
S("Pattern not implemented.", "Wzór nie zaimplementowany.")
S("Not enough space.", "Za mało miejsca.")
S("Does not work in this geometry.", "Nie działa w tej geometrii.")
S("This land is designed for bounded worlds.", "Ta kraina jest zaprojektowana dla światów skończonych.")
S("This land is designed for infinite worlds.", "Ta kraina jest zaprojektowana dla światów nieskończonych.")
S("This land is designed for infinite worlds or big spheres.", "Ta kraina jest zaprojektowana dla światów nieskończonych i dużych sfer.")
S("Out of theme for the full game.", "Nie pasuje tematycznie do pełnej gry.")
S("No game here.", "Tu nie ma gry.")
S("Does not work in chaos mode.", "Nie działa w trybie chaosu.")
S("Not in the full game.", "Nie występuje w pełnej grze.")
S("Special construction in the Chaos mode.", "Specjalna konstrukcja w trybie Chaosu.")
S("Special construction in the Euclidean mode.", "Specjalna konstrukcja w geometrii euklidesowej.")
S("Special construction in this geometry.", "Specjalna konstrukcja w tej geometrii.")
S("Not implemented.", "Nie zaimplementowane.")
S("Partially implemented.", "Częściowo zaimplementowane.")
S("No comments.", "Brak komentarza.")
S("Does not work in pure tactics mode.", "Nie działa w trybie taktycznym.")
S("Technical.", "Kraina techniczna.")
S("Full game.", "Pełna gra.")
S("Somewhat inaccurate.", "Niezbyt dokładna realizacja.")
S("Mercury rivers not implemented (or could not work) in this geometry.", "Rzeki rtęci nie zaimplementowane (lub nie działają) w tej geometrii.")
S("Patterns compatible.", "Kompatybilność wzorów.")
S("This land is specially designed for this geometry.", "Kraina specjalnie zaprojektowana dla tej geometrii.")
S("Three-colorability required.", "Ta kraina wymaga trzy-kolorowalności.")
S("Land not implemented.", "Kraina niezaimplementowana.")
S("Special interest.", "Szczególnie interesująca kombinacja.")
S("Better version exists.", "Istnieje lepsza wersja.")
S("Grid does not work in this geometry.", "Siatka nie działa w tej geometrii.")
S("Graphics not implemented in this geometry.", "Grafika nie zaimplementowana w tej geometrii.")
S("This land does not work in the current settings. Reason not available.",
  "Ta kraina nie działa w obecnych ustawieniach. Powód nieznany.")
S("This land does not work well in the current settings. Reason not available.",
  "Ta kraina źle działa w obecnych ustawieniach. Powód nieznany.")
S("This combination is known to be buggy at the moment.", "Ta kombinacja działa w tym momencie błędnie.")
S("Somewhat sloppy pattern.", "Lekko niedbały wzór.")
S("Fractal landscapes not implemented in this geometry.", "Fraktalne krajobrazy nie zaimplementowane w tej geometrii.")
S("Only simplified walls implemented.", "Tylko uproszczone ściany są zaimplementowane.")

S("cube/elliptic", "sześcian/el")
S("chamfered", "chamfered")
S("2x chamfered", "2x chamfered")
S("select", "wybierz")

S("Goldberg polyhedra are obtained by adding extra hexagons to a dodecahedron. "
  "GP(x,y) means that, to get to a nearest non-hex from any non-hex, you should move x "
  "cells in any direction, turn right 60 degrees, and move y cells. "
  "HyperRogue generalizes this to any tesselation with 3 faces per vertex. "
  "By default HyperRogue uses bitruncation, which corresponds to GP(1,1).",
  
  "Wielościany Goldberga powstają przez dodanie dodatkowych sześciokątnych ścian do dwunastościanu. "
  "GP(x,y) oznacza że, aby z danego nie-sześciokąta dostać się do najbliższego innego nie-sześciokąta, "
  "trzeba się ruszyć x pól w pewnym kierunku, skręcić o 60 stopni w prwo, i ruszyć się jeszcze o y pól. "
  "HyperRogue uogólnia powyższą konstrukcję na dowolną posadzkę z 3 ścianami stykającymi się w wierzchołku. "
  "Domyślnie HyperRogue używa operacji GP(1,1).")
  
S("extend the ends", "rozszerz końce")

S("Forbidden cells are marked with a different color.", "Zabronione pora są oznaczone innym kolorem.")
S("This orb lets you attack adjacent cells %1 steps from the primary target.", "Ta Sfera pozwala atakować pola %1 kroków od głównego celu.")

S("Note: the rules above correspond to the standard geometry; actual rules in other geometries may be different. ",
  "Uwaga: reguły powyżej są dla standardowej geometrii; właściwe reguły w innych geometriach mogą być trochę inne. ")

S("Note: the rules refer to colors which are not visible in other lands. ", "Uwaga: reguły powyżej się odnoszą do kolorów niewidocznych w innych krainach. ")

S("When the 'mark heptagons' option (hotkey '7') is on, forbidden moves are marked.", 
  "Kiedy opcja 'oznaczenia na siedmiokątach' (klawisz '7') jest włączona, zabronione ruchy są zaznaczone.")

S("When the 'mark heptagons' option (hotkey '7') is on, moves between unmarked cells are forbidden.",
  "Kiedy opcja 'oznaczenia na siedmiokątach' (klawisz '7') jest włączona, ruchy pomiędzy nieoznaczonymi polami są zabronione.")

// 10.4p

// in the sight range dialog
S("enable the cheat mode for additional options", "włącz tryb oszusta, by tu były dodatkowe opcje")

// steal a baby tortoise with Orb of Space
S("No, that would be heartless!", "Nie, to by było bez serca!")

S("show quotient spaces", "pokazuj przestrzenie ilorazowe")
S("minimal quotient", "minimalna ilorazowa")
S("Klein Quartic", "Kwartyka Kleina")
S("Bolza Surface", "Powierzchnia Bolzy")
S("Bolza Surface x2", "Powierzchnia Bolzy x2")

// extra texts for Goldberg 

S("variations", "warianty")
S("auto-adjust in Goldberg grids", "automatyczne dostrajanie w siatkach Goldberga")
S("2x bitruncated", "2x bitruncated")
S("expanded", "rozszerzona")
S("rectified", "rectified")

// irregular grids

S("irregular", "nieregularna")
S("bad cells: %1", "złe pola: %1")
S("too many edges: %1", "za dużo krawędzi: %1")
S("median edge: %1 minimum: %2", "mediana: %1 minimum: %2")
S("cells badly paired: %1", "pola źle sparowane: %1")
S("cells not covered: %1", "pola niezakryte: %1")
S("adjacent cells from nonadjacent heptagons: %1", "sąsiednie pola niesąsiednich siedmiokątów: %1")
S("bad format or bad map geometry", "zły format lub zła geometria mapy")
S("irregular grid", "nieregularna siatka")
S("density", "gęstość")
S("min edge to median", "min krawędź / mediana")
S("quality", "jakość")
S("save", "zapisz")
S("load", "wczytaj")
S("irregular to save:", "mapa nieregularna do zapisu:")
S("irregular to load:", "mapa nieregularna do wczytania:")
S("Map saved to %1", "Mapa zapisana do %1")
S("Failed to save map to %1", "Nie udało się zapisać mapy do %1")
S("Map loaded from %1", "Mapa wczytana z %1")
S("Failed to load map from %1", "Nie udało się wczytać mapy z %1")
S("reset", "resetuj")
S("OK", "OK")
S("ready", "gotowe")

S("This option creates irregular grids to play the game on. "
  "Currently rather slow algorithms are used, "
  "so not recommended with too high density or "
  "with too large periodic base geometry. "
  "For technical reasons, the density cannot be too small.",

  "Ta opcja tworzy nieregularne siatki do gry. "
  "Obecnie używane algorytmy ąs dosyć powolne, "
  "zatem nie jest ona zalecana przy za dużej gęstości "
  "lub za dużym okresie bazowej geometrii. "
  "Z powodów technicznych gęstość nie może być za mała."
  )

// vertical stretch

S("vertical stretch", "rozciągnięcie w pionie")
S("Vertical stretch factor.", "Współczynnik rozciągnięcia w pionie.")
S("Gall-Peters", "Gall-Peters")
S("The current value makes the map conformal at the latitude of %1 (%2°).", "Obecna wartość powoduje, że mapa jest konforemna na szerokości %1 (%2°).")
S("The current value makes the map conformal %1 units from the main line.", "Obecna wartość powoduje, że mapa jest konforemna %1 jednostek od głównej linii.")

// line quality

S("line quality", "jakość linii")

S("Higher numbers make the curved lines smoother, but reduce the performance.",
  "Wyższe liczby powodują, że linie krzywe są bardziej gładkie, ale pogarszają wydajność.")

// new options in mobile

S("FIRE", "CEL")

S("scrolling by device rotation", "przewijanie przez obrót urządzenia")

S(  "This lets you scroll the map by rotating your device. It can be e.g. used to "
    "play the spherical mode of HyperRogue in mobile VR goggles -- the \"spherical VR\" "
    "button configures this; this VR mode can be disabled by touching the screen for 1 second.",

  "Ta opcja pozwala przewijać mapę poprzez obrót urządzenia. Można jej użyć np. "
  "do grania w tryb sferyczny HyperRogue w mobilnych goglach VR -- przycisk \"sferyczna VR\" "
  "to konfiguruje; ten tryb VR można wyłączyć dotykając ekranu przez 1 sekundę.")

S("sensitivity", "czułość")
S("1 means that rotating the device by 1 radian corresponds to scrolling by 1 unit. In spherical geometry, 1 unit = 1 radian.", 
  "1 oznacza, że przekręcenie urządzenia o 1 radian odpowiada przesunięciu o 1 jednostkę. W geometrii sferycznej 1 jednostka = 1 radian.")

S("spherical VR", "sferyczna VR")



S("aura from texture", "aura z tekstury")

// extra for irregular
S("bitruncation count", "liczba przycięć")

S("Bitruncation introduces some regularity, allowing more sophisticated floor tilings and textures.", 
  "Przycięcie wprowadza pewną regularność, pozwalając na bardziej zaawansowane posadzki i tekstury."
  )

S(
      "The smallest allowed ratio of edge length to median edge length. "
      "Tilings with low values are easier to generate, but tend to be more ugly.", 
      "Najmniejszy dozwolony stosunek długości krawędzi do mediany długości krawędzi. "
      "Kafelki otrzymane dla małych wartości są łatwiejsze do wygenerowania, ale są zwykle brzydsze."
      )

// missing

S("The Orb of Love gives no bonus score in the Strange Challenge.", "Sfera Miłości nie daje permii w Dziwnej Misji.")
S("This does not work in non-orientable geometries", "To nie działa w geometriach nieorientowalnych")
S("This pattern needs x-y divisible by 3", "Ten wzór wymaga x-y podzielnego przez 3")
S("This pattern needs x-y divisible by 2", "Ten wzór wymaga x-y podzielnego przez 2")

// missing from the texture editor
S("p = color", "p = kolor")
S("b = brush size: %1", "b = pędzel: %1")
S("d = draw", "d = rysuj")
S("l = line", "l = linia")
S("c = circle", "c = kółko")

// new binary tiling

S("binary tiling", "parkietaż binarny")

// Archimedean tilings

S("Archimedean tilings", "parkietaże Archimedesowe")
S("Archimedean", "Archimedes")

S("This lets you generate tilings where each cell is a regular polygon, and each vertex is identical. "
  "You can choose from a big list of known Archimedean tilings, or create your own. "
  "The format of specifying Archimedean tilings is based on one used by the Hyperbolic Applet by Don Hatch.\n\n"
  "You can enhance the gameplay/land generation by adding special codes after faces: "
  "H ('heptagon' for Ruined City, Graveyard, etc.), L ('line' for Zebra, Power, Vineyard, etc.).\n"
  "Crystal World and Warped Coast work on (k,l,m) tilings by removing/enlarging the first tile listed ('k').",

  "Tutaj możesz generować parkietaże, w których każde pole jest wielokątem foremnym, i każdy wierzchołek jest identyczny. "
  "Możesz wybrać z dużej listy znanych parkietaży archimedesowych, lub stworzyć własny. "
  "Format zadawania parkietaży jest oparty na Hyperbolic Applet Dona Hatcha.\n\n"
  "Możesz ulepszyć rozgrywkę/generowanie świata przez dodanie specjalnych kodów po ścianach: "
  "H ('siedmiokąt' dla Zrujnowanego Miasta, Cmentarza itd.), L ('linia' dla Zebry, Krainy Mocy, Winnicy, itd.).\n"
  "Kryształowy Świat i Zakrzywiony Brzeg działają na parkietażach (k,l,m) poprzez usuwanie/powiększanie "
  "pierszego podanego kafelka ('k').")

S("color by symmetries", "koloruj przez symetrie")
S("color by sides", "koloruj przez liczby boków")

S("edit", "edytuj")
S("full angle", "kąt pełny")
S("not enough faces", "za mało ścian")
S("not enough edges", "za mało krawędzi")
S("polygons match incorrectly", "wielokąty źle się łączą")
S("currently no more than %1 edges", "obecnie nie więcej niż %1 krawędzi")
S("currently no more than %1 faces in vertex", "obecnie nie więcej niż %1 ścian w wierzchołku")

N("Canvas", GEN_N, "Płótno", "Płótna", "Płótno", "na Płótnie")
S("find the player", "do gracza")
S("press SPACE to find the player", "SPACE = do gracza")

S("edge length", "długość krawędzi")
S(" (arbitrary)", " (dowolna)") // arbitrary edge length
S("previous page", "poprzednia strona")
S("Defined pattern.", "Wzór zdefiniowany.")

// graphics editor
S(" (r = complex tesselations)", " (r = złożone parkietaże)")
S("area: %1", "pole: %1")

// short codes for variations
S(" (d)", " (d)") // dual
S(" (g)", " (g)") // Goldberg
S(" (i)", " (i)") // irregular

// pattern types
S("cell types", "typy pól")
S("siblings", "bliźniaki") // a pattern on the sphere
S("rotate the color groups", "obróć kolory")
S("full symmetry", "pełna symetria")
S("alternate coloring", "kolorowanie alternatywne")
S("display full floors", "pokazuj pełne podłogi")
S("remove complete classification", "usuń pełną klasyfikację")

// animations
S("animations/history", "animacje/historia")
S("animations", "animacje")
S("period", "okres")
S("game animation period", "okres animacji gry")
S("no movement animation", "brak animacji ruchu")
S("translation", "przesunięcie")
S("parabolic", "izometria paraboliczna")
S("circle", "ruch po kole")
S("animate parameter change", "animacja zmiany parametrów")
S("choose parameters to animate", "wybierz parametry do animacji")
S("frames to record", "liczba kratek do nagrania")
S("record to a file", "nagraj do pliku")

// improved worldsize demo dialog
S("which distance", "która odległość")
S("start", "start")
S("land", "kraina")

S("number codes", "liczby")
S("distance", "odległość")
S("type", "typ")
S("debug", "debug")

S("color codes", "kolory")
S("display distances from", "pokazuj odległości od")
S("disable", "wyłącz")
S("move the player", "ruch gracza")
S("show number of cells by distance", "pokaż liczbę pól w każdej odległości")
S("show number of descendants by distance", "pokaż liczbę potomków w każdej odległości")

S("triangle grid: tree edges", "siatka trójkątna: krawędzie drzewa")
S("triangle grid: other edges", "siatka trójkątna: inne krawędzie")
S("dual grid", "siatka dualna")
S("horocycles", "horocykle")
S("configure", "konfiguracja")
S("scroll", "przewijanie")

// improved sight range dialog
S("minimum visible cell in pixels", "najmniejszy rozmiar rysowanego pola w pikselach")
S("draw range based on distance", "zakres rysowania na podstawie odległości")
S("draw based on size in the projection (no generation)",
  "rysuj na podstawie rozmiaru w projekcji (bez generacji)")
S("draw based on size in the projection (generation)",
  "rysuj na podstawie rozmiaru w projekcji (z generacją)")
S("cells drawn", "liczba narysowanych pól")
S("limit on cells drawn", "ograniczenie na liczbę narysowanych pól")
S("This limit exists to protect the engine from freezing when too many cells would be drawn according to the current options.",
  "To ograniczenie istnieje by uchronić silnik przed zawieszeniem się w przypadku, gdyby zgodnie z obecnymi opcjami było za dużo pól do narysowania.")
S("generation range bonus", "bonus do zasięgu generacji")
S("game range bonus", "bonus do zasięgu gry")

// new options for models

S("Joukowsky transform", "odwzorowanie Żukowskiego")
S("Joukowsky+inversion", "Żukowski + inwersja")

S("maximum z coordinate to show", "największa rysowana wartość z")

S("model transition", "przejście między modelami")
S("You can change this parameter for a transition from another model to this one.",
  "Możesz zmienić ten parametr, by obejrzeć przejście z innego modelu do tego.")

S("model orientation", "orientacja modelu")

// 10.4z

S("left-handed", "leworęczn%y0")

// formula features

S(
  "This lets you specify the projection as a formula f. "
  "The formula has access to the value 'z', which is a complex number corresponding to the (x,y) coordinates in the currently selected model; "
  "the point z is mapped to f(z). You can also use the underlying coordinates ux, uy, uz.",
  
  "Tu możesz zdefiniować własną projekcję podając wzór f. "
  "Wzór może zależeć od zmiennej 'z', będącej liczbą zespoloną odpowiadającą współrzędnym (x,y) w obecnie ustawionym modelu; "
  "punkt z będzie przekształcony w f(z). Możesz też używać współrzędnych podstawowych ux, uy, uz.")

S("Functions available: %1", "Dostępne funkcje: %1")

S("animate parameters", "animacja parametrów")

S(
  "Most parameters can be animated simply by using '..' in their editing dialog. "
  "For example, the value of a parameter set to 0..1 will grow linearly from 0 to 1. "
  "You can also use functions (e.g. cos(0..2*pi)) and refer to other parameters.",
  
  "Większość parametrów może być animowana w prosty sposób, przez wpisanie '..' w ich pole edycji. "
  "Przykładowo, wartość parametru ustawiona na 0..1 będzie rosła liniowa od 0 do 1. "
  "Możesz też używać funkcji (np. cos(0..2*pi)) lub odnosić się do innych parametrów.")

// color edit dialog

S("background", "tło")
S("foreground", "pierwszy plan")
S("projection boundary", "brzeg modelu")
S("projection background", "tło modelu")
S("projection period", "okres modelu")
S("dialogs", "dialogi")
S("canvas colors", "kolory płótna")
S("minefield colors", "kolory pola minowego")
S("distance colors", "kolory odległości")

// extra conformal models

S("rotated hyperboles", "obrócone hiperbole")
S("spiral", "spirala")

S("spiral angle", "kąt spirali")
S("spiral period: x", "okres spirali: x")
S("spiral period: y", "okres spirali: y")
S("match the period of the torus", "dopasuj do okresu torusa")

// extra patterns

S("circles", "koła")
S("radii", "promienie")
S("meridians", "południki")
S("parallels", "równoleżniki")
S("seven-coloring", "siedmiokolorowanie")

S("half-plane scale", "skala półpłaszczyzny")
S("use GPU to compute projections", "obliczanie projekcji przy użyciu GPU")
S("N/A", "niedostępne")
S(" (2D only)", " (tylko 2D)")

// 10.5d

// this line was missing...
S("geometry experiments", "eksperymenty z geometrią")

S("cylinder (squares)", "walec (kwadraty)")
S("cylinder (hex)", "walec (heksy)")
S("Möbius band (squares)", "wstęga Möbiusa (kwadraty)")
S("Möbius band (hex)", "wstęga Möbiusa (heksy)")

S("Macbeath Surface", "powierzchnia Macbeatha")
S("Bring's Surface", "powierzchnia Bringa")
S("Schmutz's M(3)", "M(3) Schmutza")
S("Schmutz's M(4)", "M(4) Schmutza")

S("dimensional crystal", "wielowymiarowy kryształ")

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
    
    "Ta geometria pozwala Ci grać na d-wymiarowej siatce. Wybierz 3 wymiary i "
    "'tryb 3D' by zobaczyć, jak to działa -- gramy na okresowej powierzchni w "
    "trzech wymiarach, złożonej z sześciokątów; każdy sześciokąt łączy się "
    "z 6 innymi sześciokątami, w 6 kierunkach. Normalnie gra przedstawia "
    "sytuację z punktu widzenia istoty żyjącej na tej powierzchni (wyregulowanej i "
    "wygładzonej), przy założeniu, że promienie świetlne również są ograniczone "
    "do powierzchni -- w związku z czym sytuacja wygląda jak parkietaż {2d,4}, "
    "poza tym, że promienie świetlne czasami zataczają pętlę, w związku z czym "
    "w niektórych kierunkach widzisz obrazy samego siebie (innymi słowami, "
    "siatka d-wymiarowa jest przestrzenią ilorazową parkietażu hiperbolicznego). "
    "Ta sama konstrukcja działa też w innych wymiarach. Pół-wymiary są "
    "interpretowane w sposób następujący: dodatkowy wymiar ma 2 'poziomy', na przykład "
    "2.5D ma górną i dolną płaszczyznę.\n\n"
    "Można przyciąć (bitruncate) tą geometrię -- powoduje to, że lepiej ona współgra z regułami "
    "HyperRogue, ale jest trochę trudniejsza do zrozumienia.")

S("4D double bitruncated", "4 wymiary, podwójna bitrunkacja")
S("view coordinates in the cheat mode", "pokazuj współrzędne w trybie oszustwa")
S("compass probability", "prawdopodobieństwo kompasu")
S("3D display", "tryb 3D")

S("\"Each piece of the Round Table is exactly %1 steps away from the Holy Grail.\"",
  "\"Każdy fragment stołu jest dokładnie %1 kroków od Świętego Graala.\"")

S("\"According to Merlin, the Round Table is a perfect Euclidean sphere in %1 dimensions.\"",
  "\"Merlin mówi, że Okrągły Stół jest dokładną Euklidesową sferą w %1 wymiarach.")

S(
  "Lands in this geometry are usually built on North-South or West-East axis. "
  "Compasses always point North, and all the cardinal directions to the right from compass North are East (this is not "
  "true in general, but it is true for the cells where compasses are generated). "
  "North is the first coordinate, while East is the sum of other coordinates.",
  
  "Krainy w tej geometrii są zwykle budowane na osi Północ-Południe lub Wschód-Zachód. "
  "Kompasy zawsze wskazują Północ, i wszystkie główne kierunki na prawo od Północy kompasu są zachodnie "
  "(to nie jest zawsze prawda, ale jest to prawda na polach z kompasami). "
  "Północ jest pierwszą współrzędną, a Wschód jest sumą pozostałych.")

S("crystal coordinate colors", "kolory współrzędnych kryształu")

S("3D monsters/walls on the surface", "potwory/ściany 3D na powierzchni")
S("no 3D effects available in this projection", "ta projekcja nie ma efektów 3D")

// new screenshot menu

S("screenshots", "zrzuty ekranu")
S("format", "format")
S("pixels (X)", "piksele (X)")
S("pixels (Y)", "piksele (Y)")
S("supersampling", "supersampling")
S("transparent", "przezroczysość")
S("gamma", "współczynnik gamma")
S("higher value = darker", "wyższa wartość = ciemniej")
S("brightness", "jasność")
S("higher value = lighter", "wyższa wartość = jaśniej")
S("show the HUD", "pokazuj HUD")
S("SVG screenshots do not work in this 3D mode", "zrzuty ekranu w formacie SVG nie działają w tym trybie 3D")
S("SVG screenshots do not work with textures", "zrzuty ekranu w formacie SVG nie działają z teksturami")
S("take screenshot", "zrób zrzut ekranu")
S("screenshot", "zrzut ekranu")
S("shot settings", "ustawienia zrzutów ekranu")

// new warnings

S("WARNING", "UWAGA")
S("YES", "TAK")
S("NO", "NIE")

S("Are you sure you want to hit %the1?", "Czy na pewno chcesz uderzyć %a1?")
S("This will exit HyperRogue without saving your current game. Are you sure?",
  "To spowoduje wyjście z HyperRogue bez zapisania Twojej obecnej gry. Czy na pewno?")
S("This will end your current game and start a new one. Are you sure?",
  "To zakończy Twoją obecną grę i zacznie nową. Czy na pewno?")
S("This will enable the cheat mode, making this game ineligible for scoring. Are you sure?",
  "To uruchomi tryb oszusta. Wyniki przestaną się liczyć. Czy na pewno?")

// 11.0

S("debug values", "dane do debugowania")
S("internal details", "szczegóły wewnętrzne")
S("show debug data", "pokaż dane do debugowania")
S("click a cell to edit it", "kliknij pole by je edytować")
S("click a cell to view its data", "kliknij pole by obejrzeć jego wartości")

S("(these lands are not in game)", "(te krainy nie występują w tej grze)")

// new lands

N("Brown Island", GEN_F, "Brązowa Wyspa", "Brązowe Wyspy", "Brązową Wyspę", "na Brązowej Wyspie")

S("The Brown Islands have been generated by ancient underground creatures, who moved randomly and raised lands in their path... "
  "adults spawned larvae in each move, which also moved randomly and also raised lands in their path, and eventually became adults and spawned their own larvae. "
  "The creatures spawned exponentially, but the space they lived in still grew faster, so there was enough space for the whole spawn...\n\n"
  "That was a long time ago. Now, the mountainous Brown Islands are inhabited by strange monsters. Good luck adventuring!",
  
  "Brązowe Wyspy zostały stworzone przez starożytne, podziemne istoty, które poruszały się losowo i wznosiły ziemię na swojej drodze... "
  "dorosłe istoty rozmnażały się po każdym ruchu, larwy również poruszały się losowo i wznosiły ziemię na swojej drodze, w końcu "
  "dorastając i wytwarzając swoje własne larwy. Istoty rozmnażały się wykładniczo, ale żyły w przestrzeni rosnącej jeszcze szybciej, "
  "także było dość miejsca dla całej rodziny...\n\n"
  "To było dawno temu. W obecnych czasach Brązowe Wyspy są zamieszkane przez dziwne potwory. Powodzenia w poszukiwaniu przygód!")
  

N("Bronze Beast", GEN_O, "Brązowy Stwór", "Brązowe Stwory", "Brązowego Stwora", "Brązowym Stworem")

S("A large bug native to the Brown Islands. Cannot be killed easily due to their tough armor; still, they can be killed by pushing them into water or from great heights. "
"Bronze Beasts are very bad at descending slopes -- it takes them extra time to get down, and they are stunned for a longer time if you push them down.",

  "Duży owad żyjący na Brązowych Wyspach. Ma twardą pokrywę, przez co nie można go łatwo pokonać; można go jednak pokonać spychając go do wody lub z dużej wysokości. "
  "Brązowe Bestie słabo sobie radzą z chodzeniem w dół -- zajmuje im to więcej czasu, i kiedy zepchniesz je w dół, są ogłuszone przez dłuższy czas niż normalnie.")

N("Acid Gull", GEN_F, "Kwaśna Mewa", "Kwaśne Mewy", "Kwaśną Mewę", "Kwaśną Mewą")

S("Where did this strange bird come from?...\n\nAcid Gulls dissolve the land on which they fall when they die. ",

  "Skąd się wziął ten dziwny ptak?...\n\nKwaśne Mewy rozpuszczają ląd, na który spadają, gdy zostaną zabite. ")

N("Tiger's Eye", GEN_N, "Tygrysie Oko", "Tygrysie Oka", "Tygrysie Oko", "Tygrysim Okiem")
S("A brown gem.", "Brązowy kamień.")

Orb("Choice", "Wyboru")

S("Did you know that it is possible to break a magical sphere into finitely many parts, and then, out of the parts obtained, to make two magical spheres, just like the original? "
  "Hyperbolic geometry makes this even easier!\n\n"
  "When you pick up another Orb, it will not disappear from the ground -- Orb of Choice will be drained instead.",
  
  "Czy wiedzia%łeś0, że można rozbić magiczną sferę na skończenie wiele części, by potem z uzyskanych części złożyć dwie magiczne sfery, dokładnie takie jak ta wyjściowa? "
  "Geometria hiperboliczna powoduje, że staje się to łatwiejsze!\n\n"
  "Kiedy podniesiesz inną Sferę, nie zniknie ona z ziemi -- zamiast tego stracisz Sferę Wyboru.")
    
N("Free Fall", GEN_O, "Swobodny Spadek", "Swobodne Spadki", "Swobodny Spadek", "w Swobodnym Spadku")

S("What on one side looks to be a normal (well, infinite) horizontal wall, on the other side turns out to be the vertical wall"
  " of an infinitely high tower. Jump from the window, and let the magical gravity carry you...",

  "Co wygląda z jednej strony na zwykłą (nieskończoną) poziomą ścianę, z drugiej strony okazuje się być pionową ścianą nieskończenie "
  "wysokiej wieży. Wyskocz przez okno i daj się nieść magicznej grawitacji...")

N("Falling Dog", GEN_O, "Spadający Pies", "Spadające Psy", "Spadajacego Psa", "Spadającym Psem")

S("Distant relatives of the Running Dogs.", "Dalecy krewni Biegnących Psów.")

N("Western Hawk", GEN_O, "Zachodni Jastrząb", "Zachodnie Jastrzębie", "Zachodniego Jastrzębia", "Zachodnim Jastrzębiem")

N("Meteorite", GEN_O, "Meteoryt", "Meteoryty", "Meteoryt", "Meteorytem")

S("These rocks falling from the sky have been captured to fall forever in the artificial gravity. Meteorite iron is believed to be a valuable material for magical weapons.",
  "Te kamienie spadły z nieba, i zostały złapane przez sztuczną grawitację, w której będą spadać wiecznie. Żelazo pochodzące z meteorytów jest wartościowym materiałem, "
  "z którego wyrabiane są magiczne bronie.")

Orb("Gravity", "Grawitacji")

S("This Orb lets you magically control gravity around you. In lands with unusual gravity, the options are: usual gravity (no change, except that it may disrupt birds), "
  "anti-gravity (causes the direction of gravity to be reversed), levitation (you can move in directions which are neither up nor down, "
  "or between cells if one of them has a wall in a 'neutral' direction). In lands with standard gravity, levitation lets creatures to avoid traps, chasms, "
  "and water, while anti-gravity makes it possible to move only when next to a wall (movement between cells without adjacent walls is not allowed). "
  "For each move, the gravity is based on what you do, and all enemies in the effective range are affected (for example, if you move next to a wall in a non-gravity land, "
  "anti-gravity kicks in and monsters will not be able to move in open space).",
  
  "Ta Sfera pozwala Ci sterować grawitacją w Twoim otoczeniu. W krainach z nietypową grawitacją opcje są następujące: zwykła grawitacja (bez zmian, oprócz tego, "
  "że psuje lot ptaków), anty-grawitacja (odwraca kierunek grawitacji), lewitacja (można poruszać się w kierunkach nie będących ani w górę ani w dół, "
  "oraz między takimi polami, że jedno z nich ma sąsiadującą ścianę w kierunku neutralnym). W krainach ze standardową grawitacją, lewitacja pozwala "
  "na ignorowanie pułapek, przepaści i wody, podczas gdy antygrawitacja powoduje, że ruch jest możliwy tylko w sąsiedztwie ściany (nie można poruszać się "
  "między polami bez sąsiadujących ścian). W każdym ruchu grawitacja w okolicy jest określana na podstawie tego co robisz, i ma to wpływ na wszystkie istoty w zasięgu "
  "(przykładowo, gdy w krainie bez grawitacji ruszasz się koło ściany, uruchamiana jest anty-grawitacja i potwory nie mogą ruszać się w otwartej przestrzeni).")

N("Irradiated Field", GEN_N, "Napromieniowane Pole", "Napromieniowane Pola", "Napromieniowane Pole", "na Napromieniowanym Polu")

S("These fields are ravaged with many kinds of magical radiation, which not only make the ground glow nicely in various colors, "
  "but also cause the inhabitants to protect the treasures of their land in various ways. In some areas of the Irradiated Fields, "
  "you will meet powerful enemies; in other areas, you will find arrow traps, fire traps, or pools, which can kill you if you are "
  "careless, or help you if you know how to use them to your advantage. Will you walk through the Irradiated Fields randomly, or "
  "try to find areas where treasures are common but nasty monsters are not, and keep to them? It is your choice!\n\n",
  
  "Na tych polach działa wiele rodzajów promieniowania magicznego. Promieniowanie to nie tylko powoduje, że ziemia ładnie "
  "się świeci w różnych kolorach, ale także powoduje, że mieszkańcy chronią skarbów tej ziemi na różne sposoby. "
  "W niektórych rejonach spotkasz potężnych przeciwników; w innych, znajdziesz pułapki strzelające i wybuchające, albo stawy, "
  "które mogą Cię zabić jeśli nie uważasz, albo mogą być pomocne, jeśli umiesz z nich korzystać. Czy będziesz poruszać się "
  "przez Napromieniowane Pole przypadkowo, czy postarasz się trzymać miejsc, gdzie skarbów jest dużo, a niebezpiecznych "
  "potworów mało? Twój wybór!")

N("Mutant", GEN_M, "Mutant", "Mutanci", "Mutanta", "Mutantem")

S("These guys look a bit strange, but they have no special properties.",
  "Ci goście wyglądają trochę dziwnie, ale nie mają specjalnych własności.")
  
N("fire trap", GEN_F, "ognista pułapka", "ogniste pułapki", "ognistą pułapkę", "ognistą pułapką")

S("This trap will explode when stepped on, setting all the adjacent cells on fire. However, this happens on the next turn, "
  "so you can safely escape if you are fast enough.",
  "Ta pułapka wybucha, gdy się na nią nastąpi, wytwarzając ogień na wszystkich sąsiednich polach. Dzieje się to jednak "
  "w kolejnej turze, zatem zdążysz bezpiecznie uciec, jak się będziesz szybko poruszać.")

N("Explosive Barrel", GEN_F, "Wybuchowa Beczka", "Wybuchowe Beczki", "Wybuchową Beczkę", "Wybuchową Beczką")
S("These barrels can be pushed, and they will explode if next to a fire, or under some attacks.",
  "Te beczki można pchać, i wybuchają, gdy znajdą się w pobliżu ognia, lub pod wpływem pewnych ataków.")

Orb("Intensity", "Intensywności")

S("When you have this, initial and maximal charge amounts of all Orbs are increased by 20%.",
  "Kiedy to masz, początkowe i maksymalne ilości ładunków dla wszyszkich Sfer rosną o 20%.")

// new mode: Racing

S("Racing available only in unbounded worlds.", "Wyścigi są dostępne jedynie w światach nieograniczonych")
S("select the track and start!", "wybierz tor i zaczynaj!")
S("configure the projection", "skonfiguruj projekcję")
S("racing projections", "projekcje wyścigów")
S("Poincaré disk model", "model dysku Poincaré")
S("third-person perspective", "perspektywa trzeciej osoby")
S("point of view", "punkt widzenia")
S("track", "tor")
S("race angle", "kąt wyścigu")
S("show more in front", "pokazuj więcej z przodu")
S("guiding line", "linia prowadząca")
S("track seed", "ziarno toru")
S("play the official seed", "graj na torach oficjalnych")
S("play a random seed", "graj na torach losowych")
S("best scores to show as ghosts", "najlepsze wyniki pokazywane jako duchy")
S("best scores to save", "najlepsze wyniki zapisywane")
S("disable the racing mode", "wyłącz tryb wyścigu")
S("racing menu", "menu wyścigów")
S("Race did not generate correctly for some reason -- not ranked",
  "Wyścig z jakiegoś powodu nie został wygenerowany prawidłowo -- niepunktowany")

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
    
  "Brązowe Wyspy zostały stworzone przez starożytne, podziemne istoty, które poruszały się losowo i wznosiły ziemię na swojej drodze... "
  "dorosłe istoty rozmnażały się po każdym ruchu, larwy również poruszały się losowo i wznosiły ziemię na swojej drodze, w końcu "
  "dorastając i wytwarzając swoje własne larwy. Istoty rozmnażały się wykładniczo, ale żyły w przestrzeni rosnącej jeszcze szybciej, "
  "także było dość miejsca dla całej rodziny...\n\n"
  "To było dawno temu. W obecnych czasach Brązowe Wyspy są zamieszkane przez dziwne potwory. Powodzenia w poszukiwaniu przygód!"
  "Częśći Brązowych Wysp mają różne poziomy wysokości, od 0 to 3. Nie jest możliwe wzniesienie się o 2 lub więcej poziomów lub utrata 3 poziomów "
  "w jednym ruchu (ale ataki są możliwe niezależnie od różnicy poziomów. Zabite Brązowe Bestie podnoszą poziom o 1."
  )

S(
    "Some readers misinterpreted the early maps of Free Fall, thinking that it is a land to the west from some wall. "
    "The name Western Hawks remained.",
    
    "Część czytelników źle zrozumiała wczesne mapy Swobodnego Spadku, myśląc, że jest to kraina na zachód od jakiejś ściany. "
    "Nazwa Zachodni Jastrząb pozostała.")

N("Torbernite", GEN_O, "Torbernit", "Torbernity", "Torbernit", "Torbernitem")

S("Crystals emitting magical radiation.", "Te kryształy emitują magiczne promieniowanie.")

// other things:

S("pure", "czyste") // non-bitruncated

S(" (level %1)", " (poziom %1)")

S("fixed facing", "ustalony kierunek patrzenia")
S("configure TPP automatically", "automatycznie skonfiguruj TPP")

S("Finished the race! Time: %1, place: %2 out of %3", "Dotar%łeś0 do mety! Czas: %1, miejsce: %2 z %3")
S("Finished the race in time %1!", "Dotar%łeś0 do mety w czasie %1!")

S("Welcome to HyperRogue! (cheat mode on)", "Witaj w HyperRogue! (tryb oszusta uruchomiony)")
S("Welcome to the Heptagonal Mode!", "Witaj w Trybie Siedmiokątów!")

S("adult Tortoise flags", "flagi dorosłego Żółwia")
S("baby Tortoise flags", "flagi Żółwika")

S("Map settings", "Ustawienia mapy")

S("disable wandering monsters","wyłącz błądzące potwory")
S("disable ghost timer", "wyłącz duchy czasowe")
S("simple pattern generation", "proste generowanie wzorów")
S("(e.g. pure Reptile pattern)", "np. czysty wzór w Jaszczurkach")
S("safety generation", "generacja bezpieczna")
S("(no treasure, no dangers)", "(bez skarbów i niebezpieczeństw)")
S("god mode", "tryb boga")

S("(unlock all, allow cheats, normal character display, cannot be turned off!)",
  "(odblokuj wszystko, oszustwa, normalny wygląd postaci, nie można wyłączyć!)")

S("change the pattern/color of new Canvas cells", "zmień wzór/kolor nowych pól Płótna")

S("racing mode", "tryb wyścigu")
S("Racing", "Wyścigi")

S("octahedron", "ośmiościan")

S("\"Did you know that the Cultists are relatives of the Desert Men?\"", 
  "\"Czy wiedzia%łeś0, że Kultyści są krewnymi Ludzi Pustyni?\"")                      

S("do not use special centering for racing", "wyścig bez specjalnego centrowania")

S("F8 = settings", "F8 = ustawienia")

// standarization of geometry names

S("{7,3} (standard HyperRogue map)", "{7,3} (domyślna mapa HyperRogue)")
S("{6,3} (Euclidean grid)", "{6,3} (Euklidesowa siatka sześciokątna)")
S("{5,3} (dodecahedron)", "{5,3} (dwunastościan)")
S("elliptic geometry in {5,3}", "płaszczyzna eliptyczna w {5,3}")
S("torus/Klein bottle/...", "torus/butelka Kleina/...")
S("{8,3} (like standard but with octagons)", "{8,3} (jak domyślna ale ośmiokąty)")
S("{5,4} (four pentagons)", "{5,4} (cztery pięciokąty)")
S("{6,4} (four hexagons)", "{6,4} (cztery sześciokąty)")
S("{7,4} (four heptagons)", "{7,4} (cztery siedmiokąty)")
S("{4,3} (cube)", "{4,3} (sześcian)")
S("{3,3} (tetrahedron)", "{3,3} (czworościan)")
S("{4,4} (Euclidean square grid)", "{4,4} (Euklidesowa siatka kwadratowa)")
S("elliptic geometry in {4,3}", "płaszczyzna eliptyczna w {4,3}")
S("{3,4} (octahedron)", "{3,4} (ośmiościan)")

// new 3D geometries
                     
S("variant of the binary tiling", "wariant parkietażu binarnego")
S("{4,3,4} cube tiling", "{4,3,4} sześciany")
S("rhombic dodecahedral honeycomb", "dwunastościany rombowe")
S("bitruncated cubic honeycomb", "ośmiościany ścięte")

S("{4,4} on horospheres", "{4,4} na horosferach")
S("{3,6} on horospheres", "{3,6} na horosferach")
S("{6,3} on horospheres", "{6,3} na horosferach")
S("rectangles on horospheres", "prostokąty na horosferach")

// quotient space abbreviations
S("minimal", "minimal")
S("Klein", "Klein")
S("Bolza", "Bolza")
S("Bolza2", "Bolza2")
S("Macbeath", "Macbeath")
S("Bring", "Bring")
S("M3", "M3")
S("M4", "M4")
S("Crystal", "Kryształ")

#define Cell(x) \
  S(x "-cell", x "-komórka") \
  S(x "-cell (elliptic space)", x "-komórka (przestrzeń eliptyczna)")

Cell("{3,3,3} 5") Cell("{4,3,3} 8") Cell("{3,3,4} 16") Cell("{3,4,3} 24") Cell("{5,3,3} 120") Cell("{3,3,5} 600")
#undef Cell

#define Honeycomb(x) \
  S(x " hyperbolic honeycomb", x " siatka hiperboliczna") \
  S(x " field quotient space", x " przestrzeń ilorazowa ciała")

Honeycomb("{5,3,4}") Honeycomb("{4,3,5}") Honeycomb("{3,3,6}") Honeycomb("{3,4,4}") Honeycomb("{5,3,5}") Honeycomb("{5,3,6}") Honeycomb("{4,3,6}") Honeycomb("{5,3,6}")

#undef Honeycomb

// new or previously untranslated options
S("dimension", "wymiar")
S("pattern", "wzór")
S("Euler characteristics", "charakterystyka Eulera")
S("demigenus", "genus nieorientowalny")
S("number of mines", "liczba min") // for Minefield in bounded spaces
S("advanced parameters", "parametry dodatkowe")

// models & projections

S("disk/Gans", "dysk/model Gansa")
S("spiral/ring", "spirala/pierścień")
S("native perspective", "wewnętrzna perspektywa")
S("azimuthal equi-volume", "azymutalny, równe objętości")
S("ball model/Gans", "model kuli/Gansa")
S("perspective in 4D", "perspektywa w 4D")
S("half-space", "półprzestrzeń")

// projection configuration

S("projection distance", "ogległość rzutu")
S("rotation (x/z plane)", "obrót w płaszczyźnie x/z")
S("model orientation (y/z plane)", "orientacja modelu (y/z)")

S("near clipping plane", "przycinanie bliskie")
S("far clipping plane", "przycinanie dalekie")
S(
  "Your view of the 3D model is naturally bounded from four directions by your window. "
  "Here, you can also set up similar bounds in the Z direction. Radius of the ball/band "
  "models, and the distance from the center to the plane in the halfspace model, are 1.\n\n",
  
  "Model 3D widoczny na ekranie jest naturalnie ograniczony z 4 strony przez wielkość okna. "
  "Tu można również ustawić podobne ograniczenie w kierunku Z. Promień modelu kuli/wstęgi, "
  "oraz odległość od środka do płaszczyzny w modelu półprzestrzeni, są równe 1.\n\n")

S("Objects with Z coordinate "
  "bigger than this parameter are not shown. This is useful with the models which "
  "extend infinitely in the Z direction, or if you want things close to your character "
  "to be not obscured by things closer to the camera.",
  
  "Obiekty ze współrzędną Z powyżej tego parametru nie są widoczne. Jest to użyteczne "
  "w modelach rozszerzających się w nieskończoność w kierunku dodatnich Z, lub gdy "
  "chcesz, by obiekty obok Twojej postaci nie były zasłaniane przez obiekty bliżej "
  "kamery.")

S("Objects with Z coordinate "
  "smaller than this parameter are not shown; it also affects the fog effect"
  " (near clipping plane = 0% fog, far clipping plane = 100% fog).",
  
  "Obiekty ze współrzędną Z poniżej tego parametru nie są widoczne; wpływa on "
  "również na efekt mgły (przycinanie bliskie = 0% mgły, przycinanie dalekie = 100% mgły).")

S("set to 90° for the ring projection", "ustaw 90° by uzyskać rzut pierścieniowy")

S("spiral multiplier", "mnożnik spirali")
S(
  "This parameter has a bit different scale depending on the settings:\n"
  "(1) in spherical geometry (with spiral angle=90°, 1 produces a stereographic projection)\n"
  "(2) in hyperbolic geometry, with spiral angle being +90° or -90°\n"
  "(3) in hyperbolic geometry, with other spiral angles (1 makes the bands fit exactly)",
  
  "Ten parametr ma różną skalę w zależności od ustawień:\n"
  "(1) w geometrii sferycznej (przy kącie=90°, 1 to rzut stereograficzny)\n"
  "(2) w geometrii hiperbolicznej, przy kącie równym +90° lub -90°\n"
  "(3) w geometrii hiperbolicznej, przy innym kącie (1 to dokładnie dopasowana wstęga)\n"
  )

// line animation

S("animate from start to current player position", "animacja od startu do obecnej pozycji gracza")
S("animate from last recenter to current view", "animacja od ostatniej pozycji centrowania do obecnego punktu widzenia")

// 3D settings for 3D mode

S("3D sight range", "zasięg wzroku w 3D")
S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "Zasięg wzroku w geometriach 3D jest określany w jednostkach absolutnych. Od tego parametru zależy również efekt mgły.\n\n"
  "W geometrii sferycznej zasięg wzroku 2π powoduje, że rzeczy za Tobą wyglądają jakby były przed Tobą, a zasięg π (i powyżej "
  "pozwala widzieć rzeczy w punkcie antypodycznym tak, jakby były blisko Ciebie.\n\n"
  "W geometriach hiperbolicznych liczba renderowanych komórek zależy wykładniczo od zasięgu wzroku. Im więcej komórek jest "
  "renderowane, tym gorsza wydajność.\n\n"
  "Rozgrywka zależy od zasięgu wzroku: przeciwnicy działają wtedy, gdy są widoczni. Częstość generowania przeciwników jest automatycznie dostosowywana.")

S("limit generation", "ograniczenie generacji")
S("Cells over this distance will not be generated, but they will be drawn if they are already generated and in the sight range.",
  "Komórki dalsze niż w tej odległości nie będą automatycznie generowane, ale będą renderowane, jeżeli zostały wygenerowane wcześniej.")

S("inverted Poincaré model", "model Poincarégo w inwersji")
S("mouse aiming sensitivity", "czułość celowania myszą")
S("set to 0 to disable", "ustaw 0 by wyłączyć")
S("radar size", "wielkość radaru")
S("Height to width", "wysokość do szerokości")
S("Creature scale", "skala stworzeń")

// not only in 3D mode
S("desaturate colors", "desaturowane kolory")
S("Make the game colors less saturated. This is useful in the anaglyph mode.",
  "Powoduje, że kolory są mniej nasycone. Jest to użyteczne w trybie anaglifów.")

S("brightness behind the sphere", "jasność za sferą")
S("In the orthogonal projection, objects on the other side of the sphere are drawn darker.",
  "W rzucie ortogonalnym obiekty po drugiej stronie sfery są rysowane ciemniejsze.")

// restructured menus

S("colors & aura", "kolory i aura")
S("colors of some game objects can be edited by clicking them.", "można zmieniać kolory niektórych obiektów gry, klikając je.")
S("standard grid color", "standardowy kolor siatki")
S("mouse & touchscreen", "mysz i ekran dotykowy")
S("settings", "ustawienia")
S("interface", "interfejs")
S("general graphics", "grafika ogólna")
S("quick options", "szybkie opcje")
S("models & projections", "modele i projekcje")
S("keyboard & joysticks", "klawiatura i kontrolery")
S("other settings", "inne ustawienia")
S("special modes", "tryby specjalne")
S("creative mode", "tryb kreatywny")
S("multiplayer", "gra wieloosobowa")

// extra / previously untranslated options

S("smooth scrolling", "gładkie przewijanie")
S("change slideshow", "zmień pokaz slajdów")
S("move by clicking on compass", "ruch przez klikanie kompasu")
S("disable the HUD", "wyłącz elementy interfejsu (HUD)")
S("hide the player", "ukryj gracza")

// redefinable key in 3D shmup mode
S("scroll forward", "przewijanie w przód")
S("scroll backward", "przewijanie w tył")

S("note: configured input is designed for", "uwaga: sterowanie skonfigurowane jest zaprojektowane")
S("multiplayer and shmup mode; some features", "dla trybu wieloosobowwego i strzelanki; część")
S("work worse if you use it.", "funkcjonalności działa gorzej w tym trybie.")

// quick options in 3D
S("first person perspective", "perspektywa pierwszej osoby")
S("fixed point of view", "ustalony punkt widzenia")
S("third person perspective", "perspektywa trzeciej osoby")

S("Hint: these keys usually work during the game", "Wskazówka: te klawisze działają również podczas gry")

// 3D graphics editor

S("l = color group: %1", "l = grupa koloru: %1")
S("z = z-level", "z = poziom Z")
S("Changing the z-level will make this layer affected by the parallax effect.", "zmiana poziomu Z powoduje efekt paralaksy dla tej warstwy.")
S("The distance from the camera to added points.", "Punkty są dodawane w tej odległości od kamery.")

S("This land has been disabled with compilation flags.", "Ta kraina została wyłączona opcjami kompilacji.")
S("This land does not make much sense in 3D.", "Ta kraina nie ma sensu w 3D.")
S("This land does not make much sense in binary tiling.", "Ta kraina nie ma sensu w parkietażach binarnych.")
S("Special pattern implemented for this geometry.", "Specjalny wzór zaimplementowany w tej geometrii.")
S("This land works only in the shmup mode.", "Ta kraina działa jedynie w trybie strzelanki.")

// new land

N("Space Rocks", GEN_F, "Kosmiczne Skały", "Kosmiczne Skały", "Kosmicznymi Skałami", "wśród Kosmicznych Skał")
N("Space Rock", GEN_F, "Kosmiczna Skała", "Kosmiczne Skały", "Kosmiczną Skałę", "Kosmiczną Skałą")
N("Fuel", GEN_F, "Paliwo", "Paliwa", "Paliwo", "Paliwo")
S("Shoot the Space Rocks for score. Large Rocks will split into two smaller rocks.",
  "Strzelaj w Kosmiczne Skały by zdobywać punkty. Duże Skały rozpadają się na dwie mniejsze.")

N("editable statue", GEN_O, "edytowalny pomnik", "edytowalne pomniki", "edytowalny pomnik", "edytowalnym pomnikiem")
S("These statues are designed to have their graphics edited in the Vector Graphics Editor. Each number has its own, separate graphics.",
  "Można zmienić wygląd tych pomników w Edytorze Grafiki. Każdy indeks ma swoją własną, osobną grafikę.")
S("The Orb of Yendor is locked in with powerful magic.", "Potężna magia chroni Sferę Yendoru przed przemieszczaniem.")

S("general perspective", "perspektywa ogólna")
S("formula", "wzór")

S("limit generated cells per frame", "ograniczenie komórek generowanych na klatkę")

S("In the 3D mode, lowering this value may help if the game lags while exploring new areas.",
  "W trybie 3D zmniejszenie tej wartości może pomóc, gdy gra zwalnia podczas zwiedzania nowych obszarów.")
  
S("sloppy range checking", "niedbałe sprawdzanie odległości")

S("fog effect", "effect mgły")
S("memory configuration", "konfiguracja pamięci")

S("wall quality", "jakość ścian")

S(
  "Controls the number of triangles used for wall surfaces. "
  "Higher numbers reduce the performance. "
  "This has a strong effect when the walls are curved indeed "
  "(floors in 2D geometries, honeycombs based on horospheres, and projections other than native perspective), "
  "but otherwise, usually it can be set to 1 without significant adverse effects other "
  "than slightly incorrect texturing.",
  
  "Steruje liczbą trójkątów użytych w teksturach powierzchni ścian. "
  "Wyższa liczba obniża wydajność. "
  "Ma istotny wpływ na wynik w przypadku gdy ściany są krzywoliniowe "
  "(teselacje oparte na horosferach, i projekcje inne niż perspektywa wewnętrzna), "
  "w pozostałych sytuacjach zwykle można ustawić 1 bez istotnego pogorszenia jakości, "
  "oprócz lekko niepoprawnego teksturowania.")

S("use the full 3D models", "użyj pełnych modeli 3D")

S("Z shift", "przesunięcie Z")
S("fixed Y/Z rotation", "ustalona rotacja Y/Z")

S("configure FPP automatically", "automatycznie skonfiguruj FPP")

S("Look from behind.", "Patrz od tyłu.")

S("reduce if walls on the way", "zmniejsz, gdy po drodze jest ściana")

S("The camera is currently exactly on the plane P. "
  "The horizon is seen as a straight line.",
  
  "Kamera jest obecnie dokładnie na płaszczyźnie P. Horyzont jest widziany jako linia prosta.")

S(
  "The camera is currently %1 units above the plane P. "
  "This makes you see the floor level as in general perspective projection "
  "with parameter %2.", 
  
  "Kamera jest obecnie %1 jednostek ponad płaszczyzną P. "
  "Oznacza to, że poziom podłogi jest widziany jak w perspektywie ogólnej z parametrem %2.")

S("In the FPP mode, the camera will be set at this altitude (before applying shifts).", 
  "W trybie perspektywy trzeciej osoby (FPP) kamera będize ustawiona na tej wysokości "
  "(przed uwzględnieniem przesunięć).")

S("auto-adjust to eyes on the player model", "automatyczne uzgadnianie z modelem postaci gracza")

S("eye color", "kolor oczu")

// rotation (line animation etc.)

S("XY plane", "płaszczyzna XY")
S("XZ plane", "płaszczyzna XZ")
S("XY plane #2", "płaszczyzna XY #2")

// cube honeycomb

S("twisting implemented only for diagonal matrices", "skręcanie zaimplementowane tylko dla macierzy diagonalnych")
S("nothing to twist", "nic do skręcania")
S("change the bottom left corner", "ustaw dolny lewy róg")

S("flip X coordinate", "odbij współrzędną X")
S("flipping X impossible", "odbicie X niemożliwe")
S("flip Y coordinate", "odbij współrzędną Y")
S("flipping Y impossible", "odbicie Y niemożliwe")
S("flip X coordinate", "odbij współrzędną X")
S("swap X and Y", "zamień X i Y")
S("swapping impossible", "zamiana niemożliwa")

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

  "Ta macierz pozwala grać na przestrzeniach ilorazowych trójwymiarowej "
  "przestrzeni Euklidesowej. Każda kolumna określa wektor przesunięcia, "
  "po którego przejściu wracamy do punktu startowego. Przykładowo, "
  "gdy ustawimy 2, 6, 0 na przekątnej, wracamy do punktu startowego "
  "po przejściu 2 kroków w kierunku X, 6 kroków w kierunku Y (w kierunku "
  "Z przestrzeń jest nieskończona).\n\n"
  "Można także wprowadzić skręcenia: po przejściu danej liczby kroków "
  "w kierunku Z, przestrzeń jest również odbijana lub obracana. "
  "(Działa to dla macierzy diagonwalnych -- bardziej ogólne skręcenia "
  "obecnie nie są zaimplementowane.)")

// map editor 

S("pick something", "wybierz coś") // to edit

S("z = camera", "z = kamera")
S("Z = spheres", "z = sfery")
S("z = equi", "z = ekwi")

S("The distance from the camera to added points.", "Odległość dodawanych punktów od kamery")
S("place points at fixed radius", "punkty stawiaj na ustalonym promieniu")
S("place points on equidistant surfaces", "punkty stawiaj na powierzchniach ekwidystantnych")
S("mousewheel step", "krok kółka myszy")
S("hint: shift for finer steps", "wskazówka: shift by uzyskać mniejsze kroki")

S("random walk", "błądzenie losowe")

// dual like two at once, not in mathematical sense
S("dual geometry mode", "tryb podwójnej geometrii")

S("Impossible.", "Niemożliwe.")

S("subgame affected", "dotyczy podgry")
S("both", "obie")

// changing the rug model distance
S("adjust points", "popraw punkty")
S("center on camera", "centruj na kamerze")
S("adjust edges", "popraw krawędzie")
S("adjust distance", "popraw odległości")

S("memory", "pamięć")

S(
  "HyperRogue takes place in a world that is larger than anything Euclidean. "
  "Unfortunately, in some cases running it on an Euclidean computer might be "
  "a problem -- the computer could simply run out of memory. Some lands (such as the Ocean or the Brown Islands) "
  "may use up memory very fast!\n\n",
  
  "Akcja gry dzieje się w świecie większym niż cokolwiek Euklidesowego. "
  "Niestetym w pewnych sytuacjach może to powodować problemy, gdy gra jest "
  "uruchamiana na komputerze Euklidesowym -- któremu może skończyć się pamięć. "
  "Niektóre krainy (takie jak Ocean albo Brązowa Wyspa) zjadają pamięć bardzo szybko!\n\n")
  
S("You are playing a 32-bit HyperRogue executable, which can only use 4GB of memory.\n\n",
  "Używasz 32-bitowej wersji gry, zatem ma ona dostęp tylko 4 GB pamięci.")

S(
  "Although you are extremely unlikely to return to a place you have already been to, "
  "the game never forgets these areas, unless you start a new game, use an Orb of "
  "Safety (found in Land of Eternal Motion, the Prairie, and the Ocean), or activate the memory "
  "saving mode, which tries to intelligently predict which cells you will never find "
  "again and can be safely forgotten.\n\n",
  
  "Mimo że praktycznie niemożliwe jest wrócenie do miejsca, w którym się już było, "
  "gra nigdy nie zapomina tych obszarów, chyba że zaczniesz nową grę, użyjesz Sfery "
  "Bezpieczeństwa (Kraina Wiecznego Ruchu / Preria / Ocean), lub uruchomisz "
  "tryb oszczędzania pamięci, który inteligentnie przewiduje czego nigdy nie odnajdziesz "
  "i w związku z czym może być bezpiecznie zapomniane.")

S("cells in memory", "pól w pamięci")

S("memory saving mode", "tryb osczędzania pamięci")
S("show memory warnings", "pokazuj ostrzeżenia pamięci")

S("just let me find Orb of Safety or finish the game", "pozwól mi znaleźć Sferę Bezpieczeństwa lub skończyć grę")
S("memory reserve", "rezerwa pamięci")
S("When to show a memory warning.", "Kiedy pokazywać ostrzeżenia pamięci")
S("clear caches", "wyczyść pamięć podręczną")

// in animation menu
S("shmup action", "akcja trybu strzelanki")

// 11.2

// extra projections
S("central inversion", "inwersja względem środka")
S("two-point azimuthal", "rzut dwupunktowy azymutalny")
S("two-point hybrid", "rzut dwupunktowy mieszany")
S("Mollweide", "odwzorowanie Mollweidego")
S("central cylindrical", "rzut walcowy centralny")
S("Collignon", "odwzorowanie Collignona")

// extra settings

S("draw crosshair", "rysuj celownik")

S("Display a targetting reticle in the center of the screen. Might be useful when exploring 3D modes, "
  "as it precisely shows the direction we are going. However, the option is available in all modes.",
  
  "Pokazuj celownik na środku ekranu. Może być użyteczna przy eksploracji przestrzeni trójwymiarowych "
  "(celownik dokładnie pokazuje kierunek, w którym idziemy). Opcja ta jest jednak dostępna we "
  "wszystkich trybach.")

S("radar range", "zasięg radaru")

S("boundary width multiplier", "mnożnik szerokości brzegu")
S("grid width multiplier", "mnożnik szerokości siatki")
S("highlight the cell forward", "zaznaczaj pole naprzód")
S("edit widths individually", "edytuj szerokości niezależnie")

// extra tilings

S("kite-and-dart", "kite-and-dart")
S("kite-and-dart on horospheres", "kite-and-dart na horosferach")
S("standard binary tiling", "standardowy parkietaż binarny")

S("show quotient spaces", "przestrzenie ilorazowe")
S("three-dimensional", "trójwymiarowe")

// non-isotropic geometries:

S("Solv geometry", "geometria Solv")
S("Sol", "Sol")

S(
  "Solv (aka Sol) is a 3D space where directions work in different ways. It is described by the following metric:\n"
  "ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  "You are currently displaying Solv in the perspective projection based on native geodesics. You can control how "
  "the fog effects depends on the geodesic distance, and how far object in X/Y/Z coordinates are rendered.",
  
  "Solv (albo Sol) to przestrzeń trójwymiarowa, w której kierunki działają w różny sposób. Jest ona opisana "
  "następującą metryką: ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  
  "Obecnie Solv jest przedstawiona w rzucie perspektywistycznym, opartym na geodezyjnych. Możesz określić, "
  "jak efekt mgły wpyłwa na odległość po geodezyjnej, i w jakiej odległości X/Y/Z obiekty są renderowane.")

S("max difference in X/Y coordinates", "maksymalna różnica współrzędnch X/Y")
S("max difference in Z coordinate", "maksymalna różnica współrzędnej Z")

S("geodesic movement in Sol/Nil", "ruch po geodezyjnych w Sol/Nil")

// ---

S("Nil geometry", "geometria Nil")
S("Nil", "Nil")

// ---

S("product space", "przestrzeń produktowa")
S("current geometry x E", "obecna geometria x E")
S("Only works with 2D non-Euclidean geometries", "Działa jedynie w geometriach nieeuklidesowych 2D")
S("%1 x E", "%1 x E")
S("Z-level height factor", "mnożnik wysokości poziomu Z")

S("precision of S2xE rings", "precyzja pierścieni w S2xE")

S(
  "In S2xE, objects at spherical distances which are multiples of π will look like "
  "rings, and objects close to these will look like crescents. "
  "This setting controls the quality of rendering these rings and crescents.",
  
  "W S2xE, obiekty w odległościach po sferze będących wielokrotnościami π "
  "wyglądają jak pierścienie, a te blisko wyglądają jak półksiężyce. "
  "To ustawienie steruje jakością renderowania tych kształtów.")

// ---

S("rotation space", "przestrzeń obrotów")
S("steps", "kroki")

// 11.3
//======

// new geometries

S("space of rotations in current geometry", "przestrzeń obrotów obecnej geometrii")
S("~SL(2,R)~", "~SL(2,R)~")
S("Only works with (semi-)regular tilings", "Działa jedynie w parkietażach (pół)regularnych")
S("That would have %1/%2 levels", "To by miało %1/%2 poziomów")
S("number of levels", "liczba poziomów")
S("Hint: this is more playable with pure {7,3} or pure {5,4}", "Wskazówka: lepiej grać w czystym {7,3} lub czystym {5,4}")

S("view the underlying geometry", "pokaż geometrię bazową")

S("The space you are currently in is the space of rotations of the underlying hyperbolic or spherical geometry. ",
  "Jesteś obecnie w przestrzeni rotacji bazowej geometrii hiperbolicznej lub sferycznej. ")

S("This option lets you see the underlying space. Lands and some walls (e.g. in the Graveyard) are based on "
  "the respective features in the underlying world, but details such as monsters or items are ignored.",
  
  "Ta opcja pozwala obejrzeć przestrzeń bazową. Krainy i pewne ściany (np. na Cmentarzu) bazują na "
  "odpowiednich cechach świata bazowego, ale szczegóły takie jak potwory i przedmioty są ignorowane.")

// map editor in 3D nonisotropic

S("place points on surfaces of const Z", "umieszczaj punkty na powierzchniach o stałym Z")
S("place points on surfaces of const X", "umieszczaj punkty na powierzchniach o stałym X")
S("place points on surfaces of const Y", "umieszczaj punkty na powierzchniach o stałym Y")

S("parallels to draw", "ile równoleżników")
S("meridians to draw", "ile południków")
S("range of grid to draw", "zasięg siatki")

S("current filter", "obecny filtr")
S("Solv quotient spaces", "przestrzenie ilorazowe Solv")
S("%1 period", "okres %1")
S("Note: the value 0 functions effectively as the size of int (2^32).", "Uwaga: warto�� 0 dzia�a w rzeczywisto�ci jak rozmiar inta (2^32).")

S("standard ternary tiling", "standardowy parkietaż ternarny")
S("stretched hyperbolic", "rozciągnięta hiperboliczna")
S("stretched Solv", "rozciągnięta geometria Solv")

S("{3,∞} (infinite triangles)", "{3,∞} (nieskończone trójkąty)")
S("{4,∞} (infinite triangles)", "{4,∞} (nieskończone kwadraty)")
S("{3/4,∞} (infinite triangles and squares)", "nieskończone trójkąty i kwadraty")

S("4D crystal in H3", "4D kryształ w H3")
S("6D crystal in H3", "6D kryształ w H3")

S("Arnold's cat mapping torus", "torus przekształcenia kota Arnolda")
S("{5,3,5} quotient", "przestrzeń ilorazowa w {5,3,5}")
S("Seifert-Weber space", "przestrzeń Seiferta-Webera")
S("Poincaré homology sphere", "sfera homologii Poincaré")

S("load from file", "ładuj z pliku")

S("Crystal torus", "Kryształowy torus")
S("Z_k^d instead of Z^d. Only works with k even.", "Z_k^d zamiast Z^d. Działa tylko z parzystym k.")

S("Euclidean quotient spaces", "Euklidesowe przestrzenie ilorazowe")
S("columns specify periods", "kolumny określają okresy")
S("(vectors you need to take to get back to start)", "(wektory, które trzeba przejść, by wrócić do startu)")
S("third-turn space", "przestrzeń obrotu o 1/3")
S("make it even", "liczby muszą być parzyste")
S("or for columns : (A,B,C), (B,C,A), (D,D,D) where A+B+C=0", "lub dla kolumn : (A,B,C), (B,C,A), (D,D,D) gdzie A+B+C=0")
S("quarter-turn space", "przestrzeń ćwierćobrotu")

S("change the second column for Möbius bands and Klein bottles", "zmień drugą kolumnę, by uzyskać wstęgi Möbiusa i butelki Kleina")
S("second period is chiral -- cannot be mirrored", "drugi okres jest chiralny -- nie można odbić")
S("periods must be orthogonal for mirroring", "by odbić, okresy muszą być prostopadłe")
S("mirror flip in the second period", "odbicie drugiego okresu")
S("single-cell torus", "torus jednego pola")
S("Klein bottle", "butelka Kleina")
S("large regular torus", "duży torus regularny")
S("cylinder", "walec")
S("Möbius band", "wstęga Möbiusa")
S("seven-colorable torus", "siedmiokolorowalny torus")
S("HyperRogue classical torus", "klasyczny torus HyperRogue")
S("no quotient", "bez dzielenia")
S("standard rotation", "obrót standardowy")

S("field quotient", "przestrzeń ilorazowa ciała")
S("order %1%2 (%3 %4s)", "rząd %1%2 (%3 %4ów)")
S("order %1%2 (%3 %4-gons)", "rząd %1%2 (%3 %4-kątów)")

S("non-isotropic", "geometrie nieizotropowe")
S("regular 2D tesselations", "regularne parkietaże 2D")
S("regular 3D honeycombs", "regularne parkietaże 3D")
S("interesting quotient spaces", "interesujące przestrzenie ilorazowe")
S("geometries", "geometrie")
S("geometry", "geometria")

S("start discovery", "zacznij poszukiwania")
S("resume discovery", "przywróc poszukiwania")
S("suspend discovery", "wstrzymaj poszukiwania")

S("#%1, cells: %2", "#%1, komórek: %2")

S("hyperbolic (3:2)", "hiperboliczna (3:2)")
S("Sol (3:2)", "Sol (3:2)")

S("quotient spaces in ", "przestrzenie ilorazowe w: ")
S("No quotient spaces available in the current tiling.",
  "Brak dostępnych przestrzeni ilorazowych zo becnym parkietażem.")

S("You are currently in a product space.", "Jesteś w przestrzeni produktowej.")

S("Nil quotient spaces", "Przestrzenie ilorazowe Nil")
S("honeycomb", "siatka")
S("Y period must be divisible by Z period", "okres Y musi być podzielny przez okres Z")
S("quotient product spaces", "ilorazowe przestrzenie produktowe")
S("Set to 0 to make it non-periodic.", "Ustaw 0, by było nieokresowe.")
S("Works if the underlying space is symmetric.", "Działa dla symetrycznej przestrzeni bazowej.")
S("reflect", "odbicie")
S("the current rotation is invalid", "obecny obrót jest nieprawidłowy")

S("Note: adjacency rule affects environmental effects, but not movement.",
  "Uwaga: reguła sąsiedztwa definiuje efekty terenowe, ale nie ruchy.")

S("\n\nOption -mineadj 1 can be added for Moore neighborhoods.", "\n\nOpcja -mineadj 1 może zostać dodana, by uzyskać sąsiedztwo Moore'a.")

// WFC generation

S("configure WFC", "konfiguracja WFC")

S("Wave Function Collapse", "Wave Function Collapse")
S("import the current map", "zaimportuj obecną mapę")
S("generate a map using WFC", "wygeneruj mapę używając WFC")

// racing in Thurston geometries

S("Racing", "Wyścigi")
S("how fast can you reach the end?", "jak szybko dobiegniesz do końca?")
S("Racing in Thurston geometries", "Wyścigi w geometriach Thurstona")
S("race through a maze in exotic 3D geometry!", "Wyścig przez egzotyczną trójwymiarową geometrię!")

S("periodic Solv", "okresowa Solv")
S("hyperbolic crystal", "hiperboliczny kryształ")
S("hyperbolic regular", "hiperboliczna siatka regularna")
S("S2xE regular", "regularna w S2xE")
S("H2xE regular", "regularna w H2xE")
S("periodic Nil", "okresowa w Nil")
S("alternate versions", "wersje alternatywne")

// extra option

S("floors in 2D geometries", "podłogi w geometriach 2D")

S("slides", "slajdy")

// raycasting

S("configure raycasting", "skonfiguruj raycasting")
S("raycasting configuration", "konfiguracja raycastingu")
S("available in current geometry", "dostępne w obecnej geometrii")
S("use raycasting?", "czy używać raycastingu?")
S("comparison mode", "tryb porównania")
S("exponential range", "zasięg wykładniczy")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))", "wzór na jasność: max(1-d/sightrange, s*exp(-d/r))")
S("exponential start", "start wykładniczy")
S("hard limit", "twarde ograniczenie")
S("no limit", "bez ograniczeń")
S("reflective walls", "odbijające ściany")
S("max step", "maksymalny krok")
S("affects the precision of solving the geodesic equation in Solv", "określa dokładność rozwiązywania równania geodezyjnych w Solv")
S("min step", "minimalny krok")
S("how precisely should we find out when do cross the cell boundary", "jak dokładnie sprawdzamy moment przecięcia granicy pola")
S("iterations", "iteracje")
S("in H3/H2xE/E3 this is the number of cell boundaries; in nonisotropic, the number of simulation steps",
  "w H3/H2xE/E3 liczba przejść przez granicę komórki; w nieizotropowych geometriach liczba kroków symulacji")
S("max cells", "maksymalna liczba komórek")
S("generate", "generuj spoza zasięgu")
S("out-of-range color", "kolor poza zasięgiem")

// new graphical styles

S("standard graphics", "standardowa grafika")
S("neon mode", "tryb neonu")
S("no boundary mode", "tryb bez obramowań")
S("neon mode II", "tryb neonu II")
S("illustration mode", "tryb ilustracji")
S("hint: Press Alt while testing modes", "wskazówka: wciśnij Alt przy testowaniu trybów")
S("disable shadows", "wyłącz cienie")
S("bright mode", "tryb jasny")
S("colorblind simulation", "symulacja daltonizmu")
S("ASCII/3D", "ASCII/3D")

// new configuration options

S("particles on attack", "cząsteczki przy ataku")
S("floating bubbles: special", "bąbelki: specjalne")
S("floating bubbles: treasure  thresholds", "bąbelki: progi skarbów")
S("floating bubbles: all treasures", "bąbelki: wszystkie skarby")

S("variable width", "zmienna szerokość")
S("lines longer than this value will be split into shorter lines, with width computed separately for each of them.",
  "linie dłuższe niż ta wartość będą dzielone na krótsze, których szerokość jest obliczana osobno.")

S("smoothen", "wygładzanie")
S("large values help if the joystick is imprecise", "duże wartości pomagają, gdy joystick jest nieprecyzyjny")

S("level lines", "poziomice")
S("This feature superimposes level lines on the rendered screen. These lines depend on the Z coordinate. In 3D hyperbolic the Z coordinate is taken from the Klein model. "
  "Level lines can be used to observe the curvature: circles correspond to positive curvature, while hyperbolas correspond to negative. See e.g. the Hypersian Rug mode.",
  
  "Ta funkcjonalność powoduje, że na wytworzonym obrazie są rysowane poziomice, zależne od współrzędnej Z. W przypadku trójwymiarowej przestrzeni hiperbolicznej, współrzędna "
  "Z jest liczona według modelu Kleina. Poziomic można użyć do obserwacji krzywizny: koła to krzywizna dodatnia, hiperbole to krzywizna ujemna. Widać to np. w trybie "
  "hiperskiego dywanu.")

S("disable textures", "wyłącz tekstury")
S("disable level lines", "wyłącz poziomice")

S("Size of the fish eye.", "Rozmiar oka ryby.")
S("product Z stretch", "rozciągnięcie współrzędnej Z")
S("horocyclic coordinates", "współrzędne horocykliczne")

// dual geometry puzzles

S("You won!", "Wygra%łeś!")
S("dual geometry puzzles", "zagadki w dwóch geometriach")
S("move both characters to marked squares at once!", "ustaw obie postaci w zaznaczonych kwadratach jednocześnie!")
S("enter seed", "wpisz ziarno")
S("Euclidean size", "rozmiar Euklidesowy")
S("hyperbolic size", "rozmiar hiperboliczny")

// another new mode

S("play this Strange Challenge unofficially", "gram nieoficjalnie")

// new gameplay lines

S("\n\nIn the Orb Strategy Mode, the effect is increased to +100%.",
  "\n\nW Trybie Strategii Sfer efekt jest powiększony do +100%.")

S("\n\nAdditionally, your allies are protected from your indirect attacks.", 
  "\n\nDodatkowo, Twoi przyjaciele są chronieni przed Twoimi pośrednimi atakami.")

S("\n\nLeaves cut directly: %1", "\n\nLiści ściętych bezpośrednio: %1")
S("\n\nLeaves cut onscreen: %1", "\n\nLiści ściętych na ekranie: %1")
S("\n\nLeaves cut offscreen (approximately): %1", "\n\nLiści ściętych poza ekranem (mniej więcej): %1")
S("leaves cut: %1", "ściętych liści: %1")

// explanations for the start screen

S("(random option)", "(opcja losowa)")
S("(seasonal option)", "(opcja sezonowa)")

S("spinning in the band model", "obrót w modelu wstęgi")
S("spinning around", "obrót dookoała")
S("row of ghosts", "rząd duchów")
S("army of ghosts", "armia duchów")
S("ghost spiral", "spirala duchów")
S("Fibonacci ghosts", "duchy Fibonacciego")

// new welcome messages

S("Welcome to Spherindrogue!", "Witaj w Sferyndrogue!")
S("Welcome to Hyper-X-R-Rogue!", "Witaj w Hyper-X-R-Rogue!")
S("Good luck in the elliptic space!", "Powodzenia w przestrzeni eliptycznej!")
S("Welcome to SolvRogue!", "Witaj w SolvRogue!")
S("Welcome to NilRogue!", "Witaj w NilRogue!")
S("Welcome to PSL(2,R)-ogue!", "Witaj w PSL(2,R)-ogue!")

// new patterns and line patterns

S("empty", "pusty")
S("rainbow by distance", "tęcza w zal. od odległości")
S("Cage", "Klatka")
S("Hyperplanes", "Hiperpłaszczyzny")
S("Honeycomb", "Siatka")
S("Diagonal", "Przekątna")
S("Penrose staircase", "schody Penrose'a")
S("nearer end", "bliższy koniec")
S("furthest from start", "najdalszy od startu")
S("types", "typy")
S("display the inner walls", "pokazuj ściany wewnętrzne")
S("floor type", "typ podłogi")
S("(no wall)", "(bez ściany)")
S("Goldberg tree", "drzewo Goldberga")
S("irregular master", "pola główne")
S("sub-lines", "pod-linie")
S("fat edges", "grube krawędzie")
S("You chop down the shrub.", "Ścinasz krzak.")

// new lands

N("Wetland", GEN_N, "Mokradła", "Mokradła", "Mokradła", "na Mokradłach")

S("Some people have definitely drowned in this treacherous area. Better be careful!",
  "Na pewno ktoś utonął w tym zdradliwym miejscu. Lepiej uważać!")

N("Water Lily", GEN_N, "Lilia Wodna", "Lilie Wodne", "Lilię Wodną", "Lilią Wodną")
S("A beautiful plant from the Wetland.", "Piękny kwiat z Mokradeł.")

N("shallow water", GEN_F, "płytka woda", "płytkie wody", "płytką wodę", "płytką wodą")
S("Shallow water is passable both for land creatures and for water creatures.",
  "Przez płytką wodę mogą przejść i stworzenia lądowe, i wodne.")

N("deep water", GEN_F, "głęboka woda", "głębokie wody", "głęboką wodę", "głęboką wodą")

S("This body of water is calm and beautiful, but too deep to pass.",
  "Ta woda jest spokojna i piękna, ale zbyt głęboka, by przez nią przejść.")

S("You remember anglers from your hometown showing the impressive pikes they have caught. This one is much larger.",
  "Pamiętasz, jak w Twoim rodzinnym mieście wędkarze chwalili się wielkością złowionych szczupaków. Ten szczupak "
  "jest dużo większy.")

S("World Turtle", "Żółw Świata")
S("Have you reached the edge of the Poincaré disk? No, it must just be the edge of this world. "
  "Which happens to be floating on the backs of some turtles. Wondering how such not very big turtles could "
  "support such a large world? Well, there are LOTS of them! "
  "This one got so annoyed by the fact that you discovered this that it left its post. However, the world still "
  "stands, because there are so many turtles.",
  
  "Czy osiągn%ąłeś brzeg dysku Poincaré? Nie, to tylko koniec tego świata. "
  "Jak się okazuje, ten świat jest niesiony na skorupiach żółwi. Zastanawiasz się, jak takie małe żółwie "
  "mogą dźwigać taki wielki świat? Jest ich po prostu BARDZO DUŻO! Ten się trochę zdenerwował, że "
  "tu jesteś, i porzucił swoje stanowisko. Ale żółwi jest na tyle dużo, że świat wciąż stoi.")

// gender-switched
N("Rusałka", GEN_F, "Rusałka", "Rusałki", "Rusałkę", "Rusałką")
S("A malicious water being. When you kill her, she changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "Złowroga wodna istota. Gdy ją zabijesz, zmienia pole, na którym jesteś: z pola lądowego na płytką wodę lub z płytkiej wody na głęboką.")
  
N("Topielec", GEN_M, "Topielec", "Topielce", "Topielca", "Topielcem")
S("A malicious water being. When you kill him, he changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "Złowroga wodna istota. Gdy go zabijesz, zmienia pole, na którym jesteś: z pola lądowego na płytką wodę lub z płytkiej wody na głęboką.")

// przet

Orb("Plague", "Zarazy")
S(
  "When you kill an enemy, adjacent enemies are also attacked, as long as they are further away from you than from the originally attacked enemy. "
  "These enemies may in turn infect further enemies, up to distance of at most 4 from you.",
  "Kiedy zabijesz przeciwnika, sąsiedni przeciwnicy również są atakowani, o ile są dalej od Ciebie niż przeciwnik zaatakowany oryginalnie. "
  "Ci przeciwnicy mogą zarażać kolejnych, aż do odległości 4 od Ciebie.")

//---

N("Frog Park", GEN_O, "Żabi Park", "Żabie Parki", "Żabi Park", "w Żabim Parku")

S("This park is a perfect place for frogs to hide from predators. "
  "So perfect that the frogs have grown in size, gained magical abilities, "
  "and become very dangerous themselves.",
  "Ten park jest doskonałym miejscem, by żabki mogły się ukryć przed drapieżnikami. "
  "Tak doskonałym, że żabki urosły, zdobyły zdolności magiczne, i same się "
  "stały bardzo niebezpieczne."
  )

N("Gold Ball", GEN_F, "Złota Piłka", "Złote Piłki", "Złotą Piłkę", "Złotą Piłką")
S("A toy lost in the Frog Park by a spoiled child.", "Zabawka zgubiona w Żabim Parku przez rozpuszczone dziecko.")

Orb("Impact", "Uderzenia")
S("Whenever you use a ranged Orb to achieve an effect, all the single-tile monsters adjacent to the target "
  "location are stunned.",
  "Za każdym razem, gdy używasz Sfery z efektem zdalnym, by uzyskać efekt, wszystkie jednopolowe stworzenia "
  "sąsiedujące z celem są ogłuszane.")

N("shrub", GEN_O, "krzak", "krzaki", "krzaka", "krzakiem")
S("A strange small tree that cannot be attacked with mundane weapons nor phased though. "
  "It can be vaulted over, though.",
  "Małe, dziwne drzewko, które nie może być ścięte zwykłymi broniami, ani nie może być przefazowane. "
  "Można użyć przeskoku.")

N("Giant Frog", GEN_F, "Wielka Żaba", "Wielkie Żaby", "Wielką Żabę", "Wielką Żabą")

S(
  "At first, you are shocked by the agility of this frog. Such a large creature "
  "should not be able to jump that quickly!\n\n"
  "Then, you notice the green glow around its legs. This frog must be magical... "
  "it has sacrificed its swimming abilities for superfrog jumping powers.",
  
  "Na początku zaskoczyła Cię zręczność tej żaby. Tak wielkie stworzenie "
  "nie powinno być zdolne do tak szybkich skoków!\n\n"
  "Potem zauważy%łeś zieloną poświatę wokół jej nóg. Ta żaba musi być magiczna... "
  "poświęciła zdolność pływania dla nadżabich zdolności skakania.")

N("Yellow Frog", GEN_F, "Żółta Żaba", "Żółte Żaby", "Żółtą Żabę", "Żółtą Żabą")

S("A slightly transparent yellow frog. It has mastered the magical power of phasing through solid "
  "obstacles such as rock.",
  "Lekko przezroczysta żółta żaba. Opanowała ona magiczną moc Fazy, pozwaljąca na przechodzenie "
  "przez lite przeszkody, takie jak skały.")

N("Blue Frog", GEN_F, "Niebieska Żaba", "Niebieskie Żaby", "Niebieską Żabę", "Niebieską Żabą")

S(
  "This frog is able to vault over the shrubs in the Frog Park, destroying the shrub in the process. "
  "While it can also vault over monsters, it will never hurt the other frogs!",

  "Ta żaba umie przeskakiwać krzaki w Żabim Parku, przy okazji niszcząc te krzaki. "
  "Potrafi ona też przeskakiwać inne stworzenia, ale nigdy nie skrzywdzi innych żab!")

S("\n\nThis Frog uses the power of %the1. You get 5 charges yourself for killing it.", "\n\nTa Żaba używa mocy '%1'. Dostaniesz 5 ładunków tej Sfery, gdy ją zabijesz.")

//---

N("Eclectic City", GEN_N, "Eklektyczne Miasto", "Eklektyczne Miasta", "Eklektyczne Miasto", "w Eklektycznym Mieście")

S("Many kinds of creatures are living here, each with its own architectural style.",
  "Wiele rodzajów stworzeń tu mieszka, każde ze swoim własnym stylem architektury.")

N("Lazurite Figurine", GEN_F, "Lazurytowa Figurka", "Lazurytowe Figurki", "Lazurytową Figurkę", "Lazurytową Figurką")

S("A beautiful blue figurine. Every figurine is different.",
  "Piękna niebieska figurka. Każda figurka jest inna.")
  
Orb("Chaos", "Chaosu")
S(
  "When you move, the contents of the two cells adjacent to your both locations are swapped. "
  "If they contain creatures, these creatures will be stunned for some time, before they understand what happened.\n\n"
  "Does not work if one of the cells contains a "
  "multi-tile creature or a barrier-like wall.",
  
  "Kiedy się ruszasz, zawartość dwóch pól sąsiedujących z obiema Twoimi polami jest zamieniana. "
  "Jeśli były tam stworzenia, zostają one ogłuszone przez pewien czas, aż zrozumieją, co się właściwie stało.\n\n"
  "Nie działa, gdy któreś z pól zawiera istotę wielopolową lub bariero-podobną ścianę.")

S("digon", "dwukąt")
S("triangle", "trójkąt")
S("square", "kwadrat")
S("pentagon", "pięciokąt")
S("hexagon", "sześciokąt")
S("heptagon", "siedmiokąt")
S("octagon", "ośmiokąt")

// 12.0
//======

// missing Android buttons

S("WAIT", "CZEK")
S("RUG", "DYWAN")
S("CAM", "KAM")

// missing UI texts

S("error: ", "błąd: ")
S("rules: %1", "reguły: %1") // WFC

// missing game texts

S("You polymorph %the1 into %the2!", "Transformujesz %a1 w %a2!")
S("All players are in the game!", "Wszyscy gracze są w grze!")
S("\"Are there Temples of Cthulhu in your world? Why not?\"", 
  "\"Czy w Twoim świecie też są Świątynie Cthulhu? Dlaczego nie?\"")
S("saves", "zapisy")
S("players", "gracze")
S("You swing your sword at %the1.", "Zamachujesz się mieczem na %a1.")
S("You swing your sword.", "Zamachujesz się mieczem.")
S("There is no room for %the1!", "Nie ma miejsca na %a1!")
S("Don't play with slime and electricity next time, okay?", "Następnym razem nie baw się mazią i elektrycznością, OK?")
S("The Orb of Safety from the Land of Eternal Motion might save you.", 
  "Sfera Bezpieczeństwa z Krainy Wiecznego Ruchu może Cię uratować.")
S("%the1 protects your boat!", "%1 chroni Twoją łódkę!")
S("Are you sure you want to step there?", "Na pewno chcesz tam stanąć?")
S("Hint: press 1 to change the projection.", "Wskazówka: wciśnij 1 by zmienić rzut.")
S("%The1 turned out to be an illusion!", "%1 okaza%ł1 się być iluzją!")
S("You feel that these directions are %1 turns old.", "Czujesz, że te wskazówki są stare. (Liczba kolejek: %1)")
S("%The1 raises his weapon...", "%1 podnosi broń...")
S("%The1 takes %his1 revenge on %the2!", "%1 mści się, atakując %a2!")

// parameters

S("You can enter formulas in this dialog.", "W tym dialogu można podać wzór.")
S("Functions available:", "Dostępne funkcje:")
S("Constants and variables available:", "Dostępne stałe i zmienne:")
S("Animations:", "Animacje:")
S("a..b -- animate linearly from a to b", "a..b -- animuj liniowo od a do b")
S("a..b..|c..d -- animate from a to b, then from c to d", "a..b..|c..d -- animuj od a do b, potem od c do d")
S("a../x..b../y -- change smoothly, x and y are derivatives", "a../x..b../y -- gładka zmiana, x i y to pochodne")
S("Parameter names, e.g. '%1'", "Nazwy parametrów, np. '%1'")
S("Parameter names", "Nazwy parametrów")
S("These can be combined, e.g. %1", "Można łączyć, np. %1")

// manifolds

S("alternate manifolds", "alternatywne rozmaitosci")
S("generators (%1,%2), size %3", "generatory (%1,%2), rozmiar %3")
S("This option finds alternate solutions. For example, there are three {7,3} manifolds with 156 heptagons each (\"first Hurwitz triplet\").", 
  "Ta opcja znajduje alternatywne rozwiązania. Przykładowo, są trzy rozmaitości {7,3} złożonhe z 156 siedmiokątów (\"pierwsza trójka Hurwitza\").")

// unrecognized nonliteral: f->name in geom-exp.cpp:274
S("Only works with 2D geometries", "Działa tylko w geometriach 2D")
S("stereographic Panini", "stereograficzny rzut Paniniego")
S("orthographic projection", "rzut ortograficzny")
S("#%1, cells: %2, p=%3", "#%1, pól: %2, p=%3")
S("affine", "afiniczna")
// unrecognized nonliteral: qstring in geom-exp.cpp:625
S("geometry/topology/tiling", "geometria/topologia/parkietaż")
S("binary tiling width", "szerokość parkietażu binarnego")
S("Nil width", "szerokość Nil")
S("stretched geometry", "rozciągnięta geometria")
S(
  "Stretch the metric along the fibers. This can currently be done in rotation spaces and in 8-cell, 24-cell and 120-cell. "
  "Value of 0 means not stretched, -1 means S2xE or H2xE (works only in the limit). (Must be > -1)",
  
  "Rozciągnięcie metryki wzdłuż włókien. Można to zrobić w przestrzeniach obrotów, 8-komórce, 24-komórce, i 120-komórce. "
  "Wartość 0 oznacza brak rozciągnięcia, -1 to S2xE i H2xE (w granicy). Musi być > -1.")
  
S("basic tiling", "parkietaż podstawowy")
S("fake curvature", "fałszywa krzywizna")
S("tessellation sliders", "suwaki parkietażu")
S("truncate ultra-vertices with mirrors", "przytnij ultra-wierzchołki lustrami")
S("adjacency rule", "reguła sąsiedztwa")
S("vertex", "wierzchołek")
S("face", "ściana")
S("edge", "krawędź")
S("info about: %1", "info o: %1")
S("genus", "genus")

S("VR demos", "demo VR")
S("warning: these will restart your game!", "uwaga: to zrestartuje grę!")
S("standard HyperRogue but in VR", "standard HyperRogue ale VR")
S("HyperRogue FPP", "HyperRogue FPP")
S("Hypersian Rug", "Hiperski Dywan")
S("sphere from the inside", "sfera od wewnątrz")
S("sphere from the outside", "sfera od zewnątrz")
S("Thurston racing", "wyścigi w geometrii Thurstona")
S("raytracing in H3", "raytracing w H3")

S("VR settings", "ustawienia VR")
S("VR enabled", "VR włączone")
S("VR initialized correctly", "VR zainicjalizowane prawidłowo")
S("(this setting is for 3D geometries only, use 'model viewing' instead)", 
  "(to ustawienie jest dla geometrii 3D, użyj 'oglądanie modelu')")
  
S("absolute unit in meters", "jednostka absolutna w metrach")

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
  
  "Odpowiednik jednostki absolutnej geometrii nieuklidesowej w metrach. "
  "Wpływa to na ruch okularów VR i widzenie dwuoczne.\n\n"
  "W geometrii sferycznej jest to promień sfery. Im mniejsza jednostka absolutna, "
  "tym silniejsze efekty nieeuklidesowe. Elementy świata HyperRogue mają ustalony "
  "rozmiar w jednostkach absolutnych, także zmniejszenie jednostki spowoduje ich "
  "zmniejszenie. W trybie euklidesowym ten parametr po prostu skaluje wszystko "
  "(w parkietażu sześciennym jest to bok sześcianu). Parametr wpływa jedynie na "
  "perspektywę, pozostałe modele używają parametru 'skala VR' z menu rzutów.")
        
S("reset the reference point", "zresetuj punkt odniesienia")
S("pointer length", "długość wskaźnika")
S(
  "If the pointer length is 0.5m, the object pointed to is 0.5 meter from the controller. "
  "This is used in situations where the controller is used as a 3D mouse, e.g., "
  "the drawing tool in three-dimensional geometries. When pointing at two-dimensional surfaces, "
  "this is not relevant (the pointer is as long as needed to hit the surface).",
  
  "Jeśli długość wskaźnika to 0.5m, wskazywany punkt jest 0.5 metra od kontrolera. "
  "Ma to znaczenie, gdy kontroler jest używany jako mysz 3D, np. narzędzie do rysowania w "
  "geometriach 3D. Gdy wskazujemy powierzchnie, nie ma on znaczenia -- wskaźnik jest tak "
  "długi, by dotknąć powierzchni.")
  
S("UI size", "rozmiar UI")
S("How big is a pixel of the user interface (HUD and menus). The user interface is as big as the window on the desktop.",
  "Jak duży jest piksel interfejsu użytkownika (HUD i menu). Rozmiar w pikselach jest równy rozmiarowi okna z ekranu "
  "komputerowego w pikselach.")

S("UI depth", "głębokość UI")
S("How far to show the user interface (HUD and menus).", "Jak daleko jest interfejs użytkownika (HUD i menu).")

//

S("Not available in the daily challenge!", "Niedostępne w dziwnej misji!")
S("Welcome to CoverRogue!", "Witaj w CoverRogue!")
S("Welcome to SL(2,R)-ogue!", "Witaj w SL(2,R)-ogue!")
S("Emergency save at ", "Zapis bezpieczeństwa w ")

// tour

S("You cannot change geometry in this slide.", "Nie można zmieniać geometrii na tym slajdzie.")
S("This does not work in bounded geometries.", "Nie działa w rozmaitościach domkniętych.")
S("This does not work in Euclidean geometry.", "Nie działa w geometrii euklidesowej.")
S("This works only in hyperbolic geometry.", "Działa jedynie w geometrii hiperboliczej.")
S("This land has no Euclidean version.", "Ta kraina nie ma wersji euklidesowej.")
S("This land has no spherical version.", "Ta kraina nie ma wersji sferycznej.")
S("Changed the projection.", "Zmieniono projekcję.")
S("HyperRogue Guided Tour", "Wycieczka HyperRogue")
S("slideshows", "pokazy slajdów")
S("go up", "w górę")

S(" (edges too short: %1)", " (za krótkie krawędzie: %1)")
S("wait...", "czekaj...")
S("bitruncation const", "stała przycinania")
S("too small period -- irregular tiling generation fails", "za mały okres -- tworzenie parkietażu nieudane")
S("To create Euclidean irregular tesselations, first enable a torus", 
  "By stworzyć nieregularny parkietaż euklidesowy, zacznij od torusa")

S("Error: period mismatch", "Błąd: niespójność okresu")
S("Hantzsche-Wendt space", "przestrzeń Hantzsche-Wendt")
S("HyperRogue classic torus", "klasyczny torus HyperRogue")

// archimedean & arbitrile

S("invalid dihedron", "nieprawidłowy dwuścian")
S("face mismatch", "nieprawidłowość w ścianach")
S("face mismatch II ", "nieprawidłowość w ścianach ")
S("face mismatch II", "nieprawidłowość w ścianach")

S("Polygon number %1 did not close correctly (%2 %3). Here is the picture to help you understand the issue.\n\n", 
  "Wielokąt %1 nie zamknął się prawidłowo (%2 %3). Oto pomocny obrazek.\n\n")
S("connection debugger", "debugger połączeń")
S("open a tiling", "otwórz parkietaż")

// goldberg

S("inverse rectify", "odwrotne ucięcie")
S("inverse truncate", "odwrotne ścięcie")
S("warped version", "wersja zakrzywiona")
S("swap x and y", "zamień x i y")
S("dual of current", "dualny do obecnego")
S("dual", "parkietaż dualny")
S("unrectified", "unrectified")
S("warped", "warped")
S("untruncated", "untruncated")

// rug/embeddings
S("save the current embedding", "zapisz obecne zanurzenie")
S("load a saved embedding", "załaduj zanurzenie")
S("save embedding to:", "zapisz zanurzenie do:")
S("load embedding from:", "załaduj zanurzenie z:")
S("Failed to save embedding to %1", "Nie udało się zapisać zanurzenia do %1")
S("Failed to load embedding from %1", "Nie udało się wczytać zanurzenia z %1")

S("Failed to enable", "Nie udało się uruchomić")
S("protractor", "kątomierz")
S("move on touch", "ruch na dotyk")
S("finger range", "zasięg palca")
S("Press 1 to enable the finger mode.", "Wciśnij 1 by uruchomić tryb palca.")
S("finger force", "siła palca")
S("Press 1 to enable the finger force.", "Wciśnij 1 by uruchomić siłę palca.")

// patterns

S("probability of a wall (%)", "prawdopodobieństwo ściany (%)")
S("single color", "pojedynczy kolor")
S("distance from origin", "odległość od startu")
S("nice coloring", "fajne kolorowanie")
S("Crystal coordinates", "współrzędne Kryształu")
S("sides", "ściany")
S("display only chessboard white", "tylko białe pola szachownicy")
S("display only chessboard black", "tylko czarne pola szachownicy")

S(
          "This lets you specify the color pattern as a function of the cell. "
          "Available parameters:\n\n"
          "x, y, z (hyperboloid/sphere/plane coordinates in non-crystal geometries)\n"
          "ex, ey, ez (in Euclidean geometries)\n"
          "x0, x1, x2... (crystal geometry only)\n"
          "0 is black, 1 is white, rgb(1,0,0) is red, ifp(p-2,1,0) is blue (p=1 for red, 2 for green, 3 for blue).",

          "Tu możesz określić wzór jako funkcję komórki. Dostępne parametry:\n\n"
          "x, y, z (współrzędne hiperboloidy/sfery/powierczni, poza kryształami)\n"
          "ex, ey, ez (w geometriach euklidesowych)\n"
          "x0, x1, x2... (w kryształach)\n"
          "0 to czarny, 1 to biały, rgb(1,0,0) to czerwony, ifp(p-2,1,0) to niebieski (p=1 to czerwony, 2 to zielony, 3 to niebieski)."
          )
          
S(
          "w (fourth coordinate)\n"
          "wallif(condition, color)\n", 

          "w (czwarta współrzędna)\n"
          "wallif(warunek, kolor)\n"
          )

S("see compute_map_function in pattern2.cpp for more\n", "obejrzyj funkcję compute_map_function w pattern2.cpp")

S("broken Emerald Pattern", "rozbity Szmaragdowy Wzór")
S("single cells", "pojedyncze pola")
S("edit both bitrunc colors", "edytuj oba pola przycięte")
S("extra symmetries", "dodatkowe symetrie")
S("odd/even", "parzysty/nieparzysty")
S("cheat", "oszustwo")

S("cannot adjust depth", "brak możliwości ustawienia głębokości")
S("cannot adjust camera", "brak możliwości ustawienia kamery")
S("does not work with perfect Klein", "nie działa w doskonałym modelu Kleina")

S("randomize", "losowo")
S("seed", "ziarno")
S("select a puzzle", "wybierz zagadkę")

// texture mode

S("Failed to load %1", "Nie udało się wczytać %1")
S("scale/rotate the texture", "skaluj/obracaj teksturę")
S("scale/rotate the model", "skaluj/obracaj model")
S("Unexpected missing cell #%1/%1", "Niespodziewane brakujące pole #%1/%1")
S("racing in Thurston geometries", "wyścigi w geometriach Thurstona")
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
    
    "Ta przestrzeń pozwala grać na d-wymiarowej kracie. Ustaw 3 wymiary i 'tryb 3D' by zobaczyć, jak to działa -- gramy na "
    "okresowej powierzchni w trzech wymiarach, złożonej z sześciokątów; każdy sześciokąt łączy się z 6 innymi, w 6 możliwych kierunkach. "
    "Gra wizualizuje to z punktu widzenia mieszkańca tej powierzchni (po regularyzacji i wygładzeniu), przy założeniu, że "
    "promienie świetlne również są ograniczone do powierzchni -- co oznacza, że widzimy parkietaż {2d,4}, z tym, że "
    "promienie świetlne czasami trafiają w pętlę i widzimy samych siebie (innymi słowy, krata jest przestrzenią ilorazową "
    "przestrzeni hiperbolicznej). Ta sama konstrukcja działa w wyższych wymiarach. Pół wymiaru oznacza wymiar, w którym są tylko "
    "dwa poziomy, np. 2.5D ma górną i dolną płaszczyznę.\n\n"
    "Można przyciąć ten parkietaż -- reguły HyperRogue działają lepiej, ale przestrzeń jest trudniejsza do zrozumienia."    
    )
S(
    "This is a representation of four-dimensional geometry. Can you find the Holy Grail in the center of the Round Table?\n\n"
    "In 'Knight of the 16-Cell Table', each cell has 8 adjacent cells, "
    "which correspond to 8 adjacent points in the four-dimensional grid. The Round Table has the shape of a 16-cell.\n\n"
    "In 'Knight of the 3-Spherical Table', it is the same map, but double bitruncated. The Round Table has the shape of a hypersphere.\n\n",
    
    
    "To jest reprezentacja przestrzeni czterowymiarowej. Czy znajdziesz Świętego Graala w środku Okrągłego Stołu?\n\n"
    "W 'Rycerzu 16-Stołu' każde pole ma 8 sąsiadów, i Stół ma kształt 16-komórki.\n\n"
    "W 'Rycerzu 3-Sferycznego Stołu' mapa jest podwójnie przycięta, i Stół ma kształt hipersfery.\n\n"
    )
S("let me understand how the coordinates work", "pokaż mi, jak działają współrzędne")
S("thanks, I need no hints (achievement)", "dzięki, nie potrzebuję wskazówek (osiągnięcie)")
S("more about this geometry...", "więcej o tej przestrzeni...")
S("render a cut", "pokaż przecięcie")
S("cut level", "poziom przecięcia")

S("Do not use compases.\n\n", "Nie używaj kompasów.\n\n")
S("Crystal Camelot is an octahedron in 'pure' 3D crystal geometry (and a similar polytope in other pure crystals), "
      "and an Euclidean ball in bitruncated/Goldberg crystals.", 
  "Kryształowy Camelot jest ośmiościanem w 'czystej' geometrii 3D, i Euklidesową kulą po operatorach Goldberga.")
S("puzzle/exploration mode", "tryb zagadek/eksploracji")

// puzzle mode

S("exploration", "eksploracja")
S("puzzles", "łamigłówki")
S("other puzzles", "inne łamigłowki")
S("hyperbolic Minesweeper", "Hiperboliczny Saper")
S("dual geometry puzzle", "łamigłowka o podwójnej geometrii")
S("Uncover all cells which do not contain mines", "Pokaż pola nie zawierające min")
S("Cells to uncover: %1", "Pól do odkrycia: %1")
S("You won in %1", "Wygra%łeś w %1")
S("Dropped floors: %1/%2", "Zrzucone pola: %1/%2")

// shmup control
S("camera forward", "kamera do przodu")
S("camera rotate X", "obrót kamery X")
S("camera rotate Y", "obrót kamery Y")
S("Configure dead zones", "ustaw martwe strefy") // for joysticks

// keyboard help
S("arrows", "strzałki")
S("WASD keys", "klawisze WASD")
S("VI keys", "klawisze VI")

// sight range settings

S("sight range bonus", "bonus do zasięgu wzroku")
S("3D sight range for the fog effect", "zasięg wzroku 3D, dla efektu mgły")

S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "Zasięg widzenia w geometriach 3D jest podany w jednostkach absolutnych i wpływa na efekt mgły.\n\n"
  "W geometrii sferycznej 2π pozwala widzieć rzeczy za Tobą jakby były przed Tobą, a π pozwala widzieć antypody jakby były "
  "blisko. W geometrii hiperbolicznej liczba renderowanych pól zależy wykładniczo od zasięgu wzroku, i im więcej pól, "
  "tym gorsza wydajność.\n\n"
  "Zasięg wzroku wpływa na rozgrywkę, potwory działają wtw są widoczne -- generacja potworów bierze to pod uwagę.")
  
S("max difference in Z coordinates", "maksymalna różnica w Z")
S("area-based range", "zasięg oparty o powierzchnię")
S("draw range based on", "zasięg określony przez")
S("size (no gen)", "rozmiar (bez generacji)")
S("size", "rozmiar")
S("sight range settings", "ustawienia zasięgu wzroku")

// other settings

S("play music when out of focus", "muzyka out of focus")
S("floating bubbles: treasure thresholds", "bąbelki: progi skarbów")
S("vector settings", "ustawienia grafiki wektorowej")
S("Line width setting is only taken into account in OpenGL.", "szerokość linii brana pod uwagę tylko w OpenGL")
S("shots only", "tylko zrzuty")
S("hint: press Alt while testing modes", "wsk: testując tryby wciśnij Alt")
S("no fill in neon mode", "brak wypełnienia w trybie neonu")
S("(vsync disabled in VR)", "(vsync wyłączony w VR)")
S("apply changes", "wprowadź zmiany")
S("width", "szerokość")
S("camera movement speed", "szybkość ruchu kamery")
S("camera rotation speed", "szybkość obrotu kamery")
S("send scores to Steam leaderboards", "wyślij wyniki do rankingów Steam")
S("crosshair size", "rozmiar celownika")
S("crosshair color", "kolor celownika")

S("Must be less than %1°. Panini projection can be used to get higher values.", 
  "Musi być poniżej %1°. Rzut Paniniego pozwala użyć wyższych wartości.")

S("Panini projection", "Rzut Paniniego")
S(
  "The Panini projection is an alternative perspective projection "
  "which allows very wide field-of-view values.\n\n",
  
  "Rzut Paniniego to alternatywna perspektywa, pozwalająca na bardzo szerokie "
  "pole widzenia.\n\n")

S("spherical perspective projection", "rzut perspektywy sferycznej")
S(
  "Set to 1 to get stereographic projection, "
  "which allows very wide field-of-view values.\n\n",
  
  "Ustaw 1 by uzyskać rzut sterograficzny, co pozwala na bardzo szerokie "
  "pole widzenia.\n\n"  
  )

S(
  "HyperRogue uses "
  "a quick implementation, so parameter values too close to 1 may "
  "be buggy (outside of raycasting); try e.g. 0.9 instead.", 
  
  
  "Implementacja w HyperRogue jest niestaranna, także "
  "wartości bliskie 1 (poza raycastingiem) mogą powodować artefakty; lepiej "
  "użyć 0.9.")

S("Berger sphere limit", "ograniczenie sfery Bergera")

S("Primitive-based rendering of Berger sphere is currently very slow and low quality. "
  "Here you can choose how many images to draw.", 
  "Renderowanie sfery Bergera oparte na trójkątach obecnie ma niską jakość. "
  "Tu możesz wybrać liczbę pokazanych obrazów.")
  
S(
  "Camera is placed %1 absolute units above a plane P in a three-dimensional "
  "world. Ground level is actually an equidistant surface, %2 absolute units "
  "below the plane P. The plane P (as well as the ground level or any "
  "other equidistant surface below it) is viewed at an angle of %3 "
  "(the tangent of the angle between the point in "
  "the center of your vision and a faraway location is 1/cosh(c) = %4).", 
  
  "Kamera jest ustawiona %1 jednostek absolutnych nad płaszczyzną P w trójwymiarowym "
  "świecie. Poziom podłogy jest powierzchnią ekwidystantną, %2 jednostek poniżej P. "
  "P jest widziana pod kątem %3 (tangens kąta między centrum widzenia a odległą "
  "lokacją to 1/cosh(c) = %4).")
  
S("cool fog effect", "fajna mgła")
S("borders", "granice")
S("pattern colors", "kolory wzoru")
S("Galápagos shading", "cieniowanie Galápagos shading")
S("targetting ranged Orbs long-click only", "celowanie sfer na odgległość wymaga długiego kliku")

S("0 to disable", "0 by wyłązcyć")
S("recently changed settings", "ostatnio zmieniane ustawienia")
S("find a setting", "znajdź ustawienie")

// nonisotropic

S("geodesic table missing", "brak tablicy geodezyjnych")
S(
  "If the 2D underlying manifold is bounded, the period should be a divisor of the 'rotation space' "
  "value (PSL(2,R)) times the Euler characteristics of the underlying manifold. "
  "For unbounded underlying manifold, any value should work (theoretically, "
  "the current implementation in HyperRogue is not perfect).\n\n"
  "We won't stop you from trying illegal numbers, but they won't work correctly.", 
        
  "Gdy rozmaitość 2D pod spodem jest domknięta, okres powinien być dzielnikiem wartości 'przestrzeń obrotów' "
  "(PSL(2,R)) pomnożonej przez charakterystykę Eulera. Dla niedomkniętych rozmaitości teoretycznie każda wartość "
  "powinna działać (implementacja w HR nie jest doskonała).\n\n"
  "Możesz użyć wartości nielegalnych, ale nie będą działać prawidłowo."
  )
S("SL(2,R)", "SL(2,R)")
S("universal cover", "nakrycie uniwersalne")
S("maximum", "maksimum")
S("works correctly so far", "póki co działa")

S("non-periodic", "nieokresowe")
S("patched Chaos", "łaty Chaosu")
S("excessive walls", "nadmiar ścian")
S("total chaos", "totalny chaos")
S("random-walk chaos", "chaos błądzenia losowego")
S("single land: ", "jedna kraina: ")
S("single land", "jedna kraina")
S("wall-less", "bez ścian")

S("highlight stuff", "podkreśl elementy")
S("press Alt", "trzymaj Alt")
S("highlight", "podkreślenie")
S("super-highlight", "super-podkreślenie")

S("shape editor", "edytor kształtów")
S("drawing tool", "narzędzie do rysowania")
S("land structure", "struktura krain")
S("highlights & achievements", "osiągnięcia i atrakcje")
S("classic game except hyperbolic", "klasyczna gra w wersji hiperbolicznej")
S("can you find the Heptagonal Grail?", "znajdziesz Siedmiokątnego Graala?")
S("General Euclid", "Generał Euklides")
S("Worm of the World", "Robak Świata")
S("Lovász Conjecture", "Hipoteza Lovásza")
S("Knight of the 16-Cell Table", "Rycerz 16-Stołu")
S("Knight of the 3-Spherical Table", "Rycerz 3-Sferycznego Stołu")
S("Emerald Mine in {5,3,4}", "Kopalnia Szmaragdów w {5,3,4}")

// help

S("credits", "zasługi")

S("\n\nAdditionally, the probabilities of generating terrain features are subtly changed in the following lands:", 
  "\n\nDodatkowo, prawdopodobieństwa generowania terenu są lekko zmienione w następujących krainach:")
S(
    "\n\nOnce you collect a Bomberbird Egg, "
    "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
    "Collecting even more Eggs will increase the radius.",
    
    "\n\nPo zebraniu Jaja Bombardiera, stanięcie na polu bez miny ujawnia sąsiednie pola. "
    "Im więcej Jaj, tym większy promień tego efektu.")
    
S(" (shadow in %1)", " (cień w %1)")
S(", %1 the Knight", ", Rycerz %1")
S("Hypersian Rug menu", "menu Hiperskiego Dywanu")
S("HyperRogue help", "pomoc HyperRogue")

// history mode

S("s = save to .png", "s = zapis do .png")
S("rendered band ($ID=segment, $DATE=date)", "zapisana wstęga ($ID=segment, $DATE=data)")
S("animate from last recenter to precise current view", "animuj od ostatniego środkowania do obecnego stanu")
S("save the animation starting point", "zapisz punkt początkowy animacji")
S("animate from the starting point", "animacja od punktu początkowego")
S("reset animation", "reset animacji")
S("include history when auto-rendering", "historia przy auto-renderowaniu")
S("more options in hyperbolic geometry", "więcej opcji w geometrii hiperbolicznej")
S("more options in band projections", "więcej opcji w rzutach wstęgowych")
S("filename format to use ($ID=segment, $DATE=date)", "format nazwy pliku ($ID=segment, $DATE=data)")
S("band segment", "segment wstęgi")

// screenshot/animations

S("PNG", "PNG")
S("SVG", "SVG")
S("WRL", "WRL")
S("centering", "centrowanie")
S("You can pick the angle. Note: the direction the PC is facing matters.", 
  "Wybierz kąt. Uwaga. Kierunek postaci ma znaczenie.")
S("rotate PC", "obróć postać")
S(
  "The value of 1 means that the period of this animation equals the period set in the animation menu. "
  "Larger values correspond to faster animations.", 
  "Wartość 1 oznacza, że okres tej animacji jest rowny okresowi ustawionemu w trybie animacji. Wyższa wartość to szybsza animacja.")
S("shift", "przesunięcie")
S("movement angle", "kąt ruchu")
S("This is the period of the whole animation, though in some settings the animation can have a different period or be aperiodic. "
      "Changing the value will make the whole animation slower or faster.",
  "To jest okres animacji. W niektórych ustawieniach animacja może mieć inny okres lub być aperiodyczna. "
  "Zmiana wartości spowoduje, że cała animacja będzie wolniejsza lub szybsza.")
S("Least common multiple of the animation periods of all the game objects on screen, such as rotating items.", 
  "Najmniejsza wspólna wielokrotność okresów animacji wszystkich obiektów na ekranie, takich jak obracające się przedmioty.")
S("Möbius transformations", "przekształcenia Möbiusa")
S("circle spins", "obroty koła")
S("circle radius", "promień koła")
S("double spin", "podwójny obrót")
S("triple spin", "potrójny obrót")
S("draw the circle", "rysuj koło")
S("cycle length", "długość cyklu")
S("full circle", "pełne koło")
S("Zebra period", "okres Zebry")
S("Bolza period", "okres Bolza")
S("angle to screen normal", "kąt do normalnej do ekranu")
S("distance from rotation center", "odległość od środka obrotu")
S("monster turns", "kolejki potworów")
S("Number of turns to pass. Useful when simulating butterflies or cellular automata.", 
  "Liczba kolejek. Przydatne do symulacji motylków i automatów komórkowych.")
S("screen-relative rotation", "obrót względem ekranu")
S("angle", "kąt")
S("model-relative rotation", "obrót względem modelu")
S("automatic move speed", "szybkość automatycznego ruchu")
S("Move automatically without pressing any keys.", "Ruch bez wciskania klawiszy")
S("synchronize", "synchronizuj")
S("model rotation", "obrót modelu")
S("3D rotation", "obrót w 3D")
S("some parameters will only change in recorded animation", "niektóre parametry zmienią się tylko w nagranej animacji")
S("record to sequence of image files", "nagraj jako ciąg obrazków")
S("record to video file", "nagraj jako plik video")
S("explore this animation", "eksploruj obecną animację")

// raycaster

S("volumetric raycasting", "raycasting wolumetryczny")
S("active", "uruchomione")
S("intensity of random coloring", "intensywność losowych kolorów")
S("color randomly", "losowe kolory")
S("SMART", "SPRYT")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))\n", "wzór na jasność: max(1-d/sightrange, s*exp(-d/r))\n")
S("unfortunately this honeycomb is too complex for the current implementation (%1>%2)", 
  "niestety ten parkietaż 3D jest zbyt skomplikowany dla obecnej implementacji (%1>%2)")

// extra projections

S("geodesic", "goedezyjne")
S("quadrant coordinates", "współrzędne kwadrantu")
S("axial coordinates", "współrzędne osiowe")
S("anti-axial coordinates", "współrzędne anty-osiowe")
S("Werner projection", "rzut Wernera")
S("Aitoff projection", "rzut Aitoffa")
S("Hammer projection", "rzut Hammera")
S("loximuthal projection", "rzut loksymutalny")
S("Miller projection", "rzut Millera")
S("Gall stereographic", "rzut sterograficzny Galla")
S("Winkel tripel", "rzut Winkel tripel")
S("Poor man's square", "kwadrat biedaka")
S("Craig retroazimuthal", "rzut retroazymutalny Craiga")
S("Littrow retroazimuthal", "rzut retroazymutalny Littrowa")
S("Hammer retroazimuthal", "rzut retroazymutalny Hammera")
S("three-point equidistant", "rzut trójpunktowy ekwidystantny")
S("stereographic projection [VR]", "rzut stereograficzny [VR]")

// projection settings

S("simple model: projection", "prosty model: rzut")
S("simple model: perspective", "prosty model: perspektywa")
S("projection type", "typ rzutu")
S("edit VR or non-VR settings", "ustawienia VR czy non-VR")
S("rotational or Heisenberg", "model symetryczny czy Heisenberga")
S("1 = Heisenberg, 0 = rotational", "1 = Heisenberg, 0 = symetryczny")
S("use atan to make it finite", "użyj atan by rzut był skończony")
S("VR: rotate the 3D model", "VR: obróć model 3D")
S("VR: shift the 3D model", "VR: przesuń model 3D")
S("VR: scale the 3D model", "VR: skaluj model 3D")
S("match the period", "zgodne okresy")
S("(zoomed out)", "(oddalone)")
S("zoom 2x", "skala 2x")
S("zoom 1x", "skala 1x")
S("zoom 0.5x", "skala 0.5x")
S("cylinder/donut view", "widok walca/donuta")
S("first-person perspective", "perspektywa pierwszej osoby (FPP)")
S("toggle full 3D graphics", "pełna grafika 3D")
S("advanced projections", "rzuty zaawansowane")

// map editor
S("F5 = clear", "F5 = kasuj")
S("Are you sure you want to clear the map?", "Na pewno chcesz skasować mapę?")
S("select area to copy", "wybierz powierzchnię do skopiowania")
S("statue", "pomnik")
S("c = reuse", "c = ponownie")
S("d = delete", "d = kasuj")
S("p = grid color", "p = kolor siatki")
S("f = fill", "f = wypełnij")
S("T = text", "T = tekst")
S("e = erase", "e = kasuj")
S("z = spheres", "z = sfery")
S("z-level", "poziom z")

S("Failed to load pictures from %1", "Nie udało się wczytać obrazków %1")
S("Pictures loaded from %1", "Wczytano obrazki z %1")
S("Failed to save pictures to %1", "Nie udało się zapisać obrazków do %1")
S("Pictures saved to %1", "Zapisano obrazki do %1")
S("Hint: use F7 to edit floor under the player", "Wskazówka: użyj F7 by edytować pole pod graczem")
S("Are you sure you want to restart? This will let you draw on a blank screen.", "Na pewno? To pozwoli rysować po pustym ekranie.")
S("brush size", "rozmiar pędzla")

// checking configurables
S("Aitoff parameter", "parametr Aitoffa")
S("The Aitoff projection is obtained by multiplying the longitude by 1/2, using azimuthal equidistant projection, and then dividing X by 1/2. Hammer projection is similar but equi-area projection is used instead. Here you can change this parameter.", 
  "Rzut Aitoffa powstaje przez pomnożenie długości przez 1/2, użycie rzutu azymutalnego ekwidystantnego, i podzielenie X przez 1/2. Rzut Hammera jest podobny, ale używamy rzutu równopolowego. Tu możesz zmienić ten parametr.")
S("any spiral multiplier", "dowolny pmożnik spirali")
S("Collignon parameter", "parametr Collignona")
S("Collignon reflect", "odbicie Collignona")
S("ETS parameter", "parametr ETS")
S("eye level", "poziom oka")
S("off", "wył")
S("spam", "spam")
S("normal monsters", "normalne potwory")
S("high-threat monsters only", "potwory wysokiego zagrożenia")
S("faraway highlight color", "kolor podkreślania zdalnych potworów")
S("0 = monster color, 100 = red-light oscillation", "0 = kolor potwora, 100 = oscylacja czerwono-zielona")
S("font size", "rozmiar czcionki")
S("use specific fullscreen resolution", "użyj konkretnej rozdzielczości w trybie pełnego ekranu")
S("fullscreen resolution to use (X)", "rozdzielczość pełnego ekranu (X)")
S("fullscreen resolution to use (Y)", "rozdzielczość pełnego ekranu (Y)")
S("hyperbolic|spherical|Euclidean", "hiperboliczna|sferyczna|Euklidesowa")
S("show flat", "pokazuj płaski obraz")
S("loximuthal parameter", "parametr rzutu loksymutalnego")
S("Loximuthal is similar to azimuthal equidistant, but based on loxodromes (lines of constant geographic direction) rather than geodesics. The loximuthal projection maps (the shortest) loxodromes to straight lines of the same length, going through the starting point. This setting changes the latitude of the starting point.\n\nIn retroazimuthal projections, a point is drawn at such a point that the azimuth *from* that point to the chosen central point is correct. For example, if you should move east, the point is drawn to the right. This parameter is the latitude of the central point.\n\n(In hyperbolic geometry directions are assigned according to the Lobachevsky coordinates.)", 
  "Rzut loksymutalny jest podobny do azymutalnego ekwidystantnego, ale oparty na loksodromach (liniach stałego kierunku geograficznego) zamiast na geodezyjnych. "
  "Rzut loksymutalny przekształca (najkrótsze) loksodromy na linie proste tej samej długości idące przez zadany punkt startowy. Tu możesz ustawić punkt startowy.\n\n"
  "W rzutach retroazymutalnych punkt jest rysowsany tak, by azymut *od* danego punktu do wybranego punkt centralnego był prawdziwy. Przykładowo, jeśli należy ruszyć się na "
  "wschód, punkt będzie rysowany po prawej. Ten parametr określa szerokość geograficzną punktu centralnego.\n\n"
  "W geometrii hiperbolicznej kierunki są określone przez współrzędne Łobaczewskiego.")
 
S("onscreen keyboard", "klawiatura ekranowa")
S("projection distance|Gans Klein Poincare orthographic stereographic", "odległość rzutu|Gans Klein Poincare rzut ortograficzny rzut stereograficzny")
S("set relative font size", "względny rozmiar czcionki")
S("resizable window", "możliwość zmiany rozmiaru okna")
S("right spiral multiplier", "mnożnik spirali prostokątnej")
S("fisheye parameter", "parametr rybiego oka")
S("two-point parameter", "parametr modeli dwupunktowych")
S("In two-point-based models, this parameter gives the distance from each of the two points to the center.", 
  "W modelach dwupunktowych: ten parametr określa odległość punktów definiujących od środka.")
S("Winkel Tripel mixing", "mieszanie Winkel Tripel")
S("The Winkel Tripel projection is the average of Aitoff projection and equirectangular projection. Here you can change the proportion.", 
  "Rzut Winkel Tripel to średnia rzutu Aitoffa i rzutu ekwirektangularnego. Tu możesz ustawić proporcję.")
S("sphere spiral multiplier", "mnożnik spirali dla sfery")
S("spiral cone", "spiralny stożek")
S("variation|dual|bitruncated", "wariacja|dualne|przycięte")
S("screen size percentage to use (X)", "procent rozmiaru ekranu (X)")
S("screen size percentage to use (Y)", "procent rozmiaru ekranu (Y)")
S("specify relative window size", "względny rozmiar okna")
S("window resolution to use (X)", "rozmiar okna (X)")
S("window resolution to use (Y)", "rozmiar okna (Y)")

// ...

// fixed text

S("select the starting land", "wybierz startową krainę")

// missing text in animation

S("cells to go", "ile pól się ruszyć")

// missing translations for Hardcore, and the new Casual mode

S("casual mode", "tryb niezobowiązujący")
S("Switching casual allowed only before saving the game. Do you want to restart?", 
  "Przestawić tryb niezobowiązujący można jedynie przed zapisem gry. Chcesz zacząć od początku?")
S("You are in the casual mode! Achievements are disabled.", 
  "Jesteś w trybie niezobowiązującym! Osiągnięcia są wyłączone.")
S("Collect an Orb of Safety to save a checkpoint.", 
  "Zbierz Sferę Bezpieczeństwa, by zapisać stan gry.")
S("load (%1 turns passed)", "ładuj (minęło kolejek: %1)")

// missing texts in searching

S("press letters to search", "wciskaj litery, by szukać")
S("matching items", "pasujących elementów: ")

// YASC codes

S("YASC codes", "kody YASC")
S(" YASC code: ", " kod YASC: ")
S("YASC codes: Sides-Entity-Restrict-Threat-Wall", "kody YASC: Sides-Entity-Restrict-Threat-Wall")

// Dice Reserve

N("Dice Reserve", GEN_O, "Rezerwat Kostek", "Rezerwaty Kostek", "Rezerwat Kostek", "w Rezerwacie Kostek")
S("This land is full of sentient dice. Do not disturb them!",
  "Ta kraina jest pełna rozumnych kostek. Nie zakłócaj ich spokoju!")

N("Crystal Die", GEN_F, "Kryształowa Kostka", "Kryształowe Kostki", "Kryształową Kostkę", "Kryształową Kostką")
S("A nice souvenir from the Dice Reserve. Make sure to collect the whole set!",
  "Miła pamiątka z Rezerwatu Kostek. Zbierz cały zestaw!")

N("Unhappy Die", GEN_F, "Smutna Kostka", "Smutne Kostki", "Smutną Kostkę", "Smutną Kostką")
S("Sentient dice like to be in a position such that their highest number is on top. "
  "Unfortunately, someone has rolled this one into a wrong position, and did not fix this. "
  "It will reward you if you roll it so that the highest number is on top again!",

  "Kostki lubię być w takiej pozycji, że największa liczba jest na wierzchu. "
  "Niestety, ktoś tutaj przekręcił kostkę i tego nie naprawił. "
  "Przekręć ją tak, by najwyższa liczba znów była na wierzchu, a dostaniesz nagrodę!")

N("Happy Die", GEN_F, "Wesoła Kostka", "Wesołe Kostki", "Wesołą Kostkę", "Wesołą Kostką")

S(
  "A happy sentient die. Dice are happy when they are in their correct position "
  "(the highest number on the top); happy dice with one roll from their best position are sometimes found too."
  "Other positions are much less convenient for them and expose their "
  "weak spots. You can roll Happy Dice, but it may become angry!",

  "Szczęśliwa kostka! Kostki są szczęśliwe, gdy największa liczba jest na wierzchu. "
  "Czasami też można znaleźć szczęśliwe kostki położone o 1 obrót od najlepszej pozycji. "
  "Pozostałe pozycji są dużo mniej wygodne i odsłaniają ich słabe punkty. "
  "Możesz przetoczyć Szczęśliwą Kostkę, ale może się ona rozzłościć!")

N("Animated Die", GEN_F, "Ruchoma Kostka", "Ruchome Kostki", "Ruchomą Kostkę", "Ruchomą Kostką")

S("When sentient dice are too long in an incorrect position, they start to move on their own, "
  "and attack everyone. You can still convince Animated Dice of your good intentions by "
  "rolling them into a position such that the highest number is on top. "
  "If you do, the die will stop moving and (if it happens in the Dice Reserve) you will be rewarded. "
  "Other rolls and attacks are not allowed.",
  
  "Gdy rozumne kostki są za długo w złej pozycji, ruszają się same z siebie i atakują wszystkich. "
  "Możesz przekonać Ruchomą Kostkę o swoich dobrych intencjach, przekręcając ją w dobrą pozycję, "
  "czyli taką, że największa liczba jest na wierzchu. "
  "Wówczas kostka przestaje się ruszać, a Ty dostajesz nagrodę (o ile stanie się to w Rezerwacie Kostek). "
  "Pozostałe przekręcenia i ataki nie są dozwolone."
  )

N("Angry Die", GEN_F, "Wściekła Kostka", "Wściekłe Kostki", "Wściekłą Kostkę", "Wściekłą Kostką")

S("You have made a die unhappy. Taste the revenge! This one won't forgive you, no matter what you do.",
  "Przez Ciebie ta kostka jest nieszczęśliwa. Poczuj smak jej zemsty! Nie wybaczy Ci tego, co byś nie zrobi%ł0.")

S("The die is now happy, and you are rewarded!", "Kostka jest szczęśliwa. Zostajesz nagrodzon%y0!")
S("You have made a Happy Die angry!", "Rozzłości%łeś0 Szczęśliwą Kostkę!")
S("You can only push this die if the highest number would be on the top!", 
  "Możesz pchać tylko, jeżeli najwyższa liczba znalazłaby się na wierzchu!")
S("This die is really angry at you!", "Ta kostka jest bardzo, bardzo zła!")
S("The die is now happy, but won't reward you outside of the Dice Reserve!", "Kostka jest szczęśliwa. Bez nagrody poza Rezerwatem Kostek!")

// Cursed Canyon
//===============

N("Cursed Canyon", GEN_O, "Przeklęty Kanion", "Przeklęte Kaniony", "Przeklęty Kanion", "w Przeklętym Kanionie")
S("This canyon is full of curses! Curses are like Orbs, but grant negative effects.",
  "Ten kanion jest pełen klątw! Klątwy są jak Sfery, ale ich skutki są negatywne.")
  
N("Canyon Hag", GEN_F, "Wiedźma z Kanionu", "Wiedźmy z Kanionu", "Wiedźmę z Kanionu", "Wiedźmą z Kanionu")
S("A Canyon Hag can curse you from afar, if standing on top of a Curse and close enough.",
  "Wiedźma z Kanionu może Cię przekląć z daleka, gdy stoi na Klątwie dostatecznie blisko.")
S("%The1 curses you with %the2!", "%1 zaklina Cię! Dostajesz %a2!")

N("Capon Stone", GEN_O, "Kapłuni Kamień", "Kapłunie Kamienie", "Kapłuni Kamień", "Kapłunim Kamieniem")
S("A lapis alectorius, alectoria or capon stone is a non-precious stone found in the gizzard of capons (young, castrated roosters). In magic it is believed to be an effective amulet, granting the wearer a heightened sense of courage and boldness. These properties are unproven. [Wikipedia]",
  "Gdy kogut po trzech leciech bywa okapłuniony, albo oprawiony, a po okapłonieniu przez siedm lat żyje, tedy w nim rodzi się kapłuni kamień. [Słownik języka polskiego, Samuel Bogumił Linde]")

#define Curse(E, P) N("Curse of " E, GEN_F, "Klątwa " P, "Klątwy " P, "Klątwę " P, "Klątwą " P)
Curse("Weakness", "Słabości")
S("This Curse weakens your normal attack, which will only stun monsters. For monsters which are normally only stunned anyway, they don't push the victim away. You are also unable to cut trees or fight Ivy.",
  "Ta klątwa osłabia Twój zwykły atak. Będzie on jedynie ogłuszał przeciwników. Te, które i tak były jedynie ogłoszane, nie są odpychane. Nie możesz również ścinać drzew ani walczyć z Bluszczem.")

Curse("Draining", "Wysysania")
S("Drains the powers of all your orbs (and curses) by 2 charges per turn.",
  "Wysysa moc Twoich wszystkich sfer (i klątw), 2 ładunki na kolejkę.")

Curse("Water", "Wody")
S("Makes you leave a trail of shallow water. You also become afraid of water. You are unable to step into water, including shallow water and boats on water. "
  "On the good side, you are protected from fire.",
  
  "Powoduje, że pozostawiasz za sobą strugę płytkiej wody. Powoduje także, że boisz się wody. Nie możesz wejść do wody, w tym płytkiej wody i łódek na wodzie. "
  "Ma to dobrą stronę: jesteś chronion%y0 przed ogniem.")
  
Curse("Fatigue", "Zmęczenia")
S("With this Curse you cannot move too quickly. Every move increases your fatigue by 1; fatigue cannot go over 10. "
  "Resting reduces your fatigue by 5.",
  "Ta klątwa powoduje, że nie możesz chodzić zbyt szybko. Każdy ruch zwiększa zmęczenie o 1; zmęczenie nie może przekroczyć 10. "
  "Odpoczynek zmniejsza zmęczenie o 5.")
N("fatigue", GEN_N, "zmęczenie", "zmęczenia", "zmęczenie", "zmęczeniem")
S("See the Curse of Fatigue.", "Patrz Klątwa Zmęczenia.")
S("You are too fatigued!", "Jesteś zbyt zmęczon%y0!")

Curse("Repulsion", "Odpychania")
S("If you have this Curse, all the items on adjacent cells will try to escape from you.",
  "Gdy masz Klątwę Odpychania, przedmioty na sąsiednich polach próbują od Ciebie uciec.")

Curse("Gluttony", "Obżarstwa")
S("The first item you pick up is consumed without any effect.",
  "Pierwszy podniesiony przedmiot jest pożerany bez efektu.")
S("%The1 is consumed!", "Pożarto %a1!")

Orb("Purity", "Czystości")
S("Reverses all the curses. When found far away from the Cursed Canyon, you gain low amounts of various reversed curse powers.",
  "Odwraca efekty wszystkich klątw. W przypadku znaleziona daleko od Przeklętego Kanionu zdobywasz małe ilości odwróconych mocy klątw.")
#undef Curse
#undef Orb

// chapter names of the Guided Tour
S("Hyperbolic shapes", "Hiperboliczne kształty")
S("Projections of hyperbolic space", "Rzuty płaszczyzny hiperbolicznej")
S("Procedural generation", "Proceduralna generacja")

// newbie-friendly Safety

S("Find an Orb of Safety to save your game",
  "Znajdź Sferę Bezpieczeństwa by zapisać grę")

S("how to find an Orb of Safety?", "jak znaleźć Sferę Bezpieczeństwa?")

S(
  "To save the game you need an Orb of Safety.\n\n"
  "Orbs of Safety appear:\n\n"
  "* in the Crossroads and the Land of Eternal Motion, after you collect %1 Phoenix Feathers in the Land of Eternal Motion\n\n"
  "* in the Ocean after you unlock it (%2 treasures)\n\n"
  "* in the Prairie after you unlock it (%3 treasures)\n\n",
  
  "By zapisać grę potrzebujesz Sferę Bezpieczeństwa.\n\n"
  "Możesz je znaleźć:\n\n"
  "* na Skrzyżowaniu i w Krainie Wiecznego Ruchu, po znalezieniu %1 Piór Feniksa w Krainie Wiecznego Ruchu\n\n"
  "* na Oceanie po jego odblokowaniu (%2 skarbów)\n\n"
  "* na Prerii po jej odblokowaniu (%3 skarbów)\n\n"
  )

N("Palace Quest", GEN_O, "Misja w Pałacu", "Misje w Pałacu", "Misję w Pałacu", "na Misji w Pałacu")

N("Pike", GEN_F, "Szczupak", "Szczupaki", "Szczupaka", "Szczupakiem")
