// taken from Texample,
// http://fractiousg.blogspot.com/201/4/rendering-text-in-opengl-on-android.html

package com.android.texample;

import android.opengl.GLES20;

public class Shader {

   public int aPosition, aTexture, uColor;

   void setColor(float red, float green, float blue, float alpha) {
     GLES20.glUniform4f(uColor, red, green, blue, alpha);
   }
}
