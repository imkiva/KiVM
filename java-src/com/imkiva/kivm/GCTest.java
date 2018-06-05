package com.imkiva.kivm;

public class GCTest {
    public static void main(String[] args) {
        for (int i = 0; i < 2; ++i) {
            byte[] b = new byte[1024 * 1024];
        }
    }
}
