package com.roguetemple.hyperroid;
 
import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Build;
import android.os.IBinder;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.widget.Toast;


   import android.app.NotificationChannel;
import android.app.NotificationManager;

public class ForegroundService extends Service {
 
 public static String MAIN_ACTION = "com.roguetemple.hyperroid.action.main";
 public static String STARTFOREGROUND_ACTION = "com.roguetemple.hyperroid.action.startforeground";
 public static String STOPFOREGROUND_ACTION = "com.roguetemple.hyperroid.action.stopforeground";
 public static int FOREGROUND_SERVICE = 101;

    public String getChannelId() {

        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) return "";
        String NOTIFICATION_CHANNEL_ID = "com.roguetemple.hyperrogue";
        String channelName = "HyperRogue game in progress";
        NotificationChannel chan = new NotificationChannel(NOTIFICATION_CHANNEL_ID, channelName, NotificationManager.IMPORTANCE_NONE);
        chan.setLightColor(Color.BLUE);
        chan.setLockscreenVisibility(Notification.VISIBILITY_PRIVATE);
        NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        assert manager != null;
        manager.createNotificationChannel(chan);
        return NOTIFICATION_CHANNEL_ID;
    }

  @Override
 public int onStartCommand(Intent intent, int flags, int startId) {
   if (intent.getAction().equals(STARTFOREGROUND_ACTION)) {

     Intent notificationIntent = new Intent(this, HyperRogue.class);

     notificationIntent.setAction(MAIN_ACTION);
     notificationIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK
       | Intent.FLAG_ACTIVITY_CLEAR_TASK);

     PendingIntent pendingIntent = PendingIntent.getActivity(this, 0,
             notificationIntent, PendingIntent.FLAG_IMMUTABLE
     );

     Notification notification = new NotificationCompat.Builder(this, getChannelId())
     .setContentTitle("HyperRogue")
     .setContentText("Game in progress.")
     .setTicker("Game in progress. Game in progress. Game in progress.")
     .setContentIntent(pendingIntent)
     .setSmallIcon(R.drawable.icon)
     .setOngoing(true)
     .build();

     startForeground(FOREGROUND_SERVICE, notification);

     }
   else if (intent.getAction().equals(STOPFOREGROUND_ACTION)) {
     stopForeground(true);
     stopSelf();
     }
   return START_STICKY;
   }
 
  @Override
  public IBinder onBind(Intent intent) {
    // Used only in case of bound services.
    return null;
    }
  }
 