package com.example.alixgoguey.pickcellsandroid19;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
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

import org.json.JSONException;
import org.json.JSONObject;

public class SidesDebug extends Activity implements View.OnClickListener{
    Button btnNorth, btnSouth, btnEast, btnWest, btnBottom, btnTop;

    CommunicationModule com = null;
    String IMEI = null;

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

        com = new CommunicationModule(null, this);
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

        com.emitSocket("sideChange", obj);
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

    public void shutActivity() {
        endActivity();
    }
}
