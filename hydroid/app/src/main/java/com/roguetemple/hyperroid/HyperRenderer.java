package com.roguetemple.hyperroid;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

// import android.graphics.Paint;
// import android.graphics.Path;
import android.graphics.Typeface;
import android.opengl.GLSurfaceView;
// bimport android.widget.Toast;

import com.android.texample.GLText;

public class HyperRenderer implements GLSurfaceView.Renderer {
	
       int width, height;

	HyperRogue game;

	private GLText glText;                             // A GLText Instance		

	int [] graphdata;
	int gdpos;
	int gdpop() { return graphdata[gdpos++]; }
	
	public void onDrawFrame(GL10 gl) {
	  if(game.forceCanvas) return;
  	  gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
	  synchronized(game) {
		game.hv.updateGame();
		game.draw();
        graphdata = game.loadMap();
	    }

         if(graphdata == null) return;

      // Set to ModelView mode
      gl.glMatrixMode( GL10.GL_MODELVIEW );           // Activate Model View Matrix
      gl.glLoadIdentity();                            // Load Identity Matrix

      // enable texture + alpha blending
      // NOTE: this is required for text rendering! we could incorporate it into
      // the GLText class, but then it would be called multiple times (which impacts performance).
      gl.glEnable( GL10.GL_TEXTURE_2D );              // Enable Texture Mapping
      gl.glEnable( GL10.GL_BLEND );                   // Enable Alpha Blend
      gl.glBlendFunc( GL10.GL_SRC_ALPHA, GL10.GL_ONE_MINUS_SRC_ALPHA );  // Set Alpha Blend Function

      gl.glMatrixMode( GL10.GL_PROJECTION );          // Activate Projection Matrix
      gl.glLoadIdentity();                            // Load Identity Matrix
      gl.glOrthof(                                    // Set Ortho Projection (Left,Right,Bottom,Top,Front,Back)
         0, width,
         0, height,
         1.0f, -1.0f
      );

      gdpos = 0;
      while(gdpos < graphdata.length) {
        switch(gdpop()) {
          case 2: {
            int x = gdpop();
            int y = gdpop();
            int al = gdpop();
            int col = gdpop() + 0xFF000000;
            int size = gdpop();
            int b = gdpop();
            int n = gdpop();
            StringBuffer ss = new StringBuffer();
            for(int i=0; i<n; i++) {
	      char c = (char) gdpop();
              ss.append(c);
              }
            
            String s = ss.toString();
            
            y = height - y;
            glText.setScale(size / 48.0f);
            
            if(b > 0) {
            	glText.begin(0f, 0f, 0f, 1f);
            	glText.drawAlign(s, (float)(x-1), (float)y, al);
            	glText.drawAlign(s, (float)(x+1), (float)y, al);
            	glText.drawAlign(s, (float)x, (float)(y-1), al);
            	glText.drawAlign(s, (float)x, (float)(y+1), al);
            	glText.end();
            }

            glText.begin( 
              (col & 0xFF0000) / (float)0xFF0000, 
              (col & 0xFF00) / (float)0xFF00,
              (col & 0xFF) / (float)0xFF,
              1.0f );
            glText.drawAlign(s, (float) x, (float) y, al); 
            glText.end();
            }
            break;
          
        }}

      // disable texture + alpha
      gl.glDisable( GL10.GL_BLEND );                  // Disable Alpha Blend
      gl.glDisable( GL10.GL_TEXTURE_2D );             // Disable Texture Mapping
	  
	}
	
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		gl.glViewport(0, 0, width, height);

            // Save width and height
      this.width = width;                             // Save Current Width
      this.height = height;                           // Save Current Height
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		
               glText = new GLText( gl );
               glText.load(Typeface.DEFAULT_BOLD, 48, 2, 2 );  // Create Font (Height: 48 Pixels / X+Y Padding 2 Pixels)
	}
	
	
}
