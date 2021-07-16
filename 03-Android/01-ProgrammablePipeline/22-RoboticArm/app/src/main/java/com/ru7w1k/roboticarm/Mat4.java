package com.ru7w1k.roboticarm;

public class Mat4 {

    private float[] data;

    public Mat4() {
        data = new float[16];
    }

    public Mat4(float[] _data) {
        data = new float[16];
        for(int i = 0; i < 16; i++)
            data[i] = _data[i];
    }

    public float[] get() {
        return data;
    }

    public void set(float[] _data) {
        for(int i = 0; i < 16; i++)
            data[i] = _data[i];
    }

}
