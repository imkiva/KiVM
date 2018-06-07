package com.imkiva.kivm;

public class ExceptionTest {
    public ExceptionTest() {
        fun1();
    }

    public void fun1() {
        fun2();
    }

    public void fun2() {
        fun3();
    }

    public void fun3() {
        fun4();
    }

    public void fun4() {
        fun5();
    }

    public void fun5() {
        fun6();
    }

    public void fun6() {
        fun7();
    }

    public void fun7() {
        fun8();
    }

    public void fun8() {
        throw new RuntimeException("So, NVDIA, fuck you!");
    }

    public static void main(String[] args) {
        new ExceptionTest();
    }
}
