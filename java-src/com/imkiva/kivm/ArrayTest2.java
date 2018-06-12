package com.imkiva.kivm;

public class ArrayTest2 {
    public static void main(String[] args) {
        int[] i = new int[100];
        try {
            i[100] = 10;
        } catch (Throwable e) {
            e.printStackTrace();
        }

        try {
            System.out.println(i[1000]);
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }
}
