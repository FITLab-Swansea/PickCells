package com.example.alixgoguey.pickcellsandroid19;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.net.URISyntaxException;

import io.socket.client.IO;
import io.socket.client.Socket;
import io.socket.emitter.Emitter;

public class CommunicationModule {

    String IMEI = null;
    Socket socket;
    MainActivity _main_act;
    SidesDebug _side_debug;

    JSONArray obj_buff;
    String watch_id = "";
    long touch_framerate_ms = -1;

    int new_north = 0;
    int new_east  = 1;
    int new_south = 2;
    int new_west  = 3;


    @SuppressLint("MissingPermission")
    public String getDeviceIMEI() {
        if (IMEI == null) {
            IMEI = "";
            TelephonyManager tm = null;
            if (_main_act != null) {
                tm = (TelephonyManager) _main_act.getSystemService(Context.TELEPHONY_SERVICE);
            } else if (_side_debug != null) {
                tm = (TelephonyManager) _side_debug.getSystemService(Context.TELEPHONY_SERVICE);
            }
            if (null != tm) {
                IMEI = tm.getDeviceId();
            }
            if (null == IMEI || 0 == IMEI.length()) {
                if (_main_act != null) {
                    IMEI = Settings.Secure.getString(_main_act.getContentResolver(), Settings.Secure.ANDROID_ID);
                } else {
                    IMEI = Settings.Secure.getString(_side_debug.getContentResolver(), Settings.Secure.ANDROID_ID);
                }
            }
        }
        return IMEI;
    }

    public CommunicationModule(MainActivity main_act, SidesDebug side_debug) {
        _main_act = main_act;
        _side_debug = side_debug;
        try {

            // socket = IO.socket("http://172.20.10.2:9000");
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
                    touch_framerate_ms = 1000 / Integer.parseInt(obj.getString("touch_framerate"));
                } catch (JSONException e) {
                    obj = null;
                    watch_id = "";
                    touch_framerate_ms = -1;
                }

                obj = (JSONObject) args[0];
                try {
                    obj = obj.getJSONObject("params"); // get parameters
                    obj = obj.getJSONObject(IMEI); // try if inversions
                    new_north = Integer.parseInt(obj.getString("0"));
                    new_east  = Integer.parseInt(obj.getString("1"));
                    new_south = Integer.parseInt(obj.getString("2"));
                    new_west  = Integer.parseInt(obj.getString("3"));

                } catch (JSONException e) {
                    obj = null;
                    new_north = 0;
                    new_east  = 1;
                    new_south = 2;
                    new_west  = 3;
                }
                Log.d("IOIO","IMEI "+ IMEI);
                Log.d("IOIO","new_north "+ new_north);
                Log.d("IOIO","new_east "+ new_east);
                Log.d("IOIO","new_south "+ new_south);
                Log.d("IOIO","new_west "+ new_west);

                if (_main_act != null) {
                    _main_act.runOnUiThread(new Runnable() {
                        public void run() {
                            _main_act.textView.setText(IMEI);
                        }
                    });
                } else if (_side_debug != null) {
                    _side_debug.runOnUiThread(new Runnable() {
                        public void run() {
                            String s = "id: ";
                            Log.v("id: ", s + "\n" + getDeviceIMEI());
                            _side_debug.cubeIDLable.setText(s + getDeviceIMEI());

                        }
                    });
                }
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
                    if (_main_act != null) {
                        _main_act.runOnUiThread(new BitmapRunnable());
                    } else if (_side_debug != null) {
                        _side_debug.runOnUiThread(new BitmapRunnable());
                    }
                }
            }

        }).on(Socket.EVENT_DISCONNECT, new Emitter.Listener() {


            @Override
            public void call(Object... args) {
                if (_main_act != null) {
                    _main_act.runOnUiThread(new Runnable() {
                        public void run() {
                            _main_act.textView.setText("Disconnected...");
                        }
                    });
                } else if (_side_debug != null) {
                    _side_debug.runOnUiThread(new Runnable() {
                        public void run() {
                            _side_debug.cubeIDLable.setText("Disconnected...");
                            _side_debug.shutActivity();
                        }
                    });
                }
            }
        });
        socket.connect();
    }

    public void emitSocket(String msg, JSONObject obj) {
        socket.emit(msg,obj);
    }

    public int getIntSide(int side) {
        if (side == 0) {
            return new_north;
        } else if (side == 1) {
            return new_east;
        } else if (side == 2) {
            return new_south;
        } else if (side == 3) {
            return new_west;
        }
        return side;
    }

    public String getStringSide(String side) {
        if (side.contains("0")) {
            side = ""+new_north;
        } else if (side.contains("1")) {
            side = ""+new_east;
        } else if (side.contains("2")) {
            side = ""+new_south;
        } else if (side.contains("3")) {
            side = ""+new_west;
        }
        return side;
    }

    private class BitmapRunnable implements Runnable {
        public void run() {
            if (_main_act != null) {
                _main_act.textView.setText("buf len: " + obj_buff.length());
            } else if (_side_debug != null) {
                _side_debug.cubeIDLable.setText("buf len: " + obj_buff.length());
            }

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

                    if (_main_act != null) {
                        BitmapDrawable ob = new BitmapDrawable(_main_act.getResources(), bmp);
                        _main_act.layout.setBackground(ob);
                    } else if (_side_debug != null) {
                        BitmapDrawable ob = new BitmapDrawable(_side_debug.getResources(), bmp);
                        _side_debug.layout.setBackground(ob);
                    }
                } else {
                    Log.d("IOIO", "problem decoding image");
                }
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    }
}
