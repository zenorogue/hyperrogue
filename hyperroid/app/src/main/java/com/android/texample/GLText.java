// taken from Texample,
// http://fractiousg.blogspot.com/201/4/rendering-text-in-opengl-on-android.html

// Zeno Rogue's modifications (for HyperRogue 4.2):
// * to provide a Typeface as an argument, instead of Assets
// * drawAlign to provide alignment in HyperRogue-compatible fashion 
// For HyperRogue 5.3:
// * Unicode characters

// This is a OpenGL ES 1.0 dynamic font rendering system. It loads actual font
// files, generates a font map (texture) from them, and allows rendering of
// text strings.
//
// NOTE: the rendering portions of this class uses a sprite batcher in order
// provide decent speed rendering. Also, rendering assumes a BOTTOM-LEFT
// origin, and the (x,y) positions are relative to that, as well as the
// bottom-left of the string to render.

package com.android.texample;

import android.opengl.GLES20;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.opengl.GLUtils;

public class GLText {

   // 
   StringBuffer allchars;
   
   void buildAllchars() {
     allchars = new StringBuffer();
     for(char c=32; c<=126; c++)
       allchars.append(c);
     allchars.append("¨°²´½ÀÁÄÇÈÉÊÍÎÖÚÜßàáâãäçèéêìíîïòóôõöøùúûüýĄąćČčĎďĘęĚěğİıŁłńňŒœŘřŚśŞşŠšŤťůŹźŻżŽžΘδλπϕЁАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюяёᵈᶻ—’“”…⁻←→√∞⌫⏎、。一丁七万三上下不与专且世丘业丛东丢两严个中临为主丽举乃久么义之乌乍乎乐乘乞也习乡乱了予争事二于亏云互五亚些亡交亦产享京亮亲人什仅仆仇今介仍从他付仙代令以们仰件价任份仿伊伏伐休优伙会传伤伪估伴伸似但位低住体何余作你佩佳使例供依侠侣侧侵便俊俏俑俗保信修俯倍倒候借值倾假偏做停健偶偷储像僧僵儿允元充先光克免入全八公六共关兴兵其具典兼兽内再冒军冠冬冰冲决况冷冻净准凉凌减几凡凤凰凶凸凹出击函刀刃分切划列则创初删利别到制刷刺刻前剑剥剩剪副割力劝办功加务劣动助劫励劳势勒包匕化北匙匠匪匹区十千升午半单南博占卡卫危即却卷厅历压厚原去参又叉及友双反发取受变叙叠口古句另只叫召可台史右叶号吃各合同名后吐向吓吗吞否吧含听启吱吸吹吻吼呀呃呆呈告员呢周味呼命和咒咬品哇响哎哦哪唉唤唯啃商啊善喜喝喷嗯嘘嘟嘲器囊囔四回因团园困围固国图圆圈土圣在地场均坍坏坐坑块坚坛坟坠坡坦垂型垫埃埋城域基堆堡堵塌塔填境墓墙增墟墨壁士声壳处备复外多夜够大天太夫央失头夹夺奇契奔奖套女她好如妖妙妹姆姊始姿威婆婪子字存孤学孩孵宁它宇守安完官宙定宝实宠客宣室宫害家容宽寂密富寒寝察寸对寻导寿封射将尊小少尔尖尝尤就尸尺尼尽尾局层居屏展属屠山屹屿岛岩岸峡崩崽嵌巢工左巧巨巫差己已巴市布帅师希帕帝带帧帮常帽幅幕干平年并幸幻幼幽广庆序库应底庞废度座康延建开异弃弄弊式弓引张弦弧弩弯弱弹强归当录形彩彭影彼往征径待很律徒得御微德心必忆忍忘快念忽怀态怎怒怕怜思急性怪总恋恐恒恢恨恩恭息恰恶恼悬悲情惊惑惜惧惩惯想惹愈愉意愚感愤愿慎慕慢慧憎懂戈戏成我或战截戳戴户所扁扇手才扎扑打扔托执扩扫扭扰找技抉把抓投抗折抛护报抬抵押抽担拉拓拔拖招拜拟拥拦拨择括拯拱拼拿持指按挑挖挡挥挪振捅捕损捡换捧据捶捷掉掌排掘掠探接控推掩掷描提握揭搁搅搜搞摇摧摸撇撑撞撤播擅操擎擦攀支收改攻放故效敌敏救教敢散敬数敲整文斐斑斗料斜斥斩断斯新方施旁旅旋族无既日旦旧早时昂昆明易星映春是显晕普景晶智暂暗暴曲更曾替最月有朋服望朝期木未末本术朵机杀杂杆杈李材束条来杯松板极构林枚果枝枪架枷柄某染柜查柱标栏树株样核根格框案桌档桥桩桶梁梅梦梭梯检棋棒棕棘森棵植椭楚楼概榜榴槛模横次欢欧款歇止正此步武死殆殊残殖段殿毁母每毒比毕毙毛毫毯民气水永求汇汞池汲汽沃沉沌沙没河油治沿泄泊法泡波注泳洋洗洛洞活流浅浆测浏浪浮海涂消涉涌涡涨液深混淹添清渊渐渔渗渡温港渲游湖湾湿溃源溶溺滑滚满滤滩漂演漠漩漫潜潮激灌火灭灯灰灵灼炎炫炸点炼烁烂烈烦烧烫热焦焰然照熄熔熟燃燥爆爪爬爱片版牙牛牢物牲特牺犬状狂狐狗狩独狭狱狼猎猛猜猫猬猿率王玛玩玫环现玻珀珊珍珠球理琥瑙瑚瑰璃瓜瓣瓶甚甜生用甩由甲电男画畅界留略疆疏疑疫疲瘟登白百的皇皮盈盏监盒盔盖盗盘盟目盯盲直相盾省看真眠眩眼着睛睡瞄瞥瞳矢知矩短石矿码砂砍研破砸础硕硫硬确碍碎碟碰磺示礼祝神祭禁离种科秒秘秩积称移稀程稍稔稳穴究穷空穿窃窄窒窗窝立竖站竞竟竭端笑笔符第笼等筑策简算管箭篝米类粒粗粘粹精糖糟系素索紧紫繁纠红纤约级纪纬纯纷纸纹线练组细终绍经绑结绕绘给绝统继绩续维绵绿缓编缘缚缝缟缤缩缺网罗罚罢罩置美群羽翁翻耀老考者而耍耐耗耶耸聊职聪肆股肤肫肯胁背胖胜胞能脆脉脑脚脱腐腾腿自至致舌舍舞舟航般船艇艘艰色艺节花苏苗若苦英苹茂范茹荆草荐荒荡荣药荷莫莱莲获莽菊菜菱营落著葬蒸蓝蔓蕨藏藤虎虐虑虚虫虹虻虽蚁蚂蛇蛋蛙蜂蜜蜥蜴蝙蝠蝴蝶螂融螺蟑蠕蠢血行衍衡衣表被袭裁裂装裔裹西要覆见观规视览觉觊觎角解触言誓警计认讨让训记许论讽设访证诅识诉译试话诠诡该详语误诱说请诺读谁调谅谈谊谐谓谜谢谷象貌贝负财败货质贪贴贵贸费贺贼资赋赏赖赛赢赤走起超越趣足跃跌跑跖距跟跨路跳踩踪身躯躲车转轮轴轻较辐辑输辨边达迅过迎运近返还这进远违连迟迫迭述迷迹追退送适逃逆选透逐途逗通速造逼遇遍道遗遥遭遵避那邪邻部都配酒酷酸酿醒采释里重野量金鉴针钓钥钩钮钱钻铀铁铜铠铭银铺销锁错锥键锯镇镜镶长门闪闭问闲间闵闹闻阅阈阔队阱防阳阴阵阶阻阿附际陆降限除陨险陪陵陷随隐隔隙障难雀雁雄集雉雏雕雪雷雾需霆震霉霓霜露青静非靠面革韧音页顶项顺须预领频颗题颜额风飞食餮饕饥饮饰饱馈首香马驯驱驶驻驾验骑骗骨骰骷骸髅高鬼魂魔鱼鲁鲜鲨鳞鸟鸡鸣鸥鸦鸽鹉鹦鹰麻黄黏黑默鼓鼠齐齿龙龟！（），：；？�");
     }

