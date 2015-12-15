package com.praveen.furyselfdrvingcar;

import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.location.Criteria;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Message;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.UUID;
import java.util.Vector;


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
    public static LatLng sLoc = null;
    public static Location iLoc = null;
    public JSONObject checkObj = null;
    public SupportMapFragment mapFragment = null;
    public static TextView rpm;
    public static String lSensor=null;
    public static String cSensor=null;
    public static String rSensor=null;
    public static String bSensor=null;
    public static String canBattery=null;
    public static String carBattery=null;
    public static String remdist=null;
    private StringBuilder receiveData = new StringBuilder();
    public static int sensorFlag=0;
    public static int baterryFlag=0;
    public static int remDistFlag=0;
    public  AlertDialog alertDialog;
    public static int bluetoothCon=0;

    LocationManager lm =null;
    public Double totalDistance;
    public Vector<checkPointLoc> v = new Vector<checkPointLoc>();

    public class checkPointLoc {
        public int checkNo = 0;
        public double lat = 0;
        public double lng = 0;
        public int isFinal = 0;
    }

    public class httpTask extends AsyncTask<URL, Void, JSONObject> {

        JSONObject direction;

        @Override
        protected JSONObject doInBackground(URL... h) {
            String url = null;
            StringBuilder stringBuilder = new StringBuilder();


            Log.d("url", "1");
            HttpURLConnection con = null;
            try {
                con = (HttpURLConnection) h[0].openConnection();
                con.setReadTimeout(10000);
                con.setConnectTimeout(15000);
                con.setDoInput(true);
                InputStreamReader stream = new InputStreamReader(con.getInputStream());
                //InputStream stream=new BufferedInputStream(conn.getInputStream());

                Log.d("http", "stream");
                int b;
                while ((b = stream.read()) != -1) {
                    stringBuilder.append((char) b);
                }
                Log.d("HTTP", "pass");
            } catch (Exception e) {
                Log.d("HTTP", "Fail");
                e.printStackTrace();
            }
            //Log.d("str",stringBuilder.toString());

            try {
                direction = new JSONObject(stringBuilder.toString());
            } catch (Exception e) {

            }

            return direction;
        }

        protected void onPostExecute(JSONObject result) {

            super.onPostExecute(result);
            Log.d("onpost", "hh");
            // this.notify();
            checkObj = result;

            if (checkObj != null) {
                try {

                    JSONArray routeArray = checkObj.getJSONArray("routes");
                    JSONObject routes = routeArray.getJSONObject(0);
                    JSONArray legsArray = routes.getJSONArray("legs");
                    JSONObject legs = legsArray.getJSONObject(0);

                    String str = new String();

                    JSONObject distance = legs.getJSONObject("distance");

                    str = distance.getString("value");
                    totalDistance = Double.valueOf(str);

                    JSONArray stepsArray = legs.getJSONArray("steps");

                    for (int i = 0; i < stepsArray.length(); i++) {

                        checkPointLoc location = new checkPointLoc();
                        location.checkNo = i + 1;
                        Log.d("check", String.valueOf(stepsArray.length()));
                        JSONObject steps = stepsArray.getJSONObject(i);
                        Log.d("steps", steps.toString());
                        Double lat = null;
                        Double lng = null;
                        lat = steps.getJSONObject("end_location").getDouble("lat");
                        lng = steps.getJSONObject("end_location").getDouble("lng");
                        location.lat = lat;
                        location.lng = lng;

                        location.isFinal = 0;

                        if (location.checkNo == stepsArray.length()) {
                            location.isFinal = 1;

                        }
                        Log.d("block", "HER1");
                        v.add(location);
                        Log.d("block", "HER2");
                        checkPointLoc t = v.get(i);
                        Log.d("block", "HER3");
                        Log.d("CheckNO", String.valueOf(t.checkNo));
                        Log.d("lat", String.valueOf(t.lat));
                        Log.d("long", String.valueOf(t.lng));
                        Log.d("IsFinal", String.valueOf(t.isFinal));
                    }

                } catch (Exception e) {
                }

                reciever.obtainMessage(3).sendToTarget();
            }

        }
    }

    public void mapActivity() {
        setContentView(R.layout.activity_maps);
        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
        setDestination(mapFragment);
        BluetoothSetting();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d("BT", "Entered OnCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);
        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
        //  mapActivity();
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

                sLoc = latLng;


                // Creating a marker

                LatLng iLatLng = new LatLng(iLoc.getLatitude(), iLoc.getLongitude());

                MarkerOptions iMarkerOptions = new MarkerOptions();
                MarkerOptions markerOptions = new MarkerOptions();

                iMarkerOptions.position(iLatLng);
                iMarkerOptions.title(iLatLng.latitude + " : " + iLatLng.longitude);

                markerOptions.position(latLng);

                markerOptions.title(latLng.latitude + " : " + latLng.longitude);

                // Clears the previously touched position
                googleMap.clear();
                googleMap.animateCamera(CameraUpdateFactory.newLatLng(iLatLng));
                googleMap.addMarker(iMarkerOptions);
                // Animating to the touched position
                googleMap.animateCamera(CameraUpdateFactory.newLatLng(latLng));

                // Placing a marker on the touched position

                googleMap.addMarker(markerOptions);
                String url = ("https://maps.googleapis.com/maps/api/directions/json?origin=" + iLoc.getLatitude() + "," + iLoc.getLongitude() + "&destination="
                        + sLoc.latitude + "," + sLoc.longitude + "&mode=walking&sensor=false&units=metric");
                try {
                    URL link = new URL(url);
                    Log.d("url", url);
                    httpTask ht = new httpTask();
                    if (v != null) {
                        v.clear();
                    }
                    ht.execute(link);

                } catch (MalformedURLException e) {
                    e.printStackTrace();
                }

            }
        });
        Log.d("BT", "Exited SD");
    }
    final LocationListener listener = new LocationListener() {
        @Override


        public void onLocationChanged(Location location) {

            iLoc = location;
            //Double data[]={iLoc.getLatitude(),iLoc.getLongitude()};
            //locationHash.put(1,data);

            LatLng initialLocation = new LatLng(iLoc.getLatitude(), iLoc.getLongitude());
            //   LatLng initialLocation = new LatLng(37.33538164, -121.88124232);
            Log.e("GPS", String.valueOf(iLoc.getLatitude()));
            Log.e("GPS", String.valueOf(iLoc.getLongitude()));
            CameraPosition cameraPosition = new CameraPosition.Builder()
                    .target(initialLocation)      // Sets the center of the map to LatLng (refer to previous snippet)
                    .zoom(18)                   // Sets the zoom
                    .bearing(90)                // Sets the orientation of the camera to east
                    .tilt(30)                   // Sets the tilt of the camera to 30 degrees
                    .build();                   // Creates a CameraPosition from the builder
            mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
            mMap.addMarker(new MarkerOptions().position(initialLocation).title("Initial Car Location").snippet("37.33538164, -121.88124232"));
            Log.d("BT", "Exited OMR");

            reciever.obtainMessage(4).sendToTarget();

        }

        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {

            Toast.makeText(getApplicationContext(), "status changed", Toast.LENGTH_LONG).show();

        }

        @Override
        public void onProviderEnabled(String provider) {

        }

        @Override
        public void onProviderDisabled(String provider) {

        }
    };



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
    public void onMapReady(final GoogleMap googleMap) {
        Log.d("BT", "Entered OMR");
        mMap = googleMap;
        String provider;
        //get location service
        lm = (LocationManager) this.getSystemService(Context.LOCATION_SERVICE);
        Location l = new Location(LocationManager.GPS_PROVIDER);

        Criteria c = new Criteria();
        c.setAccuracy(Criteria.ACCURACY_FINE);
        lm.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0, 0, listener);

        Log.d("BT", "Entered OMR");
        provider = lm.getBestProvider(c, false);
        Log.d("GPS",provider);
        TextView map= (TextView) findViewById(R.id.mapReset);

        map.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sLoc=null;
                iLoc=null;
                bluetoothCon=0;
                
                googleMap.clear();
                String provider;
                //get location service

                Location l = new Location(LocationManager.GPS_PROVIDER);

                Criteria c = new Criteria();
                c.setAccuracy(Criteria.ACCURACY_FINE);
                lm.requestLocationUpdates(LocationManager.GPS_PROVIDER, 0, 0, listener);

                Log.d("BT", "Entered OMR");
                provider = lm.getBestProvider(c, false);
                Log.d("GPS",provider);

            }
        });
    }


    private void BluetoothSetting() {
        Log.d("BT", "Entered BS");
        BA = BluetoothAdapter.getDefaultAdapter();

        if (BA == null) {
            Log.d("BT", "Entered BS1");
            Toast.makeText(this, "No bluetooth adapter found", Toast.LENGTH_LONG).show();
        } else {
            Log.d("BT", "Entered BS2");
            Button bn1 = (Button) findViewById(R.id.Bluetooth);
            bn2 = (Button) findViewById(R.id.StartButton);
           // bn3 = (Button) findViewById(R.id.InitLoc);


            bn2.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    Log.d("BT", "Entered BS");
                    // Toast.makeText(getApplicationContext(), "Pressed start", Toast.LENGTH_SHORT).show();
                    byte[] temp = null;
                    if (sLoc != null) {
                        send("start$");
                    } else
                        Toast.makeText(getApplicationContext(), "Please Set Destination Location", Toast.LENGTH_SHORT).show();
                }
            });

            bn1.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    connect(v);
                }
            });
            on();

            TextView sensor= (TextView) findViewById(R.id.sensor);

            sensor.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    sensorFlag=1;
                    baterryFlag=0;
                    remDistFlag=0;
                    alertDialog = new AlertDialog.Builder(MapsActivity.this).create();
                    alertDialog.setTitle("Sensor Values");
                    alertDialog.setMessage("Left Sensor Value : " + lSensor + "\n" + "Right Sensor Value : " + rSensor + "\n" + "Center Sensor Value : " + cSensor
                            + "\n" + "Back Sensor Value : " + bSensor);
                    alertDialog.setButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.cancel();
                        }
                    });
                    alertDialog.show();

                }
            });
            TextView battery= (TextView) findViewById(R.id.Battery);

            battery.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    sensorFlag=0;
                    baterryFlag=1;
                    remDistFlag=0;
                    alertDialog = new AlertDialog.Builder(MapsActivity.this).create();
                    alertDialog.setTitle("Battery Values");
                    alertDialog.setMessage("Can Battery : " + canBattery + "\n" + "Car Battery : " + carBattery );
                    alertDialog.setButton("Cancel", new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            dialog.cancel();
                        }
                    });
                    alertDialog.show();

                }
            });
        }
        Log.d("BT", "Exited BS");
    }

    public void send(String dataStr) {


        byte[] t = dataStr.getBytes();
        if (mConnected != null) {
            mConnected.write(t);
        }
    }

    public void start(BluetoothDevice device) {
        if (mConnected != null) {
            mConnected.cancel();
        }
        if (mConnectTh != null) {
            mConnectTh.cancel();
        }
        mConnectTh = new connectThread(device);
        mConnectTh.start();

    }

    protected void onResume() {
        super.onResume();
        on();
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {
            case 1: {
                Toast.makeText(this, "Bluetooth turned on", Toast.LENGTH_SHORT).show();
            }
        }
    }

    public void on() {
        if (!BA.isEnabled()) {
            Intent enableBt = new Intent(BA.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBt, 1);

        }
    }


    private final android.os.Handler reciever = new android.os.Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case 1:{

                    String s = (String) msg.obj;
                    Toast.makeText(getApplicationContext(), s, Toast.LENGTH_SHORT).show();
                }
                break;
                case 2: {
                   // byte[] readBuf = (byte[]) msg.obj;
                    //StringBuffer receiveData = new StringBuffer();
                    String read = (String) msg.obj;
                    //String read = new String(readBuf, 0, msg.arg1);
                    //Toast.makeText(getApplicationContext(), read, Toast.LENGTH_LONG).show();
                    receiveData.append(read);

                    Log.d("msg", receiveData.toString());//keep appending to string until ~

                    int endIndex = receiveData.indexOf("~");
                    if (endIndex>0)
                    {
                        if(receiveData.charAt(0)=='r')
                        {
                            reciever.obtainMessage(1, (Object) String.valueOf("Retransmit Destination Location")).sendToTarget();
                            //Toast.makeText(getApplicationContext(), "Retransmit Destination Location", Toast.LENGTH_SHORT).show();
                        }

                        if(receiveData.charAt(0)=='a')

                        {

                            reciever.obtainMessage(1,(Object)String.valueOf("Destination Fixed")).sendToTarget();
                            //Toast.makeText(getApplicationContext(), "Destination Fixed", Toast.LENGTH_SHORT).show();
                        }
                        if(receiveData.charAt(0)=='v')
                        {
                            int rpmEnd=receiveData.indexOf("~");

                            String RPM=receiveData.substring(1, rpmEnd);

                            rpm.setText(RPM);
                        }
                        if(receiveData.charAt(0)=='s')
                        {

                        }
                        if(receiveData.charAt(0)=='b')
                        {

                        }
                        if(receiveData.charAt(0)=='l')
                        {

                        }
                    }

                    if(sensorFlag==1)
                    {

                        alertDialog.setTitle("Sensor Values");
                        alertDialog.setMessage("Left Sensor Value : " + lSensor + "\n" + "Right Sensor Value : " + rSensor + "\n" + "Center Sensor Value : " + cSensor
                                + "\n" + "Back Sensor Value : " + bSensor);
                        alertDialog.setButton("Cancel", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.cancel();
                            }
                        });
                        //alertDialog.show();
                    }
                    else if(baterryFlag==1)
                    {

                        alertDialog.setTitle("Battery Values");
                        alertDialog.setMessage("Can Battery : " + canBattery + "\n" + "Car Battery : " + carBattery);
                        alertDialog.setButton("Cancel", new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                dialog.cancel();
                            }
                        });
                        //alertDialog.show();
                    }

                }
                break;

                case 3: {
                    Button confirnButton = (Button) findViewById(R.id.InitLoc);

                    confirnButton.setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            AlertDialog alertDialog = new AlertDialog.Builder(MapsActivity.this).create();

                            alertDialog.setMessage("Are you Sure?");
                            alertDialog.setButton("Cancel", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {
                                    dialog.cancel();
                                }
                            });
                            alertDialog.setButton("Yes", new DialogInterface.OnClickListener() {
                                @Override
                                public void onClick(DialogInterface dialog, int which) {

                                    if (sLoc != null  ) {
                                        if(bluetoothCon!=1) {
                                            MarkerOptions cMarkerOptions = new MarkerOptions();
                                            checkPointLoc loc = new checkPointLoc();
                                            final GoogleMap googleMap = mapFragment.getMap();
                                            googleMap.clear();
                                            LatLng initMarker = new LatLng(iLoc.getLatitude(), iLoc.getLongitude());
                                            cMarkerOptions.position(initMarker);
                                            cMarkerOptions.title(initMarker.latitude + " : " + initMarker.longitude);
                                            //GoogleMap googleMap = null;
                                            googleMap.animateCamera(CameraUpdateFactory.newLatLng(initMarker));

                                            // Placing a marker on the touched position
                                            googleMap.addMarker(cMarkerOptions);
                                            for (int j = 0; j < v.size(); j++) {
                                                Log.d("CM", String.valueOf(v.size()));
                                                loc = v.get(j);
                                                LatLng ml = new LatLng(loc.lat, loc.lng);
                                                Log.d("lat", String.valueOf(loc.lat));
                                                Log.d("lng", String.valueOf(loc.lng));

                                                cMarkerOptions.position(ml);
                                                cMarkerOptions.title(ml.latitude + " : " + ml.longitude);
                                                //GoogleMap googleMap = null;
                                                googleMap.animateCamera(CameraUpdateFactory.newLatLng(ml));
                                                StringBuilder strData = new StringBuilder();
                                                strData.append("check");

                                                strData.append(' ');
                                                strData.append(String.valueOf(iLoc.getLatitude()));
                                                strData.append(' ');
                                                strData.append(String.valueOf(iLoc.getLongitude()));
                                                strData.append(' ');
                                                strData.append(String.valueOf('0'));
                                                strData.append(' ');
                                                strData.append(String.valueOf('0'));
                                                strData.append('$');

                                                // Placing a marker on the touched position
                                                googleMap.addMarker(cMarkerOptions);
                                                // BlueToothActivity.activity = "maps";
                                                //Intent sendCheckpoints = new Intent(MapsActivity.this,BlueToothActivity.class);

                                                checkPointLoc sendLoc = new checkPointLoc();
                                                for (int i = 0; i < v.size(); i++) {
                                                    sendLoc = v.get(i);
                                                    strData.append("check");
                                                    strData.append(' ');
                                                    strData.append(String.valueOf(sendLoc.lat));
                                                    strData.append(' ');
                                                    strData.append(String.valueOf(sendLoc.lng));
                                                    strData.append(' ');

                                                    strData.append(String.valueOf(sendLoc.checkNo));
                                                    strData.append(' ');
                                                    strData.append(String.valueOf(sendLoc.isFinal));
                                                    strData.append('$');
                                                }
                                                send(strData.toString());
                                                Log.d("Sending", strData.toString());

                                                //Intent sendCheckpoints = new Intent(MapsActivity.this, BlueToothActivity.class);
                                                //sendCheckpoints.putExtra("checkPoints", strData.toString());
                                                //startActivity(sendCheckpoints);


                                            }


                                        }
                                        else
                                            Toast.makeText(getApplicationContext(), "Bluetooth not Connected", Toast.LENGTH_LONG).show();
                                    }
                                    else

                                        Toast.makeText(getApplicationContext(), "Please Set Destination", Toast.LENGTH_LONG).show();
                                }
                            });
                            alertDialog.show();
                        }

                    });

                    Log.d("CM", "Exit");


                    break;


                }
                case 4:
                {

                    lm.removeUpdates(listener);
                    Toast.makeText(getApplicationContext(),"got initial location",Toast.LENGTH_SHORT).show();

                }
                break;

            }
        }
    };

        public void connect(View v) {
            if (BA.isDiscovering())

            {
                BA.cancelDiscovery();
            }

            BluetoothDevice device = BA.getRemoteDevice(DeviceAddr);

            if (device == null) {
                Toast.makeText(getApplicationContext(), "Device not available, Check!!", Toast.LENGTH_LONG).show();
            } else {
                start(device);
            }
            //Toast.makeText(this,"displaying",Toast.LENGTH_LONG).show();
        }

       private class connectThread extends Thread {

            private final BluetoothSocket mmSocket;
            private final BluetoothDevice mmDevice;

            public connectThread(BluetoothDevice device) {
                BluetoothSocket t = null;
                mmDevice = device;

                try {

                    t = device.createRfcommSocketToServiceRecord(UUID.fromString(UUID_BT));
                    Log.d("BT", "get socket for " + device.getAddress());

                } catch (IOException e) {
                    Toast.makeText(getApplicationContext(), "Device not in range", Toast.LENGTH_SHORT).show();
                    Log.d("BT", "get socket failed");
                    reciever.obtainMessage(1,(Object)String.valueOf("Failed to obtain Socket")).sendToTarget();
                }

                mmSocket = t;


            }

            public void run() {
                BA.cancelDiscovery();

                try {
                    if (mmSocket != null)
                        mmSocket.connect();
                    else
                        return;
                  //  Toast.makeText(getApplicationContext(), "Connected", Toast.LENGTH_LONG).show();
                    bluetoothCon=1;
                    reciever.obtainMessage(1,(Object)String.valueOf("Bluetooth Connected")).sendToTarget();
                    Log.d("BT", "Connecting");
                } catch (IOException e) {
                    reciever.obtainMessage(1,(Object)String.valueOf("Bluetooth Connection Failed!! Try Again")).sendToTarget();
                    Log.d("BT", "Connecting failed");

                    try {
                        mmSocket.close();
                        return;
                    } catch (IOException e1) {
                        return;
                    }

                }

                mConnected = new connectedThread(mmSocket);
                mConnected.start();

            }

            public void cancel() {
                try {
                    mmSocket.close();
                } catch (IOException e) {

                }
            }
        }

        private class AcceptThread extends Thread {
            private final BluetoothServerSocket mmServerSocket;


            public AcceptThread() {
                BluetoothServerSocket t = null;

                try {
                    Log.d("BT", "get socket");
                    t = BA.listenUsingRfcommWithServiceRecord("RC CAR", UUID.fromString(UUID_BT));
                } catch (IOException e) {

                }

                mmServerSocket = t;
            }

            public void run() {
                BluetoothSocket socket = null;
                while (true) {
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
                } catch (IOException e) {
                }
            }
        }

        private class connectedThread extends Thread {
            private final BluetoothSocket mmSocket;
            private final InputStream mmInStream;
            private final OutputStream mmOutStream;

            public connectedThread(BluetoothSocket Socket) {
                mmSocket = Socket;
                InputStream tIn = null;
                OutputStream tOut = null;

                try {
                    tIn = Socket.getInputStream();
                    tOut = Socket.getOutputStream();
                  //
                } catch (IOException e) {

                }
                mmInStream = tIn;
                mmOutStream = tOut;

            }

            public void run() {
                byte[] buffer = new byte[1024];
                int bytes;
                while (true) {
                    try {
                        bytes = mmInStream.read(buffer);
                        reciever.obtainMessage(2, bytes, -1, buffer).sendToTarget();
                    } catch (IOException e) {
                        break; //change
                    }
                }
            }

            public void write(byte[] bytes) {
                try {
                    Log.d("write",bytes.toString()    );
                    mmOutStream.write(bytes);
                    mmOutStream.flush();
                } catch (IOException e) {

                }
            }

            public void cancel() {
                try {
                    mmSocket.close();
                } catch (IOException e) {

                }
            }
        }
    }









