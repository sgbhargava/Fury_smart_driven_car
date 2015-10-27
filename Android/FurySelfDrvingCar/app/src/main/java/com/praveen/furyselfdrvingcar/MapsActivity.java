package com.praveen.furyselfdrvingcar;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Message;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;


public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

    private GoogleMap mMap;
    private Button bn3;
    private Button bn2;
    private BluetoothAdapter BA;
    private connectThread mConnectTh;
    private AcceptThread mAccept;
    private connectedThread mConnected;
    public static String UUID_BT = "00001101-0000-1000-8000-00805f9b34fb";
    public static String DeviceAddr = "00:06:66:72:6D:70";
    public static String data = "$32m23s45";
    public static LatLng sLoc=null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("BT", "Entered OnCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
        setDestination(mapFragment);
        BluetoothSetting();

        Log.d("BT", "Exited OC");

    }
    private void setDestination(SupportMapFragment mapFragment) {
        Log.d("BT", "Entered SD");
        final GoogleMap googleMap = mapFragment.getMap();

        // Setting a click event handler for the map
        googleMap.setOnMapLongClickListener(new GoogleMap.OnMapLongClickListener() {

            @Override
            public void onMapLongClick(LatLng latLng) {
                sLoc=latLng;

                // Creating a marker
                MarkerOptions markerOptions = new MarkerOptions();

                // Setting the position for the marker
                markerOptions.position(latLng);

                // Setting the title for the marker.
                // This will be displayed on taping the marker
                markerOptions.title(latLng.latitude + " : " + latLng.longitude);

                // Clears the previously touched position
                //googleMap.clear();

                // Animating to the touched position
                googleMap.animateCamera(CameraUpdateFactory.newLatLng(latLng));

                // Placing a marker on the touched position
                googleMap.addMarker(markerOptions);
            }
        });
        Log.d("BT", "Exited SD");
    }





    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        Log.d("BT", "Entered OMR");
        mMap = googleMap;


        LatLng initialLocation = new LatLng(37.33538164, -121.88124232);
        CameraPosition cameraPosition = new CameraPosition.Builder()
                .target(initialLocation)      // Sets the center of the map to LatLng (refer to previous snippet)
                .zoom(18)                   // Sets the zoom
                .bearing(90)                // Sets the orientation of the camera to east
                .tilt(30)                   // Sets the tilt of the camera to 30 degrees
                .build();                   // Creates a CameraPosition from the builder
        mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
        mMap.addMarker(new MarkerOptions().position(initialLocation).title("Initial Car Location").snippet("37.33538164, -121.88124232"));
        Log.d("BT", "Exited OMR");
        //mMap.moveCamera(CameraUpdateFactory.newLatLng(initialLocation));
    }



    private void BluetoothSetting() {
        Log.d("BT", "Entered BS");
        BA = BluetoothAdapter.getDefaultAdapter();

        if (BA == null) {
            Log.d("BT", "Entered BS1");
           Toast.makeText(this, "No bluetooth adapter found", Toast.LENGTH_LONG).show();
        } else {
            Log.d("BT", "Entered BS2");
            bn3 = (Button) findViewById(R.id.Bluetooth);
            bn2 = (Button) findViewById(R.id.StartButton);

            bn2.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.d("BT", "Entered BS");
                   // Toast.makeText(getApplicationContext(), "Pressed start", Toast.LENGTH_SHORT).show();
                    byte[] temp = null;
                    if(sLoc!=null) {
                        send(temp);
                    }else
                        Toast.makeText(getApplicationContext(), "Please Set Destination Location", Toast.LENGTH_SHORT).show();
                }
            });

            bn3.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    connect(v);
                }
            });
            on();
        }
        Log.d("BT", "Exited BS");
    }
    public void send( byte[] d)
    {
        final double lat = sLoc.latitude;
        final double longitude = sLoc.longitude;
        final int laDegree= (int) Math.abs(lat);
        final double min= ((lat%1)*60);
        final int laMin= (int) min;
        final int laSec= (int) (min*60);
        final int loDegree= (int) Math.abs(longitude);
        final double min1= ((longitude%1)*60);
        final int loMin= (int) min;
        final int loSec= (int) (min*60);
        StringBuffer data= new StringBuffer();
        data.append('$');
        data.append(laDegree);
        data.append(',');
        data.append(laMin);
        data.append(',');
        data.append(laSec);
        data.append('#');
        data.append(loDegree);
        data.append(',');
        data.append(loMin);
        data.append(',');
        data.append(loSec);
        String dataStr=data.toString();
        byte[] t = dataStr.getBytes();
        if(mConnected!=null)
        {
            mConnected.write(t);
        }
    }
    public void start(BluetoothDevice device)
    {
        if( mConnected != null)
        {
            mConnected.cancel();
        }
        if(mConnectTh != null)
        {
            mConnectTh.cancel();
        }
        mConnectTh = new connectThread(device);
        mConnectTh.start();

    }
    protected void onResume( )
    {
        super.onResume();
        on();
    }

    @Override
    protected void onActivityResult (int requestCode, int resultCode, Intent data)
    {
        switch(requestCode)
        {
            case 1:
            {
                Toast.makeText(this,"Bluetooth turned on",Toast.LENGTH_SHORT).show();
            }
        }
    }

    public void on()
    {
        if( !BA.isEnabled())
        {
            Intent enableBt = new Intent(BA.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBt, 1);

        }
    }

    private final android.os.Handler reciever = new android.os.Handler(){
        public void handleMessage(Message msg) {
            switch(msg.what)
            {
                case 2:
                {
                    byte[] readBuf = (byte[]) msg.obj;
                    String read = new String(readBuf,0,msg.arg1);
                    Toast.makeText(getApplicationContext(),read,Toast.LENGTH_LONG).show();

                }
            }
        }
    };

    public void connect(View v)
    {
        if (BA.isDiscovering())
        {
            BA.cancelDiscovery();
        }

        BluetoothDevice device = BA.getRemoteDevice(DeviceAddr);

        if(device == null)
        {
            Toast.makeText(getApplicationContext(),"Device not available, Check!!",Toast.LENGTH_LONG).show();
        }
        else {
            start(device);
        }
        //Toast.makeText(this,"displaying",Toast.LENGTH_LONG).show();
    }

    private class connectThread extends Thread{

        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        public connectThread(BluetoothDevice device)
        {
            BluetoothSocket t = null;
            mmDevice =device;

            try{

                t = device.createRfcommSocketToServiceRecord(UUID.fromString(UUID_BT));
                Log.d("BT", "get socket for " + device.getAddress());

            }
            catch(IOException e){
                Toast.makeText(getApplicationContext(),"Device not in range",Toast.LENGTH_SHORT).show();
                Log.d("BT", "get socket failed");
            }

            mmSocket =t;


        }

        public void run()
        {
            BA.cancelDiscovery();

            try{
                if(mmSocket != null)
                    mmSocket.connect();
                else
                    return;
                Log.d("BT", "Connecting");
            }catch(IOException e )
            {
                Log.d("BT", "Connecting failed");

                try{
                    mmSocket.close();
                    return;
                }catch (IOException e1){
                    return;
                }

            }

            mConnected = new connectedThread(mmSocket);
            mConnected.start();

        }
        public void cancel()
        {
            try{
                mmSocket.close();
            }catch (IOException e){

            }
        }
    }

    private class AcceptThread extends Thread{
        private final BluetoothServerSocket mmServerSocket;


        public AcceptThread()
        {
            BluetoothServerSocket t = null;

            try
            {
                Log.d("BT", "get socket");
                t = BA.listenUsingRfcommWithServiceRecord("RC CAR", UUID.fromString(UUID_BT));
            }catch (IOException e)
            {

            }

            mmServerSocket = t;
        }

        public void run()
        {
            BluetoothSocket socket = null;
            while(true) {
                try {
                    socket = mmServerSocket.accept();
                } catch (IOException e) {
                    break;
                }
                if (socket != null) {
                    // spawn connected thread
                    try {
                        mmServerSocket.close();
                        break;
                    } catch (IOException e) {
                        break;
                    }

                }
            }
        }

        public void cancel() {
            try {
                mmServerSocket.close();
            } catch (IOException e) { }
        }
    }

    private class connectedThread extends Thread{
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public connectedThread(BluetoothSocket Socket)
        {
            mmSocket = Socket;
            InputStream tIn = null;
            OutputStream tOut = null;

            try{
                tIn = Socket.getInputStream();
                tOut = Socket.getOutputStream();

            }catch (IOException e)
            {

            }
            mmInStream =tIn;
            mmOutStream = tOut;

        }

        public void run()
        {
            byte[] buffer = new byte[1024];
            int bytes ;
            while(true)
            {
                try{
                    bytes = mmInStream.read(buffer);
                    reciever.obtainMessage(2,bytes,-1,buffer).sendToTarget();
                }catch (IOException e)
                {
                    break;
                }
            }
        }

        public void write(byte[] bytes)
        {
            try{
                mmOutStream.write(bytes);
            }catch (IOException e)
            {

            }
        }

        public void cancel()
        {
            try {
                mmSocket.close();
            }catch (IOException e)
            {

            }
        }
    }
}