   int numChars() {
     return allchars.length() + 1;
     }

   int charIndex(char c) {
     int l = allchars.length();
     for(int i=0; i<l; i++) 
       if(allchars.charAt(i) == c)
         return i;
     return allchars.length();
     }
   
   //--Constants--//
   public final static char CHAR_NONE = '?';          // Character to Use for Unknown (ASCII Code)

   public final static int FONT_SIZE_MIN = 6;         // Minumum Font Size (Pixels)
   public final static int FONT_SIZE_MAX = 180;       // Maximum Font Size (Pixels)

   public final static int CHAR_BATCH_SIZE = 100;     // Number of Characters to Render Per Batch

   //--Members--//
   public Shader shader;                              // GL10 Instance
   AssetManager assets;                               // Asset Manager
   SpriteBatch batch;                                 // Batch Renderer

   int fontPadX, fontPadY;                            // Font Padding (Pixels; On Each Side, ie. Doubled on Both X+Y Axis)

   float fontHeight;                                  // Font Height (Actual; Pixels)
   float fontAscent;                                  // Font Ascent (Above Baseline; Pixels)
   float fontDescent;                                 // Font Descent (Below Baseline; Pixels)

   int textureId;                                     // Font Texture ID [NOTE: Public for Testing Purposes Only!]
   int textureSize;                                   // Texture Size for Font (Square) [NOTE: Public for Testing Purposes Only!]
   TextureRegion textureRgn;                          // Full Texture Region

