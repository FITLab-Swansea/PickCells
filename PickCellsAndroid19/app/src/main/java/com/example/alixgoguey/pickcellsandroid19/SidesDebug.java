package com.example.alixgoguey.pickcellsandroid19;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.provider.Settings;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URISyntaxException;
import java.text.SimpleDateFormat;
import java.util.Date;

import io.socket.client.IO;
import io.socket.client.Socket;
import io.socket.emitter.Emitter;

public class SidesDebug extends Activity implements View.OnClickListener{
    Button btnNorth, btnSouth, btnEast, btnWest, btnBottom;

    Socket socket;
    String IMEI = null;
    byte obj_byt[];
    String obj_str;

    boolean[] activeSides = new boolean[5];

    TextView cubeIDLable;

    @SuppressLint("MissingPermission")
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
        setContentView(R.layout.activity_sides_debug);
        setButtons();

        cubeIDLable = (TextView) findViewById(R.id.cube_id);

        try {

//            socket = IO.socket("http://172.20.10.2:9000");
            socket = IO.socket("http://172.20.10.11:9000");
            // socket = IO.socket("http://192.168.43.74:9000");
            // socket = IO.socket("http://192.168.0.26:9000");

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
                        String s = "id: ";
                        Log.v("id: ", s + "\n" + getDeviceIMEI());
                        cubeIDLable.setText(s + getDeviceIMEI());

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
                        cubeIDLable.setText(s + "\n" + obj_byt.length);

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
                        // layout.setBackground(ob);
                    }
                });
            }

        }).on(Socket.EVENT_DISCONNECT, new Emitter.Listener() {

            @Override
            public void call(Object... args) {
                runOnUiThread(new Runnable() {
                    public void run() {

                        cubeIDLable.setText("Disconnected...");
                    }
                });
            }
        });
        socket.connect();
    }

    private void setButtons() {
        btnBottom = findViewById(R.id.btn_bottom);
        btnNorth = findViewById(R.id.btn_north);
        btnSouth = findViewById(R.id.btn_south);
        btnEast = findViewById(R.id.btn_east);
        btnWest = findViewById(R.id.btn_west);

        btnBottom.setOnClickListener(this);
        btnNorth.setOnClickListener(this);
        btnSouth.setOnClickListener(this);
        btnEast.setOnClickListener(this);
        btnWest.setOnClickListener(this);
        btnBottom.setOnClickListener(this);
    }

    private String indexToChar(int index){
        switch (index){
            case 0:
                return "North";
            case 1:
                return "East";
            case 2:
                return "South";
            case 3:
                return "West";
            case 4:
                return "Bottom";
        }
        return "";
    }

    private void sendSide(int side){
        JSONObject obj = new JSONObject();
        try {
            obj.put("IMEI", getDeviceIMEI());
            obj.put("side", indexToChar(side));
            obj.put("active", activeSides[side]);

        } catch (JSONException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        socket.emit("sideChange", obj);
    }

    private void changeSide(int side){
        if (activeSides[side]){
            activeSides[side] = false;
            sendSide(side);
        } else {
            activeSides[side] = true;
            sendSide(side);
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case (R.id.btn_north) :
                changeSide(0);
                break;
            case (R.id.btn_east) :
                changeSide(1);
                break;
            case (R.id.btn_south) :
                changeSide(2);
                break;
            case (R.id.btn_west) :
                changeSide(3);
                break;
            case (R.id.btn_bottom) :
                changeSide(4);
                break;

        }

    }

}
