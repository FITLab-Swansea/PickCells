package com.example.alixgoguey.pickcellsandroid19;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.provider.Settings;
import android.support.constraint.ConstraintLayout;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.net.URISyntaxException;

import io.socket.client.IO;
import io.socket.client.Socket;
import io.socket.emitter.Emitter;

public class SidesDebug extends Activity implements View.OnClickListener{
    Button btnNorth, btnSouth, btnEast, btnWest, btnBottom, btnTop;

    Socket socket;
    String IMEI = null;
    JSONArray obj_buff;
    String watch_id = "";
    long touch_framerate_ms = (long) -1;

    boolean[] activeSides = new boolean[6];

    TextView cubeIDLable;

    ConstraintLayout layout;

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
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        cubeIDLable = (TextView) findViewById(R.id.cube_id);

        layout = (ConstraintLayout) findViewById(R.id.bug_layout);

        try {

//            socket = IO.socket("http://172.20.10.2:9000");
//            socket = IO.socket("http://172.20.10.11:9000");
            // socket = IO.socket("http://192.168.43.74:9000");
            // socket = IO.socket("http://192.168.0.26:9000");
            socket = IO.socket("http://192.168.1.100:9000");

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
                    watch_id = obj.getString("id"); // get node server ID
                    obj = obj.getJSONObject("params"); // get parameters
                    touch_framerate_ms = (long) (1000 / Integer.parseInt(obj.getString("touch_framerate")));
                } catch (JSONException e) {
                    obj = null;
                    watch_id = "";
                    touch_framerate_ms = (long) -1;
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
            obj_buff = null;
            try {
                obj = new JSONObject(obj.getString("msg"));
                obj_buff = obj.getJSONArray("data");
            } catch (JSONException e) {
                Log.d("IOIO", "problem retrieving byte array");
                obj_buff = null;
            }

            if (obj_buff != null) {
                runOnUiThread(new Runnable() {
                    public void run() {
                    cubeIDLable.setText("buf len: " + obj_buff.length());

                    Log.d("IOIO", String.valueOf(obj_buff.length()));

                    try {
                        int w = (0x000000FF & ((int) obj_buff.get(0)));//Integer.parseInt(strs[0]);
                        int h = (0x000000FF & ((int) obj_buff.get(1)));//Integer.parseInt(strs[1]);
                        Log.d("IOIO", "width = " + w);
                        Log.d("IOIO", "height = " + h);

                        int msg_size = w*h*3+2;
                        if (msg_size <= obj_buff.length()) {
                            Bitmap.Config conf = Bitmap.Config.ARGB_8888; // see other conf types
                            Bitmap bmp = Bitmap.createBitmap(w, h, conf);
                            int k = 2;
                            for (int pix_x = 0; pix_x < w; pix_x++) {
                                for (int pix_y = 0; pix_y < h; pix_y++) {
                                    bmp.setPixel(
                                            pix_x,
                                            pix_y,
                                            Color.argb(
                                                    255,
                                                    (0x000000FF & ((int) obj_buff.get(k))),
                                                    (0x000000FF & ((int) obj_buff.get(k + 1))),
                                                    (0x000000FF & ((int) obj_buff.get(k + 2))))
                                    );
                                    k += 3;
                                }
                            }

                            BitmapDrawable ob = new BitmapDrawable(getResources(), bmp);
                            layout.setBackground(ob);
                        } else {
                            Log.d("IOIO", "problem decoding image");
                        }
                    } catch (JSONException e) {
                        e.printStackTrace();
                    }
                    }
                });
            }
            }

        }).on(Socket.EVENT_DISCONNECT, new Emitter.Listener() {

            @Override
            public void call(Object... args) {
                runOnUiThread(new Runnable() {
                    public void run() {

                        cubeIDLable.setText("Disconnected...");
                        endActivity();

                    }
                });
            }
        });
        socket.connect();
    }

    private void endActivity(){

        Toast toast = Toast.makeText(getApplicationContext(), "Disconnected...", Toast.LENGTH_SHORT);
        toast.show();
        finish();
    }

    private void setButtons() {
        btnBottom = findViewById(R.id.btn_bottom);
        btnNorth = findViewById(R.id.btn_north);
        btnSouth = findViewById(R.id.btn_south);
        btnEast = findViewById(R.id.btn_east);
        btnWest = findViewById(R.id.btn_west);
        btnTop = findViewById(R.id.btn_top);

        btnBottom.setOnClickListener(this);
        btnNorth.setOnClickListener(this);
        btnSouth.setOnClickListener(this);
        btnEast.setOnClickListener(this);
        btnWest.setOnClickListener(this);
        btnTop.setOnClickListener(this);
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
            case 5:
                return "Top";
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

    private void changeSide(int side, View v){
        if (activeSides[side]){
            activeSides[side] = false;
            v.setBackgroundColor(Color.RED);
            sendSide(side);
        } else {
            activeSides[side] = true;
            v.setBackgroundColor(Color.GREEN);
            sendSide(side);

        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case (R.id.btn_north) :
                changeSide(0, v);
                break;
            case (R.id.btn_east) :
                changeSide(1, v);
                break;
            case (R.id.btn_south) :
                changeSide(2, v);
                break;
            case (R.id.btn_west) :
                changeSide(3, v);
                break;
            case (R.id.btn_bottom) :
                changeSide(4, v);
                break;
            case (R.id.btn_top) :
                changeSide(5, v);
                break;

        }

    }

}
