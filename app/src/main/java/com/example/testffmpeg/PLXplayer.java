package com.example.testffmpeg;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;

import java.util.jar.Attributes;

public class PLXplayer extends GLSurfaceView implements Runnable, SurfaceHolder.Callback {
    public PLXplayer(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public void run() {
        //Open("/sdcard/Movies/29265300/英雄时刻_20190105-21点12分41s.avi", getHolder().getSurface());
        Open("/sdcard/Movies/29265300/video1.mp4",
                getHolder().getSurface());
        //Open("/sdcard/Movies/29265300/out.yuv",
        //        getHolder().getSurface());
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder)
    {
        new Thread( this ).start();
    }
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
                               int height)
    {

    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder)
    {

    }

    public native void Open(String url, Object surface);
}
