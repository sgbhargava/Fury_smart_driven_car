package com.chinmayi.bluetooth;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import java.io.IOException;
import java.util.Set;

public class Bluetooth extends Activity {


    private Button bn1;
    private Button bn2;
    private Button bn3;
    private BluetoothAdapter BA;
    private ListView lv;
    private ArrayAdapter<String> BTArrayAdapter;

    private Set<BluetoothDevice>    pairedDevice;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_bluetooth);
        // initialize buton 1
        bn1 = (Button)findViewById(R.id.Bn1);
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


            }
        });

        BA = BluetoothAdapter.getDefaultAdapter();

        if( BA == null)
        {
            Toast.makeText(this,"No bluetooth adapter found",Toast.LENGTH_LONG).show();
        }

        bn1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                on(v);
            }
        });

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

    public void on(View v)
    {
        if( !BA.isEnabled())
        {
            Intent enableBt = new Intent(BA.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBt, 1);

        }
        else{
            Toast.makeText(this,"Bluetooth already turned on",Toast.LENGTH_LONG).show();
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






}
