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
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.BitmapDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
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

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URISyntaxException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

import io.socket.client.IO;
import io.socket.client.Socket;
import io.socket.emitter.Emitter;

import java.lang.*;

import static android.view.KeyEvent.ACTION_DOWN;
import static android.view.MotionEvent.ACTION_UP;

public class MainActivity extends Activity implements View.OnClickListener, View.OnTouchListener {

    //bluetooth Stuff
    ImageButton On, Off, Discnt, Abt;
    String address = null;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    ConnectedThread readBT;
    TextView tb;

    Handler bluetoothIn;
    final int handlerState = 0;        				 //used to identify handler message
    private StringBuilder recDataString = new StringBuilder();

    Button buttonSend;
    TextView textView;
    RelativeLayout layout;
    Socket socket;

    JSONArray obj_buff;
    String watch_id = "";
    long touch_framerate_ms = (long) -1;
    Map<Integer, Long> framerate_map = new HashMap<Integer, Long>();

    String IMEI = null;

    int dialValue;

//    CellSensor cell_sensor = null;



    // UI elements
    private TextView messages;
    private EditText input;

    // BTLE state
    private BluetoothAdapter adapter;
    private BluetoothGatt gatt;
    private BluetoothGattCharacteristic tx;
    private BluetoothGattCharacteristic rx;

    int activeSides[] = new int[5];




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

        adapter = BluetoothAdapter.getDefaultAdapter();

        // Grab references to UI elements.
        messages = (TextView) findViewById(R.id.messages);
        messages.setText("No Changes");
        input = (EditText) findViewById(R.id.editText);

        buttonSend = (Button) findViewById(R.id.button);
        buttonSend.setOnClickListener(this);

        textView = (TextView) findViewById(R.id.textView2);
        layout = (RelativeLayout)findViewById(R.id.main_layout);

        layout.setOnTouchListener(this);

        Intent newint = getIntent();
        if (newint.getStringExtra(DeviceList.EXTRA_ADDRESS)!=null) {
            address = newint.getStringExtra(DeviceList.EXTRA_ADDRESS); //receive the address of the bluetooth device
            new ConnectBT().execute(); //Call the class to connect
        }
        final ViewGroup viewGroup = (ViewGroup) ((ViewGroup) this
                .findViewById(android.R.id.content)).getChildAt(0);
        bluetoothIn = new Handler() {
            public void handleMessage(android.os.Message msg) {
                if (msg.what == handlerState) {										//if message is what we want
                    String readMessage = (String) msg.obj;                                                                // msg.arg1 = bytes from connect thread

                    recDataString.append(readMessage);

//                  textViewtView.setText("Sensor Voltage = " + readMessage);	//update the textviews with sensor values
//                  viewGroup.setBackgroundColor(Color.argb(255, Integer.parseInt(readMessage), 0, 0));
                    int endOfLineIndex = recDataString.indexOf("~");                    // determine the end-of-line
                    if (endOfLineIndex > 0) {                                           // make sure there data before ~
                        String dataInPrint = recDataString.substring(0, endOfLineIndex);    // extract string
                        Log.v("Handler", "recDataString.charAt(0): "+recDataString.charAt(0));
                        if (recDataString.charAt(0) == '#')	{
                            Log.v("Handler", "readMessage: "+readMessage);
                            if (recDataString.charAt(2) == ':') {
                                String side = recDataString.substring(1,2);
                                String attachDirection = recDataString.substring(3, dataInPrint.length());
                                // Log.v("Handler", cubeID + " : "+attachDirection);
                                Log.v("Handler", "Active Side: " + side);
                                messages.setText("Active Side: " + side);

                                try {
                                    activeSides[Integer.parseInt(side)] = Integer.parseInt(attachDirection);
                                    newCubeAdd(Integer.parseInt(side));
                                } catch (NumberFormatException e) {
                                    Log.v("Handler", "Parse int error");
                                }


                                int emptySides = 0;
                                for (int i = 0; i < 4; i++){
                                    Log.v("Handler", "Active Side: " + activeSides[i]);
                                    if (activeSides[i] == 1) {
                                        messages.setText("Active Side(s): " + activeSides[0]+ ", " + activeSides[1]+ ", " + activeSides[2]+ ", " + activeSides[3]);

                                    } else {
                                        emptySides++;
                                    }
                                }
                                if (emptySides == 4){
                                    messages.setText("No Active Side");
                                }
                            }

//                            viewGroup.setBackgroundColor(Color.argb(255, Integer.parseInt(sensor0), 0, 0));

                        }
                        if (recDataString.charAt(0) == '*')	{
                            String dial = recDataString.substring(1,2);
                            Log.v("Handler", "Dial Value: " + dialValue);
                            dialValue = Integer.parseInt(dial);
                        }
                        recDataString.delete(0, recDataString.length()); 					//clear all string data
                    }
                }
            }
        };


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
                    touch_framerate_ms = (long) (1000 / Integer.parseInt(obj.getString("touch_framerate")));
                } catch (JSONException e) {
                    obj = null;
                    watch_id = "";
                    touch_framerate_ms = (long) -1;
                }

                runOnUiThread(new Runnable() {
                    public void run() {
                        textView.setText(watch_id);
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
                    textView.setText("buf len: " + obj_buff.length());

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
                        textView.setText("Disconnected...");
                    }
                });
            }
        });
        socket.connect();

