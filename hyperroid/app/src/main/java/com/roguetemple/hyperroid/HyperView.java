package com.roguetemple.hyperroid;

import android.app.KeyguardManager;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.graphics.Typeface;
import android.os.Build;
import android.os.PowerManager;
import android.os.SystemClock;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class HyperView extends View {

    HyperRogue game;
    
    public HyperView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        initView();
        }

    public HyperView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initView();
        }
    
    int width, height, mousex, mousey;
    boolean clicked;
    int clickcnt = 0;
    Canvas dc;
    
    int[] graphdata;
    int gdpos;
    int gdpop() { return graphdata[gdpos++]; }

    int lasttick, curtick;

    int realpha(int col) {
      return ((col >> 8) & 0xFFFFFF) + ((col & 0xFF) << 24); 
      }
    
    public void drawScreen(PorterDuff.Mode mode) {
    	Paint pon = new Paint();
        pon.setXfermode(new PorterDuffXfermode(mode));

        pon.setColor(0xC0C0C0C0);
        pon.setAntiAlias(true);        

        pon.setTextSize(8);
        pon.setTextAlign(Paint.Align.RIGHT);            
        pon.setTypeface(Typeface.DEFAULT_BOLD);

        // String s = "D " + Integer.toString(graphdata.length) + " T " + Integer.toString(curtick-lasttick);
        // dc.drawText(s, width*11/12, height*1/12, pon);
        
        gdpos = 0; 
        while(gdpos < graphdata.length) {
          switch(gdpop()) {
            case 2: {
              int x = gdpop();
              int y = gdpop();
              int al = gdpop();
              int col = gdpop() + 0xFF000000;
              int size = gdpop();
              y = y + size/2;
              int b = gdpop();
              int n = gdpop();
              StringBuffer ss = new StringBuffer();
              for(int i=0; i<n; i++) {
                char c = (char)gdpop();
                ss.append(c);
                }
              pon.setStyle(Paint.Style.FILL);
              pon.setColor(col);
              pon.setTextSize(size);
              pon.setTextAlign(
                al == 0 ? Paint.Align.LEFT :
                al == 8 ? Paint.Align.CENTER :
                Paint.Align.RIGHT);
              if(b>0) {
                  pon.setColor(0xFF000000);
                  dc.drawText(ss.toString(), x-b, y, pon);
                  dc.drawText(ss.toString(), x+b, y, pon);
                  dc.drawText(ss.toString(), x, y-b, pon);
                  dc.drawText(ss.toString(), x, y+b, pon);
              }
              pon.setColor(col); 
              dc.drawText(ss.toString(), x, y, pon);
              }
              break;
            
            case 1: {
              int col = gdpop();
              int otl = gdpop();
              int num = gdpop();
              
              pon.setColor(realpha(col));
              
              /* for(int i=0; i<num; i++) {
                int x2 = gdpop();
                int y2 = gdpop();
                dc.drawText("x", x2, y2, pon);
                } */

              Path p = new Path();
              
              int x = gdpop();
              int y = gdpop();
              
              p.moveTo(x, y);
              
              for(int i=1; i<num; i++) {
                int x2 = gdpop();
                int y2 = gdpop();
                p.lineTo(x2, y2);
                }
              p.lineTo(x, y);
              
              pon.setStyle(Paint.Style.FILL);
              dc.drawPath(p, pon);
              
              pon.setStyle(Paint.Style.STROKE);
              pon.setColor(realpha(otl));
              dc.drawPath(p, pon);
              break;
          }
        
        case 3: { 
      	  int col = gdpop();
            int num = gdpop();
            
            pon.setColor(realpha(col));
            
            int x = gdpop();
            int y = gdpop();
            
            for(int i=1; i<num; i++) {
              int x2 = gdpop();
              int y2 = gdpop();
              dc.drawLine(x, y, x2, y2, pon);
              x = x2; y = y2;
              }
            break;
            }

        case 4: {
      	  int col = gdpop();
            int x = gdpop();
            int y = gdpop();
            int rad = gdpop();
            
            col += 0xFF000000;
            pon.setColor(col);
            pon.setStyle(Paint.Style.STROKE);
            
            dc.drawCircle(x, y, rad, pon);
            
            break;
            }
       
   }}
    }

    long clockzero = 0;

    public void updateGame() {
    	lasttick = curtick;
    	if(clockzero == 0) clockzero = SystemClock.elapsedRealtime();
        curtick = (int) (SystemClock.elapsedRealtime() - clockzero);
        
        if(clickcnt > 0) clickcnt--;
        
        game.update(width, height, curtick, mousex, mousey, clicked | ((clickcnt & 1) > 0));
        
        final HyperRogue fgame = game;
        game.runOnUiThread(new Runnable() { public void run() { fgame.checkMusic(); }});
        }
    
    @Override
    public void onDraw(final Canvas canvas) {


      super.onDraw(canvas);


        PowerManager pm = (PowerManager) game.getSystemService(Context.POWER_SERVICE);
      boolean isScreenOn = Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT_WATCH ? pm.isInteractive() : pm.isScreenOn();
      if(!isScreenOn) return;

      if(!game.activityVisible) return;

        dc = canvas;
      width = getWidth();
      height = getHeight();      

      if(game != null && game.glview == null) {
    	updateGame();
        game.draw();
        graphdata = game.loadMap();
        drawScreen(PorterDuff.Mode.SRC_ATOP);
        invalidate();
        }
      }

    public Bitmap screenshot() {
    	Bitmap b = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    	Canvas old = dc;
    	dc = new Canvas(b);
        synchronized(game) {
          game.forceCanvas = true;
          game.drawScreenshot();
          graphdata = game.loadMap();
          game.forceCanvas = false;
    	  }
        dc.setDensity(old.getDensity());
    	drawScreen(PorterDuff.Mode.SRC_OVER);
        return b;
    }
        
    @Override
    public boolean onTouchEvent (MotionEvent event) {
      if(event.getAction() == MotionEvent.ACTION_DOWN) {
    	mousex = (int) event.getX();
    	mousey = (int) event.getY();
    	clickcnt += 2;
    	clicked = true;
    	}
    	    
      if(event.getAction() == MotionEvent.ACTION_UP) {
    	clicked = false;
        }
      	    
      if(event.getAction() == MotionEvent.ACTION_MOVE) {
      	mousex = (int) event.getX();
      	mousey = (int) event.getY();
      	}
      	    
        return true;
      }



    public void initView() {
      setFocusable(true);
      setFocusableInTouchMode(true);
      mousex = 0; mousey = 0; clicked = false;
      }

    }
