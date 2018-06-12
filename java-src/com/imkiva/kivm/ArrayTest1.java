package com.imkiva.kivm;

public class ArrayTest1 {
    public static void main(String[] args) {
        try { int[] a = new int[-1]; } catch (Throwable ignore) { ignore.printStackTrace(); }
        try { int[][] a = new int[3][-10]; } catch (Throwable ignore) { ignore.printStackTrace(); }
        try { String[] a = new String[-5]; } catch (Throwable ignore) { ignore.printStackTrace(); }
    }
}
