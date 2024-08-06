// HyperRogue Chinese translation, by 我叫超威蓝猫 and 1iuh
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// This translation file is encoded with UTF-8

// Nouns.

// For each noun in English, provide:

// 1) the type (usually gender) of the noun. For example, in Polish each noun can be:

// masculine living (GEN_M),
// masculine object (GEN_O),
// feminine (GEN_F), or
// neuter (GEN_N).

// 2) all the forms required by HyperRogue. The following forms are given:
// nominative (referred to as %1),
// measure words (%m1).

// Feel free to add more or less forms or types if needed. Note that you don't
// have to provide all the forms in your language, only those used by HyperRogue
// (in Polish just 4 of 14 forms are used, and this is probably similar in other
// languages).  The forms being replaced are defined in language.cpp; search for
// "if(l == 8)".

// MONSTERS
// ========

N("Yeti", GEN_N, "雪人","个")
N("Icewolf", GEN_N, "冰狼","条")
N("Ranger",  GEN_N, "游侠","名")
N("Rock Troll", GEN_N, "岩石巨魔","个")
N("Goblin", GEN_N, "地精","个")
N("Sand Worm", GEN_N, "沙虫","条")
N("Sand Worm Tail", GEN_N, "沙虫尾","段")
N("Sand Worm W", GEN_N, "沙虫 W","段")
N("Desert Man", GEN_N, "沙民","位")
N("Ivy Root", GEN_N, "常春藤根","段")
N("Active Ivy", GEN_N, "活跃常春藤","段")
N("Ivy Branch", GEN_N, "常春藤枝","段")
N("Dormant Ivy", GEN_N, "休眠常春藤","段")
N("Ivy N", GEN_N, "常春藤 N","段")
N("Ivy D", GEN_N, "常春藤 D","段")
N("Giant Ape", GEN_N, "巨猿","只")
N("Slime Beast", GEN_N, "黏液兽","头")
N("Mirror Image", GEN_N, "镜像","个")
N("Mirage", GEN_N, "幻影","个")
N("Golem", GEN_N, "魔像","个")
N("Eagle", GEN_N, "雄鹰","只")
N("Seep", GEN_N, "渗怪","头")
N("Zombie", GEN_N, "僵尸","个")
N("Ghost", GEN_N, "鬼魂","个")
N("Necromancer", GEN_N, "死灵法师","位")
N("Shadow", GEN_N, "阴影","道")
N("Tentacle", GEN_N, "触手","条")
N("Tentacle Tail", GEN_N, "触手尾","段")
N("Tentacle W", GEN_N, "触手 W","段")
N("Tentacle (withdrawing)", GEN_N, "触手 （收缩）","条")
N("Cultist", GEN_N, "邪教徒","名")
N("Fire Cultist", GEN_N, "烈焰邪教徒","名")
N("Greater Demon", GEN_N, "高等恶魔","个")
N("Lesser Demon", GEN_N, "低等恶魔","个")
N("Ice Shark", GEN_N, "冰鲨","条")
N("Running Dog", GEN_N, "奔行犬","条")
N("Demon Shark", GEN_N, "恶魔鲨","条")
N("Fire Fairy", GEN_N, "烈焰仙灵","位")
N("Crystal Sage", GEN_N, "水晶智者","位")
N("Hedgehog Warrior", GEN_N, "刺猬战士","位")

// ITEMS
// =====

N("Ice Diamond", GEN_N, "冰钻","枚")
N("Gold", GEN_N, "黄金","块")
N("Spice", GEN_N, "香料","捧")
N("Ruby", GEN_N, "红宝石","枚")
N("Elixir of Life", GEN_N, "万灵药","个")
N("Shard", GEN_N, "裂片", "枚")
N("Necromancer's Totem", GEN_N, "死灵图腾","尊")
N("Demon Daisy", GEN_N, "恶魔雏菊","朵")
N("Statue of Cthulhu", GEN_N, "克苏鲁雕像","尊")
N("Phoenix Feather", GEN_N, "凤凰羽毛","根")
N("Ice Sapphire", GEN_N, "冰蓝宝石","枚")
N("Hyperstone", GEN_N, "双曲石","块")
N("Key", GEN_N, "钥匙","把")
N("Dead Orb", GEN_N, "失效法球","枚")
N("Fern Flower", GEN_N, "蕨花","朵")

// ORBS: we are using a macro here
// ===============================

#define Orb(E, P) N("Orb of " E, GEN_N, P "法球","")
Orb("Yendor", "Yendor")
Orb("Storms", "风暴")
Orb("Flash", "闪光")
Orb("Winter", "寒冬")
Orb("Speed", "速度")
Orb("Life", "生命")
Orb("Shielding", "护盾")
Orb("Teleport", "传送")
Orb("Safety", "安全")
Orb("Thorns", "荆棘")

// TERRAIN FEATURES
// ================

N("none", GEN_N, "无","")
N("ice wall", GEN_N, "冰墙","堵")
N("great wall", GEN_N, "长城","道")
N("red slime", GEN_N, "红黏液","滩")
N("blue slime", GEN_N, "蓝黏液","滩")
N("living wall", GEN_N, "生机墙","堵")
N("living floor", GEN_N, "生机地板","块")
N("dead troll", GEN_N, "死掉的巨魔","只")
N("sand dune", GEN_N, "沙丘","个")
N("Magic Mirror", GEN_N, "魔镜","面")
N("Cloud of Mirage", GEN_N, "幻影云","块")
N("Thumper", GEN_N, "振鼓","面")
N("Bonfire", GEN_N, "篝火","道")
N("ancient grave", GEN_N, "古墓","座")
N("fresh grave", GEN_N, "新坟","座")
N("column", GEN_N, "石柱","根")
N("lake of sulphur", GEN_N, "硫磺湖","片")
N("lake", GEN_N, "湖泊","片")
N("frozen lake", GEN_N, "冰冻湖","片")
N("chasm", GEN_N, "裂缝","条")
N("big tree", GEN_N, "大树","棵")
N("tree", GEN_N, "树","棵")

// LANDS
// =====

N("Great Wall", GEN_N, "长城","")
N("Crossroads", GEN_N, "交叉路口","")
N("Desert", GEN_N, "沙漠","")
N("Icy Land", GEN_N, "冰霜之地","")
N("Living Cave", GEN_N, "生机洞穴","")
N("Jungle", GEN_N, "丛林","")
N("Alchemist Lab", GEN_N, "炼金实验室","")
N("Mirror Land", GEN_N, "镜之地","")
N("Graveyard", GEN_N, "墓地","")
N("R'Lyeh", GEN_N, "拉莱耶","")
N("Hell", GEN_N, "地狱","")
N("Cocytus", GEN_N, "科赛特斯","")
N("Land of Eternal Motion", GEN_N, "永动之地","")
N("Dry Forest", GEN_N, "干燥森林","")
N("Game Board", GEN_N, "棋盘","")

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

S("You kill %the1.", "你击杀了%1。")
S("You would be killed by %the1!", "你会被%1杀掉！")
S("%The1 would get you!", "%1会抓到你！")
S("%The1 destroys %the2!", "%1摧毁了%2！")
S("%The1 eats %the2!", "%1吃掉了%2！")
S("The ivy destroys %the1!", "常春藤摧毁了%1！")
S("%The1 claws %the2!", "%1爪击%2！")
S("%The1 scares %the2!", "%1恐吓%2！")
S("%The1 melts away!", "%1融化了！")
S("%The1 stabs %the2.", "%1戳刺%2。")
S("You stab %the1.", "你戳刺%1。")

S("You cannot attack %the1 directly!", "你不能直接攻击%1！")
S("Stab them by walking around them.", "在其周围移动进行戳刺。")

S("You feel more experienced in demon fighting!", "你觉得自己在与恶魔战斗方面更有经验了！")
S("Cthulhu withdraws his tentacle!", "克苏鲁缩回他的触手！")
S("The sandworm explodes in a cloud of Spice!", "沙虫爆炸作一片香料云！")
S("%The1 is confused!", "%1混乱了！")
S("%The1 raises some undead!", "%1召唤了一些不死生物！")
S("%The1 throws fire at you!", "%1向你投掷烈焰！")
S("%The1 shows you two fingers.", "%1对你亮出两根手指。")
S("You wonder what does it mean?", "你想那是什么意思。")
S("%The1 shows you a finger.", "%1对你亮出一根手指。")
S("You think about possible meanings.", "你思考这一举动可能的含义。")
S("%The1 moves his finger downwards.", "%1将手指向下移动。")
S("Your brain is steaming.", "你感觉大脑要在飞速运转中烧化了。")
S("%The1 destroys %the2!", "%1摧毁了%2！")
S("You join %the1.", "你加入了%1。")
S("Two of your images crash and disappear!", "你的两个镜像相撞消失！")
S("%The1 breaks the mirror!", "%1打碎了镜子！")
S("%The1 disperses the cloud!", "%1拨散云雾！")
S("You activate the Flash spell!", "你启动闪光咒语！")
S("You activate the Lightning spell!", "你启动闪电咒语！")
S("Ice below you is melting! RUN!", "你身下的冰层在融化！快跑！")
S("This spot will be burning soon! RUN!", "该处即将燃起火焰！快跑！")
S("The floor has collapsed! RUN!", "地板塌陷了！快跑！")
S("You need to find the right Key to unlock this Orb of Yendor!",
  "你需要找到正确的钥匙才能解锁这个Yendor法球！")
S("You fall into a wormhole!", "你落入一处虫洞！")
S("You activate %the1.", "你启动%1。")
S("No room to push %the1.", "没有推动%1的空间。")
S("You push %the1.", "你推动%1。")
S("You start chopping down the tree.", "你开始砍树。")
S("You chop down the tree.", "你将树砍倒。")
S("You cannot attack Sandworms directly!", "你不能直接攻击沙虫！")
S("You cannot attack Tentacles directly!", "你不能直接攻击触手！")
S("You cannot defeat the Shadow!", "你无法击败阴影！")
S("You cannot defeat the Greater Demon yet!", "你目前还无法击败高等恶魔！")
S("That was easy, but groups could be dangerous.", "小菜一碟，但成群时可能会有危险。")
S("Good to know that your fighting skills serve you well in this strange world.", "幸好你的战斗技能在这个奇怪的世界里依旧有用。")
S("You wonder where all these monsters go, after their death...", "你在想这些怪物死掉之后都去了什么地方……")
S("You feel that the souls of slain enemies pull you to the Graveyard...", "你感到你杀掉怪物的灵魂将你引向墓地……")
S("Wrong color!", "颜色不对！")
S("You cannot move through %the1!", "你无法穿过%1！")
S("%The1 would kill you there!", "%1会把你杀死！")
S("Wow! %1! This trip should be worth it!", "哇哦，%1！真是不虚此行！")
S("For now, collect as much treasure as possible...", "眼下，尽可能多地收集宝藏吧……")
S("Prove yourself here, then find new lands, with new quests...", "在此地证明你的价值，然后寻找新的地域，面对新的挑战……")
S("You collect your first %1!", "你收集到了你的第一%m1%1！")
S("You have found the Key! Now unlock this Orb of Yendor!", "你找到了钥匙！解锁这个Yendor法球吧！")
S("This orb is dead...", "这个法球没有反应……")
S("Another Dead Orb.", "又一个失效的法球。")
S("You have found %the1!", "你找到了%1！")
S("You feel that %the2 become%s2 more dangerous.", "你感到%2变得更加危险。")
S("With each %1 you collect...", "随你收集的%1增长……")
S("Are there any magical orbs in %the1?...", "不知在%1是否有法球的存在？……")
S("You feel that %the1 slowly become%s1 dangerous...", "你感到%1慢慢变得危险起来……")
S("Better find some other place.", "最好离开去其他什么地方。")
S("You have a vision of the future, fighting demons in Hell...", "你预见到未来，在地狱与恶魔战斗……")
S("With this Elixir, your life should be long and prosperous...", "有了这灵药，你的生命将会漫长富足……")
S("The Necromancer's Totem contains hellish incantations...", "死灵法师的图腾中含有地狱的咒文……")
S("The inscriptions on the Statue of Cthulhu point you toward your destiny...", "克苏鲁雕像上的铭文将你指引向你的命运……")
S("Still, even greater treasures lie ahead...", "更多的宝藏依然在前方等待着你……")
S("You collect %the1.", "你收集到一%m1%1。")
S("CONGRATULATIONS!", "恭喜！")
S("Collect treasure to access more different lands...", "收集宝藏才能去往更多地域……")
S("You feel that you have enough treasure to access new lands!", "你感到你的宝藏足够让你去往新的地域！")
S("Collect more treasures, there are still more lands waiting...", "收集更多的宝藏，还有很多地域等待着你……")
S("You feel that the stars are right, and you can access R'Lyeh!", "你感到群星正居其位，可以去往拉莱耶了！")
S("Kill monsters and collect treasures, and you may get access to Hell...", "击杀怪物，收集宝藏，你也许能去往地狱……")
S("To access Hell, collect %1 treasures each of %2 kinds...", "想要去往地狱，你需要收集%2种宝藏各%1个……")
S("Abandon all hope, the gates of Hell are opened!", "放弃一切希望，地狱之门大开！")
S("And the Orbs of Yendor await!", "Yendor法球在此等候！")
S("You switch places with %the1.", "你与%1交换位置。")
S("You rejoin %the1.", "你加入%1。")
S("The mirror shatters!", "镜子破碎了！")
S("The cloud turns into a bunch of images!", "云雾变成了一群镜像！")
S("The slime reacts with %the1!", "黏液与%1发生反应！")
S("You drop %the1.", "你丢下%1。")
S("You feel great, like a true treasure hunter.", "你感觉棒极了，自己像个真正的寻宝人。")
S("Your eyes shine like gems.", "你的双眼如宝石般闪耀。")
S("Your eyes shine as you glance at your precious treasures.", "你的双眼在你瞥视自己的珍宝时闪闪发光。")
S("You glance at your great treasures.", "你瞥视自己的大量宝藏。")
S("You glance at your precious treasures.", "你瞥视自己的许多宝藏。")
S("You glance at your precious treasure.", "你瞥视自己的宝藏。")
S("Your inventory is empty.", "你的物品栏为空。")
S("You teleport to a new location!", "你传送到了新的位置！")
S("Could not open the score file: ", "无法打开得分文件：")
S("Game statistics saved to %1", "游戏统计数据存储至%1")
S("Game loaded.", "游戏已读取。")
S("You summon some Mimics!", "你召唤了一些模仿者！")
S("You summon a golem!", "你召唤了一个魔像！")
S("You will now start your games in %1", "此后你将在%1开始游戏")
S("Activated the Hyperstone Quest!", "双曲石任务已启动！")
S("Orb power depleted!", "法球能量耗尽！")
S("Orbs summoned!", "已召唤法球！")
S("Orb power gained!", "获得法球能量！")
S("Dead orbs gained!", "获得失效法球！")
S("Orb of Yendor gained!", "获得Yendor法球！")
S("Treasure gained!", "获得宝藏！")
S("Lots of treasure gained!", "获得许多宝藏！")
S("You summon a sandworm!", "你召唤了一条沙虫！")
S("You summon an Ivy!", "你召唤了一株常春藤！")
S("You summon a monster!", "你召唤了一个怪物！")
S("You summon some Thumpers!", "你召唤了一些振鼓！")
S("You summon a bonfire!", "你召唤了一处篝火！")
S("Treasure lost!", "失去宝藏！")
S("Kills gained!", "获得击杀数！")
S("Activated Orb of Safety!", "安全法球已启动！")
S("Teleported to %1!", "传送至%1！")

S("Welcome to HyperRogue", "欢迎来到HyperRogue")
S(" for Android", " Android版")
S(" for iOS", " iOS版")
S("! (version %1)\n\n", "! (版本号 %1)\n\n")
S(" (press ESC for some hints about it).", " （按ESC获取一些提示）。")
S("Press 'c' for credits.", "按“c”查看鸣谢")
S("game design, programming, texts and graphics by Zeno Rogue <zeno@attnam.com>\n\n",
  "游戏设计、程序、文本及图像由Zeno Rogue制作 <zeno@attnam.com>\n\n")
S("add credits for your translation here", "中文翻译：我叫超威蓝猫；程序：1iuh 欢迎加群144321649\n\n")
S(" (touch to activate)", " （点击启动）")
S(" (expired)", " （已过期）")
S(" [%1 turns]", " [回合：%1]")
S(", you", "，你")
S("0 = Klein model, 1 = Poincaré model", "0 = 克莱恩模型, 1 = 庞加莱模型")
S("you are looking through it!", "你正在使用该视角！")
S("simply resize the window to change resolution", "改变窗口大小即可调整分辨率")
S("[+] keep the window size, [-] use the screen resolution", "[+] 保持窗口尺寸，[-] 使用屏幕分辨率")
S("+5 = center instantly, -5 = do not center the map", "+5 = 立即居中，-5 = 不居中")
S("press Space or Home to center on the PC", "在PC端按空格或Home居中")
S("You need special glasses to view the game in 3D", "你需要特殊的眼镜来在3D模式下游玩")
S("You can choose one of the several modes", "你可以选择几种模式之一")
S("ASCII", "ASCII")
S("black", "黑色")
S("plain", "平面")
S("Escher", "埃舍尔")
S("items only", "仅物品")
S("items and monsters", "物品与怪物")
S("no axes", "无坐标轴")
S("auto", "自动")
S("light", "轻")
S("heavy", "重")
S("The axes help with keyboard movement", "坐标轴对使用键盘移动有帮助")
S("Config file: %1", "设置文件：%1")
S("joystick mode: automatic (release the joystick to move)", "摇杆模式：自动（放开摇杆移动）")
S("joystick mode: manual (press a button to move)", "摇杆模式：手动（按按钮移动）")
S("Reduce the framerate limit to conserve CPU energy", "降低帧率限制，节约CPU能耗")
S("Press F1 or right click for help", "按F1或鼠标右键查看帮助")
S("No info about this...", "没有相关信息……")
S("Press Enter or F10 to save", "按回车或F10存档")
S("Press Enter or F10 to quit", "按回车或F10退出")
S("or 'r' or F5 to restart", "或按“r”或F5重新开始")
S("or 't' to see the top scores", "或按“t”查看高分榜")
S("or another key to continue", "或按其他键继续")
S("It is a shame to cheat!", "作弊，真丢人！")
S("Showoff mode", "炫耀模式")
S("Quest status", "任务状态")
S("GAME OVER", "游戏结束")
S("Your score: %1", "你的得分：%1")
S("Enemies killed: %1", "击杀敌人数：%1")
S("Orbs of Yendor found: %1", "取得的Yendor法球数：%1")
S("Collect %1 $$$ to access more worlds", "收集%1 $$$以去往更多地域")
S("Collect at least %1 treasures in each of %2 types to access Hell", "收集%2种宝藏至少各%1个以去往地狱")
S("Collect at least %1 Demon Daisies to find the Orbs of Yendor", "收集至少%1个恶魔雏菊以取得Yendor法球")
S("Hyperstone Quest: collect at least %3 %1 in %the2", "双曲石任务：在%2收集至少%3%m1%1")
S("Hyperstone Quest completed!", "双曲石任务完成！")
S("Look for the Orbs of Yendor in Hell or in the Crossroads!", "在地狱或交叉路口寻找Yendor法球！")
S("Unlock the Orb of Yendor!", "解锁Yendor法球！")
S("Defeat %1 enemies to access the Graveyard", "击败%1个敌人以去往墓地")
S("(press ESC during the game to review your quest)", "（在游戏中按ESC检视你的任务）")
S("you have cheated %1 times", "你作弊了%1次")
S("%1 turns (%2)", "回合：%1 （%2）")
S("last messages:", "最近的信息：")
S("time elapsed", "时间")
S("date", "日期")
S("treasure collected", "收集的宝藏")
S("total kills", "总击杀")
S("turn count", "回合数")
S("cells generated", "生成的地块")
S("t/left/right - change display, up/down - scroll, s - sort by", "t/左/右 - 改变显示， 上/下 - 翻页， s - 排序")
S("kills", "击杀")
S("time", "时间")
S("ver", "版本")
S("SORT", "排序")
S("PLAY", "开始")
S("Your total wealth", "你的总财富")
S("treasure collected: %1", "收集的宝藏数：%1")
S("objects found: %1", "发现的物品数：%1")
S("orb power: %1", "法球能量：%1")
S(" (click to drop)", " （点击丢下）")
S("You can also scroll to the desired location and then press 't'.", "你也可以移动选中至目标地域，然后按“t”。")
S("Thus, it is potentially useful for extremely long games, which would eat all the memory on your system otherwise.\n",
  "因此，这也许对超长游戏有一定帮助，不会浪费你系统太多的内存。\n")
S("You can touch the Dead Orb in your inventory to drop it.", "你可以点击物品栏中的失效法球来将其丢下。")
S("This might be useful for Android devices with limited memory.", "对内存有限的Android设备，这也许有所帮助。")
S("You can press 'g' or click them in the list to drop a Dead Orb.", "你可以按“g”或在列表中点击失效法球来将其丢下。")
S("frames per second", "帧率")
S("monsters killed: %1", "击杀数：%1")
S("Drawing %1 (layer %2), F1 for help", "绘制 %1 （层 %2）, F1 - 帮助")
S("hepta floor", "七边形地板")
S("hexa floor", "六边形地板")
S("character", "角色")
S("ESC for menu/quest", "ESC - 菜单/任务")
S("vector graphics editor", "矢量图形编辑器")
S("cheat mode", "作弊模式")
S("heptagonal game board", "七边形棋盘")
S("triangular game board", "三角形棋盘")
S("HyperRogue game board", "HyperRogue棋盘")
S("first page [Space]", "第一页 [空格]")
S("Configuration:", "设置：")
S("video resolution", "显示分辨率")
S("fullscreen mode", "全屏模式")
S("animation speed", "动画速度")
S("dist from hyperboloid ctr", "双曲体中心距离")
S("scale factor", "缩放参数")
S("wall display mode", "墙体显示模式")
S("monster display mode", "怪物显示模式")
S("cross display mode", "交叉显示模式")
S("background music volume", "背景音乐音量")
S("OFF", "关")
S("ON", "开")
S("distance between eyes", "双眼间距")
S("framerate limit", "帧率限制")
S("joystick mode", "摇杆模式")
S("automatic", "自动")
S("manual", "手动")
S("language", "语言")
S("EN", "CN")
S("player character", "玩家角色")
S("male", "男")
S("female", "女")
S("use Shift to decrease and Ctrl to fine tune ", "按Shift减少，Ctrl微调")
S("(e.g. Shift+Ctrl+Z)", " （如Shift+Ctrl+Z）")
S("the second page [Space]", "第二页 [空格]")
S("special features [Space]", "特性 [空格]")
S("see the help screen", "检视帮助")
S("save the current config", "保存当前设置")
S("(v) config", "(v) 设置.")
S("Screenshot saved to %1", "截图保存至%1")
S("You need an Orb of Teleport to teleport.", "需要拥有传送法球才能传送。")
S("Use arrow keys to choose the teleport location.", "使用方向键选择传送目标。")
S("openGL mode enabled", "OpenGL模式开启")
S("openGL mode disabled", "OpenGL模式关闭")
S("openGL & antialiasing mode", "OpenGL与抗锯齿模式")
S("anti-aliasing enabled", "抗锯齿开启")
S("anti-aliasing disabled", "抗锯齿关闭")
S("You activate your demonic powers!", "你启动你的恶魔之力！")

// Steam achievement messages
// --------------------------

S("New Achievement:", "新成就：")
S("Your total treasure has been recorded in the Steam Leaderboards.", "你的总宝藏数已记录至Steam排行榜。")
S("Congratulations!", "恭喜！")
S("You have improved your total high score and %1 specific high scores!", "你更新了你的总最高分及%1个最高分！")
S("You have improved your total and '%1' high score!", "你更新了你的总最高分及“%1”最高分！")
S("You have improved your total high score on Steam. Congratulations!", "你更新了你在Steam的总最高分！恭喜！")
S("You have improved %1 of your specific high scores!", "你更新了你的%1个最高分！")
S("You have improved your '%1' high score on Steam!", "你更新了你在Steam的“%1”最高分！恭喜！")
S("You have collected 10 treasures of each type.", "你收集了每种宝藏各10个。")
S("This is your first victory!", "这是你的首次胜利！")
S("This has been recorded in the Steam Leaderboards.", "已记录至Steam排行榜。")
S("The faster you get here, the better you are!", "来得越快，你的水平就越高！")
S("You have improved both your real time and turn count. Congratulations!", "你更新了你的实时及回合数记录。恭喜！")
S("You have used less real time than ever before. Congratulations!", "你更新了最短实际时间的记录。恭喜！")
S("You have used less turns than ever before. Congratulations!", "你更新了最短回合数的记录。恭喜！")

// help texts
// ----------

// These are separated into multiple lines just for convenience,
// you don't have to follow.

S(
 "You have been trapped in a strange, non-Euclidean world. Collect as much treasure as possible "
 "before being caught by monsters. The more treasure you collect, the more "
 "monsters come to hunt you, as long as you are in the same land type. The "
 "Orbs of Yendor are the ultimate treasure; get at least one of them to win the game!",

 "你被困在一个奇异的非欧几里得世界中。在被怪物抓住之前，尽可能多地收集宝藏。你在同一类型的土地上收集的宝藏越多，就会有越多的怪物来追杀你。Yendor法球是终极宝藏；至少获得一个才能赢得游戏！"
 )

S(
 "You can fight most monsters by moving into their location. "
 "The monster could also kill you by moving into your location, but the game "
 "automatically cancels all moves which result in that.\n\n",

 "你可以通过走到怪物的位置与大多数怪物进行战斗。怪物也可以通过走到你的位置击杀你，但游戏会自动阻止你做出这样的行动。\n\n")

S(
 "Usually, you move by touching somewhere on the map; you can also touch one "
 "of the four buttons on the map corners to change this (to scroll the map "
 "or get information about map objects). You can also touch the "
 "numbers displayed to get their meanings.\n",
 "一般而言，你可以通过点击地图上的某处移动到那里；你也可以点击地图四角的按钮之一切换（拖动地图，或者查看地图上物体的信息）。你也可以点击显示的数字来查看其含义。\n")

S("Move with mouse, num pad, qweadzxc, or hjklyubn. Wait by pressing 's' or '.'. Spin the world with arrows, PageUp/Down, and Space. "
 "To save the game you need an Orb of Safety. Press 'v' for the main menu (configuration, special modes, etc.), ESC for the quest status.\n\n",
 "以鼠标、小键盘、qweadzxc或hjklyubn进行移动。按“s”或“.”等待。以方向键、PageUp/Down或空格键旋转地图。若想保存游戏，你需要找到安全法球。按“v”查看主菜单（设置、特殊模式等），ESC查看任务状态。\n\n")

S("See more on the website: ", "在网页查看更多：")

S("special thanks to the following people for their bug reports, feature requests, porting, and other help:\n\n%1\n\n",
  "特别感谢以下人员的错误反馈、功能请求、移植及其他帮助：\n\n%1\n\n")

S(
 "The total value of the treasure you have collected.\n\n"
 "Every world type contains a specific type of treasure, worth 1 $$$; "
 "your goal is to collect as much treasure as possible, but every treasure you find "
 "causes more enemies to hunt you in its native land.\n\n"
 "Orbs of Yendor are worth 50 $$$ each.\n\n",

  "你已收集宝藏的总价值。\n\n"
  "每种类型的地域包含一种特殊的宝藏，价值 1 $$$；"
  "你的目标是尽可能多地搜集宝藏，但你找到的每个宝藏"
  "都会令其原生地有更多的敌人追猎你。\n\n"
  "每个Yendor法球价值50 $$$。\n\n")

S(
 "The higher the number, the smoother the animations in the game. "
 "If you find that animations are not smooth enough, you can try "
 "to change the options ",
 "数值越高，游戏内的动画越流畅。若你感觉动画还不够流畅，你可以尝试更改设置。")

S("(Menu button) and select the ASCII mode, which runs much faster. "
 "Depending on your device, turning the OpenGL mode on or off might "
 "make it faster, slower, or cause glitches.",
 "(菜单按钮)，选中ASCII模式，提高运行速度。取决于你设备的具体情况，开关OpenGL模式可能会使游戏变快、变慢，或者产生故障。"
 )

S("(in the MENU). You can reduce the sight range, this should make "
 "the animations smoother.",
 "(在菜单中)。你可以改变视野范围，这应该会让动画变得更流畅。")

S("(press v) and change the wall/monster mode to ASCII, or change "
 "the resolution.",
 "(按“v”) 将墙壁/怪物显示模式改为ASCII，或更改分辨率。")

S(
 "In this mode you can draw your own player character and floors. "
 "Mostly for the development purposes, but you can have fun too.\n\n"
 "f - floor, p - player (repeat 'p' for layers)\n\n"
 "n - new shape, u - copy the 'player body' shape\n\n"
 "1-9 - rotational symmetries, 0 - toggle axial symmetry\n\n"
 "point with mouse and: a - add point, m - move nearest point, d - delete nearest point, c - nearest point again, b - add after nearest\n\n"
 "s - save in C++ format (but cannot be loaded yet without editing source)\n\n"
 "z - zoom, o - Poincaré model\n",

 "在该模式下你可以绘制自己的角色与地板。"
 "主要用于开发，不过你也可以用这个功能找点乐子。\n\n"
 "f - 地板， p - 玩家 （重复按“p”切换图层）\n\n"
 "n - 新形状，u - 复制“玩家躯体”形状\n\n"
 "1-9 - 旋转对称，0 - 切换轴对称\n\n"
 "鼠标指向时：a - 添加点，m - 移动最近的点， d - 删除最近的点，c - 重复最近的点，b - 在最近的点后添加\n\n"
 "s - 以C++格式保存 （不修改源码暂时无法读取）\n\n"
 "z - 缩放，o - 庞加莱模型\n")

S(
 "These huge monsters normally live below the sand, but your movements have "
 "disturbed them. They are too big to be slain with your "
 "weapons, but you can defeat them by making them unable to move. "
 "This also produces some Spice. They move two times slower than you.",

 "这些巨大的怪物一般居于沙下，但你在此地的行动打搅了它们的安眠。它们大到无法用你的武器击杀，但你可以通过让它们无法移动来击败它们。这样会产生一些香料。它们的移动速度是你的一半。"
 )

S("The tentacles of Cthulhu are like sandworms, but longer. "
 "They also withdraw one cell at a time, instead of exploding instantly.",

 "克苏鲁的触手就像沙虫，只不过更长。它们被击败时每回合只会缩回去一格，不会立即爆炸。")

S(
 "A huge plant growing in the Jungle. Each Ivy has many branches, "
 "and one branch grows per each of your moves. Branches grow in a clockwise "
 "order. The root itself is vulnerable.",
 "一株在丛林生长的巨大植物。每株常春藤都有许多枝条，你每次行动时都会有一根枝条生长。枝条以顺时针顺序生长。根系本身非常脆弱。\n")

S("The Alchemists produce magical potions from pools of blue and red slime. You "
 "can go through these pools, but you cannot move from a blue pool to a red "
 "pool, or vice versa. Pools containing items count as colorless, and "
 "they change color to the PC's previous color when the item is picked up. "
 "Slime beasts also have to keep to their own color, "
 "but when they are killed, they explode, destroying items and changing "
 "the color of the slime and slime beasts around them.",
 "炼金术士以红蓝黏液池生产魔法药水。你可以穿过这些池子，但你不能从蓝色的池子走进红色的池子，反之亦然。含有物品的池子视为无色，在你捡起物品时变成玩家之前的颜色。黏液兽也需要在其对应的颜色中移动，但它们被杀死时会爆炸，破坏物品并将周围的黏液与黏液兽染成自己的颜色。\n")

S(
 "These creatures are slow, but very powerful... more powerful than you. "
 "You need some more experience in demon fighting before you will be able to defeat them. "
 "Even then, you will be able to slay this one, but more powerful demons will come...\n\n"
 "Each 10 lesser demons you kill, you become powerful enough to kill all the greater "
 "demons on the screen, effectively turning them into lesser demons.",

 "这些生物行动迟缓，但非常强力……比你更强。你需要更多与恶魔战斗的经验才能击败它们。即使如此，你也只能杀掉这一个，但更强大的恶魔总会找上你……\n\n"
 "你每杀掉10个低等恶魔，就会强大到足以杀掉所有屏幕中的高等恶魔；效果等同于把这些恶魔变成低等恶魔。")

S("These creatures are slow, but they often appear in large numbers.",
  "这些生物行动迟缓，但常成群出现。")

S("A big monster from the Living Caves. A dead Troll will be reunited "
 "with the rocks, causing some walls to grow around its body.",
 "来自生机洞穴的庞大怪物。死掉的巨魔会回归岩石，使得一些墙壁沿其残躯周围生长。")

S("Huge, impassable walls which separate various lands.",
  "无法通过的巨大墙壁，分隔不同地域。")

S(
 "This cave contains walls which are somehow living. After each turn, each cell "
 "counts the number of living wall and living floor cells around it, and if it is "
 "currently of a different type than the majority of cells around it, it switches. "
 "Items count as three floor cells, and dead Trolls count as five wall cells. "
 "Some foreign monsters also count as floor or wall cells.\n",
 "这洞穴的墙壁不知怎么是活的。在每回合后，每格会分别统计周围的生机墙与生机地板的数目，随后若它目前不为周围数目较多的那一种，它就变成对应种类的格子。物品计为三个地板格，死巨魔计为五个墙格。有些外来的怪物也会被计作地板或墙。\n")

S(
 "This forest is quite dry. Beware the bushfires!\n"
 "Trees catch fire on the next turn. The temperature of the grass cells "
 "rises once per turn for each fire nearby, and becomes fire itself "
 "when its temperature has risen 10 times.\n"
 "You can also chop down the trees. Big trees take two turns to chop down.",
 "这处森林相当干燥。小心林火！\n"
 "树木着火需要一回合。草格的温度随周围每格火焰提高一次，在温度提高10次后被引燃。\n"
 "你也可以将树砍倒。大树需要两个回合来砍倒。"
 )

S("A big and quite intelligent monster living in the Icy Land.",
  "居住在冰霜之地的大而聪慧的怪物。")

S(
 "A nasty predator from the Icy Land. Contrary to other monsters, "
 "it tracks its prey by their heat.",
 "来自冰霜之地的凶恶捕食者。与其他怪物不同，它依靠热量追踪猎物。")

S("Rangers take care of the magic mirrors in the Land of Mirrors. "
 "They know that rogues like to break these mirrors... so "
 "they will attack you!",
 "游侠照管镜之地的魔镜。他们知道游荡者喜欢打碎这些镜子……所以他们会直接攻击你！")

// TODO update translation
S("A nasty creature that lives in caves. They don't like you for some reason.",
 "生活在洞穴中的凶恶生物。不知为什么它们不喜欢你。")

S("A tribe of men native to the Desert. They have even tamed the huge Sandworms, who won't attack them.",
  "沙漠的部落原住民。他们甚至驯服了巨大的沙虫，使得沙虫不会攻击他们。")

S("This giant ape thinks that you are an enemy.", "这只巨猿觉得你是敌人。")

S("A magical being which copies your movements.", "复制你行动的魔法生物。")

S("A magical being which copies your movements. "
 "You feel that it would be much more useful in an Euclidean space.",
 "复制你行动的魔法生物。你感到它在欧几里得空间里会有用得多。")

S("You can summon these friendly constructs with a magical process.",
  "你可以以一种魔法规程召唤这些友好的造物。")

S("A majestic bird, who is able to fly very fast.",
  "威武的大鸟，能以很快的速度飞行。")

S("A monster who is able to live inside the living cave wall.",
  "一种生活在生机洞穴墙壁之内的怪物。")

S("A typical Graveyard monster.", "寻常的墓地怪物。")

S("A typical monster from the Graveyard, who moves through walls.\n\n"
  "There are also wandering Ghosts. They will appear "
  "if you do not explore any new places for a long time (about 100 turns). "
  "They can appear anywhere in the game.",
  "寻常的墓地怪物，可以穿墙移动。\n\n"
  "也存在游荡的鬼魂，它们在你长时间不探索新地域后出现（大约100回合）。它们会出现在游戏中的任何地方。"
  )

S("Necromancers can raise ghosts and zombies from fresh graves.",
  "死灵法师可以从新坟唤起鬼魂与僵尸。")

S("A creepy monster who follows you everywhere in the Graveyard and the Cursed Canyon.",
  "在墓地与诅咒谷中不断追杀你的阴森怪物。") //TODO UPDATE

S("People worshipping Cthulhu. They are very dangerous.",
  "信仰克苏鲁的人。他们非常危险。")

S("People worshipping Cthulhu. This one is especially dangerous, "
 "as he is armed with a weapon which launches fire from afar.",
 "信仰克苏鲁的人。这一个尤其危险，因为他装备了可以在远处发射火焰的武器。")

S("This dangerous predator has killed many people, and has been sent to Cocytus.",
  "这一危险的捕食者杀死过许多人，因而被流放到了科赛特斯。")

S("This white dog is able to run all the time. It is the only creature "
 "able to survive and breed in the Land of Eternal Motion.",
 "这条白狗可以毫不停歇地跑动。这是唯一一种可以在永动之地生存繁殖的生物。")

S("Demons of Hell do not drown when they fall into the lake in Cocytus. "
 "They turn into demonic sharks, enveloped in a cloud of steam.",
 "地狱的恶魔落入科赛特斯的湖中时不会溺死，而是变为包裹于蒸汽云中的恶魔鲨鱼。")

S("These fairies would rather burn the forest, than let you get some Fern Flowers. "
 "The forest is infinite, after all...\n\n"
 "Fire Fairies transform into fires when they die.",
 "这些仙灵宁肯点燃森林也不愿让你取得蕨花。毕竟，森林是无穷大的……\n\n"
 "烈焰仙灵在死后变成火焰。")

S("These warriors of the Forest wield exotic weapons called hedgehog blades. "
 "These blades protect them from a frontal attack, but they still can be 'stabbed' "
 "easily by moving from one place next to them to another.",
 "这些森林战士持有被称作刺猬之刃的异域武器。此种利刃可以保护他们不受正前方而来的攻击，但你仍可轻易通过在相邻他们的两格间移动来进行“背刺”。")

S("This being radiates an aura of wisdom. "
 "It is made of a beautiful crystal, you would love to take it home. "
 "But how is it going to defend itself? Better not to think of it, "
 "thinking causes your brain to go hot...\n\n"
 "Crystal Sages melt at -30 °C, and they can raise the temperature around you from afar.",
 "此物由美丽的水晶构成，散发出智慧之光。你想把它带回家中。但它是怎么自卫的？最好不要想太多，你一想头脑就开始发热……\n\n水晶智者自-30°C起就会开始融化。它们可以隔空令你周围升温。")

S("Cold white gems, found in the Icy Land.", "寒冷的白色宝石，于冰霜之地寻得。")

S("An expensive metal from the Living Caves. For some reason "
 "gold prevents the living walls from growing close to it.",
 "来自生机洞穴的昂贵金属。不知为何，黄金能够阻止生机墙在其周围生长。")

S("A rare and expensive substance found in the Desert. "
 "It is believed to extend life and raise special psychic powers.",
 "一种在沙漠中寻得的稀有而昂贵的物质。据说它能延长寿命、令人获得特殊的灵能。")

S("A beautiful gem from the Jungle.", "来自丛林的美丽宝石。")

S(
 "A wonderful beverage, apparently obtained by mixing red and blue slime. You definitely feel more "
 "healthy after drinking it, but you still feel that one hit of a monster is enough to kill you.",
 "一种美妙的饮品，似乎是通过混合红蓝黏液得来的。你在喝过之后确实感到健康了不少，不过还是没有健康到能抵御怪物的一击。")

S("A piece of a magic mirror, or a mirage cloud, that can be used for magical purposes. Only mirrors and clouds "
 "in the Land of Mirrors leave these.",
 "魔镜或幻影云的一块碎片，可以用于魔法。只有在镜之地中的魔镜与云会留下这种宝藏。")

S("These sinister totems contain valuable gems.",
  "这些险恶的图腾中嵌有昂贵的宝石。")

S("These star-shaped flowers native to Hell are a valuable alchemical component.",
  "这些地狱特产的星形花朵是贵重的炼金原料。")

S("This statue is made of materials which cannot be found in your world.",
  "这一雕像以你的世界中不存在的材料制成。")

S("One of few things that does not cause the floor in the Land of Eternal Motion to collapse. Obviously they are quite valuable.",
  "少数不会让永动之地的地板塌陷的物体之一。显然相当贵重。")

S("Cold blue gems, found in the Cocytus.", "寒冷的蓝色宝石，于科赛特斯寻得。")

S("These bright yellow gems can be found only by those who have mastered the Crossroads.",
  "这些亮黄色的宝石只能被熟稔交叉路口者发现。")

S("That's all you need to unlock the Orb of Yendor! Well... as long as you are able to return to the Orb that this key unlocks...\n\n"
 "Each key unlocks only the Orb of Yendor which led you to it.",
 "有了它你就可以解锁Yendor法球！呃……只要你还能回到这把钥匙能解锁的那个法球那里。\n\n"
 "每一把钥匙都只能解锁将你引向钥匙的Yendor法球。")

S("These orbs can be found in the Graveyard. You think that they were once powerful magical orbs, but by now, their "
 "power is long gone. No way to use them, you could as well simply drop them...\n\n",
 "这些球可以在墓地中找到。你觉得它们过去一定也是魔力强大的法球，但现在，它们的力量早已消散。既然无法使用，不如就这么扔掉吧……\n\n")

S(
 "This wonderful Orb can only be collected by those who have truly mastered this hyperbolic universe, "
 "as you need the right key to unlock it. Luckily, your psychic abilities will let you know "
 "where the key is after you touch the Orb.",
 "这一神奇的法球只属于完全掌握了这个双曲宇宙的规律者，而且需要正确的钥匙来解锁。幸好，你的灵能让你在触碰到这个法球后能够感应出钥匙所在的位置。")

S(
  "This orb can be used to invoke the lightning spell, which causes lightning bolts to shoot from you in all directions.",
  "这个法球可以用于施放闪电咒语，自你起始向每个方向发射出一道闪电。")

S("This orb can be used to invoke a flash spell, which destroys almost everything in radius of 2.",
  "这个法球可以用于施放闪光咒语，摧毁你周身2格范围内的几乎所有东西。")

S("This orb can be used to invoke a wall of ice. It also protects you from fires.",
  "这个法球可以用于唤出一道冰墙，也能用来保护你不受烈焰伤害。")

S("This orb can be used to move faster for some time.",
  "这个法球可以用于让你行动变快一段时间。")

S("This orb can be used to summon friendly golems. It is used instantly when you pick it up.",
  "这个法球可以用于召唤友好的魔像。你捡起它时就会立即使用。")

S("This orb can protect you from damage.", "这个法球可以用于防护伤害。")

S("This orb lets you instantly move to another location on the map. Just click a location which "
 "is not next to you to teleport there. ",
   "这个法球可以让你立即传送至地图上另一处位置。只需要点击一处与你不相邻的格子。")

S("This orb lets you instantly move to a safe faraway location. Knowing the nature of this strange world, you doubt "
 "that you will ever find the way back...\n\n"
 "Your game will be saved if you quit the game while the Orb of Safety is still powered.\n\n"
 "Technical note: as it is virtually impossible to return, this Orb recycles memory used for the world so far (even if you do not use it to save the game). ",
 "这个法球可以将你立即移动至一处遥远的安全地带。想到这个世界诡异的本质，你觉得你会再也找不到回到使用法球位置的路……\n\n"
 "在安全法球还在生效时退出游戏将会保存当前进度。\n\n"
 "技术说明：反正基本没有返回的可能了，这个法球会回收当前世界所占用的内存（不管你有没有用它来存档）。")

S("This orb allows attacking Hedgehog Warriors directly, as well as stabbing other monsters.\n",
  "这个法球可以让你直接攻击刺猬战士，也同时让你可以背刺其他怪物。\n")

S("This flower brings fortune to the person who finds it.\n",
  "这朵花会给发现者带来好运。\n")

S("Ice Walls melt after some time has passed.", "冰墙会在一段时间后融化。")

S("A natural terrain feature of the Desert.", "沙漠中的自然地形。")

S("You can go inside the Magic Mirror, and produce some mirror images to help you.",
  "你可以进入魔镜，产生一些镜像来帮助你。")

S(
 "Tiny droplets of magical water. You see images of yourself inside them. "
 "Go inside the cloud, to make these images help you.",
 "细小的魔法水珠。你在其中见到自己的映像。走入云中，产生一些镜像帮助你。")

S("A device that attracts sandworms and other enemies. You need to activate it.",
  "一种用于吸引沙虫和其他敌人的设备。启动它才会生效。")

S("A heap of wood that can be used to start a fire. Everything is already here, you just need to touch it to fire it.",
  "一堆可以用来生火的木头。东西都在这里了，你只需要一碰就能点起火来。")

S("An ancient grave.", "一座古墓。")

S("A fresh grave. Necromancers like those.", "一座新坟。死灵法师的最爱。")

S("A piece of architecture typical to R'Lyeh.", "拉莱耶的寻常建筑。")

S("An impassable lake in Cocytus.", "科赛特斯中一处无法通行的湖泊。")

S("You can walk on it... but beware.", "你可以在其上行走……但要当心。")

S("It was a floor... until something walked on it.", "本来是一处地板……直到有东西从其上路过。")

S(
 "This land is a quick gateway to other lands. It is very easy to find other lands "
 "from the Crossroads. Which means that you find monsters from most other lands here!\n\n"
 "As long as you have found enough treasure in their native lands, you can "
 "find magical items in the Crossroads. Mirror Land brings mirrors and clouds, "
 "and other land types bring magical orbs.\n\n"
 "A special treasure, Hyperstone, can be found on the Crossroads, but only "
 "after you have found 10 of every other treasure.",

 "这一地域是通往其他地域的快速通道。在交叉路口，你很容易就能发现其他地域的入口。也就是说，你会在这里碰到大多数其他地域的怪物！\n\n"
 "只要你发现足够多其他地域中原生的宝藏，你就会开始在交叉路口发现魔法物品。镜之地会带来魔镜与云，而其他地域会带来法球。\n\n"
 "交叉路口还有一种叫做双曲石的特殊宝藏，但只有在你收集过所有其他类型宝藏各10个后开始出现。")

S("A hot land, full of sand dunes, mysterious Spice, and huge and dangerous sand worms.",
  "一片充满沙丘、神秘香料和巨大危险沙虫的炎热地域。")

S(
 "A very cold land, full of ice walls. Your mere presence will cause these ice walls to "
 "melt, even if you don't want it.",
 "一片非常寒冷的地域，到处都是冰墙。即使你不愿意，你的存在本身也会让这些冰墙开始融化。")

S("A land filled with huge ivy plants and dangerous animals.",
  "一片充满巨大常春藤植物与危险动物的地域。")

S("A strange land which contains mirrors and mirages, protected by Mirror Rangers.",
  "一片奇异的地域，有许多魔镜与幻影，受到镜子游侠的保护。")

S("All the monsters you kill are carried to this strange land, and buried. "
 "Careless Rogues are also carried here...",
 "你所有杀掉的怪物都被带到这片地域掩埋。不小心的游荡者也会被带来……")

S("An ancient sunken city which can be reached only when the stars are right.\n\n"
  "You can find Temples of Cthulhu in R'Lyeh once you collect five Statues of Cthulhu.",
  "一处沉没的远古城市，只有群星各居其位时才能进入。\n\n"
  "在你收集五个克苏鲁雕像后，你可以在拉莱耶中找到克苏鲁神殿。")

S("A land filled with demons and molten sulphur. Abandon all hope ye who enter here!",
  "一片充满恶魔与熔融硫磺的地域。进入者当放弃一切希望！")

S("This frozen lake is a hellish version of the Icy Land. Now, your body heat melts the floor, not the walls.",
  "一片冰冻湖泊，地狱版的冰霜之地。在此，你的体温会融化地板而非墙壁。")

S("A land where you cannot stop, because every piece of floor is extremely unstable. Only monsters who "
 "can run forever are able to survive there, and only phoenix feathers are so light that they do not disturb "
 "the floor.\n",
 "一片你无法停下脚步的地域，因为每一块地板都极其不稳定。只有能够永远运动的怪物才能在此地生存，也只有凤凰羽毛才轻盈到可以不影响到地板。")

S("Affects looks and grammar", "影响外观与称呼")

S("first joystick: movement threshold", "摇杆 1：移动阈限")
S("first joystick: execute movement threshold", "摇杆 1：执行移动阈限")
S("second joystick: pan threshold", "摇杆 2：视角移动阈限")
S("second joystick: panning speed", "摇杆 2：视角移动速度")
S("%The1 is frozen!", "%1被冻结了！")
S("%The1 burns!", "%1燃烧起来！")
S("message flash time", "消息闪烁时间")

S("skin color", "皮肤颜色")
S("weapon color", "武器颜色")
S("hair color", "头发颜色")
S("dress color", "衣着颜色")
S("Shift=random, Ctrl=mix", "Shift=随机，Ctrl=混合")

S("Euclidean mode", "欧几里得模式")
S("Return to the hyperbolic world", "返回双曲世界")
S("Choose from the lands visited this game.", "选择本次游戏中造访过的地域。")
S("Scores and achievements are not", "得分与成就不会")
S("saved in the Euclidean mode!", "在欧几里得模式中得到保存！")

// Android buttons (some are not translated because there are no good short words in Polish)
S("MOVE", "移动")
S("BACK", "返回")
S("DRAG", "拖动")
S("INFO", "信息")
S("MENU", "菜单")
S("QUEST", "任务")
S("HELP", "帮助")
S("NEW", "新游戏")
S("PLAY", "开始")
S("SHARE", "分享")
S("HyperRogue for Android", "HyperRogue Android版")
S("Date: %1 time: %2 s ", "日期：%1 时间：%2 秒")
S("distance: %1\n", "距离：%1\n")
S("Cheats: ", "作弊：")
S("Score: ", "得分：")
S("Kills: ", "击杀：")
S("Retrieving scores from Google Leaderboards...", "自Google排行榜获取得分中……")
S("Scores retrieved.", "得分已获取。")

S("Your total treasure has been recorded in the Google Leaderboards.", "你的总宝藏数已记录至Google排行榜。")
S("You have improved your total high score on Google. Congratulations!", "你更新了你在Google的总最高分！恭喜！")
S("You have improved your '%1' high score on Google!", "你更新了你在Google的“%1”最高分！恭喜！")
S("This has been recorded in the Google Leaderboards.", "已记录至Google排行榜。")

// this text changed a bit:

S("Ever wondered how some boardgame would look on the hyperbolic plane? "
 "I wondered about Go, so I have created this feature. Now you can try yourself!\n"
 "Enter = pick up an item (and score), space = clear an item\n"
 "Other keys place orbs and terrain features of various kinds\n"
 "In the periodic editor, press 0-4 to switch walls in different ways\n",
 "有没有想过，一些桌游在双曲平面上会是什么样？"
 "我想知道围棋该怎么玩，就做了这个功能。现在你也可以试试！\n"
 "回车 = 捡起物品（得分），空格 = 清除物品\n"
 "其他按键用于放置各种法球与地形\n"
 "在周期编辑器中，按0-4切换墙体")

S("Periodic Editor", "周期编辑器")
// also translate this line:
// "In the periodic editor, press 0-4 to switch walls in different ways\n",

S("Collect %1 $$$ to access even more lands", "收集%1 $$$以去往更多地域")

// Emerald Mine
// ------------

N("Emerald Mine", GEN_N, "绿宝石矿","")
N("Pikeman", GEN_N, "长枪兵","名")
N("Flail Guard", GEN_N, "连枷守卫","名")
N("Miner", GEN_N, "矿工","名")
N("Emerald", GEN_N, "绿宝石","枚")

Orb("the Mind", "心灵")

S(
     "Evil people are mining for emeralds in this living cave. "
     "It does not grow naturally, but it is dug out in a regular "
     "pattern, which is optimal according to the evil engineers.",
     "恶人在这处生机洞穴中挖掘绿宝石。绿宝石并不会自然生长出来，而是按一种邪恶工程师所称的“最优”规律采掘而出。")

S(
    "Miners have special tools for dealing with the Living Cave. "
    "When they die, these tools activate, destroying the living cave "
    "around them.",
    "矿工有专门对付生机洞穴的工具。他们死去时，这些工具便会启动，将周围的生机墙破坏掉。")

S(
    "When Pikemen move, they attack all cells which are now adjacent to them. "
    "Luckily, they can be killed in the same way.\n\n"
    "They never move if this would attack their friends.",
    "在长枪兵移动后，他们会攻击所有与他们相邻的格子。幸好，他们也能以相同的方式被击杀。\n\n"
    "他们不会进行会导致攻击到他们朋友的移动。")

S(
    "This guard of the Emerald Mine is wielding a huge flail. "
    "You cannot attack him directly, as the flail would still hit you then. "
    "Luckily, you have learned a trick: if you step away from him, "
    "he will hit himself with the flail!",

    "这一位绿宝石矿的守卫挥舞着巨大的连枷。你不能直接攻击他，因为连枷还是会打到你。幸好，你还会一招：如果你从他旁边走开，他的连枷会砸到自己！")

S("A precious green gem from the Emerald Mines.", "来自绿宝石矿的珍贵绿色宝石。")

S("%The1 pierces %the2!", "%1刺穿了%2！")

S("Make him hit himself by walking away from him.",
  "从他旁边走开以让他击中自己。")

S("This orb allows you to instantly kill a non-adjacent enemy by clicking it. "
    "Each use drains 30 charges.",
  "这个法球可以让你通过点击立即杀死一个不相邻的敌人。每次使用消耗30点充能。")

S("%The1 is immune to mental blasts!", "%1免疫灵能冲击！")
S("You kill %the1 with a mental blast!", "你以灵能冲击杀死了%1！")

// Vineyard
// --------

N("Vineyard", GEN_N, "藤蔓园","")
N("Vine Beast", GEN_N, "藤蔓兽","头")
N("Vine Spirit", GEN_N, "藤蔓精魂","个")
N("vine", GEN_N, "藤蔓","条")
N("partial fire", GEN_N, "部分起火","")
N("Wine", GEN_N, "红酒","瓶")

S(  "This cell is partially on fire.", "这一格有一部分着火了。")

S(
  "The Vineyard is filled with vines. A very dense pattern of straight lines here...\n\n"
  "Vine Beasts and Vine Spirits change vine cells to grass, and vice versa.",
  "藤蔓园长满了藤蔓。密集的直线结构……\n\n"
  "藤蔓兽与藤蔓精魂令藤蔓格与草格相互转化。")

S("A vine is growing here... but only on a half of the cell. How is that even possible?!"
  "Most monsters cannot move from this cell to the cell containing the other half. "
  "Vine spirits can move only to the adjacent cells which are also adjacent to the "
  "other half.",
  "一株藤蔓在此生长……不过只长了半个格子。这怎么可能？！大多数怪物都不能从这一格走到包含另外一半的格子。藤蔓精魂只能移动至同时与另外一半相邻的相邻格子。")

S(
    "A beast made of vines!\n\n"
    "Vine Beasts turn into vines when they die.",
    "藤蔓构成的野兽！\n\n"
    "藤蔓兽在死去时变成藤蔓。")

S(  "A spirit living in the vines!\n\n"
    "Vine Spirits destroy the vines when they die.",
    "居住在藤蔓中的精魂！\n\n"
    "藤蔓精魂死去时摧毁所在的藤蔓。")

S("Wine grown under hyperbolic sun would be extremely prized in your home location.",
  "在双曲阳光下酿成的红酒在你的家乡想必会非常珍贵。")

S("This orb allows one to pass through all kinds of walls and chasms.",
  "这个法球可以让你穿过各种墙壁与裂缝。")

S("You cannot attack through the Vine!",
  "你无法穿过藤蔓攻击！")

Orb("Aether", "以太")

// Dead Caves
// ----------

N("Dead Cave", GEN_N, "死寂洞穴","")
N("Dark Troll", GEN_N, "黑暗巨魔","只")
N("Earth Elemental", GEN_N, "土元素","个")
N("dead wall", GEN_N, "死寂墙","堵")
N("dead floor", GEN_N, "死寂地板","块")
N("rubble", GEN_N, "碎石","堆")
N("Silver", GEN_N, "白银","块")

S("A precious metal from the Dead Caves.", "来自死寂洞穴的昂贵金属。")

S(  "A Troll without the power of Life.", "没有生命力量的巨魔。")

S(
    "A rare unliving construct from the Dead Caves. "
    "It instantly destroys cave walls next to its path, and also leaves "
    "an impassable wall behind it. You suppose that this impassable wall helps it to "
    "escape from some threats. You hope you won't meet these threats...",

    "死寂洞穴中的一种稀有构装造物。它会摧毁前进路线相邻的洞穴墙，并在身后留下无法通过的墙。你料想这无法通过的墙是便于它逃脱某种威胁的。希望你不会碰到这些威胁……"
    )

S("%The1 punches %the2!", "%1捶打%2！")
S("%The1 is destroyed by the forces of Life!", "%1被生命之力摧毁了！")
S(  "Somehow, this cave has not received the spark of Life yet.",
    "不知为何，这处洞穴还未接收到生命的火花。")

// Hive
// ----

N("Hive", GEN_N, "虫巢","")
N("Red Hyperbug", GEN_N, "红超虫","只")
N("Green Hyperbug", GEN_N, "绿超虫","只")
N("Blue Hyperbug", GEN_N, "蓝超虫","只")
N("Royal Jelly", GEN_N, "蜂王浆","团")
N("weird rock", GEN_N, "怪异岩石","块")

Orb("Invisibility", "隐形")

S("A weirdly colored rock. Hyperentomologists claim that the "
  "Hyperbug armies use these rocks to navigate back home after a victorious battle.",
  "颜色怪异的石头。双曲昆虫学家声称超虫在赢得战斗后以这些石头作为路标返回巢穴。")

S("%The1 fights with %the2!", "%1与%2作战！")

S("The Hive is filled with Hyperbugs. They are huge insects which look a bit like "
  "ants, a bit like bees, and a bit like roaches. "
  "They live in circular nests, and an army of Hyperbugs will attack any intruder, "
  "including you and enemy Hyperbugs. Will you manage to get to the "
  "heart of such a nest, and get the precious Royal Jelly?",
  "虫巢中充满了超虫。它们是有点像蚂蚁，有点像蜜蜂，又有点像蟑螂的巨大昆虫。它们居住在圆形的巢穴之内，成群的超虫会攻击一切入侵者，包括你以及敌对的超虫。你能来到这些巢穴的中心，取得珍贵的蜂王浆吗？")

S("This is what Hyperbug Queens eat. Very tasty and healthy.",
  "超虫女王以此为食。既美味又健康。")

S("When you have this Orb, most monsters won't see you, unless "
    "you are standing still, attacking, or picking up items.",
  "你拥有这一法球时，大多怪物无法看到你，除非你站立不动、正在进攻，或在捡起物品。")

Orb("Earth", "大地")

S("This orb lets you go through living walls. It also has powers in some of the other lands.",
  "这个法球使你能够穿过生机墙。在一些其他的地域它也能发挥某些能力。")

// Land of Power
// -------------

N("Land of Power", GEN_N, "力量之地","")
N("Witch Apprentice", GEN_N, "女巫学徒","名")
N("Speed Witch", GEN_N, "速度女巫","名")
N("Flash Witch", GEN_N, "闪光女巫","名")
N("Fire Witch", GEN_N, "火焰女巫","名")
N("Aether Witch", GEN_N, "以太女巫","名")
N("Winter Witch", GEN_N, "寒冬女巫","名")
N("Evil Golem", GEN_N, "邪恶魔像","个")
N("Powerstone", GEN_N, "魔力石","块")
N("crystal cabinet", GEN_N, "水晶柜","座")

S("When you have this Orb, you will leave a trail of fire behind you.",
  "你拥有这个法球时，移动会在身后留下一道火焰。")

S(
    "A Witch without any special powers. But watch out! She will "
    "pick up any basic Orbs on her path, and use their powers.",
    "没有什么特殊能力的女巫。但要当心！她会捡起碰到的基础法球，并使用法球的力量。")

S(  "A Witch with a Speed spell. She moves twice as fast as you. Unless you "
    "have an Orb of Speed too, of course!",
    "具有速度咒语的女巫。她移动的速度是你的两倍。除非你也有一个速度法球！")

S(  "A Witch with a Fire spell. She will leave a trail of fire behind her.",
    "具有火焰咒语的女巫。她会在身后留下一道火焰。")

S(  "A Witch with a Winter spell. She is able to move through fire.",
    "具有寒冬咒语的女巫。她可以穿过火焰。")

S(  "A Witch with an Aether spell. She is able to move through fire and walls.",
    "具有以太咒语的女巫。她可以穿过火焰与墙壁。")

S(  "Somebody has summoned these evil constructs with a magical process.",
    "有人以魔法召唤出了这些邪恶的构造物。")

S(  "The Land of Power is filled with everburning fire, magical Orbs, and guarded by "
    "witches and golems. There are basic orbs lying everywhere, and more prized ones "
    "are kept in crystal cabinets.\n\n"
    "Witches are allowed to use all the powers of the "
    "basic orbs against intruders. These powers never expire, but a Witch "
    "can use only one power at a time (not counting Orbs of Life).\n\n"
    "Witches and Golems don't pursue you into other Lands. Also, most Orb powers "
    "are drained when you leave the Land of Power.",
    "力量之地遍布永燃之火与法球，受到女巫与魔像的守护。基础法球几乎随处可见，更珍贵者则被保存在水晶柜中。\n\n"
    "女巫可以使用法球的力量来对抗入侵者。她们的力量不会消退，但每个女巫同时只能使用一种力量（生命法球除外）。\n\n"
    "女巫与魔像不会追猎你至其他地域。此外，大多数法球的力量会在你离开力量之地时被汲取殆尽。")

S(
    "Witches use these crystal cabinets to protect Powerstones, as well as the more "
    "expensive Orbs. They are partially protected from thieves: they are too strong "
    "to be smashed by conventional attacks, and if you try to steal the item "
    "using an Orb of Aether, your Aether power will be completely drained.",
  "女巫使用这些水晶柜保护魔力石，以及比较昂贵的法球。它们有一定的防盗能力：它们坚韧到无法以常规攻击破坏，而如果你想用以太法球偷走里面的东西，你的以太能量会被汲取一空。")

S(
    "A Stone from the Land of Power. You are not sure what it is exactly, but "
    "as the Powerstones are kept in crystal cabinets, they are surely valuable.",
    "来自力量之地的一块石头。你不太清楚它具体是个什么东西，但既然被保存在水晶柜里，那肯定很值钱。")

Orb("Fire", "烈焰")

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

S("Your Aether power has expired! RUN!", "你的以太能量耗尽了！快跑！")

S("%The1 tricks %the2.", "%1戏弄%2。")
S("You trick %the1.", "你戏弄%1。")

S(  "A Witch with a Flash spell. Very dangerous!\n\nLuckily, she never uses the spell if it "
    "would kill her friends. She could destroy an Evil Golem, though.",
    "具有闪光咒语的女巫。非常危险！\n\n幸好，她不会在可能杀死她的朋友时使用这个咒语。不过她不在乎摧毁邪恶魔像。")

S("Your Aether powers are drained by %the1!", "你的以太能量被%1汲取！")
S("As you leave, your powers are drained!", "你一离开，法球能量就被吸干了！")
S("%The1 picks up %the2!", "%1捡起%2！") // currently only Witches, so only female form

S("You can right click any element to get more information about it.\n\n",
  "你可以右键单击任意元素来查看关于它的更多信息。\n\n")

S("TRANSLATIONWARNING", "")
S("TRANSLATIONWARNING2", "")

S("The Hyperbugs are fighting!", "超虫开始相互战斗！")

S("","") N("",GEN_N,"","")

S("Cheat-changed the display.", "作弊-修改显示")
S("Dead floor, with some rubble.", "死寂地板，有些碎石。")

// Camelot
// -------

N("Camelot", GEN_N, "卡美洛","")
N("wall of Camelot", GEN_N, "卡美洛墙","堵")
N("moat of Camelot", GEN_N, "卡美洛护城河","条")
N("Round Table", GEN_N, "圆桌","张")
N("Knight", GEN_N, "骑士","位")
N("Holy Grail", GEN_N, "圣杯","个")

S(
  "The Knights of the Round Table are the greatest warriors of these lands. "
  "They are not very inventive with names though, as they call each of their "
  "castles Camelot. "
  "You are probably worthy of joining them, but they will surely give you "
  "some quest to prove yourself...\n\n"
  "Each castle contains a single treasure, the Holy Grail, in the center. "
  "The radius of the Round Table is usually 28, but after you find a Holy Grail "
  "successfully, each new castle (and each Round Table) you find will be bigger.",

  "圆桌骑士是这万千地域中最强大的战士。不过他们没什么想象力，把他们所有的城堡都叫做卡美洛。你的武功也许足够加入他们，但他们还是会考验你，让你证明自己的价值……\n\n"
  "每座城堡中只有一件宝藏，也就是圣杯，在最中央处。圆桌的半径一般是28格，但在你找到一个圣杯以后，你发现的每座新的城堡（以及里面的圆桌）都会变得更大。")

S("It would be impolite to land on the table!",
  "踩在桌子上可不礼貌！")
S("You jump over the table!", "你越过圆桌！")
S("That was not a challenge. Find a larger castle!",
  "算不得什么挑战。再找一座更大的城堡！")
S("The Knights congratulate you on your success!",
  "骑士们庆贺你的成功！")
S("The Knights laugh at your failure!",
  "骑士们嘲笑你的失败！")
S("The Knights stare at you!", "骑士们盯着你！")
S("Come on, this is too easy... find a bigger castle!",
  "哎呀，这也太简单了……再找一座更大的城堡！")
S("The Knights wish you luck!", "骑士们祝你好运！")

S("Congratulations! You have found the Holy Grail!",
  "恭喜！你找到了圣杯！")

S("\"I would like to congratulate you again!\"", "“再次祝贺你！”")
S("\"Find the Holy Grail to become one of us!\"", "“找到圣杯就能加入我们的行列！”")
S("\"The Holy Grail is in the center of the Round Table.\"", "“圣杯就在圆桌的正中。”")
S("\"I enjoy watching the hyperbug battles.\"", "“我喜欢看超虫对打。”")
S("\"Have you visited a temple in R'Lyeh?\"", "“你去过拉莱耶的神殿吗？”")
S("\"Nice castle, eh?\"", "“这城堡不错吧，嗯？”")

// Temple
// ------

N("Temple of Cthulhu", GEN_N, "克苏鲁神殿","")
N("big statue of Cthulhu", GEN_N, "克苏鲁大雕像","尊")
N("Grimoire", GEN_N, "巨著","本")
N("Cult Leader", GEN_N, "邪教首领","名")

S("The temple of Cthulhu consists of many concentric circles of columns. "
  "You will surely encounter many Cultists there, who believe that a pilgrimage "
  "to the inner circles will bring them closer to Cthulhu himself, and Grimoires "
  "which surely contain many interesting secrets.\n\n"
  "The circles in the temple of Cthulhu are actually horocycles. They are "
  "infinite, and there is an infinite number of them.",

  "克苏鲁神殿由许多由许多围成同心圆的立柱组成。你自然会在其中碰到许多邪教徒，他们认为向中心朝圣能使他们更加接近克苏鲁本体，以及包含许多惊人秘密的巨著。\n\n"
  "神殿中的这些同心圆其实都是极限圆。它们无穷大，有无穷多这样的圆。"
  )

S("The Grimoires contain many secrets of the Great Old Ones. "
  "Each new inner circle in the Temple of Cthulhu contains new Grimoires, with new secrets. "
  "You hope to read them when you return home, and to learn many things. "
  "The knowledge is valuable to you, but it is rather pointless to try to get "
  "several copies of the same Grimoire...",
  "巨著中含有许多古神的秘密。克苏鲁神殿的每一圈层都有新的巨著，包含着新的秘密。你希望在返回家乡后阅读这些巨著，学习其中的许多内容。知识对你来说很宝贵，不过收集许多份相同的巨著并无太大意义……"
  )

S("These statues of Cthulhu are too large to carry, and they don't look too "
  "valuable anyway. Most monsters will never go through them... they probably have "
  "their reasons. But you can go! When you move into the cell containing "
  "a statue, you push the statue to the cell you left.\n",

  "这些克苏鲁的雕像大到没办法带走，况且它们看起来也不值钱。大多数怪物永远都不愿穿过这些雕像……怪物自有它们的道理吧。不过你并不在乎！你移动到含有雕像的格子时，会把雕像推到你来时的格子去。"
  )

S("These Cultists can push the statues, just like you.",
  "这些邪教徒和你一样，可以推动雕像。")

S("You already have this Grimoire! Seek new tomes in the inner circles.",
  "你已经有这本巨著了！在内圈寻找新的巨著。")

S("You push %the1 behind you!", "你将%1推至身后！")

// More texts
// ----------

S("Your total kills", "你的总击杀数")

S(
  "In most lands, more treasures are generated with each enemy native to this land you kill. "
  "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
  "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
  "but are not counted in the total kill count.",

  "在大多数地域中，你击杀原生于此地的怪物越多，宝藏生成得就越多。此外，取得100个击杀是进入墓地与虫巢的前提。\n\n"
  "友好的生物与怪物的身体部分（比如常春藤）也会在列表中出现，但不计入击杀数。")

S("There must be some temples of Cthulhu in R'Lyeh...",
  "拉莱耶中一定有某种神殿……")

S("You overheard Hedgehog Warriors talking about emeralds...",
  "你偷听到刺猬战士谈及绿宝石……")

S("You overhear miners talking about a castle...",
  "你偷听到矿工们谈及城堡……")

S("A castle in the Crossroads...", "交叉路口中的一座城堡……")

S("You have to escape first!", "你需要先逃出来！")
S("There is not enough space!", "空间不足！")

S("Customize character", "自定义角色")
S("gender", "性别")

//

Orb("the Dragon", "巨龙")

S("This Orb allows you to throw fire, just like the Fire Cultists.\n\n"
  "Each fire drains 5 charges. You are not allowed to throw fire into adjacent cells.",
  "这个法球使你能够像火焰邪教徒一样投掷火焰。\n\n"
  "每次点火消耗5点充能。你不能在相邻的格子点火。"
  )

S("You throw fire!", "你投掷火焰！")

Orb("Trickery", "诡计")

N("Illusion", GEN_N, "幻象","个")

S("This Orb allows you to create illusions of yourself. Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.\n\n"
  "Each illusion takes 5 charges to create, and one extra charge "
  "per turn. You can also click your illusion to take it away, restoring 4 charges.\n\n"
  "If you have both Orb of Teleport and Orb of Trickery, Illusion is cast "
  "first -- you can then teleport on your Illusion to switch places with it.",
  "这个法球能够用于创造出你自身的幻象。幻象与你自己、振鼓或你的伙伴一样，会被大多数敌人攻击。\n\n"
  "制造一个幻象消耗5点充能，每回合额外消耗1点。你也可以点击幻象来将它回收，得到4点充能。\n\n如果你既有传送法球又有幻象法球，幻象法球会优先被施放——你随后可以传送到幻象处，与它交换位置。"
  )

S("Illusions are targeted "
  "by most monsters, just like yourself, Thumpers, and your friends.",
  "幻象与你自己、振鼓或你的伙伴一样，会被大多数敌人攻击。")

S("You create an Illusion!", "你创造出一个幻象！")
S("You take the Illusion away.", "你令幻象消散！")

S("You cannot target that far away!", "你无法指定那么远的目标！")
S("You cannot target that close!", "你无法指定这么近的目标！")
S("Cannot teleport on a monster!", "不能传送到怪物上！")
S("Cannot teleport on an item!", "不能传送到物品上！")
S("Cannot teleport here!", "不能传送到这里！")
S("Cannot cast illusion on an item!", "不能对物品施放幻象！")
S("Cannot cast illusion here!", "不能对这里施放幻象！")
S("Cannot cast illusion on a monster!", "不能对怪物施放幻象！")
S("Cannot throw fire there!", "不能对那里投掷火焰！")

// teleport -> target
// You cannot target that close!

S("or ESC to see how it ended", "按ESC检视这局游戏结束时的状况")
S("high contrast", "高对比度")
S("draw the heptagons darker", "以深色绘制七边形")
S("targetting ranged Orbs Shift+click only",
  "仅以Shift+点击使用远程法球")
S("Shift+F, Shift+O, Shift+T, Shift+L, Shift+U, etc.",
  "Shift+F、Shift+O、Shift+T、Shift+L、Shift+U，等等。")
S("also hold Alt during the game to toggle high contrast",
  "在游戏中长按Alt也可以改变对比度")

// Crossroads II
// -------------

N("Crossroads II", GEN_N, "交叉路口 II","")

S(
  "An alternate layout of the Crossroads. It is more dense and more regular, "
  "although you won't find the castle of Camelot here.",

  "交叉路口的又一种构造。更密集，更规整，不过你不会在这里找到卡美洛的城堡。")

// Caribbean
// ---------

// note: in Polish,
// 'Karaiby' is plural in Polish, should probably fix the grammar for this

N("Caribbean", GEN_N, "加勒比","")

N("Pirate", GEN_N, "海盗","名")
N("Shark", GEN_N, "鲨鱼","条")
N("Parrot", GEN_N, "鹦鹉","只")
N("Pirate Treasure", GEN_N, "海盗宝藏","堆")
N("Compass", GEN_N, "指南针","枚")

N("sea", GEN_N, "海","片")
N("boat", GEN_N, "船","条")
N("island", GEN_N, "岛屿","座")
N("tree", GEN_N, "树","棵")

Orb("Time", "时间")

S("This dangerous sea contains nasty sharks and pirates. ",
  "这片危险的海域中藏有凶恶的鲨鱼与海盗。")

S("The islands of Caribbean are infinite and circular, and "
  "the pirates probably have hidden their treasures somewhere far from the coast.",
  "加勒比海的无穷岛屿呈环形，海盗把他们的珍宝藏在远离海岸之处。")

S(
    "Hyperbolic pirates do not need huge ships, since so many lands to conquest "
    "are so close. These small boats are enough for them.\n\n"
    "Boats allow you to go through water. If you are in a boat, you can move into "
    "a water cell (and the boat will come with you).",

    "双曲的海盗不需要太大的船，因为等待征服的无数土地离得都很近。这些小船就足够了。\n\n"
    "坐船让你能横跨水面。如果你在船上，你可以移动到水格中（船会跟随你移动）。")

S(
    "The forests of Caribbean are too dense to be traversed by humans, "
    "and they are hard to burn. Many colorful parrots can be found there.",
    "加勒比的森林过于茂密，人类无法穿越，也难以燃烧。在这里可以发现许多五彩缤纷的鹦鹉。")

S("Ye wonder where did th' Pirates find all these riches...",
    "你想知道海盗们从哪找到这么多财富的……")

S(
  "The hyperbolic pirates have no use for treasure maps. However, they have found "
  "out that a compass points to the center of the island. So they just go as "
  "far towards the center as they can, and hide their treasure there.",

  "双曲海盗没法用藏宝图。不过，他们发现指南针会指向岛屿的中心。所以他们就能往中央移动多深就走多深，然后把财宝藏在那里。")

S("Just a nasty shark.", "只是一条凶恶的鲨鱼罢了。")

S("Parrots feel safe in the forests of Caribbean, so they "
    "never leave them. But they will help the Pirates by attacking the intruders.",

  "鹦鹉在加勒比的森林中感到安全，所以它们从不离开。不过它们会帮助海盗攻击入侵者。")

S("Just a typical hyperbolic pirate.", "只是一个寻常的双曲海盗。")

S(
    "Normally, the power of most Orbs slowly fades away, even when "
    "you are not actively using them. This Orb prevents this.\n\n"

    "When you have the Orb of Time, Orbs which are not active won't lose their power. "
    "Orbs are considered active if they have a continuous power which has actually "
    "affected something in the last turn.\n\n"

    "Orbs of Shielding remain active after being activated (even if you are no longer "
    "attacked), and Orbs of Time have a bigger cap inside their native Caribbean than "
    "outside.",

    "通常，即使你没有主动使用它们，大多数法球的能量也会慢慢消散。这个法球会阻止它们的能量消散。\n\n"

    "在你拥有时间法球时，未激活的法球不会失去能量。所谓激活是指其上的持续效果在某一回合真的影响到了什么东西。\n\n"

    "护盾法球在启动后一直生效（即使你没有受到攻击）；时间法球在原生的加勒比比起其他地方能量上限更高。")

// Achievements

// Parrot : Find and collect Pirate Treasure.
// Caribbean Shark : Collect 10 Pirate Treasures.
// Pirate : Collect 25 Pirate Treasures.
// Master of Caribbean : Collect 50 Pirate Treasures.

// Red Rock Valley
// ---------------

N("Red Rock Valley", GEN_N, "红岩谷","")

N("Rock Snake", GEN_N, "岩蛇","条")
N("Rock Snake Tail", GEN_N, "岩蛇尾","段")
N("Red Troll", GEN_N, "红巨魔","只")
N("Red Gem", GEN_N, "赤色宝石","枚")

N("rock I", GEN_N, "岩石 I","")
N("rock II", GEN_N, "岩石 II","")
N("rock III", GEN_N, "岩石 III","")

Orb("Space", "恐惧")

S("Not enough power for telekinesis!", "能量不足以使用念动力！")

S("This land contains high rock formations. Most of the valley is at level 0, "
  "while items are found at level 3. It is impossible to gain two or more levels, "
  "or to lose three levels, in a single move, (attacks are possible at any "
  "difference, though). Kill Red Trolls and Rock Snakes to make a cell higher.",

  "此处地域有高高低低的岩石结构。山谷中大多数位置处于高度0，而物品在高度3才能找到。无法一次向上移动2或更多个高度，也无法一次向下移动3个（不过攻击是忽略高低差的）。"
  "击杀红巨魔与岩蛇让一格变高。")

S("A kind of Troll native to the Red Rock Valley.",
  "一种原生于红岩谷的巨魔。")

S(
  "Rock snakes are similar to Sandworms, but they move each turn. However, they "
  "only move on the hexagonal cells. They are also somewhat longer than the Sandworms.",

  "岩蛇与沙虫类似，不过每回合都会移动。幸好，它们只能在六边形格子上移动。它们比沙虫稍大一些。")

S("A gem from the Red Rock Valley.", "来自红岩谷的一种宝石。")

S(
    "This Orb is able to bring faraway items to your location, even if there are "
    "monsters or barriers on the way. The cost of "
    "bringing an item (in charges) equals the square of its distance to you. Contrary "
    "to some other Orbs, usage is not allowed if you have not enough power left.",

  "这个法球可以用来将遥远的物品移动到你的位置，即使途径上有怪物或是障碍。将物品挪来的能量消耗等同于与物品距离的平方。不像其他一些法球，如果能量不足，你就没法使用这个法球。")

// Climber : Find and collect a Red Gem.
// Red Troll : Collect 10 Red Gems.
// Rock Snake : Collect 25 Red Gems.
// Master of Red Rock : Collect 50 Red Gems.

// missing sentences

S("%The1 drowns!", "%1溺水了！")
S("%The1 falls!", "%1跌落了！")

// these were missing from the translation for some reason

S("Hell has these lakes everywhere... They are shaped like evil stars, and filled with burning sulphur.",
  "地狱中到处都是这些湖泊……它们构成邪恶的星形，充满燃烧的硫磺。")

// Hardcore Mode
// -------------

S("hardcore mode", "硬核模式")

S("One wrong move and it is game over!", "走错一步，游戏结束！")
S("Not so hardcore?", "没那么硬核？")

// Shoot'em up Mode
// ----------------

S("shoot'em up mode", "射击模式")
S("Welcome to the Shoot'em Up mode!", "欢迎来到射击模式！")
S("F/;/Space/Enter/KP5 = fire, WASD/IJKL/Numpad = move", "F/;/空格/回车/KP5 = 射击，WASD/IJKL/小键盘 = 移动")

N("Rogue", GEN_N, "游荡者","名")
N("Knife", GEN_N, "小刀","把")
N("Flail", GEN_N, "连枷","把")
N("Fireball", GEN_N, "火球","枚")
N("Tongue", GEN_N, "舌头","根")

S("In the Shoot'em Up mode, you are armed with thrown Knives.",
  "在射击模式中，你装备着飞刀。")
S("A simple, but effective, missile, used by rogues.",
  "游荡者使用的简单而有效的远程武器。")
S("This attack is likely to hit the attacker.",
  "这一攻击很可能击中攻击者自己。")
S("This magical missile burns whatever it hits.", "灼烧一切击中敌人的魔法飞弹。")
S("Some monsters have long tongues, which allow them to attack enemies in nearby cells.",
  "有些怪物长着长长的舌头，可以攻击相邻格子的敌人。")

// modes for the local highscores
// ------------------------------

S(", m - mode: normal", ", m - 普通模式")
S(", m - mode: hardcore only", ", m - 硬核模式")
S(", m - mode: shoot'em up", "m - 射击模式")

// update description for Steam

// (Shoot'em Up mode and two more lands than in the free version, and more are planned!)

// missing lines

S("You would get hurt!", "你会受伤的！")
S("PARTIAL", "部分")

S("Cannot drop %the1 here!", "不能把%1丢在这里！")

// Euclidean scores
// ----------------

S(" (E:%1)", " (E:%1)")

S("You cannot attack Rock Snakes directly!", "你不能直接攻击岩蛇！")

S("\"I am lost...\"", "“我迷路了……”")

S("You are killed by %the1!", "你被%1杀死了！")

// new menu for Version 7.1
// ------------------------

S("(v) menu", "(v) 菜单")
S("return to the game", "返回游戏")
S("or 'v' to see the main menu", "或按“v”查看菜单")

S("HyperRogue %1", "HyperRogue %1")
S("basic configuration", "基础设置")
S("advanced configuration", "高级设置")
S("local highscores", "本地最高分")
S("help", "帮助")
S("restart game", "重新开始")
S("special game modes", "特殊游戏模式")
S("save the game", "保存游戏")
S("quit the game", "退出游戏")
S("review your quest", "检视任务")
S("review the scene", "检视场景")
S("game over screen", "游戏结束显示")
S("continue game", "继续游戏")
S("play the game!", "开始游戏！")

// fixed descriptions for Shmup achievements
// -----------------------------------------

S("You have used less knives than ever before. Congratulations!",
  "你比之前用了更少的飞刀。恭喜！")

S("You have improved both your real time and knife count. Congratulations!",
  "你更新了你的游戏耗时与飞刀数记录。恭喜！")

S("%1 knives (%2)", "飞刀：%1 (%2)")

// temporary
S("No description yet." ,"暂无描述。")

// special case when no Spice is visible
S("The sandworm explodes!", "沙虫爆炸了！")

// Ocean
// -----

// achievements
// Reached Ocean: Find and collect an Amber.
// Albatross: Collect 10 Ambers.
// Amber Albatross: Collect 25 Ambers.
// Master of the Tides: Collect 50 Ambers.

N("Ocean", GEN_N, "海洋","")

S(
    "You can collect some valuable Ambers on the coast, but beware the tides!\n\n"
    "You can also take one of the boats and venture into the Ocean, "
    "to find other coasts, dangerous whirlpools, and islands inhabited by Pirates.",

  "你可以在海岸上收集到珍贵的琥珀，但要小心涨潮！\n\n你也可以坐船深入海洋，寻找其他海岸，危险的漩涡，以及海盗居住的岛屿")

N("Albatross", GEN_N, "信天翁","只")

S(
  "Those large seabirds attack you for some reason. At least they are not "
  "as fast as Eagles...",
  "这些大海鸟不知为什么要攻击你。至少它们不像雄鹰一样迅捷……")

N("stranded boat", GEN_N, "搁浅的船","条")

S(
  "This boat cannot go through the sand. But if you sit inside and "
  "wait for the tide, you will be able to use it to travel through the Ocean.",

  "这艘船不能穿过沙子，但如果你坐在里面等潮水涨起来，你可以用它在海洋中航行。"
  )

N("Amber", GEN_N, "琥珀","枚")

S(
  "When the tide is away, beautiful ambers can be found on the hyperbolic beaches. "
  "Perhaps there used to be a forest here?" ,
  "潮水退去时，美丽的琥珀留在这里的双曲海滩上。也许这里曾经是一片森林？")

Orb("Air", "空气")

S("This Orb allows you to blow your enemies away.\n\n"
  "Click a monster to blow it one cell away. It cannot be used against mimics, ghosts, sharks and other monsters restricted to a specific terrain, and multi-tile monsters.",
  "这个法球使你能将敌人吹走。\n\n点击怪物就能把它吹走一格远。不能用于模仿者、鬼魂、鲨鱼以及其他受限于特殊地形的怪物，还有多格的怪物。"
  )


N("Sea Border", GEN_N, "海疆","道")

S("Border between seas.", "不同海域之间的边界。")

// Whirlpool
// ---------

// achievements
// Escaped Whirlpool: Collect a Pearl, and escape the Whirlpool.
// Vortex Shark: Collect 10 Pearls.
// Vortex Pirate: Collect 25 Pearls.
// Master of the Whirlpool: Collect 50 Pearls.

N("Whirlpool", GEN_N, "漩涡","")

S(
  "Many lost boats are spinning in this whirlpool. Some of them contain treasures "
  "and Pirates.\n\n"
  "Unmanned boats will go one cell clockwise in each turn. Sharks and manned boats "
  "are only allowed to go with the current, that is, towards the center, or clockwise.",

  "许多遗失的船在这处漩涡中旋转。有些船上还有宝藏和海盗。\n\n"
  "无人驾驶的船每回合顺时针移动一格。鲨鱼和有人的船只能顺着水流，也就是向正中前进，或是顺时针移动。"
  )

N("Pearl", GEN_N, "珍珠","颗")

S(
  "You do not know exactly why, but there are valuable pearls on many boats "
  "in the whirlpool.",
  "你不知道为什么，但漩涡中的很多船上都有珍贵的珍珠。"
  )

Orb("Water", "水流")

S( "This Orb allows your boat to go against the current, "
   "and also to go into the land, creating water on the way.",
   "这个法球使你的船可以逆流而上，以及驶入陆地，在身后留下一道水路。")

S("You cannot go against the current!", "你不能逆水行舟！")

// Minefield
// ---------

// achievements
// Reached Minefield: Collect a Bomberbird Egg.
// Minesweeper: Collect 10 Bomberbird Eggs.
// Bomberbird: Collect 25 Bomberbird Eggs.
// Master of the Minefield: Collect 50 Bomberbird Eggs.

N("Minefield", GEN_N, "雷区","")
N("minefield", GEN_N, "雷区","处")
N("cell without mine", GEN_N, "没有雷的格子","块")

S(
  "This minefield has been created by the Bomberbirds to protect their valuable eggs. "
  "Mines explode when they are stepped on, creating fire all around them "
  "(this fire never expires in the Minefield). If you don't step on a mine, you "
  "know the number of mines in cells around you. These numbers are marked with "
  "color codes on the cells you have visited.",
  "这片雷区是炸弹鸟铺设来保护它们宝贵的蛋的。地雷在踩到时爆炸，在周围布满火焰（这些火焰在雷区中永远不会熄灭）。如果你没有踩到地雷，你会感知到周围格子中的地雷总数。这些数字会以颜色的形式标记在你经过的格子上。"
  )

N("Bomberbird", GEN_N, "炸弹鸟","只")

S(
    "Dark red birds who have created the minefield.\n\n"
    "They create a mine on the spot where they are killed, provided "
    "that the terrain is suitable. Also note that mines are triggered "
    "by dead birds falling on them.",

    "铺设雷区的暗红色鸟类。\n\n"
    "只要地形合适，它们就会在死去的位置埋设一枚地雷。要注意，地雷会被掉到其上的死鸟触发。")

N("Tame Bomberbird", GEN_N, "驯服的炸弹鸟","只")

S(
  "This bomberbird will follow you at some distance, and attack your enemies. "
  "You can kill it to place a mine.",

  "这只炸弹鸟会跟随你移动，攻击你的敌人。你可以杀掉它来布置地雷。")

N("Bomberbird Egg", GEN_N, "炸弹鸟蛋","枚")

S(
  "Bomberbird eggs are big and tasty, and thus valuable. "
  "They can hatch when left alone for some time (but this will never happen "
  "if you are watching).",

  "炸弹鸟蛋又大又美味，因而昂贵。一段时间不管，它们就会孵化（不过你看着的时候就不会）。"
  )


Orb("Friendship", "友谊")

S("This Orb summons a friendly Bomberbird.",
  "这个法球召唤一只友好的炸弹鸟。")

S("Still confused? Read the FAQ on the HyperRogue website!\n\n",
  "还是不懂？阅读HyperRogue网站上的FAQ！\n\n")

S("Nowhere to blow %the1!", "没有可以把%1吹到的地方！")
S("%The1 is immune to wind!", "%1免疫风！")
S("You blow %the1 away!", "你将%1吹走！")

S("WARNING: you are entering a minefield!", "警告：你将步入雷区！")
S("No mines next to you.", "你附近没有地雷。")
S("A mine is next to you!", "你附近有一颗地雷！")
S("Two mines next to you!", "你附近有两颗地雷！")
S("Three mines next to you!", "你附近有三颗地雷！")
S("Four mines next to you!", "你附近有四颗地雷！")
S("Five mines next to you!", "你附近有五颗地雷！")
S("Six mines next to you!", "你附近有六颗地雷！")
S("Seven mines next to you!", "你附近有七颗地雷！")

S("You have to run away from the water!", "你需要从水边离开！")

// VERSION 7.2
// ===========

// Palace
// ------

N("Palace", GEN_N, "宫殿","")
N("palace wall", GEN_N, "宫墙","堵")

S("Apparently a princess is kept locked somewhere, but you won't ever find her in this hyperbolic palace. ",
  "似乎有一位公主被关押在这里某处，但你在这双曲宫殿里永远也找不到她。")

S("Apparently a prince is kept locked somewhere, but you won't ever find him in this hyperbolic palace. ",
  "似乎有一位王子被关押在这里某处，但你在这双曲宫殿里永远也找不到他。")

S("So better just to concentrate on collecting treasure. "
  "Beware the traps, and the guards, who are hard to kill!",

  "所以还是专心找宝藏吧。小心陷阱，还有难以击杀的守卫！"
  )

N("trapdoor", GEN_N, "活板门","道")
S("This floor will fall after someone goes there. Go quickly!",
  "这块地板会在有人走过之后坠落。快走！")

N("closed gate", GEN_N, "关闭的大门","道")
N("open gate", GEN_N, "开启的大门","道")
N("closing plate", GEN_N, "关闭压力盘","块")
N("opening plate", GEN_N, "开启压力盘","块")

S("Green pressure plates open all gates in radius 3 around them. "
  "Similarly, red pressure plates close all gates in this radius.",

  "绿压力盘会开启半径3格范围内的所有大门。与之相反，红压力盘会关闭这一范围内的所有大门。")

N("Hypersian Rug", GEN_N, "超波斯地毯","块")
S("Nice, a hyperbolic rug! It will not fit very well on your flat Euclidean floor, but who cares?",
  "不错，一块双曲地毯！它没法安稳铺在你平坦的欧几里得地板上，但谁在乎呢？")

Orb("the Frog", "青蛙")
S("This Orb lets you jump to a place which is two cell away from you, in a single turn. "
  "You can jump over water, chasms and fire, but not over walls.",
  "这个法球让你可以在一回合内跳到两格以外的地方。你可以越过水面、裂缝与火焰，但不能跳过墙壁。")
S("Cannot jump through %the1!", "不能跃过%1！")
S("You jump!", "你发起跳跃！")

S("\n\nNote: when pushing %the1 off a heptagonal cell, you can control the pushing direction "
  "by clicking left or right half of the heptagon.",
  "\n\n提示：在你将%1从七边形格子推开时，你可以通过点击七边形的左半边或右半边来控制方向。"
  )

S(" With the keyboard, you can rotate the view for a similar effect (Page Up/Down).",
  "使用键盘时，你可以通过旋转视角达到这一效果（Page Up/Down）。")

N("Palace Guard", GEN_N, "宫殿守卫","名")

S("You have to hit Palace Guards several times to kill them. After each hit, they "
  "are pushed away and stunned for some time.",
  "你需要击中宫殿守卫多次才能击杀他们。在每次受击之后，他们会被推开，并击晕一段时间。")

N("Fat Guard", GEN_N, "胖守卫","名")

S("Fat guards are too heavy to be pushed away by your hits.",
  "胖守卫重得没法被你的攻击推走。")

N("Skeleton", GEN_N, "骷髅","具")
S("Skeletons work similar to Palace Guards, but they won't die no matter how many "
  "times you hit them. Well, you have to be more creative...\n\n"
  "Skeletons attacked outside of their native land, Palace, are stunned for a longer time.",
  "骷髅与宫殿守卫类似，但它们不管被打中多少次都不会死。哎呀，你得发挥一点创造力了……\n\n"
  "在骷髅的原生地宫殿之外，被击中的骷髅会眩晕更久。"
  )

N("Vizier", GEN_N, "维齐尔","位")
S("Viziers are neither pushed away nor stunned. However, you attack them automatically when "
  "escaping from them.",
  "维齐尔既不会被打退也不会被打晕。不过，你在从他们身边逃开时会自动攻击他们。")

S("You stun %the1.", "你击晕了%1。")
S("The gate is closing right on you! RUN!", "大门自你头顶落下！快跑！")
S("%The1 is crushed!", "%1被压扁了！")
S("You hit %the1.", "你击中%1。")
S("Hit him by walking away from him.", "从他身边走开来攻击他。")
S("Hmm, he has been training in the Emerald Mine. Interesting...", "嗯，他在绿宝石矿受过训练。有意思……")

// Reached the Palace: Find and collect a Hypersian Rug.
// Palace Guard: Collect 10 Hypersian Rugs.
// Vizier: Colect 25 Hypersian Rugs.
// Prince: Colect 50 Hypersian Rugs.

// Living Fjord
// ------------

N("Living Fjord", GEN_N, "生机峡湾","")

S("A coastal area, from where you can get both to the inland worlds and to the Ocean. "
  "Each turn, each cell will become water or earth, based on the majority of cells around it. "
  "Contrary to the Living Caves, this process is not affected by most items and monsters, "
  "but elementals, dead Trolls, and cells outside of the Living Fjord have "
  "a very powerful effect.",
  "一片沿海地域，你既可以从这里去往内陆地域，也可以去往海洋。在每回合后，每格都会变为水或土地，取决于周围的哪一种格子较多。与生机洞穴不同，这一过程不受大多数物品与怪物影响，但元素、死巨魔，以及生机峡湾外的格子有非常强的影响。"
  )

N("Fjord Troll", GEN_N, "峡湾巨魔","只")
S("Fjord Trolls leave a wall when they die, causing the living fjord to rise around it. "
  "Contrary to Rock Trolls, items around are not destroyed.",
  "峡湾巨魔在死后留下一堵墙，使得生机峡湾在其周围上升。与岩石巨魔不同，周围的物品不会被摧毁。"
  )

N("Viking", GEN_N, "维京人","名")
S("Mighty warriors from the Fjord, who can use boats.",
  "来自峡湾的强大战士，会驾驶船只。")

N("Water Elemental", GEN_N, "水元素","个")
S("Wherever this powerful being goes, the living fjord "
  "sinks below water, non-magical boats are destroyed, and fires are extinguished.\n\n"
  "As a special case, you can attack the Water Elemental with your blade from the water, without drowning immediately.",
  "不管这一强大的存在去往哪里，生机峡湾都会沉入水中，不具魔法的船被摧毁，火焰被熄灭。\n\n"
  "作为特例，你可以在水中以你的匕首攻击水元素，而不会立即溺水。")


N("Garnet", GEN_N, "石榴石","枚")
S("Vikings believe that garnets improve their strength.",
  "维京人相信石榴石可以增强他们的力量。")

Orb("the Fish", "游鱼")
S("This Orb lets you dive into the water. While diving, you are able to see and collect underwater treasures, "
  "while being invisible to monsters yourself. You are afraid of jumping into the water straight from a boat, so usually you won't do it.",
  "这个法球使你能够潜入水中。在潜水时，你可以见到并收集水下的宝藏，同时对怪物隐形。你害怕直接从船上跳进水里，所以你一般不会这么做。")

S("%The1 is washed away!", "%1被冲走了！")
// %1 zosta%ł1 zmyt%ya1!")
S("%The1 is extinguished!", "%1熄灭了！")

// Reached the Living Fjord: Find and collect a Garnet.
// Viking: Collect 10 Garnets.
// Water Elemental: Colect 25 Garnets.
// Master of the Fjord: Colect 50 Garnets.

Orb("Discord", "失谐")

S("Causes most monsters to attack other monsters, not only you and your friends.",
  "使得大多数怪物不仅会攻击你与你的朋友，也开始互相攻击。")

// Shmup Configuration
// -------------------

S("forward", "向前")
S("backward", "向后")
S("turn left", "左转")
S("turn right", "右转")
S("move up", "向上移动")
S("move right", "向右移动")
S("move down", "向下移动")
S("move left", "向左移动")
S("throw a knife", "投掷飞刀")
S("face the pointer", "面对指向处")
S("throw at the pointer", "向指向处投掷")
S("drop Dead Orb", "丢弃失效法球")
S("center the map on me", "居中地图于自身")
S("activate Orb power", "启动法球能力")

S("pan up", "向上移动视角")
S("pan right", "向右移动视角")
S("pan down", "向下移动视角")
S("pan left", "向左移动视角")
S("rotate left", "向左旋转")
S("rotate right", "向右旋转")
S("home", "重置视角位置")

S("do nothing", "无")
S("rotate view", "旋转视角")
S("panning X", "移动X视角")
S("panning Y", "移动Y视角")
S("player 1 X", "玩家1 X")
S("player 1 Y", "玩家1 Y")
S("player 1 go", "玩家1 移动")
S("player 1 spin", "玩家1 旋转")
S("player 2 X", "玩家2 X")
S("player 2 Y", "玩家2 Y")
S("player 2 go", "玩家2 移动")
S("player 2 spin", "玩家2 旋转")
S("player 3 X", "玩家3 X")
S("player 3 Y", "玩家3 Y")
S("player 3 go", "玩家3 移动")
S("player 3 spin", "玩家3 旋转")
S("player 4 X", "玩家4 X")
S("player 4 Y", "玩家4 Y")
S("player 4 go", "玩家4 移动")
S("player 4 spin", "玩家4 旋转")

S("Joystick %1, axis %2", "摇杆%1，轴%2")
S("one player", "一名玩家")
S("two players", "两名玩家")
S("configure player 1", "设置玩家1")
S("configure player 2", "设置玩家2")
S("configure player 3", "设置玩家3")
S("configure player 4", "设置玩家4")
S("configure panning", "设置视角移动")
S("configure joystick axes", "设置摇杆轴")
S("continue playing", "继续游戏")
S("start playing in the shmup mode", "以射击模式开始游戏")
S("return to the turn-based mode", "返回回合制模式")
S("save the configuration", "保存设置")
S("press a key to unassign", "按需要取消绑定的键")
S("press a key for '%1'", "按下“%1”所需绑定的键")
S("unassign a key", "取消绑定一个按键")

// extra pattern modes
// -------------------

S("explore the Emerald Pattern", "探索绿宝石图案")

// extra flavor/Knight lines
// -------------------------

S("Kill a Vizier in the Palace to access Emerald Mine", "在宫殿击杀维齐尔以去往绿宝石矿。")
S("Collect 5 Emeralds to access Camelot", "收集5个绿宝石以去往卡美洛。")
S("Some lands unlock at specific treasures or kills", "某些地域需要特定的宝藏或击杀解锁。")

S("You will be fighting red rock snakes, too...", "你也会与红岩蛇战斗……")

S("\"The Red Rock Valley is dangerous, but beautiful.\"",
  "“红岩谷危险而美丽。”")
S("\"Train in the Desert first!\"", "“先在沙漠练练吧！”")
S("\"I like collecting ambers at the beach.\"", "“我喜欢在海滩收集琥珀。”")
S("\"Our Table seats %1 Knights!\"", "“我们的圆桌有%1位骑士就座！”")
S("\"There are %1 floor tiles inside our Table!\"", "“我们的圆桌中有%1个格子！”")
S("\"By now, you should have your own formula, you know?\"", "“到了眼下，你该有你自己的一套办法了吧？”")
S("\"Have you tried to take a boat and go into the Ocean? Try it!\"", "“你试过取走一艘船驶向海洋吗？试试吧！”")

// VERSION 7.3
// ===========

// new name for 'Ivy D'

N("Dead Ivy", GEN_N, "死常春藤","段")

// new messages for living/nonliving monsters

S("%The1 bites %the2!", "%1啃咬%2！")
S("%The1 attacks %the2!", "%1攻击%2！")
S("You destroy %the1 with a mental blast!", "你以灵能冲击摧毁了%1！")
S("The ivy kills %the1!", "常春藤击杀了%1！")
S("You destroy %the1.", "你摧毁了%1。")
S("%The1 kills %the2!", "%1击杀了%2！")
S("%The1 sinks!", "%1沉没了！")

S("Cannot jump on %the1!", "不能跳到%1上！")

// renamed Fifty to Palace
S("explore the Palace Pattern (4 colors)", "探索宫殿图案（4色）")
S("explore the Palace Pattern (8 colors)", "探索宫殿图案（8色）")

// Map Editor
// ----------

S("map editor", "地图编辑器")
S("You activate your terraforming powers!", "你启动了你的革地能力！")
S("Press F5 to clear the board.", "按F5清空版面。")
S("Press F5 to restart the game normally.", "按F5正常重启游戏。")
S("High quality shots not available on this platform", "高品质截图在本平台不可用")
S("Saved the high quality shot to %1", "高品质截图已保存至%1")
S("Map saved to %1", "地图已保存至%1")
S("Map loaded from %1", "地图已自%1读取")
S("copying", "复制")
S("boundary", "边界")
S("clear monster", "清除怪物")
S("use at your own risk!", "使用后果自负！")

// Note: in English, these have been arranged from longest to shortest.
// This gives a somewhat nice effect on the top left corner of the screen.
// You may want to aim for the same effect when translating

S("0-9 = radius (%1)", "0-9 = 半径 （%1）")
S("b = boundary", "i = 边界")
S("m = monsters", "m = 怪物")
S("w = walls",    "w = 墙壁")
S("i = items",    "l = 物品")
S("l = lands",    "c = 地域")
S("c = copy",     "b = 复制")

// And this is from shortest to longest (bottom left corner)
S("F1 = help",                "F1 = 帮助")
S("F2 = save",                "F2 = 保存")
S("F3 = load",                "F3 = 读取")
S("F5 = restart",             "F5 = 重启")
S("F6 = HQ shot",             "F6 = 高品质截图")
S("F7 = player on/off",       "F7 = 玩家 开/关")
S("ESC = return to the game", "ESC = 返回游戏")

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


  "这个模式让你能够编辑地图。\n\n"
  "注意：后果自负。在游戏中没有出现过的组合可能不会正常起效（完全不工作、看起来很奇怪、产生奇怪的信息，甚至导致游戏崩溃）。\n\n"
  "想要最大程度利用这个编辑器，你需要一些有关HyperRogue内部工作原理的知识。每个格子有四个主要参数：地类型、墙类型、怪物类型、物品类型。相同的墙类型（尤其是“无”、“海洋”或“篝火”）可能看起来或效果上不完全相同，取决于所在的地域。有时一件物体可能在列表中出现两次，因为存在细微的区别（比如，恶魔能否下回合行动）。\n\n"
  "按w、i、l或m选择要更改的参数，然后点击格子，格子就会改变。鼠标指向格子按“c”复制这个格子，包括这个格子的一切信息。在复制大范围或多格怪物时，具体点击哪格可能会比较重要。\n\n"
  "你也可以按0-9将修改作用到更大的范围中。这也会影响复制粘贴功能，让你能复制更大的区域。\n\n"
  "按F2保存当前地图（然后F3读取）。如果你在打了很久HyperRogue以后保存（并且没有使用安全法球），文件尺寸会非常大！注意，比较大的结构，比如长城、较大的圆乃至极限圆，会因此被摧毁。\n\n"
  "按“b”将格子标记为边界。这些格子以及它们之外的格子不会被复制粘贴，也不会受到保存功能保存。\n\n"
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
  
  "一只老鼠朝你吱吱。看起来它想让你去某个地方。有点意思，你会找到什么呢？\n\n"
  "注意：你在这一任务中探索的宫殿会有稍微不同的怪物、压力盘及物品分布，更适于经验不足的玩家。怪物的数量不取决于你收集的超波斯地毯数量，地图上也会有更多开启压力盘。\n\n"
  "不过，即使分布正常，你也应当总是能够抵达你的目标。如果你想尝试这一挑战，在特殊游戏模式菜单中选择这一项。")

S("\"When I visited the Palace, a mouse wanted me to go somewhere.\"",
  "“我拜访宫殿时，有只老鼠想让我去个什么地方。”")

S("\"I wonder what was there...\"", "“我想知道那是什么地方……”")

N("Mouse", GEN_N, "老鼠","只")

S("You hear a distant squeak!", "你听到远处的吱吱声！")
S("%The1 squeaks in a confused way.", "%1困惑地吱吱。")
S("%The1 squeaks gratefully!", "%1感激地吱吱！")
S("%The1 squeaks hopelessly.", "%1无望地吱吱。")
S("%The1 squeaks in despair.", "%1绝望地吱吱。")
S("%The1 squeaks sadly.", "%1悲伤地吱吱。")
S("%The1 squeaks with hope!", "%1充满希望地吱吱！")
S("%The1 squeaks happily!", "%1开心地吱吱！")
S("%The1 squeaks excitedly!", "%1激动地吱吱！")

N("giant rug", GEN_N, "大地毯","块")

S("This is the biggest Hypersian Rug you have ever seen! "
  "Unfortunately, it is too large to take it as a trophy.",
  "这是你见过最大的超波斯地毯！可惜太大了，没法作为纪念品带走。")

N("Prince", GEN_N, "王子","名")
N("Princess", GEN_N, "公主","名")

S("%1 takes %his1 revenge on %the2!", "%1对%2复仇！")
S("\"That should teach you to take me seriously!\"",
  "“这下你该知道要认真对待我了！”")

S("%The1 kisses you, and begs you to bring %him1 away from here.",
  "%1亲吻你，并乞求你将其带离这里。")

S("\"I want my revenge. Stun a guard and leave him for me!\"",
  "“我要复仇。击晕一个守卫，剩下的事我来！”")

S("\"That felt great. Thanks!\"", "“感觉不错。谢谢！”")

S("\"Bring me out of here please!\"", "“请你把我从这里救走！”")

S("%The1 kisses you, and thanks you for saving %him1.",
  "%1亲吻你，并感谢你将其救出。")

S("\"I have been trained to fight with a Hypersian scimitar, you know?\"",
  "“我练习过用超波斯弯刀战斗，你知道吗？”")

S("\"I would love to come to your world with you!\"",
  "“我真想陪你去你的世界！”")

S("\"Straight lines stay close to each other forever, this is so romantic!\"",
  "“直线永远相互贴近，好浪漫！”")

S("\"Maps... Just like the world, but smaller... how is that even possible?!\"",
  "“地图……和世界一模一样，只不过小一号……这怎么可能？！”")

Orb("Love", "爱情")
S("Love takes time, but it heals all wounds, and transcends time and space.\n\n"
  "The Orb of Love is worth 30$$$, if you end the game with it.\n",

  "爱情需要时间，但它能治愈一切创伤，超越时间与空间。\n\n"
  "爱情法球价值30$$$，如果结束游戏时持有它。\n")

// Princess Challenge
// ------------------

S("%1 Challenge", "%1挑战")
S("Save %the1 first to unlock this challenge!", "完成营救%1来解锁这一挑战！")
S("Welcome to %the1 Challenge!", "欢迎来到%1挑战！")
S("The more Hypersian Rugs you collect, the harder it is.", "你收集的超波斯地毯越多，难度就越高。")
S("Follow the Mouse and escape with %the1!", "跟随老鼠，与%a1一同逃脱！")
S("Hardness frozen at %1.", "难度在%1锁定。")
S("Congratulations! Your score is %1.", "恭喜！你的得分为%1。")

S("u = undo",     "u = 撤销")
S("f = flip %1",     "f = 翻转%1")

S("Failed to save map to %1", "保存地图至%1失败")
S("Failed to load map from %1", "自%1读取地图失败")
S("save whom", "拯救对象")
S("other", "其他") // other keys in the main menu

// VERSION 7.4
// ===========

// missing texts, refactored things, and rule changes
// --------------------------------------------------

S("%The1 activates her Flash spell!", "%1启动了闪光咒语！")

N("Fire", GEN_N, "火焰","团")
S("This cell is on fire. Most beings and items cannot survive.",
  "这一格起火了。大多数存在与物品无法在其中存留。")

N("Eternal Fire", GEN_N, "永燃之火","团")
S("This fire never burns out.", "这一团火焰永远不会熄灭。")

S("Some rubble.", "一些碎石。")

S("The Orb loses its power as it leaves the Land of Power!",
  "法球在离开力量之地时失去了能量！")

S("%The1 fills the hole!", "%1填住了坑！")

N("Tentacle+Ghost", GEN_N, "触手+鬼魂","团")

// Land Overview
// -------------

S("world overview", "世界概览")
S("or 'o' to see the world overview", "或按“o”查看世界概览")

S("forbidden to find in %the1", "无法在%1找到")
S("too dangerous to use in %the1", "在%1使用过于危险")
S("useless in %the1", "在%1无用")
S("only native Orbs allowed in %the1", "仅有原生法球可在%1使用")
S("this Orb is never unlocked globally (only hubs)", "该法球无法在全地域解锁（仅中心）")
S("collect 25 %2 to unlock it in %the1", "收集25%m2%2以在%1解锁")
S("collect 3 %2 to unlock it in %the1", "收集3%m2%2以在%1解锁")
S("native to %the1 (collect 10 %2)", "原生于%1 （10%m2%2）")
S("native to %the1 (collect 1 %2)", "原生于%1 （1%m2%2）")
S("secondary in %the1 (collect 10 %3, or 25 %2)", "%1次要法球 （10%m3%3，或25%m2%2）")
S("the native Orb of %the1", "%1的原生法球")
S("this Orb appears on floors and is used by witches", "这个法球出现在地上，可被女巫使用")
S("a prized Orb, it appears only in cabinets", "珍贵的法球，只在柜子中出现")
S("this Orb never appears in %the1", "这一法球从不在%1出现")
S("Hub Land: orbs appear here if unlocked in their native land", "中心地域：在其他地域解锁的法球在此出现")
S("kill a monster, or collect 25 %2", "击杀一个怪物，或收集25%m2%2")

S("Always available.\n", "总是可用。\n")
S("Treasure required: %1 $$$.\n", "所需宝藏：%1 $$$。\n")
S("Accessible only from %the1.\n", "只能自%1去往。\n")
S("Accessible only from %the1 (until finished).\n", "只能自%1去往（直到完成）。\n")

S("Accessible only from %the1 or %the2.\n", "只能自%1或%2去往。\n")

S("Kills required: %1.\n", "所需击杀数：%1。\n")
S("Finished lands required: %1 (collect %2 treasure)\n",
  "所需完成地域数：%1（收集%2个宝藏）\n")

S("Treasure required: %1 x %2.\n", "所需宝藏数：%1%m2%2。\n")

S("Alternatively: kill a %1.\n", "或者：击杀一%m1%1。\n")

S("Kills required: any Elemental (Living Fjord/Dead Caves).\n",
  "所需击杀：任意元素（生机峡湾/死寂洞穴）。")

S("Variants of %the1 are always available in the Random Pattern Mode.",
  "%1的变体在随机图案模式总是可用。")

S("Variants of %the1 are available in the Random Pattern Mode after "
  "getting a highscore of at least 10 %2.",
  "%1的变体在取得10%m2%2的最高分后可用。")

S(" kills: %1", " 击杀数：%1")
S(" $$$: %1", " $$$：%1")
S(" Hyperstone: %1/%2", " 双曲石：%1/%2")
S(" Hell: %1/%2", " 地狱：%1/%2")

// improved editor

S("vector graphics editor -- press F1 for help", "矢量图形编辑器 -- 按F1查看帮助")
S("pics to save/load:", "保存/读取的图片：")
S("level to save/load:", "保存/读取的关卡：")

S("F4 = extension", "F4 = 扩展")
S("Enter = back", "回车 = 返回")
S("SPACE = map/graphics", "空格 = 地图/图像")

S("Emerald Pattern", "绿宝石图案")
S("Palace Pattern", "宫殿图案")
S("Zebra Pattern", "斑马图案")
S("rotational symmetry", "旋转对称")
S("symmetry 0-1", "对称 0-1")
S("symmetry 0-2", "对称 0-2")
S("symmetry 0-3", "对称 0-3")

S("display pattern codes (full)", "显示图案代码（完整）")
S("display pattern codes (simplified)", "显示图案代码（简化）")
S("display only hexagons", "只显示六边形")
S("display only heptagons", "只显示七边形")
S("predesigned patterns", "预设图案")

S("Gameboard", "棋盘")
S("random colors", "随机颜色")
S("emerald pattern", "绿宝石图案")
S("four elements", "四元素")
S("eight domains", "八极")
S("zebra pattern", "斑马图案")
S("three stripes", "三条")
S("random black-and-white", "随机黑白")
S("p = paint", "p = 涂色")
S("r = regular", "r = 常规")

S(
   "In this mode you can draw your own player characters, "
   "floors, monsters, and items. Press 'e' while hovering over "
   "an object to edit it. Start drawing shapes with 'n', and "
   "add extra vertices with 'a'. Press 0-9 to draw symmetric "
   "pictures easily. More complex pictures can "
   "be created by using several layers ('l'). See the edges of "
   "the screen for more keys.",

   "在该模式中。你可以绘制自己的角色、地板、怪物，还有物品。按“e”编辑指向的物体。以“n”开始绘制形状，按“a”添加更多的顶点。按0-9绘制对称图形。更复杂的图像可以使用更多的图层（“l”）来绘制。屏幕边缘还显示了更多可用按键。")

S(
   "Press 'r' to choose a regular pattern. When a pattern is on, "
   "editing a cell automatically edits all cells which are "
   "equivalent according to this pattern. You can choose from "
   "several patterns, and choose which symmetries matter "
   "for equivalence. Also, you can press Space to switch between "
   "the map and graphics editor quickly -- note that editing floors "
   "with the graphics editor also adheres to the pattern.",

   "按“r”选择一种常规图案。当选中一种图案时，编辑格子会自动编辑在该图案中所有等价的格子。你可以从多种图案中选择，并选择哪种对称对等价有效。你还可以按空格在地图与图形编辑器之间快速切换——注意使用图形编辑器编辑地板也会遵从图案。")

S("monster" ,"怪物")
S("item", "物品")
S("floor", "地板")
S("hexagonal", "六边形")
S("heptagonal", "七边形")
S("floor/pattern", "地板/图案")

S("l = layers: %1", "l = 图层：%1")
S("1-9 = rotations: %1", "1-9 = 旋转：%1")
S("0 = symmetry", "0 = 对称")
S("0 = asymmetry", "0 = 非对称")
S("%1 vertices", "顶点：%1")
S("a = add v", "a = 添加 v")
S("m = move v", "m = 移动 v")
S("d = delete v", "d = 删除 v")
S("c = readd v", "c = 重新添加 v")
S("b = reorder", "b = 重新排序")
S("t = shift", "t = 平移")
S("y = spin", "y = 旋转")

S("'n' to start", "n = 开始")
S("z = zoom in", "z = 放大")
S("o = zoom out", "o = 缩小")
S("g = grid", "g = 网格")
S("e = edit this", "e = 编辑这一格")

S("x: %1", "x：%1")
S("y: %1", "y：%1")
S("z: %1", "z：%1")
S("r: %1", "r：%1")
S("ϕ: %1°", "ϕ：%1°")

S("A fake Land with colored floors.",
  "地板上色的虚假地域。")

S("random pattern mode", "随机图案模式")

// Ivory Tower
// -----------

N("Ivory Tower", GEN_N, "象牙塔","")

S(
  "Powerful wizards claimed this part of the world, to perform their magical "
  "experiments in peace and solitude. They have changed the direction of gravity, "
  "to make it even harder for intruders to reach them.\n\n"
  "Gravity works as follows: cells are unstable if they are empty, and there is "
  "no cell immediately below them which contains a wall. It is impossible to move "
  "from one unstable cell to another, except if moving down.",

  "强大的法师占有了这一处地域，在宁静与孤立中进行他们的魔法实验。他们改变了重力的方向，使得入侵者难以深入。\n\n"
  "重力以如下规则生效：空且其下方没有相邻墙的格子不稳定。不能从不稳定的格子移动到其他不稳定的格子，除非是向下。"
  )

N("Servant", GEN_N, "仆从","名")
S("A simple servant of the master of the Ivory Tower.",
  "象牙塔主人的简单仆从。")

N("Gargoyle", GEN_N, "石像鬼","只")
N("stone gargoyle", GEN_N, "岩石石像鬼","只")
N("stone gargoyle floor", GEN_N, "岩石石像鬼地板","块")
N("stone gargoyle bridge", GEN_N, "岩石石像鬼桥","段")

S(
  "A being made of stone, who likes high buildings. It becomes normal stone when "
  "killed, but only if next to something stable -- otherwise it falls.",
  "石头构成的存在，喜欢高耸的建筑。被杀时变成普通的岩石，但只有与稳定的东西相邻时才会这样——否则就会跌落。")

N("Ivory Figurine", GEN_N, "象牙小雕像","尊")

S("A beautiful figurine, made of ivory. Figurines close to the base of the Tower "
  "tend to disappear after you have collected many of them.",
  "象牙制成的美丽小雕像。接近塔底的小雕像在你收集很多后会慢慢消失。")

N("Platform", GEN_N, "平台","道")
N("platform", GEN_N, "平台","道")

S("You can stand here.", "你可以站在这里。")

N("ladder", GEN_N, "爬梯","截")
S("You can use this ladder to climb the Tower.",
  "你可以用这节梯子爬上高塔。")

Orb("Matter", "物质")

S("This Orb allows one to temporarily create impassable matter, either to block paths or "
  "to build bridges across chasms and waters.",
  "这个法球使你能暂时创造无法通过的物质，可用于阻隔道路或是在裂缝与水面上创造桥梁。")

N("temporary wall", GEN_N, "临时墙","堵")
N("temporary floor", GEN_N, "临时地板","块")
N("temporary bridge", GEN_N, "临时桥","段")

S("This structure will disappear after some time.", "这一结构会在一段时间后消失。")

S("Nothing to stand on here!", "没有地方落脚！")
S("Gravity does not allow this!", "重力使你不能这样做！")

// Elemental Planes
// ----------------

N("Elemental Planes", GEN_N, "元素位面","")
N("Plane of Fire", GEN_N, "火元素位面","")
N("Plane of Earth", GEN_N, "土元素位面","")
N("Plane of Air", GEN_N, "气元素位面","")
N("Plane of Water", GEN_N, "水元素位面","")

S("The Elemental Planes are divided into four subzones: Planes of Fire, Water, Air, and Earth. "
  "You need to collect a Shard from each Plane to construct an Elemental Gem. "
  "It is dangerous to collect too many Shards of the same type without constructing a Gem.",

  "元素位面被分为四个子区域：火、土、气、水。你需要在每个区域各搜集一块碎片来合成元素宝石。收集太多相同类型的碎片而不合成宝石非常危险。"
  )

N("Fire Elemental", GEN_N, "火元素","个")

S("This monster leaves a trail of fire behind.",
  "这个怪物在身后留下一道火焰踪迹。")

N("Air Elemental", GEN_N, "气元素","个")

S("An Air Elemental looks like a live tornado. Once you are three (or less) cells from it, "
  "it is impossible to move closer, due to strong winds. You can stand and wait, though. "
  "This also affects most monsters.",

  "气元素看起来像是一道活着的龙卷风。你一旦处于它的三（或更少）格以内，就会因为强风无法靠近。不过你可以原地等待。这个效果也会影响大多数怪物。")

N("Fire Shard", GEN_N, "火焰碎片","块")
N("Air Shard", GEN_N, "空气碎片","块")
N("Water Shard", GEN_N, "水流碎片","块")
N("Earth Shard", GEN_N, "大地碎片","块")
N("Elemental Gem", GEN_N, "元素宝石","枚")

Orb("Summoning", "召唤")

S(
    "This orb allows you to summon monsters. Usually, they are either Elementals or "
    "native creatures. While the monsters do not like being summoned, and will "
    "attack you once they recover from summoning sickness, such summoning "
    "often has its uses.",
  "这个法球使你能够召唤怪物。通常，召唤出的怪物不是元素就是当前地域的原生生物。"
  )

N("limestone wall", GEN_N, "石灰石墙","堵")

S("Simply a wall. Mostly.", "只是堵墙罢了。")

// Zebra
// -----

N("Zebra", GEN_N, "斑马","")
S("Everything in this Land has black and white stripes.",
  "这一地域中的一切都是黑白条纹的。")

N("Striped Dog", GEN_N, "条纹犬","条")
S("A predator native to the Zebra.", "斑马的原生捕食者。")

N("Onyx", GEN_N, "缟玛瑙","枚")
S("A black gem with white stripes. It is beautiful.", "有白条纹的黑宝石。真漂亮。")

// Crossroads III
// --------------

N("Crossroads III", GEN_N, "交叉路口 III","")

S("An alternate layout of the Crossroads. Great Walls cross here at right angles.",
  "交叉路口的又一种构造。长城在此地以直角交汇。")

S("Cannot create temporary matter on a monster!", "无法在怪物上创造临时物质！")
S("Cannot create temporary matter on an item!", "无法在物品上创造临时物质！")
S("Cannot create temporary matter here!", "无法在此处创造临时物质！")
S("Cannot summon on a monster!", "无法在怪物上召唤！")
S("No summoning possible here!", "无法在此召唤！")
S("You summon %the1!", "你召唤了%1！")

S("F4 = file", "F4 = 文件")

// VERSION 8.0
//=============

S("The Air Elemental blows you away!", "气元素将你吹走！")

// appended to cave troll description
S(" Additionally, all items around the killed Troll will be destroyed.",
  " 此外，临近被击杀的巨魔的所有物品都会被摧毁。")

// 'dead cave troll' and 'dead troll' are separated now

N("dead rock troll", GEN_N, "死岩石巨魔","个")

S("There are several species of trolls living in the hyperbolic world. "
  "Some of them leave this wall behind them when they die.",

  "在这个双曲世界中有许多种巨魔存在。有些巨魔死后会留下这个。")

// paper model creator
// --------------------

S("paper model creator", "纸模型制作器")
S("synchronize net and map", "同步网格与地图")
S("display the scope", "显示尺寸")
S("create the model", "创建模型")
S("design the net", "设计网格")

S("The paper model created as papermodel-*.bmp", "纸模型创建为papermodel-*.bmp")
S("Failed to load the file 'papermodeldata.txt'", "读取文件papermodeldata.txt失败")
S("Could not save the paper model data", "无法保存纸模型数据")

// pure tactics mode
// ------------------

S("pure tactics mode", "纯粹战术模式")
S("Not available in the pure tactics mode!", "在纯粹战术模式不可用。")
S("\"The Knights of the Horocyclic Table salute you!\"", "“极限圆桌骑士向你致敬！”")
S("press 0 to leave this mode", "按0离开本模式")
S("Collect %1x %2 to unlock", "收集%1%m2%2解锁")

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

        "在纯粹战术模式中，你在一处特定的地域中战斗。你的目标是尽可能地获取高分，不使用其他地域的特性。然后你就可以和朋友比较高分了！\n\n"

        "你需要在基础游戏中比较熟练才能解锁对应地域的这一挑战（在对应地域收集20个宝藏，或在卡美洛收集2个）。\n\n"

        "由于在某些地域取得高分比较看运气，你在某一地域进行N次游戏，得分取决于这N次连续游戏。N取决于在这一地域进行游戏有多“快”，以及多随机。\n\n"

        "在加勒比中，如果你曾经在对应地域取得过25个宝藏，你就能发现荆棘、以太与空间法球。\n\n"

        "宝藏的生成率在本模式中是固定的，不因击杀怪物增加。\n\n"

        "祝你好运，玩得开心！")

// Yendor Challenge
// -----------------

S("Yendor Challenge", "Yendor挑战")
S("Collect 10 treasures in various lands to unlock the challenges there",
  "在各种地域收集10个宝藏来解锁这些挑战")

S("easy", "简单")
S("challenge", "挑战")

S("Challenges do not get harder", "挑战不会变得更难")
S("Each challenge gets harder after each victory", "每次胜利都会使随后的挑战变难")

S(" (won!)", " （获胜！）")
S(" (won at level %1!)", " （于等级%1获胜！）")
S("(locked)", "（锁定）")

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

    "Yendor任务有许多可能的解法。在Yendor挑战中，你会尝试其中许多种！\n\n"
    "每个挑战都在一处特定地域中开始，你需要利用好你能收集到的资源。\n\n"
    "你需要在基础游戏中取得一枚Yendor法球来解锁这一挑战，并在某一个或两个地域中搜集各10个宝藏来解锁某一特定关卡。\n\n"

    "在你完成一项挑战后，你可以在更高难度再次尝试。\n\n"

    "所有在Yendor挑战中用到的解法在正常游戏中也都很有用。不过，在Yendor挑战中通往其他地域的路径，以及（有时）一些地域特性不会开启，来迫使你使用设定的策略。此外，宫殿与雷区的生成规则在寻找Yendor法球时有稍作改动，使得挑战更加平衡（这些改动在正常游戏寻找Yendor法球时也生效）。\n\n"
    "你每通过一项挑战便得到1000分，每个难度等级额外得1分。"
    )

S("Unlock this challenge by getting the Orb of Yendor!",
  "取得Yendor法球来解锁这一挑战！")

S("Collect 10 treasures in various lands to unlock the challenges there",
  "在各个地域收集10个宝藏来解锁这些挑战。")

// Wild West
// ----------

N("Wild West", GEN_N, "狂野西部","")
N("Outlaw", GEN_N, "法外狂徒","名")
N("Bounty", GEN_N, "赏金","份")
N("saloon wall", GEN_N, "沙龙墙","堵")
N("Revolver", GEN_N, "左轮手枪","把")

S("%The1 shoots you!", "%1对你射击！")
S("You shoot %the1!", "你射击%1！")

S(
  "Take a revolver, kill outlaws, collect bounties.\n\n"
  "Note: since this land is anachronistic, it is not available in normal game. "
  "It is only available in special modes.",

  "拿起左轮，击毙匪徒，取得悬赏。\n\n"
  "注意：由于时代不合背景，这一地域在正常游戏中不可用。只能在特殊模式中游玩。")

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

  "在你移动后，若有荷电格与接地格相连，则其间产生短路。所有处于荷电格通往接地格的任意路径（格子不能重复，也不能有连续两个接地或荷电格）上的格子受到伤害。\n\n"

  "沙石墙与大多数生物是导体。长城是绝缘体，但其外的地域视为接地。\n\n"

  "闪电熔岩是此地的宝藏，在你短路沙石墙或富金属兽时产生。\n\n"

  "巨魔在死后留下导体岩石，金属兽只能被电击杀死——你的攻击只会击晕它们，或在已眩晕时将它们击退。")

N("Land of Storms", GEN_N, "风暴之地","")
N("charged wall", GEN_N, "荷电墙","堵")
N("grounded wall", GEN_N, "接地墙","堵")
N("metal wall", GEN_N, "金属墙","堵")
N("sandstone wall", GEN_N, "沙石墙","堵")
N("Storm Troll", GEN_N, "风暴巨魔","只")
N("Metal Beast", GEN_N, "金属兽","头")
N("Rich Metal Beast", GEN_N, "富金属兽","头")
N("electric discharge", GEN_N, "放电电荷","道")

S("There is a flash of thunder!", "雷霆一闪！")

Orb("Stunning", "眩晕")

S("This Orb allows you to target monsters to stun them. "
  "10 charges are used to stun for 5 turns. Does not "
  "work against multi-tile monsters.",

  "这个法球使你能够瞄准并击晕怪物。消耗10点充能来将怪物击晕5回合。对多格怪物不起作用。"
  )

S("You stun %the1!", "你击晕了%1！")

// Overgrown Woods
// ----------------

Orb("Luck", "幸运")

S("This Orb allows you to find new lands more easily. "
  "Lands where you have already collected less treasure, "
  "and especially the Crossroads, are more likely to "
  "spawn while you have this. Additionally, Orbs of Safety "
  "are more likely to spawn in the Whirlpool.",

  "这个法球使你更容易找到其他地域。你持有这个法球时，收集过较少宝藏的地域及交叉路口尤其容易出现。此外，安全法球更容易在漩涡中出现。"
  )

N("Overgrown Woods", GEN_N, "莽林","")
N("Mutant Ivy", GEN_N, "变异常春藤","段")
N("Mutant Sapling", GEN_N, "变异苗","株")
N("Forest Troll", GEN_N, "森林巨魔","只")

S("Forest Trolls create an impassable wall when they die.",
  "森林巨魔在死去时产生一堵无法通过的墙。")

S(
  "The Overgrown Woods are filled with mutant ivies! These plants "
  "grow very fast. Each leaf, after being grown, can grow itself "
  "on the next turn. However, each part is only able to grow "
  "once in 16 turns. Outside of the Overgrown Woods, the Mutant Ivy "
  "may grow only on hexagonal cells.\n\n"
  "Maybe such fast growing plants could help you solve the problem "
  "of hunger in your world? Kill the Mutant Ivies to collect Mutant Saplings.",

  "莽林中长满了变异的常春藤！这些植物生长飞快。每片长出的叶子都可以在下个回合再次生长。不过，每一个部分都只能在16回合中生长一次。在莽林之外，变异常春藤只能在六边形格子上生长。\n\n"
  "也许长得这么快的植物可以用来永久解决你家乡世界的饥荒问题？杀掉变异常春藤收集变异苗。"
  )

S("Trees in this forest can be chopped down. Big trees take two turns to chop down.",
  "这一森林中的树木可以被伐倒。大树需要砍伐两个回合。"
  )

S("Your power is drained by %the1!", "你的能量被%1汲取！")

S("Wow! That was close.", "哇哦！只差一点。")

S("Collect four different Elemental Shards!", "收集四个不同种类的元素碎片！")
S("Unbalanced shards in your inventory are dangerous.",
  "你物品栏中不平衡的元素碎片会招致危险。")
S("You construct some Elemental Gems!", "你合成了一些元素宝石！")

N("Fulgurite", GEN_N, "闪电熔岩","块")

S("Alternatively: kill a %1 in %the2.\n", "或者：在%2击杀一%m1%1。\n")

// VERSION 8.1
// ============

// extra config
S("Prince", "王子")
S("Princess", "公主")
S("cat" ,"猫")
S("dog", "狗")
S("dress color II", "服饰颜色 II")
S("character", "角色")

//missing texts
S("%The1 hits %the2.", "%1击中%2。")
S("%The1 is destroyed by the Flash.", "%1被闪光摧毁。")

// improved kill descriptions
S("parts destroyed: %1", "摧毁的部分数：%1")
S("friends killed: %1", "击杀的朋友数：%1")
S("monsters destroyed: %1", "摧毁的怪物数：%1")
S("friends destroyed: %1", "摧毁的朋友数：%1")
S("\n\nThis is a part of a monster. It does not count for your total kills.",
  "\n\n这是某个怪物的一部分。它不计入你的总击杀数。")
S("\n\nThis is a friendly being. It does not count for your total kills.",
  "\n\n这是一个友善的存在。它不计入你的总击杀数。")

// Overgrown Clearing
// -------------------

S("A clearing in the Overgrown Woods. Obviously, this gives "
  "the Mutant Ivies an infinite space to grow...\n\n"
  "Mutant Fruits rot if they are not adjacent to a Mutant Ivy.",

  "莽林中的一片空地。显然，这让变异常春藤有无尽的空间可以生长……\n\n"
  "变异果若不在变异常春藤旁则会腐烂。")

N("Clearing", GEN_N, "林间空地","")

N("Mutant Fruit", GEN_N, "变异果","颗")

N("Giant Fox", GEN_N, "巨狐","只")

S(
    "What is freedom for you? A situation when you can walk wherever you want? "
    "Or a situation when you do not have to work, since you have as much tasty food "
    "as you want?\n\n"
    "Well, this creature has chosen the second option. It won't be happy "
    "if you destroy its prison.\n",

    "自由是什么？是你可以去想去的任何地方？还是你可以想不做什么就不做什么，因为这里有你想要的一切美味食物？\n\n"
    "唉，这个生物觉得第二种是自由。它不会乐意见到你打破它的监狱。"
    )

Orb("Freedom", "自由")

S(
    "This orb is activated if you are unable to escape (radius 4) "
    "without making illegal moves or "
    "going through cells which are currently adjacent to enemy monsters. "
    "Most game over situations are covered by this, but generally, "
    "this orb is oversensitive...\n\n"
    "When activated, it creates a Flash effect of radius 5.",

    "这个法球会在你无法在不做出非法动作或穿过目前与敌人相邻的格子逃脱（4格半径）时自动启动。大多数可能会导致游戏结束的场景都属于此列，不过这个法球一般还是有点过于敏感了……\n\n"
    "在启动时，它产生一个5格半径的闪光效果。"
  )

S("Your %1 activates!", "你的%1启动了！")

// Haunted Woods
// --------------

S("You become a bit nervous...", "你有些紧张……")
S("Better not to let your greed make you stray from your path.",
  "最好不要让你的贪婪导致你迷失方向。")

N("Haunted Woods", GEN_N, "闹鬼树林","")

S(
  "A dark forest filled with ghosts and graves. But there is also treasure hidden "
  "deep within... But don't let greed make you stray from your path, as "
  "you can get lost!\n\n"
  "The Haunted Woods are bounded by a single equidistant curve. It is not a circle or horocycle.\n\n",

  "充满鬼魂与坟墓的黑暗森林。但深处也藏有宝藏……但不要让你的贪婪导致你偏离前行道路，你也许会迷路！\n\n"
  "闹鬼森林缚于单单一条等距曲线之内。它并非圆或极限圆。\n\n")


N("Black Lotus", GEN_N, "黑莲花","朵")

S(
    "This beautiful flower is greatly prized by wizards, as it allows them to cast powerful magical spells "
    "without preparation.\n",

    "这一美丽的花朵很受巫师珍重，有了它就可以在不做准备的情况下施放强力的咒语。\n"
    )

Orb("Undeath", "不死")

S(
    "Monsters slain by you in melee are turned into friendly ghosts. "
    "Does not affect plants and friends.",

    "被你近战击杀的敌人会变成友善的鬼魂。对植物与朋友不起作用。"
    )

N("Friendly Ghost", GEN_N, "友善鬼魂","个")

S("Friendly ghosts are friendly beings who can go through any obstacles. However, "
  "unlike most friends, they tend to fly away from you.",

  "友善鬼魂是可以穿过障碍的友善存在。不过，不像大多数朋友，它们常常会远离你。"
   )

// Windy Plains
// -------------

N("Windy Plains", GEN_N, "风袭平原","")

N("fan", GEN_N, "风扇","座")

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

  "有什么人在这片平原上架设了许多风扇，搞得到处都是强风。你觉得这可能是为了利用气元素的魔力，但你也不确定。\n\n"

  "风扇以外的所有格子都按某种规律被分为三类颜色。风在某一种颜色的一团相连格子中逆时针吹动。被墙阻隔或在气元素至多2格以内的格子不算在内。\n\n"
  "无法沿着逆风强于顺风的方向移动，不过，你可以在一个回合内一次顺风移动2格，鸟儿也可以。"
  )

N("White Dove Feather", GEN_N, "白鸽羽","根")
S("This feather is truly beautiful and strong.",
  "这根羽毛真是又美丽又强韧。")

N("Wind Crow", GEN_N, "风鸦","只")
S("A large bird who likes strong winds. Just as you, it can fly quickly in the wind.",
  "喜爱强风的大鸟。和你一样，它可以快速顺风移动。"
  )

// Orb of Empathy replaces Orb of Air on the Beach
Orb("Empathy", "共情")
S(" %1", " %1")
S(", %1", "，%1")

S(  "This Orb lets your allies share your Orb powers.\n\n"
    "The following Orbs are affected:",

    "这个法球让你的盟友们也能分享你法球的力量。\n\n"
    "以下法球受此影响：")

N("strong wind", GEN_N, "强风","阵")

S(
    "In the Windy Plains, you can let the wind carry you, "
    "causing you to move two cells with the wind in a single turn. "
    "This cannot be done if you are standing at distance at most 2 "
    "from the Air Elemental, or if any of the three cells on the way "
    "has two wind directions.\n\n"
    "Press 't' or click the destination to activate.",

    "在风袭平原中，你可以借风势而行，让你能顺风一次移动两格。在你位于气元素至多2格内或路径会经过的共3个格子中有两种风向时无法这样移动。\n\n"
    "按“t”选择目的地启动。"
  )

S("Return to the normal game", "返回正常游戏")
S("three players", "三名玩家")
S("four players", "四名玩家")
S("key Orb power", "关键法球能力") //Unclear

S("Orb power (target: mouse)", "法球能力（目标：鼠标）")
S("Orb power (target: facing)", "法球能力（目标：面对）")
S("You cannot go against the wind!", "你无法逆风而行！")

// VERSION 8.2
// ============

S("The ivy attacks %the1!", "常春藤攻击%1！")

// heptagonal mode
// ----------------

S("heptagonal mode", "七边形模式")

S("\n\n(For the heptagonal mode, the radius has been reduced to 2 for closing plates.)",
  "\n\n（在七边形模式中，关闭压力盘的作用半径减小到2格。）")

// Hypersian Rug mode
// -------------------

S("hypersian rug mode", "超波斯地毯模式")

S(
  "In this mode, HyperRogue is played on a 3D model of a part of the hyperbolic plane, "
  "similar to one you get from the 'paper model creator' or by hyperbolic crocheting.\n\n",
  "在该模式中，HyperRogue是在以双曲的一部分制成的三维模型上游玩的，与以“纸模型制作器”或双曲钩编制成的模型类似。\n\n")

S("This requires some OpenGL extensions and may crash or not work correctly -- enabling "
  "the 'render texture without OpenGL' options may be helpful in this case. Also the 'render once' option "
  "will make the rendering faster, but the surface will be rendered only once, so "
  "you won't be able to play a game on it.\n\n",

  "这需要一些OpenGL扩展，可能会崩溃或不正常运行——在这一情况下打开“不使用OpenGL渲染纹理”可能有一定帮助。此外，“仅渲染一次”选项会使渲染变快，但表面只会被渲染一次，没办法用来玩游戏。\n\n")

S("Use arrow keys to rotate, Page Up/Down to zoom.", "以方向键旋转，Page Up/Down缩放")

S("what's this?", "这是什么？")
S("enable the Hypersian Rug mode", "开启超波斯地毯模式")
S("render the texture only once", "仅渲染一次纹理")
S("render texture without OpenGL", "渲染纹理时不使用OpenGL")
S("texture size", "纹理大小")

// Crossroads IV & Chaos Mode
// ---------------------------

N("Crossroads IV", GEN_N, "交叉路口 IV","")

S("Chaos mode", "混沌模式")

S("In the Chaos mode, lands change very often, and there are no walls between them. "
  "Some lands are incompatible with this."
  "\n\nYou need to reach Crossroads IV to unlock the Chaos mode."
  ,

  "在混沌模式中，地域常常改变，之间也没有墙壁分隔。有些地域与该模式不兼容。"
  "\n\n你需要到达交叉路口 IV来解锁混沌模式。"
)

// Rose Garden
// ------------

N("Rose Garden", GEN_N, "玫瑰园","")
N("False Princess", GEN_N, "假公主","位")
N("False Prince", GEN_N, "假王子","位")
N("Rose Lady", GEN_N, "玫瑰夫人","位")
N("Rose Lord", GEN_N, "玫瑰领主","位")
N("Rose Beauty", GEN_N, "玫瑰美人","位")
N("Handsome Gardener", GEN_N, "俊俏园丁","位")
N("rosebush", GEN_N, "玫瑰丛","片")

Orb("Beauty", "美丽")
N("Thornless Rose", GEN_N, "无刺玫瑰","朵")

S("Don't be fooled by this red-haired girl, or you will be stabbed if you come too close!",
  "别被这个红发女孩骗到，否则你走太近就会被捅！")
S("Don't be fooled by this red-haired boy, or you will be stabbed if you come too close!",
  "别被这个红发男孩骗到，否则你走太近就会被捅！")
S("This false princess is immune to the alluring scent of roses.",
  "这个假公主免疫玫瑰的诱惑香气。")
S("This false prince is immune to the alluring scent of roses.",
  "这个假王子免疫玫瑰的诱惑香气。")
S("She has flowers in her long fair hair. You could not bring yourself to attack such a beautiful woman.",
  "她的长发中别有花朵。你不忍心攻击如此美丽的女人。")
S("Tall, strong, and holding a flower in his hand. You could not bring yourself to attack such a handsome man.",
  "高大威猛，手持鲜花。你不忍心攻击如此帅气的男人。")

S(  "This Orb makes you stunningly beautiful. "
    "Monsters which come next to you will be stunned for one turn. "
    "Multi-tile monsters are not affected. Additionally, it makes you immune to "
    "beauty.",

    "这个法球令你美得沉鱼落雁。接近你的怪物会被击晕一回合。多格怪物不受影响。此外，它令你免疫美丽。")

S(  "A big, beautiful, magical flower.", "一朵大而美的魔法花朵。")

S(
  "Each eight turns, each rosebush at distance at most 5 from you will "
  "release a wave of alluring scent. Creatures on the frontwave "
  "will move towards where the scent came from. Even if it causes them "
  "to attack their friends or beautiful creatures, or move into water, fire, chasm, or thorns of the rosebush. "
  "Ivies, Ghosts, Rock Snakes, Rose Ladies and Lords, and monsters restricted to a specific "
  "terrain are immune to scents.",

  "每8个回合，每个在你至多5格距离内的玫瑰丛都会放出一股诱人香气。在波前处的怪物会向香气的来源移动，即使这会导致它们攻击朋友或美丽生物，或是走入水、火、裂缝或玫瑰丛的刺当中。常春藤、鬼魂、岩蛇、玫瑰夫人与领主，以及缚于特定地形的怪物免疫香气。")

S(
  "This land is filled with beautiful, but dangerous, creatures and plants.",

  "这片地域充满了美丽但危险的动植物。")

S("%The1 is killed by thorns!", "%1被荆棘杀死了！")

S("You just cannot stand in place, those roses smell too nicely.", "玫瑰实在香气逼人，你无法驻足不前。")
S("Those roses smell too nicely. You have to come towards them.", "玫瑰实在香气逼人，你只得向其前进。")

// Warped Sea/Coast
// -----------------

N("Warped Coast", GEN_N, "扭曲海岸","")
N("Warped Sea", GEN_N, "扭曲大海","")
N("Ratling", GEN_N, "鼠人","只")
N("Coral", GEN_N, "珊瑚","块")
Orb("the Warp", "扭曲")

S(
  "This land is warped. Ironically, the coast is completely straight...",
  "此地扭曲日久。讽刺的是，海岸倒完完全全是根直线……")

S(
  "This part of the world is warped, restricting the movement somewhat. "
  "\"Diagonal\" movement and attacking between triangular cells is not allowed. "
  "Flash, Storms, and Freedom spells ignore this, and Ghosts can move, attack, and "
  "be attacked diagonally.",
  "世界的这一部分被扭曲了，对行动产生影响。不能在三角格之间进行“对角”移动与攻击。闪光、风暴与自由咒语忽略这一限制，鬼魂也可以自由地对角移动、攻击，与被攻击。")

S( "This Orb creates a warped zone of radius 5 around you, "
  "and also allows you to move diagonally in warped zones.",

  "这个法球在你周围5格半径内创造出一片被扭曲的区域，并使你能够在扭曲区域中对角移动。")

S(  "Corals have a somewhat hyperbolic structure even in your home world, "
    "but natural corals from the Warped Sea have truly beautiful shapes. "
    "Ratlings know the value of corals, and thus keep them in boats for safety.",

    "珊瑚在你的家乡世界中就有一种有些类似双曲的构造，但在扭曲之海自然生长的珊瑚才真称得上美丽。鼠人了解珊瑚的价值，为了安全将它们保存在船中。")

S(
    "These warped humanoids are skilled warriors and sailors, and they "
    "feel at home at the Warped Coast. Their battle experience has taught them "
    "that enemies who wait without moving or attacking anything are the most deadly. "
    "If they see such an enemy, they become extremely suspicious, and they also wait.",

    "这些扭曲的类人生物是技艺高超的战士与水手，它们在扭曲海岸如鱼得水。它们的战斗经验告诉它们最致命的是那些不移动或攻击，而是进行等待的敌人。如果它们看到这样的敌人，它们会变得极其多疑，也进行等待。")


S("hexagonal #1", "六边形 #1")
S("You cannot move between the triangular cells here!", "你不能在这里的三角形格子间移动！")
S("display the triheptagonal grid", "显示截半正七边形网格")

N("no wall", GEN_N, "无墙","")
N("no monster", GEN_N, "无怪物","")
N("no item", GEN_N, "无物品","")

S("\n\nA Ghost never moves to a cell which is adjacent to another Ghost of the same kind.",
  "\n\n鬼魂从不移动至同类鬼魂的相邻格中。")

S("You cannot attack diagonally!", "你不能沿对角线攻击！")

Orb("Energy", "能量")

S(  "This Orb halves the power usage of orbs which cost some "
    "charges with each activation. It even affects the "
    "one-shot orbs such as Flash or Teleport. If such an activation "
    "normally costs x charges, it costs only x/2 (rounded up) "
    "if you have an Orb of Energy.",

    "这个法球减半其他每次启动时消耗一定能量的法球的消耗。它甚至可以影响一次性法球，比如闪光或传送。如果你持有能量法球，本来启动消耗x能量，现在改为消耗x/2能量（向上取整）。")

// missing....

N("warp gate", GEN_N, "扭曲之门","道")
S("This gate separates the warped area from the normal land.",
  "这道大门分隔正常地域与扭曲区域。")
S("You cannot move between the cells without dots here!",
  "你不能在此地没有圆点的格子间移动！")
S("Those roses smell too nicely. You can only target cells closer to them!",
  "那些玫瑰太好闻了。你只能瞄准临近它们的格子！")
S("You need to move to give space to %the1!",
  "你需要移动来给%1空出空间！")
// this should be genetive, but in Polish it's the same as %a1 for both Golem and Tame Bomberbird

N("Ratling Avenger", GEN_N, "鼠人复仇者","名")

S(
  "So, you have killed a Ratling on the unwarped sea? You will be punished for this! "
  "Luckily, if you run away from the Warped Sea quickly, the Ratling Avengers will lose track of you.",

  "你在无扭曲的海域杀死了鼠人？你会因此遭受惩罚！"
  "幸好，如果你迅速从扭曲大海逃走，鼠人复仇者会失去你的踪迹。")

// VERSION 8.3
// ============

S("Kills required: %1 (%2).\n", "所需击杀数：%1（%2）。\n")

S("\"Be careful in the Rose Garden! It is beautiful, but very dangerous!\"",
  "“要当心玫瑰花园！很美，但也很危险！”")

S("\"Thank you very much for talking, and have a great rest of your day!\"",
  "“多谢你和我聊天，祝你今天开心！”")

// Conformal/history mode
// ----------------------

S("conformal/history mode", "保角/历史模式")
S("Preparing the line (%1/1000)...", "准备线条（%1/1000）……")
S("Could not create an image of that size.", "无法创建该尺寸的图像。")
S("Saved the band image as: ", "保存带图片为：")//Unclear
S("right", "右")
S("left", "左")
S("up", "上")
S("down", "下")
S("counterclockwise", "逆时针")
S("clockwise", "顺时针")
S("zoom out", "缩小")
S("zoom in", "放大")
S("left to right", "从左到右")
S("right to left", "从右到左")
S("spin down", "向下旋转")
S("spin up", "向上旋转")
S("disk", "碟")
S("half-plane", "半平面")
S("band", "带")
S("polygonal", "多边形")
S("polynomial", "多项式")
S("include history", "包括历史")
S("model used", "使用的模型")
S("rotation", "旋转")

S("coefficient", "系数")
S("which coefficient", "哪个系数")
S("polygon sides", "多边形边数")
S("star factor", "星形系数")
S("degree of the approximation", "近似度")
S("prepare the line animation", "准备线动画")
S("animation speed", "动画速度")
S("render bands automatically", "自动渲染带")
S("band width", "带宽")
S("length of a segment", "段长度")
S("spiral on rendering", "渲染螺旋线")
S("render now (length: %1)", "立刻渲染（长度：%1）")
S("Enable cheat mode or GAME OVER to use this", "开启作弊模式或游戏结束时才能使用")

// remember to note it's in English
S("see http://www.roguetemple.com/z/hyper/models.php", "请浏览 http://www.roguetemple.com/z/hyper/models.php")

// Yendorian Forest
// ----------------

N("Yendorian Forest", GEN_N, "Yendor森林","")

S(
  "This forest was planted by one of the wizards from the Ivory Tower "
  "to conduct experiments with gravity.",

  "这片森林是由一位象牙塔的法师种植的，用来进行与重力相关的实验。")

N("Yendorian Researcher", GEN_N, "Yendor研究员","名")

S(
    "These people study gravity and infinite trees. "
    "They have no special features, other than wearing a strange hat.",

  "这些人研究重力与无穷树。他们没什么特别的，除了戴着一顶怪帽子。")

N("Sparrowhawk", GEN_N, "雀鹰","只")

S("A bird who hunts in the treetops of the Yendorian Forest.",
  "在Yendor森林树顶捕猎的鸟类。")

N("Apple", GEN_N, "苹果","颗")

S("A fruit from the Yendorian Forest.", "一种来自Yendor森林的水果。")

N("trunk", GEN_N, "树桩","个")

S("The skeleton of a tree.", "树的骷髅。")

N("solid branch", GEN_N, "坚实树杈","根")

S("Branches here could bear your weight easily.",
  "此地的树杈可以轻易支撑起你的重量。")

N("weak branch", GEN_N, "虚浮树杈","根")

S("Branches here will bear you weight, but if you use them to move (not fall) to an unstable place, they will break.",
  "这里的树杈可以支撑你的重量，但如果你借助它们移动（坠落除外）至不稳定的位置，它就会崩溃。")

N("canopy", GEN_N, "树冠","处")

S(
  "Only thin twigs and leaves here. They may bear fruits, but for you, these cells count as unstable.",
  "这里只有树枝与树叶。它也许能结出硕果，但对你来说，这些格子并不稳定。")

// Dragon Chasm
// ------------

N("Dragon Chasms", GEN_N, "巨龙裂隙","")

N("Dragon", GEN_N, "巨龙","条")

N("Dragon Scale", GEN_N, "龙鳞","片")

S(
    "Dragon Scales are a prized material for armors. "
    "They are also prized by collectors, who would like to boast "
    "about how they have killed a Dragon.\n\n"
    "Dragon Scales disappear after 500 turns.",

    "龙鳞是制作铠甲的上好材料。收藏家也珍重龙鳞，他们喜欢吹嘘自己有屠龙的功绩。\n\n"
    "龙鳞在500回合后消失。"
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

    "巨龙是强大的怪物。它们缓慢但邪恶，喜爱凌虐比它们行动更缓慢的生物。必须阻止它们！\n\n"

    "巨龙每两个回合移动一次。它可以用每节身体攻击，或将整个躯体前后移动，它也可以单独将前部向后移动。要想杀死巨龙，你需要攻击它的每一段。头部会在巨龙不移动的回合中自愈，所以你需要最后攻击头部；否则，如果头部处于健康状态，它可以喷吐烈焰（3格范围），失去一点生命值。在巨龙裂隙中杀掉巨龙会给予你宝藏奖励。"
   )

S("%The1 breathes fire at you!", "%1对你喷吐烈焰！")
S("%The1 breathes fire!", "%1喷吐烈焰！")

Orb("Domination", "统御")

S(
    "This Orb lets you ride Dragons and other worm-like creatures. "
   "Simply move onto such a creature to ride them; while riding, you are protected from dangerous terrains "
   "and partially from attacks (they cause you to lose half of your Domination power), "
   "but you cannot collect items.\n\n"
   /* "When only one charge is left, "
   "you have to dismount this turn -- be very careful to make this possible, "
   "as your mount could attack you immediately!\n\n" */ "While riding, "
   "click on a location to order your mount to move or attack there.",

   "这个法球使你能够骑乘巨龙等类蠕虫生物。只要移动到这些生物上即可骑乘；在骑乘时，你获得对危险地形的防护，并可以部分抵御攻击（被击中使你失去一半的统御能量），但你无法收集物品。\n\n"
   "骑乘时，点击一处来让你的坐骑移动或攻击那里。"
   )

S("You command %the1!", "你统御%1！")
S("You need to dismount %the1!", "你需要从%1上下来！")
S("%The1 tries to dismount you!", "%1试图将你甩下！")

S("Enemies killed: %1 (%2 types)", "击杀敌人数：%1（%2种）")
S(" kills: %1/%2", " 击杀数：%1/%2")
S("Different kills required: %1.\n", "所需不同击杀：%1。\n")

// Galapagos
// ---------

N("Galápagos", GEN_N, "加拉帕戈斯","")

N("Baby Tortoise", GEN_N, "小陆龟","只")

Orb("the Shell", "龟壳")

S(
    "This Orb protects you from physical attacks. "
    "It lasts for more turns than the Orb of Shielding, but "
    "10 charges are lost whenever you are attacked. "
    "It also does not protect you from fires, scents, and being eaten.",
    "这个法球可以帮你抵御物理攻击。它的持续时间比护盾法球更长，但每次你受到攻击时损失10点充能。它也不会帮你抵御火焰、香气，或被吞食。")

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

  "加拉帕戈斯是陆龟之岛。它们非常缓慢，使得巨龙可以轻易窃走吞食它们的幼崽。应该将小陆龟带回去，不过有些条件：陆龟有许多种类，取决于它们在加拉帕戈斯中所生活的位置——一共有21种二元环境因素，因此有2097152种。你得需要找到一只与小陆龟完全相同的大陆龟！ \n\n"
  "陆龟每3回合移动一次，攻击只能将它们击晕。\n\n"
  "将小陆龟带回家计作5 $$$。加拉帕戈斯中的区域有越多因素与小陆龟对应，区域在你的屏幕中就会显示得越亮。")

S("%The1 scares %the2 a bit!", "%1把%2吓到了！")
S("%The1 attacks your shell!", "%1攻击你的龟壳！")

S("Hyperstone Quest: collect at least %3 points in %the2",
  "双曲石任务：在%2收集至少%3分")

S("animals killed: %1", "击杀动物数：%1")
S("\n\nTortoises are not monsters! They are just annoyed. They do not count for your total kills.",
  "\n\n陆龟不是怪物！它们只是有点心烦。它们不计入你的总击杀数。")

S("Aww, poor %1... where is your family?", "哦哦，可怜的%1……你的家人呢？")
S("You are now a tortoise hero!", "你现在是一位陆龟英雄！")
S("Find a %1 in %the2.", "在%2找到一%m1%1。")

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

S("(includes co-op)", "（包括合作）")
S("Revived!", "复活！")

S("You hear a distant roar!", "你听到远处传来怒吼！")

S("turn count = %1 last exploration = %2 ghost timer = %3",
  "回合数 = %1 最后一次探索 = %2 鬼魂计时器 = %3")

S("You damage %the1 with a mental blast!", "你以灵能冲击伤害%1！")

S("You are now a tortoise heroine!", "你现在是一位陆龟女英雄！")

S("Note: currently scores are saved only in the normal mode on Android",
  "注意：目前在Android设备上只有普通模式能够保存分数")
S("Note: you can play, but scores won't be saved on Android",
  "注意：你可以进行游玩，但在Android设备上无法保存分数")
S("settings set here won't be saved", "在此处进行的设置无法保存")
S("-- use the Android menu instead", "——以Android菜单代替")

S("summon Bonfire", "召唤篝火")
S("Hyperstone Quest", "双曲石任务")
S("summon dead orbs", "召唤失效法球")
S("summon a Monster", "召唤一个怪物")
S("gain orb powers", "获得法球能力")
S("summon a Golem", "召唤一个魔像")
S("summon Thumpers", "召唤振鼓")
S("summon Ivy", "召唤常春藤")
S("lose all treasure", "失去所有宝藏")
S("gain kills", "获得击杀数")
S("Select the land ---", "选择地域 ---")
S("summon Mimics", "召唤模仿者")
S("summon orbs", "召唤法球")
S("deplete orb powers", "耗竭法球能量")
S("Safety (quick save)", "安全 （快速保存）")
S("summon treasure", "召唤宝藏")
S("summon lots of treasure", "召唤大量宝藏")
S("--- and teleport there", "--- 传送到那里")
S("summon Sand Worm", "召唤沙虫")
S("summon Orb of Yendor", "召唤Yendor法球")
S("rotate the character", "旋转角色")

S("gain Orb of Yendor", "获得Yendor法球")
S("switch ghost timer", "切换鬼魂计时器")
S("switch web display", "切换网页显示")

// just copy: "player 5 X"...
S("configure player 5", "设置玩家5")
S("configure player 6", "设置玩家6")
S("configure player 7", "设置玩家7")

S("player 5 X", "玩家5 X")
S("player 5 Y", "玩家5 Y")
S("player 5 go", "玩家5 移动")
S("player 5 spin", "玩家5 旋转")
S("player 6 X", "玩家6 X")
S("player 6 Y", "玩家6 Y")
S("player 6 go", "玩家6 移动")
S("player 6 spin", "玩家6 旋转")
S("player 7 X", "玩家7 X")
S("player 7 Y", "玩家7 Y")
S("player 7 go", "玩家7 移动")
S("player 7 spin", "玩家7 旋转")

// VERSION 9.0
//=============

// extra descriptions for Orbs and some other things
// -------------------------------------------------

S("\n\nThis Orb is triggered on your first attack or illegal move.",
  "\n\n这个法球在你进行首次攻击或非法行动时触发。")

S("\nYou can never target cells which are adjacent to the player character, "
     "or ones out of the sight range.",
     "\n你无法选择相邻玩家角色或在视野以外的格子。")

S("\nThis is a ranged Orb. ", "\n这是一个远程法球。")

S("\nRanged Orbs can be targeted by long touching the desired location.",
  "\n远程法球可以通过长按目标地点来启动。")

S("\nRanged Orbs can be targeted by touching the desired location.",
  "\n远程法球可以通过点击目标地点来启动。")
  
S("\nRanged Orbs can be targeted by shift-clicking the desired location. ",
  "\n远程法球可以通过按Shift时点击目标地点来启动。")

S("\nRanged Orbs can be targeted by clicking the desired location. ",
  "\n远程法球可以通过点击目标地点来启动。")
  
S("\n\nOnce you collect 10 Bomberbird Eggs, "
  "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
  "Collecting even more Eggs will increase the radius. Additionally, collecting "
  "25 Bomberbird Eggs will reveal adjacent cells even in your future games.",
  
  "\n\n在你收集10个炸弹鸟蛋后，步入无相邻地雷的格子会同时揭示相邻的格子。收集更多的蛋来增加揭示范围。此外，收集25个炸弹鸟蛋会在你随后的每次游戏中都揭示相邻格子。")

S("This might be very useful for devices with limited memory.",
  "这也许对内存有限的设备非常有用。")
  
S("(in the MENU). You can reduce the sight range, this should make the animations smoother.",
  "（在菜单中）。你可以减少视野范围，让动画变得更流畅。")

S("Unavailable in the shmup mode.\n", "在射击模式不可用。\n")
S("Unavailable in the multiplayer mode.\n", "在多人模式不可用。\n")
S("Unavailable in the Chaos mode.\n", "在混沌模式不可用。\n")
S("Bonus land, available only in some special modes.\n",
  "额外地域，只在一些特殊模式中可用。\n")

S("\n\nThis Orb protects you from attacks, scents, and insulates you "
  "from electricity. It does not let you go through deadly terrain, but "
  "if you are attacked with fire, it lets you stay in place in it.",
  
  "\n\n这个法球助你抵御攻击、香气，并使你绝缘。它不能让你穿过致命的地域，但如果你被敌人以火焰攻击，你可以安全地待在火里。")

S("Completing the quest in this land is not necessary for the Hyperstone Quest.",
  "完成该地域的任务并非双曲石任务所需。")

// other options
// -------------

S("player", "玩家")
S("movement color", "移动颜色")
S("model used (not conformal!)", "使用的模型（非保角！）")
S("azimuthal equidistant", "方位角等距")
S("azimuthal equi-area", "方位角等面积")
S("visit the website", "访问网站")
S("leaderboards/achievements", "排行榜/成就")
S("return to the game", "返回游戏")

// Android texts
// -------------

S("connected to Google Games", "连接至Google游戏")
S("background music", "背景音乐")
S("view your achievements", "检视你的成就")
S("next page", "下一页")

// new multiplayer
// ---------------

S("Two players cannot move/attack the same location!", "两名玩家不能攻击/移动选定相同位置！")
S("Cannot move into the current location of another player!", "无法移动到另一名玩家当前的位置！")
S("Players cannot get that far away!", "玩家之间不能相距过远！")
S("Cannot push into another player!", "无法推入另一位玩家！")
S("Cannot push into the same location!", "无法推入相同位置！")

S("%The1 throws fire at %the2!", "%1对%2投掷火焰！")
S("%The1 breathes fire at %the2!", "%1对%2喷吐火焰！")

S("configure keys", "设置键位")
S("use default keys", "使用默认键位")

S("Left the game.", "离开游戏。")
S("shoot'em up mode", "射击模式")
S("turn-based mode", "回合制模式")
S("continue playing", "继续游戏")
S("start a new game", "开始新游戏")

S("five players", "五位玩家")
S("six players", "六位玩家")
S("seven players", "七位玩家")

S("You collect %the1. (%2)", "你收集了%1。（%2）")


S(
"Shmup (shoot'em up) mode: You can play a hyperbolic shoot'em up game. The game is based "
"on the usual turn-based grid-based HyperRogue, but there are some changes. You fight by "
"throwing knives, and you have three extra lives. There are no allies, so all Orbs "
"related to allies give you extra lives instead (up to 5). Some other rules have been "
"adapted too.\n\n",

"射击模式：你可以在这款双曲飞行射击游戏中游玩。游戏基于HyperRogue的基础回合网格制，但有些改动。你以飞刀进行游戏，且有额外的三条生命。没有盟友，盟友相关的法球改为给你额外的生命（至多5条）。有些其他规则也有一定改动。\n\n")

S(
"Multiplayer: Play cooperatively (locally); treasures, kills, and deaths are calculated "
"for each player too, for more competitive play. Orbs and treasures are shared, orbs drain "
"faster, knives recharge slower, and player characters are not allowed to separate.\n\n",

"多人模式：合作进行游戏（本地）；每位玩家分别计数宝藏、击杀、死亡，鼓励竞争。法球与宝藏效果共享，法球充能消耗更快，飞刀充能更慢，玩家角色不能相互远离。\n\n")

S(
"Turn-based multiplayer: Turns are executed in parallel. A player can leave the game "
"by pressing a designated key (useful when about to get killed or lost). The following "
"Orbs work to bring such players back: ",
"回合制多人模式：各个玩家的回合一同发生。玩家可以通过按设置好的某个键退出游戏（在快要被杀或输掉时很有用）。以下法球可以将这些玩家复活：")

S("This menu can be also used to configure keys.\n\n",
  "这一菜单也可用于设置按键。")

// other stuff
// -----------

S("Warnings are issued when you try to do something that appears dangerous, "
  "like stepping on a known mine, or getting your boat destroyed by "
  "a Kraken without having Orb of the Fish. In some cases the action "
  "might actually be safe -- so you can ignore the warning and do it anyway, "
  "simply by repeating the action.",
  
  "在你要做某件看起来有危险的事时，游戏会发出警告，比如踩到已知的地雷上，或者在没有游鱼法球时令你的船被海妖摧毁。有些情况下，这种行为其实是安全的——你可以通过再次重复该动作来忽略警告。")


// Kraken Depths
// -------------

N("Kraken Depths", GEN_N, "海妖深洋","")

S(
    "A long time ago, this was a trade route. But then, Krakens have risen out of the "
    "depths. Many trading ships sank here. Legend says that you can uncover the secret "
    "of a magical weapon spell somewhere in the depths...\n\n"
    
    "You can find Sunken Treasures here, but they won't appear until you have killed "
    "a Kraken. You will also need Orb of the Fish to get the treasures, luckily you can "
    "steal one from the Viking treasure hunters.",
    
    "很久很久以前，这里曾经是一条贸易路线。但随后，有海妖自深渊浮现。许多商船沉没于此。传说你可以在深渊中寻找到魔法武器咒语……\n"
    "你可以在此地找到沉没宝藏，但它们不会在你击杀一只海妖之前出现，幸好你还能从那些维京宝藏猎人处偷到。")

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
  
  "你的家乡也有海妖……能够轻易摧毁船只的巨大海怪。这个世界奇怪的几何使得庞大的物体无法迅速移动，因此不存在大船，只有小艇，而双曲海妖看起来也比较小。尽管如此，你还是觉得它们是能以此种速度移动的生物中最大的那一种。\n\n"
  
  "海妖头只能在六边形上移动。你需要攻击其所有的触手才能杀掉海妖。如果触手本回合内被攻击过，它就不能发起攻击。如果海妖攻击你时你在船上，你的船会被摧毁，但你不会死。"
  )

Orb("the Sword", "宝剑")

S(
    "This Orb gives you a weapon made of pure magical energy. You do not hold "
    "it, it simply floats in the air next to you. When you go, the energy sword moves "
    "with you, pointing at the same relative angle it pointed before -- you cannot "
    "move or rotate it otherwise. Most monsters can be killed by moving the sword into them, "
    "and won't move into the spot with the sword.",
    
    "这个法球赋予你一把以纯粹魔能构成的武器。你不需要握住它，它浮动在你身旁的空中。在你移动时，这把能量剑")

N("Sunken Treasure", GEN_N, "沉没宝藏","份")

S("Cargo of a ship which was once destroyed by a Kraken.", 
  "曾属于被海妖摧毁的船只的货物。")

// Burial Grounds
// --------------

N("Burial Grounds", GEN_N, "埋骨地","")

S(
  "Ancient Viking heroes were buried here. Their graves have barrows raised over "
  "them, and are guarded by Draugar, animated corpses who are immune to mundane weapons. "
  "You will need to use a magical weapon spell to defeat them, and to rob the "
  "ancient jewelry buried in the graves.",
  
  "远古的维京英雄埋葬于此。他们的坟墓有拱起的坟包，受到尸鬼守护。尸鬼是活化的遗骸，免疫凡俗的武器。你得需要魔法武器咒语才能击败它们，掠夺坟墓中埋藏的远古珠宝。")

N("Draugr", GEN_N, "尸鬼","个")

S(
  "Animated corpses of ancient Viking warriors. They are immune to mundane weapons, "
  "but they can be destroyed by your Orb of the Sword.",
  "活化的远古维京战士遗骸。它们免疫寻常武器，但可以被宝剑法球摧毁。")

Orb("the Sword II", "利剑")

S(
  "An alternative version of Orb of the Sword. If you have both of them, "
  "you have two energy swords, facing in opposite directions.",
  "另一种宝剑法球。如果你两种都有，你就会有两把能量剑，面对相反的方向。"
  )

N("Ancient Jewelry", GEN_N, "远古珠宝","份")

S(
  "Precious belongings of ancient Viking heroes. Your Orb of the Sword can be "
  "used to dig these treasures out of the barrows.",
  "属于远古维京英雄的珍贵财产。你的宝剑法球可以将这些财宝从坟包中挖掘出来。")

S("%The1 destroys your boat!", "%1摧毁了你的船！")
S("Your mundane weapon cannot hurt %the1!", "你的凡兵无法伤到%1！")
S("This move appears dangerous -- are you sure?", "这一行动看起来很危险——你确定要这样做吗？")
S("You slash %the1.", "你挥砍%1。")
S("You chop down %the1.", "你砍倒%1。")
S("Cannot slash another player!", "无法挥砍另一位玩家！")
S("%The1 suffocates!", "%1窒息了！")

N("barrow wall", GEN_N, "坟包墙","堵")

S("This wall is quite strong. You will need another way in.",
  "这堵墙相当坚固。你得找条其他路进去。")

N("barrow", GEN_N, "坟包","座")

S("Your Orb of the Sword can be used to dig here.", 
  "你的宝剑法球可以用来在此挖掘。")

// Trollheim
// ---------

N("Trollheim", GEN_N, "巨魔乡","")

S(
    "Many clans of Trolls spend their lives in this kingdom. You can find many "
    "statues of Trolls here. You suppose that they are not actually statues, but simply "
    "elderly Trolls, who have petrified upon death. Or maybe you have killed "
    "these Trolls yourself?",
    
    "许多巨魔部落在这一片王国中生活。你可以在这里找到许多巨魔雕像。你想那些雕像其实不是雕像，而是老死后石化的巨魔。还是说那些是你杀掉的巨魔来着？")


N("Golden Egg", GEN_N, "金蛋","枚")

S(
    "Trolls of Trollheim are descendants of a bridge Troll, who collected "
    "payments from people crossing the bridge. One of them paid with "
    "golden eggs. The bridge Troll found the eggs beautiful, but he quickly lost them. "
    "Golden eggs are still revered by Trolls, and you can find them in their "
    "caves.",
    
    "巨魔乡的巨魔是一只收集过桥费的桥梁巨魔的后裔。有个过桥的人以金蛋付费。桥梁巨魔觉得金蛋很好看，但它很快丢失了那些金蛋。巨魔们依然珍重金蛋，你可以在它们的洞穴中找到。")

Orb("the Stone", "岩石")

S(
    "Trolls turn into stone walls when they die. When you have this Orb, "
    "this happens to every monster you defeat. Statues created from this Orb "
    "have slightly different properties than Trolls who petrify naturally.",
    
    "巨魔死后变成石墙。你持有这一法球时，你击败的每个怪物都会变成石头。这个法球产生的雕像与自然石化的巨魔性质略有差异。")

N("stone statue", GEN_N, "石雕像","座")

S("A petrified creature.", "被石化的生物。")

S("mark heptagons", "标记七边形")
S("help for keyboard users", "便于键盘操作")

// missing
// -------

S("You leave %the1.", "你留下%1。") // Baby Tortoise

S("%The1 says, \"I die, but my clan in Trollheim will avenge me!\"",
  "%1说：“我虽死，但我巨魔乡中的族人会为我报仇！”")

S("After killing %the1, you feel able to reach the Elemental Planes!",
  "在击杀%1后，你感到自己可以去往元素位面了！")

S("You feel that a magical weapon is waiting for you...",
  "你感到一柄魔法武器正等待着你……")

// missing
// -------

S("reset per-player statistics", "重置玩家统计数据")
S("An alternate layout of the Crossroads, without walls.", "交叉路口的又一种构造，没有墙壁。")
S("cheats", "作弊")

S("Known mines may be marked by pressing 'm'. Your allies won't step on marked mines.", 
  "已知的地雷可以按“m”标记。你的盟友不会踩到标记过的地雷上。")

S("Known mines may be marked by touching while in drag mode. Your allies won't step on marked mines.", 
  "已知的地雷可以在拖动模式中点击标记。你的盟友不会踩到标记过的地雷上。")

// 9.0 patches
// ===========

// extra help
// ----------

S("Coastal region -- connects inland and aquatic regions.\n",
  "沿海区域 -- 连接内陆与海洋区域。\n")

S("Aquatic region -- accessible only from coastal regions and other aquatic regions.\n",
  "海洋区域 -- 仅能自沿海区域及其他海洋区域抵达。\n")

// barrow treasure protected from Orb of Space
// -------------------------------------------

S("%The1 is protected from this kind of magic!",
  "%1具有对这种魔法的防护！")

// Orb is always available in the given land
// -----------------------------------------

S("always available", "总是可用")

S("\n\nAfter the Trolls leave, you have 750 turns to collect %the1, or it gets stolen.",
  "\n\n在巨魔离开后，你有750个回合收集%1，随后它就会被盗走！")

S(
    "There are many lands in HyperRogue. Collect 10 treasure "
    "in the given land type to complete it; this enables you to "
    "find the magical Orbs of this land, and in some cases "
    "get access to new lands. At 25 treasures "
    "this type of Orbs starts appearing in other lands as well. Press 'o' to "
    "get the details of all the Lands.\n\n",
    
    "HyperRogue中有许多地域。在每个特定地域收集10个宝藏就能将其完成；这使你能在该地域寻找到原生法球，并且有时能够去往新的地域。在收集25个原生宝藏后，其他地域也会开始出现这种法球。按“o”查看所有地域的具体信息。\n\n"
    )

// welcome messages
// ----------------

S("Welcome to the Random Pattern mode!", "欢迎来到随机图案模式！")
S("You are playing %the1 in the Pure Tactics mode.", "你在纯粹战术模式中游玩%1。")
// %1 is the number
S("Welcome to the Yendor Challenge %1!", "欢迎来到Yendor挑战%1！")
S("Welcome to HyperRogue!", "欢迎来到HyperRogue！")
S("Press F1 or right-click things for help.", "对物体按F1或鼠标右键以检视帮助。")
// for iOS
S("Press F1 or right-shift-click things for help.", "对物体按F1或Shift与鼠标右键以检视帮助。")
S("Welcome to the Euclidean mode!", "欢迎来到欧几里得模式！")
S("Good luck in the elliptic plane!", "祝你在椭圆平面上好运！")
S("Welcome to Spherogue!", "欢迎来到Spherogue！")

// extra help for joystick configuration
// -------------------------------------

S("joystick configuration", "摇杆设置")
S("first joystick position (movement)", "摇杆一位置（移动）")
S("second joystick position (panning)", "摇杆二位置（视角移动）")
S("old style joystick configuration", "旧版摇杆设置")

S("move up-right", "上右移动")
S("move up-left", "上左移动")
S("move down-right", "下右移动")
S("move down-left", "下左移动")
S("stay in place (left + right)", "原地不动（左+右）")
S("cancel move", "取消移动")
S("leave the game", "离开游戏")
S("drop Dead Orb (up + down)", "丢下失效法球（上+下）")

// extra help for configuration / projection and geometry
// ------------------------------------------------------

S("Roughly 42% cells are on the edge of your sight range. Reducing "
  "the sight range makes HyperRogue work faster, but also makes "
  "the game effectively harder.",
  "显示的大约42%格子在你的视野边缘。 降低视野范围使得HyperRogue运行更快，但可能导致游戏难度提高。")

S("Watch the Minkowski hyperboloid or the hypersian rug mode with the "
  "red/cyan 3D glasses.",
  "以红蓝3D眼镜观看闵可夫斯基双曲体或超波斯地毯模式。")

S("0 - return", "0 - 返回")
S("go back", "返回")
S("F1 - help", "F1 - 帮助")

// for the conformal polynomial
S("coefficient (imaginary)", "系数（虚）")
S("Scale the displayed model.", "缩放显示的模型。")
S("Reenter HyperRogue to apply this setting", "重新启动HyperRogue以使用该设置")

S(  "HyperRogue uses the Minkowski hyperboloid model internally. "
    "Klein and Poincaré models can be obtained by perspective, "
    "and the Gans model is obtained by orthogonal projection. "
    "See also the conformal mode (in the special modes menu) "
    "for more models.",
    
    "HyperRogue内部使用闵可夫斯基双曲体模型。克莱因与庞加莱模型通过调整视角得到，而Gans模型通过正交投影得到。请检视保角模式（在特殊模式菜单中）来查看更多模型。")

S("stereographic projection", "球极投影")
S("Poincaré model", "庞加莱模型")
S("gnomonic projection", "球心投影")
S("Klein model", "克莱因模型")
S("towards orthographic", "朝向正交")
S("towards Gans model", "朝向Gans模型")

S("Euclidean/elliptic mode", "欧几里得/椭圆模式")
S("geometry", "几何")
S("spherical", "球形")
S("elliptic", "椭圆")
S("Euclidean", "欧几里得")
S("projection", "投影")

// VERSION 9.1
// ===========

N("Familiar", GEN_N, "魔宠","只")
S("Commanded %the1!", "指挥了%1！")
S("F8 = SVG shot", "F8 = SVG截图")
S("Saved the SVG shot to %1 (sightrange %2)", "SVG截图保存至%1（视野范围 %2）")

S("draw circle around the target", "在目标周围绘制圆圈")
S("reverse pointer control", "反转指针控制")
S("sight range", "视野范围")
S("configure input", "配置输入")

// quote of Euclid
S("\"There is no royal road to geometry.\"",
   "“在几何里，没有专为国王铺设的大道。”")

// quote of Lobachevsky
S("\"There is no branch of mathematics, however abstract, ",
  "“任何一门数学分支，不管它多么抽象，")
  
S("which may not some day be applied to phenomena of the real world.\"",
  "总有一天会在现实世界中找到应用。”")
  
// quote of Gauss
S("\"It is not possession but the act of getting there, ",
  "“给我最大欢乐的，不是已到达的高度，")
  
S("which grants the greatest enjoyment.\"", "而是继续不断的攀登。”")

// quote of Escher
S("\"We live in a beautiful and orderly world, ",
  "“我们生活在美丽而充满秩序的世界中，")

S("and not in a chaos without norms.\"",
  "而非在一团毫无规律的混沌中。”")

/*
		"NEW_ACHIEVEMENT_7_21_NAME"	"Deadly Death"
		"NEW_ACHIEVEMENT_7_21_DESC"	"Defeat Goblin with an energy sword."
		"NEW_ACHIEVEMENT_7_22_NAME"	"Halloween Witch"
		"NEW_ACHIEVEMENT_7_22_DESC"	"Collect 50 Treats in Halloween."
		"NEW_ACHIEVEMENT_7_23_NAME"	"Halloween Vampire"
		"NEW_ACHIEVEMENT_7_23_DESC"	"Colelct 100 Treats in Halloween."
*/

S("You have enough treasure now to revive %the1!",
  "你目前的宝藏不足以复活%1！")

S("Killed %1 can be revived with an Orb of Love, after you collect 20 more $$$.",
  "在你收集20 $$$或更多后，死去的%1可以用一枚爱情法球复活。"
  )

S("%The1 will be revivable at %2 $$$", "%1在%2 $$$时可以复活")

S("Halloween mini-game", "万圣节小游戏")


N("Vampire Bat", GEN_N, "吸血蝙蝠","只")
N("Halloween", GEN_N, "万圣节","")
N("Treat", GEN_N, "糖果","颗")

S(
  "Halloween is a special land, that is available only in the spherical "
  "or elliptic geometry (press 'o' to switch). You play on the surface of "
  "a jack-o'-lantern, "
  "and have to collect as many Treats as possible. Each Treat you collect "
  "brings new monsters to fight, and new magical powers for you. You "
  "have to fight the monsters while effectively managing your limited "
  "resources.",
  
  "万圣节是一处仅在球形或椭圆几何（按“o”切换）中可用的特殊地域。你在一盏南瓜灯的表面游玩，要尽可能多地收集糖果。你收集到的每个糖果都会使新的怪物加入战斗，而你会获得新的魔法力量。你需要在与怪物战斗的同时有效地管理你有限的资源。"
  )

S(
  "Vampire Bats don't attack normally, but they drain your magical powers if "
  "they are at distance at most 2 from you.",
  "吸血蝙蝠并不以正常方式攻击，而是会在距离你至多2格时汲取你的魔法力量。")

S("Ghosts can move through chasms!", "鬼魂可以穿越裂隙！")
S("Push Skeletons into the holes!", "把骷髅推入洞里！")
S("You'll need your magical sword against the Draugar!", "你需要魔法剑来对付尸鬼！")
S("Demons are slow, but you'll need the experience against stronger ones...",
  "恶魔行动缓慢，但你需要经验才能对付比较强大的恶魔……")
S("You will need more experience to defeat the Greater Demon!",
  "你需要更多的经验才能击败高等恶魔！")
S("Cultists throw fire at you from distance!",
  "邪教徒能在远处朝你投掷烈焰！")
S("Defeat Flail Guards by moving away from them.",
  "从连枷守卫旁移开来将他们击败。")
S("Vampire Bats drain your magical powers!",
  "吸血蝙蝠汲取你的魔法力量！")

S("Hint: use arrow keys to scroll.", "提示：使用方向键翻页。")
S("Hint: press 1 2 3 4 to change the projection.", "提示：按1 2 3 4改变投影。")
S("You gain a protective Shell!", "你获得了一层保护罩！")
S("Hint: magical powers from Treats expire after a number of uses.",
  "提示：来自糖果的魔力在使用一定次数后消散。")
S("A magical energy sword. Don't waste its power!",
  "一柄魔法剑。不要浪费它的能量！")
S("Another energy sword!", "又一柄魔法剑！")
S("You got Thorns! Stab monsters by moving around them.",
  "你长出荆棘！在怪物旁移动来戳刺它们。")
S("Aethereal powers let you go through the holes.",
  "以太之力使你能够越过这些洞。")
S("The tasty treat increases your protection.",
  "好吃的糖果增强了你的防护力。")
S("You gain your protective Shell back!", "你的保护罩恢复了！")

S("rainbow landscape", "彩虹地貌")
S("Cannot use %the1 here!", "不能在此使用%1！")
S("draw the grid", "绘制网格")
S("Escher/3D", "埃舍尔/3D")
S("plain/3D", "平面/3D")
S("3D", "3D")
S("Welcome to Halloween!", "欢迎来到万圣节！")

S("How long should the messages stay on the screen.",
  "信息在屏幕上停留的时间")
S("select this color", "选择该颜色")
S("sound effects volume", "音效音量")

S("\n\nSee sounds/credits.txt for credits for sound effects", 
  "\n\n浏览sounds/credits.txt查看音效鸣谢")

S("scrolling speed", "屏幕翻页速度")
S("movement animation speed", "移动动画速度")
S("+5 = move instantly", "+5 = 立即移动")
S("extra graphical effects", "额外图像效果")

// VERSION 9.3
// ===========

S("SORT", "排序")
S("PICK", "选择")
S("PLAY", "开始")

// 3D configuration
S("3D configuration", "3D设置")
S("High detail range", "高细节范围")
S("Mid detail range", "中细节范围")
S("Camera level above the plane", "平面之上相机高度")
S("Ground level below the plane", "平面之下地面高度")
S("Projection at the ground level", "地面高度投影")
S("Height of walls", "墙高度")
S("Rock-III to wall ratio", "岩石-III与墙的比例")
S("Human to wall ratio", "人与墙的比例")
S("Level of water surface", "水面高度")
S("Level of water bottom", "水底高度")
S("set 3D monsters or walls in basic config first",
  "首先在基础设置中设定3D怪物或墙")
S("parameters set correctly", "参数配置正确")

S("Objects at distance less than %1 absolute units "
  "from the center will be displayed with high "
  "detail, and at distance at least %2 with low detail.",
  
  "在绝对距离%1单位内的物体会以高细节显示，而在距离至少为%2单位以外以低细节显示。")

S("Ground level is actually an equidistant surface, "
  "%1 absolute units below the plane P. "
  "Theoretically, this value affects the world -- "
  "for example, eagles could fly %2 times faster by "
  "flying above the ground level, on the plane P -- "
  "but the actual game mechanics are not affected. ",

  "地表其实是一个等距平面，在P平面以下%1个绝对单位。理论上来说，这个值会影响世界——比如，雄鹰在地面以上的P平面飞行，速度应该是寻常的%2倍——但并不影响游戏机制。")
  
  
S("(Distances reported by the vector graphics editor "
  "are not about points on the ground level, but "
  "about the matching points on the plane P -- "
  "divide them by the factor above to get actual "
  "distances.)",
  
  "（向量图形编辑器反馈的距离并非相对于地面高度上的点，而是平面P上的对应点——除以系数得到实际距离。）")

S( "If we are viewing an equidistant g absolute units below a plane, "
   "from a point c absolute units above the plane, this corresponds "
   "to viewing a Minkowski hyperboloid from a point "
   "tanh(g)/tanh(c) units below the center. This in turn corresponds to "
   "the Poincaré model for g=c, and Klein-Beltrami model for g=0.",
   
   "如果我们自某平面以上c个绝对单位的点观察平面以下g个绝对单位的等距面，这与自中心以下tanh(g)/tanh(c)单位的一点观察闵可夫斯基双曲体等价。这也与庞加莱模型g=c、克莱因-贝尔特拉米模型g=0等价。")

S( "The height of walls, in absolute units. For the current values of g and c, "
   "wall height of %1 absolute units corresponds to projection value of %2.",
   
   "墙的绝对单位高度。对于当前g与c的值，墙的绝对高度%1等价于投影值%2。")

S( "The ratio of Rock III to walls is %1, so Rock III are %2 absolute units high. "
   "Length of paths on the Rock III level is %3 of the corresponding length on the "
   "ground level.",
   
   "岩石 III与墙的比例为%1，因此岩石 III的绝对单位高度为%2。在岩石 III高度处的路径长度等价于地面长度的%3倍。")

S( "Humans are %1 "
   "absolute units high. Your head travels %2 times the distance travelled by your "
   "feet.",
   
   "人类高为%1单位。你头部运动的距离为你的脚步运动距离的%2倍。")
  
// Euclidean regular patterns
// --------------------------

S("three colors", "三色")
S("three colors rotated", "三色旋转")
S("edit all three colors", "编辑三种颜色")

S(" (local treasure)", "（本地宝藏）")
S(" (increases treasure spawn)", "（增加宝藏刷新率）")
S(" (appears here)", "（在此出现）")
S("%The1 is destroyed!", "%1被摧毁了！") // generic message

S("page", "页")
S("all", "全部")

S("Your Orb of Recall is blocked by something big!",
  "你的唤回法球之力被什么大东西阻挡了！")

S("You are recalled!", "你被唤回了！")
S("You vault over %the1!", "你翻越过%1！")
S("You cannot attack %the1!", "你无法攻击%1！")

S("\n\nFast flying creatures may attack or go against gravity only in their first move.",
  "\n\n快速移动的飞行生物可以在其首次行动时攻击或逆重力而动。")

// Dungeon
// -------

N("Dungeon", GEN_N, "地牢","")

S("The result of a collaboration of the Great Vizier and the Wizard of the Ivory Tower.",
  "大维齐尔与象牙塔的法师的合作产物。")

N("Slime Mold", GEN_N, "史莱姆霉","颗")

S("A very interesting species of slime mold.",
  "某种特别有趣的史莱姆霉。")

N("Bat", GEN_N, "蝙蝠","只")

S("Someone has told you that one can get battle experience safely by "
  "killing tons of essentially harmless creatures, such as Bats. But "
  "does this make any sense?...\n\n"
  
  "It does not. Bats cannot hurt you, but may block your movement, or "
  "toggle switches if they fall on them.",
  
  "有人曾经告诉你，你可以通过杀掉一大堆基本无害的生物来安全地获取战斗经验。这对吗……\n\n"
  
  "不对。蝙蝠伤不到你，但也许会阻挡你移动，或是在死掉的时候触发身下的开关。"
  )

Orb("Recall", "唤回")

S("When the charges on this Orb expire, "
  "you will be automatically returned to the place where you have found it. "
  "Extra Orbs of Recall delay this without changing the recall location. "
  "Picking up an Orb of Safety causes an immediate recall.",
  
  "在这个法球的充能耗尽时，你会被自动传送回发现它的地方。捡到多余的法球只延长唤回的时间，而不改变唤回的位置。捡起安全法球会立即将你唤回。"
  )

// 
//

// Lost Mountain
// -------------

N("Lost Mountain", GEN_N, "失落山脉","")
  
S(
  "Gravitational anomalies in the Jungle create mountains "
  "overgrown with ivies and bushes. "
  "Will you dare to climb the ivies to get the amethysts hidden above?\n\n"
  
  "Cells adjacent to Ivies count as stable (but Ivies "
  "cannot climb themselves or other Ivies).",
  
  "丛林中的重力异常产生了长满常春藤与灌丛的山脉。你敢攀爬常春藤来取得隐藏在高处的紫水晶吗？\n\n"
  
  "在常春藤旁的格子视为稳定（但常春藤不能攀爬自己或其他常春藤）。"
  )
  
N("big bush", GEN_N, "大灌丛","片")

S("You can hold this bush to climb the Lost Mountain. "
  "Bushes block the movement of birds.",
  "你可以抓住这片灌丛来攀爬失落山脉。灌丛阻挡鸟的移动。"
  )

S("You can hold this bush to climb the Lost Mountain, "
  "but it is not very strong -- it will get destroyed "
  "if you climb from it into an unstable location. "
  "Bushes block the movement of birds.",
  
  "你可以抓住这片灌丛来攀爬失落山脉，但它不是特别稳固——如果你借助它爬到不稳定的位置，就会摧毁它。灌丛阻挡鸟的移动。")

N("Amethyst", GEN_N, "紫水晶","枚")

S("A beatiful purple gem from the Lost Mountain.",
  "来自失落山脉的紫色宝石。")

Orb("Nature", "自然")
N("Friendly Ivy", GEN_N, "友善常春藤","段")

S("This Orb allows you to grow like an Ivy. "
  "The Ivy is always rooted in your current location; "
  "moving among the Ivy cells will move the root. "
  "Moving to a new location will cause the Ivy to grow "
  ", if an Ivy could make that movement "
  "(otherwise it breaks). "
  "You can also target one of the cells adjacent to your ivy "
  "(not to you) to grow or attack there.",
  
  "这个法球让你能像常春藤一样生长。常春藤总是在你当前位置扎根；沿着常春藤格子移动会移动根。按照常春藤的移动方式移动到新位置会使常春藤生长（否则会破坏常春藤）。你也可以指定相邻你常春藤（而非相邻你）的格子来在此生长或攻击。"
  )

// Reptiles
// --------

N("Reptiles", GEN_N, "爬行","")
N("Reptile", GEN_N, "爬行类","只")
N("Reptile floor", GEN_N, "爬行类地板","块")
N("Reptile bridge", GEN_N, "爬行类桥","段")

S("These reptiles are quite strange creatures. They "
  "spend most of their lives sleeping as floors "
  "that other creatures can walk on. "
  "Sometimes they wake up to hunt their prey, "
  "but they will happily go back to sleep if they "
  "happen to move into a hole on their way. "
  "Your attacks do not kill the Reptiles, but "
  "you can push and stun them.",
  
  "这些爬行类是很奇怪的生物。它们大多数时间作为其他生物能够行走的地板休眠。有时它们醒来捕猎，但如果路上掉到坑里，它们又会开心地回到梦乡。你的攻击无法杀死爬行类，但可以将它们推走并击晕。"
  )

N("Dodecahedron", GEN_N, "十二面体","枚")
S("These dodecahedra made of a mysterious material are the Reptiles' favorite toy.",
  "这些由某种神秘材料制成的十二面体是爬行类最喜欢的玩具。")

Orb("Vaulting", "翻越")

S("This Orb allows you to jump over an adjacent monster, killing or stunning it. "
  "You can only vault in a roughly straight line. "
  "Target a cell on the other side to use it.",
  
  "这个法球使你能够跳过一个相邻的敌人，同时击杀或击晕它。你只能大概在一条直线上翻越。选中怪物另一侧的格子来使用。"
  )

S("inventory/kill sorting", "物品栏/击杀排序")
S("first on top", "旧 置顶")
S("first on bottom", "旧 沉底")
S("last on top", "新 置顶")
S("last on bottom", "新 沉底")
S("by land", "根据地域")
S("by number", "根据数字")

N("small bush", GEN_N, "小灌丛","片")

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
  "你感到这里的重力有些奇怪……")
S(
  "The rug depicts a man in a deep dungeon, unable to leave.",
  "地毯描绘着一位在深深的地牢中无法离开的男人。")
S(
  "You feel attuned to gravity, ready to face mountains and dungeons.",
  "你感到自身与重力调谐，准备好了面对山脉与地牢。")

// VERSION 9.4
// ===========

// not previously translated
S("shift+O to switch anti-aliasing", "Shift+O切换抗锯齿")
S("For some reason... cannot attack!", "不知为何……无法攻击！")
S("Your energy swords get stronger!", "你的能量剑变强了！")
S("You feel even more attuned to the magic of this land!",
  "你感到与这一地域的魔法更加调谐了！")
S("Quite tough, for your first fight.", "作为你的首场战斗，这可真激烈。")
S("(You can also use right Shift)\n\n", "（你也可以使用右Shift）\n\n")

// Crossroads V
// ------------

N("Crossroads V", GEN_N, "交叉路口 V","")
S("Extremely narrow Crossroads layout.\n", "非常狭窄的交叉路口变体。\n")

// Bull Dash/Prairie common
// ------------------------

N("Sleeping Bull", GEN_N, "沉睡公牛","头")

S("Sleeping bulls wake up when you get into distance of two cells from them.",
  "沉睡公牛在你进入其两格内时醒来。")

N("Raging Bull", GEN_N, "愤怒公牛","头")

S("Raging Bulls charge in a straight line: on heptagons, when they can choose one of two possible directions, "
  "they choose one closer to your current location. In the case of a tie, the cell where more neighbors is "
  "closer to your current location is chosen; if still a tie, past locations are considered. "
  "They can attack you in any direction, and monsters on their way are attacked even if friendly. "
  "When they crash into something, the obstacle is usually destroyed, and they are stunned for three turns, after "
  "which they charge at you again (in any direction). "
  "Raging Bulls cannot be killed or stunned conventionally.",
  
  "愤怒公牛沿直线冲刺：在七边形上可以选择两个方向之一时，它们会选择距离你当前位置更近的那一边。如果相同，会选择有更多相邻方格与你最近的那一边；如果还是相同，就你考虑之前的位置。它们可以朝任意方向攻击，如果有敌人拦路也会受到攻击。在它们撞到什么东西时，那个拦路的障碍一般会被摧毁，同时公牛被击晕三个回合，随后它们再次朝你冲刺（朝任意方向）。愤怒公牛无法被常规手段击杀或击晕。"
  )

N("Herd Bull", GEN_N, "牛群","片")

S("Herds of these Bulls are running long distances for some reason. They become Raging Bulls if something stops them.",
  "成群的公牛不知为何在长距离奔跑。如果被阻拦，它们会变成愤怒公牛。"
  )

N("Gadfly", GEN_N, "牛虻","只")
S("Annoying insects. They can awaken Sleeping Bulls.", "烦人的昆虫。它们会唤醒沉睡公牛。")
S("%The1 wakes up %the2.", "%1唤醒了%2。")

S("%The1 crashes into %the2!", "%1冲撞进%2！")
S("Fire is extinguished!", "火焰被熄灭了！")
S("%The1 is filled!", "%1被填满！")

// Prairie
// -------

N("Prairie", GEN_N, "大草原","")
N("Green Grass", GEN_N, "绿草","株")
S("You can find safety in some places in the Prairie, but if you want treasures, "
  "they can be found only on the other side of a giant herd of bulls.",
  "大草原的许多地方都相当安全，但如果你想获得宝藏，那你只能在一大群公牛的另一边找到。"
  )

Orb("the Bull", "公牛")

S("You get the powers of Shield, Horns, and Thorns after you move two moves in a straight line "
  "with this Orb.",
  
  "持有这个法球时，在你沿直线前进两格后，你就会获得护盾、牛角与荆棘的力量。")

// Bull Dash
// ---------

N("Bull Dash", GEN_N, "奔牛","")
N("Butterfly", GEN_N, "蝴蝶","只")
N("Spinel", GEN_N, "尖晶石","枚")

S(
  "Butterflies don't pursue you -- unless you get next to them, they just spin around the obstacles. "
  "They cannot be killed conventionally, but you get treasure when a Raging Bull crashes into a Butterfly. ",

  "蝴蝶不会追逐你——它们只会绕着障碍物转圈，除非你靠近它们。它们无法被常规方式击杀，但你把愤怒公牛引到撞向蝴蝶时能得到宝藏。"
  )

Orb("Horns", "牛角")

S(
  "After you move while having this Orb, you immediately attack the next cell in the straight line "
  "(or two cells, when moving on a heptagon). This attack is slightly stronger than your normal "
  "attack: it can stun some of the monsters which cannot be killed or stunned normally.",
  
  "持有这个法球时，你在移动后会立即攻击沿直线的下一个格子（或两个，如果是在七边形上）。这一攻击比你的普通攻击稍强：它可以击晕一些平常无法杀死或击晕的怪物。"
  )

S("You pierce %the1.", "你刺穿了%1。")

// new 3D options
// --------------

S("Y shift", "Y轴平移")
S("Don't center on the player character.", "不将视角中心固定在玩家角色上。")

S("camera rotation", "相机旋转")
S("Rotate the camera. Can be used to obtain a first person perspective, "
  "or third person perspective when combined with Y shift.",
  "旋转相机。可用于进行第一人称视角游戏，结合Y轴平移可进行第三人称游戏。")

S("ball model", "球面模型")
S("hyperboloid model", "双曲体模型")

S("projection in ball model", "球面模型投影")
S("This parameter affects the ball model the same way as the projection parameter affects the disk model.",
  "这一参数影响球面模型的方式与投影参数影响碟模型的方式相同。")
  

S("camera rotation in ball model", "球面模型相机旋转")
S("Rotate the camera in ball/hyperboloid model.", "在球面/双曲体模型中旋转相机。")

// extra help
// ----------

S("\nSpecial conduct (still valid)\n", "\n挑战（仍生效）：\n")
S("\nSpecial conduct failed:\n", "\n挑战（已失败）：\n")
S("Avoid escaping from a discharge (\"That was close\").",
  "不碰到需要逃离放电的情况（“好险啊”）。")
S("Avoid chopping trees, using Orbs, and non-graveyard monsters in the Haunted Woods.",
  "不在闹鬼森林砍树、使用法球、吸引非墓地怪物。")
S("\n\nEasy %1 might disappear when you collect more of its kind.",
  "\n\n简单的%1可能在你收集许多时消失。")
S(" You need to go deep to collect lots of them.", "你需要深入才能收集到很多。")

S("\nSpawn rate (as prize Orb): %1%/%2\n", "\n刷新率（原生）：%1%/%2\n")
S("\nSpawn rate (in Hubs): %1%/%2\n", "\n刷新率（中心）：%1%/%2\n")

S("Orbs of Safety always appear here, and may be used to escape.\n",
  "安全法球总是在这里出现，可以用于逃脱。\n")
  
S("You may be unable to leave %the1 if you are not careful!\n",
  "如果你不当心，你可能会被永远困在%1！")

N("Cellular Automaton", GEN_N, "元胞自动机","组")

S("A land for people wanting to experiment with cellular automata in the HyperRogue grid. "
  "Rules can be given on the command line; the default rules are:\n"
  "-c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "(-c0 or -c1 can be given if the same rule is to be used for hexagonal "
  "and heptagonal cells).",
  "为想在HyperRogue的格子中实验元胞自动机的玩家准备的地域。规则以命令行输入；默认规则为：\n"
  "-c07 00100000 -c06 0010000 -c17 00011000 -c16 0001100 -caprob 0.3\n"
  "（如果六边形与七边形使用相同的规则，可输入-c0 or -c1）。")

S("compass size", "指南针尺寸")

S("Zebra quotient", "斑马商数")
S("field quotient", "域商数")

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

N("Tortoise", GEN_N, "陆龟","只")
S("line patterns", "线条图案")
S("1 turn", "1回合")
S("%1 turns", "%1回合")
S("inventory/kill mode", "物品栏/击杀模式")
S("images", "图像")
S("letters", "字母")
S("input", "输入")
S("default", "默认")
S("config", "设置")
S("expansion", "扩展")

S("continue", "继续")
S("main menu", "主菜单")
S("restart", "重新开始")
S("quit", "退出")
S("exit menu", "关闭菜单")

S(
  "A tourist from another world. They mutter something about the 'tutorial', "
  "and claim that they are here just to learn, and to leave without any treasures. "
  "Do not kill them!",
  
  "来自另一个世界的游客。他们嘟囔着什么“教程”，说自己来这里只不过是为了学习，不会带走宝藏。不要杀了他们！")

S(
  "This monster has come from another world, presumably to steal our treasures. "
  "Not as fast as an Eagle, not as resilient as the guards from the Palace, "
  "and not as huge as the Mutant Ivy from the Clearing; however, "
  "they are very dangerous because of their intelligence, "
  "and access to magical powers.\n\n",

  "这个怪物来自另一个世界，大概是来偷窃我们的宝藏的。没有雄鹰快、没有宫殿的守卫坚韧，也没有林间空地的变异常春藤巨大，但他们因智力超群而非常危险，还能使用魔法。\n\n"
  )

S(
  "Actually, their powers appear god-like...\n\n",
  "说实话，它的力量看起来如神明般强大……\n\n"
  )

S(
  "Rogues will never make moves which result in their immediate death. "
  "Even when cornered, they are able to instantly teleport back to their "
  "home world at any moment, taking the treasures forever... but "
  "at least they will not steal anything further!\n\n",
  
  "游荡者从不会做出能导致他们立即死亡的行动。即使被困，也能马上传送回他们的家园世界，永远带走宝藏……至少他们没法再偷东西了！\n\n")

S(
  "Despite this intelligence, Rogues appear extremely surprised "
  "by the most basic facts about geometry. They must come from "
  "some really strange world.\n\n",
  
  "虽然聪明，但游荡者似乎对最基本的几何知识都感到非常惊奇。他们肯定是来自什么特别奇怪的世界。\n\n")  

S("change the alpha parameter to show the lines",
  "改变alpha参数显示线条")

S("triangle grid: not rings", "三角形格：无环")
S("triangle grid: rings", "三角形格：有环")
S("heptagonal grid", "七边形格")
S("rhombic tesselation", "菱形密铺")
S("triheptagonal tesselation", "截半正七边形密铺")
S("normal tesselation", "正常密铺")
S("big triangular grid", "大三角形网格")
S("underlying tree", "下层树")
S("circle/horocycle tree", "圆/极限圆树")
S("zebra triangles", "斑马三角形")
S("zebra lines", "斑马线条")
S("vineyard pattern", "藤蔓园图案")
S("firewall lines", "防火墙线条")
S("firewall lines: Palace", "防火墙线条：宫殿")
S("firewall lines: Power", "防火墙线条：力量")

S("(ESC) tour menu", "（ESC）游览菜单")
S("Try the Guided Tour to help with understanding the "
  "geometry of HyperRogue (menu -> special modes).\n\n",
  "尝试指南游览模式来帮助理解HyperRogue中的几何学（菜单->特殊模式）。\n\n")

S("guided tour", "指南游览")
S("spherical geometry", "球体几何")
S("Euclidean geometry", "欧几里得几何")
S("more curved hyperbolic geometry", "更弯曲的双曲几何")
S("teleport away", "传送离开")
S("Shift-click a location to teleport there.", "按住Shift并点击某处来传送。")
S("Click a location to teleport there.", "点击某处进行传送。")

S("move through walls", "穿墙移动")
S("flash", "闪光")
S("static mode", "静态模式")
S("Static mode enabled.", "静态模式已启用。")
S("Static mode disabled.", "静态模式已禁用。")
S("enable/disable texts", "启用/禁用文本")
S("Help texts disabled.", "帮助文本已禁用。")
S("next slide", "下一页")
S("previous slide", "上一页")

S("This guided tour is different than most other game tutorials -- "
  "you are not forced to do anything, and you can go wherever you want.\n\n"
  "However, %the1 is not what we are talking about now. "
  "We will not explain this land at the moment, and you could potentially "
  "get lost there.\n\n"
  "Remember that you can get to the next slide by pressing Enter.",
  
  "这一指南与大多数游戏的教程不同——你并不必须要做任何事，可以想去哪里就去哪里。\n\n"
  "不过，%1不是我们正在讨论的内容。我们暂时不会解释这一地域，你有可能会在其中迷路。\n\n"
  "请记住你可以通过按回车键切换到下一页。")

S("Introduction", "介绍")
S("Welcome to the HyperRogue Guided Tour!", "欢迎来到HyperRogue指南游览！")
S(
  "This tour is mostly aimed to show what is "
  "special about the geometry used by HyperRogue. "
  "It also shows the basics of gameplay, and "
  "how is it affected by geometry.\n\n"
  "You decide when you want to stop playing with the "
  "current \"slide\" and go to the next one, by pressing Enter. You can also "
  "press ESC to see a "
  "menu with other options.",
  
  "这场旅行主要是想向你展示HyperRogue所使用几何的特别之处。它也包括基本的游戏内容，以及游戏内容是如何受到几何影响的。\n\n"
  "由你来决定何时结束游玩当前的“一页”，按回车切换到下一页。你也可以按ESC检视有其他选项的菜单。")

S("Basics of gameplay", "游戏基础")
S("gain Ice Diamonds", "获取冰钻")
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
  "游戏在冰霜之地开始。收集冰钻（如果你不知道如何移动，按F1）。在你收集许多个冰钻后，怪物会多到开始变得具有挑战。\n与许多roguelike或基于战术而非叙事的游戏一样，如果你死掉，你需要从头开始新游戏。不过，在这个教程中，你碰到恶劣的情况按“4”就可以传送离开了。大体而言，这一教程为了给你展现内容动了许多手脚——比如说，如果你现在按“5”，就会获得一大堆冰钻。")

S("Hypersian Rug model", "超波斯地毯模型")
S(
  "New players think that the action of HyperRogue takes place on a sphere. "
  "This is not true -- the next slide will show the surface HyperRogue "
  "actually takes place on.\n\n"
  "Use arrow keys to rotate the model, and Page Up/Down to zoom.\n\n"
  "If you do not see anything, press '5' to try a safer renderer.",
  

  "新玩家可能会觉得HyperRogue是在一颗球上玩的。并非如此——下一页就会向你展示HyperRogue所在平面的真实样貌。\n\n"
  "使用方向键旋转模型，Page Up/Down缩放。\n\n如果你什么都看不到，按“5”切换到安全一点的渲染器。"
  )

S("Expansion", "展开")
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
  
  "下一页显示了距离你1、2、3……格远处的格子数目。格子数呈指数级增长：在距离你1000格处有10^100个格子，而你在游戏中会走得远超1000格！\n\n这对HyperRogue的设计影响巨大。HyperRogue中有许多寻路谜题——在欧几里得世界中轻而易举之事放在双曲几何中就变得非常艰难（要去20格外的某处，那么你要选择几千条路线中的哪一种？）；不过，在欧几里得世界中毫无可能之事在HyperRogue中也许会变得简单。HyperRogue专门设计使你无法在无限的世界中刷东西。基本没有永久升级；收集宝藏有好处，但捡同一种宝藏太多又非常危险。")

S("Tiling and Tactics", "格子与战术")
S(
  "The tactics of fighting simple monsters, such as the Yetis from the Icy Lands, "
  "might appear shallow, but hyperbolic geometry is essential even there. "
  "In the next slide, you are attacked by two monsters at once. "
  "You can make them line up simply by "
  "running away in a straight line. "
  "Press '2' to try the same in the Euclidean world -- it is impossible.",
  "与简单敌人的战斗，比如冰霜之地的雪人，可能看起来简单，但双曲几何即使在这里也十分关键。在下一页中，你会同时受两名敌人攻击。你只需要沿直线逃跑就能让它们排成一排。按“2”在欧几里得世界中尝试相同的挑战——不可能做到。")

S("Straight Lines", "直线")
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
  
  "双曲几何是在19世纪中由思考平行本质的数学家发现的。给定直线L与点A。存在有超过一条穿过A点而不与L相交的直线的世界吗？\n\n"
  "如果你拥有许多冰钻，冰霜之地会变得非常危险——许多雪人与冰狼会追猎你！但你没有获取过宝藏的其他地域还是（相对）安全的。\n\n"
  "再四处走走，你应该很快会碰到交叉路口——长城是直线，它们的确有与欧几里得几何中不同的特性。在长城的另一侧，你会看到其他地域——HyperRogue中有大约60种地域，基于不同的机制与双曲几何的各种特点。")

S("Running Dogs", "奔行犬")
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
  "想要学到更多直线相关的知识，就再四处走走，你会找到永动之地。试着在奔行犬在你旁边时沿直线前进。即使奔行犬的速度与你相同，它看起来还是会走得更慢——这是因为你在沿直线行走，而奔行犬在所谓的等距曲线上奔跑。\n\n"
IF_MAC(
 "请记住你可以通过按住右Shift并点击来查看物体的更多信息。"
,
 "请记住你可以通过右键点击来查看物体的更多信息。"
)
  )

S("Equidistants", "等距曲线")
S(
  "Equidistants are curves which are at some fixed distance from a "
  "straight line. Some lands in HyperRogue are based on equidistants; "
  "you should see them after wandering a bit more.\n\n"
  "This tutorial gives you freedom to go wherever you choose, "
  "but we do not recommend going deep into the Dungeon or the Ocean -- "
  "getting back might be difficult.",
  
  "等距曲线是距离某一条直线距离固定的曲线。HyperRogue中的一些地域正是基于等距曲线的；在你探索一段时间后应该会碰到。\n\n"
  "这一教程给了你想去哪里就去哪里的自由，但我们不推荐你太过深入地牢或海洋——也许很难回来。")

S("Circles", "圆圈")
S(
  "Circles are strange in hyperbolic geometry too. "
  "Look for the Castle of Camelot in the Crossroads; "
  "the Round Table inside is a circle of radius 28. "
  "Finding its center is a difficult challenge.\n\n"
  "Press '5' to cheat by seeing the smaller circles too.\n\n"
  "Note: Camelot and some other lands are unlocked earlier in the Tutorial than in a real game.",

  "圆圈在双曲几何中也很奇怪。在交叉路口中寻找卡美洛城堡，其中的圆桌是半径为28的圆。找到它的中心是一项很难的挑战。\n\n"
  "按“5”来作弊，使你能够看到半径更小的同心圆。\n\n"
  "注意：卡美洛与其他地域在教程中提前解锁。正常游戏不会这么快。")

S("Horocycles", "极限圆")
S("Horocycles are similar to circles, but you cannot reach their center at all -- "
  "they can be understood as limit circles of infinite radius centered in some point "
  "in infinity (also called an ideal point).\n\n"
  "Go to R'Lyeh, and you should quickly find a Temple of Cthulhu there. "
  "Each circle of columns is actually a horocycle. Horocycles in a given "
  "temple are concentric, and there is an infinite number of them.",

  "极限圆与圆类似，但你无法抵达它们的中心——它们可以被视为圆心位于某无穷远处的点（称为理想点）、半径无穷大的圆。\n\n"
  "去拉莱耶，你很快会找到其中的克苏鲁神殿。每一道立柱组成的圆圈都是极限圆。每座神殿中的极限圆都是同心的，有无穷多这样的圆。")

S("Half-plane model", "半平面模型")
S("The game is normally displayed in the so called Poincaré disk model, "
  "which is a kind of a map of the infinite hyperbolic world. "
  "There are many projections of Earth, but since Earth is curved, "
  "all of them have to distort distances or angles in some way -- "
  "the same is true in hyperbolic geometry. "
  "The next slide shows another model, called the Poincaré upper half-plane model. In this model, "
  "horocycles centered at one specific ideal point are drawn as straight lines.",
  
  "这个游戏一般是以所谓的庞加莱碟模型显示的，是无穷的双曲世界的一种映射。地球有各种各样的投影，但由于地球是圆的，所有的投影都会在距离或角度上有所扭曲——双曲几何也是如此。下一页展示另一个模型，称为庞加莱上半平面模型。在这个模型中，圆心为某一特定点的所有极限圆都绘制为直线。")

S("Curvature", "曲率")
S("gain Orb of the Sword", "获取宝剑法球")
S(
  "Now, go to the Burial Grounds and find an Orb of the Sword. The Sword appears to "
  "always be facing in the same direction whatever you do, and it appears that "
  "you have to rotate the sword to excavate the treasures; "
  "yet, it is possible to excavate them! You might have already noticed "
  "that the world rotates after you move around a loop and return to an old "
  "place.\n\n"
  "This is related to the fact that the world of HyperRogue is curved, and "
  "the sum of angles in a triangle is not equal to 180 degrees.",

  "现在，去埋骨地寻找一个宝剑法球。宝剑看起来不管你做什么都会面对同一个方向，而且似乎你需要旋转宝剑才能挖掘出宝藏；尽管如此，挖掘并非不可能做到！你也许已经注意到你绕一圈回到原点后世界发生了旋转。\n\n"
  "这是由于HyperRogue的世界是弯曲的，三角形的内角和并非180度。")

S("Periodic patterns", "周期图案")
S("Hyperbolic geometry yields much more interesting periodic patterns "
  "than Euclidean.",
  "双曲几何有比欧几里得几何有趣得多的周期图案。")

S("Periodic patterns: application", "周期图案：应用")
S("Many lands in HyperRogue are based around periodic patterns. "
  "For example, both Zebra and Windy Plains are based on the pattern "
  "shown in the previous slide. "
  "Such lands often have tree-like nature.",
  "HyperRogue中的许多地域都基于周期图案。举例来说，斑马和风袭平原都基于上一页中给出的图案。这些地域大多有类似树的特征。")

S("Fractal landscapes", "分形地貌")
S("On the following slide, the colors change smoothly in the whole infinite world. "
  "Again, this works better than in Euclidean geometry.",
  "在下一页中，颜色平滑地在整个无尽的世界中渐变。这比欧几里得几何中要顺畅。")

S("Fractal landscapes: application", "分形地貌：应用")
S("create a baby tortoise", "创造一只小陆龟")
S(
  "This is applied in HyperRogue to create landscapes, such as the chasms in the "
  "land of Reptiles or the Dragon Chasms, which you should find quickly. "
  "Also in the Dragon Chasms, you can find a Baby Tortoise, and try to find "
  "a matching adult tortoise in the Galápagos. "
  "There are over two millions of species, but since there is so much space in "
  "hyperbolic geometry, finding a matching tortoise is possible. The brighter "
  "the color in Galápagos is, the more aspects of the tortoises in the given "
  "area are matching.",
  
  "这一点被HyperRogue用于构建地貌，比如爬行中的裂缝或巨龙裂隙，你很快就会找到它们。此外，在巨龙裂隙中，你会找到一只小陆龟，要去加拉帕戈斯找到与它匹配的大陆龟。陆龟有超过两百万个品种，但双曲几何中有如此之多的空间，找到匹配的陆龟也并非难如登天。加拉帕戈斯中地面的颜色越亮，表明对应区域中陆龟所匹配的因素就越多。")

S("Poincaré Ball model", "庞加莱球面模型")
S(
    "The Poincaré disk model is a model of a hyperbolic *plane* -- you "
    "might wonder why are the walls rendered in 3D then.\n\n"
    "HyperRogue actually assumes that the floor level is an equidistant surface "
    "in a three-dimensional hyperbolic world, and the camera is placed above the "
    "plane that the surface is equidistant to (which boils down to showing "
    "the floor level in Poincaré disk model).\n\n"
    "This is shown on the next slide, in the Poincaré ball model, which is "
    "the 3D analog of the Poincaré disk model.",
    
  "庞加莱碟模型是双曲“平面”的一个模型——你也许会想那为什么墙壁都渲染成了立体的。\n\n实际上HyperRogue假设地面高度是在三位双曲世界中的一个等距面，而相机位于地表所等距的那个平面之上（结果就是地面高度以庞加莱碟模型显示）。\n\n"
  "这点在下一页的庞加莱球面模型中有所展示，它是庞加莱碟模型的三维类比。")

S("Hyperboloid model", "双曲体模型")
S(
    "Let's see more models of the hyperbolic plane. "
    "This model uses a hyperboloid in the Minkowski geometry; "
    "it is used internally by HyperRogue.",
    
    "让我们来看看更多双曲面的模型。这一模型使用闵可夫斯基几何中的双曲体；这是HyperRogue内部使用的模型。")

S("Beltrami-Klein model", "克莱因-贝尔特拉米模型")
S(
    "This model renders straight lines as straight, but it distorts angles.",
    "这一模型将直线渲染为直线，但角度会被扭曲。")

S("Gans model", " Gans模型")
S(
  "Yet another model, which corresponds to orthographic projection of the "
  "sphere. Poincaré disk model, Beltrami-Klein model, and the Gans "
  "model are all obtained by looking at either the hyperboloid model or an "
  "equidistant surface from various distances.",
  "又一个模型，可以类比为球面的正交投影。庞加莱碟模型、克莱因-贝尔特拉米模型与Gans模型都是以在各种距离观察双曲体或一个等距面得到的。")

S("Band model", "带模型")
S("render spiral", "渲染螺旋线")
S(
  "The band model is the hyperbolic analog of the Mercator projection of the sphere: "
  "a given straight line is rendered as a straight line, and the rest of the "
  "world is mapped conformally, that is, angles are not distorted. "
  "Here, we take the straight line connecting your starting point and your "
  "current position -- usually the path taken by the player is surprisingly "
  "close to a straight line. Press '8' to see this path.\n\n"
  "If you want, press '5' to see it rendered as a spiral, although it takes lots of time and "
  "memory.",
  "带模型是球面的墨卡托投影的双曲类比：某一条特定直线被渲染为直线，其余的世界作保角映射，也就是说角度不变。在此，我们选取你的起始位置与目前所在的位置相连的直线——一般而言玩家采取的路线都接近直线得令人吃惊。按“8”来查看这一路线。\n\n"
  "你若愿意，可按“5”来将它渲染为一个螺旋线，不过这会消耗多得多的时间与内存。")

S("Conformal square model", "保角方模型")
S("The world can be mapped conformally to a square too.",
  "这个世界也可以被保角映射到一个方块上。")

S("Shoot'em up mode", "射击模式")
S("In the shoot'em up mode, space and time is continuous. "
  "You attack by throwing knives. "
  "Very fun with two players!\n\n"
  "There are other special modes too which change the gameplay or "
  "focus on a particular challenge.",
  "在射击模式中，时间与空间是连续的。你使用飞刀攻击。两位玩家时特别好玩！\n\n"
  "还有其他特殊模式，有的修改游戏内容，有的专注于某一种挑战。")

S("THE END", "尾声")
S("leave the tour mode", "离开游览模式")
S(
  "This tour shows just a small part of what you can see in the world of HyperRogue. "
  "For example, "
  "hyperbolic mazes are much nicer than their Euclidean counterparts. "
  "Have fun exploring!\n\n"
  "Press '5' to leave the tour mode.",

  "这次游览不过是你会在HyperRogue世界中见到的冰山一角。比如说，双曲迷宫比欧几里得的要好看得多。祝你探索愉快！\n\n按“5”离开游览模式。"
  )

/*
// for 10.0
*/

// Orb Strategy mode
// -----------------

S("Orb Strategy mode", "法球策略模式")

S(
  "You are playing in the Orb Strategy Mode. Collecting treasure "
  "gives you access to magical Orb powers. In this mode, "
  "unlocking requirements are generally higher, and "
  "several quests and lands "
  "give you extremely powerful Orbs of the Mirror.\n",

  "你正在游玩法球策略模式。收集宝藏会赋予你法球的力量。在这一模式中，解锁的要求普遍更高，许多任务与地域会给你无比强大的镜像法球。\n")

S("The treasure gives your magical powers!", "宝藏赋予你魔法力量！")
S("Press 'i' to access your magical powers.", "按“i”检视你的魔法力量。")
S("inventory", "物品栏")
S("mirror what?", "复制什么？")
S("Which orb to use?", "使用哪个法球？")
S("Unlocked by: %1 in %2", "解锁条件：%2中的%1")
S(" (next at %1)", " （下一个需要%1）")
S(" (next at %1 to %2)", " （下一个需要%1到%2）")
S("Number of uses left: %1", "剩余使用次数：%1")
S("You mirror %the1.", "你复制出%1的镜像。")
S("You need to stand next to a magic mirror or cloud to use %the1.", 
  "你需要站在魔镜或云旁边使用%1。")
S("Each orb type can be mirrored only once.", "每种类型的法球只能被复制一次。")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Yendor appear in Hell after "
  "you collect 25 Demon Daisies in Hell, in Crossroads/Ocean after you collect 50, "
  "and everywhere after you collect 100.",

  "\n\n在法球策略模式中，Yendor法球在你收集25个恶魔雏菊后在地狱中开始出现，交叉路口/海洋需要50个，其余地域需要100个。"
  )  

S(
  "\n\nIn the Orb Strategy Mode, dead orbs are available once you collect "
  "10 Necromancer Totems in the Graveyard.",
  
  "\n\n在法球策略模式中，你在墓地收集10个死灵图腾后解锁失效法球。")

S(
  "\n\nIn the Orb Strategy Mode, Orbs of Safety can be gained by "
  "collecting Phoenix Feathers in the Land of Eternal Motion. "
  "You can also find unlimited Orbs of Safety in the Crossroads "
  "and the Ocean (after collecting 25 Phoenix Feathers) "
  "and in the Prairie.",
  
  "\n\n在法球策略模式中，安全法球可以通过在永动之地收集凤凰羽毛得到。你也可以在交叉路口、海洋和大草原中找到（收集25个凤凰羽毛后）。"
  )

S(
  "\n\nCollect %the1 to gain an extra Orb of the Mirror. "
  "You can gain further Orbs of the Mirror by collecting 2, 4, 8...",

  "\n\n收集%1来获得额外的镜像法球。你在收集2、4、8……个时获得更多的镜像法球。"
  )

S(
  "\n\nIn the Orb Strategy Mode, the Orb of Luck also "
  "significantly increases the frequency of Great Walls, Crossroads IV, "
  "and sub-lands.",
  
  "\n\n在法球策略模式中，幸运法球同时也会大幅提升长城、交叉路口 IV与子地域的出现频率。")

S("\n\nIn the Orb Strategy Mode, each 25 Necromancer's Totems "
  "you are given a random offensive Orb.",
  
  "\n\n在法球策略模式中，每收集25个死灵图腾会给予你一个随机进攻法球。")
 
S(
  "Use Orb of the Mirror to gain copies of one of your Orbs; "
  "mirroring weaker Orbs usually yields more copies. "
  "It can only be used once per Orb type, "
  "and only when you are next to a mirror.",
  
  "使用镜像法球来获取你法球的复制；复制比较弱的法球一般得到的数量会比较多。每种类型的法球只能复制一次，而且只能在你在镜子旁边时启动复制。")
  
S("Uses to gain: %1", "能够获得的次数：%1")
S("already mirrored", "复制过了")
  
N("your orbs", GEN_N, "你的法球","")
S("Click this to see your orbs.", "点击此处查看你的法球。")                                                   

// peaceful mode
// -------------

S("configure keys/joysticks", "设置键位/摇杆")
S("peaceful mode", "和平模式")

// config changes
S("Press F5 or 'o' to try again!", "按F5或“O”重试！")  
S("aura brightness", "光环亮度")
S("aura smoothening factor", "光环平滑系数")
S("graphics configuration", "图像设置")
S("special display modes", "特殊显示模式")
S("openGL mode", "OpenGL模式")
S("anti-aliasing", "抗锯齿")
S("line width", "线宽")
S("configure panning and general keys", "设置镜头移动与通用按键")

S("\n\nHint: use 'm' to toggle cells quickly",
  "\n\n提示：按“m”快速调整格子")

// cell pattern names
S("football", "足球")
S("dark rainbow landscape", "暗彩虹地貌")
S("field pattern", "域图案")
S("field pattern C", "域图案C")
S("field pattern D", "域图案D")
S("field pattern N", "域图案N")
S("field pattern S", "域图案S")
S("four triangles", "四三角形")
S("big triangles: rings", "大三角形：环")

// missing for the Tutorial
S("This Orb is not compatible with the Tutorial.", "该法球与教程不兼容。")

// local scores
S("turns", "回合")
S("cells", "格子")
S("sort", "排序")
S("choose", "选择")
S("play", "开始")

// draw editor
S("autochoose", "自动选择")
S("c = choose", "c = 选择")
S("b = switch auto", "b = 切换自动")

// mission screen hints

S(
  "If you collect too many treasures in a given land, it will become "
  "extremely dangerous. Try other lands once you have enough!",
  "如果你在同一地域收集太多的宝藏，它会变得极其危险。在你拿够以后就去其他地方吧！")

S(
  "Remember that you can right click almost anything for more information.",
  "请记住你可以右键点击几乎任何物体来查看它的信息。")

S("Want to understand the geometry in HyperRogue? Try the Guided Tour!",
  "想要理解HyperRogue中的几何？试试指南游览吧！")

S(
  "Collecting 25 treasures in a given land may be dangerous, "
  "but allows magical Orbs of this land to appear in other places!",
  "在任意地域收集25个宝藏可能很危险，但会让那个地域的法球在其他地方出现！")

S(
  "Press ESC to view this screen during the game.",
  "在游戏中按ESC来检视该屏幕。")

S("The 'world overview' shows all the lands in HyperRogue.",
  "“世界概览”显示HyperRogue中的所有地域。"
  )

S("Press 'o' to see all the lands in HyperRogue.",
  "按“o”检视HyperRogue中的所有地域。")

S(
  "Want another type of game? Want more challenge?\n"
  "HyperRogue has many special modes and challenges that "
  "significantly change the gameplay. Try them!",
  
  "想玩其他类型的游戏？想要挑战自我？\n"
  "HyperRogue有许多对游戏改动很大的特殊模式与挑战。试试吧！")

S(
  "Hyperbolic geometry can be shown in many ways.",
  "有许多种表现双曲几何的方式。")

S(
  "You do not want to lose the game from a single mistake?\n"
  "Do you want to use the Orbs strategically?\n"
  "Try the Orb Strategy mode!",
  
  "你不想仅仅因为一次失误便输掉游戏？你想要有计划地使用法球吗？试试法球策略模式吧！")

S(
  "Do you think you are playing on a ball? "
  "This is far from the truth!\n",
  
  "你觉得这个游戏是发生在一枚球体上吗？大错特错！\n")

S(
  "Did you know that the path you take during the game "
  "is usually very close to a straight line?\n",

  "你知道你在游戏中行走的路线一般都非常接近于直线吗？\n")

S("Show me!", "让我看看！")

S(
  "You are %1 cells away from the starting point, or "
  "the place where you used an Orb of Safety last time. "
  "There are %2 such cells.\n",
  
  "你距离起点或你上次使用安全法球的位置有%1格。距离这么远的格子有%2个。\n")

S("about ", "关于 ")
S(" (%1 more digits)", "（还有%1位数字）")

S("see how it ended", "观赏你的末日")

// other missing/new things
S("Orb unlocked: %1", "解锁的法球：%1")
S("Secondary orb: %1", "次要法球：%1")
S(" to submerge", "后被淹没")
S(" to surface", "后露出水面")
S("%The1 says, \"not this place, it looks even worse...\"", 
  "%1说：“这里不行，这里看起来更糟了……”")
S("torus", "环面")
S(" (click to use)", "（点击使用）")
N("Hall of Mirrors", GEN_N, "镜像大厅","")
Orb("the Mirror", "镜像")
N("Reflection", GEN_N, "映像", "个")
N("mirror wall", GEN_N, "镜墙", "堵")

S("This would only move you deeper into the trap!",
  "这只会让你陷得更深！")

S("You swing your sword at the mirror.", "你对镜子挥剑。")
N("Mimic", GEN_N, "模仿者","个")
N("Narcissist", GEN_N, "自恋狂","个")
N("Mirror Spirit", GEN_N, "镜像精魂","个")

S("This person loves to look at their own reflection in the mirror. "
  "He believes himself to be one of the most important creatures in this world, "
  "and hates those who do not admire him.",

  "这人喜欢端详自己在镜中的映像。他相信自己是这世界上最重要的生物之一，憎恨并不仰慕他的人。"
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

S("memory game", "记忆游戏")
S("display hints", "显示提示")
S("hyperbolic puzzles", "双曲谜题")

// missing descriptions

S(  "A strange land filled with mirrors. "
    "Break magic mirrors and enter clouds of mirage to "
    "gain treasures and helpful Mimics.",
    
    "充满镜子的奇异地域。打破魔镜或进入幻象云来获取宝藏与帮助你的模仿者。")

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
  
  "一面完美的镜墙。既无法被破坏，也不能穿过，即使是以太生物也不行，你在其中看到的东西只不过是真实世界的映像；你可以对它们挥剑，但这也无法将它们摧毁。镜墙会完美地反射模仿者、闪电，以及远程武器。"
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

"在和平模式中，你仅探索世界而无需战斗；你还会遇到许多寻路谜题。在记忆游戏中，你需要尽可能多地收集十二面体，然后返回原点——双曲几何令此无比困难！其他双曲谜题包括埋骨地（使用魔法剑挖掘宝藏）、加拉帕戈斯（寻找与宝宝匹配的大陆龟）、卡美洛（寻找一个大双曲圆的圆心），以及宫殿（跟随老鼠）。其他列出的地域仅供探索。")

S("puzzles and exploration", "谜题与探索")

S("Collect as many Dodecahedra as you can, then return here!",
  "尽可能多地收集十二面体，然后返回这里！")

S("reset all configuration", "重置所有设置")
S("Are you sure?", "你确认吗？")
S("yes, and delete the config file", "是，并删除设置文件")
S("yes", "是")
S("cancel", "取消")
S("reset the special game modes", "重置特殊游戏模式")

// extra flavor messages for the OSM
// ---------------------------------

S("You feel the presence of free saves on the Crossroads.",
  "你感到交叉路口中出现了免费的存档。")

S("You feel the Orbs of Yendor nearby...",
  "你感到Yendor法球就在附近……")
  
S("You feel the Orbs of Yendor in the Crossroads...",
  "你感到Yendor法球出现在交叉路口……")

S("You feel the Orbs of Yendor everywhere...",
  "你感到Yendor法球遍布各处……")

S("You have gained an offensive power!",
  "你获得了一项进攻能力！")

S("A small reward for braving the Hell.",
  "抗击地狱的小小奖励。")

S(" (mirrored)", "（复制）")
S(" (used %1 times)", "（使用过%1次）")
S("Extras:", "额外：") // extra Orbs gained in OSM

// cheats
// ------

S("unlock Orbs of Yendor", "解锁Yendor法球")
S("Collected the keys!", "收集了钥匙")
S("Saved the Princess!", "救下了公主！")
S("save a Princess", "救下公主")

// other
// -----
  
S("Note for mobiles", "移动端提示")
S(
    "This tour is designed for computers, "
    "and keys are given for all actions. It will "
    "work without a keyboard though, although less "
    "comfortably -- just ignore the keys "
    "given and select options from MENU.\n\n"
    "Select 'next slide' from MENU.",

    "这一游览是为电脑设计的，附带了每个行动的按键。不过，它没有键盘时也能生效，只是麻烦点——忽略给出的按键，在菜单中选择选项。\n\n"
    "“在菜单中选择“下一页””\n\n")

S("quick mouse", "快速鼠标")
S("This combination is known to be buggy at the moment.", "这一组合目前存在故障。")

// extra Princess texts
// --------------------
 
S("\"I do not like butterflies. They are treacherous.\"", 
  "“我不喜欢蝴蝶。它们背信弃义。”")

S("\"I hate roses.\"", "“我恨死玫瑰了。”")

S("\"In this world there is plenty of space for everyone. We do not need wars.\"", 
  "“这个世界的空间足够所有人生存。我们不需要战争。”")

S("\"Only the stupid hyperbugs do not understand this.\"", 
  "“只有愚蠢的超虫不懂。”")

S("\"I have once talked to a Yendorian researcher... he was only interested in infinite trees.\"",
  "“我和Yendor研究员聊过……他只对无穷树感兴趣。”")

S("\"Infinite trees are boring. I prefer other graphs.\"", 
  "“无穷树很无聊。我喜欢其他图。”")

// new start menu
// --------------

S("skip the start menu", "跳过开始菜单")

S("HyperRogue classic", "HyperRogue经典版")
S("explore the world, collect treasures", "探索世界，收集宝藏")
S("do not get checkmated", "不要被困死")
S("use your Orbs in tough situations", "在走投无路时使用法球")
S("continuous spacetime", "连续时空")
S("(most achievements are not available)", "（大多成就不可用）")
S("learn about hyperbolic geometry!", "学习双曲几何相关知识！")
S("more options", "更多设置")

S(
  "A strange land filled with mirrors. Break magic mirrors and mirage clouds to "
  "gain treasures and helpful Mimics.",
  "充满镜子的奇异地域。打破魔镜或进入幻象云来获取宝藏与帮助你的模仿者。"
  )
  
// were missing

N("Kraken", GEN_N, "海妖","只")
N("Kraken Tentacle", GEN_N, "海妖触手","条")
S(" (killing increases treasure spawn)", "（击杀增加宝藏刷新率）")

N("stepping stones", GEN_N, "垫脚石","块")

// 10.1

// mew message options
S("centered", "居中")
S("left-aligned", "左对齐")
S("line-broken", "换行")
S("message style", "信息样式")
S("limit messages shown", "限制信息显示")
S("Maximum number of messages on screen.", "屏幕显示信息的最大数量。")

S("message log", "信息记录")
S("real time", "实际时间")
S("game time", "游戏时间")
S("precise", "精确")
S("no time", "无时间")
S("back", "返回")
S("clear", "清除")

// 'u' now loads the current picture in the Map Editor
S("'u' to load current", "按“u”读取当前")

// Happy Hunting Ground
//----------------------

N("Hunting Ground", GEN_N, "猎场","")

S("The Happy Hunting Ground is the place where hunting is easy; "
  "the spirits of hunters go here after their death, if their life was deemed worthy. "
  "You did not qualify though, so you will not ever be able to find your way to the places "
  "where the hunter spirits and game are... "
  "and their hunting dogs will hunt you instead!\n\n"
  "You hope to use this to learn some "
  "skills in fighting in open hyperbolic space, though.",
  
  "在欢乐猎场中狩猎轻而易举；荣耀的猎人在死后灵魂会来到这里。不过你可算不上荣耀，因此你永远也找不到猎人的灵魂和猎物……他们的猎犬还会追你！\n\n"
  "不过，你还是希望能从这一过程中学到一些在开放的双曲空间中战斗的技巧。"
  )
  
N("Hunting Dog", GEN_N, "狩猎犬","条")
N("Hunting Dog (guarding)", GEN_N, "狩猎犬（看守中）","只")
N("Turquoise", GEN_N, "绿松石","枚")
Orb("Ferocity", "凶残")

S("Hunters believe that wearing a Turquoise amulet will improve their accuracy. "
  "This one has been lost, but the hunting dogs are guarding it until the owner returns.",
  
  "猎人相信佩戴绿松石护符能提高自己的精准度。这一枚不慎丢失了，但狩猎犬会看守它到主人归来。"
  )

S("Whenever you attack with this Orb, you also hit the monsters adjacent to you and opposite to the monster you originally attacked.",
  "每当你以此法球攻击时，你还会同时击中被攻击怪物反方向的怪物。"
  )

// Volcanic Wasteland
//--------------------

N("Volcanic Wasteland", GEN_N, "火山荒原","")
N("lava", GEN_N, "熔岩","滩")

S("The volcanic activity in this land is extremely unstable. "
  "Lava is too hot to walk on. It cools quickly, but another "
  "flow will come soon...",
  
  "这一地域的火山活动极其不稳定。岩浆太烫，没法在其上行走。它会很快凉掉，但下一次喷涌又快来了……"
  )

N("Salamander", GEN_N, "火蜥蜴","只")

S("Salamanders are tough lizard-like creatures. Their tough skin protects them "
  "from both physical attacks and heat. Salamanders "
  "are stunned for a longer time if you push them into lava, fire, or a solid obstacle.",
  
  "火蜥蜴是坚韧的蜥蜴类生物。它们的厚皮能够防护物理攻击以及热量。如果你将火蜥蜴推入岩浆、火焰或坚硬的障碍物，它们会眩晕更久。"
  )

N("Lava Wolf", GEN_N, "岩浆狼","条")

S("While Ice Wolves love heat, their instincts usually will not let them leave the Icy Lands -- "
  "they are afraid that they will be unable to get back home, and that they will lose track of their prey. "
  "However, they find the Volcanic Wasteland so hot and attractive that they abandon their natural instincts... "
  "and try to track their prey using their other senses and intelligence.",
  
  "尽管冰狼喜欢热量，但它们的本能通常不会让它们离开冰霜之地——它们担心无法回到家乡，并且会失去猎物的踪迹。然而，它们发现火山荒原如此炎热而又迷人，以至于它们抛弃了自然的本能……试图用其他感官和智慧去追踪猎物。")
  
N("Lava Lily", GEN_N, "岩浆百合","朵")

S("This plant, able to survive in the extreme conditions of the Volcanic Wasteland, "
  "is a valuable alchemical ingredient.",
  
  "这种植物能在火山荒原的极端条件下生存，是珍贵的炼金原料。")

Orb("Lava", "岩浆")
S("This Orb summons a minor volcanic activity around you. "
  "All the heptagonal cells adjacent to enemies in distance at most 5 to you "
  "will be set on fire. Does not affect the cell you are on, and enemies resistant to fire.",
  
  "这个法球在你周围召唤一次小小的的火山活动。在至多5格距离内与敌人相邻的七边形格子都会被点燃。不会影响你所在的格子与耐受火焰的敌人。"
  )

// Orbs would be destroyed in Volcanic Wasteland
S("would be destroyed in %the1", "会在%1中被摧毁") 
S(" to go cold", "来降温")
S("%The1 is destroyed by lava!", "%1被岩浆摧毁了！")
S("%The1 is killed by lava!", "%1被岩浆杀死了！")
S("Run away from the lava!", "快从岩浆上跑开！")

// Terracotta Army
//-----------------

N("Terracotta Army", GEN_N, "兵马俑","")

S("The Emperor's mausoleum. The army "
  "of Terracotta Warriors is supposed to protect him in his "
  "afterlife, while the traps are to protect the treasures "
  "in the mausoleum from thieves.",

  "皇帝的陵寝。兵马俑军队用于保护他的来世，陷阱用于杀死觊觎陵寝中财宝的盗贼。"
  )

S("Spending 3-5 turns next to the Terracotta Warriors will cause them to awaken. "
  "They have 7 HP, and get stunned for longer and longer time with each attack.",
  
  "在兵马俑旁呆3-5回合会使它们苏醒。它们有7点生命，每次被击中都会让它们眩晕更久。"
  )

N("arrow trap", GEN_N, "弓箭陷阱","处")

S("The arrow traps will automatically launch a deadly arrow when stepped on. They are quite old though, so the arrow will "
  "actually fly at the end of the next turn.",
  
  "这个弓箭陷阱会在被踩中时自动发射致命的箭矢。不过它老化太严重了，箭矢其实在下回合结束时才会射出来。"
  )

N("mercury river", GEN_N, "汞河","条")
S("A river of mercury.", "一条水银河流。")

N("Terracotta Warrior", GEN_N, "兵马俑战士","尊")

N("Jiangshi", GEN_N, "僵尸","个")
S("You think this was one of the people who have constructed the Terracotta Warriors and the arrow traps. "
  "They have been locked inside, so that they will never divulge the secrets of the mausoleum. They would like to return their homes and families, though.",
  
  "你觉得这是建造兵马俑战士与弓箭陷阱的那些工匠之一。他们被锁在陵寝里，这样才能永不泄露其中的秘密。不过，他们还是想要回家。" 
  )

Orb("Slashing", "割斩")
S("Whenever you attack with this Orb, you also hit the monsters adjacent both to you and the monster you originally attacked.",
  "在你持有这个法球攻击时，你同时会攻击到与你及你的目标相邻的怪物。")

N("Ancient Weapon", GEN_N, "远古兵器","柄")
S("This ancient weapon is beautifully encrusted with precious gems, but you prefer your own -- it is much lighter.",
  "这把远古的武器嵌有美丽的宝石，但你还是更愿意用自己的——你的武器要轻得多。")

// Blizzard
//----------

N("Blizzard", GEN_N, "暴风雪","")
N("Ice Golem", GEN_N, "冰魔像","座")
N("Void Beast", GEN_N, "虚空兽","头")
N("Forgotten Relic", GEN_N, "遗忘之物","件")

S("The Ice Golems are powered by intense magical coldness. When destroyed in the Blizzard or another icy land, they become "
  "ice walls, and freeze the land around them.",
  
  "冰魔像由强烈的魔法寒冷驱动。它在暴风雪或其他寒冷地域被摧毁时会变成冰墙，并且冻结周围的地板。")

S("Are creatures of Void actual monsters, or just monster-shaped holes in the air?\n\nVoid Beasts move simply by letting the air move into their current location -- "
  "the hole will move to the place where the air came from! Void Beasts are great at moving against the wind, but they have problems "
  "moving with the wind.",
  
  "虚空生物到底是真的怪物，还是空气里怪物形状的洞？\n\n"
  "虚空兽只需要让空气流动到它们目前的位置就能移动——那个洞则会跑到空气过来的地方！虚空兽擅长逆风而行，但它们难以顺风移动。"
  )

S("Once in the past there lived a tribe whose prophets warned about the great blizzard that was to come and freeze the world. "
  "They thought it was the wrath of the gods, so they put some altars, so that the gods could hear their pleas. "
  "The mighty golems guarded those altars, making sure nobody steals the jewels, because a robbed god is a angry god. "
  "Unfortunately winter has come (maybe because only the monks knew how to stop the golems, and they were said to had escaped earlier with some of the jewels). "
  "The tribe has frozen to death, altars got covered with snow and ice. Only ice golems still guard them waiting for the spring.",
  
  "曾经有过一个部落，它的先知警告世人未来会有冻结世界的大暴雪。部落以为这是神明的愤怒，因此修筑祭坛，让神听到他们的呼唤。强大的魔像镇守这些祭坛，使得无人能偷走珠宝，避免激怒神明。不幸的是，冬天还是到来了（可能是因为只有僧侣们知道怎么让魔像停止运转，但据说这些僧侣早就带着一些珠宝跑掉了）。部落被冰冻毁灭，祭坛覆满霜雪。只有冰魔像依然在此守卫，等待春天来临。"
  )

// Winter becomes native in Blizzard, while Change replaces Winter in Cocytus
Orb("Change", "变化")
S("This ranged Orb will transform the target monster into one without any special powers. It also stuns them for one turn. "
  "Does not affect multi-tile monsters.",
  "这个远程法球会将目标怪物变成不具有任何特殊能力的怪物，同时击晕一回合。"
  )

// yet unused orb
Orb("the Triangle", "三角")
S("Whenever you attack with this Orb, you also hit the monsters adjacent to you which are neither adjacent nor opposite to the monster "
  "you originally attacked.",
  "在你持有这个法球攻击时，你同时会击中既不与目标怪物相邻也不在该怪物反方向的怪物。")
                                                                    
S("Using %the1 in %the2 sounds dangerous...", "在%2使用%1听起来很危险……")
S("%The1 is mostly useless in %the2...", "%1在%2基本没用……")
S("%The1 is forbidden in %the2 (disables some achievements)", "%1在%2受到禁止（会影响一些成就）")

S("You are ambushed!", "你中埋伏了！")
S("teleport", "传送")
S("ambush:", "埋伏：")

N("Hunting Dog (regrouping)", GEN_N, "狩猎犬（撤退）","条")
S("When your plan has clearly failed, it is better to abandon it and go to a safe place, to have a chance of succeeding next time. This dog clearly knows this.",
  "在你的计划明显失败后，最好放弃并躲到安全的地方，这样下次还有成功的机会。这条狗显然懂得这个道理。")

S("The Hunting Dogs give up.", "狩猎犬放弃了。")

// missing from previous versions:

S("\n\nThis orb also allows you to collect items encased in ice.",
  "\n\n这个法球使你能够收集冰封的物品。")

// vampire attack
S("%The1 drains your powers!", "%1汲取了你的力量！")

// additions
S(" This tour will not advance on its own -- you have to press Enter (not while reading help text).", 
  "这一游览不会自动前进——你需要按回车（在阅读帮助文本以外的地方）。"
  )

S("Hint: hold Alt to highlight enemies and other important features.",
  "提示：按住Alt来高亮敌人与其他重要物体。")

// Crystal World
N("Crystal World", GEN_N, "水晶世界","")
N("Glowing Crystal", GEN_N, "生长水晶","块")

S("Yes, this is definitely a crystal. A very regular crystalline structure.\n\n"
  "This land has been designed as a tool to play with various geometries, and it does not appear during a normal gameplay.",
  
  "没错，这肯定是块水晶。非常规则的晶体结构。\n\n"
  "这一地域被设计成实验各种几何的工具，不会在正常游戏中出现。")

S("You cannot move there!", "你不能移动到那里！")

// geometry stuff

S("experiment with geometry", "几何实验")

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
  
  "如果你想知道HyperRogue中的双曲几何如何影响游戏性，这个模式就是为你准备的！\n\n"
  "你可以在这里尝试各种各样的几何。我们首先把n边形以一种使每个顶点有k个相邻的n边形的方式粘合。取决于n与k的值，这也许能够折叠成一个球面、展开成一个平面，或者表现为双曲空间。可以通过把顶点替换为2k边形来将结果“截角”。此外，你可以尝试一下商几何。举例来说，椭圆几何是通过令球面上的对跖点为同一点达成的，因此在你从某点到达对跖点时，你会回到原点（但成为镜像）。玩得开心！大多数成就与排行榜在几何实验中不可用。\n\n"
  "在标准几何中（不管是否截角），你可以进行完整的游戏，但在其他几何中你需要选择特定的地域。地域在你在本次游戏流程中去往后解锁，或是在你收集25个对应宝藏后永久解锁。如果你想拜访很多地域，可以在欧几里得模式或非标准、非商的双曲混沌模式中的交叉路口寻找。\n"
  "亮点：\n"
  "* 水晶世界与扭曲海岸可以被视为额外的几何。\n"
  "* 万圣节专为球面几何设计。\n"
  "* 尝试在猎场切换为欧几里得几何来观察区别——变得不可能得分了。"  
  )

// new geometries available:
S("standard", "标准") // renamed from 'hyperbolic'

S("octagons", "八边形")
S("four pentagons", "四个五边形")
S("four hexagons", "四个六边形")
S("four heptagons", "四个七边形")
S("cube", "立方")
S("tetrahedron (buggy)", "四面体（有故障）")
S("tetrahedron", "四面体") // in case if it gets better

S("land", "地域")
S("bitruncated", "截角")
S("does not matter", "不影响")
S("sides per face", "每个面的边数")
S("faces per vertex", "每个顶点的面数")
S("none", "无")
S("zebra", "斑马")
S("field", "域")
S("quotient space", "商空间")
S("size of the world", "世界尺寸")
S("flat", "平坦")

// bitruncated or not bitrunc
S(" (b)", "（b）")
S(" (n)", "（n）")

S("(locked)", "（锁定）")

S("stereographic/orthogonal", "球极投影/正交")
S("Poincaré/Klein", "庞加莱/克莱因")
  
// Paper Model Creator
S("Useless in Euclidean geometry.", "在欧几里得几何中无用。")
S("Not implemented for spherical geometry. Please tell me if you really want this.",
  "未为球面几何开发，你要是真的想玩，向我反馈。")

// === 10.3 ===
// ============

// Missing/minor updated texts
//-----------------------------

// file selection dialog
S("Enter = choose", "Enter = 选择")
S("Esc = cancel", "Esc = 取消")

S("hyperbolic", "双曲")
S("reset special modes", "重置特殊模式")
S("orthogonal projection", "正交投影")
S("small Poincaré model", "小庞加莱模型")
S("big Poincaré model", "大庞加莱模型")
S("Klein-Beltrami model", "克莱因-贝尔特拉米模型")
S("bestiary of %the1", "%1的图鉴")

// for the map editor
N("Dragon Head", GEN_N, "巨龙头","颗")

S("score: %1", "得分：%1")
S("kills: %1", "击杀：%1")

// advanced config of quotient geometry
//--------------------------------------

S(
  "This geometry is obtained by applying the same 'generators' which "
  "lead to creating the given basic hyperbolic geometry, "
  "but using a fixed finite field instead of the field of reals. "
  "It can be also interpreted as a quotient of the given basic geometry. "
  "Warning: field patterns based on large primes might generate for a long time.",
  
  "这一几何由与基础双曲几何相同的“生成器”产生，但使用固定的有限域而非实数域。也可以理解为给定的基础几何的一个商。警告：基于大素数的域图案可能会耗时较长。")

S("find the next prime", "寻找下一个素数")
S("activate", "启动")
S("order %1%2 (non-bitruncated cells: %3)", "rząd %1%2 (nieprzyciętych pól: %3)")

// Torus/Klein bottle, Euclidean Squares, and their advanced configuration
//-------------------------------------------------------------------------

S("square grid", "方网格")

S("torus/Klein bottle", "环面/克莱因瓶")
S("mode", "模式")

S("single row (hex)", "单行（六边形）")
S("single row (squares)", "单行（方形）")
S("parallelogram (hex)", "平行四边形（六边形）")
S("rectangle (squares)", "矩形（方形）")
S("rectangle (hex)", "矩形（六边形）")
S("Klein bottle (squares)", "克莱因瓶（方形）")
S("Klein bottle (hex)", "克莱因瓶（六边形）")

S("this mode has bad patterns", "这一模式的图案不好")
S("incompatible with bitruncating", "与截角不兼容")
S("best if %1 is divisible by %2", "%1能被%2整除最好")

S("number of cells (n)", "格子数 (n)")
S("cell bottom-right from 0 (d)", "0右下的格子 (d)")
S("cell below 0 (d)", "低于0的格子 (d)")
S("width (x)", "宽度 (x)")
S("height (y)", "高度 (y)")

// upgraded Hypersian Rug mode
//-----------------------------

S("In the perspective projection, you can use arrows to rotate the camera, Page Up/Down to go forward/backward, Shift+arrows to strafe, and Ctrl+arrows to rotate the model.",
  "在视角投影中，你可以使用方向键旋转相机，Page Up/Down来前进/后退，Shift+方向键来平移，Ctrl+方向键旋转模型。")

S("Note: this mode is based on what you see on the screen -- but re-rendered in another way. "
  "If not everything is shown on the screen (e.g., too zoomed in), the results will be incorrect "
  "(though possibly interesting). "
  "Use a different projection to fix this.",
  
  "提示：这一模式基于你在屏幕上见到的内容——但以另一种方式重新渲染。如果并非一切都在屏幕上显示了（比如缩放太多），结果可能不准确（但也许很有意思）。换一种投影来避免这一点。")

S("vertex limit", "顶点限制")
S("The more vertices, the more accurate the Hypersian Rug model is. "
  "However, a number too high might make the model slow to compute and render.",
  "顶点越多，超波斯地毯模型就越精准。不过，太多顶点会让模型计算与渲染变慢。")

S("model distance", "模型距离")
S("In the perspective projection, this sets the distance from the camera to the center of the model. "
  "In the orthogonal projection this just controls the scale.",
  
  "在视角投影中，这一选项设定相机到模型中心的距离。在正交投影中，这一选项只控制缩放。")

S("maximum error", "最大误差")
S("New points are added when the current error in the model is smaller than this value.",
  "当前模型中误差小于该值时添加新点。")

S("field of view", "视野")
S("Horizontal field of view, in the perspective projection. "
  "In the orthogonal projection this just controls the scale.",
  "视角投影中的水平视野。在正交投影中，这一选项只控制缩放。")

S("native geometry", "原生几何")
S("scale model", "缩放模型")
S("This is relevant when the native geometry is not Euclidean. "
  "For example, if the native geometry is spherical, and scale < 1, a 2d sphere will be rendered as a subsphere; "
  "if the native geometry is hyperbolic, and scale > 1, a hyperbolic plane will be rendered as an equidistant surface. ",
  
  "在原生几何不为欧几里得时有用。"
  "比如说，如果原生几何为球面，缩放 < 1，2D球会被渲染为子球； "
  "如果原生几何为双曲，缩放 > 1，双曲面会被渲染为等距面。")

S("This just shows the 'z' coordinate of the selected point.",
  "这一项显示所选点的“z”坐标。")

S("radar", "雷达")
S("model iterations", "模型迭代")
S("keep shape", "保持形状")

// texture mode
//--------------

S("texture mode", "纹理模式")

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
  
  "这一模式使你能轻易切换地板密铺——选择“绘制新纹理”，然后像在画图程序里一样画就是了。得到的图案可以转换为几何，或者保存。\n\n"
  "你也可以使用任意图片作为纹理。最好使用球面/欧几里得/双曲密铺的图像（比如足球的照片，或者M.C.埃舍尔设计的密铺图像之一），但任意图片均可使用（最好在几何选择中开启“大图片”选项）。可以改变选择密铺的几何。\n\n")

S("texture mode (off)", "纹理模式（关）")
S("select geometry/pattern", "选择几何/图案")
S("reactivate the texture", "重新启用纹理")
S("open PNG as texture", "打开PNG作为纹理")
S("load texture config", "读取纹理设置")
S("warning: unable to find the center", "警告：无法确认中心")
S("texture size", "纹理尺寸")
S("paint a new texture", "绘制新纹理")
S("precision", "精确度")

S("texture mode (overlay)", "纹理模式（覆盖）")
S("select the texture's pattern", "选择纹理图案")
S("enable the texture", "启用纹理")
S("cancel the texture", "取消纹理")
S("move the model", "移动模型")
S("move the texture", "移动纹理")
S("zoom/scale the texture", "缩放纹理")
S("zoom/scale the model", "缩放模型")
S("projection", "投影")
S("affine transformations", "仿射变换")
S("magic", "魔法")

S("grid color (master)", "网格颜色（主）")
S("grid color (copy)", "网格颜色（复制）")
S("save the raw texture", "保存原始纹理")
S("Saved the raw texture to %1", "原始纹理已保存至%1")

S("texture auto-adjustment", "自动调整纹理")
S("drag from the model to the texture", "自模型拖动至纹理")
S("affect model scale", "影响模型缩放")
S("affect model projection", "影响模型投影")
S("affect model rotation", "影响模型旋转")
S("affect model position", "影响模型位置")
S("affect texture slanting", "影响模型倾斜")
S("affect texture stretching", "影响模型拉伸")
S("delete markers", "删除标记")
S("perform auto-adjustment", "进行自动调整")

S("texture mode (active)", "纹理模式（启用）")
S("deactivate the texture", "关闭纹理")
S("back to overlay mode", "返回覆盖模式")
S("change the geometry", "改变几何")
S("grid color", "网格颜色")
S("mesh color", "模型网格颜色")
S("color alpha", "颜色alpha")
S("The higher the value, the less important the color of underlying terrain is.",
  "值越高，其下地形颜色越不明显。")
S("edit the texture", "编辑纹理")
S("save the full texture image", "保存完整纹理图像")
S("Saving full texture to %1...", "完整纹理图像保存至%1中……")
S("save texture config", "保存纹理设置")

// pattern groups

S("football", "足球")
S("three colors", "三色")
S("chessboard", "棋盘")
S("single type", "单一类型")
S("large picture", "大图像")
S("periodic patterns", "周期图案")
S("more tuning", "更多调整")
S("BAD", "坏")

// extra patterns

S("Docks pattern", "港口图案")
S("broken Emerald pattern", "破碎绿宝石图案")
S("coloring", "上色")


// Docks
//-------

N("Docks", GEN_N, "港口","")
N("dock", GEN_N, "港口","处")
S("A dock.", "一处海港。")
N("burning dock", GEN_N, "燃烧港口","处")
S("A burning dock.", "一处着火的港口。")

N("Sea Glass", GEN_N, "海玻璃","块")

// Snake Nest

N("Snake Nest", GEN_N, "蛇巢","")

N("Snake Oil", GEN_N, "蛇油","瓶")

S("Made of actual snakes!", "含有真蛇成分！")

// Ruined City

N("Ruined City", GEN_N, "城市废墟", "")

S("Once a beautiful city... but now overrun by the mighty Raiders of unknown origin.\n\n"
 "Raiders cannot be harmed with mundane weapons, but each color has its movement restrictions.",
 
 "曾经是一座美丽的城市……现在被不知从何而来的强大劫掠者侵占。\n\n"
 "劫掠者无法以寻常兵刃伤到，但每种颜色的劫掠者各有移动限制。")

N("ruin wall", GEN_N, "废墟墙","堵")

N("Chrysoberyl", GEN_N, "金绿宝石","枚")
S("Fragment of the past glory.", "Fragment dawnej chwały过去荣耀的碎片。")

N("Red Raider", GEN_N, "红劫掠者","名")
S("Red Raiders travel in pairs. They have promised to always watch each other's backs. They are able to destroy walls on their way.",
  "红劫掠者成对而行。他们发誓要互相断后。他们可以摧毁前进途中的墙壁。")

N("Gray Raider", GEN_N, "灰劫掠者","名")
S("Gray Raiders never step on gray cells.",
  "灰劫掠者从不踩上灰色格子。")

N("Green Raider", GEN_N, "绿劫掠者","名")
S("Green Raiders never step from one green cell to another.",
  "绿劫掠者从不从一处绿色格子走上另一处绿色格子。")

N("Brown Raider", GEN_N, "棕劫掠者","名")
S("Brown Raiders never move adjacent to an item.",
  "棕劫掠者从不移动至物品旁。")


N("Blue Raider", GEN_N, "蓝劫掠者","名")
S("Blue Raiders have a powerful attack which takes two turns to complete, and also makes the Blue Raider stunned "
  "for a long time. This attack can destroy other Raiders if it hits them.",
  "蓝劫掠者会使用需要两个回合才能完成的强力攻击，结束后还会眩晕一段时间。这种攻击如果击中其他劫掠者会将其摧毁。"
  )

S("You cannot attack Raiders directly!", "你无法直接攻击劫掠者！")

Orb("Slaying", "斩杀")

S("This Orb lets you defeat Raiders and other tough single-cell monsters in melee.",
  "这个法球让你能够近战击杀劫掠者或其他单格的坚韧敌人。")

// Jelly Kingdom

N("Jelly Kingdom", GEN_N, "果冻王国","")

S("Some of the Slime Beasts have decided to revolt against the color rules in the "
  "Alchemist Lab. They have changed their shape and consistency, declared independence, and established their own Kingdom.\n\n"
  "Jellies switch between being a wall and being a monster after every treasure you collect.",
  
  "有些黏液兽决定反抗炼金实验室的颜色规则。它们改变了自己的外形与性质，宣告独立，建立自己的王国。\n\n"
  "果冻在你收集宝藏后会从墙壁变为怪物，或从怪物变为墙壁。"
  )

N("Red Jelly", GEN_N, "红果冻","块")
N("Blue Jelly", GEN_N, "蓝果冻","块")

N("Tasty Jelly", GEN_N, "可口果冻","块")
S("A tasty byproduct of the Jelly Revolution.",
  "果冻起义的美味副产品。")

Orb("Phasing", "相位")
S("This orb lets you pass through walls (one cell wide), and also through monsters, as long as they will not attack you in transit.",
  "这个法球使你能穿墙（一格厚）而过，也可以穿过怪物，只要它们不会在路途中攻击你。")

S("You cannot attack Jellies in their wall form!", "你无法攻击墙形态的果冻！")

// keyboard help
//---------------

S("advanced keyboard shortcuts", "高级快捷键")

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

S("pics to save:", "要保存的图片：")
S("level to save:", "要保存的关卡：")
S("pics to load:", "要读取的图片：")
S("level to load:", "要读取的图片：")

// new option
S("forget faraway cells", "遗忘远处的格子")
N("Lost Memory", GEN_N, "失落记忆","处")
S(  "Because of the properties of hyperbolic geometry, it is extremely unlikely to randomly "
    "get back to a faraway place you have been to. However, you have managed to get there "
    "somehow. In the meantime, its memory has been cleared, since the 'forget faraway cells'"
    " option was on.",
    
    "由于双曲几何的性质，你回到某一遥远位置的概率极低。不知为何，你还是找到了曾经去过的地方。在你离开时，这里的内存已经被清理了，因为你启用了“遗忘远处的格子”选项。"
    )

// shmup 
N("Blueball", GEN_N, "蓝色球","枚")
S("A powerful missile from a Blue Raider.", "蓝劫掠者发射的强大远程武器。")

// missing
S(" fps: ", " fps: ")
S("default value", "默认值")
S("You can scroll with arrow keys -- Ctrl to fine-tune", "你可以使用方向键滚动——Ctrl微调")
S("font scale", "字体缩放")
S("background particle effects", "背景粒子效果")

// not yet translated for Hypersian Rug mode
S("orthogonal", "正交")
S("perspective", "视角")
S("model scale factor", "模型缩放参数")

// stereo vision config
S("stereo vision config", "立体视野设置")

S("This allows you to view the world of HyperRogue in three dimensions. "
  "Best used with the Hypersian Rug mode. When used in the disk model, "
  "this lets you look at the Minkowski hyperboloid (which means the "
  "depth of terrain features is actually reversed). It also works with non-disk models, "
  "from the conformal menu.",
  "这一选项使你能够在三维中观看HyperRogue的世界。最好配合超波斯地毯模式使用。在碟模型中，你所观看的是闵可夫斯基双曲体（因此地形的深度其实是反转的）。这一选项也可用于非碟模型，在保形菜单中选择。"
  )

S("Currently, red-cyan anaglyph glasses and mobile VR googles are supported.",
  "目前支持红蓝眼镜及移动VR眼镜。")

S("stereo mode", "立体模式")
S("anaglyph", "浮雕")
S("side-by-side", "双侧")

S("pupillary distance", "瞳距")
S("The distance between your eyes in the represented 3D object. This is given in absolute units.",
  "给出3D物体中你双眼的间距。以绝对单位衡量。")

S("distance between images", "图像距离")
S("The distance between your eyes. 1 is the width of the screen.",
  "你双眼的间距。1为屏幕宽度。")

// field of view
S("Horizontal field of view, in angles. "
  "This affects the Hypersian Rug mode (even when stereo is OFF) "
  "and non-disk models.",
  "水平视野角度。这会影响超波斯地毯模式（即使关闭立体）与非碟模型。")

// more magic configuration
S("affect model central point", "影响模型中心点")
S("affect texture position X", "影响纹理位置 X")
S("affect texture position Y", "影响纹理位置 Y")

S("select master cells", "选择主格子")
S("select master triangles", "选择主三角形")
S("fine tune vertices", "微调顶点")

// main menu
// an option to keep the current settings
S("use current/saved settings", "使用当前/已保存设置")
// randomly chosen mode: hypersian rug
S("see the true form", "观察真实形态")
// randomly chosen mode: heptagonal mode
S("more curvature", "更多曲率")

// anti-crossing in the Hypersian Rug
S("anti-crossing", "反相交")
S("The anti-crossing algorithm prevents the model from crossing itself, "
  "by preventing points which should not be close from being close. "
  "The bigger number, the more sensitive it is, but the embedding is slower. Set 0 to disable.",
  
  "反相交算法通过防止不应过于接近的点靠近来防止模型与自身相交。参数越高，算法越敏感，但运行越慢。设置为0来禁用。"
  )

// new 3D models

// various menu names

S("constant curvature surfaces", "常曲率表面")
S("smooth surfaces", "平滑表面")
S("models of hyperbolic geometry", "双曲几何模型")
S("history mode", "历史模式") // old 'conformal/history mode' has been split into two
S("other 3D models", "其他3D模型")

// various models

S("Minkowski hyperboloid", "闵可夫斯基双曲体")
S("plane", "平面")
S("sphere", "球")
S("hemisphere", "半球")
S("tractricoid", "伪球面")
S("concave barrel", "凹管")
S("Dini's surface", "Dini的曲面")
S("Kuen surface", "库恩曲面")
S("inversion", "反演")

S(
  "In this menu you can choose from several known smooth surfaces of constant negative curvature. "
  "Since the curvature of hyperbolic plane is also constant negative, these surfaces "
  "are great to draw hyperbolic tesselations on. While they look great, only a small part "
  "of the hyperbolic plane can be visibly represented in this way, so they are not "
  "good for playing HyperRogue; however, the coverage extends far away in specific directions, "
  "so first increasing sight range in graphics config and texture size in hypersian rug settings may improve the effect. "
  "For convenience, you can also choose other 3D models from this menu.",
  
  "在该菜单中你可以选择多种负曲率的已知光滑曲面。由于双曲面的曲率也是负常数，这些曲面很适合用于绘制双曲密铺。虽然看起来不错，但只有一些双曲面能以这种方式表示出来，并不适合游玩HyperRogue；不过，在某些特定方向的覆盖会延伸很远，因此在图形设置中增加视野范围并在超波斯地毯设置中增加纹理尺寸可能会提升效果。出于方便考虑，你也可以在该菜单选择其他3D模型。"
  )

// coverage
S("display coverage", "显示覆盖率")
S("on surface", "表面")
S("on H² (static)", "H²（静态）")
S("on H² (dynamic)", "H²（动态）")

S("Computing these models involves integrals and differential equations, which are currently solved numerically. This controls the precision.",
  "计算这些模型需要积分与微分方程，目前以数值方式求解。该选项控制精度。")

// too bored for declension of %1 (either a surface name or a a part of a surface)
S("solving the geodesics on: %1, %2/%3", "求解测地线： %1，%2/%3")

S("the upper component", "上侧组成部分")
S("the lower center", "下侧中心")
S("the lower left", "下侧左")
S("the lower right", "下侧右")

S("camera rotation in 3D models", "3D模型相机旋转")

S("Rotate the camera in 3D models (ball model, hyperboloid, and hemisphere). "
  "Note that hyperboloid and hemisphere models are also available in the "
  "Hypersian Rug surfaces menu, but they are rendered differently there -- "
  "by making a flat picture first, then mapping it to a surface. "
  "This makes the output better in some ways, but 3D effects are lost. "
  "Hypersian Rug model also allows more camera freedom.",
  
  "旋转3D模型中的相机（球面模型、双曲体、半球）。注意双曲体与半球模型也在超波斯地毯模式的曲面菜单中存在，但渲染效果不同——首先产生一张平的图片，然后映射到曲面上。这使得输出质量稍好，但失去了3D效果。超波斯地毯模式也使相机更灵活。")

// parameter for the given model
S("parameter", "参数")
S( "Stereographic projection to a sphere. Choose the radius of the sphere.",
  "球极投影至球面。选择球的半径。")
// parameter for Dini
S("The larger the number, the more twisted it is.",
  "数值越高越扭曲。")
// parameter for concave barrel
S("Controls the inner radius.", "控制内半径。")

// improved rotation
S(
  "This controls the automatic rotation of the world. "
  "It affects the line animation in the history mode, and "
  "lands which have a special direction. Note that if finding this special direction is a part of the puzzle, "
  "it works only in the cheat mode.",
  
  "这一选项控制世界的自动旋转。它影响历史模式中的线动画，以及有特定方向的地域。请注意如果寻找特殊方向是谜题的一部分，那该选项只在作弊模式中生效。"
  )
S("NEVER", "永不")

S("Mercator", "墨卡托")
S("band equidistant", "带等距")
S("band equi-area", "带等面积")
S("sinusoidal", "正弦")
S("two-point equidistant", "两点等距")

S(
  "This model maps the world so that the distances from two points "
  "are kept. This parameter gives the distance from the two points to "
  "the center.",
  
  "这一模型映射世界的方式使得两点间距不变。该参数给出两点与中心的距离。")

S("enable the Camelot cheat", "开启卡美洛作弊")
S("disable the Camelot cheat", "关闭卡美洛作弊")
S("gain Orb of the Sword", "获取宝剑法球")
S("Static mode enabled.", "静态模式已启动。")
S("Static mode disabled.", "静态模式已关闭。")

S("Returned to your game.", "返回到了你的游戏。")
S("Spherical version of %the1. ", "%1的球面版。")
S("Euclidean version of %the1. ", "%1的欧几里得版。")
S("Click again to go back to your game.", "再次点击以返回你的游戏。")
S("Press %1 again to go back to your game.", "再次按%1以返回你的游戏。")
S("return to your game", "返回你的游戏")

S("orthogonal projection", "正交投影")
S("stereographic projection", "球极投影")
S("zoomed stereographic projection", "缩放球极投影")
S("zoomed out", "缩小")
S("zoomed in", "放大")
S("gnomonic projection", "球心投影")
S("Goldberg", "戈德堡")
S("fisheye", "语言")
S("paint pictures", "绘制图像")
S("list of slides", "页面列表")
S("back to the start menu", "返回起始菜单")
S("You juggle the Dead Orbs.", "你拿失效法球玩起了杂耍。")
S("You give %the1 a grim look.", "你阴森地看了%1一眼。")

S("Strange Challenge", "奇异挑战")
S("compete with other players on random lands in random geometries", "与其他玩家在随机几何下的随机地域中竞争。")

S("Strange Challenge #%1", "奇异挑战 #%1")
S("next challenge in %1", "下一次挑战：%1")
S("synchronizing scores...", "同步得分中……")
S("already played", "已经玩过了")
S("play!", "开始！")
S("See past challenges", "浏览过去的挑战")
S("Past Strange Challenges. You can see the setting and high scores, but you can no longer play them.",
  "过去的奇异挑战。可以浏览设置和高分榜，但是已经不能玩了。")

// land compatibility guides

S("Cannot kill Hedgehog Warriors in this geometry.", "无法在该地形中击杀刺猬战士。")
S("No random pattern version.", "无随机图案版。")
S("Great Walls not implemented.", "长城未在此实现。")
S("Pattern incompatible.", "图案不兼容。")
S("Pattern not implemented -- using random.", "图案未实现——使用随机中。")
S("Pattern not implemented.", "图案未实现。")
S("Not enough space.", "空间不足。")
S("Does not work in this geometry.", "在该几何中无效。")
S("This land is designed for bounded worlds.", "该地域专为有界世界设计。")
S("This land is designed for infinite worlds.", "该地域专为无限世界设计。")
S("This land is designed for infinite worlds or big spheres.", "该地域专为无限世界或大球面设计。")
S("Out of theme for the full game.", "与完整游戏的格调不合。")
S("No game here.", "这里没有游戏内容。")
S("Does not work in chaos mode.", "在混沌模式中无效。")
S("Not in the full game.", "不在完整游戏中出现。")
S("Special construction in the Chaos mode.", "混沌模式的特殊构造。")
S("Special construction in the Euclidean mode.", "欧几里得模式的特殊构造。")
S("Special construction in this geometry.", "该几何中的特殊构造。")
S("Not implemented.", "未实现。")
S("Partially implemented.", "部分实现。")
S("No comments.", "无注释。")
S("Does not work in pure tactics mode.", "在纯粹战术模式中无效。")
S("Technical.", "技术。")
S("Full game.", "完整游戏。")
S("Somewhat inaccurate.", "有些不准确。")
S("Mercury rivers not implemented (or could not work) in this geometry.", "水银河在该地域未实现（或无效）。")
S("Patterns compatible.", "图案可用。")
S("This land is specially designed for this geometry.", "该地域专为该几何设计。")
S("Three-colorability required.", "需要三色。")
S("Land not implemented.", "地域未实现。")
S("Special interest.", "特殊兴趣。")//unclear
S("Better version exists.", "存在更好的版本。")
S("Grid does not work in this geometry.", "网格在该地域无效。")
S("Graphics not implemented in this geometry.", "该地域未实现图像。")
S("This land does not work in the current settings. Reason not available.",
  "该地域在当前设置下无效。原因不明。")
S("This land does not work well in the current settings. Reason not available.",
  "该地域在当前设置下效果不佳。原因不明。")
S("This combination is known to be buggy at the moment.", "这一组合目前已知故障较多。")
S("Somewhat sloppy pattern.", "有些难看的图案。")
S("Fractal landscapes not implemented in this geometry.", "分形地形在当前几何未实现。")
S("Only simplified walls implemented.", "仅实现了简单墙壁。")

S("cube/elliptic", "立方/椭圆")
S("chamfered", "倒角")
S("2x chamfered", "2x 倒角")
S("select", "选择")

S("Goldberg polyhedra are obtained by adding extra hexagons to a dodecahedron. "
  "GP(x,y) means that, to get to a nearest non-hex from any non-hex, you should move x "
  "cells in any direction, turn right 60 degrees, and move y cells. "
  "HyperRogue generalizes this to any tesselation with 3 faces per vertex. "
  "By default HyperRogue uses bitruncation, which corresponds to GP(1,1).",
  
  "戈德堡多面体是由将六边形拼接在十二面体上得到的。GP(x,y)指，想要从非六边形抵达另一个最近的非六边形，你应该向任意方向移动x格，转60度，再移动y格。HyperRogue将此扩大到任意每个顶点三个面的密铺。HyperRogue默认使用截角，对应GP(1,1)。")
  
S("extend the ends", "扩展末端")

S("Forbidden cells are marked with a different color.", "禁用的格子以不同的颜色标记。")
S("This orb lets you attack adjacent cells %1 steps from the primary target.", "这个法球使你能够攻击距离主要目标%1格外的相邻格子。")

S("Note: the rules above correspond to the standard geometry; actual rules in other geometries may be different. ",
  "提示：以上的规则对应于标准几何；其他几何的具体规则可能不同。")

S("Note: the rules refer to colors which are not visible in other lands. ", "提示：该规则指向其他地域中不可见的颜色。")

S("When the 'mark heptagons' option (hotkey '7') is on, forbidden moves are marked.", 
  "在“标记七边形”选项（快捷键“7”）启用时，不能进行的行动会被标记。")

S("When the 'mark heptagons' option (hotkey '7') is on, moves between unmarked cells are forbidden.",
  "在“标记七边形”选项（快捷键“7”）启用时，不能在未被标记的格子间移动。")

// 10.4p

// in the sight range dialog
S("enable the cheat mode for additional options", "开启作弊模式来查看更多选项")

// steal a baby tortoise with Orb of Space
S("No, that would be heartless!", "不行，这太无情了！")

S("show quotient spaces", "显示商空间")
S("minimal quotient", "最小商")
S("Klein Quartic", "克莱因四次曲线")
S("Bolza Surface", "博尔查曲面")
S("Bolza Surface x2", "博尔查曲面 x2")

// extra texts for Goldberg 

S("variations", "变化")
S("auto-adjust in Goldberg grids", "在戈德堡格子中自动调整")
S("2x bitruncated", "2x 截角")
S("expanded", "扩展")
S("rectified", "纠正")

// irregular grids

S("irregular", "不规则")
S("bad cells: %1", "坏格子：%1")
S("too many edges: %1", "过多边缘：%1")
S("median edge: %1 minimum: %2", "边缘中位数：%1 最小值：%2")
S("cells badly paired: %1", "匹配不当格子：%1")
S("cells not covered: %1", "未覆盖格子：%1")
S("adjacent cells from nonadjacent heptagons: %1", "非相邻七边形的相邻格子：%1")
S("bad format or bad map geometry", "格式不当或地图几何不当")
S("irregular grid", "不规则网格")
S("density", "密度")
S("min edge to median", "最小边缘/中位数")
S("quality", "质量")
S("save", "保存")
S("load", "读取")
S("irregular to save:", "要保存的不规则：")
S("irregular to load:", "要读取的不规则：")
S("Map saved to %1", "地图已保存至%1")
S("Failed to save map to %1", "地图保存至%1失败")
S("Map loaded from %1", "地图已读取自%1")
S("Failed to load map from %1", "地图读取自%1失败")
S("reset", "重置")
S("OK", "OK")
S("ready", "准备")

S("This option creates irregular grids to play the game on. "
  "Currently rather slow algorithms are used, "
  "so not recommended with too high density or "
  "with too large periodic base geometry. "
  "For technical reasons, the density cannot be too small.",

  "该选项创造出不规则的格子用来进行游戏。目前所使用的算法比较慢，所以最好不要使用太高的密度或太大的周期几何。由于某些技术原因，密度也不能太低。"
  )

// vertical stretch

S("vertical stretch", "垂直拉伸")
S("Vertical stretch factor.", "垂直拉伸参数。")
S("Gall-Peters", "高尔-彼得斯投影")
S("The current value makes the map conformal at the latitude of %1 (%2°).", "当前值使得地图在纬度%1(%2°)时保形。")
S("The current value makes the map conformal %1 units from the main line.", "当前值使得地图在距离主线%1个单位时保形。")

// line quality

S("line quality", "线质量")

S("Higher numbers make the curved lines smoother, but reduce the performance.",
  "该数值越高，曲线越平滑，但会影响性能。")

// new options in mobile

S("FIRE", "发射")

S("scrolling by device rotation", "重力感应")

S(  "This lets you scroll the map by rotating your device. It can be e.g. used to "
    "play the spherical mode of HyperRogue in mobile VR goggles -- the \"spherical VR\" "
    "button configures this; this VR mode can be disabled by touching the screen for 1 second.",

  "这一选项使你能通过旋转设备来滚动地图。使用移动VR眼镜时可以用于游玩HyperRogue的球面模式——“球面VR”按钮提供相关设置；该VR模式可以通过触摸屏幕1秒来关闭。")

S("sensitivity", "敏感度")
S("1 means that rotating the device by 1 radian corresponds to scrolling by 1 unit. In spherical geometry, 1 unit = 1 radian.", 
  "1代表移动设备1弧度等同于滚动1单位。在球面几何中，1单位=1弧度。")

S("spherical VR", "球面VR")



S("aura from texture", "纹理光环")

// extra for irregular
S("bitruncation count", "截角计数")

S("Bitruncation introduces some regularity, allowing more sophisticated floor tilings and textures.", 
  "截角产生更多规律，便于构建复杂的地板镶嵌与纹理。"
  )

S(
      "The smallest allowed ratio of edge length to median edge length. "
      "Tilings with low values are easier to generate, but tend to be more ugly.", 
      "边缘长度与中位边缘长度的最小允许比率。"
      "数值小的镶嵌更容易生成，但往往更难看。"
      )

// missing

S("The Orb of Love gives no bonus score in the Strange Challenge.", "爱情法球在这个奇异挑战中不给予额外分数。")
S("This does not work in non-orientable geometries", "在没有取向的地域无效。")
S("This pattern needs x-y divisible by 3", "该图案需要x-y可被3除尽")
S("This pattern needs x-y divisible by 2", "该图案需要x-y可被2除尽")

// missing from the texture editor
S("p = color", "p = 颜色")
S("b = brush size: %1", "b = 笔刷尺寸：%1")
S("d = draw", "d = 绘制")
S("l = line", "l = 线条")
S("c = circle", "c = 圆")

// new binary tiling

S("binary tiling", "二元镶嵌")

// Archimedean tilings

S("Archimedean tilings", "阿基米德镶嵌")
S("Archimedean", "阿基米德")

S("This lets you generate tilings where each cell is a regular polygon, and each vertex is identical. "
  "You can choose from a big list of known Archimedean tilings, or create your own. "
  "The format of specifying Archimedean tilings is based on one used by the Hyperbolic Applet by Don Hatch.\n\n"
  "You can enhance the gameplay/land generation by adding special codes after faces: "
  "H ('heptagon' for Ruined City, Graveyard, etc.), L ('line' for Zebra, Power, Vineyard, etc.).\n"
  "Crystal World and Warped Coast work on (k,l,m) tilings by removing/enlarging the first tile listed ('k').",

  "该选项生成每一格子都是规则多边形、每一顶点都等价的镶嵌。你可以在已知的阿基米德镶嵌列表中选择，或自己创建。具体阿基米德镶嵌的格式基于Don Hatch的Hyperbolic Applet。\n\n"
  "你可以通过在面后增加额外代码来增强游戏内容/地域生成：H（城市废墟、墓地等的“七边形”）、L（斑马、力量、藤蔓园等的“线”）。\n"
  "水晶世界与扭曲海岸在移除/放大“k”后适用于(k,l,m)镶嵌。")

S("color by symmetries", "颜色 对称")
S("color by sides", "颜色 边")

S("edit", "编辑")
S("full angle", "全角度")
S("not enough faces", "面不足")
S("not enough edges", "边不足")
S("polygons match incorrectly", "多边形匹配不正确")
S("currently no more than %1 edges", "目前不多于%1个边")
S("currently no more than %1 faces in vertex", "目前每个顶点不多于%1个面")

N("Canvas", GEN_N, "画布","块")
S("find the player", "寻找玩家")
S("press SPACE to find the player", "SPACE = 寻找玩家")

S("edge length", "边长")
S(" (arbitrary)", "（任意）") // arbitrary edge length
S("previous page", "上一页")
S("Defined pattern.", "规定图案。")

// graphics editor
S(" (r = complex tesselations)", " （r = 复杂密铺）")
S("area: %1", "面积：%1")

// short codes for variations
S(" (d)", " (d)") // dual
S(" (g)", " (g)") // Goldberg
S(" (i)", " (i)") // irregular

// pattern types
S("cell types", "格子类型")
S("siblings", "姊妹") // a pattern on the sphere
S("rotate the color groups", "旋转颜色组")
S("full symmetry", "完全对称")
S("alternate coloring", "替代颜色")
S("display full floors", "显示完整地板")
S("remove complete classification", "移除完整分类")

// animations
S("animations/history", "动画/历史")
S("animations", "动画")
S("period", "周期")
S("game animation period", "游戏动画周期")
S("no movement animation", "无移动动画")
S("translation", "过渡")
S("parabolic", "抛物线")
S("circle", "圆")
S("animate parameter change", "动画参数修改")
S("choose parameters to animate", "选择动画参数")
S("frames to record", "录制帧")
S("record to a file", "录制自文件")

// improved worldsize demo dialog
S("which distance", "何种距离")
S("start", "开始")
S("land", "地域")

S("number codes", "数字代码")
S("distance", "距离")
S("type", "类别")
S("debug", "debug")

S("color codes", "颜色代码")
S("display distances from", "显示距离")
S("disable", "禁用")
S("move the player", "移动玩家")
S("show number of cells by distance", "按距离显示格子数")
S("show number of descendants by distance", "按距离显示衍生数")

S("triangle grid: tree edges", "三角形网格：树边")
S("triangle grid: other edges", "三角形网格：其他边")
S("dual grid", "双网格")
S("horocycles", "极限圆")
S("configure", "设置")
S("scroll", "滚动")

// improved sight range dialog
S("minimum visible cell in pixels", "可见格子的最小像素尺寸")
S("draw range based on distance", "基于距离的绘制范围")
S("draw based on size in the projection (no generation)",
  "基于投影中尺寸的绘制（无生成）")
S("draw based on size in the projection (generation)",
  "基于投影中尺寸的绘制（生成）")
S("cells drawn", "绘制的格子")
S("limit on cells drawn", "绘制格子的限制")
S("This limit exists to protect the engine from freezing when too many cells would be drawn according to the current options.",
  "该限制用于防止引擎在当前设置下因绘制过多格子而卡死。")
S("generation range bonus", "额外生成范围")
S("game range bonus", "额外游戏范围")

// new options for models

S("Joukowsky transform", "茹科夫斯基变换")
S("Joukowsky+inversion", "茹科夫斯基 + 反转")

S("maximum z coordinate to show", "显示的最大z坐标")

S("model transition", "模型渐变")
S("You can change this parameter for a transition from another model to this one.",
  "你可以修改该参数来使另一个模型渐变为这一个。")

S("model orientation", "模型取向")

// 10.4z

S("left-handed", "左撇子")

// formula features

S(
  "This lets you specify the projection as a formula f. "
  "The formula has access to the value 'z', which is a complex number corresponding to the (x,y) coordinates in the currently selected model; "
  "the point z is mapped to f(z). You can also use the underlying coordinates ux, uy, uz.",
  
  "该选项使你能给定投影的公式f。 "
  "该公式中的值“z”是对应目前所选模型中(x,y)坐标的复数；点z被投影到f(z)。你也可以使用坐标ux、uy、uz。")

S("Functions available: %1", "可用函数：%1")

S("animate parameters", "动态化参数")

S(
  "Most parameters can be animated simply by using '..' in their editing dialog. "
  "For example, the value of a parameter set to 0..1 will grow linearly from 0 to 1. "
  "You can also use functions (e.g. cos(0..2*pi)) and refer to other parameters.",
  
  "大多参数可以在编辑对话中使用“..”来动态化。比如，设为0..1的参数会从0线性增长至1.=。你也可以使用函数（比如cos(0..2*pi)）并引用其他参数。")

// color edit dialog

S("background", "背景")
S("foreground", "前景")
S("projection boundary", "投影范围")
S("projection background", "投影背景")
S("projection period", "投影周期")
S("dialogs", "对话")
S("canvas colors", "画布颜色")
S("minefield colors", "雷区颜色")
S("distance colors", "距离颜色")

// extra conformal models

S("rotated hyperboles", "旋转双曲线")
S("spiral", "螺旋")

S("spiral angle", "螺旋角")
S("spiral period: x", "螺旋周期：x")
S("spiral period: y", "螺旋周期：y")
S("match the period of the torus", "与环面周期对应")

// extra patterns

S("circles", "许多圆")
S("radii", "半径")
S("meridians", "子午线")
S("parallels", "平行线")
S("seven-coloring", "七色")

S("half-plane scale", "半平面缩放")
S("use GPU to compute projections", "使用GPU计算投影")
S("N/A", "不可用")
S(" (2D only)", " (仅2D)")

// 10.5d

// this line was missing...
S("geometry experiments", "几何实验")

S("cylinder (squares)", "圆柱体（方形）")
S("cylinder (hex)", "圆柱体（六边形）")
S("Möbius band (squares)", "莫比乌斯环（方形）")
S("Möbius band (hex)", "莫比乌斯环（六边形）")

S("Macbeath Surface", "Macbeath曲面")
S("Bring's Surface", "Bring曲面")
S("Schmutz's M(3)", "Schmutz曲面 M(3)")
S("Schmutz's M(4)", "Schmutz曲面 M(4)")

S("dimensional crystal", "维度晶体")

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
    
    "这种几何基本上是让你在d维网格中进行游戏。选择三维与“3D显示”来看看是怎么回事——我们其实是在三维空间中的一处六边形拼成的周期平面上进行游戏；每个六边形在可能的六个方向连接其他的六边形。通常，游戏以居住在其中的生物的视角显示（经过规整与平滑后），假定光线也被束缚在平面上——这看起来和{2d,4}镶嵌完全相同，只除了光线有时会形成环路，使得你能在某些方向看到自己的影像（换句话说，d维网格是双曲面镶嵌的商）。同样的建构对于其他维度也生效。半维度采用如下定义：多余的维度仅有两个“层次”，比如2.5D存在上半平面与下半平面。\n\n"
    "你也可以对该几何进行截角——这使其在HyperRogue的规则下效果更好，但有些难以理解。")

S("4D double bitruncated", "4D双截角")
S("view coordinates in the cheat mode", "在作弊模式中查看坐标")
S("compass probability", "指南针概率")
S("3D display", "3D显示")

S("\"Each piece of the Round Table is exactly %1 steps away from the Holy Grail.\"",
  "“圆桌的每一块都正好距离圣杯%1步。”")

S("\"According to Merlin, the Round Table is a perfect Euclidean sphere in %1 dimensions.\"",
  "“梅林说，圆桌在%1维中是完美的欧几里得球面。”")

S(
  "Lands in this geometry are usually built on North-South or West-East axis. "
  "Compasses always point North, and all the cardinal directions to the right from compass North are East (this is not "
  "true in general, but it is true for the cells where compasses are generated). "
  "North is the first coordinate, while East is the sum of other coordinates.",
  
  "该几何中的地域大多基于南北或东西轴。指南针总是指向北方，自指南针北方右侧所有正方向都为东方（这并非在任意地方适用，但一定在生成指南针的格子适用）。北方是第一项坐标，东方是其他坐标的和。")

S("crystal coordinate colors", "水晶坐标颜色")

S("3D monsters/walls on the surface", "曲面上的3D怪物/墙壁")
S("no 3D effects available in this projection", "在该投影中没有可用的3D效果")

// new screenshot menu

S("screenshots", "截图")
S("format", "格式")
S("pixels (X)", "像素 (X)")
S("pixels (Y)", "像素 (Y)")
S("supersampling", "超采样")
S("transparent", "透明")
S("gamma", "gamma")
S("higher value = darker", "值越高 = 越暗")
S("brightness", "亮度")
S("higher value = lighter", "值越高 = 越亮")
S("show the HUD", "显示HUD")
S("SVG screenshots do not work in this 3D mode", "SVG截图在该3D模式中不可用")
S("SVG screenshots do not work with textures", "SVG截图与纹理不兼容")
S("take screenshot", "进行截图")
S("screenshot", "截图")
S("shot settings", "截图设置")

// new warnings

S("WARNING", "警告")
S("YES", "是")
S("NO", "否")

S("Are you sure you want to hit %the1?", "你确定你想要攻击%1？")
S("This will exit HyperRogue without saving your current game. Are you sure?",
  "你将在未保存当前游戏的情况下退出HyperRogue。确定吗？")
S("This will end your current game and start a new one. Are you sure?",
  "这会结束你当前的游戏并开始新游戏。确定吗？")
S("This will enable the cheat mode, making this game ineligible for scoring. Are you sure?",
  "这会开启作弊模式，使得游戏无法得分。确定吗？")

// 11.0

S("debug values", "debug值")
S("internal details", "内部细节")
S("show debug data", "显示debug数据")
S("click a cell to edit it", "点击格子编辑")
S("click a cell to view its data", "点击格子检视数据")

S("(these lands are not in game)", "（这些地域并不在游戏中出现）")

// new lands

N("Brown Island", GEN_N, "棕岛","")

S("The Brown Islands have been generated by ancient underground creatures, who moved randomly and raised lands in their path... "
  "adults spawned larvae in each move, which also moved randomly and also raised lands in their path, and eventually became adults and spawned their own larvae. "
  "The creatures spawned exponentially, but the space they lived in still grew faster, so there was enough space for the whole spawn...\n\n"
  "That was a long time ago. Now, the mountainous Brown Islands are inhabited by strange monsters. Good luck adventuring!",
  
  "棕岛由远古的地下生物生成，它们随机移动，在路径上抬升土地……成体生物每一步都产生幼体，幼体也随机移动，抬升土地，最后变成成体开始产生自己的幼体。这些生物呈指数级增长，但它们生存的空间增长还要更快，所以足够它们繁衍……\n\n"
  "那些都是很久以前发生的事了。现在，多山的棕岛里居住着奇异的怪物。祝你冒险好运！")
  

N("Bronze Beast", GEN_N, "青铜兽","头")

S("A large bug native to the Brown Islands. Cannot be killed easily due to their tough armor; still, they can be killed by pushing them into water or from great heights. "
"Bronze Beasts are very bad at descending slopes -- it takes them extra time to get down, and they are stunned for a longer time if you push them down.",

  "一种棕岛原生的大虫子。长有厚重的盔甲，无法轻易杀死；不过还是可以通过把它们推进水里或者从高处丢下去来杀死它们。青铜兽非常不擅长下坡——它们向下移动要多花很多时间，而且如果你把它们推到坡下面，它们会眩晕更久。")

N("Acid Gull", GEN_N, "酸鸥","只")

S("Where did this strange bird come from?...\n\nAcid Gulls dissolve the land on which they fall when they die. ",

  "这些怪鸟是从哪来的？……\n\n酸鸥死掉时会溶解它们的尸体落到的格子。")

N("Tiger's Eye", GEN_N, "虎眼石","枚")
S("A brown gem.", "一枚棕色的宝石。")

Orb("Choice", "抉择")

S("Did you know that it is possible to break a magical sphere into finitely many parts, and then, out of the parts obtained, to make two magical spheres, just like the original? "
  "Hyperbolic geometry makes this even easier!\n\n"
  "When you pick up another Orb, it will not disappear from the ground -- Orb of Choice will be drained instead.",
  
  "你知道可以把一枚法球敲成有限多块碎片，然后以得到的碎片拼成两枚一样的法球吗？"
  "双曲几何使得这样做更加简单了！\n\n"
  "在你捡起另一个法球时，它不会从地上消失——但抉择法球会失去一些充能。")
    
N("Free Fall", GEN_N, "自由落体","")

S("What on one side looks to be a normal (well, infinite) horizontal wall, on the other side turns out to be the vertical wall"
  " of an infinitely high tower. Jump from the window, and let the magical gravity carry you...",

  "一边看起来是普通的（好吧，无尽的）水平墙壁，另一边却其实是无限高的塔楼的垂直墙壁。从窗户一跃而下，让魔法重力引你前行……")

N("Falling Dog", GEN_N, "坠落犬","条")

S("Distant relatives of the Running Dogs.", "奔行犬的远亲。")

N("Western Hawk", GEN_N, "西域鹰","只")

N("Meteorite", GEN_N, "陨石","颗")

S("These rocks falling from the sky have been captured to fall forever in the artificial gravity. Meteorite iron is believed to be a valuable material for magical weapons.",
  "这些石头从天而降，被人造重力场永远地束缚在下落中。据说陨铁是能用于制作魔法武器的珍贵材料。")

Orb("Gravity", "重力")

S("This Orb lets you magically control gravity around you. In lands with unusual gravity, the options are: usual gravity (no change, except that it may disrupt birds), "
  "anti-gravity (causes the direction of gravity to be reversed), levitation (you can move in directions which are neither up nor down, "
  "or between cells if one of them has a wall in a 'neutral' direction). In lands with standard gravity, levitation lets creatures to avoid traps, chasms, "
  "and water, while anti-gravity makes it possible to move only when next to a wall (movement between cells without adjacent walls is not allowed). "
  "For each move, the gravity is based on what you do, and all enemies in the effective range are affected (for example, if you move next to a wall in a non-gravity land, "
  "anti-gravity kicks in and monsters will not be able to move in open space).",
  
  "这个法球让你能控制周身的重力。在重力异常的地域中，选项有：常规重力（没有变化，不过也许会干扰鸟类）、反重力（使得重力的方向相反）、悬浮（令你能向非上非下的方向移动，或是在相邻有处于“中立”方向的墙的格子之间移动）。在重力正常的地域中，悬浮使生物能避开陷阱、裂缝，还有水面，而反重力使人只有相邻墙壁时才能移动（在没有相邻墙壁的格子之间无法移动）。对于每次移动而言，重力取决于你做什么，效果范围内的所有敌人也会被影响（比如说，如果你在正常重力的地域移动到一堵墙旁边，反重力会生效，敌人无法移动到开阔空间）。")

N("Irradiated Field", GEN_N, "辐射场","")

S("These fields are ravaged with many kinds of magical radiation, which not only make the ground glow nicely in various colors, "
  "but also cause the inhabitants to protect the treasures of their land in various ways. In some areas of the Irradiated Fields, "
  "you will meet powerful enemies; in other areas, you will find arrow traps, fire traps, or pools, which can kill you if you are "
  "careless, or help you if you know how to use them to your advantage. Will you walk through the Irradiated Fields randomly, or "
  "try to find areas where treasures are common but nasty monsters are not, and keep to them? It is your choice!\n\n",
  
  "该地域有许多种魔法辐射肆虐，使得不仅地面发出五颜六色的好看的光，地域的原住民还能以各种方式保护他们的宝藏。在辐射场中的一些区域，你会碰到强力的敌人；在另一些区域，你会发现弓箭陷阱、火焰陷阱，或是池子，如果你不当心它们就会把你害死，但如果你了解如何利用它们就能帮到你。你会在辐射场里随机穿行，还是寻找财宝多多、怪物少少的区域，呆在里面？由你决定！\n\n")

N("Mutant", GEN_N, "变种人","名")

S("These guys look a bit strange, but they have no special properties.",
  "这些人看起来有点怪，但没有什么特殊能力。")
  
N("fire trap", GEN_N, "火焰陷阱","处")

S("This trap will explode when stepped on, setting all the adjacent cells on fire. However, this happens on the next turn, "
  "so you can safely escape if you are fast enough.",
  "这陷阱会在被踩到时爆炸，点燃所有相邻的格子。不过，这些到下回合才会发生，所以你可以迅速逃到安全地带。")

N("Explosive Barrel", GEN_N, "爆炸桶","个")
S("These barrels can be pushed, and they will explode if next to a fire, or under some attacks.",
  "这些桶子可以被推动，如果被攻击或靠近火边时会爆炸。")

Orb("Intensity", "强度")

S("When you have this, initial and maximal charge amounts of all Orbs are increased by 20%.",
  "在你拥有该法球时，所有法球的起始与最大充能增加20%。")

// new mode: Racing

S("Racing available only in unbounded worlds.", "竞速仅在无限世界中可用。")
S("select the track and start!", "选择赛道并开始！")
S("configure the projection", "设置投影")
S("racing projections", "竞速投影")
S("Poincaré disk model", "庞加莱碟模型")
S("third-person perspective", "第三人称视角")
S("point of view", "视角")
S("track", "赛道")
S("race angle", "竞速角度")
S("show more in front", "显示正前方更多内容")
S("guiding line", "引导线")
S("track seed", "赛道种子")
S("play the official seed", "游玩官方种子")
S("play a random seed", "游玩随机种子")
S("best scores to show as ghosts", "最高分显示为鬼魂")
S("best scores to save", "保存最高分")
S("disable the racing mode", "禁用竞速模式")
S("racing menu", "竞速菜单")
S("Race did not generate correctly for some reason -- not ranked",
  "竞速由于某些原因未能正常生成——不产生排名")

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
    
  "棕岛由远古的地下生物生成，它们随机移动，在路径上抬升土地……成体生物每一步都产生幼体，幼体也随机移动，抬升土地，最后变成成体开始产生自己的幼体。这些生物呈指数级增长，但它们生存的空间增长还要更快，所以足够它们繁衍……\n\n"
  "那些都是很久以前发生的事了。现在，多山的棕岛里居住着奇异的怪物。祝你冒险好运！\n\n"
  "棕岛有些地方高低起伏，有从0到3的高度。无法一次向上移动2或更多个高度，也无法一次向下移动3个（不过攻击是忽略高低差的）。"
  "击杀青铜兽让一格变高。"
  )

S(
    "Some readers misinterpreted the early maps of Free Fall, thinking that it is a land to the west from some wall. "
    "The name Western Hawks remained.",
    
    "有些读者理解错了过去自由落体的地图，觉得它是某一堵墙西边的什么地域。西域鹰这个名字得以保留至今。")

N("Torbernite", GEN_N, "铜铀云母","块")

S("Crystals emitting magical radiation.", "散发魔法辐射的晶体。")

// other things:

S("pure", "纯粹") // non-bitruncated

S(" (level %1)", " (层级 %1)")//unclear

S("fixed facing", "固定朝向")
S("configure TPP automatically", "自动设置TPP")

S("Finished the race! Time: %1, place: %2 out of %3", "完成竞速！耗时：%1，名次：%2/%3")
S("Finished the race in time %1!", "在%1内完成竞速！")

S("Welcome to HyperRogue! (cheat mode on)", "欢迎来到HyperRogue！（作弊模式已启动）")
S("Welcome to the Heptagonal Mode!", "欢迎来到七边形模式！")

S("adult Tortoise flags", "成年陆龟标记")
S("baby Tortoise flags", "小陆龟标记")

S("Map settings", "地图设置")

S("disable wandering monsters","禁用游荡怪物")
S("disable ghost timer", "禁用鬼魂计时器")
S("simple pattern generation", "简单图案生成")
S("(e.g. pure Reptile pattern)", "（即纯粹爬行图案）")
S("safety generation", "安全生成")
S("(no treasure, no dangers)", "（没有宝藏，没有危险）")
S("god mode", "上帝模式")

S("(unlock all, allow cheats, normal character display, cannot be turned off!)",
  "（解锁一切，允许作弊，正常角色显示，无法关闭！）")

S("change the pattern/color of new Canvas cells", "改变新画布格子的图案/颜色")

S("racing mode", "竞速模式")
S("Racing", "竞速")

S("octahedron", "八面体")

S("\"Did you know that the Cultists are relatives of the Desert Men?\"", 
  "“你知道邪教徒是沙民的远亲吗？”")                      

S("do not use special centering for racing", "竞速不使用特殊居中")

S("F8 = settings", "F8 = 设置")

// standarization of geometry names

S("{7,3} (standard HyperRogue map)", "{7,3} (标准HyperRogue地图)")
S("{6,3} (Euclidean grid)", "{6,3} (欧几里得网格)")
S("{5,3} (dodecahedron)", "{5,3} (十二面体)")
S("elliptic geometry in {5,3}", "{5,3}中的椭圆几何")
S("torus/Klein bottle/...", "环面/克莱因瓶/……")
S("{8,3} (like standard but with octagons)", "{8,3} (类似于标准，但使用八边形)")
S("{5,4} (four pentagons)", "{5,4} (四个五边形)")
S("{6,4} (four hexagons)", "{6,4} (四个六边形)")
S("{7,4} (four heptagons)", "{7,4} (四个七边形)")
S("{4,3} (cube)", "{4,3} (立方体)")
S("{3,3} (tetrahedron)", "{3,3} (四面体)")
S("{4,4} (Euclidean square grid)", "{4,4} (欧几里得方形网格)")
S("elliptic geometry in {4,3}", "{4,3}中的椭圆几何")
S("{3,4} (octahedron)", "{3,4} (八面体)")

// new 3D geometries
                     
S("variant of the binary tiling", "二元镶嵌变体")
S("{4,3,4} cube tiling", "{4,3,4}立方镶嵌")
S("rhombic dodecahedral honeycomb", "菱形十二面体蜂窝")
S("bitruncated cubic honeycomb", "截角立方蜂窝")

S("{4,4} on horospheres", "极限球中的{4,4}")
S("{3,6} on horospheres", "极限球中的{3,6}")
S("{6,3} on horospheres", "极限球中的{6,3}")
S("rectangles on horospheres", "极限球中的矩形")

// quotient space abbreviations
S("minimal", "最小")
S("Klein", "克莱因")
S("Bolza", "Bolza")
S("Bolza2", "Bolza2")
S("Macbeath", "Macbeath")
S("Bring", "Bring")
S("M3", "M3")
S("M4", "M4")
S("Crystal", "晶体")

#define Cell(x) \
  S(x "-cell", x "-格子") \
  S(x "-cell (elliptic space)", x "-格子 (椭圆空间)")

Cell("{3,3,3} 5") Cell("{4,3,3} 8") Cell("{3,3,4} 16") Cell("{3,4,3} 24") Cell("{5,3,3} 120") Cell("{3,3,5} 600")
#undef Cell

#define Honeycomb(x) \
  S(x " hyperbolic honeycomb", x " 双曲蜂窝") \
  S(x " field quotient space", x " 场商空间")

Honeycomb("{5,3,4}") Honeycomb("{4,3,5}") Honeycomb("{3,3,6}") Honeycomb("{3,4,4}") Honeycomb("{5,3,5}") Honeycomb("{5,3,6}") Honeycomb("{4,3,6}") Honeycomb("{5,3,6}")
Honeycomb("{3,4,5}") Honeycomb("{3,5,3}") Honeycomb("{3,5,4}") Honeycomb("{3,5,5}")

#undef Honeycomb

// new or previously untranslated options
S("dimension", "维度")
S("pattern", "图案")
S("Euler characteristics", "欧拉特性")
S("demigenus", "不定向亏格")
S("number of mines", "地雷数量") // for Minefield in bounded spaces
S("advanced parameters", "高级参数")

// models & projections

S("disk/Gans", "碟/Gans模型")
S("spiral/ring", "螺旋/环")
S("native perspective", "原生视角")
S("azimuthal equi-volume", "方位角等体积")
S("ball model/Gans", "球/Gans模型")
S("perspective in 4D", "4D视角")
S("half-space", "半空间")

// projection configuration

S("projection distance", "投影距离")
S("rotation (x/z plane)", "旋转（x/z平面）")
S("model orientation (y/z plane)", "模型取向（y/z平面）")

S("near clipping plane", "近端裁切平面")
S("far clipping plane", "远端裁切平面")
S(
  "Your view of the 3D model is naturally bounded from four directions by your window. "
  "Here, you can also set up similar bounds in the Z direction. Radius of the ball/band "
  "models, and the distance from the center to the plane in the halfspace model, are 1.\n\n",
  
  "你对3D模型的视角自然被游戏显示窗口的四边约束。在此，你可以在Z方向也设置类似的约束。球/带模型的半径，以及半平面模型中中心到平面的距离均为1。\n\n")

S("Objects with Z coordinate "
  "bigger than this parameter are not shown. This is useful with the models which "
  "extend infinitely in the Z direction, or if you want things close to your character "
  "to be not obscured by things closer to the camera.",
  
  "Z坐标大于该值的物体不会被显示。该项对于在Z方向无限延伸的模型中进行游戏，或是你不想让贴近相机的物体阻挡贴近玩家角色的物体时比较有用。")

S("Objects with Z coordinate "
  "smaller than this parameter are not shown; it also affects the fog effect"
  " (near clipping plane = 0% fog, far clipping plane = 100% fog).",
  
  "Z坐标小于该值的物体不会被显示；也会影响雾效果（近端裁切平面 = 0% 雾，远端裁切平面 = 100% 雾）")

S("set to 90° for the ring projection", "环投影设置为90°")

S("spiral multiplier", "螺旋倍数")
S(
  "This parameter has a bit different scale depending on the settings:\n"
  "(1) in spherical geometry (with spiral angle=90°, 1 produces a stereographic projection)\n"
  "(2) in hyperbolic geometry, with spiral angle being +90° or -90°\n"
  "(3) in hyperbolic geometry, with other spiral angles (1 makes the bands fit exactly)",
  
  "该参数取决于设置有不同的倍数：\n"
  "(1) 球面几何（螺旋角=90°，1产生球极投影）\n"
  "(2) 双曲几何，螺旋角为+90°或-90°\n"
  "(3) 双曲几何，其它螺旋角（1使带恰好填满）\n"
  )

// line animation

S("animate from start to current player position", "自起点至玩家当前位置动态化")
S("animate from last recenter to current view", "自上一次重设的中心至当前视野动态化")

// 3D settings for 3D mode

S("3D sight range", "3D视野范围")
S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "在3D几何中的视野范围以绝对单位衡量。该值也影响雾效果。\n\n"
  "在球面几何中，视野范围为2π使你能够见到背后的物体，就如同在面前一般；π（或更多）使你能够看到对跖点，就好像在你身旁。\n\n"
  "在双曲几何中，渲染的格子数目呈视野范围的指数级。绘制太多格子会影响性能。\n\n"
  "视野范围影响游戏内容，可见的怪物会行动。怪物生成也会考虑视野范围。")

S("limit generation", "限制生成")
S("Cells over this distance will not be generated, but they will be drawn if they are already generated and in the sight range.",
  "超过该距离的格子不会生成，但如果已经被生成过且在视野范围内会渲染。")

S("inverted Poincaré model", "反转庞加莱模型")
S("mouse aiming sensitivity", "鼠标指向灵敏度")
S("set to 0 to disable", "设为0来禁用")
S("radar size", "雷达尺寸")
S("Height to width", "高宽比")
S("Creature scale", "生物尺寸")

// not only in 3D mode
S("desaturate colors", "颜色去饱和")
S("Make the game colors less saturated. This is useful in the anaglyph mode.",
  "使得游戏颜色不那么饱和。在浮雕模式中比较有用。")

S("brightness behind the sphere", "球背亮度")
S("In the orthogonal projection, objects on the other side of the sphere are drawn darker.",
  "在正交投影中，球面另一侧的物体绘制偏暗。")

// restructured menus

S("colors & aura", "颜色与光环")
S("colors of some game objects can be edited by clicking them.", "点击一些游戏物体可以修改它们的颜色。")
S("standard grid color", "标准网格颜色")
S("mouse & touchscreen", "鼠标与触摸")
S("settings", "设置")
S("interface", "界面")
S("general graphics", "基本图形")
S("quick options", "快速设置")
S("models & projections", "模型与投影")
S("keyboard & joysticks", "键盘与摇杆")
S("other settings", "其他设置")
S("special modes", "特殊模式")
S("creative mode", "创意模式")
S("multiplayer", "多人游戏")

// extra / previously untranslated options

S("smooth scrolling", "平滑滚动")
S("change slideshow", "切换页面")
S("move by clicking on compass", "点击指南针来移动")
S("disable the HUD", "禁用HUD")
S("hide the player", "隐藏玩家")

// redefinable key in 3D shmup mode
S("scroll forward", "向前滚动")
S("scroll backward", "向后滚动")

S("note: configured input is designed for", "注意：设置输入为")
S("multiplayer and shmup mode; some features", "多人游戏与射击模式设计；有些功能")
S("work worse if you use it.", "在你使用时效果较差。")

// quick options in 3D
S("first person perspective", "第一人称视角")
S("fixed point of view", "固定视角")
S("third person perspective", "第三人称视角")

S("Hint: these keys usually work during the game", "提示：这些按键一般在游戏中能够生效")

// 3D graphics editor

S("l = color group: %1", "l = 颜色组：%1")
S("z = z-level", "z = Z高度")
S("Changing the z-level will make this layer affected by the parallax effect.", "改变Z高度会使该层受视差影响。")
S("The distance from the camera to added points.", "相机与新增加点间的距离。")

S("This land has been disabled with compilation flags.", "该地域被编译标记禁用。")
S("This land does not make much sense in 3D.", "该地域在3D中没什么意义。")
S("This land does not make much sense in binary tiling.", "该地域在二元镶嵌中没什么意义。")
S("Special pattern implemented for this geometry.", "该几何使用特殊图案。")
S("This land works only in the shmup mode.", "该地域仅在射击模式中起效。")

// new land

N("Space Rocks", GEN_N, "太空岩区","")
N("Space Rock", GEN_N, "太空岩石","块")
N("Fuel", GEN_N, "燃料","份")
S("Shoot the Space Rocks for score. Large Rocks will split into two smaller rocks.",
  "射击太空岩石得分。大石头会碎成两块小石头。")

N("editable statue", GEN_N, "可编辑雕像","座")
S("These statues are designed to have their graphics edited in the Vector Graphics Editor. Each number has its own, separate graphics.",
  "这些雕像是用来在向量图形编辑器中更改图形的。每个数字对应一种图形。")
S("The Orb of Yendor is locked in with powerful magic.", "Yendor法球被强力的魔法锁住了。")

S("general perspective", "一般视角")
S("formula", "公式")

S("limit generated cells per frame", "限制每帧生成格子")

S("In the 3D mode, lowering this value may help if the game lags while exploring new areas.",
  "在3D模式中，降低该值也许能提高探索新区域时的游戏流畅度。")
  
S("sloppy range checking", "滑坡范围检测")

S("fog effect", "雾效果")
S("memory configuration", "内存设置")

S("wall quality", "墙壁质量")

S(
  "Controls the number of triangles used for wall surfaces. "
  "Higher numbers reduce the performance. "
  "This has a strong effect when the walls are curved indeed "
  "(floors in 2D geometries, honeycombs based on horospheres, and projections other than native perspective), "
  "but otherwise, usually it can be set to 1 without significant adverse effects other "
  "than slightly incorrect texturing.",
  
  "控制墙面使用的三角形数。较高的数值影响性能。在墙壁为曲线时影响较大（2D几何中的地板、基于极限球的蜂窝，以及原生投影以外的投影），"
  "但在其他情况下，设为1也不会有偶尔出现不正确纹理以外的严重负面影响。")

S("use the full 3D models", "使用全3D模型")

S("Z shift", "Z平移")
S("fixed Y/Z rotation", "固定Y/Z旋转")

S("configure FPP automatically", "自动设置FPP")

S("Look from behind.", "在身后观察。")

S("reduce if walls on the way", "被墙壁阻挡则降低")

S("The camera is currently exactly on the plane P. "
  "The horizon is seen as a straight line.",
  
  "相机目前恰好位于平面P。地平线为直线。")

S(
  "The camera is currently %1 units above the plane P. "
  "This makes you see the floor level as in general perspective projection "
  "with parameter %2.", 
  
  "相机目前在P平面以上%1个单位。这使你观察地面高度的效果如同在参数%2的一般视角中。")

S("In the FPP mode, the camera will be set at this altitude (before applying shifts).", 
  "在FPP模式中，相机会被设置在该高度（平移之前）。")

S("auto-adjust to eyes on the player model", "自动调整至玩家模型的眼部")

S("eye color", "眼睛颜色")

// rotation (line animation etc.)

S("XY plane", "XY平面")
S("XZ plane", "XZ平面")
S("XY plane #2", "XY平面 #2")

// cube honeycomb

S("twisting implemented only for diagonal matrices", "仅为对角矩阵实现扭转")
S("nothing to twist", "没有扭转对象")
S("change the bottom left corner", "更改左下角")

S("flip X coordinate", "翻转X坐标")
S("flipping X impossible", "翻转Y坐标不可能")
S("flip Y coordinate", "翻转Y坐标")
S("flipping Y impossible", "翻转Y坐标不可能")
S("flip X coordinate", "翻转X坐标")
S("swap X and Y", "交换X与Y")
S("swapping impossible", "交换不可能")

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

  "该矩阵是你在三维欧几里得空间的商空间中进行游戏。每列为一个将你带回原点的平移向量。例如，如果你将2、6、0置于对角，你在X方向移动2步、Y方向移动6步后回到原点（商空间在Z方向是无穷的）。\n"
  "你也可以在对角矩阵中引入扭转：在Z方向移动给定步数后，空间会被镜像或旋转。（更一般化的“扭转”空间暂未实现。）")

// map editor 

S("pick something", "选择目标") // to edit

S("z = camera", "z = 相机")
S("Z = spheres", "z = 球面")
S("z = equi", "z = 赤道")

S("The distance from the camera to added points.", "相机与新增加点间的距离。")
S("place points at fixed radius", "在固定半径上放置点")
S("place points on equidistant surfaces", "在等距面上放置点")
S("mousewheel step", "鼠标滚轮步进")
S("hint: shift for finer steps", "提示：shift精细调整")

S("random walk", "随机行走")

// dual like two at once, not in mathematical sense
S("dual geometry mode", "双几何模式")

S("Impossible.", "不可能。")

S("subgame affected", "影响子游戏")
S("both", "均起效")

// changing the rug model distance
S("adjust points", "调整点")
S("center on camera", "相机居中")
S("adjust edges", "调整边")
S("adjust distance", "调整距离")

S("memory", "内存")

S(
  "HyperRogue takes place in a world that is larger than anything Euclidean. "
  "Unfortunately, in some cases running it on an Euclidean computer might be "
  "a problem -- the computer could simply run out of memory. Some lands (such as the Ocean or the Brown Islands) "
  "may use up memory very fast!\n\n",
  
  "HyperRogue发生在比任何欧几里得物体都大的世界里。不幸的是，它在欧几里得计算机上运行可能会有些困难——电脑会用光内存。有些地域（比如海洋或棕岛）可能会非常迅速地消耗内存！\n\n")
  
S("You are playing a 32-bit HyperRogue executable, which can only use 4GB of memory.\n\n",
  "你在游玩32位的HyperRogue，只能使用4GB内存。\n\n")

S(
  "Although you are extremely unlikely to return to a place you have already been to, "
  "the game never forgets these areas, unless you start a new game, use an Orb of "
  "Safety (found in Land of Eternal Motion, the Prairie, and the Ocean), or activate the memory "
  "saving mode, which tries to intelligently predict which cells you will never find "
  "again and can be safely forgotten.\n\n",
  
  "尽管你回到某一遥远位置的概率极低，游戏也永远不会忘记这些区域，除非你开始新游戏、使用安全法球（可以在永动之地、海洋和大草原找到），或是启动了会自动预测你不可能返回的格子并将其删除的内存节省模式。")

S("cells in memory", "内存中的格子数")

S("memory saving mode", "内存节省模式")
S("show memory warnings", "显示内存警告")

S("just let me find Orb of Safety or finish the game", "让我找到安全法球或结束游戏")
S("memory reserve", "内存保留量")
S("When to show a memory warning.", "剩余内存为此数值时显示内存警告。")
S("clear caches", "清理缓存")

// in animation menu
S("shmup action", "射击行动")

// 11.2

// extra projections
S("central inversion", "中心反转")
S("two-point azimuthal", "两点方位角")
S("two-point hybrid", "两点混合")
S("Mollweide", "Mollweide投影")
S("central cylindrical", "中心圆柱")
S("Collignon", "Collignon投影")

// extra settings

S("draw crosshair", "绘制准星")

S("Display a targetting reticle in the center of the screen. Might be useful when exploring 3D modes, "
  "as it precisely shows the direction we are going. However, the option is available in all modes.",
  
  "在屏幕中心显示瞄准标识。也许在3D模式探索中有用，因为准星明确给出了前进的方向。不过，该选项在所有模式都可用。")

S("radar range", "雷达范围")

S("boundary width multiplier", "边界宽度倍数")
S("grid width multiplier", "网格宽度倍数")
S("highlight the cell forward", "高亮前方格子")
S("edit widths individually", "单独编辑宽度")

// extra tilings

S("kite-and-dart", "彭罗斯镶嵌")
S("kite-and-dart on horospheres", "极限球上的彭罗斯镶嵌")
S("standard binary tiling", "标准二元镶嵌")

S("show quotient spaces", "显示商空间")
S("three-dimensional", "三维")

// non-isotropic geometries:

S("Solv geometry", "Solv几何")
S("Sol", "Sol")

S(
  "Solv (aka Sol) is a 3D space where directions work in different ways. It is described by the following metric:\n"
  "ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  "You are currently displaying Solv in the perspective projection based on native geodesics. You can control how "
  "the fog effects depends on the geodesic distance, and how far object in X/Y/Z coordinates are rendered.",
  
  "Solv几何（即Sol）是一种方向的运作方式不同的3D空间。它由以下参数定义：ds² = (eᶻdx)² + (e⁻ᶻdy)² + dz²\n\n"
  
  "目前正在以基于原生测地线的视角投影显示Solv几何。你可以控制雾效果对测地线距离的依赖，以及X/Y/Z方向多远处的物体会被渲染。")

S("max difference in X/Y coordinates", "X/Y方向的最大距离")
S("max difference in Z coordinate", "Z方向的最大距离")

S("geodesic movement in Sol/Nil", "Sol/Nil中的测地线运动")

// ---

S("Nil geometry", "Nil几何")
S("Nil", "Nil")

// ---

S("product space", "积空间")
S("current geometry x E", "目前的几何 x E")
S("Only works with 2D non-Euclidean geometries", "仅在2D非欧几何中生效")
S("%1 x E", "%1 x E")
S("Z-level height factor", "Z层级系数")

S("precision of S2xE rings", "S2xE环精度")

S(
  "In S2xE, objects at spherical distances which are multiples of π will look like "
  "rings, and objects close to these will look like crescents. "
  "This setting controls the quality of rendering these rings and crescents.",
  
  "在S2xE中，物体在为π整倍数的球距离时会看起来像环，接近的物体则会看起来像月牙。该设置控制渲染这些环与月牙的质量。")

// ---

S("rotation space", "旋转空间")
S("steps", "步数")

// 11.3
//======

// new geometries

S("space of rotations in current geometry", "当前几何的旋转空间")
S("~SL(2,R)~", "~SL(2,R)~")
S("Only works with (semi-)regular tilings", "仅作用于（半）规则镶嵌")
S("That would have %1/%2 levels", "这会有%1/%2层")
S("number of levels", "层数")
S("Hint: this is more playable with pure {7,3} or pure {5,4}", "提示：在纯{7,3}或纯{5,4}中更好玩")

S("view the underlying geometry", "检视其下几何")

S("The space you are currently in is the space of rotations of the underlying hyperbolic or spherical geometry. ",
  "你目前所在的空间是双曲或球面几何的旋转空间。")

S("This option lets you see the underlying space. Lands and some walls (e.g. in the Graveyard) are based on "
  "the respective features in the underlying world, but details such as monsters or items are ignored.",
  
  "该选项使你检视其下的空间。地板与一些墙壁（比如在墓地中的）基于其下世界的特征，但怪物或物品等细节被忽略。")

// map editor in 3D nonisotropic

S("place points on surfaces of const Z", "在常数Z曲面上放置点")
S("place points on surfaces of const X", "在常数X曲面上放置点")
S("place points on surfaces of const Y", "在常数Y曲面上放置点")

S("parallels to draw", "要绘制的平行线")
S("meridians to draw", "要绘制的子午线")
S("range of grid to draw", "要绘制的格子范围")

S("current filter", "当前滤镜")
S("Solv quotient spaces", "Solv商空间")
S("%1 period", "%1周期")
S("Note: the value 0 functions effectively as the size of int (2^32).", "注意：数值0的效果等同于int大小(2^32)。")

S("standard ternary tiling", "标准三元镶嵌")
S("stretched hyperbolic", "拉伸双曲")
S("stretched Solv", "拉伸Solv")

S("{3,∞} (infinite triangles)", "{3,∞} (无穷三角形)")
S("{4,∞} (infinite squares)", "{4,∞} (无穷方形)")
S("{3/4,∞} (infinite triangles and squares)", "无穷三角形与方形")

S("4D crystal in H3", "H3中的4D水晶")
S("6D crystal in H3", "H3中的6D水晶")

S("Arnold's cat mapping torus", "Arnold的猫映射环面")
S("{5,3,5} quotient", "{5,3,5}商")
S("Seifert-Weber space", "Seifert-Weber空间")
S("Poincaré homology sphere", "庞加莱同源球")

S("load from file", "从文件读取")

S("Crystal torus", "水晶环面")
S("Z_k^d instead of Z^d. Only works with k even.", "Z_k^d而非Z^d。仅在k为偶数时有效。")

S("Euclidean quotient spaces", "欧几里得商空间")
S("columns specify periods", "列指定周期")
S("(vectors you need to take to get back to start)", "(回到原点所需的向量)")
S("third-turn space", "przestrzeń obrotu o 1/3")
S("make it even", "liczby muszą być parzyste")
S("or for columns : (A,B,C), (B,C,A), (D,D,D) where A+B+C=0", "或列：(A,B,C), (B,C,A), (D,D,D) 其中 A+B+C=0")
S("quarter-turn space", "przestrzeń ćwierćobrotu")

S("change the second column for Möbius bands and Klein bottles", "改变莫比乌斯环与克莱因瓶的第二列")
S("second period is chiral -- cannot be mirrored", "第二周期为手性——无法镜像翻转")
S("periods must be orthogonal for mirroring", "镜像翻转的周期必须为正交")
S("mirror flip in the second period", "在第二周期镜像翻转")
S("single-cell torus", "单格环面")
S("Klein bottle", "克莱因瓶")
S("large regular torus", "大规则环面")
S("cylinder", "圆柱体")
S("Möbius band", "莫比乌斯环")
S("seven-colorable torus", "七色环面")
S("HyperRogue classical torus", "HyperRogue经典环面")
S("no quotient", "无商")
S("standard rotation", "标准旋转")

S("field quotient", "场商")
S("order %1%2 (%3 %4s)", "阶 %1%2 (%3 %4ów)")
S("order %1%2 (%3 %4-gons)", "阶 %1%2 (%3 %4边形)")

S("non-isotropic", "各向异性")
S("regular 2D tesselations", "规则2D镶嵌")
S("regular 3D honeycombs", "规则3D蜂窝")
S("interesting quotient spaces", "有趣的商空间")
S("geometries", "几何")
S("geometry", "几何")

S("start discovery", "开始探索")
S("resume discovery", "恢复探索")
S("suspend discovery", "暂缓探索")

S("#%1, cells: %2", "#%1，格子数：%2")

S("hyperbolic (3:2)", "双曲 (3:2)")
S("Sol (3:2)", "Sol (3:2)")

S("quotient spaces in ", "商空间：")
S("No quotient spaces available in the current tiling.",
  "当前镶嵌无可用商空间。")

S("You are currently in a product space.", "你目前位于积空间中。")

S("Nil quotient spaces", "Nil商空间")
S("honeycomb", "蜂窝")
S("Y period must be divisible by Z period", "Y周期必须能被Z周期整除")
S("quotient product spaces", "商积空间")
S("Set to 0 to make it non-periodic.", "0为非周期。")
S("Works if the underlying space is symmetric.", "其下空间为对称时正常运转。")
S("reflect", "反射")
S("the current rotation is invalid", "当前旋转不可用。")

S("Note: adjacency rule affects environmental effects, but not movement.",
  "注意：相邻规则影响环境效果，但不影响移动。")

S("\n\nOption -mineadj 1 can be added for Moore neighborhoods.", "\n\n添加选项“-mineadj 1”来启用Moore相邻。")

// WFC generation

S("configure WFC", "设置WFC")

S("Wave Function Collapse", "波函数坍缩")
S("import the current map", "导入当前地图")
S("generate a map using WFC", "使用WFC生成地图")

// racing in Thurston geometries

S("Racing", "竞速")
S("how fast can you reach the end?", "你抵达终点需要多久？")
S("Racing in Thurston geometries", "在Thurston几何中竞速")
S("race through a maze in exotic 3D geometry!", "在奇异的3D几何迷宫中竞速！")

S("periodic Solv", "周期Solv几何")
S("hyperbolic crystal", "双曲晶体")
S("hyperbolic regular", "双曲规则")
S("S2xE regular", "S2xE规则")
S("H2xE regular", "H2xE规则")
S("periodic Nil", "周期Nil")
S("alternate versions", "替代版本")

// extra option

S("floors in 2D geometries", "2D几何地板")

S("slides", "页面")

// raycasting

S("configure raycasting", "设置光线投射")
S("raycasting configuration", "光线投射设置")
S("available in current geometry", "在当前几何中可用")
S("use raycasting?", "使用光线投射？")
S("comparison mode", "对比模式")
S("exponential range", "指数级范围")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))", "亮度公式：max(1-d/视野范围, s*exp(-d/r))")
S("exponential start", "指数级启动")
S("hard limit", "硬性限制")
S("no limit", "无限制")
S("reflective walls", "墙壁反射")
S("max step", "最大步数")
S("affects the precision of solving the geodesic equation in Solv", "影响Solv中测地线方程求解的精度")
S("min step", "最小步数")
S("how precisely should we find out when do cross the cell boundary", "跨过格子边界的精度")
S("iterations", "迭代")
S("in H3/H2xE/E3 this is the number of cell boundaries; in nonisotropic, the number of simulation steps",
  "在H3/H2xE/E3中这是格子的边界数；在各向异性中这是模拟的步数")
S("max cells", "最大格子数")
S("generate", "生成")
S("out-of-range color", "超出范围的颜色")

// new graphical styles

S("standard graphics", "基础图形")
S("neon mode", "霓虹模式")
S("no boundary mode", "无边界模式")
S("neon mode II", "霓虹模式 II")
S("illustration mode", "绘画模式")
S("hint: Press Alt while testing modes", "提示：测试模式时按Alt")
S("disable shadows", "禁用阴影")
S("bright mode", "明亮模式")
S("colorblind simulation", "色盲模拟")
S("ASCII/3D", "ASCII/3D")

// new configuration options

S("particles on attack", "攻击粒子效果")
S("floating bubbles: special", "漂浮泡泡：特殊")
S("floating bubbles: treasure  thresholds", "漂浮泡泡：宝藏门槛")
S("floating bubbles: all treasures", "漂浮泡泡：所有宝藏")

S("variable width", "可变宽度")
S("lines longer than this value will be split into shorter lines, with width computed separately for each of them.",
  "长度大于该值的线会被分成较短的线，单独计算每条的宽度。")

S("smoothen", "平滑")
S("large values help if the joystick is imprecise", "如果摇杆不准确，较大的值会有帮助")

S("level lines", "层级线")
S("This feature superimposes level lines on the rendered screen. These lines depend on the Z coordinate. In 3D hyperbolic the Z coordinate is taken from the Klein model. "
  "Level lines can be used to observe the curvature: circles correspond to positive curvature, while hyperbolas correspond to negative. See e.g. the Hypersian Rug mode.",
  
  "该选项将渲染屏幕上的层级线叠加。这些线取决于Z坐标。在3D双曲中Z坐标取自克莱因模型。"
  "层级线可用于观察曲率：圆对应正曲率，双曲线对应负曲率。请查看超波斯地毯模式。")

S("disable textures", "禁用纹理")
S("disable level lines", "禁用层级线")

S("Size of the fish eye.", "鱼眼的尺寸。")
S("product Z stretch", "积Z拉伸")
S("horocyclic coordinates", "极限圆坐标")

// dual geometry puzzles

S("You won!", "你获胜了！")
S("dual geometry puzzles", "双几何谜题")
S("move both characters to marked squares at once!", "同时将两位角色移动至标记处！")
S("enter seed", "输入种子")
S("Euclidean size", "欧几里得尺寸")
S("hyperbolic size", "双曲尺寸")

// another new mode

S("play this Strange Challenge unofficially", "游玩该非官方的奇异挑战")

// new gameplay lines

S("\n\nIn the Orb Strategy Mode, the effect is increased to +100%.",
  "\n\n在法球策略模式中，效果增加至+100%。")

S("\n\nAdditionally, your allies are protected from your indirect attacks.", 
  "\n\n此外，你的盟友具有对你非直接攻击的防护。")

S("\n\nLeaves cut directly: %1", "\n\n直接剪切叶片：%1")
S("\n\nLeaves cut onscreen: %1", "\n\n屏内剪切叶片：%1")
S("\n\nLeaves cut offscreen (approximately): %1", "\n\n屏外剪切叶片（估计）：%1")
S("leaves cut: %1", "叶片剪切：%1")

// explanations for the start screen

S("(random option)", "(随机选项)")
S("(seasonal option)", "(专业选项)")

S("spinning in the band model", "带模型中旋转")//unclear
S("spinning around", "到处旋转")
S("row of ghosts", "鬼魂行列")
S("army of ghosts", "鬼魂大军")
S("ghost spiral", "鬼魂螺旋")
S("Fibonacci ghosts", "斐波那契鬼魂")

// new welcome messages

S("Welcome to Spherindrogue!", "欢迎来到Spherindrogue！")
S("Welcome to Hyper-X-R-Rogue!", "欢迎来到Hyper-X-R-Rogue！")
S("Good luck in the elliptic space!", "祝你在椭圆空间中好运！")
S("Welcome to SolvRogue!", "欢迎来到SolvRogue！")
S("Welcome to NilRogue!", "欢迎来到NilRogue！")
S("Welcome to PSL(2,R)-ogue!", "欢迎来到PSL(2,R)-ogue！")

// new patterns and line patterns

S("empty", "空")
S("rainbow by distance", "彩虹，根据距离")
S("Cage", "笼子")
S("Hyperplanes", "超平面")
S("Honeycomb", "蜂窝")
S("Diagonal", "对角")
S("Penrose staircase", "彭罗斯阶梯")
S("nearer end", "近端")
S("furthest from start", "距起点最远")
S("types", "类别")
S("display the inner walls", "显示内墙")
S("floor type", "地板类型")
S("(no wall)", "(无墙壁)")
S("Goldberg tree", "Goldberg树")
S("irregular master", "不规则主")
S("sub-lines", "子线")
S("fat edges", "宽边缘")
S("You chop down the shrub.", "你砍倒了灌丛。")

// new lands

N("Wetland", GEN_N, "湿地","")

S("Some people have definitely drowned in this treacherous area. Better be careful!",
  "肯定有人在这凶险的区域里淹死过。最好当心！")

N("Water Lily", GEN_N, "睡莲","株")
S("A beautiful plant from the Wetland.", "一种湿地的美丽植物。")

N("shallow water", GEN_N, "浅水","片")
S("Shallow water is passable both for land creatures and for water creatures.",
  "不管是陆地生物还是水生生物都可以通过浅水。")

N("deep water", GEN_N, "深水","片")

S("This body of water is calm and beautiful, but too deep to pass.",
  "这片水域宁静而美丽，但太深了无法涉过。")

S("You remember anglers from your hometown showing the impressive pikes they have caught. This one is much larger.",
  "你回忆起家乡的渔夫炫耀自己钓起来的大梭鱼。这一条可比那大多了。")

S("World Turtle", "世界龟")
S("Have you reached the edge of the Poincaré disk? No, it must just be the edge of this world. "
  "Which happens to be floating on the backs of some turtles. Wondering how such not very big turtles could "
  "support such a large world? Well, there are LOTS of them! "
  "This one got so annoyed by the fact that you discovered this that it left its post. However, the world still "
  "stands, because there are so many turtles.",
  
  "你有抵达过庞加莱碟的边缘吗？不，只不过是这个世界的边缘罢了。这个世界漂浮在一些乌龟的背上。在想为什么这么点大的乌龟能撑起世界吗？哎呀，是因为有很多这样的乌龟！这一只因为你发现它擅离职守而非常恼怒。不过世界仍然屹立，因为还有那么多乌龟。")

// gender-switched
N("Rusałka", GEN_N, "水仙女","名")
S("A malicious water being. When you kill her, she changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "恶毒的水生生物。你击杀她时，她会改变你站立的格子，将陆地变为浅水，浅水变为深水。")
  
N("Topielec", GEN_N, "溺水妖","名")
S("A malicious water being. When you kill him, he changes the tile you are standing on, from land to shallow water, or from shallow water to deep water.",
  "恶毒的水生生物。你击杀他时，他会改变你站立的格子，将陆地变为浅水，浅水变为深水。")

// przet

Orb("Plague", "瘟疫")
S(
  "When you kill an enemy, adjacent enemies are also attacked, as long as they are further away from you than from the originally attacked enemy. "
  "These enemies may in turn infect further enemies, up to distance of at most 4 from you.",
  "在你击杀敌人时，它相邻的敌人也会被你攻击，只要它们距离你比距离原来被攻击的敌人更近。这些敌人还会感染更远处的敌人，至多到距离你4格。")

//---

N("Frog Park", GEN_N, "青蛙公园","")

S("This park is a perfect place for frogs to hide from predators. "
  "So perfect that the frogs have grown in size, gained magical abilities, "
  "and become very dangerous themselves.",
  "这座公园是青蛙躲避捕食者的完美地点，完美到这些青蛙尺寸变大，获得了魔力，自己都成为了危险的生物。"
  )

N("Gold Ball", GEN_N, "金球","枚")
S("A toy lost in the Frog Park by a spoiled child.", "被惯坏的孩子留在青蛙公园的玩具。")

Orb("Impact", "冲击")
S("Whenever you use a ranged Orb to achieve an effect, all the single-tile monsters adjacent to the target "
  "location are stunned.",
  "在你使用远程法球完成某种效果时，所有相邻于目标位置的单格怪物均被击晕。")

N("shrub", GEN_N, "灌丛","片")
S("A strange small tree that cannot be attacked with mundane weapons nor phased though. "
  "It can be vaulted over, though.",
  "一颗奇怪的小树，既不能以寻常武器攻击也不能用魔法穿行。不过你可以翻越过去。")

N("Giant Frog", GEN_N, "巨蛙","只")

S(
  "At first, you are shocked by the agility of this frog. Such a large creature "
  "should not be able to jump that quickly!\n\n"
  "Then, you notice the green glow around its legs. This frog must be magical... "
  "it has sacrificed its swimming abilities for superfrog jumping powers.",
  
  "你乍一见到青蛙时被它的敏捷震惊。这么大的东西怎么能跳那么快！\n\n"
  "随后你就注意到它腿部的绿光。这只青蛙必定身怀魔法……它牺牲了自己的游泳能力，换取超蛙级的跳跃。")

N("Yellow Frog", GEN_N, "黄蛙","只")

S("A slightly transparent yellow frog. It has mastered the magical power of phasing through solid "
  "obstacles such as rock.",
  "有点透明的黄色青蛙。它掌握了穿透岩石等固体障碍物的魔力。")

N("Blue Frog", GEN_N, "蓝蛙","只")

S(
  "This frog is able to vault over the shrubs in the Frog Park, destroying the shrub in the process. "
  "While it can also vault over monsters, it will never hurt the other frogs!",

  "这只青蛙可以越过青蛙公园的灌丛，在这一过程中将其摧毁。虽然它也能翻越过其他怪物，但它绝不会伤害其他青蛙！")

S("\n\nThis Frog uses the power of %the1. You get 5 charges yourself for killing it.", "\n\n这只青蛙使用%1的力量。你在击杀它时也会获得5点对应充能。")

//---

N("Eclectic City", GEN_N, "能源之城","")

S("Many kinds of creatures are living here, each with its own architectural style.",
  "各种各样的生物生存在这里，每种都有自己的构造。")

N("Lazurite Figurine", GEN_N, "青金石雕像","座")

S("A beautiful blue figurine. Every figurine is different.",
  "一座美丽的青金石小雕像。每个小雕像的外观都各不相同。")
  
Orb("Chaos", "混沌")
S(
  "When you move, the contents of the two cells adjacent to your both locations are swapped. "
  "If they contain creatures, these creatures will be stunned for some time, before they understand what happened.\n\n"
  "Does not work if one of the cells contains a "
  "multi-tile creature or a barrier-like wall.",
  
  "你移动时，交换同时相邻你移动前后位置的两个格子中的内容。如果其中有生物，这些生物会在弄清楚到底发生了什么之前被击晕一段时间。\n\n"
  "如果其中一格有多格生物或类似障碍的墙时不能交换。")

S("digon", "二边形")
S("triangle", "三角形")
S("square", "方形")
S("pentagon", "五边形")
S("hexagon", "六边形")
S("heptagon", "七边形")
S("octagon", "八边形")

// 12.0
//======

// missing Android buttons

S("WAIT", "等待")
S("RUG", "地毯")
S("CAM", "相机")

// missing UI texts

S("error: ", "错误：")
S("rules: %1", "规则：%1") // WFC

// missing game texts

S("You polymorph %the1 into %the2!", "你将%1变成%2！")
S("All players are in the game!", "所有玩家都在游戏中！")
S("\"Are there Temples of Cthulhu in your world? Why not?\"", 
  "“你的世界里有克苏鲁的神殿吗？为什么没有？”")
S("saves", "存档")
S("players", "玩家")
S("You swing your sword at %the1.", "你对%1挥剑。")
S("You swing your sword.", "你挥剑。")
S("There is no room for %the1!", "没有给%1的空间！")
S("Don't play with slime and electricity next time, okay?", "下次不要再玩黏液和电了，好吗？")
S("The Orb of Safety from the Land of Eternal Motion might save you.", 
  "永动之地的安全法球也许能救你。")
S("%the1 protects your boat!", "%1保护了你的船！")
S("Are you sure you want to step there?", "你确认想要走到这里吗？")
S("Hint: press 1 to change the projection.", "提示：按1改变投影。")
S("%The1 turned out to be an illusion!", "%1其实是幻象！")
S("You feel that these directions are %1 turns old.", "你感觉到这些方向目前经过了%1回合。)")
S("%The1 raises his weapon...", "%1举起武器……")
S("%The1 takes %his1 revenge on %the2!", "%1对%2发起复仇！")

// parameters

S("You can enter formulas in this dialog.", "你可以在该对话框中输入方程。")
S("Functions available:", "可用的函数：")
S("Constants and variables available:", "可用的常量与变量：")
S("Animations:", "动态化：")
S("a..b -- animate linearly from a to b", "a..b——从a至b线性动态化")
S("a..b..|c..d -- animate from a to b, then from c to d", "a..b..|c..d——从a至b，然后从c至d动态化")
S("a../x..b../y -- change smoothly, x and y are derivatives", "a../x..b../y——平滑改变，x与y为导数")
S("Parameter names, e.g. '%1'", "参数名称，如“%1”")
S("Parameter names", "参数名称")
S("These can be combined, e.g. %1", "这些可以互相结合，如%1")

// manifolds

S("alternate manifolds", "替代流形")
S("generators (%1,%2), size %3", "生成器(%1,%2)，尺寸%3")
S("This option finds alternate solutions. For example, there are three {7,3} manifolds with 156 heptagons each (\"first Hurwitz triplet\").", 
  "该选项寻找其他解。如存在三个{7,3}流形，它们各有156个七边形（“first Hurwitz triplet”）。")

// unrecognized nonliteral: f->name in geom-exp.cpp:274
S("Only works with 2D geometries", "仅对2D几何生效")
S("stereographic Panini", "球极帕尼尼")
S("orthographic projection", "正交投影")
S("#%1, cells: %2, p=%3", "#%1，格子：%2，p=%3")
S("affine", "仿射")
// unrecognized nonliteral: qstring in geom-exp.cpp:625
S("geometry/topology/tiling", "几何/拓扑/镶嵌")
S("binary tiling width", "二元镶嵌宽度")
S("Nil width", "Nil宽度")
S("stretched geometry", "拉伸几何")
S(
  "Stretch the metric along the fibers. This can currently be done in rotation spaces and in 8-cell, 24-cell and 120-cell. "
  "Value of 0 means not stretched, -1 means S2xE or H2xE (works only in the limit). (Must be > -1)",
  
  "沿纤维拉伸度量。目前可以在旋转空间、8格、24格及120格中使用。0代表不拉伸，-1代表S2xE或H2xE（仅在限制内有效）。（必须 > -1）")
  
S("basic tiling", "基础镶嵌")
S("fake curvature", "虚假曲率")
S("tessellation sliders", "密铺页")
S("truncate ultra-vertices with mirrors", "截短超顶点带镜像")//unclear
S("adjacency rule", "相邻规则")
S("vertex", "顶点")
S("face", "面")
S("edge", "边")
S("info about: %1", "信息：%1")
S("genus", "属")

S("VR demos", "VR演示")
S("warning: these will restart your game!", "警告：这会重启你的游戏！")
S("standard HyperRogue but in VR", "标准HyperRogue，但使用VR")
S("HyperRogue FPP", "HyperRogue第一人称")
S("Hypersian Rug", "超波斯地毯")
S("sphere from the inside", "球内部")
S("sphere from the outside", "球外部")
S("Thurston racing", "Thurston竞速")
S("raytracing in H3", "H3光线追踪")

S("VR settings", "VR设置")
S("VR enabled", "VR启用")
S("VR initialized correctly", "VR正确初始化")
S("(this setting is for 3D geometries only, use 'model viewing' instead)", 
  "（该设置仅作用于3D几何，请改为使用“模型浏览”）")
  
S("absolute unit in meters", "以米为绝对单位")

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
  
  "非欧几何中绝对单位的长度，以米衡量。这会影响头设移动与双眼视觉。\n\n"
  "在球面几何中，绝对单位是球的半径。绝对单位越小，非欧效果越强。\n\nHyperRogue世界中的元素有固定的绝对单位尺寸，因此减小绝对单位会使它们变小。如果你在游玩欧几里得模式，该选项效果等同于缩放（比如在立方镶嵌中“绝对单位”就是立方的边长）。只会影响视角投影，其他模型使用投影菜单中的“VR尺寸”选项。")
        
S("reset the reference point", "重设参考点")
S("pointer length", "指针长度")
S(
  "If the pointer length is 0.5m, the object pointed to is 0.5 meter from the controller. "
  "This is used in situations where the controller is used as a 3D mouse, e.g., "
  "the drawing tool in three-dimensional geometries. When pointing at two-dimensional surfaces, "
  "this is not relevant (the pointer is as long as needed to hit the surface).",
  
  "如果指针长度为0.5m，则指向的物体距离控制器有0.5m。控制器被当作3D鼠标使用时（比如在三维几何中使用绘制工具）用到该数值。在指向二维面时，该选项没什么意义（指针的长度改为足以指向曲面）。")
  
S("UI size", "UI尺寸")
S("How big is a pixel of the user interface (HUD and menus). The user interface is as big as the window on the desktop.",
  "用户界面（HUD与菜单）的单个像素大小。用户界面与窗口大小相当。")

S("UI depth", "UI深度")
S("How far to show the user interface (HUD and menus).", "显示用户界面（HUD与菜单）的距离。")

//

S("Not available in the daily challenge!", "在每日挑战中不可用！")
S("Welcome to CoverRogue!", "欢迎来到CoverRogue！")//tag
S("Welcome to SL(2,R)-ogue!", "欢迎来到SL(2,R)-ogue！")
S("Emergency save at ", "紧急存档于")

// tour

S("You cannot change geometry in this slide.", "你无法改变该页的几何。")
S("This does not work in bounded geometries.", "该配置在有限几何中无效。")
S("This does not work in Euclidean geometry.", "该配置在欧几里得几何中无效。")
S("This works only in hyperbolic geometry.", "该配置在双曲几何中无效。")
S("This land has no Euclidean version.", "该地域没有欧几里得版。")
S("This land has no spherical version.", "该地域没有球面版。")
S("Changed the projection.", "更改投影。")
S("HyperRogue Guided Tour", "HyperRogue指南游览")
S("slideshows", "页面显示")
S("go up", "w górę")

S(" (edges too short: %1)", " (边过短：%1)")
S("wait...", "等等……")
S("bitruncation const", "截角常数")
S("too small period -- irregular tiling generation fails", "周期太小——不规则镶嵌生成失败")
S("To create Euclidean irregular tesselations, first enable a torus", 
  "首先开启环面才能创建欧几里得不规则密铺")

S("Error: period mismatch", "错误：周期不匹配")
S("Hantzsche-Wendt space", "Hantzsche-Wendt空间")
S("HyperRogue classic torus", "HyperRogue经典环面")

// archimedean & arbitrile

S("invalid dihedron", "不可用的二面体")
S("face mismatch", "面不匹配")
S("face mismatch II ", "面不匹配II ")
S("face mismatch II", "面不匹配II")

S("Polygon number %1 did not close correctly (%2 %3). Here is the picture to help you understand the issue.\n\n", 
  "多边形数%1未正确闭合(%2 %3)。此处有图片帮助你理解当前的状况。\n\n")
S("connection debugger", "连接debugger")
S("open a tiling", "打开镶嵌")

// goldberg

S("inverse rectify", "反转修正")
S("inverse truncate", "反转截断")
S("warped version", "扭曲版")
S("swap x and y", "交换x与y")
S("dual of current", "dualny do obecnego")
S("dual", "双")
S("unrectified", "未修正")
S("warped", "扭曲")
S("untruncated", "未截断")

// rug/embeddings
S("save the current embedding", "保存当前嵌入")
S("load a saved embedding", "读取已保存的嵌入")
S("save embedding to:", "嵌入保存至：")
S("load embedding from:", "读取嵌入自：")
S("Failed to save embedding to %1", "嵌入保存至%1失败")
S("Failed to load embedding from %1", "自%1读取嵌入失败")

S("Failed to enable", "启用失败")
S("protractor", "量角器")
S("move on touch", "触摸移动")
S("finger range", "手指范围")
S("Press 1 to enable the finger mode.", "按1开启手指模式。")
S("finger force", "手指力量")
S("Press 1 to enable the finger force.", "按1开启手指力量。")

// patterns

S("probability of a wall (%)", "墙壁概率 (%)")
S("single color", "单色")
S("distance from origin", "与原点间距")
S("nice coloring", "好看的颜色")
S("Crystal coordinates", "晶体坐标")
S("sides", "面")
S("display only chessboard white", "仅显示棋盘白色")
S("display only chessboard black", "仅显示棋盘黑色")

S(
          "This lets you specify the color pattern as a function of the cell.\n",
          "该选项使你能将上色规律设为格子的一个函数。\n")

S(
          "Available parameters:\n\n"
          "x, y, z (hyperboloid/sphere/plane coordinates in non-crystal geometries)\n"
          "ex, ey, ez (in Euclidean geometries)\n"
          "x0, x1, x2... (crystal geometry only)\n",

          "可用参数：\n\n"
          "x, y, z (非晶体几何中的双曲/球面/平面坐标)\n"
          "ex, ey, ez (欧几里得几何)\n"
          "x0, x1, x2... (晶体几何)\n"
          )

S(
          "w (fourth coordinate)\n"
          "wallif(condition, color)\n", 

          "w (第四坐标)\n"
          "wallif(条件, 颜色)\n"
          )

S("see compute_map_function in pattern2.cpp for more\n", "检视pattern2.cpp中的compute_map_function来具体了解")

S("broken Emerald Pattern", "破碎绿宝石图案")
S("single cells", "单格")
S("edit both bitrunc colors", "同时编辑两种截角颜色")
S("extra symmetries", "额外对称")
S("odd/even", "奇/偶")
S("cheat", "作弊")

S("cannot adjust depth", "无法调整深度")
S("cannot adjust camera", "无法调整相机")
S("does not work with perfect Klein", "不能用于完美克莱因模型")

S("randomize", "随机化")
S("seed", "种子")
S("select a puzzle", "选择谜题")

// texture mode

S("Failed to load %1", "读取%1失败")
S("scale/rotate the texture", "缩放/旋转纹理")
S("scale/rotate the model", "缩放/旋转模型")
S("Unexpected missing cell #%1/%1", "缺失格子 #%1/%1")
S("racing in Thurston geometries", "在Thurston几何中竞速")
S("S2xE", "S2xE")
S("H2xE", "H2xE")
S("PSL(2,R)", "PSL(2,R)")
S("torus x E", "环面 x E")

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
    
    "这种几何基本上是让你在d维网格中进行游戏。选择三维与“3D显示”来看看是怎么回事——我们其实是在三维空间中的一处六边形拼成的周期平面上进行游戏；每个六边形在可能的六个方向连接其他的六边形。通常，游戏以居住在其中的生物的视角显示（经过规整与平滑后），假定光线也被束缚在平面上——这看起来和{2d,4}镶嵌完全相同，只除了光线有时会形成环路，使得你能在某些方向看到自己的影像（换句话说，d维网格是双曲面镶嵌的商）。同样的建构对于其他维度也生效。半维度采用如下定义：多余的维度仅有两个“层次”，比如2.5D存在上半平面与下半平面。\n\n"
    "你也可以对该密铺进行截角——这使其在HyperRogue的规则下效果更好，但有些难以理解。"    
    )
S(
    "This is a representation of four-dimensional geometry. Can you find the Holy Grail in the center of the Round Table?\n\n"
    "In 'Knight of the 16-Cell Table', each cell has 8 adjacent cells, "
    "which correspond to 8 adjacent points in the four-dimensional grid. The Round Table has the shape of a 16-cell.\n\n"
    "In 'Knight of the 3-Spherical Table', it is the same map, but double bitruncated. The Round Table has the shape of a hypersphere.\n\n",
    
    
    "这是四维几何的一种表示。你能在圆桌的中心寻找到圣杯吗？\n\n"
    "在“16格圆桌骑士”中，每个格子有8个相邻格子，对应四维网格的8个相邻点。圆桌因此有16格的形状。\n\n"
    "在“3球圆桌骑士”中，我们使用相同的地图，只不过经过截角。圆桌的形状是超球。\n\n"
    )
S("let me understand how the coordinates work", "让我看看坐标是怎么一回事")
S("thanks, I need no hints (achievement)", "谢谢，我不需要提示（成就）")
S("more about this geometry...", "更多有关该几何的信息……")
S("render a cut", "绘制截断")
S("cut level", "截断高度")

S("Do not use compases.\n\n", "不要使用指南针。\n\n")
S("Crystal Camelot is an octahedron in 'pure' 3D crystal geometry (and a similar polytope in other pure crystals), "
      "and an Euclidean ball in bitruncated/Goldberg crystals.", 
  "晶体卡美洛在“纯”3D晶体几何中为八面体（在其他纯晶体中为类似的多胞形），在截角/戈德堡晶体中为欧几里得球体。")
S("puzzle/exploration mode", "解谜/探索模式")

// puzzle mode

S("exploration", "探索")
S("puzzles", "谜题")
S("other puzzles", "其他谜题")
S("hyperbolic Minesweeper", "双曲扫雷")
S("dual geometry puzzle", "双几何谜题")
S("Uncover all cells which do not contain mines", "揭示所有不包含地雷的格子")
S("Cells to uncover: %1", "需要揭示的格子：%1")
S("You won in %1", "你在%1内获胜")
S("Dropped floors: %1/%2", "落下的地板：%1/%2")//unclear

// shmup control
S("camera forward", "相机前移")
S("camera rotate X", "相机旋转 X")
S("camera rotate Y", "相机旋转 Y")
S("Configure dead zones", "设置死区") // for joysticks

// keyboard help
S("arrows", "方向键")
S("WASD keys", "WASD")
S("VI keys", "VI按键")

// sight range settings

S("sight range bonus", "额外视野范围")
S("3D sight range for the fog effect", "雾效果3D视野范围")

S(
  "Sight range for 3D geometries is specified in the absolute units. This value also affects the fog effect.\n\n"
  "In spherical geometries, the sight range of 2π will let you see things behind you as if they were in front of you, "
  "and the sight range of π (or more) will let you see things on the antipodal point just as if they were close to you.\n\n"
  "In hyperbolic geometries, the number of cells to render depends exponentially on the sight range. More cells to drawn "
  "reduces the performance.\n\n"
  "Sight range affects the gameplay, and monsters act iff they are visible. Monster generation takes this into account.",
  
  "3D几何的视野范围以绝对单位指定。这个值也会影响雾效果。\n\n"
  "在球面几何中，2π的视野范围可以让你把身后的东西看成是眼前的一样，"
  "而π（或更大）的视野范围可以让你看到对跖点上的东西，就像它们离你很近一样。\n\n"
  "在双曲几何中，需要渲染的格子数量与视野成指数关系。绘制过多的格子会降低性能。\n\n"
  "视野范围会影响游戏内容，如果你能看到怪物，它们就会行动。怪物生成时会考虑到这一点。")
  
S("max difference in Z coordinates", "Z坐标差最大值")
S("area-based range", "基于区域的范围")
S("draw range based on", "绘制距离根据")
S("size (no gen)", "尺寸（不生成）")
S("size", "尺寸")
S("sight range settings", "视野范围设置")

// other settings

S("play music when out of focus", "窗口不活动时播放音乐")
S("floating bubbles: treasure thresholds", "漂浮泡泡：宝藏门槛")
S("vector settings", "向量设置")
S("Line width setting is only taken into account in OpenGL.", "仅在OpenGL中考虑线宽设置。")
S("shots only", "仅发射")
S("hint: press Alt while testing modes", "提示：测试模式时按Alt")
S("no fill in neon mode", "霓虹模式中不填充")
S("(vsync disabled in VR)", "（VR中禁用垂直同步）")
S("apply changes", "应用更改")
S("width", "宽度")
S("camera movement speed", "相机移动速度")
S("camera rotation speed", "相机旋转速度")
S("send scores to Steam leaderboards", "将得分上传至Steam排行榜")
S("crosshair size", "准星尺寸")
S("crosshair color", "准星颜色")

S("Must be less than %1°. Panini projection can be used to get higher values.", 
  "必须少于%1°。可以使用帕尼尼投影获得更高值。")

S("Panini projection", "帕尼尼投影")
S(
  "The Panini projection is an alternative perspective projection "
  "which allows very wide field-of-view values.\n\n",
  
  "帕尼尼投影是另一种视野非常宽的视角投影。\n\n")

S("spherical perspective projection", "球面视角投影")
S(
  "Set to 1 to get stereographic projection, "
  "which allows very wide field-of-view values.\n\n",
  
  "设为1为球极投影，视野非常宽。\n\n"  
  )

S(
  "HyperRogue uses "
  "a quick implementation, so parameter values too close to 1 may "
  "be buggy (outside of raycasting); try e.g. 0.9 instead.", 
  
  
  "HyperRogue使用快速方法，特别接近1的参数可能会存在问题（光线投射以外）；可以以0.9代替。")

S("Berger sphere limit", "Berger球限制")

S("Primitive-based rendering of Berger sphere is currently very slow and low quality. "
  "Here you can choose how many images to draw.", 
  "Berger球基于图元的渲染目前非常缓慢且低质量。在此你可以选择绘制多少图像。")
  
S(
  "Camera is placed %1 absolute units above a plane P in a three-dimensional "
  "world. Ground level is actually an equidistant surface, %2 absolute units "
  "below the plane P. The plane P (as well as the ground level or any "
  "other equidistant surface below it) is viewed at an angle of %3 "
  "(the tangent of the angle between the point in "
  "the center of your vision and a faraway location is 1/cosh(c) = %4).", 
  
  "相机在三维世界中位于P平面%1个绝对单位以上。地表其实是一个等距平面，在P平面以下%2个绝对单位。检视P平面（以及地表层级或下方的任意等距面）时的夹角为%3（你视野中心与极远处一点之间夹角的正切为1/cosh(c) = %4）。")
  
S("cool fog effect", "炫酷的雾效果")
S("borders", "边缘")
S("pattern colors", "图案颜色")
S("Galápagos shading", "加拉帕戈斯阴影")
S("targetting ranged Orbs long-click only", "仅长按时使用远距离法球")

S("0 to disable", "0至距离")
S("recently changed settings", "最近修改的设置")
S("find a setting", "寻找设置")

// nonisotropic

S("geodesic table missing", "测地线表缺失")
S(
  "If the 2D underlying manifold is bounded, the period should be a divisor of the 'rotation space' "
  "value (PSL(2,R)) times the Euler characteristics of the underlying manifold. "
  "For unbounded underlying manifold, any value should work (theoretically, "
  "the current implementation in HyperRogue is not perfect).\n\n"
  "We won't stop you from trying illegal numbers, but they won't work correctly.", 
        
  "如果二维底层流形是有界的，周期应该是“旋转空间”的除数，值为(PSL(2,R))乘以底层流形的欧拉特征。对于无边界的底层流形，任何数值都可以（理论上如此，HyperRogue目前的实现并不完美）。\n\n"
  "游戏不会阻止你尝试不合法的数值，但它们无法正常工作。"
  )
S("SL(2,R)", "SL(2,R)")
S("universal cover", "普遍覆盖")
S("maximum", "最大")
S("works correctly so far", "目前正常运行在")

S("non-periodic", "非周期")
S("patched Chaos", "修正混沌")
S("excessive walls", "过多墙壁")
S("total chaos", "完全混沌")
S("random-walk chaos", "随机行走混沌")
S("single land: ", "单地域：")
S("single land", "单地域")
S("wall-less", "无墙壁")

S("highlight stuff", "高亮物体")
S("press Alt", "按Alt")
S("highlight", "高亮")
S("super-highlight", "超级高亮")

S("shape editor", "形状编辑器")
S("drawing tool", "绘制工具")
S("land structure", "地形结构")
S("highlights & achievements", "高亮与成就")
S("classic game except hyperbolic", "经典游戏，但双曲")
S("can you find the Heptagonal Grail?", "你能找到七边形圣杯吗？")
S("General Euclid", "一般欧几里得")
S("Worm of the World", "世界之虫")
S("Lovász Conjecture", "Lovász猜想")
S("Knight of the 16-Cell Table", "16格圆桌骑士")
S("Knight of the 3-Spherical Table", "3球圆桌骑士")
S("Emerald Mine in {5,3,4}", "{5,3,4}中的绿宝石矿")

// help

S("credits", "鸣谢")

S("\n\nAdditionally, the probabilities of generating terrain features are subtly changed in the following lands:", 
  "\n\n此外，生成特殊地形的概率在以下地域中稍有修改：")
S(
    "\n\nOnce you collect a Bomberbird Egg, "
    "stepping on a cell with no adjacent mines also reveals the adjacent cells. "
    "Collecting even more Eggs will increase the radius.",
    
    "\n\n在你找到一枚炸弹鸟蛋后，踩到周围没有地雷的格子会同时揭示相邻的格子。收集更多的炸弹鸟蛋来增加揭示的范围。")
    
S(" (shadow in %1)", " (%1中的阴影)")
S(", %1 the Knight", ", 骑士%1")
S("Hypersian Rug menu", "超波斯地毯菜单")
S("HyperRogue help", "HyperRogue帮助")

// history mode

S("s = save to .png", "s = 保存为.png")
S("rendered band ($ID=segment, $DATE=date)", "渲染带 ($ID=片段, $DATE=日期)")
S("animate from last recenter to precise current view", "自上一次重设中心点至当前精确视野动态化")
S("save the animation starting point", "保存动态化原点")
S("animate from the starting point", "自原点动态化")
S("reset animation", "重置动态化")
S("include history when auto-rendering", "自动渲染时包含历史")
S("more options in hyperbolic geometry", "在双曲几何中有更多选项")
S("more options in band projections", "在带投影中有更多选项")
S("filename format to use ($ID=segment, $DATE=date)", "使用的文件格式 ($ID=片段, $DATE=日期)")
S("band segment", "带片段")

// screenshot/animations

S("PNG", "PNG")
S("SVG", "SVG")
S("WRL", "WRL")
S("centering", "居中")
S("You can pick the angle. Note: the direction the PC is facing matters.", 
  "你可以挑选角度。注意玩家角色面对的方向。")
S("rotate PC", "旋转玩家角色")
S(
  "The value of 1 means that the period of this animation equals the period set in the animation menu. "
  "Larger values correspond to faster animations.", 
  "1代表该动画的周期等同于动画菜单中设置的周期。更高的值对应更快的动画。")
S("shift", "平移")
S("movement angle", "移动角")
S("This is the period of the whole animation, though in some settings the animation can have a different period or be aperiodic. "
      "Changing the value will make the whole animation slower or faster.",
  "这是整个动画的周期，不过在某些设置下动画可能会具有不同的周期，或不具有周期。改动该值会使整段动画变快或变慢。")
S("Least common multiple of the animation periods of all the game objects on screen, such as rotating items.", 
  "屏幕上所有如旋转的物品等游戏物体的动画周期的最小公倍数。")
S("Möbius transformations", "莫比乌斯变形")
S("circle spins", "圆圈扭转")
S("circle radius", "圆圈半径")
S("double spin", "两重扭转")
S("triple spin", "三重扭转")
S("draw the circle", "绘制圆圈")
S("cycle length", "周期长度")
S("full circle", "完整圆圈")
S("Zebra period", "斑马周期")
S("Bolza period", "Bolza周期")
S("angle to screen normal", "与常态屏幕的夹角")//unclear
S("distance from rotation center", "与旋转中心的距离")
S("monster turns", "怪物回合")
S("Number of turns to pass. Useful when simulating butterflies or cellular automata.", 
  "要经过的回合数。适用于模拟蝴蝶或元胞自动机。")
S("screen-relative rotation", "相对屏幕旋转")
S("angle", "角度")
S("model-relative rotation", "相对模型旋转")
S("automatic move speed", "自动移动速度")
S("Move automatically without pressing any keys.", "不按键而自动移动。")
S("synchronize", "同步")
S("model rotation", "模型旋转")
S("3D rotation", "3D旋转")
S("some parameters will only change in recorded animation", "有些参数仅在录制的动画中改变")
S("record to sequence of image files", "录制为一系列图片文件")
S("record to video file", "录制为视频文件")
S("explore this animation", "探索该动画")

// raycaster

S("volumetric raycasting", "体积光线投射")
S("active", "启用")
S("intensity of random coloring", "随机上色强度")
S("color randomly", "随机上色")
S("SMART", "智能")
S("brightness formula: max(1-d/sightrange, s*exp(-d/r))\n", "亮度公式：max(1-d/视野范围, s*exp(-d/r))\n")
S("unfortunately this honeycomb is too complex for the current implementation (%1>%2)", 
  "该蜂窝对于目前的配置过于复杂了 (%1>%2)")

// extra projections

S("geodesic", "测地线")
S("quadrant coordinates", "象限坐标")
S("axial coordinates", "轴坐标")
S("anti-axial coordinates", "反轴坐标")
S("Werner projection", "Werner投影")
S("Aitoff projection", "Aitoff投影")
S("Hammer projection", "Hammer投影")
S("loximuthal projection", "恒向方位角投影")
S("Miller projection", "Miller投影")
S("Gall stereographic", "Gall球极投影")
S("Winkel tripel", "温克尔三重")
S("Poor man's square", "粗略平方")
S("Craig retroazimuthal", "Craig反方位角投影")
S("Littrow retroazimuthal", "Littrow反方位角投影")
S("Hammer retroazimuthal", "Hammer反方位角投影")
S("three-point equidistant", "三点等距")
S("stereographic projection [VR]", "球极投影 [VR]")

// projection settings

S("simple model: projection", "简单模型：投影")
S("simple model: perspective", "简单模型：视角")
S("projection type", "投影类型")
S("edit VR or non-VR settings", "编辑VR或非VR设置")
S("rotational or Heisenberg", "旋转或海森堡")
S("1 = Heisenberg, 0 = rotational", "1 = 海森堡，0 = 旋转")
S("use atan to make it finite", "使用atan来设为有限值")
S("VR: rotate the 3D model", "VR：旋转3D模型")
S("VR: shift the 3D model", "VR：平移3D模型")
S("VR: scale the 3D model", "VR：缩放3D模型")
S("match the period", "匹配周期")
S("(zoomed out)", "(缩小)")
S("zoom 2x", "缩放 2x")
S("zoom 1x", "缩放 1x")
S("zoom 0.5x", "缩放 0.5x")
S("cylinder/donut view", "圆柱/甜甜圈视野")
S("first-person perspective", "第一人称视角(FPP)")
S("toggle full 3D graphics", "切换全3D画面")
S("advanced projections", "高级投影")

// map editor
S("F5 = clear", "F5 = 清除")
S("Are you sure you want to clear the map?", "你确定想清空地图吗？")
S("select area to copy", "选择复制的区域")
S("statue", "雕像")
S("c = reuse", "c = 再次使用")
S("d = delete", "d = 删除")
S("p = grid color", "p = 网格颜色")
S("f = fill", "f = 填充")
S("T = text", "T = 文本")
S("e = erase", "e = 擦除")
S("z = spheres", "z = 球")
S("z-level", "z层级")

S("Failed to load pictures from %1", "自%1读取图片失败")
S("Pictures loaded from %1", "已自%1读取图片")
S("Failed to save pictures to %1", "保存图片至%1失败")
S("Pictures saved to %1", "图片已保存至%1")
S("Hint: use F7 to edit floor under the player", "提示：按F7编辑玩家下方的地板")
S("Are you sure you want to restart? This will let you draw on a blank screen.", "确定要重新开始吗？这会令你在空白屏幕上绘制内容。")
S("brush size", "笔刷尺寸")

// checking configurables
S("Aitoff parameter", "Aitoff参数")
S("The Aitoff projection is obtained by multiplying the longitude by 1/2, using azimuthal equidistant projection, and then dividing X by 1/2. Hammer projection is similar but equi-area projection is used instead. Here you can change this parameter.", 
  "Aitoff投影通过将经度乘1/2，使用方位角投影，然后将X除以1/2得到。Hammer投影与之类似，但使用等面积投影。在此处你可以修改该参数。")
S("any spiral multiplier", "任意螺旋倍数")
S("Collignon parameter", "Collignon参数")
S("Collignon reflect", "Collignon反射")
S("ETS parameter", "ETS参数")
S("eye level", "眼部高度")
S("off", "关")
S("spam", "spam")
S("normal monsters", "普通怪物")
S("high-threat monsters only", "仅高威胁怪物")
S("faraway highlight color", "远处高亮颜色")
S("0 = monster color, 100 = red-light oscillation", "0 = 怪物颜色，100 = 红光振荡")
S("font size", "字体尺寸")
S("use specific fullscreen resolution", "使用特定全屏幕分辨率")
S("fullscreen resolution to use (X)", "使用的全屏幕分辨率 (X)")
S("fullscreen resolution to use (Y)", "使用的全屏幕分辨率 (Y)")
S("hyperbolic|spherical|Euclidean", "双曲|球面|欧几里得")
S("show flat", "显示平坦")
S("loximuthal parameter", "恒向方位角参数")
S("Loximuthal is similar to azimuthal equidistant, but based on loxodromes (lines of constant geographic direction) rather than geodesics. The loximuthal projection maps (the shortest) loxodromes to straight lines of the same length, going through the starting point. This setting changes the latitude of the starting point.\n\nIn retroazimuthal projections, a point is drawn at such a point that the azimuth *from* that point to the chosen central point is correct. For example, if you should move east, the point is drawn to the right. This parameter is the latitude of the central point.\n\n(In hyperbolic geometry directions are assigned according to the Lobachevsky coordinates.)", 
  "恒向方位角类似于等距方位角，但基于恒向线（球面上与所有经线相交成等方位角的曲线）而非测地线。恒向方位角投影将（最短的）恒向线映射为穿过原点的等长直线。该设置改变原点的纬度。\n\n"
  "在反方位角投影中，点绘制的位置使得该点与选定的中心点之间的方位角呈直角。例如，如果你要向东移动，点绘制于右侧。该参数为中心点的维度。\n\n（在双曲几何中方向根据罗巴切夫斯基坐标指定。）")
 
S("onscreen keyboard", "屏幕键盘")
S("projection distance|Gans Klein Poincare orthographic stereographic", "投影距离|Gans Klein Poincare正交球极投影")
S("set relative font size", "设置相对字体尺寸")
S("resizable window", "窗口可调整尺寸")
S("right spiral multiplier", "右螺旋倍数")
S("fisheye parameter", "鱼眼参数")
S("two-point parameter", "两点参数")
S("In two-point-based models, this parameter gives the distance from each of the two points to the center.", 
  "在基于两点的模型中，该参数给出两点分别相对中心的距离。")
S("Winkel Tripel mixing", "温克尔三重混合")
S("The Winkel Tripel projection is the average of Aitoff projection and equirectangular projection. Here you can change the proportion.", 
  "温克尔三重投影是Aitoff投影与等矩形投影的平均。在此你可以修改比例。")
S("sphere spiral multiplier", "球螺旋倍数")
S("spiral cone", "螺旋锥")
S("variation|dual|bitruncated", "变体|双|截角")
S("screen size percentage to use (X)", "使用的屏幕尺寸百分比 (X)")
S("screen size percentage to use (Y)", "使用的屏幕尺寸百分比 (Y)")
S("specify relative window size", "指定相对窗口尺寸")
S("window resolution to use (X)", "使用的窗口分辨率 (X)")
S("window resolution to use (Y)", "使用的窗口分辨率 (Y)")

// ...

// fixed text

S("select the starting land", "选择起始地域")

// missing text in animation

S("cells to go", "剩余格子数")

// missing translations for Hardcore, and the new Casual mode

S("casual mode", "轻松模式")
S("Switching casual allowed only before saving the game. Do you want to restart?", 
  "只能于存档过之前切换至轻松模式。你想重新开始吗？")
S("You are in the casual mode! Achievements are disabled.", 
  "你进入了轻松模式！成就已禁用。")
S("Collect an Orb of Safety to save a checkpoint.", 
  "收集安全法球来设置检查点。")
S("load (%1 turns passed)", "读取（已经过%1回合）")

// missing texts in searching

S("press letters to search", "输入字母进行搜索")
S("matching items", "匹配的条目：")

// YASC codes

S("YASC codes", "YASC代码")
S(" YASC code: ", "YASC代码：")
S("YASC codes: Sides-Entity-Restrict-Threat-Wall", "YASC代码：Sides-Entity-Restrict-Threat-Wall")

// Dice Reserve

N("Dice Reserve", GEN_N, "骰子保护区","")
S("This land is full of sentient dice. Do not disturb them!",
  "这片地域充满了智慧骰子。不要打搅它们！")

N("Crystal Die", GEN_N, "水晶骰子","枚")
S("A nice souvenir from the Dice Reserve. Make sure to collect the whole set!",
  "来自骰子保护区的小纪念品。要集齐一整套哦！")

N("Unhappy Die", GEN_N, "不快骰子","枚")
S("Sentient dice like to be in a position such that their highest number is on top. "
  "Unfortunately, someone has rolled this one into a wrong position, and did not fix this. "
  "It will reward you if you roll it so that the highest number is on top again!",

  "智慧骰子喜欢以最大的点数在顶部的姿态休息。可惜有人把这个骰子滚动到了其他位置，而且没有复原。如果你把它最大的点数再滚到顶上，它会奖励你！")

N("Happy Die", GEN_N, "快乐骰子","枚")

S(
  "A happy sentient die. Dice are happy when they are in their correct position "
  "(the highest number on the top); happy dice with one roll from their best position are sometimes found too."
  "Other positions are much less convenient for them and expose their "
  "weak spots. You can roll Happy Dice, but it may become angry!",

  "一枚快乐的智慧骰子。骰子在所处位置正确时感到开心（最大的点数在顶部）；有时也会发现距离正确位置只差一次滚动的快乐骰子。其他位置对它们来说很难受，会暴露它们的弱点。你可以滚动快乐骰子，但它可能会生气！")

N("Animated Die", GEN_N, "活化骰子","枚")

S("When sentient dice are too long in an incorrect position, they start to move on their own, "
  "and attack everyone. You can still convince Animated Dice of your good intentions by "
  "rolling them into a position such that the highest number is on top. "
  "If you do, the die will stop moving and (if it happens in the Dice Reserve) you will be rewarded. "
  "Other rolls and attacks are not allowed.",
  
  "活化骰子在不正确的位置待太久后会自己开始移动、攻击一切。你还是能通过把活化骰子滚动到最大点数在顶部的位置来劝服它们。如果你成功了，它们会停止移动并（如果在骰子保护区中）奖励你。不能对它们进行其他滚动或攻击。"
  )

N("Angry Die", GEN_N, "愤怒骰子","枚")

S("You have made a die unhappy. Taste the revenge! This one won't forgive you, no matter what you do.",
  "你把一枚骰子惹怒了。品尝你种下的苦果吧！这一枚不管你做什么都不会原谅你。")

S("The die is now happy, and you are rewarded!", "骰子开心了，而你获得了奖励！")
S("You have made a Happy Die angry!", "你激怒了一枚快乐骰子！")
S("You can only push this die if the highest number would be on the top!", 
  "你只能进行使它最大点数位于顶部的滚动！")
S("This die is really angry at you!", "这枚骰子对你非常愤怒！")
S("The die is now happy, but won't reward you outside of the Dice Reserve!", "骰子开心了，但它不会在保护区外奖励你！")

// Cursed Canyon
//===============

N("Cursed Canyon", GEN_N, "诅咒峡谷","")
S("This canyon is full of curses! Curses are like Orbs, but grant negative effects.",
  "这处峡谷中满是诅咒！诅咒和法球类似，但效果是负面的。")
  
N("Canyon Hag", GEN_N, "峡谷妖婆","个")
S("A Canyon Hag can curse you from afar, if standing on top of a Curse and close enough.",
  "如果峡谷妖婆站在诅咒上且离你够近，她就可以隔空诅咒你。")
S("%The1 curses you with %the2!", "%1以%2诅咒你！")

N("Capon Stone", GEN_N, "鸡内金","块")
S("A lapis alectorius, alectoria or capon stone is a non-precious stone found in the gizzard of capons (young, castrated roosters). In magic it is believed to be an effective amulet, granting the wearer a heightened sense of courage and boldness. These properties are unproven. [Wikipedia]",
  "鸡内金，中药名。为雉科动物家鸡Gallusgallusdomesticus Brisson的干燥砂囊内壁。杀鸡后，取出鸡肫，立即剥下内壁，洗净，干燥。[百度百科]")

#define Curse(E, P) N("Curse of " E, GEN_N, P"诅咒","")
Curse("Weakness", "虚弱")
S("This Curse weakens your normal attack, which will only stun monsters. For monsters which are normally only stunned anyway, they don't push the victim away. You are also unable to cut trees or fight Ivy.",
  "这一诅咒弱化你的普通攻击，使得攻击只能击晕怪物。对于本来就只会被击晕的怪物，它们改为不会被推走。你同时也无法砍伐树木或常春藤。")

Curse("Draining", "汲取")
S("Drains the powers of all your orbs (and curses) by 2 charges per turn.",
  "每回合汲取你所有法球（与诅咒）的2点充能。")

Curse("Water", "水流")
S("Makes you leave a trail of shallow water. You also become afraid of water. You are unable to step into water, including shallow water and boats on water. "
  "On the good side, you are protected from fire.",
  
  "使你身后留下一道浅水。你同时变得怕水。你不能走入水中，包括浅水与水面上的船。不过，你变得防火。")
  
Curse("Fatigue", "疲劳")
S("With this Curse you cannot move too quickly. Every move increases your fatigue by 1; fatigue cannot go over 10. "
  "Resting reduces your fatigue by 5.",
  "你具有该诅咒时不能移动太快。每次移动都增加你1点疲劳；疲劳不能超过10。休息降低5点疲劳。")
N("fatigue", GEN_N, "疲劳","")
S("See the Curse of Fatigue.", "请检视疲劳诅咒。")
S("You are too fatigued!", "你太疲劳了！")

Curse("Repulsion", "反斥")
S("If you have this Curse, all the items on adjacent cells will try to escape from you.",
  "在你拥有该诅咒时，相邻格子上的所有物品都会试图逃离你。")

Curse("Gluttony", "饕餮")
S("The first item you pick up is consumed without any effect.",
  "你捡起的第一件物品会被吞下，不起任何效果。")
S("%The1 is consumed!", "%1被你吃了！")

Orb("Purity", "纯净")
S("Reverses all the curses. When found far away from the Cursed Canyon, you gain low amounts of various reversed curse powers.",
  "反转所有的诅咒。在远离诅咒峡谷的地方找到该法球时，你获得少量各种反转的诅咒力量。")
#undef Curse
#undef Orb

// chapter names of the Guided Tour
S("Hyperbolic shapes", "双曲形状")
S("Projections of hyperbolic space", "双曲空间的投影")
S("Procedural generation", "程序生成")

// newbie-friendly Safety

S("Find an Orb of Safety to save your game",
  "寻找安全法球来保存游戏")

S("how to find an Orb of Safety?", "怎么才能找到安全法球？")

S(
  "To save the game you need an Orb of Safety.\n\n"
  "Orbs of Safety appear:\n\n"
  "* in the Crossroads and the Land of Eternal Motion, after you collect %1 Phoenix Feathers in the Land of Eternal Motion\n\n"
  "* in the Ocean after you unlock it (%2 treasures)\n\n"
  "* in the Prairie after you unlock it (%3 treasures)\n\n",
  
  "如果想要保存游戏，你需要找到一枚安全法球。\n\n"
  "安全法球出现在：\n\n"
  "* 交叉路口与永动之地，在你在永动之地收集%1个凤凰羽毛后\n\n"
  "* 海洋，在你解锁该法球后（%2个宝藏）\n\n"
  "* 大草原，在你解锁该法球后（%3个宝藏）\n\n"
  )

// 13.0

// gameplay
//==========

// these were missing somehow

N("Palace Quest", GEN_N, "宫殿任务","")
N("Pike", GEN_N, "梭鱼","条")

// crossbow

S("The warped space distracts you from reloading while staying in place!",
  "扭曲的空间干扰你原地装弹！")
S("Fire!", "发射！")
S("(shooting while unstable -- no turn passes)", "（在不稳定处射击——未消耗回合）")
S("Stab them by shooting around them.", "（射击它们身侧来进行背刺。）")
S("You are too weakened to attack %the1!", "你太虚弱了，无法攻击%2！")
S("You cannot attack your own mount!", "你不能攻击自己的坐骑！")
S("A magical shield protects %the1!", "魔法护盾保护着%1！")
S("You have no melee weapon!", "你没有近战武器！")
S("Your crossbow is still reloading!", "你的弩还在装弹！")
S("Trying to fire.", "试图射击。")
S(" (turns to reload: %1)", "（装弹所需回合数：%1）")
S(" (fire mode on / turns to reload: %1)", "（发射模式开启/装弹所需回合数：%1）")
S(" (fire mode on)", "（发射模式开启）")
S(" (click to fire)", "（点击发射）")
S("You fire an angry glance at your enemies.", "你对敌人投去愤怒的一瞥。")
S("Note: cannot fire again yet. Turns to reload: %1.", "注意：目前无法再次射击。装弹所需回合数：%1")
S("Fire crossbow! Click to aim, click again to confirm.", "发射弩箭！点击瞄准，再次点击确认。")
S("Fire mode disabled.", "发射模式已关闭。")
S("Firing cancelled.", "取消发射。")
S("Cannot fire again yet. Turns to reload: %1.", "目前无法再次射击。装弹所需回合数：%1")
S("No way to hit this place.", "无法击中此处。")
S("Shooting impossible.", "无法发射。")
S("%The1 is enraged!", "%1被激怒了！")
S("weapon selection", "武器选择")
S("Cannot hit anything by shooting this direction!", "对该方向发射无法击中任何东西！")
S("Welcome to HyperRanger!", "欢迎来到HyperRanger！")
S("bow color", "弓颜色")
S("bowstring color", "弓弦颜色")
S("%The1 alarms other dogs as it dies!", "%1在死前警告了其他犬只！")
S("crossbow straight line style", "弩直线样式")
S("bull line", "公牛线")
S("Can go in either direction on odd shapes. 3 turns to reload.", "可以射击到奇数边形任意一侧。3回合装弹。")
S("Graph geodesic: any sequence of tiles is OK as long as there are no shortcuts. 4 turns to reload.", "图测地线：只要没有更短路线，则任意格子序列均可。4回合装弹。")
S("geometric", "几何线")
S("Approximations of geometric straight lines.", "近似为几何直线。")
S("bump to shoot", "碰撞发射")
S("mouse auto-fire mode", "鼠标自动发射模式")
S("explicit", "刻意")
S("You need to click crossbow or be close to fire.", "你需要点击弩或相邻时才能发射。")
S("priority", "优先")
S("Click on a faraway monster to fire if possible, or move if not.", "点击远处怪物则尽可能对其射击，否则进行移动。")
S("Clicking on a faraway monster always means an attempt to fire.", "点击远处怪物总是尝试对其射击。")
S("blade", "短刀")
S("Standard Rogue weapon. Bump into a monster to hit. Most monsters attack you the same way.",
  "标准游荡者武器。碰撞怪物来攻击。大多数怪物使用相同方式攻击你。")
S("crossbow", "弩")
S("Hits all monsters in a straight line, but slow to reload. Press 'f' or click the crossbow icon to target.",
  "击中一条直线上所有的怪物，但装弹缓慢。按“f”或点击弩图标来瞄准。")

// 'click to use orb' errors

S("You have no ranged Orbs!", "你没有远程法球！")
S("Strong wind can only take you to a specific place!", "强风只能将你引向特定位置！")
S("%The1 can only be used on items!", "%1只能对物品使用！")
S("Nothing to blow here!", "没有东西可吹走！")
S("Cannot be used in multiplayer", "无法在多人游戏中使用！")
S("You cannot grow on yourself!", "你不能在自己身上生长！")
S("Cannot attack there!", "不能攻击那里！")
S("Cannot grow there!", "不能在那里生长！")
S("Cannot grow against gravity!", "不能违背重力生长！")
S("You cannot grow there from any adjacent cell!", "你无法通过任何相邻格子生长到那里！")
S("Cannot vault that far!", "无法翻越那么远！")
S("ERROR: No common neighbor to vault through!", "错误：没有用于翻越的共同相邻物体！")
S("Can only vault in a roughly straight line!", "只能大致沿直线翻越！")
S("Nothing to vault over!", "没有翻越的对象！")
S("Cannot pass through %the1 while vaulting!", "不能越过%1！")
S("Cannot vault onto %the1!", "不能翻越到%1上！")
S("Cannot vault to %the1!", "不能翻越至%1！")
S("Cannot attack %the1 while vaulting!", "不能在翻越时攻击%1！")
S("Cannot jump that far!", "不能跳跃那么远！")
S("Cannot jump onto %the1!", "不能跳跃到%1上！")
S("Cannot phase that far!", "不能穿墙那么远！")
S("Cannot phase onto %the1!", "不能穿墙到%1上！")
S("Cannot phase to %the1!", "不能穿墙到%1里！")
S("Nothing to phase through!", "没有穿墙的目标！")
S("Can only use %the1 on a monster!", "只能对怪物使用%1！")
S("%The1 is immune to %the2!", "%1免疫%2！")
S("%The1 can only be used on monsters.", "%1只能被用于怪物。")
S("%The1 cannot be used on big monsters.", "%1不能被用于大体型怪物。")
S("%The1 cannot be used on %the2.", "%1不能被用于%2。")
S("%The1 is already stunned!", "%1已经被击晕了！")

// multiplayer settings

S("friendly fire", "友军伤害")
S("friendly fire off -- achievements disabled", "友军伤害已关闭——成就已禁用")

S("player vs player", "玩家vs玩家")
S("PvP available only in shmup", "PvP只能在射击模式中进行")
S("PvP grants infinite lives -- achievements disabled", "PvP无限生命——成就已禁用")

S("split screen mode", "分屏模式")
S("achievements disabled in split screen", "分屏模式——成就已禁用")

S("auto-adjust dual-focus projections", "自动调整双焦点投影")
S("autoscale dual focus", "自动缩放双焦点")

S("self hits", "自我攻击")

S(" (%1 $$$, %2 kills, %3 deaths)", "(%1 $$$，击杀数：%2，死亡数：%3)")
S(" (%1 pkills)", " (%1 玩家击杀)")
S(" (%1 self)", " (%1 自杀)")

// racing mode

S("play on an official track", "在官方赛道进行游戏")
S("generate a random track", "生成随机赛道")
S("Too many pauses!", "暂停太多了！")
S("Pauses: %1 of %2 allowed", "暂停：%1/%2次可用")

// new land structures

S("horodisks", "极限碟")
S("land size in horodisk mode", "极限碟模式地域大小")
S("Set this to -2 to get perfect horodisks. Smaller values yield less dense horodisks, and larger values might produce horodisks with errors or crashing into each other.",
  "设为-2则为完美极限碟。更小的值产生更稀疏的极限碟，更大的值可能存在错误，或极限碟互相碰撞。")

S("ideal Voronoi", "理想沃罗诺伊图")
S("display Voronoi tie debug values", "显示沃罗诺伊图结debug值")

S("land size in randomwalk mode", "随机行走模式地域大小")
S("The average size of a land in randomwalk mode.", "随机行走模式中的地域平均大小。")

S("this starting land is not eligible for achievements", "使用该起始地域不能解锁成就。")
S("eligible for most achievements", "可解锁大多数成就")
S("eligible for Chaos mode achievements", "可解锁混沌模式成就")
S("eligible for special achievements", "可解锁特殊成就")
S("not eligible for achievements", "无法解锁成就")
S("(but the cheat mode is on)", "(但作弊模式已开启)")
S("(but the casual mode is on)", "(但轻松模式已开启)")

// other gameplay

S("There should be a Palace somewhere nearby...", "附近应该有一处宫殿……")
S("The following Orbs act an extra lives in the shmup mode:", "以下法球在射击模式中等同于一条额外生命……")
S("\n\nThis Orb is triggered on your first direct melee attack or illegal move.",  /* 'direct melee' added to an earlier version of this message */
  "\n\n该法球在你进行第一次直接物理攻击或不合法行动时触发。")
S("\n\nOrb unlocked: %1", "\n\n解锁的法球：%1")
S("toggle numerical display", "切换数值显示") // in help
S("display mine counts numerically", "以数字显示地雷数。")
S("Accessible only from %the1, %2, or %3.\n", "可用地域：%1、%2、%3。\n")
S("Cannot switch places with %the1!", "无法与%1交换位置！")
S("d%1 rolled %2", "d%1掷出%2")

S("orb display mode", "法球显示模式")
S("icons", "图标")

// menu & configuration
//======================

S("default: ", "默认：")
S("use the default value", "使用默认值")

S("pick scores", "挑选得分")

S("Angle to rotate by.", "旋转的角度。")
S("dxy(n) = rotate n degrees from x to y\n\nd-degree, r-radian, t-turn\n\nexample: dxy(30)*dyz(45)",
   "dxy(n) = 自x至y旋转n度\n\nd-角度，r-弧度，t-回合\n\n例如：dxy(30)*dyz(45)")

S("no filters", "无过滤")

S("yet another classic game", "又一款经典游戏") // hyperbolic Minesweeper

S("context help", "上下文帮助")
S("all context help/welcome messages", "全部上下文帮助/欢迎信息")
S("no context help/welcome messages", "无上下文帮助/欢迎信息")
S("I know I can press F1 for help", "我知道我可以按F1获取帮助")

S("menu map darkening", "菜单地图变暗")
S("A larger number means darker game map in the background. Set to 8 to disable the background.",
  "数值越大代表背景的游戏地图越黑。设为8禁用背景。")
S("centered menus in widescreen", "宽屏时居中菜单")

S("less items/kills in landscape", "横屏时更少物品/击杀")
S("less items/kills in portrait", "竖屏时更少物品/击杀")

S("forced center down", "取消强制中心")
S("make the center not the actual screen center", "使得中心并非为实际屏幕中心")

// animations

S("idle animation speed", "闲置动画速度")
S("flashing effects", "闪光效果")
S("Disable if you are photosensitive. Replaces flashing effects such as Orb of Storms lightning with slow, adjustable animations.",
  "如果你对光敏感就禁用。将风暴法球等的闪光效果替换为缓慢可调的动画。")

S("start animations", "开始动画")

S("movement animation", "移动动画")

S("animation rug angle", "动画地毯角")
S("rug forward movement angle", "地毯前向运动角")
S("rug_camera angle", "地毯相机角")
S("translation+rotation", "平移+旋转")

// save file selection

S("select the score/save file on startup", "启动时选择得分/存档文件")
S("choose your score/save file", "选择你的得分/存档文件")
S("Save the config and restart to select another score/save file.", "保存设置并重启以选择另一个得分/存档文件。")
S("Save the config to always play without recording your progress.", "保存设置为在游戏时从不录制过程。")
S("Save the config to always use %1.", "保存设置为总是使用%1。")
S("Your progress will not be saved.", "你的进度不会被保存。")

// online demo (not translated but just in case)

S("HyperRogue %1: online demo", "HyperRogue %1：在线demo")
S("play the game", "开始游戏")
S("learn about hyperbolic geometry", "学习双曲几何")
S("toggle high detail", "切换高细节")

// other

S("wiki", "百科")
S("highlight faraway monsters", "高亮远处怪物")

// map editor, line patterns, etc.
//=================================

S("hint: shift+A to enter the map editor", "提示：shift+A进入地图编辑器")

S("z = set Shift+click", "z = 设置Shift+点击")
S("B = build on walls ", "B = 在墙壁上建设")
S("S = snap (%1)", "S = 快照 (%1)")
S("Z =", "Z = ")
S("X =", "X = ")
S("Y =", "Y = ")
S("w: %1", "w: %1")
S("λ: %1°", "λ: %1°")

S("edit cell values", "编辑格子值")
S("canvas floor shape", "画布地板形状")
S("canvas darkness", "画布黑暗")

S("die shape", "骰子形状")
S("die face", "骰子面")
S("die direction", "骰子方向")
S("die mirror status", "骰子镜像状态")
S("mirrored", "已镜像")

// line patterns

S("parallel/meridian orientation", "平行/子午线取向")
S("number of parallels drawn", "平行线绘制数量")
S("last parallel drawn", "最后绘制的平行线")
S("tree-drawing parameter", "树绘制参数")
S("How much of edges to draw for tree patterns (to show how the tree edges are oriented).",
  "为树图案绘制的边数（用于显示树边的取向）。")

// debug tools

S("error", "错误")

S("display tile debug values", "显示地块debug值")
S("Display cell type IDs, as well as vertex and edge identifiers.\n\n"
  "Setting 1 uses the internal shape IDs, while setting 2 in tes files uses the original IDs"
  " in case if extra tile types were added to separate mirror images or different football types.",

  "显示格子类别ID，以及顶点与边标识。\n\n设置1使用内部形状ID，设置2在额外地块类型被添加至单独的镜像或不同足球类型时在tes文件中使用原始ID。")

// tessellations / honeycombs / projections / embeddings
//========================================================

// new geometry settings

S("projective Bring's Surface", "投影Bring曲面")
S("aperiodic hat", "非周期帽子")
S("Sierpiński triangle", "Sierpiński三角形")
S("Sierpiński carpet", "Sierpiński地毯")
S("6-flake fractal", "6瓣分形雪花")
S("Menger sponge", "门格海绵")
S("Sierpiński tetrahedron", "Sierpiński四面体")
S("aperiodic spectre", "非周期幽灵")

// inter-geometric portals

S("Welcome to a portal map! Press 'o' to configure.", "欢迎来到传送门地图！按“o”进行设置。")
S("become a portal map", "成为传送门地图")
S("yes, that's what I want", "没错，我就想这样")
S("world list", "世界列表")
S("manage portals", "管理传送门")
S("view another world", "拜访其他世界")
S("connect ", "连接")
S("disconnect this portal", "断开传送门连接")
S("remove %1 from the list", "将%1从列表移除")
S("add to list", "添加至列表")
S("portal orientation", "传送门取向")
S("mirror connection", "镜像连接")
S("test portal here", "在此测试传送门")
S("set recommended settings", "配置推荐设置")
S("height-to-width ratio", "高宽比")

S("keep eye level when walking enabled", "移动时保持眼睛高度已开启")
S("walking eye angle", "行走眼睛角度")
S("0 = looking forward, 90 = looking upward. In VR this is adjusted automatically.",
  "0 = 向前看，90 = 向上看。在VR模式中自动调节。")
S("eye angle scale", "眼睛角度缩放")
S("1 = the angle can be changed with keyboard or mouse movements, 0 = the angle is fixed",
  "1 = 角度可以通过键盘或鼠标移动更改，0 = 角度固定")
S("walking eye level", "行走眼睛高度")
S("Distance from the floor to the eye in the walking mode, in absolute units. In VR this is adjusted automatically.", 
  "行走模式中地面至眼睛的高度，以绝对单位衡量。在VR模式中自动调节。")

S("move the camera with arrow keys and Home/End", "以方向键及Home/End来移动相机")
S("the map is fixed (improves performance)", "地图固定（提高性能）")

S("You are currently in a visualization. Press wasd to scroll, qe to rotate. You can also use the arrow keys. ESC for menu.\n\n",
  "你目前处于可视化中。按wasd来平移，qe旋转。你也可以使用方向键。ESC打开菜单。\n\n")

S("You are currently in a visualization. Press wasdqe to rotate the camera, ijklyh to move. You can also use the arrow keys and Home/End and PgUp/PgDn. ESC for menu.\n\n",
  "你目前处于可视化中。按wasdqe来旋转相机，ijklyh来移动。你也可以使用方向键、Home/End与PgUp/PgDn。ESC打开菜单。\n\n")

S("pure exploration (game keys scroll)", "纯探索 (游戏按键平移)")

// subdivisions of 3D honeycombs

S("sub-cubes", "亚立方")
S("dual sub-cubes", "双亚立方")
S("bitruncated sub-cubes", "截角亚立方")
S("note: more choices in cubic honeycombs", "注意：立方蜂窝中有更多选择")
S("subdivision", "亚切分")
S("split by original faces", "根据原始面分割")
S("split by vertex axes", "根据顶点轴分割")
S("split by midedges", "根据中边分割")
S("Outside of the supported limits", "超出所支持的范围")
// let's just not translate this
S("subcubed", "亚立方")
S("dual-subcubed", "双亚立方")
S("bitruncated-subcubed", "截角亚立方")
S("subdivided", "亚切分")

// restrict map to disk

S("disk size", "碟尺寸")
S("Play on a disk. Enables the special game rules for small bounded spaces (especially relevant for e.g. Minefield and Halloween). "
  "The number given is the number of tiles to use; it is not used exactly, actually the smallest disk above this size is used. Set to 0 to disable.",
  "在碟面上进行游戏。开启有限小空间的特殊游戏规则（对雷区与万圣节等特别有帮助）。此处的数值是使用的地块数；并非正好这么多，实际上会使用尺寸比这个值大的最小的碟面。设为0禁用。")

S("disk shape", "碟形状")
S("distance in tiles", "距离，以地块衡量")
S("distance in vertices", "距离，以顶点衡量")
S("geometric distance", "几何距离")
S("fraction of mine in bounded minefield", "有限雷区中的地雷比例")

// tessellation tree generation

S("strict tree maps", "严格树状图")
S("display distances up to", "最大显示距离")
S("extend automatically", "自动拓展") // the table of tile count by distances
S("rules generated successfully: %1 states using %2-%3 cells", "成功生成规则：%1个态，使用%2-%3个格子")
S("too difficult: %1", "过于困难：%1")
S("bug: %1", "故障：%1")
S(
    "Strict tree maps are generated using a more powerful algorithm.\n\nThis algorithm supports horocycles and knows the expansion rates of various "
    "tessellations (contrary to the basic implementation of Archimedean, tes, and unrectified/warped/untruncated tessellations).\n\nYou can convert mostly any "
    "non-spherical periodic 2D tessellation to strict tree based.\n\nSwitching the map format erases your map.",

    "严格树状图使用更强大的算法生成。\n\n该算法支持极限圆，并了解各种密铺的扩张率（与阿基米德、tes、未修正/扭曲/截角密铺的基础实现不同）\n\n你基本可以把任何非球面2D周期密铺转化为基于严格树。\n\n切换地图格式会清除你的地图。"
  )

S("in tes internal format", "在内部.tes格式中")
S("converted successfully -- %1 cell types", "成功恢复：%1种格子")
S("cannot be disabled for this tiling", "该镶嵌中无法禁用")
S("extended football colorability", "扩展足球上色")
S("consider all symmetries when converting", "转换时考虑所有对称")
S("tes reverse order on convert", "转换时反转tes顺序")
S("maximum cellcount", "最大格子数")
S("controls the max memory usage of conversion algorithm -- the algorithm fails if exceeded",
  "控制转换算法使用的最大内存——超过则算法失败")

// hat tiling

S("hat/spectre/turtle parameter", "帽子/幽灵/乌龟参数")
S("Apeirodic hat tiling based on: https://arxiv.org/pdf/2303.10798.pdf\n\n"
  "This controls the parameter discussed in Section 6. Parameter p is Tile(p, (2-p)√3), "
  "scaled so that the area is the same for every p.\n\n"
  "Aperiodic spectre tiling based on: https://arxiv.org/abs/2305.17743\n\n"
  "In the spectre tiling, set the parameter to 'spectre' value to make all tiles have the same shape.",

  "非周期帽子镶嵌基于：https://arxiv.org/pdf/2303.10798.pdf\n\n"
  "该选项控制第6节中的参数。参数p为Tile(p, (2-p)√3)，"
  "经过缩放使其对于任意p面积相同。\n\n"
  "非周期幽灵镶嵌基于：https://arxiv.org/abs/2305.17743\n\n"
  "在幽灵镶嵌中，将参数设为“幽灵”值来使所有地块具有相同形状。")

S("Welcome to HatRogue!", "欢迎来到HatRogue！")
S("hat in cluster", "成堆帽子")
S("hat clusters", "帽子堆")
S("hat superclusters", "超帽子堆")
S("types (mark reverse)", "类别（标记反转）")
S("display small floors", "显示小地板")
S("chevron (periodic)", "V字（周期）")
S("hat", "帽子")
S("spectre", "幽灵")
S("turtle", "乌龟")
S("comma (periodic)", "逗号（周期）")

S("hat parameter (imaginary)", "帽子参数（虚部）")
S("Imaginary part of the hat parameter. This corresponds to the usual interpretation of complex numbers in Euclidean planar geometry: "
  "rather than shortened or lengthened, the edges are moved in the other dimension.",
  "帽子参数的虚部。这对应于欧几里得几何中复数通常的诠释：边并非伸长或缩短，而是向其他维度移动。"
  )

// fake curvature setting in experiments in geometry (some texts were not translated)

S(
  "This feature lets you construct the same tiling, but "
  "from shapes of different curvature.\n\n"
  "The number you give here is (2D) vertex degree or (3D) "
  "the number of cells around an edge.\n\n",

 "该功能使你可以使用不同曲率的形状构建相同的镶嵌。\n\n"
 "在此处给出顶点角(2D)或边周围的格子数(3D)。\n\n")

S("original", "原件")
S("double original", "双原件")
S("draw all if multiple of original", "多个原件时全部绘制")
S("draw copies (2D only)", "绘制复制(仅2D)")
S("unordered", "无序")
S("pre-ordered", "排序前")
S("post-ordered", "排序后")

// other tessellation-related

S("color by symmetries (reversed tiles marked)", "按对称上色(标记保留地块)")
S("unreversed colors", "未保留颜色")
S("symmetric subdivision", "对称子区分")
S("simplified display of apeirogons", "简化显示无限边形")

S("Width of cell boundaries", "格子边界宽度")
S("How wide should the cell boundaries be.", "格子的边界应当有多宽。")

S("the following sliders will restart the game", "下一页将会重新开始游戏")

// new projections and parameters

S("horocyclic equal-area", "极限圆等面积")
S("conformal square", "保角方形")
S("Lie perspective", "李视角")
S("Lie orthogonal", "李正交")
S("relativistic perspective", "相对论视角")
S("relativistic orthogonal", "相对论正交")
S("angle between the axes", "轴夹角")
S("In two-axe-based models, this parameter gives the angle between the two axes.", "在基于二轴的模型中，该参数给出两个轴之间的夹角。")
S("model orientation 3D", "3D模型取向")
S("semidirect_rendering (perspective on GPU)", "半直接渲染 (GPU处理视角)")
S("this is not a Lie group", "这不是李群")
S("not implemented", "未实现")

S("auto rotation", "自动旋转")
S("auto rotation in 3D", "3D中自动旋转")

// alternative screen projections

S("stereo/high-FOV mode", "立体/广视角模式")
S("linear perspective", "线性视角")
S("for red-cyan glasses", "用于红蓝3D眼镜")
S("for mobile VR", "用于移动端VR")
S("ODS", "ODS")
S("for rendering 360° VR videos (implemented only in raycaster and some other parts)",
  "用于渲染360°VR视频（仅在光线投射等部件中实现）")
S("Panini", "帕尼尼")
S("Projection believed to be used by Italian painters. Allows very high FOV angles while rendering more straight lines as straight than the stereographic projection.",
  "据说是意大利画家使用的投影。使得视野角度极广，相对于球极投影将更多实际的直线渲染为直线。")
S("stereographic", "球极")
S("Stereographic projection allows very high FOV angles.", "球极投影使视野角度极广。")
S("equirectangular", "等矩形")
S("for rendering 360° videos (implemented only in raycaster)", "用于渲染360°视频（仅在光线投射中实现）")
S("stereographic/Panini parameter", "球极/帕尼尼参数")
S("1 for full stereographic/Panini projection. Lower values reduce the effect.\n\n"
  "HyperRogue uses a quick implementation, so parameter values too close to 1 may be buggy (outside of raycasting); try e.g. 0.9 instead.",
  "1为完全球极/帕尼尼投影。较低的值减小效果。\n\n"
  "HyperRogue使用快速方法，特别接近1的参数可能会存在问题（光线投射以外）；可以以0.9代替。"
  )

// embeddings
//============

S("3D embedding method", "3D嵌入模式")
S("3D style", "3D样式")
S("2D engine", "2D引擎")
S("Use HyperRogue's 2D engine to simulate same curvature. Works well in top-down and third-person perspective. The Hypersian Rug mode can be used to project this to a surface.",
  "使用HyperRogue的2D引擎来模拟相同的曲率。适用于俯视及第三人称视角。超波斯地毯模式可用于将该显示投影至一个曲面。")
S("same curvature", "相同曲率")
S("Embed as an equidistant surface in the 3D version of the same geometry.",
  "在相同几何的3D版中嵌入为等距面。")
S("lower curvature", "低曲率")
S("Embed as a surface in a space of lower curvature.", "在较低曲率的空间中嵌入为曲面。")
S("much lower curvature", "超低曲率")
S("Embed sphere as a sphere in hyperbolic space.", "将球作为球嵌入双曲空间。")
S("product", "积")
S("Add one extra dimension in the Euclidean way.", "以欧几里得方式增加一个维度。")
S("Embed Euclidean plane into Nil.", "将欧几里得平面嵌入Nil。")
S("Embed Euclidean or hyperbolic plane into Sol.", "将欧几里得或双曲平面嵌入Sol。")
S("Embed Euclidean or hyperbolic plane into stretched hyperbolic geometry.", "将欧几里得或双曲平面嵌入拉伸双曲几何。")
S("stretched Sol", "拉伸Sol。")
S("Embed Euclidean or hyperbolic plane into stretched Sol geometry.", "将欧几里得或双曲平面嵌入拉伸Sol。")
S("Clifford Torus", "Clifford环面")
S("Embed Euclidean rectangular torus into S3.", "将欧几里得矩形环面嵌入S3。")
S("hyperbolic product", "双曲积")
S("Embed Euclidean or hyperbolic plane in the H2xR product space.", "将欧几里得或双曲平面嵌入H2xR积空间。")
S("spherical product", "球面积")
S("Embed Euclidean cylinder or spherical plane in the H2xR product space.", "将欧几里得圆柱或球面嵌入H2xR积空间。")
S("Embed Euclidean plane in twisted product geometry.", "将欧几里得平面嵌入扭转积几何。")
S("Embed Euclidean cylinder in Euclidean space.", "将欧几里得圆柱嵌入欧几里得空间。")
S("hyperbolic cylinder", "双曲圆柱")
S("Embed Euclidean cylinder in hyperbolic space.", "将欧几里得圆柱嵌入双曲空间。")
S("product cylinder", "圆柱积")
S("Embed Euclidean cylinder in H2xR space.", "将欧几里得圆柱嵌入H2xR空间。")
S("Nil cylinder", "Nil圆柱")
S("Embed Euclidean cylinder in Nil.", "将欧几里得圆柱嵌入Nil.")
S("horocylinder", "极限圆柱")
S("Embed Euclidean as a horocylinder in H2xR space.", "将欧几里得作为极限圆柱嵌入H2xR空间。")
S("SL2 cylinder", "SL2圆柱")
S("Embed Euclidean as a cylinder in twisted product geometry.", "将欧几里得作为圆柱嵌入扭转积几何。")

// embedding settings

S("flat, not equidistant", "平坦，非等距")
S("invert convex/concave", "反转凹凸")

// embedding errors

S("set square tiling", "设置方形镶嵌")
S("set hex tiling", "设置六边形镶嵌")
S("error: currently works only in PURE Euclidean regular square or hex tiling", "错误：当前仅在纯欧几里得规则方形或六边形镶嵌中起效")
S("error: currently works only in pure Euclidean, or binary tiling and similar", "错误：当前仅在纯欧几里得或二元镶嵌等中起效")
S("set binary tiling variant", "设置二元镶嵌变体")
S("set ternary tiling", "设置三元镶嵌")
S("set binary tiling", "设置二元镶嵌")
S("error: this embedding does not work in shmup", "错误：该镶嵌在射击模式中无效")
S("error: this method works only in rectangular torus", "错误：该方法仅在矩形环面中起效")
S("set 20x20 torus", "设置20x20环面")
S("error: this method works only in cylinder", "错误：该方法仅在圆柱中起效")
S("set cylinder", "设置圆柱体")
S("error: not supported", "错误：nie działa")
S("error: this method does not work in spherical geometry", "错误：该方法在球面几何中无效")
S("error: this method does not work in hyperbolic geometry", "错误：该方法在双曲几何中无效")
S("3D styles", "3D式样")
S("3D detailed settings", "3D细节设置")
S("more options in 3D engine", "3D引擎中有更多选项")
S("configure Hypersian Rug", "配置超波斯地毯")
S("view shift for embedded planes", "观察嵌入面平移")
S("always move on geodesics", "总在测地线上移动")
S("keep levels", "保持高度")
S("keep the vertical angle of the camera", "保持相机垂直角度")
S("mixed", "混合")
S("on geodesics when moving camera manually, keep level when auto-centering", "在测地线上手动移动相机自动居中时保持高度")
S("Euclidean embedding rotation", "欧几里得嵌入旋转")
S("How to rotate the Euclidean embedding, in degrees.", "如何旋转欧几里得嵌入，以角度为单位。")
S("Euclidean embedding scale", "欧几里得嵌入缩放")
S("How to scale the Euclidean map, relatively to the 3D absolute unit.", "如何缩放欧几里得映射，相对于3D绝对单位。")
S("Euclidean embedding scale Y/X", "欧几里得嵌入缩放 Y/X")
S("This scaling factor affects only the Y coordinate.", "该缩放参数仅作用于Y坐标。")
S("(fix errors)", "(修复故障)")
S("reset view", "重设视野")
S("needs", "需要 ")

// embeddings: detailed 3D parameters

S("set 3D settings automatically", "自动3D设置")

S("draw plain floors in 3D", "3D绘制地板")
S("floor alpha", "地板alpha")
S("255 = opaque", "255 = 不透明")

S("altitude of the stars", "星星海拔")
S("star probability", "星星概率")
S("probability of star per tile", "每格星星概率")
S("night star size (relative to item sizes)", "夜星尺寸（相对于物品）")
S("sun size (relative to item sizes)", "太阳尺寸（相对于物品）")
S("infinite sky", "无限天空")
S("ratio of high walls to normal walls", "高墙与普通墙比例")
S("ratio of very high walls to normal walls", "超高墙与普通墙比例")
S("altitude of the sky", "天空海拔")
S("sky fake height", "天空虚假高度")
S("Sky is rendered at the distance computed based on the sky height, "
  "which might be beyond the range visible in fog. To prevent this, "
  "the intensity of the fog effect depends on the value here rather than the actual distance. Stars are affected similarly.",

  "天空基于天空高度计算出的距离进行渲染，也许会超过雾中的可见范围。为了防止这个问题发生，雾效果的强度基于该值而非实际距离。星星也受到近似效果影响。"
  )
S("sky rendering", "天空渲染")
S("do not draw sky", "不绘制天空")
S("skybox", "天空盒")
S("infinite depth", "无限深度")
S("The unit this value is given in is wall height. "
  "Note that, in exponentially expanding spaces, too high values could cause rendering issues. "
  "So if you want infinity, values of 5 or similar should be used -- there is no visible difference from infinity and glitches are avoided.",
  "该数值的单位为墙高度。注意在指数级增长的空间中，数值太高会产生渲染问题。如果你想要无限大，大概5就行——和无限大没有明显区别，也不会出故障。")
S("Level of shallow water", "浅水层数")
S("do not render higher levels if camera too high", "相机过高时不渲染高处")
S("works only in Euclidean", "仅在欧几里得中可用")
S("always", "总是")
S("might be glitched in some settings", "在有些设置下可能会出故障")
S("prevent exceeding recommended altitudes", "禁止超过推荐海拔")

S("camera angle", "相机角度")
S("render behind the camera", "在相机后渲染")

// embeddings: pseudohedral setting

S("make the tiles flat", "平整格子")
S("the tiles are curved", "弯曲格子")
S("inscribed", "标记")
S("the tiles are inscribed", "地块已标记")
S("circumscribed", "环标记")
S("the tiles are circumscribed", "地块已环标记")
S("depth bonus in pseudohedral", "伪体额外深度")

// embeddings: geodesic movement in Solv etc

S("straight lines", "直线")
S("Lie group", "李群")
S("light, camera, and objects move in lines of constant direction, in the Lie group sense", "光、相机与物体沿李群意义上的同一方向行进")
S("geodesics", "测地线")
S("light, camera, and objects always take the shortest path", "光、相机与物体总是沿最短路径行进")

// achievement eligiblity
S("achievement/leaderboard eligiblity:", "成就/排行榜可用：")
S("eligible for most", "大多数成就可解锁")
S("eligible for most -- leaderboards separate", "大多数成就可解锁——单独排行榜")
S("eligible for racing", "竞速模式可解锁")
S("eligible for shmup", "射击模式可解锁")
S("eligible for multiplayer", "多人游戏可解锁")
S("eligible for Chaos mode", "混沌模式可解锁")
S("eligible for Princess Challenge", "公主挑战可解锁")
S("eligible for heptagonal", "七边形可解锁")
S("eligible for special geometry", "特殊几何可解锁")
S("eligible for Strange Challenge", "奇异挑战可解锁")
S("disabled in cheat mode", "作弊模式中不可解锁")
S("disabled in casual mode", "轻松模式中不可解锁")
S("not eligible due to current mode settings", "由于当前模式设置不可解锁")
S("no achievements/leaderboards in this version", "该版本中无成就/排行榜")
