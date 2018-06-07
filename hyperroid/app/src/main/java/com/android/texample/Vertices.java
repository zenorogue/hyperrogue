// taken from Texample,
// http://fractiousg.blogspot.com/201/4/rendering-text-in-opengl-on-android.html

package com.android.texample;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;

import android.opengl.GLES20;

public class Vertices {

   //--Constants--//

   final static int INDEX_SIZE = Short.SIZE / 8;      // Index Byte Size (Short.SIZE = bits)

   //--Members--//
   // NOTE: all members are constant, and initialized in constructor!
   final Shader sh;                                   // GL Instance
   public final int positionCnt;                      // Number of Position Components (2=2D, 3=3D)
   public final int vertexStride;                     // Vertex Stride (Element Size of a Single Vertex)
   public final int vertexSize;                       // Bytesize of a Single Vertex
   final IntBuffer vertices;                          // Vertex Buffer
   final ShortBuffer indices;                         // Index Buffer
   public int numVertices;                            // Number of Vertices in Buffer
   public int numIndices;                             // Number of Indices in Buffer
   final int[] tmpBuffer;                             // Temp Buffer for Vertex Conversion

   //--Constructor--//
   // D: create the vertices/indices as specified (for 2d/3d)
   // A: gl - the gl instance to use
   //    maxVertices - maximum vertices allowed in buffer
   //    maxIndices - maximum indices allowed in buffer
   //    hasColor - use color values in vertices
   //    hasTexCoords - use texture coordinates in vertices
   //    hasNormals - use normals in vertices
   //    use3D - (false, default) use 2d positions (ie. x/y only)
   //            (true) use 3d positions (ie. x/y/z)
   public Vertices(Shader sh, int maxVertices, int maxIndices)  {
      this.sh = sh;                                   // Save GL Instance
      this.positionCnt = 4;  // Set Position Component Count
      this.vertexStride = 6;  // Calculate Vertex Stride
      this.vertexSize = this.vertexStride * 4;        // Calculate Vertex Byte Size

      ByteBuffer buffer = ByteBuffer.allocateDirect( maxVertices * vertexSize );  // Allocate Buffer for Vertices (Max)
      buffer.order( ByteOrder.nativeOrder() );        // Set Native Byte Order
      this.vertices = buffer.asIntBuffer();           // Save Vertex Buffer

      if ( maxIndices > 0 )  {                        // IF Indices Required
         buffer = ByteBuffer.allocateDirect( maxIndices * INDEX_SIZE );  // Allocate Buffer for Indices (MAX)
         buffer.order( ByteOrder.nativeOrder() );     // Set Native Byte Order
         this.indices = buffer.asShortBuffer();       // Save Index Buffer
      }
      else                                            // ELSE Indices Not Required
         indices = null;                              // No Index Buffer

      numVertices = 0;                                // Zero Vertices in Buffer
      numIndices = 0;                                 // Zero Indices in Buffer

      this.tmpBuffer = new int[maxVertices * vertexSize / 4];  // Create Temp Buffer
   }

   //--Set Vertices--//
   // D: set the specified vertices in the vertex buffer
   //    NOTE: optimized to use integer buffer!
   // A: vertices - array of vertices (floats) to set
   //    offset - offset to first vertex in array
   //    length - number of floats in the vertex array (total)
   //             for easy setting use: vtx_cnt * (this.vertexSize / 4)
   // R: [none]
   public void setVertices(float[] vertices, int offset, int length)  {
      this.vertices.clear();                          // Remove Existing Vertices
      int last = offset + length;                     // Calculate Last Element
      for ( int i = offset, j = 0; i < last; i++, j++ )  // FOR Each Specified Vertex
         tmpBuffer[j] = Float.floatToRawIntBits( vertices[i] );  // Set Vertex as Raw Integer Bits in Buffer
      this.vertices.put( tmpBuffer, 0, length );      // Set New Vertices
      this.vertices.flip();                           // Flip Vertex Buffer
      this.numVertices = length / this.vertexStride;  // Save Number of Vertices
      //this.numVertices = length / ( this.vertexSize / 4 );  // Save Number of Vertices
   }

   //--Set Indices--//
   // D: set the specified indices in the index buffer
   // A: indices - array of indices (shorts) to set
   //    offset - offset to first index in array
   //    length - number of indices in array (from offset)
   // R: [none]
   public void setIndices(short[] indices, int offset, int length)  {
      this.indices.clear();                           // Clear Existing Indices
      this.indices.put( indices, offset, length );    // Set New Indices
      this.indices.flip();                            // Flip Index Buffer
      this.numIndices = length;                       // Save Number of Indices
   }

   //--Bind--//
   // D: perform all required binding/state changes before rendering batches.
   //    USAGE: call once before calling draw() multiple times for this buffer.
   // A: [none]
   // R: [none]
   public void prepare()  {
      vertices.position( 0 );                         // Set Vertex Buffer to Position
      // GLES20.glVertexPointer( positionCnt, GLES20.GL_FLOAT, vertexSize, vertices );  // Set Vertex Pointer
      GLES20.glVertexAttribPointer(sh.aPosition, 4, GLES20.GL_FLOAT, false, vertexSize, vertices);

      vertices.position( positionCnt );  // Set Vertex Buffer to Texture Coords (NOTE: position based on whether color is also specified)
      // GLES20.glTexCoordPointer( TEXCOORD_CNT, GLES20.GL_FLOAT, vertexSize, vertices );  // Set Texture Coords Pointer
      GLES20.glVertexAttribPointer(sh.aTexture, 2, GLES20.GL_FLOAT, false, vertexSize, vertices);
      }

   //--Draw--//
   // D: draw the currently bound vertices in the vertex/index buffers
   //    USAGE: can only be called after calling bind() for this buffer.
   // A: primitiveType - the type of primitive to draw
   //    offset - the offset in the vertex/index buffer to start at
   //    numVertices - the number of vertices (indices) to draw
   // R: [none]
   public void draw(int primitiveType, int offset, int numVertices)  {
      if ( indices != null )  {                       // IF Indices Exist
         indices.position( offset );                  // Set Index Buffer to Specified Offset
         GLES20.glDrawElements( primitiveType, numVertices, GLES20.GL_UNSIGNED_SHORT, indices );  // Draw Indexed
      }
      else  {                                         // ELSE No Indices Exist
         GLES20.glDrawArrays( primitiveType, offset, numVertices );  // Draw Direct (Array)
      }
   }

}
