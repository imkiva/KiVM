package com.imkiva.kivm;

public class StringBuilderTest1 {
    public static void main(String[] args) {
        StringBuilder sb = new StringBuilder();
        String s = new String("world");
        sb.append("hello, " + s);
        System.out.println(sb.toString());
    }
}
