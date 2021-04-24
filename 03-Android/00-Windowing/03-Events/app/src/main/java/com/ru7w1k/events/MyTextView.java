package com.ru7w1k.events;

import android.content.Context;
import androidx.appcompat.widget.AppCompatTextView;

import android.graphics.Color;
import android.view.Gravity;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnDoubleTapListener;
import android.view.GestureDetector.OnGestureListener;

public class MyTextView extends AppCompatTextView implements OnDoubleTapListener, OnGestureListener {

    private GestureDetector gestureDetector;

    public MyTextView(Context context) {
        super(context);

        setText("Hello World! ~Ru7w1k");
        setTextSize(32);
        setTextColor(Color.rgb(0, 255, 0));
        setGravity(Gravity.CENTER);
        setBackgroundColor(Color.rgb(0, 0, 0));

        gestureDetector = new GestureDetector(context, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    // ~ WndProc()
    @Override 
    public boolean onTouchEvent(MotionEvent event) {
        // not used now, but require in event driven apps
        int eventaction = event.getAction();

        // if the event is not related to me, pass it to super class
        if (!gestureDetector.onTouchEvent(event)) {
            super.onTouchEvent(event);
        }

        // else that event will be handled using following 9 methods from this class
        return(true);
    }

    // methods from onDoubleTapListener
    @Override
    public boolean onDoubleTap(MotionEvent e) {
        setText("Double Tap");
        return(true);
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent e) {
        return(true);
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent e) {
        setText("Single Tap");
        return(true);
    }

    // methods from onGestureListener
    @Override
    public boolean onDown(MotionEvent e) {
        return(true);
    }

    @Override
    public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
        return(true);
    }

    @Override
    public void onLongPress(MotionEvent e) {
        setText("Long Press");
    }

    @Override
    public void onShowPress(MotionEvent e) {
        // empty
    }

    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
        setText("Scroll");
        return(true);
    }

    @Override
    public boolean onSingleTapUp(MotionEvent e) {
        return(true);
    }
}