   float charWidthMax;                                // Character Width (Maximum; Pixels)
   float charHeight;                                  // Character Height (Maximum; Pixels)
   final float[] charWidths;                          // Width of Each Character (Actual; Pixels)
   TextureRegion[] charRgn;                           // Region of Each Character (Texture Coordinates)
   int cellWidth, cellHeight;                         // Character Cell Width/Height
   int rowCnt, colCnt;                                // Number of Rows/Columns

   float scaleX, scaleY;                              // Font Scale (X,Y Axis)
   float spaceX;                                      // Additional (X,Y Axis) Spacing (Unscaled)


   //--Constructor--//
   // D: save GL instance + asset manager, create arrays, and initialize the members
   // A: gl - OpenGL ES 10 Instance
   public GLText(Shader sh) {
      this.shader = sh;                                   // Save the GL10 Instance

      buildAllchars();

      batch = new SpriteBatch( sh, CHAR_BATCH_SIZE );  // Create Sprite Batch (with Defined Size)

      charWidths = new float[numChars()];               // Create the Array of Character Widths
      charRgn = new TextureRegion[numChars()];          // Create the Array of Character Regions

      // initialize remaining members
      fontPadX = 0;
      fontPadY = 0;

      fontHeight = 0.0f;
      fontAscent = 0.0f;
      fontDescent = 0.0f;

      textureId = -1;
      textureSize = 0;

      charWidthMax = 0;
      charHeight = 0;

      cellWidth = 0;
      cellHeight = 0;
      rowCnt = 0;
      colCnt = 0;

      scaleX = 1.0f;                                  // Default Scale = 1 (Unscaled)
      scaleY = 1.0f;                                  // Default Scale = 1 (Unscaled)
      spaceX = 0.0f;
   }

