package com.imkiva.kivm;

public class ExceptionTest3 {
    public static void main(String[] args) throws ClassNotFoundException {
        try {
            test1();
        } catch (NullPointerException npe) {
            npe.printStackTrace();
        }

        try {
            test2();
        } catch (ArrayIndexOutOfBoundsException e) {
            e.printStackTrace();
        }

        try {
            test3();
        } catch (NullPointerException npe) {
            npe.printStackTrace();
        }

        try {
            test4();
        } catch (ArrayIndexOutOfBoundsException e) {
            e.printStackTrace();
        }
    }


    public static void test1() {
        // test IALOAD and so on
        int[] a = null;
        System.out.println(a[0]);
    }

    public static void test2() {
        // test IALOAD and so on
        int[] a = new int[2];
        System.out.println(a[5]);
    }

    public static void test3() {
        // test AALOAD
        String[] s = null;
        System.out.println(s[0]);
    }

    public static void test4() {
        // test AALOAD
        String[] s = new String[3];
        System.out.println(s[8]);
    }
}
