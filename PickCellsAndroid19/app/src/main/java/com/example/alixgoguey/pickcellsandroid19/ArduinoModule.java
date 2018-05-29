package com.example.alixgoguey.pickcellsandroid19;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.os.AsyncTask;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.util.UUID;

public class ArduinoModule {

    private String address;
    BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    ConnectedThread readBT;

    Handler bluetoothIn;
    final int handlerState = 0;

    private StringBuilder recDataString = new StringBuilder();
    private int[] activeSides = new int[5];
    private boolean newSide;
    private int lastSide;

    ArduinoModule(String address){
        this.address = address;
        new ConnectBT().execute();
        setBTHandler();
    }

    @SuppressLint("HandlerLeak")
    private void setBTHandler() {
        bluetoothIn = new Handler() {
            public void handleMessage(android.os.Message msg) {
                if (msg.what == handlerState) {
                    String readMessage = (String) msg.obj;
                    recDataString.append(readMessage);
                    int endOfLineIndex = recDataString.indexOf("~");
                    if (endOfLineIndex > 0) {
                        String dataInPrint = recDataString.substring(0, endOfLineIndex);
                        if (recDataString.charAt(0) == '#')	{
                            if (recDataString.charAt(2) == ':') {
                                activeSides[parseInt(recDataString.substring(1,2))] = parseInt(recDataString.substring(3, dataInPrint.length()));
                                lastSide = parseInt(recDataString.substring(1,2));
                                setNewSide(true);
                            }
                        }
                        recDataString.delete(0, recDataString.length());
                    }
                }
            }
        };
    }

    private int parseInt(String string){
        int integer;
        try {
            integer = Integer.parseInt(string);
        } catch (NumberFormatException e) {
            Log.v("Handler", "Parse int error");
            integer = -1;
        }
        return integer;
    }

    public int[] getActiveSides() {
        return activeSides;
    }

    public String printActiveSides(){
        Log.v("ArduinoModule","Active Side(s): " + this.getActiveSides()[0]+ ", " + this.getActiveSides()[1]+ ", " + this.getActiveSides()[2]+ ", " + this.getActiveSides()[3] + ", " + this.getActiveSides()[4]);
        return ("Active Side(s): " + this.getActiveSides()[NORTH]+ ", " + this.getActiveSides()[EAST]+ ", " + this.getActiveSides()[SOUTH]+ ", " + this.getActiveSides()[WEST] + ", " + this.getActiveSides()[BOTTOM]);
    }

    public boolean isNewSide() {
        return newSide;
    }

    public void setNewSide(boolean newSide) {
        this.newSide = newSide;
    }

    public int getLastSide() {
        return lastSide;
    }
    public int emptySides(){
        int emptySides = 0;
        for (int i = 0; i < 5; i++) {
            if (this.getActiveSides()[i] == 0) {
                emptySides++;
            }
        }
        return emptySides;
    }

    private class ConnectedThread extends Thread {
        private final InputStream mmInStream;
        public ConnectedThread(BluetoothSocket socket) {
            InputStream tmpIn = null;
            try {
                tmpIn = btSocket.getInputStream();
            } catch (IOException e) {
                e.printStackTrace();
            }
            mmInStream = tmpIn;
        }

        public void run() {
            byte[] buffer = new byte[256];
            int bytes;
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
    }


    private class ConnectBT extends AsyncTask<Void, Void, Void> {
        private boolean ConnectSuccess = true; //if it's here, it's almost connected

        @Override
        protected void onPreExecute() {
            //progress = ProgressDialog.show(context, "Connecting...", "Please wait!!!");  //show a progress dialog
            Log.v("ConnectBT", "Connecting... Please wait!!!");
        }

        @Override
        protected Void doInBackground(Void... devices) {
            try {
                if (btSocket == null || !isBtConnected) {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();//get the mobile bluetooth device
                    BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);//connects to the device's address and checks if it's available
                    btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);//create a RFCOMM (SPP) connection
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    btSocket.connect();//start connection
                }
            } catch (IOException e) {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }

            readBT = new ConnectedThread(btSocket);
            new Thread(readBT).start();
            return null;
        }
        @Override
        protected void onPostExecute(Void result) {
            super.onPostExecute(result);
            if (!ConnectSuccess) {
                Log.v("ConnectBT", "Connection Failed. Is it a SPP Bluetooth? Try again.");
                // finish();
            } else {
                // msg("Connected.");
                Log.v("ConnectBT", "Connected.");

                isBtConnected = true;
            }
            // progress.dismiss();
        }
    }

    final static String[] SIDENAMES =  {"North", "East","South","West","Bottom","Top"};

    final static int BOTTOM = 4;
    final static int WEST = 3;
    final static int SOUTH = 2;
    final static int EAST = 1;
    final static int NORTH = 0;

}