   //--Load Font--//
   // description
   //    this will load the specified font file, create a texture for the defined
   //    character range, and setup all required values used to render with it.
   // arguments:
   //    file - Filename of the font (.ttf, .otf) to use. In 'Assets' folder.
   //    size - Requested pixel size of font (height)
   //    padX, padY - Extra padding per character (X+Y Axis); to prevent overlapping characters.
   public boolean load(Typeface tf, int size, int padX, int padY) {

      // setup requested values
      fontPadX = padX;                                // Set Requested X Axis Padding
      fontPadY = padY;                                // Set Requested Y Axis Padding

      // load the font and setup paint instance for drawing
      // Typeface tf = Typeface.createFromAsset( assets, file );  // Create the Typeface from Font File
      Paint paint = new Paint();                      // Create Android Paint Instance
      paint.setAntiAlias( true );                     // Enable Anti Alias
      paint.setTextSize( size );                      // Set Text Size
      paint.setColor( 0xffffffff );                   // Set ARGB (White, Opaque)
      // paint.setTypeface( tf );                        // Set Typeface
      paint.setTypeface(tf);

      // get font metrics
      Paint.FontMetrics fm = paint.getFontMetrics();  // Get Font Metrics
      fontHeight = (float)Math.ceil( Math.abs( fm.bottom ) + Math.abs( fm.top ) );  // Calculate Font Height
      fontAscent = (float)Math.ceil( Math.abs( fm.ascent ) );  // Save Font Ascent
      fontDescent = (float)Math.ceil( Math.abs( fm.descent ) );  // Save Font Descent

      // determine the width of each character (including unknown character)
      // also determine the maximum character width
      char[] s = new char[2];                         // Create Character Array
      charWidthMax = charHeight = 0;                  // Reset Character Width/Height Maximums
      float[] w = new float[2];                       // Working Width Value
      int cnt = 0;                                    // Array Counter
      for ( int i=0; i<allchars.length(); i++ )  {   // FOR Each Character
         s[0] = allchars.charAt(i);                   // Set Character
         paint.getTextWidths( s, 0, 1, w );           // Get Character Bounds
         charWidths[cnt] = w[0];                      // Get Width
         if ( charWidths[cnt] > charWidthMax )        // IF Width Larger Than Max Width
            charWidthMax = charWidths[cnt];           // Save New Max Width
         cnt++;                                       // Advance Array Counter
      }
      s[0] = CHAR_NONE;                               // Set Unknown Character
      paint.getTextWidths( s, 0, 1, w );              // Get Character Bounds
      charWidths[cnt] = w[0];                         // Get Width
      if ( charWidths[cnt] > charWidthMax )           // IF Width Larger Than Max Width
         charWidthMax = charWidths[cnt];              // Save New Max Width
      cnt++;                                          // Advance Array Counter

      // set character height to font height
      charHeight = fontHeight;                        // Set Character Height

      // find the maximum size, validate, and setup cell sizes
      cellWidth = (int)charWidthMax + ( 2 * fontPadX );  // Set Cell Width
      cellHeight = (int)charHeight + ( 2 * fontPadY );  // Set Cell Height
      int maxSize = cellWidth > cellHeight ? cellWidth : cellHeight;  // Save Max Size (Width/Height)
      if ( maxSize < FONT_SIZE_MIN || maxSize > FONT_SIZE_MAX )  // IF Maximum Size Outside Valid Bounds
         return false;                                // Return Error

      // set texture size based on max font size (width or height)
      // NOTE: these values are fixed, based on the defined characters. when
      // changing start/end characters (CHAR_START/CHAR_END) this will need adjustment too!
      if ( maxSize <= 24 )                            // IF Max Size is 18 or Less
         textureSize = 256;                           // Set 256 Texture Size
      else if ( maxSize <= 40 )                       // ELSE IF Max Size is 40 or Less
         textureSize = 512;                           // Set 512 Texture Size
      else if ( maxSize <= 80 )                       // ELSE IF Max Size is 80 or Less
         textureSize = 1024;                          // Set 1024 Texture Size
      else                                            // ELSE IF Max Size is Larger Than 80 (and Less than FONT_SIZE_MAX)
         textureSize = 2048;                          // Set 2048 Texture Size
      textureSize = 4096; /* needed for Chinese */

      // create an empty bitmap (alpha only)
      Bitmap bitmap = Bitmap.createBitmap( textureSize, textureSize, Bitmap.Config.ARGB_8888 );  // Create Bitmap
      Canvas canvas = new Canvas( bitmap );           // Create Canvas for Rendering to Bitmap
      bitmap.eraseColor( 0x00000000 );                // Set Transparent Background (ARGB)

      // calculate rows/columns
      // NOTE: while not required for anything, these may be useful to have :)
      colCnt = textureSize / cellWidth;               // Calculate Number of Columns
      rowCnt = (int)Math.ceil( (float)numChars() / (float)colCnt );  // Calculate Number of Rows

      // render each of the characters to the canvas (ie. build the font map)
      float x = fontPadX;                             // Set Start Position (X)
      float y = ( cellHeight - 1 ) - fontDescent - fontPadY;  // Set Start Position (Y)
      for (int i=0; i<allchars.length(); i++) {       // FOR Each Character
         s[0] = allchars.charAt(i);                   // Set Character to Draw
         canvas.drawText( s, 0, 1, x, y, paint );     // Draw Character
         x += cellWidth;                              // Move to Next Character
         if ( ( x + cellWidth - fontPadX ) > textureSize )  {  // IF End of Line Reached
            x = fontPadX;                             // Set X for New Row
            y += cellHeight;                          // Move Down a Row
         }
      }
      s[0] = CHAR_NONE;                               // Set Character to Use for NONE
      canvas.drawText( s, 0, 1, x, y, paint );        // Draw Character

      // generate a new texture
      int[] textureIds = new int[1];                  // Array to Get Texture Id
      GLES20.glGenTextures( 1, textureIds, 0 );           // Generate New Texture
      textureId = textureIds[0];                      // Save Texture Id

      // setup filters for texture
      GLES20.glBindTexture( GLES20.GL_TEXTURE_2D, textureId );  // Bind Texture
      GLES20.glTexParameterf( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST );  // Set Minification Filter
      GLES20.glTexParameterf( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR );  // Set Magnification Filter
      GLES20.glTexParameterf( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE );  // Set U Wrapping
      GLES20.glTexParameterf( GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE );  // Set V Wrapping

      // load the generated bitmap onto the texture
      GLUtils.texImage2D( GLES20.GL_TEXTURE_2D, 0, bitmap, 0 );  // Load Bitmap to Texture
      GLES20.glBindTexture( GLES20.GL_TEXTURE_2D, 0 );      // Unbind Texture

      // release the bitmap
      bitmap.recycle();                               // Release the Bitmap

      // setup the array of character texture regions
      x = 0;                                          // Initialize X
      y = 0;                                          // Initialize Y
      for ( int c = 0; c < numChars(); c++ )  {       // FOR Each Character (On Texture)
         charRgn[c] = new TextureRegion( textureSize, textureSize, x, y, cellWidth-1, cellHeight-1 );  // Create Region for Character
         x += cellWidth;                              // Move to Next Char (Cell)
         if ( x + cellWidth > textureSize )  {
            x = 0;                                    // Reset X Position to Start
            y += cellHeight;                          // Move to Next Row (Cell)
         }
      }

      // create full texture region
      textureRgn = new TextureRegion( textureSize, textureSize, 0, 0, textureSize, textureSize );  // Create Full Texture Region

      // return success
      return true;                                    // Return Success
   }

