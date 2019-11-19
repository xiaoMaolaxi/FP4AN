package com.example.testffmpeg;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

    // Example of a call to a native method
    TextView tv = findViewById(R.id.sample_text);
    tv.setText(stringFromJNI());
        //open("/sdcard/Movies/29265300/英雄时刻_20190105-21点12分41s.avi", this);
        open("/sdcard/Movies/29265300/独家整理---百度云泄密流出视图新婚大喜发现豪乳伴娘和新郎原来有一腿 2[7-57] - 在綫成人影院 - 草榴社區 - t66y.com.mp4", this);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native boolean open(String url, Object handle);

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
