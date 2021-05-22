package com.ru7w1k.threelightsonsphere;

public class Light {
    public float[] lightAmbient;
    public float[] lightDiffuse;
    public float[] lightSpecular;
    public float[] lightPosition;
    public float angle;

    public Light() {
        this.lightAmbient = new float[4];
        this.lightDiffuse = new float[4];
        this.lightSpecular = new float[4];
        this.lightPosition = new float[4];
    }

    public Light(float[] la, float[] ld, float[] ls, float[] lp, float a) {
        this.lightAmbient = la;
        this.lightDiffuse = ld;
        this.lightSpecular = ls;
        this.lightPosition = lp;
        this.angle = a;
    }
}