   //--Begin/End Text Drawing--//
   // D: call these methods before/after (respectively all draw() calls using a text instance
   //    NOTE: color is set on a per-batch basis, and fonts should be 8-bit alpha only!!!
   // A: red, green, blue - RGB values for font (default = 1.0)
   //    alpha - optional alpha value for font (default = 1.0)
   // R: [none]
   public void begin()  {
      begin( 1.0f, 1.0f, 1.0f, 1.0f );                // Begin with White Opaque
   }
   public void begin(float alpha)  {
      begin( 1.0f, 1.0f, 1.0f, alpha );               // Begin with White (Explicit Alpha)
   }
   public void begin(float red, float green, float blue, float alpha)  {
      shader.setColor( red, green, blue, alpha );        // Set Color+Alpha
      GLES20.glBindTexture( GLES20.GL_TEXTURE_2D, textureId );  // Bind the Texture
      batch.beginBatch();                             // Begin Batch
   }
   public void end()  {
      batch.endBatch();                               // End Batch
   }

   //--Draw Text--//
   // D: draw text at the specified x,y position
   // A: text - the string to draw
   //    x, y - the x,y position to draw text at (bottom left of text; including descent)
   // R: [none]
   public void draw(String text, float x, float y)  {
      float chrHeight = cellHeight * scaleY;          // Calculate Scaled Character Height
      float chrWidth = cellWidth * scaleX;            // Calculate Scaled Character Width
      int len = text.length();                        // Get String Length
      x += ( chrWidth / 2.0f ) - ( fontPadX * scaleX );  // Adjust Start X
      y += ( chrHeight / 2.0f ) - ( fontPadY * scaleY );  // Adjust Start Y
      for ( int i = 0; i < len; i++ )  {              // FOR Each Character in String
         int c = charIndex(text.charAt( i ));  // Calculate Character Index (Offset by First Char in Font)
         batch.drawSprite( x, y, chrWidth, chrHeight, charRgn[c] );  // Draw the Character
         x += ( charWidths[c] + spaceX ) * scaleX;    // Advance X Position by Scaled Character Width
      }
   }

