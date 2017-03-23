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

import javax.microedition.khronos.opengles.GL10;

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
     allchars.append("°´ÁÄÇÉÍÎÖÚÜßáâäçèéìíîóöøùúüýąćČčĎďĘęĚěğİıŁłńňŘřŚśŞşŠšťůŹźŻżŽžϕЁАБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЬЭЮЯабвгдежзийклмнопрстуфхцчшщъыьэюяё");
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
   GL10 gl;                                           // GL10 Instance
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
   public GLText(GL10 gl) {
      this.gl = gl;                                   // Save the GL10 Instance

      buildAllchars();

      batch = new SpriteBatch( gl, CHAR_BATCH_SIZE );  // Create Sprite Batch (with Defined Size)

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

      // create an empty bitmap (alpha only)
      Bitmap bitmap = Bitmap.createBitmap( textureSize, textureSize, Bitmap.Config.ALPHA_8 );  // Create Bitmap
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
      gl.glGenTextures( 1, textureIds, 0 );           // Generate New Texture
      textureId = textureIds[0];                      // Save Texture Id

      // setup filters for texture
      gl.glBindTexture( GL10.GL_TEXTURE_2D, textureId );  // Bind Texture
      gl.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MIN_FILTER, GL10.GL_NEAREST );  // Set Minification Filter
      gl.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_MAG_FILTER, GL10.GL_LINEAR );  // Set Magnification Filter
      gl.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_S, GL10.GL_CLAMP_TO_EDGE );  // Set U Wrapping
      gl.glTexParameterf( GL10.GL_TEXTURE_2D, GL10.GL_TEXTURE_WRAP_T, GL10.GL_CLAMP_TO_EDGE );  // Set V Wrapping

      // load the generated bitmap onto the texture
      GLUtils.texImage2D( GL10.GL_TEXTURE_2D, 0, bitmap, 0 );  // Load Bitmap to Texture
      gl.glBindTexture( GL10.GL_TEXTURE_2D, 0 );      // Unbind Texture

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
      gl.glColor4f( red, green, blue, alpha );        // Set Color+Alpha
      gl.glBindTexture( GL10.GL_TEXTURE_2D, textureId );  // Bind the Texture
      batch.beginBatch();                             // Begin Batch
   }
   public void end()  {
      batch.endBatch();                               // End Batch
      gl.glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );         // Restore Default Color/Alpha
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
