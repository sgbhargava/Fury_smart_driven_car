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
import android.widget.Toast;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;
import java.util.logging.Handler;
import java.util.logging.LogRecord;

public class Bluetooth extends Activity {

    private Button bn2;
    private Button bn3;
    private BluetoothAdapter BA;
    private ListView lv;
    private ArrayAdapter<String> BTArrayAdapter;
    private connectThread mConnectTh;
    private AcceptThread mAccept;
    private connectedThread mConnected;

    private Set<BluetoothDevice>    pairedDevice;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth);
        // initialize buton 1

        bn2 = (Button)findViewById(R.id.Bn2);
        bn3 = (Button)findViewById(R.id.Bn3);
        lv = (ListView)findViewById(R.id.list);

        BTArrayAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_list_item_1);
        lv.setAdapter(BTArrayAdapter);

        lv.setClickable(true);
        lv.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

                if (BA.isDiscovering())
                {
                    BA.cancelDiscovery();
                }
                String str = BTArrayAdapter.getItem(position);
                String macStr = str.substring(str.lastIndexOf("\n") + 1);
                BluetoothDevice device = BA.getRemoteDevice(macStr);

               start(device);
                //TO Do  start client thread

            }
        });


        BA = BluetoothAdapter.getDefaultAdapter();

        if( BA == null)
        {
            Toast.makeText(this,"No bluetooth adapter found",Toast.LENGTH_LONG).show();
        }

        bn2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                off(v);
            }
        });

        bn3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                display(v);
            }
        });

        on();
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
                Toast.makeText(this,"Bluetooth turned on",Toast.LENGTH_LONG).show();
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

    public void off(View v)
    {
        if( BA.isEnabled())
        {
            BA.disable();
            Toast.makeText(this, "Bluetooth turned off", Toast.LENGTH_LONG).show();
        }
        else {
            Toast.makeText(this, "Bluetooth already turned off", Toast.LENGTH_LONG).show();
        }
    }
    final BroadcastReceiver bDiscovery = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if( BluetoothDevice.ACTION_FOUND.equals(intent.getAction()))
            {
                BluetoothDevice btDevice  = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                BTArrayAdapter.add(btDevice.getName()+"\n"+btDevice.getAddress());
                BTArrayAdapter.notifyDataSetChanged();
            }
        }
    };

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

    public void display(View v)
    {
        if( BA.isDiscovering())
        {
            BA.cancelDiscovery();
        }
        else
        {
            if( BA.startDiscovery())
            {
                BTArrayAdapter.clear();
                BTArrayAdapter.notifyDataSetChanged();
                registerReceiver(bDiscovery, new IntentFilter(BluetoothDevice.ACTION_FOUND));
            }
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

                Log.d("BT", "get socket for"+device.getAddress());
                t = device.createRfcommSocketToServiceRecord(UUID.fromString("fa87c0d0-afac-11de-8a39-0800200c9a66"));

            }
            catch(IOException e){
                Log.d("BT", "get socket failed");
            }
            mmSocket =t;

        }

        public void run()
        {
            BA.cancelDiscovery();

            try{
                mmSocket.connect();
                Log.d("BT", "Connecting");
            }catch(IOException e )
            {
                Log.d("BT", "Connecting failed");
                try{
                    mmSocket.close();
                }catch (IOException e1){

                }
                return;
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
                t = BA.listenUsingRfcommWithServiceRecord("RC CAR", UUID.fromString("1f11cfa9-a542-41e3-9231-e0e1222cae8d"));
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




