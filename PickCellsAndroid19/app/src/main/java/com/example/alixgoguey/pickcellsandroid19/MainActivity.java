package com.example.alixgoguey.pickcellsandroid19;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.provider.Settings;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URISyntaxException;
import java.text.SimpleDateFormat;
import java.util.Date;

import io.socket.client.IO;
import io.socket.client.Socket;
import io.socket.emitter.Emitter;

public class MainActivity extends AppCompatActivity implements View.OnClickListener, View.OnTouchListener {

    Button buttonSend;
    TextView textView;
    RelativeLayout layout;
    Socket socket;

    byte obj_byt[];
    String obj_str;

    String IMEI = null;

    CellSensor cell_sensor = null;

    public String getDeviceIMEI() {
        if (IMEI == null) {
            IMEI = "";
            TelephonyManager tm = (TelephonyManager) this.getSystemService(Context.TELEPHONY_SERVICE);
            if (null != tm) {
                IMEI = tm.getDeviceId();
            }
            if (null == IMEI || 0 == IMEI.length()) {
                IMEI = Settings.Secure.getString(this.getContentResolver(), Settings.Secure.ANDROID_ID);
            }
        }
        return IMEI;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        buttonSend = (Button) findViewById(R.id.button);
        buttonSend.setOnClickListener(this);

        textView = (TextView) findViewById(R.id.textView);
        layout = (RelativeLayout)findViewById(R.id.main_layout);

        layout.setOnTouchListener(this);

        try {
            socket = IO.socket("http://192.168.43.74:9000");
        } catch (URISyntaxException e) {
            e.printStackTrace();
        }
        socket.on(Socket.EVENT_CONNECT, new Emitter.Listener() {

            @Override
            public void call(Object... args) {
                // Sending an object
                JSONObject obj = new JSONObject();
                try {
                    obj.put("Hi", "I'm a watch");
                    obj.put("IMEI", getDeviceIMEI());
                } catch (JSONException e) {
                    e.printStackTrace();
                }

                socket.emit("connected", obj);
            }

        }).on("event", new Emitter.Listener() {

            @Override
            public void call(Object... args) {
                JSONObject obj = (JSONObject) args[0];
                try {
                    obj_str = (String) obj.get("id");
                } catch (JSONException e) {
                    obj_str = null;
                }

                runOnUiThread(new Runnable() {
                    public void run() {
                        String s = "Hello World!";
                        textView.setText(s + "\n" + obj_str);
                    }
                });
            }

        }).on("qt", new Emitter.Listener() {

            @Override
            public void call(Object... args) {
                JSONObject obj = (JSONObject) args[0];
                try {
                    obj_byt = (byte[]) obj.get("msg");
                } catch (JSONException e) {
                    obj_byt = null;
                }

                runOnUiThread(new Runnable() {
                    public void run() {
                        String s = "Hello World!";
                        textView.setText(s + "\n" + obj_byt.length);

                        //Log.d("IOIO", String.valueOf(obj_byt.length));

                        int w = (0x000000FF & ((int)obj_byt[0]));//Integer.parseInt(strs[0]);
                        int h = (0x000000FF & ((int)obj_byt[1]));//Integer.parseInt(strs[1]);
                        //Log.d("IOIO", "width = " + w);
                        //Log.d("IOIO", "height = " + h);

                        Bitmap.Config conf = Bitmap.Config.ARGB_8888; // see other conf types
                        Bitmap bmp = Bitmap.createBitmap(w,h, conf);
                        int k = 2;
                        for (int pix_x = 0; pix_x < w; pix_x++) {
                            for (int pix_y = 0; pix_y < h; pix_y++) {
                                bmp.setPixel(
                                        pix_x,
                                        pix_y,
                                        Color.argb(
                                                255,
                                                (0x000000FF & ((int)obj_byt[k])),
                                                (0x000000FF & ((int)obj_byt[k+1])),
                                                (0x000000FF & ((int)obj_byt[k+2])))
                                );
                                k += 3;
                            }
                        }

                        BitmapDrawable ob = new BitmapDrawable(getResources(), bmp);
                        layout.setBackground(ob);
                    }
                });
            }

        }).on(Socket.EVENT_DISCONNECT, new Emitter.Listener() {

            @Override
            public void call(Object... args) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        String s = "Hello World!";
                        textView.setText(s + "\nDisconnected...");
                    }
                });
            }
        });
        socket.connect();

        cell_sensor = new CellSensor();
    }

    @Override
    public void onClick(View v) {
        // Sending an object
        JSONObject obj = new JSONObject();
        try {
            SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
            obj.put("IMEI", getDeviceIMEI());
            obj.put("Date", dateFormat.format(new Date()));
        } catch (JSONException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        socket.emit("clicked", obj);
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {

        if (motionEvent.getPointerId(motionEvent.getActionIndex()) == 0) {
            // Sending an object
            JSONObject obj = new JSONObject();
            JSONObject touch = new JSONObject();
            try {
                obj.put("x", motionEvent.getX());
                obj.put("y", motionEvent.getY());
                obj.put("type", motionEvent.getAction());

                obj.put("IMEI", getDeviceIMEI());
                obj.put("Touch", touch);
            } catch (JSONException e) {
                e.printStackTrace();
            }

            //Log.d("IOIO", " touch frame");
            socket.emit("touchframe", obj);
        }
        return true;
    }

    protected void onResume() {
        super.onResume();
        Log.d("SENSOR", "resume ??");
        cell_sensor.mSensorManager.registerListener(
                cell_sensor,
                cell_sensor.accelerometer,
                SensorManager.SENSOR_DELAY_NORMAL
        );
    }

    protected void onPause() {
        super.onPause();
        Log.d("SENSOR", "pause ??");
        cell_sensor.mSensorManager.unregisterListener(cell_sensor);
    }

    private class CellSensor implements SensorEventListener {
        SensorManager mSensorManager = null;
        Sensor accelerometer = null;

        float[] acc;

        public CellSensor() {
            mSensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
            accelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            Log.d("SENSOR", "creation ??");
            Log.d("SENSOR", "sensor list: "+mSensorManager.getSensorList(Sensor.TYPE_ALL).toString());
//            Log.d("SENSOR", "acc sensor: "+accelerometer.toString());
//            mSensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
//            mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_NORMAL);
        }

        @Override
        public void onSensorChanged(SensorEvent sensorEvent) {
            Log.d("SENSOR", "hey yo acceleroooo ??");

            if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
                Log.d("SENSOR", "hey yeah");
                acc = sensorEvent.values;

                // Sending an object
                JSONObject obj = new JSONObject();
                JSONObject touch = new JSONObject();

                try {
                    obj.put("ax", acc[0]);
                    obj.put("ay", acc[1]);
                    obj.put("az", acc[2]);

                    obj.put("IMEI", getDeviceIMEI());
                    obj.put("Touch", touch);
                } catch (JSONException e) {
                    e.printStackTrace();
                }

                socket.emit("sensor", obj);
            }
        }

        @Override
        public void onAccuracyChanged(android.hardware.Sensor sensor, int i) {
            // Nothing
            Log.d("SENSOR", "Accuracy");
        }
    };
}