package com.imkiva.kivm;

public class CovScriptJNITest {
    public static native void csSayHello();

    public static native void csSayHello(int magicNumber);

    public static void oldMethod() {
        System.out.println("I am too old!");
    }

    public int test(int a, int b) {
        return a+b;
    }

    public static void main(String[] args) {
        csSayHello();
        csSayHello(10086);
        oldMethod();
    }
}