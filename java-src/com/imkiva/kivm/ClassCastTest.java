package com.imkiva.kivm;

public class ClassCastTest {
    public static void main(String[] args) {
        Object x = null;
        String s = (String) x;

        Object z = new ClassCastTest();
        String y = (String) z;
    }
}
