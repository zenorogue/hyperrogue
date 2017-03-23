package com.roguetemple.hyperroid;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.database.Cursor;
import android.net.Uri;
import android.os.ParcelFileDescriptor;

public class HyperProvider extends ContentProvider {
  public static final Uri CONTENT_URI=Uri.parse("content://com.roguetemple.hyperroid/");
  
  @Override
  public boolean onCreate() {
    File f=new File(getContext().getCacheDir(), "cache.jpg");
    return f.exists();
  }

  @Override
  public String getType(Uri uri) {
    return "image/jpeg";
    }
    
  @Override
  public ParcelFileDescriptor openFile(Uri uri, String mode) throws FileNotFoundException {
    File f=new File(getContext().getCacheDir(), "cache.jpg");
    return ParcelFileDescriptor.open(f,
                                        ParcelFileDescriptor.MODE_READ_ONLY);
    }
  
  @Override
  public Cursor query(Uri url, String[] projection, String selection,
                        String[] selectionArgs, String sort) {
    throw new RuntimeException("Operation not supported");
  }
  
  @Override
  public Uri insert(Uri uri, ContentValues initialValues) {
    throw new RuntimeException("Operation not supported");
  }
  
  @Override
  public int update(Uri uri, ContentValues values, String where, String[] whereArgs) {
    throw new RuntimeException("Operation not supported");
  }
  
  @Override
  public int delete(Uri uri, String where, String[] whereArgs) {
    throw new RuntimeException("Operation not supported");
  }
  
  static public void copy(InputStream in, File dst) throws IOException {
    FileOutputStream out=new FileOutputStream(dst);
    byte[] buf=new byte[1024];
    int len;
    
    while((len=in.read(buf))>0) {
      out.write(buf, 0, len);
    }
    
    in.close();
    out.close();
  }
}
