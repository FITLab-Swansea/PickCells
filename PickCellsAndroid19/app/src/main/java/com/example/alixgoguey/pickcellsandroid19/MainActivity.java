package com.example.alixgoguey.pickcellsandroid19;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import java.lang.*;

import static android.view.KeyEvent.ACTION_DOWN;
import static android.view.MotionEvent.ACTION_UP;

public class MainActivity extends Activity implements View.OnClickListener, View.OnTouchListener {

    String address = null;
    SidesConnectThread sidesConnectThread;
    Button buttonSend;
    TextView textView;
    RelativeLayout layout;
    CommunicationModule com = null;
    Map<Integer, Long> framerate_map = new HashMap<Integer, Long>();
    String IMEI = null;

//    int dialValue;

    private TextView messages;
    private EditText input;


    ArduinoModule arduinoModule;
    Handler uiUpdate;


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



    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);


        // Grab references to UI elements.
        messages = findViewById(R.id.messages);
        messages.setText("No Changes");
        input = findViewById(R.id.editText);

        buttonSend = findViewById(R.id.button);
        buttonSend.setOnClickListener(this);

        textView = findViewById(R.id.textView2);
        layout = findViewById(R.id.main_layout);

        layout.setOnTouchListener(this);

        Intent newInt = getIntent();
        if (newInt.getStringExtra(DeviceList.EXTRA_ADDRESS)!=null) {
            address = newInt.getStringExtra(DeviceList.EXTRA_ADDRESS); //receive the address of the bluetooth device
        }

        final ViewGroup viewGroup = (ViewGroup) ((ViewGroup) this
                .findViewById(android.R.id.content)).getChildAt(0);

        arduinoModule = new ArduinoModule(address);
        com = new CommunicationModule(this,null);

        sidesConnectThread = new SidesConnectThread();
        new Thread(sidesConnectThread).start();

        uiUpdate = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                if (arduinoModule.emptySides() == 0) {
                    messages.setText("No Active Side");
                } else {
                    messages.setText(arduinoModule.printActiveSides());
                }
            }
        };

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {

        // Inflate the menu; this adds items to the action bar if it is present.
        //getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
//        if (id == R.id.action_settings) {
//            return true;
//        }
        return super.onOptionsItemSelected(item);
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

        com.emitSocket("clicked", obj);
    }



    public void newCubeAdd(int side) {
        JSONObject obj = new JSONObject();
        try {
            obj.put("IMEI", getDeviceIMEI());
            obj.put("side", arduinoModule.SIDENAMES[side]);
            obj.put("active", arduinoModule.getActiveSides()[side]);

        } catch (JSONException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        com.emitSocket("sideChange", obj);
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        // TODO: add code to send the active side
        int ptr_id = motionEvent.getPointerId(motionEvent.getActionIndex());
        if (ptr_id == 0) {
            // Sending an object
            JSONObject obj = new JSONObject();
            JSONObject touch = new JSONObject();
            try {
                obj.put("x", motionEvent.getX());
                obj.put("y", motionEvent.getY());
                obj.put("id", ptr_id);
                obj.put("type", motionEvent.getAction());

                obj.put("IMEI", getDeviceIMEI());
                obj.put("Touch", touch);
            } catch (JSONException e) {
                e.printStackTrace();
            }

            if (!framerate_map.containsKey(ptr_id)) {
                framerate_map.put(ptr_id, (long) 0);
            }
            long elapsed = System.currentTimeMillis() - framerate_map.get(ptr_id);


            if ((com.touch_framerate_ms == -1) ||
                (motionEvent.getAction() == ACTION_DOWN) ||
                (motionEvent.getAction() == ACTION_UP) ||
                (elapsed >= com.touch_framerate_ms)) {
                framerate_map.put(ptr_id, System.currentTimeMillis());

                com.emitSocket("touchframe", obj);
            }
        }

        return true;
    }

    protected void onResume() {
        super.onResume();
        Log.d("SENSOR", "resume ??");
    }

    protected void onPause() {
        super.onPause();
        Log.d("SENSOR", "pause ??");
    }

    // fast way to call Toast
    public  void msg(String s) {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }

    //create new class for connect thread
    private class SidesConnectThread extends Thread {
        public void run() {
            while(true) {
                if (arduinoModule.isNewSide()){
                    newCubeAdd(arduinoModule.getLastSide());
                    uiUpdate.sendEmptyMessage(0);
                }
                arduinoModule.setNewSide(false);
            }
        }

    }
}