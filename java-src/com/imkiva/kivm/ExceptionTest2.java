package com.imkiva.kivm;

public class ExceptionTest2 {
    private int X;

    public static void main(String[] args) throws ClassNotFoundException {
        try {
            test1();
        } catch (NullPointerException npe) {
            npe.printStackTrace();
        }
        try {
            test2();
        } catch (NullPointerException npe) {
            npe.printStackTrace();
        }
    }


    public static void test1() {
        // test GETFIELD
        ExceptionTest2 s = null;
        System.out.println(s.X);
    }

    public static void test2() {
        // test MONITORENTER
        String s = null;
        synchronized (s) {
            System.out.println("should never reach here");
        }
    }
}
