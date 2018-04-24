package com.q2zy.babyandroid;

import android.app.Application;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.util.StringBuilderPrinter;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.lang.reflect.Method;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private static String TAG = "java";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

//        Button button = (Button) findViewById(R.id.button);
//        button.setOnClickListener(this);

//        EditText editText = (EditText) findViewById(R.id.editText);
//        editText.setText("1nTr3stIng-AnDr0id-A&D!");
        Log.w(TAG, "oncreate");
    }

    @Override
    public void onClick(View v) {
//        Log.w(TAG, Thread.currentThread().getContextClassLoader().toString());
//        Log.w(TAG, this.getClassLoader().toString());
//        if ((new MainActivity()) == null) {
//            Log.w(TAG, "fuck");
//        }
//        Log.w(TAG, (new MainActivity()).getClassLoader().toString());
//        Log.w(TAG, this.getClass().getClassLoader().toString());
//        Log.w(TAG, Runtime.getRuntime().getClass().getClassLoader().toString());
//        Log.w(TAG, ClassLoader.getSystemClassLoader().toString());
//        loadLibrary("a");
//        Log.w(TAG, "return to java");
        EditText editText = (EditText) findViewById(R.id.editText);
        TextView tv = (TextView) findViewById(R.id.sample_text);
        String input = null;
        if (editText != null) {
            input = editText.getText().toString();
        }
        if (tv != null) {
            tv.setText(check0(input) ? "flag is your input" : "failed");
        }
    }

    public static native boolean check5(String s);

    public static native boolean check4(String s);

    public static native boolean check3(String s);

    public static native boolean check2(String s);

    public static native boolean check1(String s);

    public static native boolean check0(String s);

    public static native boolean check(String s);

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native String stringFromJNI();

    public void xxx() {
        Log.w(TAG, "enter xxx");
        try {
            Log.w(TAG, Thread.currentThread().getContextClassLoader().toString());
//            if (this.getClassLoader() == null) {
//                Log.w(TAG, "no class loader");
//            } else {
//                Log.w(TAG, this.getClassLoader().getClass().getName());
//                Log.w(TAG, this.getClass().getClassLoader().toString());
//            }
        } catch (Exception e) {
            Log.w(TAG, e.getClass().getName());
            Log.w(TAG, e);
            Log.w(TAG, MainActivity.class.getClassLoader().toString());
            Log.w(TAG, ClassLoader.getSystemClassLoader().toString());
        }
        Log.w(TAG, "exit xxx");
    }

    public static void load(String libName) {
        try {
            Log.w(TAG, "log b in java");
//        System.load(s);
            Log.w(TAG, Thread.currentThread().getContextClassLoader().toString());
            Runtime runtime = Runtime.getRuntime();
            ClassLoader classLoader = Thread.currentThread().getContextClassLoader();
            Log.w(TAG, classLoader.toString());
            Method method = Runtime.class.getDeclaredMethod("load", String.class, ClassLoader.class);
            method.setAccessible(true);
            method.invoke(runtime, libName, classLoader);
        } catch (Exception e) {
            Log.w(TAG, e);
        }


//        new MainActivity().xxx();
    }

    public static void loadLibrary(String s) {
        System.loadLibrary(s);
    }
    // Used to load the native library on application startup.
    static {
        System.loadLibrary("a");
    }
}