//        cell_sensor = new CellSensor();
    }




    // Boilerplate code from the activity creation:

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

        socket.emit("clicked", obj);
    }

    public String indexToChar(int index){
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


    public void newCubeAdd(int side) {
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


            if ((touch_framerate_ms == -1) ||
                (motionEvent.getAction() == ACTION_DOWN) ||
                (motionEvent.getAction() == ACTION_UP) ||
                (elapsed >= touch_framerate_ms)) {
                framerate_map.put(ptr_id, System.currentTimeMillis());
                socket.emit("touchframe", obj);
            }
        }

        return true;
    }

    protected void onResume() {
        super.onResume();
        Log.d("SENSOR", "resume ??");
//        cell_sensor.mSensorManager.registerListener(
//                cell_sensor,
//                cell_sensor.accelerometer,
//                SensorManager.SENSOR_DELAY_NORMAL
//        );
    }

    protected void onPause() {
        super.onPause();
        Log.d("SENSOR", "pause ??");
//        cell_sensor.mSensorManager.unregisterListener(cell_sensor);
    }


//    private class CellSensor implements SensorEventListener {
//        SensorManager mSensorManager = null;
//        Sensor accelerometer = null;
//
//        float[] acc;
//
//        public CellSensor() {
//            mSensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
//            accelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
//            Log.d("SENSOR", "creation ??");
//            Log.d("SENSOR", "sensor list: "+mSensorManager.getSensorList(Sensor.TYPE_ALL).toString());
////            Log.d("SENSOR", "acc sensor: "+accelerometer.toString());
////            mSensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL);
////            mSensorManager.registerListener(this, mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER), SensorManager.SENSOR_DELAY_NORMAL);
//        }
//
//        @Override
//        public void onSensorChanged(SensorEvent sensorEvent) {
//            Log.d("SENSOR", "hey yo acceleroooo ??");
//
//            if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
//                Log.d("SENSOR", "hey yeah");
//                acc = sensorEvent.values;
//
//                // Sending an object
//                JSONObject obj = new JSONObject();
//                JSONObject touch = new JSONObject();
//
//                try {
//                    obj.put("ax", acc[0]);
//                    obj.put("ay", acc[1]);
//                    obj.put("az", acc[2]);
//
//                    obj.put("IMEI", getDeviceIMEI());
//                    obj.put("Touch", touch);
//                } catch (JSONException e) {
//                    e.printStackTrace();
//                }
//
//                socket.emit("sensor", obj);
//            }
//        }
//
//        @Override
//        public void onAccuracyChanged(android.hardware.Sensor sensor, int i) {
//            // Nothing
//            Log.d("SENSOR", "Accuracy");
//        }
//    };

    // fast way to call Toast
    private void msg(String s)
    {
        Toast.makeText(getApplicationContext(),s,Toast.LENGTH_LONG).show();
    }

    //create new class for connect thread
    private class ConnectedThread extends Thread {
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        //creation of the connect thread
        public ConnectedThread(BluetoothSocket socket) {
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try {
                //Create I/O streams for connection
                tmpIn = btSocket.getInputStream();
                tmpOut = btSocket.getOutputStream();
            } catch (IOException e) { }

            mmInStream = tmpIn;
            mmOutStream = tmpOut;
            Log.v("ConnectedThread",  "Constuctor");
        }



        public void run() {
            byte[] buffer = new byte[256];
            int bytes;
            // Keep looping to listen for received messages
            while (btSocket!=null) {
                try {
                    bytes = mmInStream.read(buffer);        	//read bytes from input buffer
                    String readMessage = new String(buffer, 0, bytes);
                    // Send the obtained bytes to the UI Activity via handler
                    bluetoothIn.obtainMessage(handlerState, bytes, -1, readMessage).sendToTarget();
                    Log.v("ConnectedThread", "msg: "+readMessage);

                } catch (IOException e) {
                    Log.v("ConnectedThread", "Error");
                }
            }
        }
        //write method
        public void write(String input) {
            byte[] msgBuffer = input.getBytes();           //converts entered String into bytes
            try {
                mmOutStream.write(msgBuffer);                //write bytes over BT connection via outstream
            } catch (IOException e) {
                //if you cannot write, close the application
                Toast.makeText(getBaseContext(), "Connection Failure", Toast.LENGTH_LONG).show();
                finish();

            }
        }
    }


    private class ConnectBT extends AsyncTask<Void, Void, Void>  // UI thread
    {
        private boolean ConnectSuccess = true; //if it's here, it's almost connected

        @Override
        protected void onPreExecute()
        {
            progress = ProgressDialog.show(MainActivity.this, "Connecting...", "Please wait!!!");  //show a progress dialog
        }

        @Override
        protected Void doInBackground(Void... devices) //while the progress dialog is shown, the connection is done in background
        {
            try
            {
                if (btSocket == null || !isBtConnected)
                {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();//get the mobile bluetooth device
                    BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);//connects to the device's address and checks if it's available
                    btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);//create a RFCOMM (SPP) connection
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    btSocket.connect();//start connection
                }
            }
            catch (IOException e)
            {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }


            // new Thread(p).start();
            readBT = new ConnectedThread(btSocket);
            new Thread(readBT).start();
            return null;
        }
        @Override
        protected void onPostExecute(Void result) //after the doInBackground, it checks if everything went fine
        {
            super.onPostExecute(result);

            if (!ConnectSuccess)
            {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            }
            else
            {
                msg("Connected.");
                isBtConnected = true;
            }
            progress.dismiss();
        }
    }
}