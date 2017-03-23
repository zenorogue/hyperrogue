// taken from Texample,
// http://fractiousg.blogspot.com/201/4/rendering-text-in-opengl-on-android.html

package com.android.texample;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.opengles.GL10;

public class Vertices {

   //--Constants--//
   final static int POSITION_CNT_2D = 2;              // Number of Components in Vertex Position for 2D
   final static int POSITION_CNT_3D = 3;              // Number of Components in Vertex Position for 3D
   final static int COLOR_CNT = 4;                    // Number of Components in Vertex Color
   final static int TEXCOORD_CNT = 2;                 // Number of Components in Vertex Texture Coords
   final static int NORMAL_CNT = 3;                   // Number of Components in Vertex Normal

   final static int INDEX_SIZE = Short.SIZE / 8;      // Index Byte Size (Short.SIZE = bits)

   //--Members--//
   // NOTE: all members are constant, and initialized in constructor!
   final GL10 gl;                                     // GL Instance
   final boolean hasColor;                            // Use Color in Vertices
   final boolean hasTexCoords;                        // Use Texture Coords in Vertices
   final boolean hasNormals;                          // Use Normals in Vertices
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
   public Vertices(GL10 gl, int maxVertices, int maxIndices, boolean hasColor, boolean hasTexCoords, boolean hasNormals)  {
      this( gl, maxVertices, maxIndices, hasColor, hasTexCoords, hasNormals, false );  // Call Overloaded Constructor
   }
   public Vertices(GL10 gl, int maxVertices, int maxIndices, boolean hasColor, boolean hasTexCoords, boolean hasNormals, boolean use3D)  {
      this.gl = gl;                                   // Save GL Instance
      this.hasColor = hasColor;                       // Save Color Flag
      this.hasTexCoords = hasTexCoords;               // Save Texture Coords Flag
      this.hasNormals = hasNormals;                   // Save Normals Flag
      this.positionCnt = use3D ? POSITION_CNT_3D : POSITION_CNT_2D;  // Set Position Component Count
      this.vertexStride = this.positionCnt + ( hasColor ? COLOR_CNT : 0 ) + ( hasTexCoords ? TEXCOORD_CNT : 0 ) + ( hasNormals ? NORMAL_CNT : 0 );  // Calculate Vertex Stride
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
   public void bind()  {
      gl.glEnableClientState( GL10.GL_VERTEX_ARRAY ); // Enable Position in Vertices
      vertices.position( 0 );                         // Set Vertex Buffer to Position
      gl.glVertexPointer( positionCnt, GL10.GL_FLOAT, vertexSize, vertices );  // Set Vertex Pointer

      if ( hasColor )  {                              // IF Vertices Have Color
         gl.glEnableClientState( GL10.GL_COLOR_ARRAY );  // Enable Color in Vertices
         vertices.position( positionCnt );            // Set Vertex Buffer to Color
         gl.glColorPointer( COLOR_CNT, GL10.GL_FLOAT, vertexSize, vertices );  // Set Color Pointer
      }

      if ( hasTexCoords )  {                          // IF Vertices Have Texture Coords
         gl.glEnableClientState( GL10.GL_TEXTURE_COORD_ARRAY );  // Enable Texture Coords in Vertices
         vertices.position( positionCnt + ( hasColor ? COLOR_CNT : 0 ) );  // Set Vertex Buffer to Texture Coords (NOTE: position based on whether color is also specified)
         gl.glTexCoordPointer( TEXCOORD_CNT, GL10.GL_FLOAT, vertexSize, vertices );  // Set Texture Coords Pointer
      }

      if ( hasNormals )  {
         gl.glEnableClientState( GL10.GL_NORMAL_ARRAY );  // Enable Normals in Vertices
         vertices.position( positionCnt + ( hasColor ? COLOR_CNT : 0 ) + ( hasTexCoords ? TEXCOORD_CNT : 0 ) );  // Set Vertex Buffer to Normals (NOTE: position based on whether color/texcoords is also specified)
         gl.glNormalPointer( GL10.GL_FLOAT, vertexSize, vertices );  // Set Normals Pointer
      }
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
         gl.glDrawElements( primitiveType, numVertices, GL10.GL_UNSIGNED_SHORT, indices );  // Draw Indexed
      }
      else  {                                         // ELSE No Indices Exist
         gl.glDrawArrays( primitiveType, offset, numVertices );  // Draw Direct (Array)
      }
   }

   //--Unbind--//
   // D: clear binding states when done rendering batches.
   //    USAGE: call once before calling draw() multiple times for this buffer.
   // A: [none]
   // R: [none]
   public void unbind()  {
      gl.glDisableClientState( GL10.GL_VERTEX_ARRAY );  // Clear Vertex Array State

      if ( hasColor )                                 // IF Vertices Have Color
         gl.glDisableClientState( GL10.GL_COLOR_ARRAY );  // Clear Color State

      if ( hasTexCoords )                             // IF Vertices Have Texture Coords
         gl.glDisableClientState( GL10.GL_TEXTURE_COORD_ARRAY );  // Clear Texture Coords State

      if ( hasNormals )                               // IF Vertices Have Normals
         gl.glDisableClientState( GL10.GL_NORMAL_ARRAY );  // Clear Normals State
   }