   //--Draw Text Centered--//
   // D: draw text CENTERED at the specified x,y position
   // A: text - the string to draw
   //    x, y - the x,y position to draw text at (bottom left of text)
   // R: the total width of the text that was drawn
   public float drawC(String text, float x, float y)  {
      float len = getLength( text );                  // Get Text Length
      draw( text, x - ( len / 2.0f ), y - ( getCharHeight() / 2.0f ) );  // Draw Text Centered
      return len;                                     // Return Length
   }
   public float drawCX(String text, float x, float y)  {
      float len = getLength( text );                  // Get Text Length
      draw( text, x - ( len / 2.0f ), y );            // Draw Text Centered (X-Axis Only)
      return len;                                     // Return Length
   }
   public void drawCY(String text, float x, float y)  {
      draw( text, x, y - ( getCharHeight() / 2.0f ) );  // Draw Text Centered (Y-Axis Only)
   }

   public void drawAlign(String text, float x, float y, int align)  {
	   float len = getLength( text );                  // Get Text Length
	   draw( text, x - len * (align / 16.0f), y - ( getCharHeight() / 2.0f ) );  // Draw Text Centered (Y-Axis Only)
	   }

   //--Set Scale--//
   // D: set the scaling to use for the font
   // A: scale - uniform scale for both x and y axis scaling
   //    sx, sy - separate x and y axis scaling factors
   // R: [none]
   public void setScale(float scale)  {
      scaleX = scaleY = scale;                        // Set Uniform Scale
   }
   public void setScale(float sx, float sy)  {
      scaleX = sx;                                    // Set X Scale
      scaleY = sy;                                    // Set Y Scale
   }

   //--Get Scale--//
   // D: get the current scaling used for the font
   // A: [none]
   // R: the x/y scale currently used for scale
   public float getScaleX()  {
      return scaleX;                                  // Return X Scale
   }
   public float getScaleY()  {
      return scaleY;                                  // Return Y Scale
   }

   //--Set Space--//
   // D: set the spacing (unscaled; ie. pixel size) to use for the font
   // A: space - space for x axis spacing
   // R: [none]
   public void setSpace(float space)  {
      spaceX = space;                                 // Set Space
   }

   //--Get Space--//
   // D: get the current spacing used for the font
   // A: [none]
   // R: the x/y space currently used for scale
   public float getSpace()  {
      return spaceX;                                  // Return X Space
   }

   //--Get Length of a String--//
   // D: return the length of the specified string if rendered using current settings
   // A: text - the string to get length for
   // R: the length of the specified string (pixels)
   public float getLength(String text) {
      float len = 0.0f;                               // Working Length
      int strLen = text.length();                     // Get String Length (Characters)
      for ( int i = 0; i < strLen; i++ )  {           // For Each Character in String (Except Last
         int c =  charIndex(text.charAt( i ));        // Calculate Character Index (Offset by First Char in Font)
         len += ( charWidths[c] * scaleX );           // Add Scaled Character Width to Total Length
      }
      len += ( strLen > 1 ? ( ( strLen - 1 ) * spaceX ) * scaleX : 0 );  // Add Space Length
      return len;                                     // Return Total Length
   }

   //--Get Width/Height of Character--//
   // D: return the scaled width/height of a character, or max character width
   //    NOTE: since all characters are the same height, no character index is required!
   //    NOTE: excludes spacing!!
   // A: chr - the character to get width for
   // R: the requested character size (scaled)
   public float getCharWidth(char chr)  {
      return ( charWidths[charIndex(chr)] * scaleX );              // Return Scaled Character Width
   }
   public float getCharWidthMax()  {
      return ( charWidthMax * scaleX );               // Return Scaled Max Character Width
   }
   public float getCharHeight() {
      return ( charHeight * scaleY );                 // Return Scaled Character Height
   }

   //--Get Font Metrics--//
   // D: return the specified (scaled) font metric
   // A: [none]
   // R: the requested font metric (scaled)
   public float getAscent()  {
      return ( fontAscent * scaleY );                 // Return Font Ascent
   }
   public float getDescent()  {
      return ( fontDescent * scaleY );                // Return Font Descent
   }
   public float getHeight()  {
      return ( fontHeight * scaleY );                 // Return Font Height (Actual)
   }

   //--Draw Font Texture--//
   // D: draw the entire font texture (NOTE: for testing purposes only)
   // A: width, height - the width and height of the area to draw to. this is used
   //    to draw the texture to the top-left corner.
   public void drawTexture(int width, int height)  {
      batch.beginBatch( textureId );                  // Begin Batch (Bind Texture)
      batch.drawSprite( textureSize / 2, height - ( textureSize / 2 ), textureSize, textureSize, textureRgn );  // Draw
      batch.endBatch();                               // End Batch
   }


}
