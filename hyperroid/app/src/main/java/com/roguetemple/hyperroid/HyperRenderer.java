package com.roguetemple.hyperroid;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

// import android.graphics.Paint;
// import android.graphics.Path;
import android.content.Context;
import android.graphics.Typeface;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.PowerManager;
// bimport android.widget.Toast;

import com.android.texample.GLText;
import com.android.texample.Shader;

public class HyperRenderer implements GLSurfaceView.Renderer {
	
       int width, height;

	HyperRogue game;

	private GLText glText;                             // A GLText Instance		

	int [] graphdata;
	int gdpos;
	int gdpop() { return graphdata[gdpos++]; }
    boolean initialized;
	
	public void onDrawFrame(GL10 unused) {
	  if(game.forceCanvas) return;

          PowerManager pm = (PowerManager) game.getSystemService(Context.POWER_SERVICE);
          boolean isScreenOn = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT_WATCH ? pm.isInteractive() : pm.isScreenOn();
          if(!isScreenOn) return;
          if(!game.activityVisible) return;

          GLES20.glClear(GL10.GL_COLOR_BUFFER_BIT);
	  synchronized(game) {
          if(!initialized) { game.glhrinit(); initialized = true; }
          game.hv.updateGame();
        game.draw();
        graphdata = game.loadMap();
            glText.shader.aPosition = game.getaPosition();
            glText.shader.aTexture = game.getaTexture();
            glText.shader.uColor = game.getuColor();
	    }

         if(graphdata == null) return;

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
	}
	
	public void onSurfaceChanged(GL10 unused, int width, int height) {
		GLES20.glViewport(0, 0, width, height);

            // Save width and height
      this.width = width;                             // Save Current Width
      this.height = height;                           // Save Current Height
        initialized = false;
	}

	public void onSurfaceCreated(GL10 unused, EGLConfig config) {
  
		
               glText = new GLText( new Shader() );
               glText.load(Typeface.DEFAULT_BOLD, 48, 2, 2 );  // Create Font (Height: 48 Pixels / X+Y Padding 2 Pixels)
        initialized = false;
	}
	
	
}
