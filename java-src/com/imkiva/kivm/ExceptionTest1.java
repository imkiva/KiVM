package com.imkiva.kivm;

public class ExceptionTest1 {
    public static void main(String[] args) throws ClassNotFoundException {
        Class.forName("a.non.existing.Class");
    }
}
