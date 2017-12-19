

package com.android.pate;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;



public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        System.loadLibrary("fmod");
        System.loadLibrary("native-activity");


        super.onCreate(savedInstanceState);

        Intent intent = new Intent(MainActivity.this, android.app.NativeActivity.class);
        MainActivity.this.startActivity(intent);
    }
}
