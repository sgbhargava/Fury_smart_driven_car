package com.chinmayi.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Message;
import android.provider.SyncStateContract;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;
import java.util.logging.Handler;
import java.util.logging.LogRecord;

public class Bluetooth extends Activity {

    private Button bn3;
    private Button bn2;
    private BluetoothAdapter BA;
    private connectThread mConnectTh;
    private AcceptThread mAccept;
    private connectedThread mConnected;
    public static String UUID_BT = "00001101-0000-1000-8000-00805f9b34fb";
    public static String DeviceAddr = "00:06:66:72:6D:70";
    public static String data = "$32m23s45";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth);

                //TO Do  start client thread
        BA = BluetoothAdapter.getDefaultAdapter();

        if( BA == null)
        {
            Toast.makeText(this,"No bluetooth adapter found",Toast.LENGTH_LONG).show();
        }
        else {
            bn3 = (Button) findViewById(R.id.Bn3);
            bn2 = (Button) findViewById(R.id.bn2);

            bn2.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    byte[] temp = null;
                    send(temp);
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
    }

    public void send( byte[] d)
    {
        byte[] t = data.getBytes();
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
               case Constant.MESSAGE_READ:
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
                Log.d("BT", "get socket for "+device.getAddress());

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
                    reciever.obtainMessage(Constant.MESSAGE_READ,bytes,-1,buffer).sendToTarget();
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