   //--Draw Full--//
   // D: draw the vertices in the vertex/index buffers
   //    NOTE: unoptimized version! use bind()/draw()/unbind() for batches
   // A: primitiveType - the type of primitive to draw
   //    offset - the offset in the vertex/index buffer to start at
   //    numVertices - the number of vertices (indices) to draw
   // R: [none]
   public void drawFull(int primitiveType, int offset, int numVertices)  {
      gl.glEnableClientState( GL10.GL_VERTEX_ARRAY ); // Enable Position in Vertices
      vertices.position( 0 );                         // Set Vertex Buffer to Position
      gl.glVertexPointer( positionCnt, GL10.GL_FLOAT, vertexSize, vertices );  // Set Vertex Pointer

      if ( hasColor )  {                              // IF Vertices Have Color
         gl.glEnableClientState( GL10.GL_COLOR_ARRAY );  // Enable Color in Vertices
         vertices.position( positionCnt );            // Set Vertex Buffer to Color
         gl.glColorPointer( COLOR_CNT, GL10.GL_FLOAT, vertexSize, vertices );  // Set Color Pointer
      }

      if ( hasTexCoords )  {                          // IF Vertices Have Texture Coords
         gl.glEnableClientState( GL10.GL_TEXTURE_COORD_ARRAY );  // Enable Texture Coords in Vertices
         vertices.position( positionCnt + ( hasColor ? COLOR_CNT : 0 ) );  // Set Vertex Buffer to Texture Coords (NOTE: position based on whether color is also specified)
         gl.glTexCoordPointer( TEXCOORD_CNT, GL10.GL_FLOAT, vertexSize, vertices );  // Set Texture Coords Pointer
      }

      if ( indices != null )  {                       // IF Indices Exist
         indices.position( offset );                  // Set Index Buffer to Specified Offset
         gl.glDrawElements( primitiveType, numVertices, GL10.GL_UNSIGNED_SHORT, indices );  // Draw Indexed
      }
      else  {                                         // ELSE No Indices Exist
         gl.glDrawArrays( primitiveType, offset, numVertices );  // Draw Direct (Array)
      }

      if ( hasTexCoords )                             // IF Vertices Have Texture Coords
         gl.glDisableClientState( GL10.GL_TEXTURE_COORD_ARRAY );  // Clear Texture Coords State

      if ( hasColor )                                 // IF Vertices Have Color
         gl.glDisableClientState( GL10.GL_COLOR_ARRAY );  // Clear Color State

      gl.glDisableClientState( GL10.GL_VERTEX_ARRAY );  // Clear Vertex Array State
   }

   //--Set Vertex Elements--//
   // D: use these methods to alter the values (position, color, textcoords, normals) for vertices
   //    WARNING: these do NOT validate any values, ensure that the index AND specified
   //             elements EXIST before using!!
   // A: x, y, z - the x,y,z position to set in buffer
   //    r, g, b, a - the r,g,b,a color to set in buffer
   //    u, v - the u,v texture coords to set in buffer
   //    nx, ny, nz - the x,y,z normal to set in buffer
   // R: [none]
   void setVtxPosition(int vtxIdx, float x, float y)  {
      int index = vtxIdx * vertexStride;              // Calculate Actual Index
      vertices.put( index + 0, Float.floatToRawIntBits( x ) );  // Set X
      vertices.put( index + 1, Float.floatToRawIntBits( y ) );  // Set Y
   }
   void setVtxPosition(int vtxIdx, float x, float y, float z)  {
      int index = vtxIdx * vertexStride;              // Calculate Actual Index
      vertices.put( index + 0, Float.floatToRawIntBits( x ) );  // Set X
      vertices.put( index + 1, Float.floatToRawIntBits( y ) );  // Set Y
      vertices.put( index + 2, Float.floatToRawIntBits( z ) );  // Set Z
   }
   void setVtxColor(int vtxIdx, float r, float g, float b, float a)  {
      int index = ( vtxIdx * vertexStride ) + positionCnt;  // Calculate Actual Index
      vertices.put( index + 0, Float.floatToRawIntBits( r ) );  // Set Red
      vertices.put( index + 1, Float.floatToRawIntBits( g ) );  // Set Green
      vertices.put( index + 2, Float.floatToRawIntBits( b ) );  // Set Blue
      vertices.put( index + 3, Float.floatToRawIntBits( a ) );  // Set Alpha
   }
   void setVtxColor(int vtxIdx, float r, float g, float b)  {
      int index = ( vtxIdx * vertexStride ) + positionCnt;  // Calculate Actual Index
      vertices.put( index + 0, Float.floatToRawIntBits( r ) );  // Set Red
      vertices.put( index + 1, Float.floatToRawIntBits( g ) );  // Set Green
      vertices.put( index + 2, Float.floatToRawIntBits( b ) );  // Set Blue
   }
   void setVtxColor(int vtxIdx, float a)  {
      int index = ( vtxIdx * vertexStride ) + positionCnt;  // Calculate Actual Index
      vertices.put( index + 3, Float.floatToRawIntBits( a ) );  // Set Alpha
   }
   void setVtxTexCoords(int vtxIdx, float u, float v)  {
      int index = ( vtxIdx * vertexStride ) + positionCnt + ( hasColor ? COLOR_CNT : 0 );  // Calculate Actual Index
      vertices.put( index + 0, Float.floatToRawIntBits( u ) );  // Set U
      vertices.put( index + 1, Float.floatToRawIntBits( v ) );  // Set V
   }
   void setVtxNormal(int vtxIdx, float x, float y, float z)  {
      int index = ( vtxIdx * vertexStride ) + positionCnt + ( hasColor ? COLOR_CNT : 0 ) + ( hasTexCoords ? TEXCOORD_CNT : 0 );  // Calculate Actual Index
      vertices.put( index + 0, Float.floatToRawIntBits( x ) );  // Set X
      vertices.put( index + 1, Float.floatToRawIntBits( y ) );  // Set Y
      vertices.put( index + 2, Float.floatToRawIntBits( z ) );  // Set Z
   }
}
