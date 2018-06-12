package com.imkiva.kivm;

import java.io.*;

public class FileTest {
    public static void main(String[] args) {
        File f = new File(".");
        for (String x : f.list()) {
            System.out.println(x);
        }
    }
}
