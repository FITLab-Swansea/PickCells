package com.example.alixgoguey.pickcellsandroid19;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.provider.Settings;
import android.support.constraint.ConstraintLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
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

public class DialMenu extends AppCompatActivity implements View.OnClickListener {


    Button btnHue, btnSat, btnValue;


    Button[] btns = new Button[3];

    Socket socket;
    String IMEI = null;
    JSONArray obj_buff;
    String obj_str;

    boolean[] activeColourChanels = new boolean[3];
    int[] colourChanelsValue = new int[3];

    TextView cubeIDLable;

    ConstraintLayout layout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_dial_menu);

        setButtons();
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        cubeIDLable = findViewById(R.id.cube_id);

        layout = findViewById(R.id.dial_layout);

        btns[0] = btnHue;
        btns [1] = btnSat;
        btns [2] =btnValue;

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
                            String s = "Hello World!";
                            cubeIDLable.setText(s + "\n" + obj_buff.length());

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


    private void endActivity(){

        Toast toast = Toast.makeText(getApplicationContext(), "Disconnected...", Toast.LENGTH_SHORT);
        toast.show();
        finish();
    }

    private void setButtons() {
        btnHue = findViewById(R.id.btn_hue);
        btnSat = findViewById(R.id.btn_Sat);
        btnValue = findViewById(R.id.btn_value);

        btnSat.setOnClickListener(this);
        btnValue.setOnClickListener(this);
        btnHue.setOnClickListener(this);
    }

    private String indexToChar(int index){
        switch (index){
            case 0:
                return "Hue";
            case 1:
                return "Sat";
            case 2:
                return "Value";
        }
        return "";
    }

    private void sendSide(int channel){
        JSONObject obj = new JSONObject();
        try {
            obj.put("IMEI", getDeviceIMEI());
            obj.put("channel", indexToChar(channel));
            obj.put("value", colourChanelsValue[channel]);

        } catch (JSONException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }
        socket.emit("sideChange", obj);
    }

    private void changeSide(int channel, View v){
        if (activeColourChanels[channel]){
            activeColourChanels[channel] = false;
            v.setBackgroundColor(Color.RED);
            sendSide(channel);
        } else {

            for (int i = 0; i < 3; i++) {
                if (i != channel) {
                    activeColourChanels[i] = false;
                    btns[i].setBackgroundColor(Color.RED);
                }
            }
            activeColourChanels[channel] = true;
            v.setBackgroundColor(Color.GREEN);
            sendSide(channel);
        }

    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case (R.id.btn_hue) :
                changeSide(0, v);
                break;
            case (R.id.btn_Sat) :
                changeSide(1, v);
                break;
            case (R.id.btn_value) :
                changeSide(2, v);
                break;
        }

    }

}
